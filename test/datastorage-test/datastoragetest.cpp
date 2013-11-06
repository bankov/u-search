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

#include "common.h"
#include "datastoragetest.h"

void FileEntryTest::setUp() {
  CPPUNIT_ASSERT_MESSAGE("Error in reading configuration files",
                         read_database_config(&name_, &server_, &user_,
                                              &password_,
                                              "../" DATABASE_CONFIG) == 0);
}

void FileEntryTest::GetByPathOnServerTestCase() {
  std::string name("test file");
  std::string path("path/to/test_file");
  std::string server("test.server");

  CPPUNIT_ASSERT_MESSAGE("Connect to data base",
                         DatabaseEntity::ConnectToServer(name_, server_, user_,
                                                         password_, false));

  struct timeval time;
  gettimeofday(&time, NULL);

  FileEntry(name, path, server);

  std::shared_ptr<FileEntry> db_file = FileEntry::GetByPathOnServer(path,
                                                                    server);
  CPPUNIT_ASSERT_MESSAGE("Error in GetByPathOnServer", db_file);
  CPPUNIT_ASSERT_MESSAGE("Error in name", db_file->get_name() == name);
  CPPUNIT_ASSERT_MESSAGE("Error in server",
                         db_file->get_server_name() == server);
  CPPUNIT_ASSERT_MESSAGE("Error in path", db_file->get_file_path() == path);
  CPPUNIT_ASSERT_MESSAGE("Error in timestamp",
                         db_file->get_timestamp() >= time.tv_sec);

  // Try to add russian file.
  name = ("русский файл");
  path = ("path/to/русский_файл");

  gettimeofday(&time, NULL);

  FileEntry(name, path, server);

  db_file = FileEntry::GetByPathOnServer(path, server);
  CPPUNIT_ASSERT_MESSAGE("Error in GetByPathOnServer", db_file);
  CPPUNIT_ASSERT_MESSAGE("Error in name", db_file->get_name() == name);
  CPPUNIT_ASSERT_MESSAGE("Error in server",
                         db_file->get_server_name() == server);
  CPPUNIT_ASSERT_MESSAGE("Error in path", db_file->get_file_path() == path);
  CPPUNIT_ASSERT_MESSAGE("Error in timestamp",
                         db_file->get_timestamp() >= time.tv_sec);
}

void FileAttributeTest::setUp() {
  CPPUNIT_ASSERT_MESSAGE("Error in reading configuration files",
                         read_database_config(&name_, &server_, &user_,
                                              &password_,
                                              "../" DATABASE_CONFIG) == 0);
}

void FileAttributeTest::ConstructorsTestCase() {
  CPPUNIT_ASSERT_MESSAGE("Connect to data base",
                         DatabaseEntity::ConnectToServer(name_, server_, user_,
                                                         password_, false));

  FileAttribute attr("test-attr", FileAttribute::faString);
  CPPUNIT_ASSERT_MESSAGE("Attribute type",
                         attr.get_type() == FileAttribute::faString);
  CPPUNIT_ASSERT_MESSAGE("Attribute name", attr.get_name() == "test-attr");
  CPPUNIT_ASSERT_MESSAGE("FileAttribute",
                         FileAttribute::GetByNameAndType(
                             "test-attr", FileAttribute::faString));
}

void FileParameterTest::setUp() {
  CPPUNIT_ASSERT_MESSAGE("Error in reading configuration files",
                         read_database_config(&name_, &server_, &user_,
                                              &password_,
                                              "../" DATABASE_CONFIG) == 0);
}

void FileParameterTest::ConstructorsTestCase() {
  CPPUNIT_ASSERT_MESSAGE("Connect to data base",
                         DatabaseEntity::ConnectToServer(name_, server_, user_,
                                                         password_, false));

  // Create an attribute
  FileAttribute attr("test-attr", FileAttribute::faString);
  CPPUNIT_ASSERT_MESSAGE("Attribute type",
                         attr.get_type() == FileAttribute::faString);
  CPPUNIT_ASSERT_MESSAGE("Attribute name", attr.get_name() == "test-attr");
  CPPUNIT_ASSERT_MESSAGE("FileAttribute",
                         FileAttribute::GetByNameAndType(
                             "test-attr", FileAttribute::faString));

  // Create an entry
  std::string name("test file");
  std::string path("path/to/test_file");
  std::string server("test.server");
  FileEntry entry(name, path, server);

  // Create a parameter for attribute and entry
  FileParameter(entry, attr, "test-param", 0, true);
  auto param = FileParameter::GetByFileAndAttribute(entry, attr);
  CPPUNIT_ASSERT_MESSAGE("FileParameter", param);
  CPPUNIT_ASSERT_MESSAGE("Wrong number of parameters", param->size() == 1);
}
