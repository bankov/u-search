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

#ifndef TEST_SERVERQUEUETEST_H_
#define TEST_SERVERQUEUETEST_H_

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

#include "scheduler/serverqueue.h"

#define SERVERQUEUETEMPLATE "/tmp/u-search.XXXXXXXXXX"

class ServerQueueTest : public CppUnit::TestFixture, public ServerQueue {
 public:
  ServerQueueTest();
  void ReadDevNull();
  void ReadTestFile();
  void ReadNonexistingFile();
  void GetFromEmptyQueue();
  void GetOnQueueWithFreeServer();
  void GetOnQueueWithoutFreeServer();
  void ReleaseOfBusyServer();
  void GetNonExistentServer();
  void ReleaseNonExistentServer();
  void GetAfterRelease();

  void setUp();
  void tearDown();

 private:
  CPPUNIT_TEST_SUITE(ServerQueueTest);
  CPPUNIT_TEST(ReadDevNull);
  CPPUNIT_TEST(ReadTestFile);
  CPPUNIT_TEST(ReadNonexistingFile);
  CPPUNIT_TEST(GetFromEmptyQueue);
  CPPUNIT_TEST(GetOnQueueWithFreeServer);
  CPPUNIT_TEST(GetOnQueueWithoutFreeServer);
  CPPUNIT_TEST(ReleaseOfBusyServer);
  CPPUNIT_TEST(GetNonExistentServer);
  CPPUNIT_TEST(ReleaseNonExistentServer);
  CPPUNIT_TEST(GetAfterRelease);
  CPPUNIT_TEST_SUITE_END();

  char buf_[sizeof SERVERQUEUETEMPLATE];
};
#endif  // TEST_SERVERQUEUETEST_H_
