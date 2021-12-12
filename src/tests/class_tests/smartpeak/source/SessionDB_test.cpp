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
// $Authors: Douglas McCloskey $
// --------------------------------------------------------------------------

#include <gtest/gtest.h>
#include <SmartPeak/test_config.h>
#include <SmartPeak/io/SessionDB.h>

using namespace SmartPeak;
using namespace std;

TEST(SessionDB, dbFilePath)
{
  SessionDB session_db;
  EXPECT_EQ("", session_db.getDBFilePath());
  auto path_db = std::tmpnam(nullptr);
  session_db.setDBFilePath(path_db);
  EXPECT_EQ(path_db, session_db.getDBFilePath());
}

TEST(SessionDB, WriteAndRead)
{
  SessionDB session_db;

  // create db
  auto path_db = std::tmpnam(nullptr);
  session_db.setDBFilePath(path_db);
  
  // write
  auto context = session_db.beginWrite("table1", "col1", "INT", "col2", "REAL", "col3", "STRING");
  ASSERT_TRUE(context);
  session_db.write(*context, 42, 3.14, "test0");
  session_db.write(*context, 142, 13.14, "test1");
  session_db.write(*context, 242, 23.14, "test2");
  session_db.endWrite(*context);

  // read
  context = session_db.beginRead("table1", "col1", "col2", "col3");
  ASSERT_TRUE(context);
  int db_int = 0;
  float db_float = 0.0f;
  std::string db_string;
  bool has_more = session_db.read(*context, db_int, db_float, db_string);
  EXPECT_TRUE(has_more);
  EXPECT_EQ(db_int, 42);
  EXPECT_FLOAT_EQ(db_float, 3.14);
  EXPECT_EQ(db_string, "test0");
  has_more = session_db.read(*context, db_int, db_float, db_string);
  EXPECT_TRUE(has_more);
  EXPECT_EQ(db_int, 142);
  EXPECT_FLOAT_EQ(db_float, 13.14);
  EXPECT_EQ(db_string, "test1");
  has_more = session_db.read(*context, db_int, db_float, db_string);
  EXPECT_TRUE(has_more);
  EXPECT_EQ(db_int, 242);
  EXPECT_FLOAT_EQ(db_float, 23.14);
  EXPECT_EQ(db_string, "test2");
  has_more = session_db.read(*context, db_int, db_float, db_string);
  EXPECT_FALSE(has_more);
}

struct PropertyHandlerTest : IPropertiesHandler
{
  /**
  IPropertiesHandler
  */
  virtual std::string getPropertiesHandlerName() const override
  {
    return "test";
  }

  virtual std::map<std::string, CastValue::Type> getPropertiesSchema() const override
  {
    std::map<std::string, CastValue::Type> properties;
    properties.emplace("test float", CastValue::Type::FLOAT);
    properties.emplace("test int", CastValue::Type::INT);
    properties.emplace("test bool", CastValue::Type::BOOL);
    properties.emplace("test string", CastValue::Type::STRING);
    properties.emplace("test float list", CastValue::Type::FLOAT_LIST);
    properties.emplace("test int list", CastValue::Type::INT_LIST);
    properties.emplace("test bool list", CastValue::Type::BOOL_LIST);
    properties.emplace("test string list", CastValue::Type::STRING_LIST);
    return properties;
  }

  virtual std::optional<CastValue> getProperty(const std::string& property, const size_t row) const override
  {
    if (property == "test float")
    {
      return test_float;
    }
    else if (property == "test int")
    {
      return test_int;
    }
    else if (property == "test bool")
    {
      return test_bool;
    }
    else if (property == "test string")
    {
      return test_string;
    }
    else if (property == "test float list")
    {
      return test_float_list;
    }
    else if (property == "test int list")
    {
      return test_int_list;
    }
    else if (property == "test bool list")
    {
      return test_bool_list;
    }
    else if (property == "test string list")
    {
      return test_string_list;
    }
    return std::nullopt;
  }

  virtual void setProperty(const std::string& property, const CastValue& value, const size_t row) override
  {
    if (property == "test float")
    {
      test_float = value.f_;
    }
    else if (property == "test int")
    {
      test_int = value.i_;
    }
    else if (property == "test bool")
    {
      test_bool = value.b_;
    }
    else if (property == "test string")
    {
      test_string = value.s_;
    }
    else if (property == "test float list")
    {
      test_float_list = value.fl_;
    }
    else if (property == "test int list")
    {
      test_int_list = value.il_;
    }
    else if (property == "test bool list")
    {
      test_bool_list = value.bl_;
    }
    else if (property == "test string list")
    {
      test_string_list = value.sl_;
    }
  }

  float test_float = 0.0f;
  int test_int = 0;
  bool test_bool = false;
  std::string test_string = "nothing";
  std::vector<float> test_float_list = { };
  std::vector<int> test_int_list = { };
  std::vector<bool> test_bool_list = { };
  std::vector<std::string> test_string_list = { };
};

TEST(SessionDB, WriteAndReadPropertiesHandler)
{
  SessionDB session_db;
  auto path_db = std::tmpnam(nullptr);
  session_db.setDBFilePath(path_db);

  PropertyHandlerTest properties_handler_write_test;
  properties_handler_write_test.test_float = 3.14f;
  properties_handler_write_test.test_int = 42;
  properties_handler_write_test.test_bool = true;
  properties_handler_write_test.test_string = "one";
  properties_handler_write_test.test_float_list = { 3.14f, 13.14f, 103.14f };
  properties_handler_write_test.test_int_list = { 42, 43, 44 };
  properties_handler_write_test.test_bool_list = { true, false, true };
  properties_handler_write_test.test_string_list = { "one", "two", "three" };
  EXPECT_TRUE(session_db.writePropertiesHandler(properties_handler_write_test));

  PropertyHandlerTest properties_handler_read_test;
  EXPECT_TRUE(session_db.readPropertiesHandler(properties_handler_read_test));

  EXPECT_EQ(properties_handler_read_test.test_float, properties_handler_write_test.test_float);
  EXPECT_EQ(properties_handler_read_test.test_int, properties_handler_write_test.test_int);
  EXPECT_EQ(properties_handler_read_test.test_bool, properties_handler_write_test.test_bool);
  EXPECT_EQ(properties_handler_read_test.test_string, properties_handler_write_test.test_string);
  EXPECT_EQ(properties_handler_read_test.test_float_list, properties_handler_write_test.test_float_list);
  EXPECT_EQ(properties_handler_read_test.test_int_list, properties_handler_write_test.test_int_list);
  EXPECT_EQ(properties_handler_read_test.test_bool_list, properties_handler_write_test.test_bool_list);
  EXPECT_EQ(properties_handler_read_test.test_string_list, properties_handler_write_test.test_string_list);
}
