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

#ifndef SERVER_BROADCASTLISTENER_H_
#define SERVER_BROADCASTLISTENER_H_

#include <pthread.h>
#include <stdlib.h>
#include <strings.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <iostream>
#include "threadobject.h"

#define DEFAULT_ANSWER "mipt-smb-search preview server is here"
#define DEFAULT_LISTEN "mipt-smb-search preview order"
#define DEFAULT_PORT 38000

class BroadcastListener : public ThreadObject {
  public:
    BroadcastListener();
    virtual ~BroadcastListener();
    explicit BroadcastListener(int port);
    BroadcastListener(int port, char *answer, char *listen);

    // Inline methods
    void StartReply() { answery = true; }
    void StopReply() { answery = false; }
    int GetTransportSocket() { return transportSocket; }
    int get_listenport() { return listenport; }
    bool get_answery() { return answery; }

    bool set_listenport(int newport);
    bool ChangeListenMessage(const char *newMessage);
    bool ChangeAnswerMessage(const char *newMessage);

  protected:
    char *answermessage;
    char *listenmessage;

    static void CloseSocket(void *socket_to_close);
    static void FreeMessage(void *message_to_free);

  private:
    bool answery;  // Shoud thread answer on query?
    int listenport;
    int transportSocket;

    virtual void *ThreadRoutine();
    bool set_listenmessage(const char *newmessage);
    bool set_answermessage(const char *newmessage);
    void InitMessageToNull();
};

#endif  // SERVER_BROADCASTLISTENER_H_
