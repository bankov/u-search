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

#include <fcntl.h>
#include <string.h>
#include <syslog.h>

#include <string>

#include "common.h"
#include "socketaddress.h"
#include "abstractsocket.h"
#include "datasocket.h"
#include "tcplistener.h"

TCPListener::TCPListener() : AbstractSocket() {
  backlog_ = 0;
}

TCPListener::TCPListener(SocketAddress *local_address, int backlog)
    : AbstractSocket() {
  backlog_ = backlog;

  // Creating socket
  int temp = socket(PF_INET, SOCK_STREAM, 0);
  if (UNLIKELY(temp < 0)) {
    DetectError();
    MSS_DEBUG_ERROR("socket", get_error());
    return;
  }

  set_socket(temp);

  int optval = 1;
  if (UNLIKELY(setsockopt(get_socket(), SOL_SOCKET, SO_REUSEADDR, &optval,
                          sizeof(optval)) < 0)) {
    DetectError();
    MSS_DEBUG_ERROR("setsockopt", get_error());
    return;
  }

  struct sockaddr_in serverAddress;
  memset(&serverAddress, 0, sizeof(serverAddress));
  serverAddress.sin_family = AF_INET;
  serverAddress.sin_addr.s_addr = local_address->GetAddressAsNet();
  serverAddress.sin_port = local_address->GetPortAsNet();
  socklen_t serverStructSize = sizeof(serverAddress);

  if (UNLIKELY(bind(temp, (struct sockaddr *)&serverAddress,
                    sizeof(serverAddress)) < 0)) {
    DetectError();
    MSS_DEBUG_ERROR("bind", get_error());
    return;
  }

  // Detecting real binded address
  if (UNLIKELY(getsockname(temp, (struct sockaddr*) &serverAddress,
                           &serverStructSize) < 0)) {
    DetectError();
    MSS_DEBUG_ERROR("getsockname", get_error());
    return;
  }

  if (UNLIKELY(listen(temp, backlog_) < 0)) {
    DetectError();
    MSS_DEBUG_ERROR("listen", get_error());
    return;
  }

  set_local_address(serverAddress.sin_addr.s_addr);
  set_local_port(serverAddress.sin_port);
  set_socket(temp);
  set_state(ListeningState);
  set_type(Listener);
}

TCPListener::~TCPListener() {
  Abort();
}

DataSocket *TCPListener::AcceptNoWait() {
  // Reading the socket status flags
  int temp = fcntl(get_socket(), F_GETFL, NULL);
  if (UNLIKELY(temp < 0)) {
    DetectError();
    MSS_DEBUG_ERROR("fcntl", get_error());
    return NULL;
  }

  // Setting flag no wait
  if (UNLIKELY((fcntl(get_socket(), F_SETFL, temp | O_NONBLOCK)) == -1)) {
    DetectError();
    MSS_DEBUG_ERROR("fcntl", get_error());
    return NULL;
  }

  // Try to accept connection
  struct sockaddr_in clientAddress;
  socklen_t clientAddressLength = sizeof(clientAddress);

  if (UNLIKELY((temp = accept(get_socket(), (struct sockaddr *)&clientAddress,
                              &clientAddressLength)) < 0)) {
    DetectError();
    MSS_DEBUG_ERROR("accept", get_error());
    return NULL;
  }

  SocketAddress addr(clientAddress.sin_addr.s_addr, clientAddress.sin_port);
  DataSocket *data_socket =
      new(std::nothrow) DataSocket(temp, get_local_socket_address(),
                                   addr, TCP);
  if (UNLIKELY(data_socket == NULL)) {
    set_error(ENOMEM);
    MSS_DEBUG_ERROR("DataSocket", get_error());
    return NULL;
  }

  return data_socket;
}

DataSocket *TCPListener::Accept() {
  // Try to accept connection
  struct sockaddr_in clientAddress;
  socklen_t clientAddressLength = sizeof(clientAddress);

  int temp = accept(get_socket(), (struct sockaddr *)&clientAddress,
                    &clientAddressLength);
  if (UNLIKELY(temp < 0)) {
    DetectError();
    MSS_DEBUG_ERROR("accept", get_error());
    return NULL;
  }

  SocketAddress addr(clientAddress.sin_addr.s_addr, clientAddress.sin_port);
  DataSocket *data_socket =
      new(std::nothrow) DataSocket(temp, get_local_socket_address(),
                                   addr, TCP);
  if (UNLIKELY(data_socket == NULL)) {
    set_error(ENOMEM);
    MSS_DEBUG_ERROR("DataSocket", get_error());
    return NULL;
  }

  return data_socket;
}
