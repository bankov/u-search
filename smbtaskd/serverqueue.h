#ifndef SMBTASKD_SERVERQUEUE_H_
#define SMBTASKD_SERVERQUEUE_H_

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <dirent.h>
#include <unistd.h>

#include <algorithm>
#include <iterator>
#include <string>
#include <list>

#include "common.h"
#include "data-storage/entities.h"

class ServerQueue {
 public:


class Server {
 public:
  Server() : name_() { timestamp_ = 0; }
  Server(const std::string name) : name_(name) { timestamp_ = 0; }
  Server(const std::string name, const time_t timestamp) : name_(name) { timestamp_ = timestamp; }

  time_t get_timestamp() const { return timestamp_; }
  std::string get_name() const { return name_; }

  void set_timestamp(const time_t timestamp) { timestamp_ = timestamp; }
  void refresh() { timestamp_ = time(NULL); }
  void set_name(const std::string name) { name_ = name; }
  bool operator==(Server val) { return name_ == val.name_; }

 private:
  std::string name_;
  time_t timestamp_;
};

  /**
   * Simple constructor which inits all variables.
   */
  ServerQueue();

  /**
   * Constructor which inits all variables and read file with servers to
   * scan.
   *
   * @param servers_file File with list of servers.
   */
  ServerQueue(const std::string &servers_file);

  /**
   * Destructor.
   */
  ~ServerQueue();

  /**
   * Get list of servers to be indexed.
   *
   * @return List of servers to be indexed.
   */
  std::list<ServerQueue::Server> get_servers_list() const {
    return *servers_list_;
  }

  /**
   * Get name of the file with list of servers.
   *
   * @return Name of config file with servers list.
   */
  std::string get_servers_file() const { return servers_file_; }

  /**
   * Add a server to the list
   */
  void AddServer(std::string address);

  /**
   * @brief Get command handling, task query
   */
  std::string cmdGet();

  /**
   * @brief Get command handling, keepalive
   */
  void cmdGet(const std::string address);

  /**
   * @brief Release command handling
   */
  void cmdRelease(const std::string address);

  /**
   * Set file with list of servers.
   *
   * @param servers_file Path to config file with servers list.
   */
  void set_servers_file(const std::string &servers_file) {
    servers_file_ = servers_file;
  }

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
  std::list<ServerQueue::Server> *servers_list_ = NULL;

  /**
   * @brief iterator pointing to the next server
   */
  std::list<ServerQueue::Server>::iterator ilast_server_;

  /**
   * @brief Maximum time between keepalive messages
   */
  const time_t MAX_WAIT = 60;
};

#endif // SMBTASKD_SERVERQUEUE_H_
