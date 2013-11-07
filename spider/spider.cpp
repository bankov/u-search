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

#include <sys/types.h>
#include <sys/stat.h>
#include <libsmbclient.h>
#include <unistd.h>
#include <dirent.h>

#include <algorithm>
#include <string>
#include <list>
#include <vector>
#include <memory>

#include "common.h"
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
    : db_name_(),
      db_server_(),
      db_user_(),
      db_password_() {
  openlog("spider", LOG_CONS | LOG_ODELAY | LOG_PERROR, LOG_USER);

  mime_type_attr_ = NULL;
  result_ = NULL;
  pserver_manager_ = NULL;

  if (UNLIKELY(smbc_init(libsmbmm_guest_auth_smbc_get_data, 0) < 0)) {
    DetectError();
    MSS_FATAL("smbc_init", error_);
    return;
  }

  // Create a directory to store file headers.
  if (UNLIKELY(mkdir(TMPDIR, 00744 /* rwxr--r-- */) && errno != EEXIST)) {
    DetectError();
    MSS_ERROR("mkdir", error_);
    return;
  }

  // Prepare to work with libmagic
  if (UNLIKELY((cookie_ = magic_open(MAGIC_MIME_TYPE | MAGIC_ERROR)) == NULL)) {
    error_ = magic_errno(cookie_);
    MSS_ERROR("magic_open", error_);
    return;
  }
  if (UNLIKELY(magic_load(cookie_, NULL) == -1)) {
    error_ = magic_errno(cookie_);
    MSS_ERROR("magic_open", error_);
    return;
  }

  // Allocate memory to the result vector.
  result_ = new(std::nothrow) std::vector<std::string>(VECTOR_SIZE);
  if (UNLIKELY(result_ == NULL)) {
    error_ = ENOMEM;
    MSS_FATAL("result_", error_);
    return;
  }
  last_ = result_->begin();

  error_ = 0;
}

Spider::Spider(const std::string &server, const std::string &service) : Spider() {
  if (UNLIKELY(error_))
    return;

  pserver_manager_ = new(std::nothrow) ServerManager(server, service);
}

Spider::Spider(const std::string &server, const std::string &service,
               const std::string &db_name,
               const std::string &db_server, const std::string &db_user,
               const std::string &db_password) : Spider(server, service) {
  if (UNLIKELY(error_))
    return;

  db_name_ = db_name;
  db_server_ = db_server;
  db_user_ = db_user;
  db_password_ = db_password;

  if (UNLIKELY(ConnetToDataBase())) {
    MSS_FATAL_MESSAGE(DatabaseEntity::get_db_error().c_str());
    error_ = ENOMSG;
    delete result_;
    result_ = NULL;
    return;
  }

  // Detect an attribute to store mime types
  mime_type_attr_ = FileAttribute::GetByNameAndType("mime-type",
                                                    FileAttribute::faString);
  if (UNLIKELY(!mime_type_attr_)) {
    // Create attribute if it doesn't exists
    mime_type_attr_ = std::shared_ptr<FileAttribute>(
        new(std::nothrow) FileAttribute("mime-type", FileAttribute::faString));
    if (UNLIKELY((!mime_type_attr_))) {
      MSS_DEBUG_MESSAGE(DatabaseEntity::get_db_error().c_str());
      error_ = ENOMSG;
      delete result_;
      result_ = NULL;
    }
  }
}

Spider::~Spider() {
  // Close connection with data base
  if (UNLIKELY(!DatabaseEntity::Disconnect()))
    MSS_DEBUG_MESSAGE(DatabaseEntity::get_db_error().c_str());

  if (UNLIKELY(DeleteDir(TMPDIR))) {
    MSS_DEBUG_ERROR("DeleteDir", error_);
  }

  if (pserver_manager_ != NULL)
    delete pserver_manager_;

  if (cookie_)
    magic_close(cookie_);

  if (result_)
    delete result_;

  closelog();
}

int Spider::DumpToFile(const std::string &name,
                       std::vector<std::string> *files) {
  FILE *fout;
  if (UNLIKELY((fout = fopen(name.c_str(), "w")) == NULL)) {
    DetectError();
    MSS_ERROR("fopen", error_);
    return -1;
  }

  for (std::vector<std::string>::iterator itr = files->begin();
       itr != files->end(); ++itr) {
    if (UNLIKELY(fprintf(fout, "%s\n", itr->c_str()) < 0)) {
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
  while(1) {
    std::string server = pserver_manager_->GetServer();
    // Scan each server for all files.
    if (UNLIKELY(ScanSMBDir("smb://" + server))) {
      MSS_DEBUG_ERROR(("ScanSMBDir smb://" + server).c_str(), error_);
    }
    // Added content to data base.
    if (UNLIKELY(DumpToDataBase())) {
      MSS_DEBUG_ERROR(("DumpToDataBase smb://" + server).c_str(), error_);
    }
  }
}

std::shared_ptr<std::list<std::string> > Spider::GetSMBDirContent(
    const std::string &dir) {
  int directory_handler = 0, dirc = 0, dsize = 0;
  char *dirp = NULL;
  char buf[BUF_SIZE];

  // Open given smb directory.
  if (UNLIKELY((directory_handler = smbc_opendir(dir.c_str())) < 0)) {
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
    if (UNLIKELY((dirc = smbc_getdents(directory_handler,
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
  if (UNLIKELY(smbc_closedir(directory_handler) < 0)) {
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
  if (UNLIKELY((directory_handler = smbc_opendir(dir.c_str())) < 0)) {
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
    if (UNLIKELY((dirc = smbc_getdents(directory_handler,
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
  if (UNLIKELY(smbc_closedir(directory_handler) < 0)) {
    DetectError();
    MSS_ERROR(("smbc_closedir " + dir).c_str(), error_);
  }

  return 0;
}

int Spider::AddFileEntryInDataBase(const std::string &file,
                                   const std::string &server) {
  if (UNLIKELY(file.empty() || server.empty())) {
    MSS_ERROR_MESSAGE("Given string is empthy.");
    error_ = EINVAL;
    return -1;
  }

  size_t pos = file.rfind("/");
  if (UNLIKELY(pos == std::string::npos)) {
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
  if (UNLIKELY(NameParser(&name))) {
    MSS_DEBUG_MESSAGE("NameParser: -1 returned");
    return -1;
  }

  // Try to find this entry in database
  auto db_file = FileEntry::GetByPathOnServer(path, server);

  // Add new entry or updaste existing
  FileEntry entry(name, path, server);
  // TODO(yulyugin): Add error check

  if (db_file == nullptr) {
    // No such entry in database so we need to add a mime type information
    FileParameter(entry, *mime_type_attr_, DetectMimeType(file), 0, true);
    // TODO(yulyugin): Add error check
  }

  return 0;
}

int Spider::NameParser(std::string *name) {
  if (UNLIKELY(name->empty())) {
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
  if (UNLIKELY(last_ == result_->begin())) {
    MSS_DEBUG_MESSAGE("No result's to dump.");
    return 0;
  }

  // Extract the name of server.
  // "smb://some.server/path/to/file" -> "some.server"
  std::string server(result_->front(), 6, result_->front().find("/", 6) - 6);

  DatabaseEntity::StartTransaction();
  /*if (UNLIKELY(!DatabaseEntity::get_db_error().empty())) {
    MSS_ERROR_MESSAGE(DatabaseEntity::get_db_error().c_str());
    error_ = ENOMSG;
    return -1;
  }*/

  for (std::vector<std::string>::iterator itr = result_->begin();
       itr != last_; ++itr) {
    if (UNLIKELY(AddFileEntryInDataBase(*itr, server))) {
      if (error_ == ENOMSG) {  // Data base error.
        MSS_DEBUG_MESSAGE(DatabaseEntity::get_db_error().c_str());
      } else {
        MSS_DEBUG_ERROR("AddFileEntryInDataBase", error_);
      }
    }
  }

  DatabaseEntity::CommitTransaction();
  /*if (UNLIKELY(!DatabaseEntity::get_db_error().empty())) {
    MSS_ERROR_MESSAGE(DatabaseEntity::get_db_error().c_str());
    error_ = ENOMSG;
    return -1;
  }*/

  return 0;
}

void Spider::AddSMBFile(const std::string &name) {
  *last_ = name;
  ++last_;

  if (UNLIKELY(last_ == result_->end())) {
    DumpToDataBase();
    last_ = result_->begin();
  }
}

const char *Spider::DetectMimeType(const std::string &path) {
  int smb_fd = smbc_open(path.c_str(), O_RDONLY, 0);
  if (UNLIKELY(smb_fd < 0)) {
    if (LIKELY(errno == EISDIR))
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
  if (UNLIKELY(fd == -1)) {
    if (LIKELY(errno = ENOTDIR)) {
      // TODO(yulyugin): Check if TMPDIR doesn't exists create it.
    }
    DetectError();
    MSS_ERROR("open", error_);
    if (UNLIKELY(smbc_close(smb_fd))) {
      DetectError();
      MSS_ERROR("smbc_close", error_);
    }
    return "unknown";
  }

  void *buf = malloc(HEADERSIZE);  // Buffer to store header.

  // Copy file header to TMPDIR
  if (UNLIKELY(smbc_read(smb_fd, buf, HEADERSIZE) < 0)) {
    DetectError();
    MSS_ERROR("smbc_read", error_);
    if (UNLIKELY(smbc_close(smb_fd))) {
      DetectError();
      MSS_ERROR("smbc_close", error_);
    }
    if (UNLIKELY(close(fd))) {
      DetectError();
      MSS_ERROR("close", error_);
    }
    free(buf);
    return "unknown";
  }

  if (UNLIKELY(write(fd, buf, HEADERSIZE) < 0)) {
    DetectError();
    MSS_ERROR("write", error_);
    if (UNLIKELY(smbc_close(smb_fd))) {
      DetectError();
      MSS_ERROR("smbc_close", error_);
    }
    if (UNLIKELY(close(fd))) {
      DetectError();
      MSS_ERROR("close", error_);
    }
    free(buf);
    return "unknown";
  }

  // Move to the begining of the file
  if (UNLIKELY(lseek(fd, 0, SEEK_SET) != 0)) {
    DetectError();
    MSS_ERROR("lseek", error_);
    if (UNLIKELY(smbc_close(smb_fd))) {
      DetectError();
      MSS_ERROR("smbc_close", error_);
    }
    if (UNLIKELY(close(fd))) {
      DetectError();
      MSS_ERROR("close", error_);
    }
    free(buf);
    return "unknown";
  }

  const char *mime_type = magic_descriptor(cookie_, fd);
  if (UNLIKELY(mime_type == NULL)) {
    error_ = magic_errno(cookie_);
    MSS_ERROR("magic_descriptor", error_);
    if (UNLIKELY(smbc_close(smb_fd))) {
      DetectError();
      MSS_ERROR("smbc_close", error_);
    }
    if (UNLIKELY(close(fd))) {
      DetectError();
      MSS_ERROR("close", error_);
    }
    free(buf);
    return "unknown";
  }

  if (UNLIKELY(smbc_close(smb_fd))) {
    DetectError();
    MSS_ERROR("smbc_close", error_);
  }
  free(buf);

  // Remove temporary file.
  if (UNLIKELY(unlink((TMPDIR + name).c_str()))) {
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
  if (UNLIKELY(!mime_type_attr_)) {
    // Create attribute if it doesn't exists
    mime_type_attr_ = std::shared_ptr<FileAttribute>(
        new(std::nothrow) FileAttribute("mime-type", FileAttribute::faString));
    if (UNLIKELY((!mime_type_attr_))) {
      MSS_DEBUG_MESSAGE(DatabaseEntity::get_db_error().c_str());
      error_ = ENOMSG;
      return -1;
    }
  }

  return 0;
}

int Spider::DeleteDir(const std::string &dir) {
  // Try to delete directory
  if (rmdir(dir.c_str())) {
    if (UNLIKELY(errno != ENOTEMPTY)) {
      DetectError();
      MSS_ERROR("rmdir", error_);
      return -1;
    }
  } else {
    return 0;
  }

  // Delete all content of the directory if it's noe empty and then delete it
  DIR *dirfd = opendir(dir.c_str());
  if (UNLIKELY(!dirfd)) {
    DetectError();
    MSS_ERROR("opendir", error_);
    return -1;
  }

  struct dirent entry;
  struct dirent *result = NULL;

  while (true) {
    if (UNLIKELY(readdir_r(dirfd, &entry, &result))) {
      DetectError();
      MSS_ERROR("readdir_r", error_);
      break;
    }

    if (result == NULL) {
      break;  // No more content in this directory
    }

    if (!strcmp(entry.d_name, ".") || !strcmp(entry.d_name, "..")) {
      continue;  // Don't try to delete "." and ".."
    }

    if (unlink((dir + "/" + entry.d_name).c_str())) {
      if (LIKELY(errno == EISDIR)) {
        if (UNLIKELY(DeleteDir(dir + "/" + entry.d_name))) {
          MSS_DEBUG_ERROR("DeleteDir", error_);
          break;
        }
      } else {
        DetectError();
        MSS_ERROR("unlink", error_);
        break;
      }
    }
  }

  if (UNLIKELY(closedir(dirfd))) {
    DetectError();
    MSS_ERROR("closedir", error_);
    return -1;
  }

  // Don't try to remove directory again
  // if an error occurs during content removing
  if (LIKELY(error_ == 0)) {
    if (UNLIKELY(rmdir(dir.c_str()))) {
      DetectError();
      MSS_ERROR("rmdir", error_);
      return -1;
    }
  } else {
    return -1;
  }

  return 0;
}
