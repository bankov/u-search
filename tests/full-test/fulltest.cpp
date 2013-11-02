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

#include <sys/socket.h>

#include <arpa/inet.h>

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <magic.h>
#include <syslog.h>

#include <mutex>
#include <thread>

#include <cppunit/TestAssert.h>
#include <cppunit/TestFixture.h>
#include <cppunit/ui/text/TestRunner.h>
#include <cppunit/extensions/HelperMacros.h>

#define MYSQLPP_SSQLS_NO_STATICS
#include <mysql++/mysql++.h>
#include <mysql++/ssqls.h>
#include <mysql++/result.h>
#include <mysql++/row.h>

#include "common.h"
#include "data-storage/entities.h"
#include "spider/spider.h"
#include "cppsockets/cppsockets.h"
#include "tests/cppsockets-test/cppsocketstest.h"
#include "tests/datastorage-test/datastoragetest.h"
#include "tests/spider-test/spidertest.h"

CPPUNIT_TEST_SUITE_REGISTRATION(SocketAddressTest);
CPPUNIT_TEST_SUITE_REGISTRATION(UDPSocketTest);
CPPUNIT_TEST_SUITE_REGISTRATION(TCPSocketTest);
CPPUNIT_TEST_SUITE_REGISTRATION(SpiderTest);
CPPUNIT_TEST_SUITE_REGISTRATION(AbstractSocketTest);
CPPUNIT_TEST_SUITE_REGISTRATION(FileEntryTest);
CPPUNIT_TEST_SUITE_REGISTRATION(FileAttributeTest);
CPPUNIT_TEST_SUITE_REGISTRATION(FileParameterTest);

int main() {
  // To print log message in file
  //FILE *fout = freopen("../testing.log", "w", stdout);
  //if (fout == NULL) {
  //  perror("freopen");
  //  exit(EXIT_FAILURE);
  //}

  CppUnit::TextUi::TestRunner runner;
  CppUnit::TestFactoryRegistry &registry =
      CppUnit::TestFactoryRegistry::getRegistry();
  runner.addTest( registry.makeTest() );
  runner.run();

  //fclose(fout);
  return 0;
}
