#ifndef SERVERMANAGER_H
#define SERVERMANAGER_H

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <dirent.h>
#include <unistd.h>

#include <algorithm>
#include <iterator>
#include <string>
#include <list>
#include <vector>

#include "common.h"
#include "data-storage/entities.h"

class ServerManager {
 public:
  /**
   * Simple constructor which inits all variables.
   */
  ServerManager();

  /**
   * Constructor which inits all variables and read file with servers to
   * scan.
   *
   * @param servers_file File with list of servers.
   */
  ServerManager(const std::string &servers_file);

  /**
   * Destructor.
   */
  ~ServerManager();

  /**
   * @brief Get server to be indexed.
   */
  std::string GetServer();

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
   * Get name of the files with list of servers.
   *
   * @return Name of config file with servers list.
   */
  inline std::string get_servers_file() const { return servers_file_; }

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
    * @brief Read servers list from servers file.
    *
    * @return 0 on success, -1 otherwise.
    */
  int ReadServersList();

 private:
  /**
   * @brief servers_file_ name of the file with list of servers.
   */
  std::string servers_file_;

  /**
   * @brief servers_list_ list of the servers.
   */
  std::list<std::string> *servers_list_ = NULL;

  int error_;
};

#endif // SERVERMANAGER_H
