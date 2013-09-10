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
  set_local_address(local_address->GetAddressAsNet());
  set_local_port(local_address->GetPortAsNet());

  // Creating socket
  int temp = socket(PF_INET, SOCK_STREAM, 0);

  if (UNLIKELY(temp < 0)) {
    DetectError();
    MSS_DEBUG_ERROR("socket", get_error());
    return;
  }

  int optval = 1;
  if (UNLIKELY(setsockopt(temp, SOL_SOCKET, SO_REUSEADDR, &optval,
                          sizeof(optval)) < 0)) {
    DetectError();
    MSS_DEBUG_ERROR("setsockopt", get_error());
    return;
  }

  struct sockaddr_in serverAddress;
  serverAddress.sin_family = AF_INET;
  serverAddress.sin_addr.s_addr = get_local_address();
  serverAddress.sin_port = get_local_port();

  // Bind a name to a socket
  if (UNLIKELY(bind(temp, (struct sockaddr *)&serverAddress,
                    sizeof(serverAddress)) < 0)) {
    DetectError();
    MSS_DEBUG_ERROR("bind", get_error());
    return;
  }

  set_error(0);
  set_socket(temp);
}

TCPSocket::TCPSocket(SocketAddress *local_address,
                     SocketAddress *remote_address) : TCPSocket(local_address) {
  set_remote_address(remote_address->GetAddressAsNet());
  set_remote_port(remote_address->GetPortAsNet());

  // Connecting to remote address
  set_state(ConnectingState);

  struct sockaddr_in clientAddress;
  clientAddress.sin_family = AF_INET;
  clientAddress.sin_addr.s_addr = get_remote_address();
  clientAddress.sin_port = get_remote_port();
  socklen_t clientAddressLength = sizeof(clientAddress);

  // Connect a socket
  if (UNLIKELY(connect(get_socket(), (struct sockaddr *)&clientAddress,
                       clientAddressLength) < 0)) {
    DetectError();
    MSS_DEBUG_ERROR("connect", get_error());
    set_state(UnconnectedState);
    return;
  }

  set_state(ConnectedState);
}

TCPSocket::~TCPSocket() {
  Abort();
}

int TCPSocket::ConnectToHost(const in_addr_t address, const in_port_t port) {
  set_remote_address(address);
  set_remote_port(port);

  if (get_socket() <= 0) {
    // Creating socket if we had bad one
    set_socket(socket(PF_INET, SOCK_STREAM, 0));

    if (UNLIKELY(get_socket() < 0)) {
      DetectError();
      MSS_DEBUG_ERROR("socket", get_error());
      return -1;
    }

    int optval = 1;
    if (UNLIKELY(setsockopt(get_socket(), SOL_SOCKET, SO_REUSEADDR, &optval,
                            sizeof(optval)) < 0)) {
      DetectError();
      MSS_DEBUG_ERROR("setsockopt", get_error());
      return -1;
    }

    struct sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = get_local_address();
    serverAddress.sin_port = get_local_port();

    // Bind a name to a socket
    if (UNLIKELY(bind(get_socket(), (struct sockaddr *)&serverAddress,
                      sizeof(serverAddress)) < 0)) {
      DetectError();
      MSS_DEBUG_ERROR("bind", get_error());
      return -1;
    }
  }

  struct sockaddr_in clientAddress;
  clientAddress.sin_family = AF_INET;
  clientAddress.sin_port = get_remote_port();
  clientAddress.sin_addr.s_addr = get_remote_address();
  socklen_t clientAddressLength = sizeof(clientAddress);

  // Connecting to remote host
  set_state(ConnectingState);

  // Connect a socket
  if (UNLIKELY(connect(get_socket(), (struct sockaddr *)&clientAddress,
                       clientAddressLength) < 0)) {
    DetectError();
    MSS_DEBUG_ERROR("connect", get_error());
    set_state(UnconnectedState);
    return -1;
  }

  set_state(ConnectedState);
  return 0;
}

int TCPSocket::ConnectToHost(const char *address,
                             const unsigned short port) {
  if (address == NULL) {
    set_error(EINVAL);
    MSS_DEBUG_ERROR("ConnectToHost", get_error());
    return -1;
  }

  SocketAddress server_address(address, port);
  if (UNLIKELY(ConnectToHost(server_address.GetAddressAsNet(),
                             server_address.GetPortAsNet()))) {
    MSS_DEBUG_ERROR("ConnectToHost", get_error());
    return -1;
  }

  set_state(ConnectedState);
  return 0;
}

int TCPSocket::ConnectToHost(const std::string *address,
                             const unsigned short port) {
  if (UNLIKELY(address == NULL)) {
    set_error(EINVAL);
    MSS_DEBUG_ERROR("ConnectToHost", get_error());
    return -1;
  }

  SocketAddress server_address(address, port);
  if (UNLIKELY(ConnectToHost(server_address.GetAddressAsNet(),
                             server_address.GetPortAsNet()))) {
    MSS_DEBUG_ERROR("ConnectToHost", get_error());
    return -1;
  }

  return 0;
}

size_t TCPSocket::WriteInSocket(void *buffer, size_t size) {
  if (UNLIKELY((get_error() != 0) || (get_state() != ConnectedState))) {
    MSS_DEBUG_MESSAGE("WriteInSocket: Bad socket");
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
    MSS_DEBUG_MESSAGE("ReadFromSocket: Bad socket");
    return -1;
  }

  ssize_t count = read(this->get_socket(), buffer, size);

  if (UNLIKELY(count < 0)) {
    DetectError();
    MSS_DEBUG_ERROR("read", get_error());
  }

  return count;
}
