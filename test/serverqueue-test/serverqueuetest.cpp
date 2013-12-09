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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <cppunit/TestAssert.h>

#include "scheduler/serverqueue.h"
#include "serverqueuetest.h"

ServerQueueTest::ServerQueueTest() : ServerQueue() {}

void ServerQueueTest::setUp() {
  memcpy(buf_, SERVERQUEUETEMPLATE, sizeof buf_);

  int fd = mkstemp(buf_);
  CPPUNIT_ASSERT(fd != -1);

  FILE *fp = fdopen(fd, "w+");
  CPPUNIT_ASSERT(fp != NULL);

  CPPUNIT_ASSERT(fputs("test1\n", fp) > 0);
  CPPUNIT_ASSERT(fputs("test2\n", fp) > 0);
  CPPUNIT_ASSERT(fclose(fp) == 0);
}

void ServerQueueTest::tearDown() {
  unlink(buf_);
}

void ServerQueueTest::ReadDevNull() {
  ReadServersList("/dev/null");
  CPPUNIT_ASSERT_MESSAGE("Non-empty server returned from /dev/null",
                         CmdGet().empty());
}

void ServerQueueTest::ReadTestFile() {
  ReadServersList(buf_);
  CPPUNIT_ASSERT(CmdGet() == "test2");
  CPPUNIT_ASSERT(CmdGet() == "test1");
  CPPUNIT_ASSERT(CmdGet().empty());
}

void ServerQueueTest::ReadNonexistingFile() {
  ReadServersList("/zzz");
  CPPUNIT_ASSERT_MESSAGE("Non-empty server returned from /zzz",
                         CmdGet().empty());
}

void ServerQueueTest::GetFromEmptyQueue() {
  CPPUNIT_ASSERT_MESSAGE("Non-empty server returned from empty queue",
                         CmdGet().empty());
}

void ServerQueueTest::GetOnQueueWithFreeServer() {
  AddServer("foo");

  CPPUNIT_ASSERT_MESSAGE("Wrong server got from a queue with one free server",
                         CmdGet() == "foo");
}

void ServerQueueTest::GetOnQueueWithoutFreeServer() {
  AddServer("foo");
  AddServer("bar");

  CPPUNIT_ASSERT_MESSAGE("Wrong server got from a queue with 2 free servers",
                         CmdGet() == "bar");
  CPPUNIT_ASSERT_MESSAGE("Wrong server got from a queue with 1 free server",
                         CmdGet() == "foo");
  CPPUNIT_ASSERT_MESSAGE("Wrong server got from a queue without free servers",
                         CmdGet() == "");
}

void ServerQueueTest::ReleaseOfBusyServer() {
  AddServer("foo");
  AddServer("bar");

  CPPUNIT_ASSERT_MESSAGE("Wrong server got from a queue with 2 free servers",
                         CmdGet() == "bar");
  CPPUNIT_ASSERT_MESSAGE("Wrong server got from a queue with 1 free server",
                         CmdGet() == "foo");
  CmdRelease("bar");
  CPPUNIT_ASSERT_MESSAGE("Misbehaviour when releasing a server",
                         servers_list_->begin()->get_timestamp() == 0);
}

void ServerQueueTest::GetNonExistentServer() {
  AddServer("foo");

  CmdGet("bar");
  CPPUNIT_ASSERT_MESSAGE("Timestamp updated for the wrong server when trying"
                         " to get non-existent server",
                         servers_list_->begin()->get_timestamp() == 0);
}

void ServerQueueTest::ReleaseNonExistentServer() {
  AddServer("foo");

  CPPUNIT_ASSERT_MESSAGE("Wrong server got from a queue with 1 free server",
                         CmdGet() == "foo");
  CmdRelease("bar");
  CPPUNIT_ASSERT_MESSAGE("Wrong behaviour when releasing a non-existent server",
                          servers_list_->begin()->get_timestamp() != 0);
}

void ServerQueueTest::GetAfterRelease() {
  AddServer("one");
  AddServer("two");
  AddServer("three");
  AddServer("four");

  CPPUNIT_ASSERT_MESSAGE("Misplaced head of the queue", CmdGet() == "four");
  CPPUNIT_ASSERT_MESSAGE("Misplaced head of the queue", CmdGet() == "three");
  CmdRelease("three");
  CPPUNIT_ASSERT_MESSAGE("Misplaced head of the queue", CmdGet() == "two");
  CPPUNIT_ASSERT_MESSAGE("Misplaced head of the queue", CmdGet() == "one");
  CmdRelease("three");

  CPPUNIT_ASSERT_MESSAGE("Misplaced head of the queue", CmdGet() == "three");
}
