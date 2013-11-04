#ifndef SERVERMANAGER_H
#define SERVERMANAGER_H

#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>

#include <algorithm>
#include <iterator>
#include <string>
#include <list>
#include <vector>
#include <thread>
#include <mutex>

#include "common.h"
#include "data-storage/entities.h"

class ServerManager {
 public:
  /**
   * Constructor which inits all variables and connects to scheduler server.
   *
   * @param server Server address.
   * @param service Service name or port number.
   */
  ServerManager(const std::string &server, const std::string &service);

  /**
   * Destructor.
   */
  ~ServerManager();

  /**
   * @brief Get server to be indexed.
   */
  std::string GetServer();

  /**
   * @brief Release server when indexing is finished.
   */
  void ReleaseServer();

 private:
   std::mutex keepalivemutex_;
   int keepalive_;
   std::thread keepalivethread_;
   std::string smbserver_;
   int sockfd_;
};

#endif // SERVERMANAGER_H
