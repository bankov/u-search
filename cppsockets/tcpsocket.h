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

#include <unistd.h>
#include <string>

#include "datasocket.h"

/**
 * @brief Class to work with TCP.
 */
class TCPSocket : public DataSocket {
 public:
  /**
   * Simple constructor.
   */
  TCPSocket();

  /**
   * Constructor which create an object and set a local address.
   *
   * @param local_address Local address and port.
   */
  explicit TCPSocket(SocketAddress *local_address);

  /**
   * Constructor which create an object, set a local and remote
   * addresses.
   *
   * @param local_address Local address and port.
   * @param remote_address Remote address and port.
   */
  TCPSocket(SocketAddress *local_address, SocketAddress *remote_address);

  /**
   * Destructor.
   */
  ~TCPSocket();

  /**
   * @brief Connect the socket to host with specified address and port.
   *
   * @param address Address.
   * @param port Port.
   *
   * @return 0 on success, -1 otherwise.
   */
  int ConnectToHost(const in_addr_t address, const in_port_t port);

  /**
   * @brief Connect the socket to host with specified address and port.
   *
   * @param address Address.
   * @param port Port.
   *
   * @return 0 on success, -1 otherwise.
   */
  int ConnectToHost(const char *address, const unsigned short port);

  /**
   * @brief Connect the socket to host with specified address and port.
   *
   * @param address Address.
   * @param port Port.
   *
   * @return 0 on success, -1 otherwise.
   */
  int ConnectToHost(const std::string *address, const unsigned short port);

  /**
   * @brief Connect the socket to host with specified address and port.
   *
   * @return 0 on success, -1 otherwise.
   */
  int Connect();

  /**
   * @brief WriteData Write size data from data to socket.
   *
   * @param buffer Buffer.
   * @param size Size of buffer.
   *
   * @return Readen size. On error return -1.
   */
  size_t WriteInSocket(void *buffer, size_t size);

  /**
   * @brief ReadData Read not more than size bytes from socket.
   *
   * @param buffer Buffer.
   * @param size Size of buffer.
   *
   * @return Readen size. On error return -1.
   */
  size_t ReadFromSocket(void *buffer, size_t size);

 private:
  int CreateSocket();
};

#endif  // LIBCPPSOCKETS_TCPSOCKET_H_
