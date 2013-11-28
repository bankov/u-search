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

#ifndef SPIDER_SERVERMANAGER_H_
#define SPIDER_SERVERMANAGER_H_

#include <string>
#include <thread>
#include <mutex>

#include "common-inl.h"

class ServerManager {
 public:
  /**
   * Constructor which inits all variables and connects to scheduler server.
   *
   * @param server Server address.
   */
  explicit ServerManager(const std::string &server);

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
  DISALLOW_COPY_AND_ASSIGN(ServerManager);
  std::mutex keepalivemutex_;
  int keepalive_;
  std::thread keepalivethread_;
  std::string smbserver_;
  int sockfd_;
};

#endif  // SPIDER_SERVERMANAGER_H_
