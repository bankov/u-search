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

#include "tcpsocket.h"

TCPSocket::TCPSocket() : DataSocket() {
  set_type(TCP);
}

TCPSocket::TCPSocket(SocketAddress *local_address) : TCPSocket() {
  set_local_address(local_address);

  if (CreateSocket())
    return;

  set_error(0);
}

TCPSocket::TCPSocket(SocketAddress *local_address,
                     SocketAddress *remote_address) : TCPSocket(local_address) {
  set_remote_address(remote_address);
  Connect();
}

TCPSocket::~TCPSocket() {
  Abort();
}

int TCPSocket::CreateSocket() {
  // Creating socket
  int temp = socket(PF_INET, SOCK_STREAM, 0);

  if (UNLIKELY(temp < 0)) {
    DetectError();
    MSS_DEBUG_ERROR("socket", get_error());
    return -1;
  }

  int optval = 1;
  if (UNLIKELY(setsockopt(temp, SOL_SOCKET, SO_REUSEADDR, &optval,
                          sizeof(optval)) < 0)) {
    DetectError();
    MSS_DEBUG_ERROR("setsockopt", get_error());
    return -1;
  }

  struct sockaddr_in server_address;
  server_address.sin_family = AF_INET;
  server_address.sin_addr.s_addr = get_local_address();
  server_address.sin_port = get_local_port();

  // Bind a name to a socket
  if (UNLIKELY(bind(temp, (struct sockaddr *)&server_address,
                    sizeof(server_address)) < 0)) {
    DetectError();
    MSS_DEBUG_ERROR("bind", get_error());
    return -1;
  }

  set_socket(temp);
  return 0;
}

int TCPSocket::Connect() {
  if (get_error())
    return -1;

  if (get_socket() <= 0) {
    if (CreateSocket())
      return -1;
  }

  struct sockaddr_in client_address;
  client_address.sin_family = AF_INET;
  client_address.sin_port = get_remote_port();
  client_address.sin_addr.s_addr = get_remote_address();

  // Connect a socket
  set_state(ConnectingState);
  if (UNLIKELY(connect(get_socket(), (struct sockaddr *)&client_address,
                       sizeof(client_address)) < 0)) {
    DetectError();
    MSS_DEBUG_ERROR("connect", get_error());
    set_state(UnconnectedState);
    return -1;
  }

  set_state(ConnectedState);
  return 0;
}

int TCPSocket::ConnectToHost(const in_addr_t address, const in_port_t port) {
  set_remote_address(SocketAddress(address, port));
  return Connect();
}

int TCPSocket::ConnectToHost(const char *address,
                             const unsigned short port) {
  set_remote_address(SocketAddress(address, port));
  return Connect();
}

int TCPSocket::ConnectToHost(const std::string *address,
                             const unsigned short port) {
  set_remote_address(SocketAddress(address, port));
  return Connect();
}

size_t TCPSocket::WriteInSocket(void *buffer, size_t size) {
  if (UNLIKELY((get_error() != 0) || (get_state() != ConnectedState))) {
    MSS_DEBUG_MESSAGE("TCPSocket::WriteInSocket: Bas socket");
    return -1;
  }

  ssize_t count = write(this->get_socket(), buffer, size);

  if (UNLIKELY(count < 0)) {
    DetectError();
    MSS_DEBUG_ERROR("write", get_error());
  }

  return count;
}

size_t TCPSocket::ReadFromSocket(void *buffer, size_t size) {
  if (UNLIKELY((get_error() != 0) || (this->get_state() != ConnectedState))) {
    MSS_DEBUG_MESSAGE("TCPSocket::ReadFromSocket: Bas socket");
    return -1;
  }

  ssize_t count = read(this->get_socket(), buffer, size);

  if (UNLIKELY(count < 0)) {
    DetectError();
    MSS_DEBUG_ERROR("read", get_error());
  }

  return count;
}
