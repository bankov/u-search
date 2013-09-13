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

#include "socketaddress.h"

SocketAddress::SocketAddress() : error_(0), address_(0), port_(0) {}

SocketAddress::SocketAddress(const SocketAddress &address)
    : error_(0), address_(address.address_), port_(address.port_) {}

SocketAddress::SocketAddress(const char *address, const short port)
    : error_(0) {
  set_address(address);
  set_port(port);
}

SocketAddress::SocketAddress(const std::string *address, const short port)
    : error_(0) {
  set_address(address);
  set_port(port);
}

SocketAddress::SocketAddress(const in_addr_t address, const in_port_t port)
    : error_(0), address_(address), port_(port) {}

SocketAddress::SocketAddress(const int address, const short port)
    : error_(0), address_(htonl(address)), port_(htons(port)) {}

SocketAddress::SocketAddress(const in_port_t port)
    : error_(0), address_(0), port_(port) {}

SocketAddress::SocketAddress(const short port)
    : error_(0), address_(0), port_(htons(port)) {}

int SocketAddress::set_address(const char *address) {
  if (UNLIKELY(address == NULL)) {
    error_ = EINVAL;
    MSS_DEBUG_ERROR("set_address", error_);
    return -1;
  }

  // We can safely convert in_addr_t to struct in_addr because it 32-bits long
  if (UNLIKELY(inet_aton(address, (struct in_addr*) &address_) == 0)) {
    DetectError();
    MSS_DEBUG_ERROR("inet_aton", error_);
    return -1;
  }

  return 0;
}

int SocketAddress::set_address(const std::string *address) {
  if (UNLIKELY(address == NULL)) {
    error_ = EINVAL;
    MSS_DEBUG_ERROR("set_address", error_);
    return -1;
  }

  return set_address(address->c_str());
}

// Getters
char *SocketAddress::GetAddressAsChar() {
  char *addressAsChar =
      (char*) malloc(sizeof(char) * INET_ADDRSTRLEN);
  if (UNLIKELY(addressAsChar == NULL)) {
    DetectError();
    MSS_DEBUG_ERROR("inet_ntop", error_);
    return NULL;
  }

  if (UNLIKELY(inet_ntop(AF_INET, &address_, addressAsChar,
                         INET_ADDRSTRLEN) == NULL)) {
    DetectError();
    MSS_DEBUG_ERROR("inet_ntop", error_);
    return NULL;
  }

  return addressAsChar;
}

std::string SocketAddress::GetAddressAsString() {
  char *addressAsChar = GetAddressAsChar();
  if (UNLIKELY(addressAsChar == NULL)) {
    MSS_DEBUG_ERROR("GetAddressAsChar",  error_);
    return NULL;
  }

  std::string convertedAddress(addressAsChar);
  free(addressAsChar);

  return convertedAddress;
}

// Out stream operators
SocketAddress& SocketAddress::operator=(const SocketAddress &address) {
  set_address(address.address_);
  set_port(address.port_);
  return *this;
}

std::ostream& operator<<(std::ostream& __cout, const SocketAddress &obj) {
  char *addressAsChar =
      (char*) malloc(sizeof(char) * INET_ADDRSTRLEN);
  if (addressAsChar == NULL) {
    __cout << "Convert error, memory allocating error ";
    return __cout;
  }

  if (inet_ntop(AF_INET, &obj.address_, addressAsChar,
                INET_ADDRSTRLEN) == NULL) {
    __cout << "Convert error, invalid string error ";
    free(addressAsChar);
    return __cout;
  }

  __cout << addressAsChar << ":" << ntohs(obj.port_);
  free(addressAsChar);
  return __cout;
}

std::ostream& operator<<(std::ostream& __cout, SocketAddress *obj) {
  __cout << obj->GetAddressAsString() << ":" << obj->GetPortAsHost();
  return __cout;
}
