#ifndef TESTS_SERVERQUEUETEST_H_
#define TESTS_SERVERQUEUETEST_H_

#include <cppunit/TestAssert.h>
#include <cppunit/TestFixture.h>
#include <cppunit/ui/text/TestRunner.h>
#include <cppunit/extensions/HelperMacros.h>

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <dirent.h>
#include <unistd.h>

#include <algorithm>
#include <iterator>
#include <string>
#include <list>

#include "common.h"
#include "smbtaskd/serverqueue.h"

class ServerQueueTest : public CppUnit::TestFixture, public ServerQueue {
 public:
  ServerQueueTest();
  void GetFromEmptyQueueTestCase();
  void GetOnQueueWithFreeServerTestCase();
  void GetOnQueueWithoutFreeServerTestCase();
  void ReleaseOfBusyServerTestCase();
  void GetNonExistentServerTestCase();
  void ReleaseNonExistentServerTestCase();
  void GetAfterAllServersHaveBeenBusyAndSomeOfThemReleasedTestCase();

 private:
  CPPUNIT_TEST_SUITE(ServerQueueTest);
  CPPUNIT_TEST(GetFromEmptyQueueTestCase);
  CPPUNIT_TEST(GetOnQueueWithFreeServerTestCase);
  CPPUNIT_TEST(GetOnQueueWithoutFreeServerTestCase);
  CPPUNIT_TEST(ReleaseOfBusyServerTestCase);
  CPPUNIT_TEST(GetNonExistentServerTestCase);
  CPPUNIT_TEST(ReleaseNonExistentServerTestCase);
  CPPUNIT_TEST(GetAfterAllServersHaveBeenBusyAndSomeOfThemReleasedTestCase);
  CPPUNIT_TEST_SUITE_END();
};

#endif // TESTS_SERVERQUEUETEST_H_
