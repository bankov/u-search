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

// Constructors
BroadcastListener::BroadcastListener() : ThreadObject() {
  this->InitMessageToNull();
  this->set_answermessage(DEFAULT_ANSWER);
  this->set_listenmessage(DEFAULT_LISTEN);

  this->set_listenport(DEFAULT_PORT);

  this->answery = false;

  this->Start();
}

BroadcastListener::BroadcastListener(int port) : ThreadObject() {
  this->InitMessageToNull();
  this->set_answermessage(DEFAULT_ANSWER);
  this->set_listenmessage(DEFAULT_LISTEN);

  this->set_listenport(port);

  this->StopReply();

  this->Start();
}

BroadcastListener::BroadcastListener(int port, char *answer, char *listen) :
  ThreadObject() {
  this->InitMessageToNull();
  this->set_answermessage(answer);
  this->set_listenmessage(listen);

  this->set_listenport(port);

  this->StopReply();

  this->Start();
}

void BroadcastListener::InitMessageToNull() {
  this->answermessage = NULL;
  this->listenmessage = NULL;
}

// Destructors
BroadcastListener::~BroadcastListener() {
    this->Cancel();
    free(this->answermessage);
    free(this->listenmessage);
}

// Methods
bool BroadcastListener::set_listenport(int newport) {
  if (!this->is_run())
    this->listenport = newport;

  int status = this->Cancel();
  if (status != 0) {
    this->set_lasterrno(status);
    return false;
  }

  status = this->Join();
  if (status != 0) {
    this->set_lasterrno(status);
    return false;
  }

  listenport = newport;

  status = this->Start();
  if (status != 0) {
    this->set_lasterrno(status);
    return false;
  }

  return true;
}

bool BroadcastListener::set_answermessage(const char *newmessage) {
  if (this->is_run()) {
    this->set_lasterrno(EBUSY);
    return false;
  }

  if (answermessage != NULL)
    free(answermessage);

  answermessage = (char*) malloc((strlen(newmessage) + 1) * sizeof(char));
  if (this->answermessage == NULL) {
    perror("malloc answermessage");
    this->set_lasterrno(errno);
    return false;
  }

  strcpy(answermessage, newmessage);

  return true;
}

bool BroadcastListener::set_listenmessage(const char *newmessage) {
  if (this->is_run()) {
    this->set_lasterrno(EBUSY);
    return false;
  }

  if (listenmessage != NULL)
    free(listenmessage);

  listenmessage = (char*) malloc((strlen(newmessage) + 1) * sizeof(char));
  if (listenmessage == NULL) {
    perror("malloc listenmessage");
    this->set_lasterrno(errno);
    return false;
  }

  strcpy(listenmessage, newmessage);

  return true;
}

bool BroadcastListener::ChangeAnswerMessage(const char *newMessage) {
  int state = this->Cancel();
  if (state != 0) {
    this->set_lasterrno(state);
    return false;
  }

  state = this->Join();
  if (state != 0) {
    this->set_lasterrno(state);
    return false;
  }

  if (!this->set_answermessage(newMessage))
    return false;

  state = this->Start();
  if (state != 0) {
    this->set_lasterrno(state);
    return false;
  }

  return true;
}

bool BroadcastListener::ChangeListenMessage(const char *newMessage) {
  int state = this->Cancel();
  if (state != 0) {
    this->set_lasterrno(state);
    return false;
  }

  state = this->Join();
  if (state != 0) {
    this->set_lasterrno(state);
    return false;
  }

  if (!this->set_listenmessage(newMessage))
    return false;

  state = this->Start();
  if (state != 0) {
    this->set_lasterrno(state);
    return false;
  }

  return true;
}

void *BroadcastListener::ThreadRoutine() {
  if (this->answermessage == NULL || this->listenmessage == NULL) {
    this->set_lasterrno(EBADF);
    return NULL;
  }

  // Create socket for listen query
  int sock_broadcast = socket(PF_INET, SOCK_DGRAM, 0);
  if (sock_broadcast == -1) {
    perror("Create broadcast socket failed");
    set_lasterrno(errno);
    pthread_exit(NULL);
  }

  // Struct for define and bind recive socket
  struct sockaddr_in addr_broadcast_recive;
  bzero(&addr_broadcast_recive, sizeof(addr_broadcast_recive));
  addr_broadcast_recive.sin_family = AF_INET;
  addr_broadcast_recive.sin_addr.s_addr = htonl(INADDR_ANY);
  addr_broadcast_recive.sin_port = htons(this->get_listenport());

  pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);

  // Bind structure to socket
  if(bind(sock_broadcast, (struct sockaddr*) &addr_broadcast_recive,
          sizeof(addr_broadcast_recive)) < 0) {
    perror("Bind broadcast socket failed");
    this->set_lasterrno(errno);
    pthread_exit(NULL);
  }

  // Function for close the socket when thread canceled
  pthread_cleanup_push(BroadcastListener::CloseSocket,
                       (void*) &sock_broadcast);
  pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
  pthread_testcancel();

  // Create a set on which we will wait query with "select"
  fd_set broadcast_read_set;
  FD_ZERO(&broadcast_read_set);
  FD_SET(sock_broadcast, &broadcast_read_set);

  // Define listening message size
  int message_length = strlen(this->listenmessage);

  pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);

  // Buffer for store message from client
  char *hello_message = (char*) malloc(sizeof(char) * (message_length + 1));
  bzero(hello_message, sizeof(char) * (message_length + 1));
  pthread_cleanup_push(BroadcastListener::FreeMessage,
                       (void*)hello_message);

  pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
  pthread_testcancel();

  int sockstate = 1;

  while (sockstate > 0) {
    sockstate = select(sock_broadcast + 1, &broadcast_read_set, NULL,
                       &broadcast_read_set, NULL);

    if (sockstate == -1) {
      perror("Problems on broadcast socket");
      this->set_lasterrno(errno);
      pthread_exit(NULL);
    }

    // Struct to store data about asking the client
    struct sockaddr_in addr_client_info;
    // Her size
    socklen_t client_info_size = sizeof(addr_client_info);

    // Read from socket
    int readed_byte = recvfrom(sock_broadcast, (void*) hello_message,
                               message_length, 0,
                               (struct sockaddr*) &addr_client_info,
                               &client_info_size);
     if (readed_byte < 0) {
       this->set_lasterrno(errno);
       pthread_exit(NULL);
     }

     // If client send right message and we allow to answer
     if (strcmp(hello_message, listenmessage) == 0 &&
         this->get_answery() == true) {
       int sended_data =
           sendto(sock_broadcast, answermessage, strlen(answermessage), 0,
                  (struct sockaddr*) &addr_client_info,
                  sizeof(struct sockaddr_in));
       if (sended_data < 0) {
         std::cerr << "Error on answering to client " <<
                      inet_ntoa(addr_client_info.sin_addr) << " on port" <<
                      ntohs(addr_client_info.sin_port) << "\n";
         perror("Problem is");
       }
     }

     // Set a set to listen again
     FD_ZERO(&broadcast_read_set);
     FD_SET(sock_broadcast, &broadcast_read_set);
  }

  pthread_cleanup_pop(1);
  pthread_cleanup_pop(1);

  return NULL;
}

// Function for close socket when thread is canceled
void BroadcastListener::CloseSocket(void *socket_to_close) {
  close(*(int*)socket_to_close);
}

// Function for free allocated memory when thread is canceled
void BroadcastListener::FreeMessage(void *message_to_free) {
  free(message_to_free);
}

