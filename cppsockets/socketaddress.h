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

#ifndef LIBCPPSOCKETS_SOCKETADDRESS_H_
#define LIBCPPSOCKETS_SOCKETADDRESS_H_

#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <strings.h>
#include <stdio.h>
#include <errno.h>

#include <ostream>
#include <string>

#include "common-inl.h"

/**
 * @brief The class allows you to store and work with IPv4 addresses.
 *
 * The class allows you to store and work with full IPv4 addresses. Class allows
 * you to store and retrieve IPv4 address (IP address and port) formats: string,
 * network byte order, host byte order.
 */
class SocketAddress {
 public:
  /**
   * Copy constructor. Create the same object as sock.
   *
   * @param address a copy of the object that want to be.
   */
  SocketAddress(const SocketAddress &address);

  /**
   * Create object with specified address and empty port.
   *
   * @param address Address, in the form of an array of characters, to be
   * stored in the new object.
   * @param port Port, in the host byte order, to be
   * stored in the new object.
   */
  SocketAddress(const char *address, const short port = 0);

  /**
   * Create object with specified address and empty port.
   *
   * @param address Address, in the form of std::string, to be
   * stored in the new object.
   * @param port Port, in the host byte order, to be
   * stored in the new object.
   */
  SocketAddress(const std::string *address, const short port = 0);

  /**
   * Create object with specified address and port.
   *
   * @param address Address, in the network byte order, to be
   * stored in the new object.
   * @param port Port, in the network byte order, to be
   * stored in the new object.
   */
  SocketAddress(const in_addr_t address = 0, const in_port_t port = 0);

  /**
   * Create object with specified address and empty port.
   *
   * @param address Address, in the host byte order, to be
   * stored in the new object.
   * @param port Port, in the host byte order, to be
   * stored in the new object.
   */
  SocketAddress(const int address, const short port = 0);

  /**
   * Set addess.
   *
   * @param address Address in network byte order
   */
  inline void set_address(const in_addr_t address) { address_ = address; }

  /**
   * Set address.
   *
   * @param address Address in host byte order
   */
  inline void set_address(const int address) { address_ = htonl(address); }

  /**
   * Set address.
   *
   * @param address Pointer to human readable string of Address, stored in
   * array of characters.
   *
   * @return 0 on success, -1 otherwise.
   */
  int set_address(const char *address);

  /**
   * Set address.
   *
   * @param address Pointer to human readable string of Address, stored in
   * std::string object.
   *
   * @return 0 on success, -1 otherwise.
   */
  int set_address(const std::string *address);

  /**
   * Set port.
   *
   * @param port Port in network byte order.
   */
  inline void set_port(const in_port_t port) { port_ = port; }

  /**
   * Set port.
   *
   * @param port Port in host byte order
   */
  inline void set_port(const short port) { port_ = htons(port); }

  /**
   * Return code of error occurred during the last operation.
   *
   * @return Last error or 0.
   */
  inline int get_error() { return error_; }

  /**
   * @brief Convert saved address in array of characters.
   *
   * Function convert saved address to human readable char array and return
   * it. After the call internal format of the data in the object does not
   * change.
   *
   * @return Address in the human readable format as array of characters.
   */
  char *GetAddressAsChar();

  /**
   * @brief Convert saved address in std::string.
   *
   * Function convert saved address to std::string human readable format and
   * return it. After the call internal format of the data in the object does
   * not change.
   *
   * @return Address in the human readable format as std::string.
   */
  std::string GetAddressAsString();

  /** @brief Convert saved address to network byte order.
   *
   * Function convert saved address to network byte order and
   * return it. After the call internal format of the data in the object does
   * not change.
   *
   * @return Address in the network byte order.
   */
  inline in_addr_t GetAddressAsNet() { return address_; }

  /**
   * @brief Convert saved address to network byte order.
   *
   * Function convert saved address to host byte order and
   * return it. After the call internal format of the data in the object does
   * not change.
   *
   * @return Address in the host byte order.
   */
  inline int GetAddressAsHost() { return ntohl(address_); }

  /**
   * @brief Convert saved port to network byte order.
   *
   * Function convert saved port to network byte order and
   * return it. After the call internal format of the data in the object does
   * not change.
   *
   * @return Port in the network byte order.
   */
  inline in_port_t GetPortAsNet() { return port_; }

  /**
   * @brief Convert saved port to host byte order.
   *
   * Function convert saved port to network byte order and
   * return it. After the call internal format of the data in the object does
   * not change.
   *
   * @return Port in the host byte order.
   */
  inline int GetPortAsHost() { return ntohs(port_); }

#ifndef DOXYGEN_SHOULD_SKIP_THIS
  /**
   * Makes a copy of the object argument.
   *
   * @param address A copy of the object that you want to be.
   *
   * @return A copy of given object.
   */
  SocketAddress& operator=(const SocketAddress &address);

  /**
   * Function displays the object to the standard output in the form of
   * a.b.c.d:efghi.
   *
   * @param __cout Output stream.
   * @param obj Object for writing to stream.
   *
   * @return ostream object for given SocketAddress.
   */
  friend std::ostream& operator<<(std::ostream& __cout,
                                  const SocketAddress &obj);
  /**
   * Function displays the object to the standard output in the form of
   * a.b.c.d:efghi
   *
   * @param __cout Output stream
   * @param obj Object for writing to stream
   *
   * @return ostream object for given SocketAddress.
   */
  friend std::ostream& operator<<(std::ostream& __cout, SocketAddress *obj);
#endif  // DOXYGEN_SHOULD_SKIP_THIS

 private:
  inline void DetectError() { error_ = errno; }

  /** Variable stores the last error */
  int error_;

  /** Variable stores address in the network byte order */
  in_addr_t address_;

  /** Variable stores port in the network byte order */
  in_port_t port_;
};

#endif  // LIBCPPSOCKETS_SOCKETADDRESS_H_
