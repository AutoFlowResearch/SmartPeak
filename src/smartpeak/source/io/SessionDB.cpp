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

#include <SmartPeak/io/SessionDB.h>
#include <SmartPeak/core/Utilities.h>
#include <plog/Log.h>
#include <regex>

namespace SmartPeak
{
  std::optional<sqlite3*> SessionDB::openSessionDB()
  {
    if (session_file_name_.empty())
    {
      LOGE << "Can't open database, path is empty";
      return std::nullopt;
    }

    sqlite3* db = nullptr;
    int rc;
    rc = sqlite3_open(session_file_name_.generic_string().c_str(), &db);

    if (rc)
    {
      LOGE << "Can't open database (" << session_file_name_.generic_string() << ") : " << sqlite3_errmsg(db);
      return std::nullopt;
    }
    else
    {
      return db;
    }
  }

  void SessionDB::closeSessionDB(sqlite3* db)
  {
    sqlite3_close(db);
  }

  void SessionDB::endRead(SessionDB::DBContext& db_context)
  {
    if (db_context.stmt)
    {
      sqlite3_finalize(db_context.stmt);
      db_context.stmt = nullptr;
    }
    closeSessionDB(db_context.db);
  }

  void SessionDB::endWrite(SessionDB::DBContext& db_context)
  {
    if (db_context.stmt)
    {
      sqlite3_finalize(db_context.stmt);
      db_context.stmt = nullptr;
    }
    closeSessionDB(db_context.db);
  }

  void SessionDB::displaySessionInfo()
  {
    if (!session_info_logged_)
    {
      LOGI << "Generated with SmartPeak Version: " << smartpeak_version_;
      session_info_logged_ = true;
    }
  }

  void SessionDB::updateSessionInfo(sqlite3* db)
  {
    if (smartpeak_version_ == "Unknown")
    {
      int rc;
      sqlite3_stmt* stmt = nullptr;

      std::string sql = "SELECT smartpeak_version from session_info;";
      rc = sqlite3_prepare(db, sql.c_str(), sql.size(), &stmt, NULL);
      if (rc != SQLITE_OK)
      {
        writeSessionInfo(db);
      }
      else
      {
        switch (sqlite3_step(stmt))
        {
        case SQLITE_ROW:
          smartpeak_version_ = (const char*)sqlite3_column_text(stmt, 0);
          break;
        case SQLITE_DONE:
          break;
        default:
          LOGE << "Error reading database: " << sqlite3_errmsg(db);
          break;
        }
      }
      sqlite3_finalize(stmt);
    }
  }

  void SessionDB::writeSessionInfo(sqlite3* db)
  {
    sqlite3_stmt* stmt = nullptr;
    {
      char* zErrMsg = nullptr;
      std::ostringstream os;
      os << "DROP TABLE session_info;";
      std::string sql = os.str();
      int rc = sqlite3_exec(db, sql.c_str(), NULL, 0, &zErrMsg);
    }
    {
      char* zErrMsg = nullptr;
      std::ostringstream os;
      os.str("");
      os << "CREATE TABLE session_info ";
      os << "(";
      os << "ID INTEGER PRIMARY KEY, ";
      os << "smartpeak_version STRING";
      os << "); ";
      std::string sql = os.str();
      int rc = sqlite3_exec(db, sql.c_str(), NULL, 0, &zErrMsg);
      if (rc != SQLITE_OK)
      {
        logSQLError(zErrMsg, sql);
        sqlite3_free(zErrMsg);
      }
    }
    {
      char* zErrMsg = nullptr;
      std::ostringstream os;
      os << "INSERT INTO session_info ";
      os << "(smartpeak_version) ";
      os << "VALUES (";
      os << "\'" << Utilities::getSmartPeakVersion() << "\'";
      os << "); ";
      std::string sql = os.str();
      int rc = sqlite3_exec(db, sql.c_str(), NULL, 0, &zErrMsg);
      if (rc != SQLITE_OK)
      {
        logSQLError(zErrMsg, sql);
        sqlite3_free(zErrMsg);
        return;
      }
      smartpeak_version_ = Utilities::getSmartPeakVersion();
    }
  }

  template <>
  void SessionDB::readRowFromDB<std::string>(SessionDB::DBContext& db_context, int column, std::string& value)
  {
    value = std::string((const char*)sqlite3_column_text(db_context.stmt, column));
  }

  template <>
  void SessionDB::readRowFromDB<OpenMS::String>(SessionDB::DBContext& db_context, int column, OpenMS::String& value)
  {
    value = OpenMS::String((const char*)sqlite3_column_text(db_context.stmt, column));
  }

  template <>
  void SessionDB::readRowFromDB<double>(SessionDB::DBContext& db_context, int column, double& value)
  {
    value = sqlite3_column_double(db_context.stmt, column);
  }

  template <>
  void SessionDB::readRowFromDB<float>(SessionDB::DBContext& db_context, int column, float& value)
  {
    value = sqlite3_column_double(db_context.stmt, column);
  }

  template <>
  void SessionDB::readRowFromDB<int>(SessionDB::DBContext& db_context, int column, int& value)
  {
    value = sqlite3_column_int(db_context.stmt, column);
  }

template <>
void SessionDB::write<std::string>(std::ostringstream& os, SessionDB::DBContext& db_context, const std::string& value)
{
  std::string escaped_string(value);
  escaped_string = std::regex_replace(escaped_string, std::regex("\'"), "\'\'");
  os << "'" << escaped_string << "'";
}

void SessionDB::logSQLError(const std::string& error_message, const std::string& sql_command) const
{
  LOGE << "SQL Error: " << error_message;
  if (!sql_command.empty())
  {
    LOGE << "SQL Command: " << sql_command;
  }
}

int64_t SessionDB::getLastInsertedRowId(SessionDB::DBContext& db_context) const
{
  return sqlite3_last_insert_rowid(db_context.db);
}

std::string valueTypeToString(const CastValue::Type value_type)
{
  switch (value_type)
  {
  case CastValue::Type::BOOL:
    return "INTEGER";
  case CastValue::Type::INT:
    return "INTEGER";
  case CastValue::Type::FLOAT:
    return "REAL";
  case CastValue::Type::STRING:
  case CastValue::Type::BOOL_LIST:
  case CastValue::Type::FLOAT_LIST:
  case CastValue::Type::INT_LIST:
  case CastValue::Type::STRING_LIST:
    return "TEXT";
  default:
    throw std::invalid_argument("Type not supported");
  }
}

std::string castValueToSqlString(const CastValue& cast_value)
{
  switch (cast_value.getTag())
  {
  case CastValue::Type::BOOL:
    return (cast_value.b_ ? "1" : "0");
  case CastValue::Type::STRING:
  case CastValue::Type::STRING_LIST:
  {
    std::string escaped_string(cast_value);
    escaped_string = std::regex_replace(escaped_string, std::regex("\'"), "\'\'");
    return std::string("'") + escaped_string + std::string("'");
  }
  case CastValue::Type::BOOL_LIST:
  case CastValue::Type::FLOAT_LIST:
  case CastValue::Type::INT_LIST:
    return std::string("'") + std::string(cast_value) + std::string("'");
  default:
    return std::string(cast_value);
  }
}

bool SessionDB::writePropertiesHandler(const IPropertiesHandler& properties_handler)
{
  auto table_name = properties_handler.getPropertiesHandlerName();
  LOGD << "Writting " << table_name << " to session db.";

  std::ostringstream os;
  int rc;
  char* zErrMsg = nullptr;
  DBContext db_context;

  // open DB
  auto db = openSessionDB();
  if (!db)
  {
    return false;
  }

  updateSessionInfo(*db);

  db_context.table = table_name;
  db_context.db = *db;

  // first, we delete table (to remove)
  os.str("");
  os << "DROP TABLE ";
  os << "\"" << table_name << "\"";
  os << ";";
  std::string sql = os.str();
  rc = sqlite3_exec(*db, sql.c_str(), NULL, 0, &zErrMsg);
  // -- ignore error

  // create table
  auto properties = properties_handler.getPropertiesSchema();
  os.str("");
  os << "CREATE TABLE ";
  os << "\"" << table_name << "\"";
  os << " (";
  os << "ID INTEGER PRIMARY KEY, ";
  std::string separator;
  for (const auto& [column_name, column_type] : properties)
  {
    // beginWrite(os, db_context.columns, value, value_type, args...);
    os << separator;
    os << "\"" << column_name << "\"";
    os << " ";
    os << valueTypeToString(column_type);
    os << "  NOT NULL";
    separator = ", ";
  }
  os << "); ";
  sql = os.str();
  rc = sqlite3_exec(*db, sql.c_str(), NULL, 0, &zErrMsg);
  if (rc != SQLITE_OK)
  {
    logSQLError(zErrMsg, sql);
    sqlite3_free(zErrMsg);
    return false;
  }

  // write data
  os.str("");
  auto nb_rows = properties_handler.getNbRows();
  for (size_t i = 0; i < nb_rows; ++i)
  {
    os << "INSERT INTO ";
    os << "\"" << table_name << "\"";
    separator = "";
    os << " (";
    for (const auto& [column_name, column_type] : properties)
    {
      os << separator;
      os << "\"" << column_name << "\"";
      separator = ", ";
    }
    os << ") ";

    os << "VALUES (";
    separator = "";
    for (const auto& [column_name, column_type] : properties)
    {
      os << separator;
      auto value = properties_handler.getProperty(column_name);
      if (value)
      {
        os << castValueToSqlString(*value);
      }
      separator = ", ";
    }
    os << "); ";
    std::string sql = os.str();
    rc = sqlite3_exec(db_context.db, sql.c_str(), NULL, 0, &zErrMsg);
    if (rc != SQLITE_OK) {
      logSQLError(zErrMsg, sql);
      sqlite3_free(zErrMsg);
      return false;
    }
  }

  // end write
  if (db_context.stmt)
  {
    sqlite3_finalize(db_context.stmt);
    db_context.stmt = nullptr;
  }
  closeSessionDB(db_context.db);

  return true;
}

bool SessionDB::readPropertiesHandler(IPropertiesHandler& properties_handler)
{
  auto table_name = properties_handler.getPropertiesHandlerName();
  LOGD << "Reading " << table_name << " from session db.";

  std::ostringstream os;
  int rc;
  DBContext db_context;

  // open DB
  auto db = openSessionDB();
  if (!db)
  {
    return false;
  }

  updateSessionInfo(*db);
  displaySessionInfo();

  db_context.table = table_name;
  db_context.db = *db;

  os.str("");
  os << "SELECT * FROM ";
  os << "\"" << table_name << "\"";
  os << "; ";
  std::string sql = os.str();
  rc = sqlite3_prepare(*db, sql.c_str(), sql.size(), &db_context.stmt, NULL);
  if (rc != SQLITE_OK)
  {
    logSQLError(sqlite3_errmsg(*db), sql);
    return false;
  }

  // read rows
  auto properties = properties_handler.getPropertiesSchema();
  bool has_more = true;
  while (has_more)
  {
    switch (sqlite3_step(db_context.stmt))
    {
    case SQLITE_ROW:
    {
      auto nb_columns = sqlite3_column_count(db_context.stmt);
      for (int i = 0; i < nb_columns; ++i)
      {
        std::string column_name(sqlite3_column_name(db_context.stmt, i));
        if (properties.count(column_name))
        {
          auto column_type = properties.at(column_name);
          switch (column_type)
          {
          case CastValue::Type::BOOL:
          {
            bool value(!(sqlite3_column_int(db_context.stmt, i) == 0));
            properties_handler.setProperty(column_name, CastValue(value), i);
            break;
          }
          case CastValue::Type::INT:
          {
            int value(sqlite3_column_int(db_context.stmt, i));
            properties_handler.setProperty(column_name, CastValue(value), i);
            break;
          }
          case CastValue::Type::FLOAT:
          {
            double value(sqlite3_column_double(db_context.stmt, i));
            properties_handler.setProperty(column_name, CastValue(static_cast<float>(value)), i);
            break;
          }
          case CastValue::Type::STRING:
          {
            const std::string value(reinterpret_cast<const char*>(sqlite3_column_text(db_context.stmt, i)));
            properties_handler.setProperty(column_name, CastValue(value), i);
            break;
          }
          case CastValue::Type::BOOL_LIST:
          case CastValue::Type::FLOAT_LIST:
          case CastValue::Type::INT_LIST:
          case CastValue::Type::STRING_LIST:
          {
            LOGE << "STRING_LIST -> start " << column_name;
            CastValue v;
            Utilities::parseString(reinterpret_cast<const char*>(sqlite3_column_text(db_context.stmt, i)), v);
            properties_handler.setProperty(column_name, v , i);
            LOGE << "STRING_LIST -> end" << column_name;
            break;
          }
          default:
          {
            throw std::invalid_argument("Not supported");
          }
          }
        }
      }
    }
    break;
    case SQLITE_DONE:
      has_more = false;
      break;
    default:
      logSQLError(sqlite3_errmsg(db_context.db));
      return false;
    }
  }

  // end reading
  if (db_context.stmt)
  {
    sqlite3_finalize(db_context.stmt);
    db_context.stmt = nullptr;
  }
  closeSessionDB(db_context.db);
  return true;
}

}
