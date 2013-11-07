#ifndef SPIDER_SERVERMANAGER_H_
#define SPIDER_SERVERMANAGER_H_

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
   */
  ServerManager(const std::string &server);

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

#endif // SPIDER_SERVERMANAGER_H_
