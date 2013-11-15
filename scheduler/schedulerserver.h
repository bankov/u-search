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

#ifndef SCHEDULER_SCHEDULERSERVER_H_
#define SCHEDULER_SCHEDULERSERVER_H_

#include <string>

#include "scheduler/schedulerserver.h"
#include "scheduler/serverqueue.h"

/**
 * @brief Scheduler server, used to distribute jobs among spiders.
 */
class SchedulerServer {
 public:
  /**
   * Constructor.
   *
   * @param serversfile Name of file with the list of servers.
   */
  explicit SchedulerServer(const std::string serversfile);

  /**
   * Destructor, closes socket.
   */
  ~SchedulerServer();

  /**
   * Start listening on socket and hand out jobs.
   */
  void Run();

  /**
   * Check if errors occured during initialization.
   */
  bool is_error() const { return error_; }

 private:
  /**
   * @brief Some queue to get servers from.
   */
  ServerQueue queue_;
  /**
   * @brief socket file descriptor
   */
  int sockfd_;
  /**
   * @brief If error occured.
   */
  bool error_;
};

#endif  // SCHEDULER_SCHEDULERSERVER_H_
