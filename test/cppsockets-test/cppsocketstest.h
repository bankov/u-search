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

#ifndef TEST_CPPSOCKETSTEST_H_
#define TEST_CPPSOCKETSTEST_H_

#include <cppunit/TestAssert.h>
#include <cppunit/TestFixture.h>
#include <cppunit/ui/text/TestRunner.h>
#include <cppunit/extensions/HelperMacros.h>
#include <mutex>
#include <thread>
#include <iostream>
#include <sstream>

#include "cppsockets/tcplistener.h"
#include "cppsockets/tcpsocket.h"
#include "cppsockets/abstractsocket.h"
#include "cppsockets/socketaddress.h"
#include "cppsockets/udpsocket.h"

#define TSTMSG "Hello sockets!"
#define TSTMSGSZE 15

class AbstractSocketTest : public CppUnit::TestFixture, public AbstractSocket {
 public:
  void ConstructorsTestCase();
  void GetSetTestCase();
 private:
  CPPUNIT_TEST_SUITE(AbstractSocketTest);
  CPPUNIT_TEST(ConstructorsTestCase);
  CPPUNIT_TEST(GetSetTestCase);
  CPPUNIT_TEST_SUITE_END();
};

class SocketAddressTest : public CppUnit::TestFixture {
 public:
  void ConstructorsTestCase();
  void GetSetTestCase();
  void OperatorsTestCase();

 private:
  CPPUNIT_TEST_SUITE(SocketAddressTest);
  CPPUNIT_TEST(ConstructorsTestCase);
  CPPUNIT_TEST(GetSetTestCase);
  CPPUNIT_TEST(OperatorsTestCase);
  CPPUNIT_TEST_SUITE_END();
};

class TCPSocketTest : public CppUnit::TestFixture, public std::thread {
  public:
    void Run();
    void BasicTestCase();
  private:
    char *message;
    std::mutex mutex_;
    std::thread *listener_;
    CPPUNIT_TEST_SUITE(TCPSocketTest);
    CPPUNIT_TEST(BasicTestCase);
    CPPUNIT_TEST_SUITE_END();
};

class UDPSocketTest : public CppUnit::TestFixture {
 public:
  void BasicTestCase();
 private:
  CPPUNIT_TEST_SUITE(UDPSocketTest);
  CPPUNIT_TEST(BasicTestCase);
  CPPUNIT_TEST_SUITE_END();
};

#endif  // TEST_CPPSOCKETSTEST_H_
