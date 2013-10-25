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

#ifndef SCHEDULER_SERVERQUEUE_H_
#define SCHEDULER_SERVERQUEUE_H_

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

#include "common-inl.h"

class ServerQueue {
  /**
   * Represents server in the queue.
   */
  class Server {
   public:
    Server() : name_() { timestamp_ = 0; }
    Server(const std::string name, const time_t timestamp = 0) : name_(name),
           timestamp_(timestamp) {};

    /**
     * Getter for server name.
     */
    std::string get_name() const { return name_; }

    /**
     * Getter for timestamp.
     */
    time_t get_timestamp() const { return timestamp_; }

    /**
     * Setter for timestamp.
     */
    void set_timestamp(const time_t timestamp) { timestamp_ = timestamp; }

    /**
     * Update timestamp, called when server scan begins.
     */
    void Refresh() { set_timestamp(time(NULL)); }

    /**
     * Set the timestamp of the server to 0 (year 1970).
     */
    void Reset() { set_timestamp(0); }

    /**
     * Tell if two servers are equal.
     */
    bool operator==(Server val) { return get_name() == val.get_name(); }

   private:
    /**
     * @brief Server hostname or address.
     */
    std::string name_;
    /**
     * @brief Last time server was scanned.
     */
    time_t timestamp_;
  };

 public:
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
  explicit ServerQueue(const std::string &servers_file);

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
  std::string CmdGet();

  /**
   * @brief Get command handling, keepalive
   */
  void CmdGet(const std::string address);

  /**
   * @brief Release command handling
   */
  void CmdRelease(const std::string address);

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
  const time_t kMaxWait = 60;
};

#endif  // SCHEDULER_SERVERQUEUE_H_
