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
      LOGI << "Opened database successfully";
      return db;
    }
  }

  void SessionDB::closeSessionDB(sqlite3* db)
  {
    sqlite3_close(db);
  }

  void SessionDB::endRead(SessionDB::DBContext& db_context)
  {
    closeSessionDB(db_context.db);
  }

  void SessionDB::endWrite(SessionDB::DBContext& db_context)
  {
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
        {
          smartpeak_version_ = (const char*)sqlite3_column_text(stmt, 0);
          return;
        }
        case SQLITE_DONE:
          return;
        default:
          LOGE << "Error reading database: " << sqlite3_errmsg(db);
          return;
        }
      }
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
}
