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

#define EXPAND_MY_SSQLS_STATICS
#include "entities.h"

mysqlpp::TCPConnection DatabaseEntity::db_connection_;
std::string DatabaseEntity::db_error_;
std::shared_ptr<mysqlpp::Transaction> DatabaseEntity::current_transaction_;

mysqlpp::TCPConnection & DatabaseEntity::get_db_connection() {
  if (db_connection_.connected())
    return db_connection_;
  throw mysqlpp::ConnectionFailed();
}

bool DatabaseEntity::ConnectToServer(const std::string &db_name,
                                     const std::string &server,
                                     const std::string &user,
                                     const std::string &password,
                                     const bool reconnect) {
  if (db_connection_.connected() && !reconnect)
    return true;

  try {
    db_connection_ = mysqlpp::TCPConnection(server.c_str(), db_name.c_str(),
                                            user.c_str(), password.c_str());
    // We need in utf-8 encoding support
    db_connection_.query("SET CHARSET UTF8").execute();
    return true;
  } catch(const mysqlpp::Exception &exception) {
    db_error_= std::string(exception.what());
    return false;
  }
}

bool DatabaseEntity::Disconnect() {
  if (!db_connection_.connected())
    return true;

  try {
    db_connection_.disconnect();
    return true;
  } catch(const mysqlpp::Exception &exception) {
    db_error_ = std::string(exception.what());
    return false;
  }
}

bool DatabaseEntity::StartTransaction() {
  if (current_transaction_ != nullptr)
    return true;
  try {
    current_transaction_ =
        std::shared_ptr<mysqlpp::Transaction>(
            new mysqlpp::Transaction(get_db_connection()));
    return true;
  } catch(const mysqlpp::Exception &e) {
    db_error_ = e.what();
    return false;
  } catch(const std::bad_alloc &e) {
    db_error_ = e.what();
    return false;
  }
}

bool DatabaseEntity::CommitTransaction() {
  if (current_transaction_ == nullptr) {
    MSS_DEBUG_MESSAGE("Attemp detecting to commit not started transaction");
    return true;
  }

  try {
    current_transaction_->commit();
    current_transaction_.reset();
    return true;
  } catch(const mysqlpp::Exception &e) {
    db_error_ = e.what();
    return false;
  }
}

bool DatabaseEntity::RollbackTransaction() {
  if (current_transaction_ == nullptr) {
    MSS_DEBUG_MESSAGE("Attemp detecting to rollback not started transaction");
    return true;
  }

  try {
    current_transaction_->rollback();
    current_transaction_.reset();
    return true;
  } catch(const mysqlpp::Exception &e) {
    db_error_ = e.what();
    return false;
  }
}

FileAttribute::FileAttribute(const std::string &name,
                             const AttributeType type) {
  try {
    mysqlpp::Query insert_query = get_db_connection().query();
    mss_attributes row(0, name, AttrTypeToString(type));

    insert_query.replace(row);
    insert_query.execute();

    id_ = insert_query.insert_id();
    row.id = id_;
    name_ = row.name;
    type_ = type;
    orig_row_ = row;
  } catch(const mysqlpp::Exception &e) {
    db_error_ = e.what();
    throw e;
  }
}

FileAttribute::FileAttribute(const mss_attributes &orig_row) {
  id_ = orig_row.id;
  name_ = orig_row.name;
  type_ = StringToAttrType(orig_row.type);
  orig_row_ = orig_row;
}

std::shared_ptr<FileAttribute> FileAttribute::GetById(const int id) {
  try {
    mysqlpp::Query query = get_db_connection().query("select * from "
                                                "mss_attributes attr where "
                                                "attr.id = %0:id");
    query.parse();
    mss_attributes row(query.store(id)[0]);

    return std::shared_ptr<FileAttribute>(new FileAttribute(row));
  } catch(const mysqlpp::Exception &e) {
    db_error_ = e.what();
    return nullptr;
  } catch(const std::bad_alloc &e) {
    db_error_ = e.what();
    return nullptr;
  }
}

std::shared_ptr<FileAttribute> FileAttribute::GetByName(
    const std::string &name) {
  try {
    mysqlpp::Query query = get_db_connection().query("select * from "
                                                "mss_attributes attr where "
                                                "attr.name like %0q:name");
    query.parse();
    mss_attributes row(query.store(name)[0]);

    return std::shared_ptr<FileAttribute>(new FileAttribute(row));
  } catch(const mysqlpp::Exception &e) {
    db_error_ = e.what();
    return nullptr;
  } catch(const std::bad_alloc &e) {
    db_error_ = e.what();
    return nullptr;
  }
}

std::shared_ptr<FileAttribute> FileAttribute::GetByNameAndType(
        const std::string &name, const AttributeType type) {
  try {
    mysqlpp::Query query = get_db_connection().query("select * from "
                                                "mss_attributes attr where "
                                                "attr.name like %0q:name "
                                                "and attr.type = %1q:type");

    query.parse();
    mysqlpp::StoreQueryResult findedRows = 
      query.store(name, AttrTypeToString(type));
    if (findedRows.num_rows() < 1)
      return nullptr;
    mss_attributes row(findedRows[0]);

    return std::shared_ptr<FileAttribute>(new FileAttribute(row));
  } catch(const mysqlpp::Exception &e) {
    db_error_ = e.what();
    return nullptr;
  } catch(const std::bad_alloc &e) {
    db_error_ = e.what();
    return nullptr;
  }
}

std::string FileAttribute::AttrTypeToString(const AttributeType type) {
  std::string result;
  switch (type) {
    case faString:
      result = "str";
      break;
    case faNum:
      result = "num";
      break;
    case faBool:
      result = "bool";
    default:
      result ="";
      break;
  }
  return result;
}

FileAttribute::AttributeType FileAttribute::StringToAttrType(
    const std::string &string) {
  if (string == "str")
    return faString;
  if (string == "num")
    return faNum;
  if (string == "bool")
    return faBool;
  return faUnknown;
}

FileEntry::FileEntry(const mss_files &orig_row)
  : id_(orig_row.id),
    name_(orig_row.name),
    file_path_(orig_row.file_path),
    server_name_(orig_row.server_name),
    timestamp_(orig_row.last_seen),
    orig_row_(orig_row) {
}

FileEntry::FileEntry(const int id, const std::string &name,
                     const std::string &file_path,
                     const std::string &server_name,
                     const time_t timestamp, const mss_files &orig_row)
  : id_(id),
    name_(name),
    file_path_(file_path),
    server_name_(server_name),
    timestamp_(timestamp),
    orig_row_(orig_row) {
}

FileEntry::FileEntry(const std::string &file_name, const std::string &file_path,
                     const std::string &server_name) {
  try {
    mysqlpp::Query insert_query = get_db_connection().query();
    struct timeval current_time;
    gettimeofday(&current_time, NULL);

    mss_files row(0, file_name, file_path, server_name);

    insert_query.replace(row);
    insert_query.execute();

    id_ = insert_query.insert_id();
    row.id = id_;

    name_ = file_name;
    file_path_ = file_path;
    server_name_ = server_name;
    timestamp_ = current_time.tv_sec;
    orig_row_ = row;
  } catch(const mysqlpp::Exception &e) {
    db_error_ = std::string(e.what());
  }
}

std::vector<std::shared_ptr<FileEntry> > *FileEntry::FindByName(
    const std::string &name, const int min_rownum, const int max_rownum) {
  mysqlpp::StoreQueryResult search_result;

  try {
    // Create query for searching row with specifed name and beetwen
    std::string query_text = "select * from mss_files files where "
        "files.id like %0%q:name:%\n";
    if (min_rownum > 0)
      query_text.append("and rownum > %1:min_rownum \n");
    if (max_rownum > 0)
      query_text.append("and max_rownum < %2:max_rownum");

    mysqlpp::Query search_query = get_db_connection().query(query_text.c_str());
    search_query.parse();

    search_result = search_query.store(name.c_str(), min_rownum, max_rownum);
  } catch(const mysqlpp::Exception &e) {
    db_error_ = std::string(e.what());
    return NULL;
  }

  return QueryResultToVector(search_result);
}

std::vector<std::shared_ptr<FileEntry> > *FileEntry::GetByName(
    const std::string &name, const int min_rownum, const int max_rownum) {
  mysqlpp::StoreQueryResult search_result;

  try {
    // Create query for searching row with specifed name and beetwen
    std::string query_text = "select * from mss_files files where "
        "files.id like %0q:name:\n";
    if (min_rownum > 0)
      query_text.append("and rownum > %1:min_rownum \n");
    if (max_rownum > 0)
      query_text.append("and max_rownum < %2:max_rownum");

    mysqlpp::Query search_query = get_db_connection().query(query_text.c_str());
    search_query.parse();

    search_result = search_query.store(name.c_str(), min_rownum, max_rownum);
  } catch(const mysqlpp::Exception &e) {
    db_error_ = std::string(e.what());
    return NULL;
  }

  return QueryResultToVector(search_result);
}

std::vector<std::shared_ptr<FileEntry> > *FileEntry::GetByServer(
    const std::string &server_name, const int min_rownum,
    const int max_rownum) {
  mysqlpp::StoreQueryResult search_result;

  try {
    // Create query for searching row with specifed name and beetwen
    std::string query_text = "select * from mss_files files where "
        "files.server_name like %0q:name:\n";
    if (min_rownum > 0)
      query_text.append("and rownum > %1:min_rownum \n");
    if (max_rownum > 0)
      query_text.append("and max_rownum < %2:max_rownum");

    mysqlpp::Query search_query = get_db_connection().query(query_text.c_str());
    search_query.parse();

    search_result = search_query.store(server_name.c_str(), min_rownum,
                                       max_rownum);
  } catch(const mysqlpp::Exception &e) {
    db_error_ = std::string(e.what());
    return NULL;
  }

  return QueryResultToVector(search_result);
}

std::shared_ptr<FileEntry> FileEntry::GetByPathOnServer(
    const std::string &path, const std::string &server) {
  mysqlpp::StoreQueryResult result;

  try {
    std::string query_text = "select * from mss_files files "
        "where files.file_path = %0q:path and files.server_name = %1q:server";
    mysqlpp::Query query = get_db_connection().query(query_text.c_str());
    query.parse();

    result = query.store(path, server);
  } catch(const mysqlpp::Exception &e) {
    db_error_ = e.what();
    MSS_DEBUG_MESSAGE(e.what());
  }

  // On Success
  if (result.size() == 1) {
    try {
      mss_files result_row(result[0]);
      return std::shared_ptr<FileEntry>(new FileEntry(result_row));
    } catch(const std::bad_alloc &e) {
      db_error_ = std::string(e.what());
    }
  }

  // On error
  if (result != 0)
    db_error_ = std::string("More then one row finded, this is db error");

  return nullptr;
}

std::vector<std::shared_ptr<FileEntry> > *FileEntry::QueryResultToVector(
    mysqlpp::StoreQueryResult &result) {
  // Final query result
  auto final_result =
      new(std::nothrow) std::vector<std::shared_ptr<FileEntry>>(result.size());
  if (final_result == NULL)  {
    db_error_ = std::string("Error while allocating memory");
    return NULL;
  }

  auto insert_iterator = final_result->begin();
  for (mysqlpp::Row &row : result) {
    mss_files typed_row(row);

    try {
      *insert_iterator = std::shared_ptr<FileEntry>(new FileEntry(typed_row));
    } catch(const std::bad_alloc &e) {
      db_error_ = std::string(e.what());
      delete final_result;
      return NULL;
    }

    insert_iterator++;
  }

  return final_result;
}

std::shared_ptr<FileEntry> FileEntry::GetById(const int id) {
  mysqlpp::Query query =
      get_db_connection().query("select * from mss_files files "
                                "where files.id = %0:id");
  query.parse();

  mss_files only_row;
  try {
    mysqlpp::StoreQueryResult query_result = query.store(id);
    if (query_result.num_rows() != 1) {
      if (query_result.num_rows() == 0)
        return nullptr;
      db_error_ = std::string("Query return more than one row, "
                              "this is db error");
      return nullptr;
    }
    only_row = query_result[0];
  } catch(const mysqlpp::Exception &e) {
    db_error_ = std::string(e.what());
    return nullptr;
  }

  return std::shared_ptr<FileEntry> (new FileEntry(only_row));
}

FileParameter::FileParameter(const mss_parameters &orig_row)
  : str_value_(orig_row.str_value),
    num_value_(orig_row.num_value),
    bool_value_(orig_row.bool_value) {
  try {
    attr_ = FileAttribute::GetById(orig_row.attr_id);
    if (attr_ == nullptr)
      throw std::runtime_error("Error while get attribute with id: " +
                               std::to_string(orig_row.attr_id));

    file_ = FileEntry::GetById(orig_row.file_id);
    if (file_ == nullptr)
      throw std::runtime_error("Error while get file with id: " +
                               std::to_string(orig_row.file_id));
    orig_row_ = orig_row;
  } catch(const mysqlpp::Exception &e) {
    db_error_ = e.what();
  } catch(const std::bad_alloc &e) {
    db_error_ = e.what();
  }
}

FileParameter::FileParameter(const mysqlpp::Row &orig_row)
  : FileParameter(mss_parameters(orig_row)) {
}

FileParameter::FileParameter(const FileEntry &file,
                             const FileAttribute &attribute,
                             const std::string &str_value, const int num_value,
                             const bool bool_value)
  : str_value_(str_value),
    num_value_(num_value),
    bool_value_(bool_value) {
  mss_parameters row(attribute.get_id(), file.get_id(), str_value, num_value,
                     bool_value);

  try {
    mysqlpp::Query insert_query = get_db_connection().query();

    insert_query.replace(row);
    insert_query.execute();

    orig_row_ = row;
    attr_ =
        std::shared_ptr<FileAttribute>(CopyToHeap<FileAttribute>(attribute));
    file_ = std::shared_ptr<FileEntry>(CopyToHeap<FileEntry>(file));
  } catch(const mysqlpp::Exception &e) {
    db_error_ = e.what();
  }
}

FileParameter::FileParameter(const int file_id, const int attr_id,
                             const std::string &str_value, const int num_value,
                             const bool bool_value)
  : str_value_(str_value),
    num_value_(num_value),
    bool_value_(bool_value) {
  mss_parameters row(attr_id, file_id, str_value, num_value,
                     bool_value);

  try {
    mysqlpp::Query insert_query = get_db_connection().query();

    insert_query.replace(row);
    insert_query.execute();

    orig_row_ = row;
    attr_ = FileAttribute::GetById(file_id);
    file_ = FileEntry::GetById(file_id);
  } catch(const mysqlpp::Exception &e) {
    db_error_ = e.what();
  }
}

std::shared_ptr<std::vector<std::shared_ptr<FileParameter> > >
FileParameter::GetByFileAndAttribute(const int file_id, const int attr_id) {
  mysqlpp::Query query =
      get_db_connection().query("select * from mss_parameters param "
                           "where param.file_id = %0:file_id "
                           "and param.attr_id = %1:attr_id");
  query.parse();

  try {
    mysqlpp::StoreQueryResult result = query.store(file_id, attr_id);

    auto finded_parameters = QueryResultToVector(result);

    // Get associated with parameters file and attribute object
    auto attr = FileAttribute::GetById(attr_id);
    if (attr == nullptr)
      return nullptr;

    auto file = FileEntry::GetById(file_id);
    if (file == nullptr)
      return nullptr;

    for (auto parameter : *finded_parameters) {
      parameter->attr_ = attr;
      parameter->file_ = file;
    }

    return finded_parameters;
  } catch(const mysqlpp::Exception &e) {
    db_error_ = e.what();
    return nullptr;
  } catch(const std::bad_alloc &e) {
    db_error_ = e.what();
    return nullptr;
  }
}

std::shared_ptr<std::vector<std::shared_ptr<FileParameter> > >
FileParameter::GetByFileAndAttribute(const FileEntry &file,
                                     const FileAttribute &attribute) {
  mysqlpp::Query query =
      get_db_connection().query("select * from mss_parameters param "
                           "where param.file_id = %0:file_id "
                           "and param.attr_id = %1:attr_id");
  query.parse();

  try {
    mysqlpp::StoreQueryResult result = query.store(file.get_id(),
                                                   attribute.get_id());

    std::shared_ptr<std::vector<std::shared_ptr<FileParameter> > >
        finded_parameters = QueryResultToVector(result);

    auto file_obj = std::shared_ptr<FileEntry>(CopyToHeap<FileEntry>(file));
    auto attr =
        std::shared_ptr<FileAttribute>(CopyToHeap<FileAttribute>(attribute));

    for (std::shared_ptr<FileParameter> parameter : *finded_parameters) {
      parameter->attr_ = attr;
      parameter->file_ = file_obj;
    }
    return finded_parameters;
  } catch(const mysqlpp::Exception &e) {
    db_error_ = e.what();
    return nullptr;
  } catch(const std::bad_alloc &e) {
    db_error_ = e.what();
    return nullptr;
  }
}

std::shared_ptr<std::vector<std::shared_ptr<FileParameter> > >
FileParameter::GetByFile(const int file_id) {
  mysqlpp::Query query =
      get_db_connection().query("select * from mss_parameters param "
                           "where param.file_id = %0:file_id");
  query.parse();
  try {
    mysqlpp::StoreQueryResult results = query.store(file_id);
    return QueryResultToVector(results);
  } catch(const mysqlpp::Exception &e) {
    db_error_ = e.what();
    return nullptr;
  } catch(const std::bad_alloc &e) {
    db_error_ = e.what();
    return nullptr;
  }
}

std::shared_ptr<std::vector<std::shared_ptr<FileParameter> > >
    FileParameter::GetByFile(const FileEntry &file) {
  return GetByFile(file.get_id());
}

std::shared_ptr<std::vector<std::shared_ptr<FileParameter> > >
FileParameter::QueryResultToVector(const mysqlpp::StoreQueryResult &result) {
  auto final_result =
    std::shared_ptr<std::vector<std::shared_ptr<FileParameter>>>(
        new std::vector<std::shared_ptr<FileParameter>>(result.size()));

  for (unsigned int i = 0; i < final_result->size(); ++i)
    final_result->at(i) = std::shared_ptr<FileParameter>(
        new FileParameter(result[i]));

  return final_result;
}
