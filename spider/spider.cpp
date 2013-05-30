/*
 * Copyright (c) 2013 Morgen Matvey, Yulugin Evgeny and others.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *   * The names of its contributors may be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "spider.h"

inline void libsmbmm_guest_auth_smbc_get_data(const char *server,
                                              const char *share,
                                              char *workgroup, int wgmaxlen,
                                              char *username, int unmaxlen,
                                              char *password, int pwmaxlen) {
  strncpy(username, "Guest", unmaxlen - 1);
  strncpy(password, "", pwmaxlen - 1);
  strncpy(workgroup, "", wgmaxlen - 1);
  // Hack to prevent qt warnings
  server = server;
  share = share;
}

Spider::Spider()
    : servers_file_(),
      db_name_(),
      db_server_(),
      db_user_(),
      db_password_() {
  openlog("spider", LOG_CONS | LOG_ODELAY, LOG_USER);

  mime_type_attr_ = NULL;

  if (unlikely(smbc_init(libsmbmm_guest_auth_smbc_get_data, 0) < 0)) {
    DetectError();
    MSS_FATAL("smbc_init", error_);
    servers_list_ = NULL;
    result_ = NULL;
    return;
  }

  // Create a directory to store file headers.
  if (unlikely(mkdir(TMPDIR, 00744 /* rwxr--r-- */) && errno != EEXIST)) {
    DetectError();
    MSS_ERROR("mkdir", error_);
    servers_list_ = NULL;
    result_ = NULL;
    return;
  }

  // Prepare to work with libmagic
  if (unlikely((cookie_ = magic_open(MAGIC_MIME_TYPE | MAGIC_ERROR)) == NULL)) {
    error_ = magic_errno(cookie_);
    MSS_ERROR("magic_open", error_);
    servers_list_ = NULL;
    result_ = NULL;
    return;
  }
  if (unlikely(magic_load(cookie_, NULL) == -1)) {
    error_ = magic_errno(cookie_);
    MSS_ERROR("magic_open", error_);
    servers_list_ = NULL;
    result_ = NULL;
    return;
  }

  // Allocare memory to the servers list.
  servers_list_ = new(std::nothrow) std::list<std::string>();
  if (unlikely(servers_list_ == NULL)) {
    error_ = ENOMEM;
    MSS_FATAL("", error_);
    servers_list_ = NULL;
    result_ = NULL;
    return;
  }

  // Allocate memory to the result vector.
  result_ = new(std::nothrow) std::vector<std::string>(VECTOR_SIZE);
  if (unlikely(result_ == NULL)) {
    error_ = ENOMEM;
    MSS_FATAL("result_", error_);
    delete servers_list_;
    servers_list_ = NULL;
    result_ = NULL;
    return;
  }
  last_ = result_->begin();

  error_ = 0;
}

Spider::Spider(const std::string &servers_file) : Spider() {
  if (unlikely(error_))
    return;

  servers_file_ = servers_file;

  // Read servers_file_.
  if (unlikely(ReadServersList())) {
    MSS_DEBUG_ERROR("ReadServersList", error_);
    delete servers_list_;
    servers_list_ = NULL;
    result_ = NULL;
    return;
  }
}

Spider::Spider(const std::string &servers_file, const std::string &db_name,
               const std::string &db_server, const std::string &db_user,
               const std::string &db_password) : Spider(servers_file) {
  if (unlikely(error_))
    return;

  db_name_ = db_name;
  db_server_ = db_server;
  db_user_ = db_user;
  db_password_ = db_password;

  if (unlikely(ConnetToDataBase())) {
    MSS_FATAL_MESSAGE(DatabaseEntity::get_db_error().c_str());
    error_ = ENOMSG;
    delete servers_list_;
    servers_list_ = NULL;
    delete result_;
    result_ = NULL;
    return;
  }

  // Detect an attribute to store mime types
  mime_type_attr_ = FileAttribute::GetByNameAndType("mime-type",
                                                    FileAttribute::faString);
  if (unlikely(!mime_type_attr_)) {
    // Create attribute if it doesn't exists
    mime_type_attr_ = std::shared_ptr<FileAttribute>(
        new(std::nothrow) FileAttribute("mime-type", FileAttribute::faString));
    if (unlikely((!mime_type_attr_))) {
      MSS_DEBUG_MESSAGE(DatabaseEntity::get_db_error().c_str());
      error_ = ENOMSG;
      delete servers_list_;
      servers_list_ = NULL;
      delete result_;
      result_ = NULL;
    }
  }
}

Spider::~Spider() {
  // Close connection with data base
  if (unlikely(!DatabaseEntity::Disconnect()))
    MSS_DEBUG_MESSAGE(DatabaseEntity::get_db_error().c_str());

  if (unlikely(rmdir(TMPDIR))) {
    if (likely(errno == ENOTEMPTY)) {
      // TODO(yulyugin): Delete all content from this directory
    }
    DetectError();
    MSS_ERROR("rmdir", error_);
  }

  if (cookie_)
    magic_close(cookie_);

  if (servers_list_)
    delete servers_list_;

  if (result_)
    delete result_;

  closelog();
}

int Spider::ReadServersList() {
  if (!servers_list_->empty()) {
    return 0;  // Servers file was read earlier.
  }

  FILE *fin;
  fin = fopen(servers_file_.c_str(), "r");
  if (unlikely(fin == NULL)) {
    DetectError();
    MSS_ERROR("fopen", error_);
    return -1;
  }

  std::string temp;
  char *buf = NULL;
  size_t size = 0;
  // Read lines from servers_file_ and put names servers names in servers_list_
  while (getline(&buf, &size, fin) != -1) {
    temp.insert(0, buf);
    // Deleting '\n' symbols from the end of the string
    temp.erase(temp.end() - 1);

    if (unlikely(temp.empty())) {
      continue;  // Do nothing if find an empthy string.
    }

    if (unlikely(std::find(servers_list_->begin(), servers_list_->end(),
                           temp) != servers_list_->end())) {
      MSS_WARN_MESSAGE(("Duplicated server: " + temp).c_str());
      temp.clear();
      continue;  // Do nothing if current server in servers_list_ yet.
    }

    // Add current server to the servers_list_.
    servers_list_->push_back(temp);
    temp.clear();
  }

  free(buf);
  fclose(fin);
  return 0;
}

int Spider::DumpToFile(const std::string &name,
                       std::vector<std::string> *files) {
  FILE *fout;
  if (unlikely((fout = fopen(name.c_str(), "w")) == NULL)) {
    DetectError();
    MSS_ERROR("fopen", error_);
    return -1;
  }

  for (std::vector<std::string>::iterator itr = files->begin();
       itr != files->end(); ++itr) {
    if (unlikely(fprintf(fout, "%s\n", itr->c_str()) < 0)) {
      DetectError();
      MSS_ERROR("fprintf", error_);
      fclose(fout);
      return -1;
    }
  }

  fclose(fout);
  return 0;
}

void Spider::Run() {
  for (std::list<std::string>::iterator itr = servers_list_->begin();
       itr != servers_list_->end(); ++itr) {
    // Scan each server for all files.
    if (unlikely(ScanSMBDir("smb://" + *itr))) {
      MSS_DEBUG_ERROR(("ScanSMBDir smb://" + *itr).c_str(), error_);
    }
    // Added content to data base.
    if (unlikely(DumpToDataBase())) {
      MSS_DEBUG_ERROR(("DumpToDataBase smb://" + *itr).c_str(), error_);
    }
  }
}

int Spider::AddServer(const std::string &name) {
  if (std::find(servers_list_->begin(), servers_list_->end(),
                name) != servers_list_->end()) {
    MSS_WARN_MESSAGE(("Server " + name + "is in servers_list_").c_str());
    return 0;
  }

  servers_list_->push_back(name);

  FILE *fout = fopen(servers_file_.c_str(), "a");
  if (unlikely(!fout)) {
    DetectError();
    MSS_ERROR("fopen", error_);
    return -1;
  }

  if (unlikely(fprintf(fout, "%s\n", name.c_str()) < 0)) {
    DetectError();
    MSS_ERROR("fprintf", error_);
    fclose(fout);
    return -1;
  }

  fclose(fout);
  return 0;
}

int Spider::DelServer(const std::string &name) {
  if (std::find(servers_list_->begin(), servers_list_->end(),
                name) == servers_list_->end()) {
    MSS_WARN_MESSAGE(("Server " + name + "not in servers_list_").c_str());
    return 0;
  }

  servers_list_->remove(name);

  FILE *fout = fopen(servers_file_.c_str(), "w");
  if (unlikely(!fout)) {
    DetectError();
    MSS_ERROR("fopen", error_);
    return -1;
  }

  for (std::list<std::string>::iterator itr = servers_list_->begin();
       itr != servers_list_->end(); ++itr) {
    if (unlikely(fprintf(fout, "%s\n", itr->c_str()) < 0)) {
      DetectError();
      MSS_ERROR("fprintf", error_);
      fclose(fout);
      return -1;
    }
  }

  fclose(fout);
  return 0;
}

std::shared_ptr<std::list<std::string> > Spider::GetSMBDirContent(
    const std::string &dir) {
  int directory_handler = 0, dirc = 0, dsize = 0;
  char *dirp = NULL;
  char buf[BUF_SIZE];

  // Open given smb directory.
  if (unlikely((directory_handler = smbc_opendir(dir.c_str())) < 0)) {
    DetectError();
    MSS_ERROR(("smbc_opendir " + dir).c_str(), error_);
    return nullptr;
  }

  std::shared_ptr<std::list<std::string> > content(
      new(std::nothrow) std::list<std::string>());

  // Getting content of the directory.
  // smbc_getdents() returns the readen size.
  // When no more content in the directory smbc_getdents() returns 0.
  // Use smbc_getdents() while returned value not equal 0.
  while (true) {
    dirp = static_cast<char *>(buf);

    // Get dir content which can placed in buf.
    if (unlikely((dirc = smbc_getdents(directory_handler,
                                       (struct smbc_dirent *)dirp,
                                       sizeof(buf)))) < 0) {
      DetectError();
      MSS_ERROR("smbc_getdents", error_);
      return nullptr;
    }

    // Break if no more content in this directory.
    if (dirc == 0)
      break;

    // Put readen content in list
    while (dirc > 0) {
      dsize = ((struct smbc_dirent *)dirp)->dirlen;

      // Ignoring "." and ".."
      if ((strcmp(((struct smbc_dirent *)dirp)->name, ".") != 0) &&
          (strcmp(((struct smbc_dirent *)dirp)->name, "..") != 0)) {
        content->push_back(dir+"/"+((struct smbc_dirent *)dirp)->name);
      }

      dirp += dsize;  // Promote pointer
      dirc -= dsize;  // Decrease size
    }
  }

  // Close given smb directory
  if (unlikely(smbc_closedir(directory_handler) < 0)) {
    DetectError();
    MSS_ERROR(("smbc_closedir " + dir).c_str(), error_);
  }

  return content;
}

int Spider::ScanSMBDir(const std::string &dir) {
  int directory_handler = 0, dirc = 0, dsize = 0;
  char *dirp = NULL;
  char buf[BUF_SIZE];

  // Open given smb directory.
  if (unlikely((directory_handler = smbc_opendir(dir.c_str())) < 0)) {
    DetectError();
    MSS_ERROR(("smbc_opendir " + dir).c_str(), error_);
    return -1;
  }

  // Getting content of the directory.
  // smbc_getdents() returns the readen size.
  // When no more content in the directory smbc_getdents() returns 0.
  // Use smbc_getdents() while returned value not equal 0.
  while (true) {
    dirp = static_cast<char *>(buf);

    // Get dir content which can placed in buf.
    if (unlikely((dirc = smbc_getdents(directory_handler,
                                       (struct smbc_dirent *)dirp,
                                       sizeof(buf)))) < 0) {
      DetectError();
      MSS_ERROR("smbc_getdents", error_);
      return -1;
    }

    // Break the cycle if no more content in this directory.
    if (dirc == 0)
      break;

    // Put readen content in list
    while (dirc > 0) {
      dsize = ((struct smbc_dirent *)dirp)->dirlen;

      // Ignoring "." and ".."
      if ((strcmp(((struct smbc_dirent *)dirp)->name, ".") == 0) ||
          (strcmp(((struct smbc_dirent *)dirp)->name, "..") == 0)) {
        dirp += dsize;  // Promote pointer
        dirc -= dsize;  // Decrease size
        continue;
      }

      switch (((struct smbc_dirent *)dirp)->smbc_type) {
        case SMBC_WORKGROUP: {
          ScanSMBDir(dir + "/" + ((struct smbc_dirent *)dirp)->name);
          break;
        }
        case SMBC_SERVER: {
          ScanSMBDir(dir + "/" + ((struct smbc_dirent *)dirp)->name);
          break;
        }
        case SMBC_FILE_SHARE: {
          ScanSMBDir(dir + "/" + ((struct smbc_dirent *)dirp)->name);
          break;
        }
        case SMBC_PRINTER_SHARE: {
          // Do nothing
          break;
        }
        case SMBC_COMMS_SHARE: {
          // Do nothing
          break;
        }
        case SMBC_IPC_SHARE: {
          // Do nothing
          break;
        }
        case SMBC_DIR: {
          ScanSMBDir(dir + "/" + ((struct smbc_dirent *)dirp)->name);
          break;
        }
        case SMBC_FILE: {
          AddSMBFile(dir+"/"+((struct smbc_dirent *)dirp)->name);
          break;
        }
        case SMBC_LINK: {
          // Do nothing
          break;
        }
        default: {
          MSS_FATAL_MESSAGE("Unknown smb entry type");
          assert(0);  // This can't happen
        }
      }

      dirp += dsize;  // Promote pointer
      dirc -= dsize;  // Decrease size
    }
  }

  // Close given smb directory
  if (unlikely(smbc_closedir(directory_handler) < 0)) {
    DetectError();
    MSS_ERROR(("smbc_closedir " + dir).c_str(), error_);
  }

  return 0;
}

int Spider::AddFileEntryInDataBase(const std::string &file,
                                   const std::string &server) {
  if (unlikely(file.empty() || server.empty())) {
    MSS_ERROR_MESSAGE("Given string is empthy.");
    error_ = EINVAL;
    return -1;
  }

  size_t pos = file.rfind("/");
  if (unlikely(pos == std::string::npos)) {
    MSS_ERROR_MESSAGE(("Given string " + file + "have no '/' symbol.").c_str());
    error_ = EINVAL;
    return -1;
  }
  std::string name = file.substr(pos + 1);  // '+ 1' to delete '/' symbol.

  // '+ 7':
  // '+ 6' to delete "smb://" from full path to file
  // '+ 1' to delete "/" after hostname
  //
  // Example:
  // full path to a file = "smb://some.server/path/to/file"
  // server = some.server
  // path = path/to/file
  // file = file
  std::string path = file.substr(server.length() + 7);

  // Parsing file name to simplify further search.
  if (unlikely(NameParser(&name))) {
    MSS_DEBUG_MESSAGE("NameParser: -1 returned");
    return -1;
  }

  FileEntry entry(name, path, server);  // Put new entity in data base.
  // TODO(yulyugin): Add normal error check
  /*if (unlikely(!DatabaseEntity::get_db_error().empty())) {
    DEBUG_INFO(DatabaseEntity::get_db_error().c_str());
    error_ = ENOMSG;
    return -1;
  }*/

  // Added a mime type information for entry
  FileParameter(entry, *mime_type_attr_, DetectMimeType(file), 0, true);

  return 0;
}

int Spider::NameParser(std::string *name) {
  if (unlikely(name->empty())) {
    MSS_ERROR_MESSAGE("empty string is given.");
    error_ = EINVAL;
    return -1;
  }

  size_t pos;
  while (true) {
    if ((pos = name->find("_")) == std::string::npos)
      return 0;  // No '_' symbols in name.

    name->replace(pos, 1, " ");  // Replase '_' with space.
  }

  return 0;
}

// TODO(yulyugin): Add check on errors and rollback transaction in case
// of fatal error.
int Spider::DumpToDataBase() {
  if (unlikely(last_ == result_->begin())) {
    MSS_DEBUG_MESSAGE("No result's to dump.");
    return 0;
  }

  // Extract the name of server.
  // "smb://some.server/path/to/file" -> "some.server"
  std::string server(result_->front(), 6, result_->front().find("/", 6) - 6);

  DatabaseEntity::StartTransaction();

  for (std::vector<std::string>::iterator itr = result_->begin();
       itr != last_; ++itr) {
    if (unlikely(AddFileEntryInDataBase(*itr, server))) {
      if (error_ == ENOMSG) {  // Data base error.
        MSS_DEBUG_MESSAGE(DatabaseEntity::get_db_error().c_str());
      } else {
        MSS_DEBUG_ERROR("AddFileEntryInDataBase", error_);
      }
    }
  }

  DatabaseEntity::CommitTransaction();

  return 0;
}

void Spider::AddSMBFile(const std::string &name) {
  *last_ = name;
  ++last_;

  if (unlikely(last_ == result_->end())) {
    DumpToDataBase();
    last_ = result_->begin();
  }
}

const char *Spider::DetectMimeType(const std::string &path) {
  int smb_fd = smbc_open(path.c_str(), O_RDONLY, 0);
  if (unlikely(smb_fd < 0)) {
    if (likely(errno == EISDIR))
      return "inode/directory";

    DetectError();
    MSS_ERROR(("smbc_open " + path).c_str(), error_);
    return "unknown";
  }

  // Extract name of the file
  // Don't detele '/' symbol it need to form path.
  std::string name(path, path.rfind("/"));

  // Create a storage for file header in TMPDIR and open it
  int fd = open((TMPDIR + name).c_str(), O_CREAT | O_RDWR | O_EXCL,
                00744 /* rwxr--r-- */);
  if (unlikely(fd == -1)) {
    if (likely(errno = ENOTDIR)) {
      // TODO(yulyugin): Check if TMPDIR doesn't exists create it.
    }
    DetectError();
    MSS_ERROR("open", error_);
    if (unlikely(smbc_close(smb_fd))) {
      DetectError();
      MSS_ERROR("smbc_close", error_);
    }
    return "unknown";
  }

  void *buf = malloc(HEADERSIZE);  // Buffer to store header.

  // Copy file header to TMPDIR
  if (unlikely(smbc_read(smb_fd, buf, HEADERSIZE) < 0)) {
    DetectError();
    MSS_ERROR("smbc_read", error_);
    if (unlikely(smbc_close(smb_fd))) {
      DetectError();
      MSS_ERROR("smbc_close", error_);
    }
    if (unlikely(close(fd))) {
      DetectError();
      MSS_ERROR("close", error_);
    }
    free(buf);
    return "unknown";
  }

  if (unlikely(write(fd, buf, HEADERSIZE) < 0)) {
    DetectError();
    MSS_ERROR("write", error_);
    if (unlikely(smbc_close(smb_fd))) {
      DetectError();
      MSS_ERROR("smbc_close", error_);
    }
    if (unlikely(close(fd))) {
      DetectError();
      MSS_ERROR("close", error_);
    }
    free(buf);
    return "unknown";
  }

  // Move to the begining of the file
  if (unlikely(lseek(fd, 0, SEEK_SET) != 0)) {
    DetectError();
    MSS_ERROR("lseek", error_);
    if (unlikely(smbc_close(smb_fd))) {
      DetectError();
      MSS_ERROR("smbc_close", error_);
    }
    if (unlikely(close(fd))) {
      DetectError();
      MSS_ERROR("close", error_);
    }
    free(buf);
    return "unknown";
  }

  const char *mime_type = magic_descriptor(cookie_, fd);
  if (unlikely(mime_type == NULL)) {
    error_ = magic_errno(cookie_);
    MSS_ERROR("magic_descriptor", error_);
    if (unlikely(smbc_close(smb_fd))) {
      DetectError();
      MSS_ERROR("smbc_close", error_);
    }
    if (unlikely(close(fd))) {
      DetectError();
      MSS_ERROR("close", error_);
    }
    free(buf);
    return "unknown";
  }

  if (unlikely(smbc_close(smb_fd))) {
    DetectError();
    MSS_ERROR("smbc_close", error_);
  }
  free(buf);

  // Remove temporary file.
  if (unlikely(unlink((TMPDIR + name).c_str()))) {
    DetectError();
    MSS_ERROR("unlink", error_);
  }

  return mime_type;
}

int Spider::InitMimeTypeAttr()  {
  if (mime_type_attr_)
    return 0;

  if (ConnetToDataBase()) {
    MSS_DEBUG_MESSAGE(DatabaseEntity::get_db_error().c_str());
    error_ = ENOMSG;
    return -1;
  }

  mime_type_attr_ = FileAttribute::GetByNameAndType("mime-type",
                                                    FileAttribute::faString);
  if (unlikely(!mime_type_attr_)) {
    // Create attribute if it doesn't exists
    mime_type_attr_ = std::shared_ptr<FileAttribute>(
        new(std::nothrow) FileAttribute("mime-type", FileAttribute::faString));
    if (unlikely((!mime_type_attr_))) {
      MSS_DEBUG_MESSAGE(DatabaseEntity::get_db_error().c_str());
      error_ = ENOMSG;
      return -1;
    }
  }

  return 0;
}
