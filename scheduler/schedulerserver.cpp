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

#include <sys/socket.h>
#include <sys/types.h>

#include <netdb.h>
#include <string.h>
#include <unistd.h>

#include <list>
#include <string>

#include "scheduler/schedulerserver.h"
#include "scheduler/serverqueue.h"
#include "common-inl.h"

SchedulerServer::SchedulerServer(const std::string serversfile)
    : queue_(serversfile) {
  // Prepare hists for getaddrinfo.
  struct addrinfo hints;
  memset(&hints, 0, sizeof hints);
  hints.ai_flags = AI_PASSIVE;
  hints.ai_family = PF_UNSPEC;
  hints.ai_socktype = SOCK_DGRAM;

  struct addrinfo *res;
  int error = getaddrinfo(NULL, SCHEDULERPORT, &hints, &res);
  if (error) {
    MSS_FATAL_MESSAGE(gai_strerror(error));
    error_ = true;
    return;
  }

  // Just in case res == NULL.
  sockfd_ = -1;

  // Iterate over list of address structures and try to bind socket
  // to any of them.
  struct addrinfo *p;
  for (p = res; p != NULL; p = p->ai_next) {
    sockfd_ = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
    if (sockfd_ == -1) {
      MSS_ERROR("socket", errno);
      continue;
    }

    int val = 1;
    if (setsockopt(sockfd_, SOL_SOCKET, SO_REUSEADDR, &val, sizeof val)) {
      MSS_ERROR("setsockopt", errno);
      close(sockfd_);
      continue;
    }

    if (bind(sockfd_, p->ai_addr, p->ai_addrlen) == -1) {
      MSS_ERROR("bind", errno);
      close(sockfd_);
      continue;
    }

    break;
  }

  if (p == NULL) {
    MSS_FATAL_MESSAGE("can not bind socket");
    error_ = true;
    return;
  }
  freeaddrinfo(res);
  error_ = false;
}

SchedulerServer::~SchedulerServer() {
  if (sockfd_ != -1)
    close(sockfd_);
}

void SchedulerServer::Run() {
  struct sockaddr_storage theiraddr;

  while (1) {
    // Commands consist of one-byte command and, possibly, domain name.
    // Domain name length is no greater than 255
    // See https://tools.ietf.org/html/rfc1035
    // One byte is for '\0'.
    char buf[257];
    memset(buf, '\0', sizeof buf);

    socklen_t salen;
    if (recvfrom(sockfd_, buf, sizeof buf - 1, 0,
        (struct sockaddr *)&theiraddr, &salen) == -1) {
      MSS_ERROR("recvfrom", errno);
      continue;
    }

    std::string cmd(buf);

    std::size_t pos = cmd.find("\n");
    if (pos != std::string::npos)
      cmd.erase(pos);

    if (cmd.empty())
      continue;

    std::string server = cmd.substr(1);
    switch (cmd[0]) {
    case 'G':
      if (server.empty()) {
        server = queue_.CmdGet();
        if (!server.empty())
          if (UNLIKELY(sendto(sockfd_, server.c_str(), server.size(), 0,
                              (struct sockaddr *)&theiraddr, salen) == -1))
            MSS_ERROR("sendto", errno);
      } else {
        queue_.CmdGet(server);
      }
      break;
    case 'R':
      server = cmd.substr(1);
      queue_.CmdRelease(server);
      break;
    }
  }
}
