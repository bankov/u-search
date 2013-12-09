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

#include <stdio.h>

#include <list>
#include <string>
#include <algorithm>
#include <iterator>

#include "scheduler/serverqueue.h"
#include "common-inl.h"

ServerQueue::ServerQueue() {
  servers_list_ = new(std::nothrow) std::list<Server>();
  if (UNLIKELY(servers_list_ == NULL)) {
    MSS_FATAL("servers_list_", ENOMEM);
    return;
  }

  ilast_server_ = servers_list_->begin();
}

ServerQueue::ServerQueue(const std::string &servers_file) : ServerQueue() {
  if (ReadServersList(servers_file))
    MSS_DEBUG_ERROR("ReadServersList", errno);
}

ServerQueue::~ServerQueue() {
  if (servers_list_)
    delete servers_list_;
}

void ServerQueue::AddServer(std::string server_name) {
  if (UNLIKELY(server_name.empty())) {
    return;  // Do nothing if an empty server name is passed.
  }

  if (UNLIKELY(std::find(servers_list_->begin(), servers_list_->end(),
      server_name) != servers_list_->end())) {
    MSS_WARN_MESSAGE(("Duplicated server: " + server_name).c_str());
    return;  // Do nothing if server name is in servers_list_ yet.
  }

  // Add a new server before the head of the queue so that it will be
  // returned by CmdGet() the next time
  ilast_server_ = servers_list_->emplace(ilast_server_, server_name);
}

int ServerQueue::ReadServersList(std::string servers_file) {
  FILE *fin = fopen(servers_file.c_str(), "r");
  if (UNLIKELY(fin == NULL)) {
    MSS_ERROR(("fopen: " + servers_file).c_str(), errno);
    return -1;
  }

  std::string temp;
  char *buf = NULL;
  size_t size = 0;
  // Read lines from servers_file and put names servers names in servers_list_
  while (getline(&buf, &size, fin) != -1) {
    temp.insert(0, buf);
    // Deleting '\n' symbols from the end of the string
    temp.erase(temp.end() - 1);
    if (UNLIKELY(temp.empty())) {
      continue;  // Do nothing if find an empty string.
    }
    AddServer(temp);
    temp.clear();
  }
  free(buf);
  fclose(fin);

  return 0;
}

std::string ServerQueue::CmdGet() {
  if (UNLIKELY(servers_list_ == NULL || servers_list_->empty())) {
    MSS_FATAL("", ENOMEM);
    return "";  // list of servers is empty
  }

  time_t current = time(NULL);

  // Iterator pointing to the current head of the queue.
  std::list<ServerQueue::Server>::iterator iprev_head = ilast_server_;

  bool found = false;
  do {
    found = current - ilast_server_->get_timestamp() > kMaxWait;
    if (found)
      break;
    if (UNLIKELY(++ilast_server_ == servers_list_->end())) {
      // If the last server is in the end of the list,
      // the next server to choose is from the beginning of the list.
      ilast_server_ = servers_list_->begin();
    }
  } while (ilast_server_ != iprev_head);

  if (!found)
    return "";

  ilast_server_->Refresh();

  std::string last_server_name = ilast_server_->get_name();

  if (UNLIKELY(++ilast_server_ == servers_list_->end()))
    ilast_server_ = servers_list_->begin();

  return last_server_name;
}

void ServerQueue::CmdGet(const std::string address) {
  if (UNLIKELY(servers_list_ == NULL || servers_list_->empty())) {
    // List of servers is empty.
    MSS_FATAL("", ENOMEM);
    return;
  }

  std::list<ServerQueue::Server>::iterator it =
    std::find(servers_list_->begin(), servers_list_->end(), address);
  if (UNLIKELY(it == servers_list_->end())) {
    // Server with name address hasn't been found.
    return;
  }

  it->Refresh();
}

void ServerQueue::CmdRelease(const std::string address) {
  if (UNLIKELY(servers_list_ == NULL || servers_list_->empty())) {
    // List of servers is empty.
    MSS_FATAL("", ENOMEM);
    return;
  }

  std::list<Server>::iterator it =
    std::find(servers_list_->begin(), servers_list_->end(), address);
  if (UNLIKELY(it == servers_list_->end())) {
    // Server with name address hasn't been found.
    return;
  }

  it->Reset();
}
