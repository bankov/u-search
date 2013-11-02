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

#include <netinet/in.h>
#include <arpa/inet.h>

#include <sys/select.h>
#include <sys/socket.h>

#include <errno.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>

#include <iostream>

#include "threadobject.h"
#include "broadcastlistener.h"
#include "task-listener.h"

void *TaskListener::ThreadRoutine() {
  int connection_socket;
  struct sockaddr_in listen_addr;

  connection_socket = socket(PF_INET, SOCK_STREAM, 0);
  if (connection_socket < 0) {
    perror("Can't create listen socket");
    return NULL;
  }

  listen_addr.sin_family = AF_INET;
  listen_addr.sin_port = htons(this->get_listenport());
  listen_addr.sin_addr.s_addr = INADDR_ANY;

  int a = 1;
  if (setsockopt(connection_socket, SOL_SOCKET, SO_REUSEADDR, &a, sizeof(a))
      < 0) {
    perror("Set listener socket options");
    return NULL;
  }

  if (bind(connection_socket, (struct sockaddr*) &listen_addr,
           sizeof(listen_addr)) < 0) {
    perror("Can't bind listen socket");
    return NULL;
  }

  if (listen(connection_socket, 0) < 0) {
    perror("Error listen socket");
    return NULL;
  }

  while (this->get_lasterrno() == 0) {
    std::cout << "\nWait new connection on port " << this->get_listenport()
              << "\n";
    int client_stream;
    this->StartReply();

    struct sockaddr_in client_addr;
    socklen_t client_addr_length = sizeof(client_addr);

    client_stream = accept(connection_socket, (struct sockaddr*) &client_addr,
                           &client_addr_length);
    if (client_stream < 0) {
      perror("Client accepting");
      // this->set_lasterrno(errno);
      pthread_exit(NULL);
    }
    std::cout << "[" << inet_ntoa(client_addr.sin_addr) << "]" <<
                 "New connection," << "waiting data\n";
    fd_set client_set;
    FD_ZERO(&client_set);
    FD_SET(client_stream, &client_set);

    struct timeval timeout;
    timeout.tv_sec = 20;
    timeout.tv_usec = 10;

    // Wait data from client for 20 seconds
    int wait_result = select(client_stream + 1, &client_set, NULL, NULL,
                             &timeout);

    if (wait_result == -1) {
      // If fail, close connection and report about problem
      std::cerr << inet_ntoa(client_addr.sin_addr) << "No data in 20 " <<
                   " seconds, close connection\n";
      // close(client_stream);
    } else {
      // Else read data
      pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);
      void *message_size = malloc(sizeof(unsigned int));
      pthread_cleanup_push(free, message_size);
      pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);

      int recv_bytes = recv(client_stream, message_size, sizeof(unsigned int),
                            0);
      if (recv_bytes < (int) sizeof(unsigned int)) {
        std::cerr << inet_ntoa(client_addr.sin_addr) <<
                     "Invalid message size, close connection";
      }
      pthread_cleanup_pop(1);
    }
  }

  return NULL;
}

