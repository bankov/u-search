#include "serverqueuetest.h"

CPPUNIT_TEST_SUITE_REGISTRATION(ServerQueueTest);

int main() {
  CppUnit::TextUi::TestRunner runner;
  CppUnit::TestFactoryRegistry &registry = CppUnit::TestFactoryRegistry::getRegistry();
  runner.addTest( registry.makeTest() );
  runner.run();
  return 0;
}
