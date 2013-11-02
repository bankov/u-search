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

/**
 * @brief Class to work with UDP.
 */
class UDPSocket : public DataSocket {
 public:
  /**
   * Simple constructor.
   */
  UDPSocket();

  /**
   * Create an object and set set an address
   *
   * @param listenSocketAddress Address and port to be set as local address and
   * port.
   */
  explicit UDPSocket(const SocketAddress *listenSocketAddress);

  /**
   * @brief Tell socket allow transmiting broadcast message.
   *
   * @return Return true on success, or false on fail.
   */
  bool AllowBroadcast();

  /**
   * @brief Tell socket reject transmiting broadcast message. Default state.
   *
   * @return Return true on success, or false on fail.
   */
  bool RejectBroadcast();

  /**
   * @brief Suspend thread which call her until some data for recive
   * would be avialible.
   *
   * @return Return 0 on success, -1 otherwise.
   */
  int WaitDatagram();

  /**
   * @brief Bind socket to address bindAddress, after this socket
   * can read data from network, sended on address and port matches with binded.
   *
   * @param address Address to bind.
   *
   * @return 0 on success, -1 otherwise.
   */
  int Bind(const SocketAddress &address);

  /**
   * @brief Receive datagram.
   *
   * Receive datagram no larger then maxSize storage point on begin at *buf,
   * write source of datagram to address. If datagram message is larger then
   * size, the excess bytes will be discarded.
   *
   * @param buf Buffer.
   * @param address Address.
   * @param size Buffer size.
   *
   * @return Return count of recived bytes or -1.
   */
  ssize_t ReceiveDatagram(void **buf, SocketAddress *address, ssize_t size);

  /**
   * @brief Receive datagram.
   *
   * Recieve datagram and storage point on begin of datagram data at buf,
   * write source to address.
   *
   * @param buf Buffer.
   * @param address Address
   *
   * @return Return count of recived bytes or -1.
   */
  ssize_t ReceiveDatagram(void **buf, SocketAddress *address);

  /**
   * @brief Receive datagram and storage begin of them in *buf.
   *
   * @param buf Buffer.
   * @return Return count of recived bytes or -1.
   */
  ssize_t ReceiveDatagram(void **buf);

  /**
   * @brief Receive datagram
   *
   * Receive datagram no larger then size storage point on begin at *buf. If
   * datagram message is larger then size, the excess bytes will be discarded.
   *
   * @param buf Buffer.
   * @param size Maximum size of datagram.
   * @return Return count of recived bytes or -1.
   */
  ssize_t ReceiveDatagram(void **buf, ssize_t size);

  /**
   * @brief Send data of size bytes in datagram to specified address and port.
   *
   * @param data Datagram.
   * @param address Destination address.
   * @param size Size of datagram.
   *
   * @return return count of sended bytes or -1.
   */
  ssize_t SendDatagram(void *data, SocketAddress *address, ssize_t size);
};
