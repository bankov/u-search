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
#include <sys/socket.h>

#include <netdb.h>
#include <errno.h>
#include <unistd.h>

#include <string>

#include "spider/servermanager.h"
#include "config.h"

ServerManager::ServerManager(const std::string &server) : keepalive_(0) {
  /*
   * Create socket and connect it to scheduler server.
   */
  struct addrinfo hints;
  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_DGRAM;

  struct addrinfo *servinfo;
  int error = getaddrinfo(server.c_str(), SCHEDULERPORT, &hints, &servinfo);
  if (error != 0) {
    MSS_FATAL_MESSAGE(gai_strerror(error));
    return;
  }

  struct addrinfo *p;
  for (p = servinfo; p != NULL; p = p->ai_next) {
    sockfd_ = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
    if (sockfd_ == -1) {
      MSS_ERROR("socket", errno);
      continue;
    }

    if (connect(sockfd_, p->ai_addr, p->ai_addrlen) == -1) {
      MSS_ERROR("connect", errno);
      close(sockfd_);
      continue;
    }

    break;
  }

  if (p == NULL) {
    MSS_FATAL_MESSAGE("failed to create socket");
    return;
  }

  freeaddrinfo(servinfo);

  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_DGRAM;

  // Select source port.
  error = getaddrinfo(NULL, SPIDERPORT, &hints, &servinfo);
  if (error != 0) {
    MSS_FATAL_MESSAGE(gai_strerror(error));
    return;
  }

  for (p = servinfo; p != NULL; p = p->ai_next) {
    bind(sockfd_, p->ai_addr, p->ai_addrlen);
  }

  // Try to request new server after 5 seconds if no reply received.
  struct timeval timeout;
  timeout.tv_sec = 5;
  timeout.tv_usec = 0;
  if (setsockopt(sockfd_, SOL_SOCKET, SO_RCVTIMEO, &timeout,
                 sizeof timeout) == -1)
    MSS_FATAL_MESSAGE("setsockopt");
}

ServerManager::~ServerManager() {
  close(sockfd_);
}

std::string ServerManager::GetServer() {
  char buf[257];

  do {
    send(sockfd_, "G", 1, 0);
    memset(buf, 0, sizeof buf);
    // Timeout if there are no free servers.
    if (recv(sockfd_, buf, sizeof buf - 1, 0) == -1) {
      MSS_ERROR("recv", errno);
      sleep(5);
    }
  } while (buf[0] == '\0');

  smbserver_ = buf;

  keepalivemutex_.lock();
  keepalive_ = 1;
  keepalivemutex_.unlock();

  // Send Keep Alive messages until server is released.
  keepalivethread_ = std::thread([this]() {
    while (1) {
      std::this_thread::sleep_for(std::chrono::seconds(1));
      std::string cmd = "G" + smbserver_;
      send(sockfd_, cmd.c_str(), cmd.size(), 0);

      {
        std::lock_guard<std::mutex> lock(keepalivemutex_);
        if (!keepalive_)
          return;
      }
    }
  });
  return smbserver_;
}

void ServerManager::ReleaseServer() {
  keepalivemutex_.lock();
  keepalive_ = 0;
  keepalivemutex_.unlock();
  keepalivethread_.join();

  std::string cmd = "R" + smbserver_;
  send(sockfd_, cmd.c_str(), cmd.size(), 0);
}
