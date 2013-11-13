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

#ifndef LIBCPPSOCKETS_ABSTRACTSOCKET_H_
#define LIBCPPSOCKETS_ABSTRACTSOCKET_H_

#include <unistd.h>
#include <errno.h>
#include <stdio.h>

#include "socketaddress.h"
#include "common-inl.h"

// The maximum size of message.
#define MAX_SIZE 2048

/**
 * @brief Class to work with abstract socket.
 */
class AbstractSocket {
 public:
  /**
   * Simple constructor which create an object and init all fields.
   */
  AbstractSocket();

  /**
   * Destructor.
   */
  virtual ~AbstractSocket();

  /**
   * @brief The NetworkLayerProtocol enum describes the network layer protocol
   * values
   */
  enum NetworkLayerProtocol {
    Other = 0,
    IPv4,
    IPv6
  };

  /**
   * @brief The SocketState enum describes the different states in which a
   * socket can be
   */
  enum SocketState {
    UnconnectedState = 0,
    HostLookupState,
    ConnectingState,
    ConnectedState,
    BoundState,
    ClosingState,
    ListeningState
  };

  /**
   * @brief The SocketType enum describes transport layer protocol.
   */
  enum SocketType {
    Raw = 0,
    Listener,
    TCP,
    UDP
  };

  /**
   * Get last occures error.
   *
   * @return Last occures error.
   */
  inline int get_error() { return error_; }

  /**
   * Get local address.
   *
   * @return Local address in network byte order.
   */
  inline in_addr_t get_local_address() {
    return local_address_.GetAddressAsNet();
  }

  /**
   * Get local port.
   *
   * @return Local port in network byte order.
   */
  inline in_port_t get_local_port() { return local_address_.GetPortAsNet(); }

  /**
   * Get remote address.
   *
   * @return Remote address in network byte order.
   */
  inline in_addr_t get_remote_address() {
    return remote_address_.GetAddressAsNet();
  }

  /**
   * Get remote port.
   *
   * @return Remote port in network byte order.
   */
  inline in_port_t get_remote_port() { return remote_address_.GetPortAsNet(); }

  /**
   * Get trasnsport layer protocol.
   *
   * @return Transport layer protocl.
   */
  inline SocketType get_type() { return type_; }

  /**
   * Get state of the socket.
   *
   * @return State of the socket.
   */
  inline SocketState get_state() { return state_; }

  /**
   * Get local address and port.
   *
   * @return Local address and port.
   */
  inline SocketAddress &get_local_socket_address() { return local_address_; }

  /**
   * Get remote address and port.
   *
   * @return Remote address and port.
   */
  inline SocketAddress &get_remote_socket_address() { return remote_address_; }

  /**
   * @brief Abort the socket connection immediately and reset socket,
   * free resources.
   */
  void Abort();

  /**
   * @brief Close socket, free all attached resources.
   *
   * @return 0 if success, -1 otherwise.
   */
  int Close();

  /**
   * @brief Disconect from host.
   */
  void Disconnect();

 protected:
  /**
   * Get socket.
   *
   * @return Socket.
   */
  inline int get_socket() { return socket_; }

  /**
   * Set a error.
   *
   * @param error Error code.
   */
  inline void set_error(int error) { error_ = error; }

  /**
   * Set local address.
   *
   * @param address Local address in network byte order.
   */
  inline virtual void set_local_address(in_addr_t address) {
    local_address_.set_address(address);
  }

  /**
   * Set local port.
   *
   * @param port Local port in network byte order.
   */
  inline virtual void set_local_port(in_port_t port) {
    local_address_.set_port(port);
  }

  /**
   * Set remote address.
   *
   * @param address Remote address in network byte order.
   */
  inline virtual void set_remote_address(in_addr_t address) {
    remote_address_.set_address(address);
  }

  /**
   * Set remote port.
   *
   * @param port Remote port in network byte order.
   */
  inline virtual void set_remote_port(in_port_t port) {
    remote_address_.set_port(port);
  }

  /**
   * Set state.
   *
   * @param state State of the socket.
   */
  inline void set_state(const SocketState state) { state_ = state; }

  /**
   * Set socket.
   *
   * @param socket Socket.
   */
  inline void set_socket(const int socket) { socket_ = socket; }

  /**
   * Set transport layer protocol.
   *
   * @param type Transport layer protocol.
   */
  inline void set_type(const SocketType type) { type_ = type; }

  /**
   * Set local address and port.
   *
   * @param address Local address and port.
   */
  virtual void set_local_address(const SocketAddress &address);

  /**
   * Set remote address and port.
   *
   * @param address Remote address and port.
   */
  virtual void set_remote_address(const SocketAddress &address);

  /**
   * Set local address and port.
   *
   * @param address Local address and port.
   */
  virtual void set_local_address(SocketAddress *address);

  /**
   * Set remote address and port.
   *
   * @param address Remote address and port.
   */
  virtual void set_remote_address(SocketAddress *address);

  /**
   * @brief Init all private variables by NULL, 0, e.t.c.
   */
  void InitAllVariables();

  /**
   * Set port and address.
   *
   * @param address Address.
   * @param port Port.
   */
  virtual void UseAddressAndPort(in_addr_t address, in_port_t port);

  /**
   * @brief Puts errno into object's error_.
   */
  inline void DetectError() { error_ = errno; }

 private:
  DISALLOW_COPY_AND_ASSIGN(AbstractSocket);

  /**
   * File descruptor associated with socket.
   */
  int socket_;

  /**
   * Variable for storage last error.
   */
  int error_;

  /**
   * Variable for storage socket type.
   */
  SocketType type_;

  /**
   * Variable for storage socket state.
   */
  SocketState state_;

  /**
   * Local address and port associated with socket.
   */
  SocketAddress local_address_;

  /**
   * Remote address and port associated with socket or
   * default destination for UDP.
   */
  SocketAddress remote_address_;
};

#endif  // LIBCPPSOCKETS_ABSTRACTSOCKET_H_
