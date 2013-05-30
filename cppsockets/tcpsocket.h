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

#ifndef LIBCPPSOCKETS_TCPSOCKET_H_
#define LIBCPPSOCKETS_TCPSOCKET_H_

#include "datasocket.h"
#include <unistd.h>
#include <string>

class TCPSocket : public DataSocket {
 public:
  TCPSocket();
  explicit TCPSocket(SocketAddress *local_address);
  TCPSocket(SocketAddress *local_address, SocketAddress *remote_address);

  ~TCPSocket();

  // Connect the socket to host with specifed address and port
  int ConnectToHost(const in_addr_t address, const in_port_t port);
  int ConnectToHost(const char *address, const short port);
  int ConnectToHost(const std::string *address, const short port);

  /**
   * @brief WriteData Write size data from data to socket.
   * @param data Buffer.
   * @param size Size of buffer.
   * @return Readen size. On error return -1.
   */
  size_t WriteInSocket(void *buffer, size_t size);

  /**
   * @brief ReadData Read not more than size bytes from socket.
   * @param data Buffer.
   * @param size Size of buffer.
   * @return Readen size. On error return -1.
   */
  size_t ReadFromSocket(void *buffer, size_t size);
};

#endif  // LIBCPPSOCKETS_TCPSOCKET_H_
