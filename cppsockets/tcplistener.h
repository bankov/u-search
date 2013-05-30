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

#ifndef LIBCPPSOCKETS_TCPLISTENER_H_
#define LIBCPPSOCKETS_TCPLISTENER_H_

#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>

#include "abstractsocket.h"
#include "datasocket.h"
#include "socketaddress.h"

/**
 * @brief The TCPListener class allows you to listen TCP connections.
 */
class TCPListener : public AbstractSocket {
 public:
  /**
   * Standart constructor. Create an empthy object.
   */
  TCPListener();

  /**
   * @brief TCPListener create an object with specified address and port.
   *
   * @param newLocalAddress instance of class SocketAddress which contain
   * address and port to be listened.
   * @param backlog provide possibility to limit the number of outstanding
   * connections in the socket's listen queue. Implementations may impose a
   * limit on backlog and silently reduce the specified value. Normally, a
   * larger backlog argument value shall result in a larger or equal length of
   * the listen queue. Defined in <sys/socket.h>.
   * If  TCPListener() is called with a backlog argument value that is less
   * than 0, the function behaves as if it had been called with a backlog
   * argument value of 0.
   */
  TCPListener(SocketAddress *local_address, int backlog);

  /**
   * Destructor.
   */
  ~TCPListener();

  /**
   * @brief AcceptNoWait accept a new connection on a socket. If connection is
   * not set AcceptNoWait() shall not block.
   *
   * @return instance of class DataSocket.
   */
  DataSocket *AcceptNoWait();

  /**
   * @brief Accept accept a new connection on a socket. If connection is not
   * set Accept() shall block until a connection is present.
   *
   * @return accepted instance of class DataSocket.
   */
  DataSocket *Accept();

 protected:
  /**
   * Provide possibility to limit the number of outstanding connections in the
   * socket's listen queue. Implementations may impose a limit on backlog and
   * silently reduce the specified value. Normally, a larger backlog argument
   * value shall result in a larger or equal length of the listen queue.
   * Defined in <sys/socket.h>.
   */
  int backlog_;
};

#endif  // LIBCPPSOCKETS_TCPLISTENER_H_
