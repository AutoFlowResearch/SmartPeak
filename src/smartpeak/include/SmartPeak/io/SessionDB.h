// --------------------------------------------------------------------------
//   SmartPeak -- Fast and Accurate CE-, GC- and LC-MS(/MS) Data Processing
// --------------------------------------------------------------------------
// Copyright The SmartPeak Team -- Novo Nordisk Foundation 
// Center for Biosustainability, Technical University of Denmark 2018-2021.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL ANY OF THE AUTHORS OR THE CONTRIBUTING
// INSTITUTIONS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
// OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
// WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
// OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
// ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// --------------------------------------------------------------------------
// $Maintainer: Douglas McCloskey, Bertrand Boudaud $
// $Authors: Douglas McCloskey, Bertrand Boudaud $
// --------------------------------------------------------------------------

#pragma once

#include <sqlite3.h>
#include <filesystem>
#include <string>
#include <map>
#include <vector>
#include <optional>
#include <plog/Log.h>
#include <SmartPeak/iface/IPropertiesHandler.h>
#include <OpenMS/DATASTRUCTURES/String.h>

namespace SmartPeak
{
  class SessionDB
  {
  public:

    struct DBContext
    {
      sqlite3* db = nullptr;
      std::vector<std::string> columns;
      std::string table;
      sqlite3_stmt* stmt = nullptr;
    };

    void setDBFilePath(std::filesystem::path session_file_name)
    {
      session_file_name_ = session_file_name;
      smartpeak_version_ = "Unknown";
      session_info_logged_ = false;
    }

    const std::filesystem::path& getDBFilePath() const
    {
      return session_file_name_;
    }

    bool writePropertiesHandler(const IPropertiesHandler& properties_handler);
    bool readPropertiesHandler(IPropertiesHandler& properties_handler);

    template<typename Value, typename ...Args>
    std::optional<DBContext> beginRead(const std::string& table_name, const Value& value, const Args& ...args);

    template<typename WhereValue, typename Value, typename ...Args>
    std::optional<SessionDB::DBContext> beginReadWhere(const std::string& table_name, const std::string& where_id, const WhereValue& where_value, const Value& value, const Args& ...args);

    template<typename ...Args>
    bool read(SessionDB::DBContext& db_context, Args& ...args);

    void endRead(SessionDB::DBContext& db_context);

    template<typename Value, typename ...Args>
    std::optional<DBContext> beginWrite(const std::string& table_name, const Value& value, const char* value_type, const Args& ...args);

    template<typename Value, typename ...Args>
    void write(SessionDB::DBContext& db_context, const Value& value, const Args& ...args);

    void endWrite(SessionDB::DBContext& db_context);

    int64_t getLastInsertedRowId(SessionDB::DBContext& db_context) const;

  protected:
    template<typename Value, typename ...Args>
    void beginRead(std::ostringstream& os, std::vector<std::string>& columns, const Value& value, const Args& ...args);

    template<typename Value>
    void beginRead(std::ostringstream& os, std::vector<std::string>& columns, const Value& value);

    template<typename Value, typename ...Args>
    void beginWrite(std::ostringstream& os, std::vector<std::string>& columns, const Value& value, const char* value_type, const Args& ...args);

    template<typename Value>
    void beginWrite(std::ostringstream& os, std::vector<std::string>& columns, const Value& value, const char* value_type);

    template<typename Value, typename ...Args>
    void write(std::ostringstream& os, SessionDB::DBContext& db_context, const Value& value, const Args& ...args);

    template<typename Value>
    void write(std::ostringstream& os, SessionDB::DBContext& db_context, const Value& value);

    template<typename Value, typename ...Args>
    void readRowFromDB(SessionDB::DBContext& db_context, int column, Value& value, Args& ...args);

    template<typename Value>
    void readRowFromDB(SessionDB::DBContext& db_context, int column, Value& value);

    std::optional<sqlite3*> openSessionDB();

    void closeSessionDB(sqlite3* db);

    void updateSessionInfo(sqlite3* db);

    void displaySessionInfo();

    void writeSessionInfo(sqlite3* db);

    void logSQLError(const std::string& error_message, const std::string& sql_command = "") const;

  protected:
    std::filesystem::path session_file_name_;
    std::string smartpeak_version_ = "Unknown";
    bool session_info_logged_ = false;
  };

  template<typename Value, typename ...Args>
  std::optional<SessionDB::DBContext> SessionDB::beginWrite(const std::string& table_name, const Value& value, const char* value_type, const Args& ...args)
  {
    std::ostringstream os;
    int rc;
    char* zErrMsg = nullptr;
    DBContext db_context;

    // open DB
    auto db = openSessionDB();
    if (!db)
    {
      return std::nullopt;
    }

    updateSessionInfo(*db);

    db_context.table = table_name;
    db_context.db = *db;

    // first, we delete table (to remove)
    os.str("");
    os << "DROP TABLE ";
    os << table_name;
    os << ";";
    std::string sql = os.str();
    rc = sqlite3_exec(*db, sql.c_str(), NULL, 0, &zErrMsg);
    // -- ignore error

    // create table
    os.str("");
    os << "CREATE TABLE ";
    os << table_name;
    os << " (";
    os << "ID INTEGER PRIMARY KEY, ";
    beginWrite(os, db_context.columns, value, value_type, args...);
    os << "); ";
    sql = os.str();
    rc = sqlite3_exec(*db, sql.c_str(), NULL, 0, &zErrMsg);
    if (rc != SQLITE_OK)
    {
      logSQLError(zErrMsg, sql);
      sqlite3_free(zErrMsg);
      return std::nullopt;
    }
    return db_context;
  }

  template<typename Value, typename ...Args>
  void SessionDB::beginWrite(std::ostringstream& os, std::vector<std::string>& columns, const Value& value, const char* value_type, const Args& ...args)
  {
    beginWrite(os, columns, value, value_type);
    os << ", ";
    beginWrite(os, columns, args...);
  }

  template<typename Value>
  void SessionDB::beginWrite(std::ostringstream& os, std::vector<std::string>& columns, const Value& value, const char* value_type)
  {
    columns.push_back(value);
    os << value;
    os << " ";
    os << value_type;
    os << "  NOT NULL";
  }

  template<typename Value, typename ...Args>
  void SessionDB::write(SessionDB::DBContext& db_context, const Value& value, const Args& ...args)
  {
    std::ostringstream os;
    int rc;
    char* zErrMsg = nullptr;

    os << "INSERT INTO ";
    os << db_context.table;
    os << " (";
    for (size_t i = 0; i < db_context.columns.size(); ++i)
    {
      const auto& column = db_context.columns[i];
      os << column;
      if (i < (db_context.columns.size() - 1))
      {
        os << ", ";
      }
    }
    os << ") ";
    os << "VALUES (";
    write(os, db_context, value, args...);
    os << "); ";
    std::string sql = os.str();
    rc = sqlite3_exec(db_context.db, sql.c_str(), NULL, 0, &zErrMsg);
    if (rc != SQLITE_OK) {
      logSQLError(zErrMsg, sql);
      sqlite3_free(zErrMsg);
    }
  }

  template<typename Value, typename ...Args>
  void SessionDB::write(std::ostringstream& os, SessionDB::DBContext& db_context, const Value& value, const Args& ...args)
  {
    write(os, db_context, value);
    os << ", ";
    write(os, db_context, args...);
  }

  template<typename Value>
  void SessionDB::write(std::ostringstream& os, SessionDB::DBContext& db_context, const Value& value)
  {
    os << "'" << value << "'";
  }

  template<typename Value, typename ...Args>
  std::optional<SessionDB::DBContext> SessionDB::beginRead(const std::string& table_name, const Value& value, const Args& ...args)
  {
    std::ostringstream os;
    int rc;
    DBContext db_context;

    // open DB
    auto db = openSessionDB();
    if (!db)
    {
      return std::nullopt;
    }

    updateSessionInfo(*db);
    displaySessionInfo();

    db_context.table = table_name;
    db_context.db = *db;

    os.str("");
    os << "SELECT ";
    beginRead(os, db_context.columns, value, args...);
    os << " FROM ";
    os << table_name;
    os << "; ";
    std::string sql = os.str();
    rc = sqlite3_prepare(*db, sql.c_str(), sql.size(), &db_context.stmt, NULL);
    if (rc != SQLITE_OK)
    {
      logSQLError(sqlite3_errmsg(*db), sql);
      return std::nullopt;
    }
    return db_context;
  }

  template<typename WhereValue, typename Value, typename ...Args>
  std::optional<SessionDB::DBContext> SessionDB::beginReadWhere(const std::string& table_name, const std::string& where_id, const WhereValue& where_value, const Value& value, const Args& ...args)
  {
    std::ostringstream os;
    int rc;
    DBContext db_context;

    // open DB
    auto db = openSessionDB();
    if (!db)
    {
      return std::nullopt;
    }

    updateSessionInfo(*db);
    displaySessionInfo();

    db_context.table = table_name;
    db_context.db = *db;

    os.str("");
    os << "SELECT ";
    beginRead(os, db_context.columns, value, args...);
    os << " FROM ";
    os << table_name;
    os << " ";
    os << " WHERE ";
    os << where_id << "=" << where_value << "; ";
    std::string sql = os.str();
    rc = sqlite3_prepare(*db, sql.c_str(), sql.size(), &db_context.stmt, NULL);
    if (rc != SQLITE_OK)
    {
      logSQLError(sqlite3_errmsg(*db), sql);
      return std::nullopt;
    }
    return db_context;
  }

  template<typename Value, typename ...Args>
  void SessionDB::beginRead(std::ostringstream& os, std::vector<std::string>& columns, const Value& value, const Args& ...args)
  {
    beginRead(os, columns, value);
    os << ", ";
    beginRead(os, columns, args...);
  }

  template<typename Value>
  void SessionDB::beginRead(std::ostringstream& os, std::vector<std::string>& columns, const Value& value)
  {
    columns.push_back(value);
    os << value;
  }

  template<typename ...Args>
  bool SessionDB::read(SessionDB::DBContext& db_context, Args& ...args)
  {
    switch (sqlite3_step(db_context.stmt))
    {
    case SQLITE_ROW:
    {
      readRowFromDB(db_context, 0, args...);
      return true;
    }
    case SQLITE_DONE:
      return false;
    default:
      logSQLError(sqlite3_errmsg(db_context.db));
      return false;
    }
  }

  template<typename Value, typename ...Args>
  void SessionDB::readRowFromDB(SessionDB::DBContext& db_context, int column, Value& value, Args& ...args)
  {
    readRowFromDB(db_context, column++, value);
    readRowFromDB(db_context, column++, args...);
  }

  template <>
  void SessionDB::write<std::string>(std::ostringstream& os, SessionDB::DBContext& db_context, const std::string& value);

}
