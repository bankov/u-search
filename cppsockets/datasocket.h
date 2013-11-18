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

#ifndef LIBCPPSOCKETS_DATASOCKET_H_
#define LIBCPPSOCKETS_DATASOCKET_H_

#include <vector>

#include "abstractsocket.h"

/**
 * Socket which can send and receive data.
 */
class DataSocket : public AbstractSocket {
 public:
  /**
   * Simple constructor which create an object and init all fields.
   */
  DataSocket();

  /**
   * Constructor which create an object, init all fields and set
   * socket local address and port, remote address and port and type of socket.
   *
   * @param socket Socket.
   * @param local_address Local address and port.
   * @param remote_address Remote address and port.
   * @param type Type of socket.
   */
  DataSocket(int socket, SocketAddress &local_address,
             SocketAddress &remote_address, SocketType type);

  /**
   * Flush socket buffer (incoming buffer will be empty after call).
   */
  void Flush();

  /**
   * Read not more than size bytes from socket.
   *
   * @param data Buffer.
   * @param size Size of buffer.
   *
   * @return Readen size. On error return -1.
   */
  virtual size_t ReadData(void *data, size_t size);

  /**
   * Write size data from data to socket.
   *
   * @param data Buffer.
   * @param size Size of buffer.
   *
   * @return Readen size. On error return -1.
   */
  virtual size_t WriteData(void *data, size_t size);
};

#endif  // LIBCPPSOCKETS_DATASOCKET_H_
