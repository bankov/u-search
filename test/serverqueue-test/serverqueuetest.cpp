#include "serverqueuetest.h"

ServerQueueTest::ServerQueueTest() : ServerQueue() {}

void ServerQueueTest::GetFromEmptyQueueTestCase() {
  CPPUNIT_ASSERT_MESSAGE("Non-empty server returned from empty queue", cmdGet().empty());
}

void ServerQueueTest::GetOnQueueWithFreeServerTestCase() {
  AddServer("foo");

  CPPUNIT_ASSERT_MESSAGE("Wrong server got from a queue with one free server", cmdGet() == "foo");
}

void ServerQueueTest::GetOnQueueWithoutFreeServerTestCase() {
  AddServer("foo");
  AddServer("bar");

  CPPUNIT_ASSERT_MESSAGE("Wrong server got from a queue with 2 free servers",  cmdGet() == "bar");
  CPPUNIT_ASSERT_MESSAGE("Wrong server got from a queue with 1 free server",  cmdGet() == "foo");
  CPPUNIT_ASSERT_MESSAGE("Wrong server got from a queue without free servers", cmdGet() == "");
}

void ServerQueueTest::ReleaseOfBusyServerTestCase() {
  AddServer("foo");
  AddServer("bar");

  CPPUNIT_ASSERT_MESSAGE("Wrong server got from a queue with 2 free servers",  cmdGet() == "bar");
  CPPUNIT_ASSERT_MESSAGE("Wrong server got from a queue with 1 free server",  cmdGet() == "foo");
  cmdRelease("bar");
  CPPUNIT_ASSERT_MESSAGE("Misbehaviour when releasing a server", get_servers_list().begin()->get_timestamp() == 0);
}

void ServerQueueTest::GetNonExistentServerTestCase() {
  AddServer("foo");

  cmdGet("bar");
  CPPUNIT_ASSERT_MESSAGE("Timestamp updated for the wrong server when trying to get non-existent server", get_servers_list().begin()->get_timestamp() == 0);
}

void ServerQueueTest::ReleaseNonExistentServerTestCase() {
  AddServer("foo");

  CPPUNIT_ASSERT_MESSAGE("Wrong server got from a queue with 1 free server",  cmdGet() == "foo");
  cmdRelease("bar");
  CPPUNIT_ASSERT_MESSAGE("Wrong behaviour when releasing a non-existent server", get_servers_list().begin()->get_timestamp() != 0);
}

void ServerQueueTest::GetAfterAllServersHaveBeenBusyAndSomeOfThemReleasedTestCase() {
  AddServer("one");
  AddServer("two");
  AddServer("three");
  AddServer("four");
 
  CPPUNIT_ASSERT_MESSAGE("Misplaced head of the queue", cmdGet() == "four");
  CPPUNIT_ASSERT_MESSAGE("Misplaced head of the queue", cmdGet() == "three");
  cmdRelease("three");
  CPPUNIT_ASSERT_MESSAGE("Misplaced head of the queue", cmdGet() == "two");
  CPPUNIT_ASSERT_MESSAGE("Misplaced head of the queue", cmdGet() == "one");
  cmdRelease("three");

  CPPUNIT_ASSERT_MESSAGE("Misplaced head of the queue", cmdGet() == "three");
}
