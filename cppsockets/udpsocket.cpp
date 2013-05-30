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

#include "udpsocket.h"

UDPSocket::UDPSocket() : DataSocket() {}


UDPSocket::UDPSocket(const SocketAddress *listenSocketAddress) : DataSocket() {
  set_local_address(*listenSocketAddress);
  if (unlikely(Bind(*listenSocketAddress)))
    MSS_DEBUG_MESSAGE("Bind");
}

bool UDPSocket::AllowBroadcast() {
  if (unlikely(get_socket() < 0)) {
    set_error(EBADF);
    MSS_DEBUG_ERROR("UDPSocket::AllowBroadcast", get_error());
    return false;
  }

  // Set value to true
  int optval = 1;
  if (unlikely(setsockopt(get_socket(), SOL_SOCKET, SO_BROADCAST, &optval,
                          sizeof(optval)) == -1)) {
    DetectError();
    MSS_DEBUG_ERROR("setsockopt", get_error())
    return false;
  }

  return true;
}

bool UDPSocket::RejectBroadcast() {
  if (unlikely(get_socket() < 0)) {
    set_error(EBADF);
    MSS_DEBUG_ERROR("UDPSocket::RejectBroadcast", get_error());
    return false;
  }

  // Set value to false
  int optval = 0;
  if (unlikely(setsockopt(get_socket(), SOL_SOCKET, SO_BROADCAST, &optval,
                          sizeof(optval)) == -1)) {
    DetectError();
    MSS_DEBUG_ERROR("setsockopt", get_error());
    return false;
  }

  return true;
}

int UDPSocket::WaitDatagram() {
  if (unlikely(get_socket() < 0)) {
    set_error(EBADF);
    MSS_DEBUG_ERROR("UDPSocket::WaitDatagram", get_error());
    return -1;
  }

  fd_set waitReadSet, errorSet;
  FD_ZERO(&waitReadSet);
  FD_SET(this->get_socket(), &waitReadSet);
  FD_ZERO(&errorSet);
  FD_SET(this->get_socket(), &errorSet);

  int selectStatus = select(get_socket() + 1, &waitReadSet, NULL, &errorSet,
                            NULL);
  if (unlikely(selectStatus == -1)) {
    DetectError();
    MSS_DEBUG_ERROR("select", get_error());
    return -1;
  }

  // Check is socket alert on error
  if (unlikely(FD_ISSET(this->get_socket(), &errorSet))) {
    DetectError();
    MSS_DEBUG_ERROR("FD_ISSET", get_error());
    return -1;
  }

  return selectStatus;
}

int UDPSocket::Bind(const SocketAddress &bindAddress) {
  // Inform that we connecting (binding) socket
  set_state(ConnectingState);

  set_socket(socket(AF_INET, SOCK_DGRAM, 0));
  if (unlikely(get_socket() == -1)) {
    DetectError();
    MSS_DEBUG_ERROR("socket", get_error());
    set_state(UnconnectedState);
    return -1;
  }

  this->set_local_address(bindAddress);

  struct sockaddr_in localAddr;
  memset(&localAddr, 0, sizeof(localAddr));
  localAddr.sin_family = PF_INET;
  localAddr.sin_addr.s_addr = get_local_address();
  localAddr.sin_port = get_local_port();

  if (unlikely(bind(get_socket(), (sockaddr*) &localAddr,
                    sizeof(localAddr)) == -1)) {
    DetectError();
    MSS_DEBUG_ERROR("bind", get_error());
    Close();
    return -1;
  }

  set_state(BoundState);
  return 0;
}

ssize_t UDPSocket::ReciveDatagram(void **__buf, SocketAddress *sourceAddress,
                                  ssize_t maxSize) {
  // We shouldn't cancel thread until allocated data wouldn't freed
  // We shouldn't asynchronicaly exit from thread
  int oldCancelType;
  pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, &oldCancelType);

  // Until we set cancelation action, we shouldn't exit on receive cancel signal
  int oldCancelState;
  pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &oldCancelState);

  *__buf = NULL;

  bool ignoreSize = false;
  if (unlikely(maxSize == 0)) {
    MSS_DEBUG_MESSAGE("UDPSocket::ReciveDatagram: maxSize is 0");
    maxSize = MAX_SIZE;
    ignoreSize = true;
  }

  // Define before call pthread_cleanup_push macros for return after
  ssize_t datagrammLength;
  struct iovec inputData;
  struct msghdr messageInfo;
  void *buf = malloc(maxSize);
  if (unlikely(buf == NULL)) {
    DetectError();
    MSS_DEBUG_ERROR("malloc", get_error());
  }

  memset(&messageInfo, 0, sizeof(messageInfo));
  memset(&inputData, 0, sizeof(inputData));
  memset(buf, 0, maxSize);

  inputData.iov_base = buf;
  inputData.iov_len = maxSize;

  // Add freeding action on cancel
  pthread_cleanup_push(free, buf);

  pthread_setcancelstate(oldCancelState, NULL);

  messageInfo.msg_iov = &inputData;
  messageInfo.msg_iovlen = 1;

  sockaddr_in sourceAddressAddr;
  messageInfo.msg_name = &sourceAddressAddr;
  messageInfo.msg_namelen = sizeof(sourceAddressAddr);

  // Try to recive message
  datagrammLength = recvmsg(get_socket(), &messageInfo, MSG_PEEK);
  if (unlikely(datagrammLength < 0)) {
    DetectError();
    MSS_DEBUG_ERROR("recvmsg", get_error());
    free(buf);
    return -1;
  }

  // If bufer less then message length we need to rereceive them
  if (maxSize < datagrammLength && ignoreSize) {
    void *newbuf = realloc(buf, datagrammLength);
    if (unlikely(newbuf == NULL)) {
      DetectError();
      MSS_DEBUG_ERROR("realloc", get_error());
      free(buf);
      return -1;
    }

    buf = newbuf;

    int datagrammLength = recvmsg(get_socket(), &messageInfo, 0);
    if (unlikely(datagrammLength < 0)) {
      DetectError();
      MSS_DEBUG_ERROR("recvmsg", get_error());
      free(buf);
      return -1;
    }
  } else if (maxSize < datagrammLength) {
    int tempVar = recv(get_socket(), &tempVar, sizeof(tempVar), 0);
    if (unlikely(tempVar < 0)) {
      DetectError();
      MSS_DEBUG_ERROR("recv", get_error());
      free(buf);
      return -1;
    }

    // If message greater then buffer, then bufer is full
    datagrammLength = maxSize;
  }

  if (sourceAddress != NULL) {
    sourceAddress->set_address(sourceAddressAddr.sin_addr.s_addr);
    sourceAddress->set_port(sourceAddressAddr.sin_port);
  }

  *__buf = buf;

  pthread_cleanup_pop(0);

  pthread_setcanceltype(oldCancelType, NULL);

  return datagrammLength;
}

ssize_t UDPSocket::ReciveDatagram(void **__buf, SocketAddress *sourceAddress) {
  return ReciveDatagram(__buf, sourceAddress, 0);
}

ssize_t UDPSocket::ReciveDatagram(void **__buf, ssize_t maxSize) {
  return ReciveDatagram(__buf, NULL, maxSize);
}

ssize_t UDPSocket::ReciveDatagram(void **__buf) {
  return ReciveDatagram(__buf, NULL, 0);
}

ssize_t UDPSocket::SendDatagram(void *data, SocketAddress *destinationAddres,
                                ssize_t size) {
  if (size < 1) {
    set_error(EINVAL);
    MSS_DEBUG_ERROR("SendDatagram", get_error());
    return 0;
  }

  struct sockaddr_in destinationAddr;
  memset(&destinationAddr, 0, sizeof(destinationAddr));
  destinationAddr.sin_family = PF_INET;
  destinationAddr.sin_addr.s_addr = destinationAddres->GetAddressAsNet();
  destinationAddr.sin_port = destinationAddres->GetPortAsNet();

  int sendedBytesCount = sendto(get_socket(), data, size, 0,
                                (struct sockaddr*) &destinationAddr,
                                sizeof(destinationAddr));
  if (unlikely(sendedBytesCount < 0)) {
    DetectError();
    MSS_DEBUG_ERROR("sendto", get_error());
    return -1;
  }

  return sendedBytesCount;
}
