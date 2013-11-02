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

#include <cppunit/TestAssert.h>
#include <cppunit/TestFixture.h>
#include <cppunit/ui/text/TestRunner.h>
#include <cppunit/extensions/HelperMacros.h>

#include <string.h>
#include <syslog.h>

#include <mutex>
#include <thread>

#include "common.h"
#include "cppsockets/cppsockets.h"
#include "cppsocketstest.h"

void  AbstractSocketTest::ConstructorsTestCase() {
  AbstractSocket socket;

  CPPUNIT_ASSERT_MESSAGE("Error in error_", !socket.get_error());
  CPPUNIT_ASSERT_MESSAGE("Error in local_address_.address_",
                         !socket.get_local_address());
  CPPUNIT_ASSERT_MESSAGE("Error in local_address_.port_",
                         !socket.get_local_port());
  CPPUNIT_ASSERT_MESSAGE("Error in local_address_",
                         !socket.get_local_socket_address().GetAddressAsNet());
  CPPUNIT_ASSERT_MESSAGE("Error in local_address_",
                         !socket.get_local_socket_address().GetPortAsNet());
  CPPUNIT_ASSERT_MESSAGE("Error in remote_address_.address_",
                         !socket.get_remote_address());
  CPPUNIT_ASSERT_MESSAGE("Error in remote_address_.port_",
                         !socket.get_remote_port());
  CPPUNIT_ASSERT_MESSAGE("Error in remote_address_",
                         !socket.get_remote_socket_address().GetAddressAsNet());
  CPPUNIT_ASSERT_MESSAGE("Error in remote_address_",
                         !socket.get_remote_socket_address().GetPortAsNet());
}

void  SocketAddressTest::ConstructorsTestCase() {
  SocketAddress addr;
  CPPUNIT_ASSERT_MESSAGE("Error in error_", addr.get_error() == 0);
  CPPUNIT_ASSERT_MESSAGE("Error in port_", addr.GetPortAsNet() == 0);
  CPPUNIT_ASSERT_MESSAGE("Error in address_", addr.GetAddressAsNet() == 0);

  SocketAddress addr1("127.0.0.1");
  CPPUNIT_ASSERT_MESSAGE("Error in error_", addr1.get_error() == 0);
  CPPUNIT_ASSERT_MESSAGE("Error in port_", addr1.GetPortAsNet() == 0);
  CPPUNIT_ASSERT_MESSAGE("Error in address_",
                         addr1.GetAddressAsNet() == 0x0100007f);

  SocketAddress addr2("127.0.0.1", 22805);
  CPPUNIT_ASSERT_MESSAGE("Error in error_", addr2.get_error() == 0);
  CPPUNIT_ASSERT_MESSAGE("Error in port_", addr2.GetPortAsNet() == 0x1559);
  CPPUNIT_ASSERT_MESSAGE("Error in address_",
                         addr2.GetAddressAsNet() == 0x0100007f);

  SocketAddress addr3(addr2);
  CPPUNIT_ASSERT_MESSAGE("Error in error_", addr3.get_error() == 0);
  CPPUNIT_ASSERT_MESSAGE("Error in port_", addr3.GetPortAsNet() == 0x1559);
  CPPUNIT_ASSERT_MESSAGE("Error in address_",
                         addr3.GetAddressAsNet() == 0x0100007f);

  std::string localhost("127.0.0.1");
  SocketAddress addr4(&localhost);
  CPPUNIT_ASSERT_MESSAGE("Error in error_", addr4.get_error() == 0);
  CPPUNIT_ASSERT_MESSAGE("Error in port_", addr4.GetPortAsNet() == 0);
  CPPUNIT_ASSERT_MESSAGE("Error in address_",
                         addr4.GetAddressAsNet() == 0x0100007f);

  SocketAddress addr5(&localhost, 22805);
  CPPUNIT_ASSERT_MESSAGE("Error in error_", addr5.get_error() == 0);
  CPPUNIT_ASSERT_MESSAGE("Error in port_", addr5.GetPortAsNet() == 0x1559);
  CPPUNIT_ASSERT_MESSAGE("Error in address_",
                         addr5.GetAddressAsNet() == 0x0100007f);

  SocketAddress addr6((in_addr_t)0x0100007f);
  CPPUNIT_ASSERT_MESSAGE("Error in error_", addr6.get_error() == 0);
  CPPUNIT_ASSERT_MESSAGE("Error in port_", addr6.GetPortAsNet() == 0);
  CPPUNIT_ASSERT_MESSAGE("Error in address_",
                         addr6.GetAddressAsNet() == 0x0100007f);

  SocketAddress addr7((in_addr_t)0x0100007f, 0x5915);
  CPPUNIT_ASSERT_MESSAGE("Error in error_", addr7.get_error() == 0);
  CPPUNIT_ASSERT_MESSAGE("Error in port_", addr7.GetPortAsNet() == 0x5915);
  CPPUNIT_ASSERT_MESSAGE("Error in address_",
                         addr7.GetAddressAsNet() == 0x0100007f);

  SocketAddress addr8(0x7f000001);
  CPPUNIT_ASSERT_MESSAGE("Error in error_", addr8.get_error() == 0);
  CPPUNIT_ASSERT_MESSAGE("Error in port_", addr8.GetPortAsNet() == 0);
  CPPUNIT_ASSERT_MESSAGE("Error in address_",
                         addr8.GetAddressAsNet() == 0x0100007f);

  SocketAddress addr9(0x7f000001, 0x1559);
  CPPUNIT_ASSERT_MESSAGE("Error in error_", addr9.get_error() == 0);
  CPPUNIT_ASSERT_MESSAGE("Error in port_", addr9.GetPortAsNet() == 0x5915);
  CPPUNIT_ASSERT_MESSAGE("Error in address_",
                         addr9.GetAddressAsNet() == 0x0100007f);

  SocketAddress addr10((in_port_t)0x5915);
  CPPUNIT_ASSERT_MESSAGE("Error in error_", addr10.get_error() == 0);
  CPPUNIT_ASSERT_MESSAGE("Error in port_", addr10.GetPortAsNet() == 0x5915);
  CPPUNIT_ASSERT_MESSAGE("Error in address_", addr10.GetAddressAsNet() == 0);

  SocketAddress addr11((short)0x1559);
  CPPUNIT_ASSERT_MESSAGE("Error in error_", addr11.get_error() == 0);
  CPPUNIT_ASSERT_MESSAGE("Error in port_", addr11.GetPortAsNet() == 0x5915);
  CPPUNIT_ASSERT_MESSAGE("Error in address_", addr11.GetAddressAsNet() == 0);
}

void SocketAddressTest::GetSetTestCase() {
  SocketAddress addr;

  addr.set_address((in_addr_t)0x0100007f);
  CPPUNIT_ASSERT_MESSAGE("Error in address_",
                         addr.GetAddressAsNet() == 0x0100007f);
  CPPUNIT_ASSERT_MESSAGE("Error in address_",
                         addr.GetAddressAsHost() == 0x7f000001);
  char *address = addr.GetAddressAsChar();
  CPPUNIT_ASSERT_MESSAGE("Error in address_", !strcmp(address, "127.0.0.1"));
  free(address);
  CPPUNIT_ASSERT_MESSAGE("Error in address_",
                         !strcmp(addr.GetAddressAsString().c_str(),
                                 "127.0.0.1"));

  addr.set_address(0x7f000011);
  CPPUNIT_ASSERT_MESSAGE("Error in address_",
                         addr.GetAddressAsNet() == 0x1100007f);
  CPPUNIT_ASSERT_MESSAGE("Error in address_",
                         addr.GetAddressAsHost() == 0x7f000011);
  address = addr.GetAddressAsChar();
  CPPUNIT_ASSERT_MESSAGE("Error in address_", !strcmp(address, "127.0.0.17"));
  free(address);
  CPPUNIT_ASSERT_MESSAGE("Error in address_",
                         !strcmp(addr.GetAddressAsString().c_str(),
                                 "127.0.0.17"));

  addr.set_address("127.0.0.10");
  CPPUNIT_ASSERT_MESSAGE("Error in address_",
                         addr.GetAddressAsNet() == 0x0a00007f);
  CPPUNIT_ASSERT_MESSAGE("Error in address_",
                         addr.GetAddressAsHost() == 0x7f00000a);
  address = addr.GetAddressAsChar();
  CPPUNIT_ASSERT_MESSAGE("Error in address_", !strcmp(address, "127.0.0.10"));
  free(address);
  CPPUNIT_ASSERT_MESSAGE("Error in address_",
                         !strcmp(addr.GetAddressAsString().c_str(),
                                 "127.0.0.10"));

  std::string localhost("127.0.0.1");
  addr.set_address(&localhost);
  CPPUNIT_ASSERT_MESSAGE("Error in address_",
                         addr.GetAddressAsNet() == 0x0100007f);
  CPPUNIT_ASSERT_MESSAGE("Error in address_",
                         addr.GetAddressAsHost() == 0x7f000001);
  address = addr.GetAddressAsChar();
  CPPUNIT_ASSERT_MESSAGE("Error in address_", !strcmp(address, "127.0.0.1"));
  free(address);
  CPPUNIT_ASSERT_MESSAGE("Error in address_",
                         !strcmp(addr.GetAddressAsString().c_str(),
                                 "127.0.0.1"));

  addr.set_port((short)0x1559);
  CPPUNIT_ASSERT_MESSAGE("Error in port_", addr.GetPortAsHost() == 0x1559);
  CPPUNIT_ASSERT_MESSAGE("Error in port_", addr.GetPortAsNet() == 0x5915);

  addr.set_port((in_port_t)0x1234);
  CPPUNIT_ASSERT_MESSAGE("Error in port_", addr.GetPortAsHost() == 0x3412);
  CPPUNIT_ASSERT_MESSAGE("Error in port_", addr.GetPortAsNet() == 0x1234);
}

void SocketAddressTest::OperatorsTestCase() {
  SocketAddress addr("127.0.0.1", 22805);
  CPPUNIT_ASSERT_MESSAGE("Error in error_", addr.get_error() == 0);
  CPPUNIT_ASSERT_MESSAGE("Error in port_", addr.GetPortAsNet() == 0x1559);
  CPPUNIT_ASSERT_MESSAGE("Error in address_",
                         addr.GetAddressAsNet() == 0x0100007f);

  SocketAddress test_addr;
  test_addr = addr;
  CPPUNIT_ASSERT_MESSAGE("Error in error_", test_addr.get_error() == 0);
  CPPUNIT_ASSERT_MESSAGE("Error in port_", test_addr.GetPortAsNet() == 0x1559);
  CPPUNIT_ASSERT_MESSAGE("Error in address_",
                         test_addr.GetAddressAsNet() == 0x0100007f);

  // Testing output from iostreams operators
  std::stringstream oss;
  std::streambuf* old = std::cout.rdbuf();
  std::cout.rdbuf(oss.rdbuf());

  std::cout << addr;
  std::cout.rdbuf(old);
  CPPUNIT_ASSERT_MESSAGE("Bad output in std::cout << addr;",
                         !strcmp(oss.str().c_str(), "127.0.0.1:22805"));

  std::cout.rdbuf(oss.rdbuf());
  oss.str("");  // Clear previous content from stream.
  std::cout << &addr;
  std::cout.rdbuf(old);
  CPPUNIT_ASSERT_MESSAGE("Bad output in std::cout << &addr;",
                         !strcmp(oss.str().c_str(), "127.0.0.1:22805"));
}

void TCPSocketTest::Run() {
  SocketAddress address("127.0.0.1", 25555);
  TCPListener *genericListener = new(std::nothrow) TCPListener(&address, 10);
  CPPUNIT_ASSERT(genericListener);

  mutex_.unlock();
  DataSocket *testClient = genericListener->Accept();
  CPPUNIT_ASSERT(testClient);
  char *message = (char*) malloc(TSTMSGSZE);
  CPPUNIT_ASSERT(message);
  int recived = testClient->ReadData(message, TSTMSGSZE);

  CPPUNIT_ASSERT_MESSAGE("Wrong recived data, listener", recived == TSTMSGSZE);
  CPPUNIT_ASSERT_MESSAGE("Wrong recived data, listener",
                         !strcmp(message, TSTMSG));

  int sended = testClient->WriteData(message, TSTMSGSZE);

  CPPUNIT_ASSERT_MESSAGE("Wrong sending data, listener", sended == TSTMSGSZE);

  delete testClient;

  testClient = genericListener->Accept();
  CPPUNIT_ASSERT_MESSAGE("Accept failed", testClient);

  CPPUNIT_ASSERT_MESSAGE("Wrong recived data, listener 2",
                         recived == TSTMSGSZE);
  CPPUNIT_ASSERT_MESSAGE("Wrong recived data, listener 2",
                         !strcmp(message, TSTMSG));

  sended = testClient->WriteData(message, TSTMSGSZE);

  CPPUNIT_ASSERT_MESSAGE("Wrong sending data, listener 2", sended == TSTMSGSZE);

  delete testClient;
  delete genericListener;
  free(message);
}

void TCPSocketTest::BasicTestCase() {
  CPPUNIT_ASSERT(message = (char*) malloc(sizeof(char) * TSTMSGSZE));

  // Made etalon string
  strcpy(message, TSTMSG);

  // Creating thread for listener
  mutex_.lock();
  //listener_->join();
  listener_ = new(std::nothrow) std::thread(&TCPSocketTest::Run, this);
  CPPUNIT_ASSERT(listener_);
  mutex_.lock();

  // Simple constructor and destructor test
  TCPSocket *testedSocket = new(std::nothrow) TCPSocket();
  CPPUNIT_ASSERT(testedSocket);
  delete testedSocket;

  // Simple test for other constructor
  SocketAddress *lAddr = new(std::nothrow) SocketAddress("127.0.0.1", 30777);
  CPPUNIT_ASSERT(lAddr);
  testedSocket = new(std::nothrow) TCPSocket(lAddr);
  CPPUNIT_ASSERT(testedSocket);

  delete lAddr;
  delete testedSocket;

  // Test for constructor with connection and sending data
  lAddr = new(std::nothrow) SocketAddress("127.0.0.1", 30777);
  CPPUNIT_ASSERT(lAddr);
  SocketAddress *rAddr = new(std::nothrow) SocketAddress("127.0.0.1", 25555);
  CPPUNIT_ASSERT(rAddr);
  testedSocket = new(std::nothrow) TCPSocket(lAddr, rAddr);
  CPPUNIT_ASSERT(testedSocket);

  char *recv = (char*) malloc(TSTMSGSZE);
  CPPUNIT_ASSERT(recv);

  int writedBytes = testedSocket->WriteInSocket(message, TSTMSGSZE);
  int readedBytes = testedSocket->ReadFromSocket(recv, TSTMSGSZE);

  CPPUNIT_ASSERT_MESSAGE("Error on sending, test for constructor",
                         writedBytes != -1);
  CPPUNIT_ASSERT_MESSAGE("Error on recieving, test for constructor",
                         readedBytes != -1);
  CPPUNIT_ASSERT_MESSAGE("Wrong recived data, test for constructor",
                         !strcmp(recv, TSTMSG));

  delete lAddr;
  delete rAddr;
  delete testedSocket;

  // Creating socket and try to connect
  lAddr = new(std::nothrow) SocketAddress("127.0.0.1", 30778);
  CPPUNIT_ASSERT(lAddr);
  testedSocket = new(std::nothrow) TCPSocket(lAddr);
  CPPUNIT_ASSERT(testedSocket);
  int result = testedSocket->ConnectToHost("127.0.0.1", 25555);
  CPPUNIT_ASSERT_MESSAGE("Error in connect to host", !result);

  writedBytes = testedSocket->WriteInSocket(message, TSTMSGSZE);
  readedBytes = testedSocket->ReadFromSocket(recv, TSTMSGSZE);

  CPPUNIT_ASSERT_MESSAGE("Error on sending, try to connect", writedBytes != -1);
  CPPUNIT_ASSERT_MESSAGE("Error on recieving, try to connect",
                         readedBytes != -1);
  CPPUNIT_ASSERT_MESSAGE("Wrong recived data, test for constructor",
                         !strcmp(recv, TSTMSG));

  listener_->join();

  free(message);
  free(recv);

  delete lAddr;
  delete testedSocket;
  delete listener_;
}

void UDPSocketTest::BasicTestCase() {
  // Simple Constructor and destructor test
  UDPSocket *testedSocket = new(std::nothrow) UDPSocket();
  CPPUNIT_ASSERT(testedSocket);
  delete testedSocket;

  // Simple creation and deleting test for SocketAddress and UDPSocket
  SocketAddress *localhostAddress =
      new(std::nothrow) SocketAddress("127.0.0.1", 35678);
  CPPUNIT_ASSERT(localhostAddress);
  testedSocket = new(std::nothrow) UDPSocket(localhostAddress);
  CPPUNIT_ASSERT(testedSocket);
  delete localhostAddress;
  delete testedSocket;

  // Recive method testing
  localhostAddress = new(std::nothrow) SocketAddress("127.0.0.1", 35678);
  CPPUNIT_ASSERT(localhostAddress);
  SocketAddress *localhostAddress2 =
      new(std::nothrow) SocketAddress("127.0.0.1", 35555);
  CPPUNIT_ASSERT(localhostAddress2);
  testedSocket = new(std::nothrow) UDPSocket(localhostAddress);
  CPPUNIT_ASSERT(testedSocket);
  UDPSocket *secondSocket = new(std::nothrow) UDPSocket(localhostAddress2);
  CPPUNIT_ASSERT(secondSocket);

  // Send one
  void *buf = malloc(sizeof(char) * 25);
  CPPUNIT_ASSERT(buf);
  memset(buf, 0, 25);
  const char *message = "Hello sockets world!";
  strcpy((char*) buf, message);

  testedSocket->SendDatagram(buf, localhostAddress2, 25);
  free(buf);

  secondSocket->ReceiveDatagram(&buf);
  CPPUNIT_ASSERT_MESSAGE("Recived and sended not same",
                         !strcmp((char*) buf, message));
  free(buf);

  // Send two
  buf = malloc(sizeof(char) * 25);
  CPPUNIT_ASSERT(buf);
  memset(buf, 0, 25);
  strcpy((char*) buf, message);
  testedSocket->SendDatagram(buf, localhostAddress, 25);
  free(buf);

  testedSocket->ReceiveDatagram(&buf);

  delete testedSocket;
  delete localhostAddress;
  delete localhostAddress2;
  delete secondSocket;

  CPPUNIT_ASSERT_MESSAGE("Recived and sended not same",
                         !strcmp((char*) buf, message));
  free(buf);
}
