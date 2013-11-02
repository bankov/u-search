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

#include <errno.h>
#include <string.h>
#include <syslog.h>
#include <unistd.h>

#include <string>

#include "common.h"
#include "socketaddress.h"
#include "abstractsocket.h"

AbstractSocket::AbstractSocket()
    : socket_(0),
      error_(0),
      type_(Raw),
      state_(UnconnectedState),
      local_address_(),
      remote_address_() {
  openlog("cppsockets", LOG_CONS | LOG_ODELAY, LOG_USER);
}

AbstractSocket::~AbstractSocket() {
  Abort();
  closelog();
}

void AbstractSocket::InitAllVariables() {
  socket_ = 0;
  state_ = UnconnectedState;
  error_ = 0;
  local_address_.set_address(0);
  local_address_.set_port((in_port_t) 0);
  remote_address_.set_address(0);
  remote_address_.set_port((in_port_t) 0);
  type_ = Raw;
}

void AbstractSocket::set_local_address(const SocketAddress &address) {
  local_address_ = address;
}

void AbstractSocket::set_remote_address(const SocketAddress &address) {
  remote_address_ = address;
}

void AbstractSocket::set_local_address(SocketAddress *address) {
  local_address_ = *address;
}

void AbstractSocket::set_remote_address(SocketAddress *address) {
  remote_address_ = *address;
}

void AbstractSocket::UseAddressAndPort(in_addr_t address, in_port_t port) {
  set_local_address(address);
  set_local_port(port);
}

void AbstractSocket::Abort() {
  if (Close()) {
    MSS_DEBUG_ERROR("Close", error_);
  }
  InitAllVariables();
}

int AbstractSocket::Close() {
  if (socket_ > 0) {
    if (close(socket_) == -1) {
      DetectError();
      MSS_DEBUG_ERROR("close", error_);
      return -1;
    }
    state_ = UnconnectedState;
  }

  return 0;
}

void AbstractSocket::Disconnect() {
  if (state_ != UnconnectedState)
    Close();
}
