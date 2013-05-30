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

#include "datasocket.h"

DataSocket::DataSocket() : AbstractSocket() {
}

DataSocket::DataSocket(int socket, SocketAddress &local_address,
                       SocketAddress &remote_address, SocketType type)
    : AbstractSocket() {
  set_local_address(local_address);
  set_remote_address(remote_address);
  set_socket(socket);
  set_type(type);
}

DataSocket::~DataSocket() {
  Abort();
}

void DataSocket::Flush() {
  int socket = get_socket();

  if (socket < 0) {
    set_error(EINVAL);
    MSS_DEBUG_ERROR("DataSocket::Flush", get_error());
  }

  int read_state = MAX_SIZE;
  std::vector<unsigned char> buffer;
  buffer.resize(MAX_SIZE);
  while (read_state == MAX_SIZE) {
    if (unlikely(read_state = read(socket, &buffer, MAX_SIZE) == -1)) {
      DetectError();
      MSS_DEBUG_ERROR("read", get_error());
    }
  }
}

size_t DataSocket::ReadData(void *data, size_t size) {
  if (unlikely(data == NULL || size < 1)) {
    set_error(EINVAL);
    MSS_DEBUG_ERROR("DataSocket::ReadData", get_error());
    return -1;
  }

  int readed_bytes = read(get_socket(), data, size);
  if (unlikely(readed_bytes == -1)) {
    DetectError();
    MSS_DEBUG_ERROR("read", get_error());
    return -1;
  } else if (readed_bytes == 0) {
    MSS_DEBUG_MESSAGE("0 bytes readen.");
    set_state(UnconnectedState);
    return 0;
  }

  return readed_bytes;
}

size_t DataSocket::WriteData(void *data, size_t size) {
  if (unlikely(data == NULL || size < 1)) {
    set_error(EINVAL);
    MSS_DEBUG_ERROR("DataSocket::WriteData", get_error());
    return -1;
  }

  int sended_bytes = write(get_socket(), data, size);
  if (sended_bytes == -1) {
    DetectError();
    MSS_DEBUG_ERROR("write", get_error());
    return -1;
  }

  return sended_bytes;
}
