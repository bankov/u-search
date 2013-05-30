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

#ifndef LIBCPPSOCKETS_UDPSOCKET_H_
#define LIBCPPSOCKETS_UDPSOCKET_H_

#include "datasocket.h"

class UDPSocket : public DataSocket {
 public:
  UDPSocket();
  explicit UDPSocket(const SocketAddress *listenSocketAddress);

  /**
   * @brief AllowBroadcast Function tell to socket allow transmiting broadcast
   * message.
   *
   * @return Return true on success, or false on fail.
   */
  bool AllowBroadcast();

  /**
   * @brief RejectBroadcast Function tell socket reject transmiting broadcast
   * message. Default state.
   *
   * @return Return true on success, or false on fail.
   */
  bool RejectBroadcast();

  /**
   * @brief WaitDatagram Function will suspend thread which call her until some
   * data for recive would be avialible.
   *
   * @return
   */
  int WaitDatagram();

  /**
   * @brief Bind Function bind socket to address bindAddress, after this socket
   * can read data from network, sended on address and port matches with binded.
   *
   * @param bindAddress Address to bind.
   * @return 0 on success, -1 otherwise.
   */
  int Bind(const SocketAddress &bindAddress);

  /**
   * @brief ReciveDatagram Recive datagram no larger then maxSize storage point
   * on begin at *__buf, write source of datagram to address. If datagram
   * message is larger then maxSize, the excess bytes will be discarded.
   * @param __buf Buffer.
   * @param sourceAddress Address.
   * @param maxSize Buffer size.
   * @return Return count of recived bytes or -1.
   */
  ssize_t ReciveDatagram(void **__buf, SocketAddress *sourceAddress,
                       ssize_t maxSize);

  /**
   * @brief ReciveDatagram Recive datagram and storage point on begin of
   * datagram data at *__buf, write source to address.
   *
   * @param __buf Buffer.
   * @param sourceAddress Address
   * @return Return count of recived bytes or -1.
   */
  ssize_t ReciveDatagram(void **__buf, SocketAddress *sourceAddress);

  /**
   * @brief ReciveDatagram Recive datagram and storage begin of them in *__buf.
   *
   * @param __buf Buffer.
   * @return Return count of recived bytes or -1.
   */
  ssize_t ReciveDatagram(void **__buf);

  /**
   * @brief ReciveDatagram Recive datagram no larger then maxSize storage point
   * on begin at *__buf. If datagram message is larger then maxSize, the excess
   * bytes will be discarded.
   *
   * @param __buf Buffer.
   * @param maxSize Maximum size of datagram.
   * @return Return count of recived bytes or -1.
   */
  ssize_t ReciveDatagram(void **__buf, ssize_t maxSize);

  /**
   * @brief SendDatagram Send data of size bytes in datagram to
   * destinationAddress address and port.
   *
   * @param data Datagram.
   * @param destinationAddres Destination address.
   * @param size Size of datagram.
   * @return return count of sended bytes or -1.
   */
  ssize_t SendDatagram(void *data, SocketAddress *destinationAddres,
                     ssize_t size);
};

#endif  // LIBCPPSOCKETS_UDPSOCKET_H_
