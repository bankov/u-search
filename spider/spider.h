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
#include "spider/servermanager.h"
#include "data-storage/entities.h"

/**
 * Class to index files located in local network.
 */
class Spider {
 public:
  /**
   * Simple constructor which init all variables.
   */
  Spider();
  /**
   * Constructor which inits all variables and connects to scheduler server.
   *
   * @param server Scheduler server address.
   */
  explicit Spider(const std::string &server);
  /**
   * Constructor which create an object, init all variables, connect to
   * data base, create a mimt type attribute if it doesn't exists.
   *
   * @param server Scheduler server address.
   * @param db_name Name of the data base.
   * @param db_server Name of the data base server.
   * @param db_user Name of the data base user.
   * @param db_password Password to data base.
   */
  Spider(const std::string &config,
         const std::string &db_name,
         const std::string &db_server,
         const std::string &db_user,
         const std::string &db_password);

  /**
   * Read configuration file.
   *
   * @param config Configuration file name.
   */
  int ReadConfig(const std::string &config);

#ifndef DOXYGEN_SHOULD_SKIP_THIS
  /**
   * Destructor.
   */
  ~Spider();
#endif  // DOXYGEN_SHOULD_SKIP_THIS

  /**
   * Index all servers from servers list.
   */
  void Run();

#ifndef DOXYGEN_SHOULD_SKIP_THIS
  /**
   * Get last occured error.
   *
   * @return Last occured error.
   */
  inline int get_error() const { return error_; }

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
   * Get a mime type attribute.
   *
   * @return Mime type attribute.
   */
  inline FileAttribute get_mime_type_attr() const { return *mime_type_attr_; }
#endif  // DOXYGEN_SHOULD_SKIP_THIS
 protected:
#ifndef DOXYGEN_SHOULD_SKIP_THIS
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
#endif  // DOXYGEN_SHOULD_SKIP_THIS

  /**
   * Dump the result vector to data base.
   *
   * @return 0 on success, -1 otherwise.
   */
  int DumpToDataBase();

  /**
   * Connect to data base server.
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
   * Add new file entry in data base.
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
   * Search files in smb directory and all subdirectories.
   *
   * @param dir name of the smb directory.
   *
   * @return 0 if functions completed, -1 otherwise.
   */
  int ScanSMBDir(const std::string &dir);

  /**
   * Parsing the given name.
   *
   * Now just replace '_' symbols with spaces.
   *
   * @param name Name to be parsed.
   *
   * @return 0 on success, -1 otherwise.
   */
  int NameParser(std::string *name);

  /**
   * Add a file to result vector and if it full - dump it to data base.
   *
   * @param name Name to be added.
   *
   * @return 0 on success, -1 otherwise.
   */
  void AddSMBFile(const std::string &name);

  /**
   * Detect mime type of given file.
   *
   * @param name Name of the file to be observed.
   *
   * @return Mime type of given file on success, "unknown" otherwise.
   */
  const char *DetectMimeType(const std::string &name);

  /**
   * Initilize file attribute to store mime type in data base.
   *
   * @return 0 on success, -1 otherwise.
   */
  int InitMimeTypeAttr();

 private:
  /**
   * Save last occured error in error_.
   */
  inline void DetectError() { error_ = errno; }

  /**
   * Vector with scan results.
   */
  std::vector<std::string> *result_ = NULL;

  /**
   * last Iterator on last valid element in result vector.
   */
  std::vector<std::string>::iterator last_;

  /**
   * Name of the database on the server where data is stored.
   */
  std::string db_name_;

  /**
   * Domain name or ip address of server where data is stored.
   */
  std::string db_server_;

  /**
   * Username with access to connect to the specified data base
   * on specified server.
   */
  std::string db_user_;

  /**
   * Password for specifed user.
   */
  std::string db_password_;

  /**
   * Cookie for magic library to detect mime-types.
   */
  magic_t cookie_;

  /**
   * Id of attribute to store mime type in data base.
   */
  std::shared_ptr<FileAttribute> mime_type_attr_;

  /*
   * Scheduler hostname.
   */
  std::string scheduler_;

  /**
   * Server manager which is used to obtain server names to index
   */
  ServerManager *pserver_manager_;

  /**
   * Last occured error.
   */
  int error_;

  DISALLOW_COPY_AND_ASSIGN(Spider);
};

#endif  // SPIDER_SPIDER_H_
