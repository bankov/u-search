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
