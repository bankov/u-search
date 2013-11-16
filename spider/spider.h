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

#ifndef SPIDER_SPIDER_H_
#define SPIDER_SPIDER_H_

#include <magic.h>

#include <string>
#include <list>
#include <vector>
#include <memory>

#include "common-inl.h"
#include "data-storage/entities.h"

// Size of buffer which used to get smb directory entries.
#define BUF_SIZE 512

// Maximum size of vector with scan results.
#define VECTOR_SIZE 2048

// Name of directory to store headrs of files.
#define TMPDIR "/tmp/u-search"

// The size of file header to copy in TMPDIR to detect mime type of file.
#define HEADERSIZE 1024

/**
 * @brief Class to index files located in local network.
 */
class Spider {
 public:
  /**
   * Simple constructor which init all variables.
   */
  Spider();
  /**
   * Constructor which create an object, init all variables, connect to
   * data base, create a mimt type attribute if it doesn't exists.
   *
   * @param servers_file File with list of servers.
   * @param db_name Name of the data base.
   * @param db_server Name of the data base server.
   * @param db_user Name of the data base user.
   * @param db_password Password to data base.
   */
  Spider(const std::string &servers_file, const std::string &db_name,
         const std::string &db_server, const std::string &db_user,
         const std::string &db_password);

  /**
   * Destructor.
   */
  ~Spider();

  /**
   * @brief Index all servers from servers list.
   */
  void Run();

  /**
   * Get last occured error.
   *
   * @return Last occured error.
   */
  inline int get_error() const { return error_; }

  /**
   * Get list of servers to be indexed.
   *
   * @return List of servers to be indexed.
   */
  inline std::list<std::string> get_servers_list() const {
    return *servers_list_;
  }

  /**
   * Get set of indexed files which still don't dumped in data base.
   *
   * @return Get vector of indexed files.
   */
  inline std::vector<std::string> get_result() const { return *result_; }

  /**
   * Get an iterator to the last indexed file.
   *
   * @return iterator to the last indexed file.
   */
  inline std::vector<std::string>::iterator get_last() const { return last_; }

  /**
   * Get name of the files with list of servers.
   *
   * @return Name of config file with servers list.
   */
  inline std::string get_servers_file() const { return servers_file_; }

  /**
   * Get a mime type attribute.
   *
   * @return Mime type attribute.
   */
  inline FileAttribute get_mime_type_attr() const { return *mime_type_attr_; }

 protected:
  /**
   * Set a error.
   *
   * @param error Error code.
   */
  inline void set_error(const int error) { error_ = error; }

  /**
   * Set file with list of servers.
   *
   * @param servers_file Path to config file with servers list.
   */
  inline void set_servers_file(const std::string &servers_file) {
    servers_file_ = servers_file;
  }

  /**
   * Set the name of the data base.
   *
   * @param db_name Name of the data base.
   */
  inline void set_db_name(const std::string &db_name) { db_name_ = db_name; }

  /**
   * Set the name of data base server.
   *
   * @param db_server Name of the server with data base.
   */
  inline void set_db_server(const std::string &db_server) {
    db_server_ = db_server;
  }

  /**
   * Set the name of data base user.
   *
   * @param db_user Name of the data base user.
   */
  inline void set_db_user(const std::string &db_user) {
    db_user_ = db_user;
  }

  /**
   * Set the data base password.
   *
   * @param db_password Password to data base.
   */
  inline void set_db_password(const std::string &db_password) {
    db_password_ = db_password;
  }

  /**
   * @brief Dump the result vector in file.
   * Each entry should started at new line.
   *
   * @param name Name of the file.
   * @param files Vector with scan results to be dumped to data base.
   *
   * @return 0 on success, -1 otherwise.
   */
  int DumpToFile(const std::string &name, std::vector<std::string> *files);

  /**
   * @brief Dump the result vector to data base.
   *
   * @return 0 on success, -1 otherwise.
   */
  int DumpToDataBase();

  /**
   * @brief Add server to the servers_list_ and write it in
   * servers_file_.
   *
   * @param name Name of the server.
   *
   * @return 0 on success, -1 otherwise.
   */
  int AddServer(const std::string &name);

  /**
   * @brief Delete server from servers_list_ and delete it from
   * servers_file_.
   *
   * @param name Name of the server.
   *
   * @return 0 on success, -1 otherwise.
   */
  int DelServer(const std::string &name);

  /**
   * @brief Save last occured error in error_.
   */
  inline void DetectError() { error_ = errno; }

  /**
   * @brief Connect to data base server.
   *
   * @return 0 if success -1 otherwise.
   */
  inline int ConnectToDataBase() {
    if (!DatabaseEntity::ConnectToServer(db_name_, db_server_, db_user_,
                                         db_password_, false))
      return -1;
    return 0;
  }

  /**
    * @brief Read servers list from servers file.
    *
    * @return 0 on success, -1 otherwise.
    */
  int ReadServersList();

  /**
   * @brief Get content of the smb directory.
   *
   * @param dir URL to the directory. For example "smb://server/share".
   *
   * @return list of files in directory.
   */
  std::shared_ptr<std::list<std::string> > GetSMBDirContent(
      const std::string &dir);

  /**
   * @brief AddFileEntryInDataBase Added new file entry in data base.
   *
   * @param file Full path to file in network which should be added in data
   * base.
   * Example: Full path to a file = "smb://some.server/path/to/file"
   * server = some.server
   * path = path/to/file
   * file = file
   *
   * @param server Name of the server when file is stored.
   *
   * @return 0 on siccess, -1 otherwise.
   */
  int AddFileEntryInDataBase(const std::string &file,
                             const std::string &server);

  /**
   * @brief Search files in smb directory and all subdirectories.
   *
   * @param dir name of the smb directory.
   *
   * @return 0 if functions completed, -1 otherwise.
   */
  int ScanSMBDir(const std::string &dir);

  /**
   * @brief Parsing the given name.
   *
   * Now just replace '_' symbols with spaces.
   *
   * @param name Name to be parsed.
   *
   * @return 0 on success, -1 otherwise.
   */
  int NameParser(std::string *name);

  /**
   * @brief Add a file to result vector and if it full - dump it to data base.
   *
   * @param name Name to be added.
   *
   * @return 0 on success, -1 otherwise.
   */
  void AddSMBFile(const std::string &name);

  /**
   * @brief Detect mime type of given file.
   *
   * @param name Name of the file to be observed.
   *
   * @return Mime type of given file on success, "unknown" otherwise.
   */
  const char *DetectMimeType(const std::string &name);

  /**
   * @brief Initilize file attribute to store mime type in data base.
   *
   * @return 0 on success, -1 otherwise.
   */
  int InitMimeTypeAttr();

 private:
  /**
   * @brief servers_file_ name of the file with list of servers.
   */
  std::string servers_file_;

  /**
   * @brief servers_list_ list of the servers.
   */
  std::list<std::string> *servers_list_ = NULL;

  /**
   * @brief result_ Vector with scan results.
   */
  std::vector<std::string> *result_ = NULL;

  /**
   * @brief last Iterator on last valid element in result vector.
   */
  std::vector<std::string>::iterator last_;

  /**
   * @brief db_name_ Name of the database on the server where data is stored.
   */
  std::string db_name_;

  /**
   * @brief db_server_ Domain name or ip address of server where data is stored.
   */
  std::string db_server_;

  /**
   * @brief db_user_ Username with access to connect to the specified data base
   * on specified server.
   */
  std::string db_user_;

  /**
   * @brief db_password_ Password for specifed user.
   */
  std::string db_password_;

  /**
   * @brief cookie_ Cookie for magic library to detect mime-types.
   */
  magic_t cookie_;

  /**
   * @brief mime_type_attr_ Id of attribute to store mime type in data base.
   */
  std::shared_ptr<FileAttribute> mime_type_attr_;

  /**
   * @brief error_ Last occured error.
   */
  int error_;

  DISALLOW_COPY_AND_ASSIGN(Spider);
};

#endif  // SPIDER_SPIDER_H_
