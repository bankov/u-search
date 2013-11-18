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

#ifndef DATA_STORAGE_ENTITIES_H_
#define DATA_STORAGE_ENTITIES_H_

#define MYSQLPP_MYSQL_HEADERS_BURIED

#ifndef EXPAND_MY_SSQLS_STATICS
#define MYSQLPP_SSQLS_NO_STATICS
#endif  // #ifndef EXPAND_MY_SSQLS_STATICS

#include <sys/time.h>

#include <mysql++/mysql++.h>
#include <mysql++/ssqls.h>
#include <string>
#include <memory>
#include <vector>

sql_create_5(mss_parameters, 2, 5,
             mysqlpp::sql_int, attr_id,
             mysqlpp::sql_int, file_id,
             mysqlpp::sql_text, str_value,
             mysqlpp::sql_int, num_value,
             mysqlpp::sql_boolean, bool_value);

sql_create_3(mss_attributes, 1, 3,
             mysqlpp::sql_int, id,
             mysqlpp::sql_varchar, name,
             mysqlpp::sql_enum, type);

sql_create_5(mss_files, 1, 4,
             mysqlpp::sql_int, id,
             mysqlpp::sql_varchar, name,
             mysqlpp::sql_varchar, file_path,
             mysqlpp::sql_varchar, server_name,
             mysqlpp::sql_timestamp, last_seen);

/**
 * Class to work with data base.
 */
class DatabaseEntity {
  public:
    /** Create an object immediately connected to a database and
      * meet to receive the data and store them in the database. This method
      * must be called with success for properly work of entities objects.
      *
      * @param db_name Name of the database on the server where data is stored.
      * @param server Domain name or ip address of server where data is stored.
      * @param user Username with access to connect to the specified database
      * on specified server.
      * @param password Password for specifed user.
      * @param reconnect should we reconnect programm to another database (or
      * to the same) if we alredy connected?
      *
      * @return true if connection successfull, false otherwise.
      */
    static bool ConnectToServer(const std::string &db_name,
                                const std::string &server,
                                const std::string &user,
                                const std::string &password,
                                const bool reconnect);

    /**
     * Disconnect from connected server.
     *
     * @return true on success, false otherwise.
     */
    static bool Disconnect();

    /**
     * Stores the object in the database.
     * If this object is new and it still does not correspond to any record in
     * the database, created by appropriate record in the database table.
     *
     * @return true if success, else - false.
     */
    virtual bool Commit() = 0;

    /**
     * Removes the record from the database corresponding to the
     * current object.
     *
     * If you try to delete a record that does not exist in the database (for
     * example, the newly created locally or already deleted) the operation was
     * a success.
     *
     * @return true on success, otherwise - false.
     */
    virtual bool Delete() = 0;

    /**
     * Start transaction for opened connection, all next quires will
     * executed in this transaction, until it would be commited or rollbacked.
     *
     * @return true on success, false otherwise.
     */
    static bool StartTransaction();

    /**
     * Commit opened transaction.
     *
     * @return True if transaction opened or false on error.
     */
    static bool CommitTransaction();

    /**
     * Rolling back opened transaction.
     *
     * @return true on success, false otherwise.
     */
    static bool RollbackTransaction();

    /**
     * Returns last occures data base error.
     *
     * @return error string.
     */
    static inline std::string get_db_error() { return db_error_; }

  protected:
    /**
     * Get connection with data base.
     *
     * @return Connection with data base.
     */
    static mysqlpp::TCPConnection & get_db_connection();

    /**
     * Connection with data base.
     */
    static mysqlpp::TCPConnection db_connection_;

    /**
     * Last occured error.
     */
    static std::string db_error_;

    /**
     * Current transaction.
     */
    static std::shared_ptr<mysqlpp::Transaction> current_transaction_;

    /**
     * Last occured error.
     */
    std::string error_;
};

/**
 * One instance of this class corresponds to a single row in the database
 * mss_attributes table.
 *
 * mss_attributes - it's a table containing a records of the possible file
 * attributes stored in the table mss_files.
 */
class FileAttribute : DatabaseEntity {
  public:
    virtual bool Commit() { return false; }
    virtual bool Delete() { return false; }

    /**
     * Enum defines avialible type of value for attribute
     */
    enum AttributeType {
      faString,
      faNum,
      faBool,
      faUnknown
    };

    /**
     * Constructor which create entry with specifed parameters at the
     * database and return object corresponding to this entry.
     *
     * @param name name of new attribute.
     *
     * @param type type of new attribute.
     */
    FileAttribute(const std::string &name, const AttributeType type);

    /**
     * Finds the attribyte entry with specifed id.
     *
     * @param id id of needed row at the database.
     *
     * @return Object corresponding to records at database or nullptr if error
     * occurs or row not founded.
     */
    static std::shared_ptr<FileAttribute> GetById(const int id);

    /**
     * Find attribute entries with the name exactly matches with
     * specified.
     *
     * @param name name to search.
     *
     * @return pointer to vector with objects corresponding to records founded
     * in the database, if error will ocured - returns nullptr.
     */
    static std::shared_ptr<FileAttribute> GetByName(const std::string &name);

    /**
     * Find attribute entries with the name and type exactly matches with
     * specified.
     *
     * @param name name to search
     * @param type type which record must be.
     *
     * @return pointer to vector with objects corresponding to record founded
     * in the database, if error will ocured - returns nullptr.
     */
    static std::shared_ptr<FileAttribute> GetByNameAndType(
        const std::string &name, const AttributeType type);

    /**
     * Set a type of the attribute.
     *
     * @param type Type of the attribute.
     */
    inline void set_type(const AttributeType type) {
      type_ = type;
    }

    /**
     * Set a name of the attribute.
     *
     * @param name of the attribute.
     */
    inline void set_name(const std::string &name) {
      name_ = name;
    }

    /**
     * Get a type of the attribute.
     *
     * @return Type of the attribute.
     */
    inline AttributeType get_type() const {
      return type_;
    }

    /**
     * Get a name of the attribute.
     *
     * @return Name of the attribute.
     */
    inline std::string get_name() const {
      return name_;
    }

    /**
     * Get id of the attribute.
     *
     * @return id of the attribute.
     */
    inline int get_id() const {
      return id_;
    }

  protected:
    /**
     * Convert attributes type to string.
     *
     * @param type Type of the attribute as AttributeType.
     *
     * @return Type of the attribute converted to string.
     */
    static std::string AttrTypeToString(const AttributeType type);

    /**
     * Convert attributes type to AttributeType.
     *
     * @param string Type of the attribute as string.
     *
     * @return Type of the attribute as AttributeType.
     */
    static AttributeType StringToAttrType(const std::string &string);

  private:
    FileAttribute();
    explicit FileAttribute(const mss_attributes &orig_row);
    int id_;
    std::string name_;
    AttributeType type_;
    mss_attributes orig_row_;
};

/**
 * One instance of this class corresponds to a single row in the database
 * mss_files table.
 *
 * mss_files - a table containing information about the files found on the net.
 */
class FileEntry : DatabaseEntity {
  public:
    virtual bool Commit() { return false; }
    virtual bool Delete() { return false; }

    /**
     * Constructor which create entry with specifed parameters at the
     * database and return object corresponding to this entry.
     *
     * @param file_name name of new entry.
     * @param file_path path to file on server corresponding to new entry.
     * @param server_name name or ip address of server where file located.
     */
    FileEntry(const std::string &file_name, const std::string &file_path,
              const std::string &server_name);

    /**
     * The function finds the file entry by name. Insensitive comparison.
     *
     * The search is performed on a %quered_name% template. If you request a
     * search for "Vladimir", the result will be records "Vladimir Visotsky",
     * "Putin Vladimir Vladimirovich" etc.
     *
     * Limitations on min_rownum and max_rownum allow DB to not make unnecessary
     * disk reads with long results. This significantly increases the speed of
     * such queries.
     *
     * @param name name to search.
     * @param min_rownum Limits founded rows from the top, rows from 1 to
     * min_rowsnum will be removed from the results.
     * @param max_rownum Limits founded rows from he bottom, rows from
     * max_rownum to last will be removed from result.
     *
     * @return pointer to vector with objects corresponding to records founded
     * in the database, if error will ocured - returns NULL.
     */
    static std::vector<std::shared_ptr<FileEntry> > *FindByName(
        const std::string &name, const int min_rownum = 0,
        const int max_rownum = 0);

    /**
     * Find file entries with the name exactly matches with specified.
     *
     * @param name name to search.
     * @param min_rownum Limits founded rows from the top, rows from 1 to
     * min_rowsnum will be removed from the results.
     * @param max_rownum Limits founded rows from he bottom, rows from
     * max_rownum to last will be removed from result.
     *
     * @return pointer to vector with objects corresponding to records founded
     * in the database, if error will ocured - returns NULL.
     */
    static std::vector<std::shared_ptr<FileEntry> > *GetByName(
        const std::string &name, const int min_rownum = 0,
        const int max_rownum = 0);

    /**
     * Find the entries relevant to file located on specified server.
     *
     * @param server_name name or ip address of server from which files should
     * be found
     * @param min_rownum Limits founded rows from the top, rows from 1 to
     * min_rowsnum will be removed from the results.
     * @param max_rownum Limits founded rows from he bottom, rows from
     * max_rownum to last will be removed from result.
     *
     * @return pointer to vector with objects corresponding to recordss founded
     * on the database, if error will ocured - return NULL.
     */
    static std::vector<std::shared_ptr<FileEntry> > *GetByServer(
        const std::string &server_name, const int min_rownum = 0,
        const int max_rownum = 0);

    /**
     * Find record which path matches with specifed.
     *
     * @param path path of aimed entry.
     * @param min_rownum Limits founded rows from the top, rows from 1 to
     * min_rowsnum will be removed from the results.
     * @param max_rownum Limits founded rows from he bottom, rows from
     * max_rownum to last will be removed from result.
     *
     * @return pointer to object corresponding to records founded
     * in the database, if error will ocured - return NULL.
     */
    static std::vector<std::shared_ptr<FileEntry> > *GetByPath(
        const std::string &path, const int min_rownum = 0,
        const int max_rownum = 0);

    /**
     * Find row with specifed server and path
     *
     * @param path path to file on server
     * @param server server where file is located
     *
     * @return pointer to object corresponding to record in the database, on
     * error or if nothing was founded return nullptr.
     */
    static std::shared_ptr<FileEntry> GetByPathOnServer(
        const std::string &path, const std::string &server);

    /**
     * Find the file entry with specifed id.
     *
     * @param id id of needed row at the database.
     *
     * @return Object corresponding to records at database or nullptr if error
     * or row not founded.
     */
    static std::shared_ptr<FileEntry> GetById(const int id);

    /**
     * Set name of the file.
     *
     * @param name Name of the file.
     */
    inline void set_name(const std::string &name) {
      name_ = name;
    }

    /**
     * Set path to the file.
     *
     * @param path Path to the file.
     */
    inline void set_file_path(const std::string &path) {
      file_path_ = path;
    }

    /**
     * Set name of the host when file situates.
     *
     * @param hostname name of the host when file situates.
     */
    inline void set_server_name(const std::string &hostname) {
      server_name_ = hostname;
    }

    /**
     * Get id of the file.
     *
     * @return if of the file.
     */
    inline int get_id() const {
      return id_;
    }

    /**
     * Get name of the file.
     *
     * @return Name of the file.
     */
    inline std::string get_name() const {
      return name_;
    }

    /**
     * Get path to the file.
     *
     * @return Path to the file.
     */
    inline std::string get_file_path() const {
      return file_path_;
    }

    /**
     * Get name of the host when file situates.
     *
     * @return name of the host when file situates.
     */
    inline std::string get_server_name() const {
      return server_name_;
    }

    /**
     * Get time when this entry was update at the last time.
     *
     * @return Time when entry was update at the last time.
     */
    inline time_t get_timestamp() const {
      return timestamp_;
    }

  private:
    FileEntry();

    explicit FileEntry(const mss_files &orig_row);

    FileEntry(const int id, const std::string &name,
              const std::string &file_path, const std::string &server_name,
              const time_t timestamp, const mss_files &orig_row);

    static std::vector<std::shared_ptr<FileEntry> > *QueryResultToVector(
        mysqlpp::StoreQueryResult &result);

    int id_;
    std::string name_;
    std::string file_path_;
    std::string server_name_;
    time_t timestamp_;
    mss_files orig_row_;
};

/**
 * One instance of this class corresponds to a single row in the database
 * mss_parameters table.
 *
 * mss_parameters
 */
class FileParameter : DatabaseEntity {
  public:
    virtual bool Commit() { return false; }
    virtual bool Delete() { return false; }
    /**
     * Constructor which create entry with specifed parameters at the
     * database and return object corresponding to this entry.
     *
     * @param file file that corresponds to the parameter.
     * @param attribute attribute that corresponds to the parameter.
     * @param str_value parameter string value.
     * @param num_value parameter numerical value.
     * @param bool_value parameter boolean value.
     */
    FileParameter(const FileEntry &file, const FileAttribute &attribute,
                  const std::string &str_value, const int num_value,
                  const bool bool_value);

    /**
     * Constructor which create entry with specifed parameters at the
     * database and return object corresponding to this entry.
     *
     * @param file_id od of file that corresponds to the parameter.
     * @param attr_id id of attribute that corresponds to the parameter.
     * @param str_value parameter string value.
     * @param num_value parameter numerical value.
     * @param bool_value parameter boolean value.
     */
    FileParameter(const int file_id, const int attr_id,
                  const std::string &str_value, const int num_value,
                  const bool bool_value);

    /**
     * Find entry by file and attribute.
     *
     * @param file_id id of corresponding file to aimed parameter.
     * @param attr_id id of corresponding attribute to aimed parameter.
     *
     * @return pointer to vector with objects corresponding to record founded
     * in the database, if error will ocured - returns nullptr.
     */
    static std::shared_ptr<std::vector<std::shared_ptr<FileParameter> > >
        GetByFileAndAttribute(const int file_id, const int attr_id);

    /**
     * Find entry by file and attribute.
     *
     * @param file file object corresponding to aimed parameter.
     * @param attribute attribute object corresponding to aimed parameter.
     *
     * @return pointer to vector with objects corresponding to record founded
     * in the database, if error will ocured - returns nullptr.
     */
    static std::shared_ptr<std::vector<std::shared_ptr<FileParameter> > >
        GetByFileAndAttribute(const FileEntry &file,
                              const FileAttribute &attribute);

    /**
     * Method returns all parameters associated with specifed file.
     *
     * @param file_id id of specifed file.
     *
     * @return pointer to vector with objects corresponding to records founded
     * in the database, if error will ocured - returns nullptr.
     */
    static std::shared_ptr<std::vector<std::shared_ptr<FileParameter> > >
        GetByFile(const int file_id);

    /**
     * Method returns all parameters associated with specifed file.
     *
     * @param file object corresponding to specifed file.
     *
     * @return pointer to vector with objects corresponding to records founded
     * in the database, if error will ocured - returns nullptr.
     */
    static std::shared_ptr<std::vector<std::shared_ptr<FileParameter> > >
        GetByFile(const FileEntry &file);

    /**
     * Method search entries with specifed string value.
     *
     * @param str_value String value that should have records.
     * @param attr_id Optionally we can search only by specifed attribute.
     *
     * @return pointer to vector with objects corresponding to records founded
     * in the database, if error will ocured - returns nullptr.
     */
    static std::shared_ptr<std::vector<std::shared_ptr<FileParameter> > >
        GetByValue(const std::string &str_value, const int attr_id = -1);

    /**
     * Method search entries with specifed numerical value.
     *
     * @param num_value value that should have records.
     * @param attr_id optionally we can search only by specifed attribute.
     *
     * @return pointer to vector with objects corresponding to records founded
     * in the database, if error will ocured - returns nullptr.
     */
    static std::shared_ptr<std::vector<std::shared_ptr<FileParameter> > >
        GetByValue(const int num_value, const int attr_id = -1);

    /**
     * Method search entries with specifed boolean value.
     *
     * @param bool_value value that should have records.
     * @param attr_id optionally we can search only by specifed attribute.
     *
     * @return pointer to vector with objects corresponding to records founded
     * in the database, if error will ocured - returns nullptr.
     */
    static std::shared_ptr<std::vector<std::shared_ptr<FileParameter> > >
        GetByValue(const bool bool_value, const int attr_id = -1);

    /**
     * Get the attribute the parameter associated with.
     *
     * @return The attribute the parameter is assciated with.
     */
    inline std::shared_ptr<FileAttribute> get_attr() const { return attr_; }

    /**
     * Get file the parameter associoated with.
     *
     * @return The file the parameter associated with.
     */
    inline std::shared_ptr<FileEntry> get_file() const { return file_; }

    /**
     * Get the string value of the attribute.
     *
     * @return The string value of the attribute.
     */
    inline std::string get_str_value() const { return str_value_; }

    /**
     * Get numeric value ot the attribute.
     *
     * @return Numeric value of the attribute.
     */
    inline int get_num_value() const { return num_value_; }

    /**
     * Get boolean value of the attribute.
     *
     * @return Boolean value of the attribute.
     */
    inline bool get_bool_value() const { return bool_value_; }

  private:
    FileParameter();
    explicit FileParameter(const mss_parameters &orig_row);
    explicit FileParameter(const mysqlpp::Row &orig_row);

    static std::shared_ptr<std::vector<std::shared_ptr<FileParameter> > >
        QueryResultToVector(const mysqlpp::StoreQueryResult &result);

    std::shared_ptr<FileAttribute> attr_;
    std::shared_ptr<FileEntry> file_;
    std::string str_value_;
    int num_value_;
    bool bool_value_;
    mss_parameters orig_row_;
};

#endif  // DATA_STORAGE_ENTITIES_H_
