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

#ifndef TEST_SPIDERTEST_H_
#define TEST_SPIDERTEST_H_

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

#include <string>

#include "spider/spider.h"

class SpiderTest : public CppUnit::TestFixture, public Spider {
 public:
  SpiderTest();
  void setUp();
  void ConstructorsTestCase();
  void GetSetTestCase();
  void ReadServersListTestCase();
  void DumpToFileTestCase();
  void GetSMBDirContentTestCase();
  void ScanSMBDirTestCase();
  void NameParserTestCase();
  void AddServerTestCase();
  void DelServerTestCase();
  void AddFileEntryInDataBaseTestCase();
  void DetectMimeTypeTestCase();
  void DumpToDataBaseTestCase();

 private:
  CPPUNIT_TEST_SUITE(SpiderTest);
  CPPUNIT_TEST(ConstructorsTestCase);
  CPPUNIT_TEST(GetSetTestCase);
  CPPUNIT_TEST(ReadServersListTestCase);
  CPPUNIT_TEST(DumpToFileTestCase);
  CPPUNIT_TEST(GetSMBDirContentTestCase);
  CPPUNIT_TEST(ScanSMBDirTestCase);
  CPPUNIT_TEST(NameParserTestCase);
  CPPUNIT_TEST(AddServerTestCase);
  CPPUNIT_TEST(DelServerTestCase);
  CPPUNIT_TEST(AddFileEntryInDataBaseTestCase);
  CPPUNIT_TEST(DetectMimeTypeTestCase);
  CPPUNIT_TEST(DumpToDataBaseTestCase);
  CPPUNIT_TEST_SUITE_END();

  std::string name_;
  std::string server_;
  std::string user_;
  std::string password_;
};

#endif  // TEST_SPIDERTEST_H_
