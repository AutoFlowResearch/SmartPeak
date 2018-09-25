// TODO: Add copyright

#define BOOST_TEST_MODULE Utilities test suite
#include <boost/test/included/unit_test.hpp>
#include <SmartPeak/core/Utilities.h>

using namespace SmartPeak;
using namespace std;

BOOST_AUTO_TEST_SUITE(utilities)

BOOST_AUTO_TEST_CASE(castValue_constructor_copyConstructor)
{
  Utilities::CastValue c;
  BOOST_CHECK_EQUAL(c.getTag(), Utilities::CastValue::UNKNOWN);
  BOOST_CHECK_EQUAL(c.s_, "");

  c = 7;

  BOOST_CHECK_EQUAL(c.getTag(), Utilities::CastValue::INT);
  BOOST_CHECK_EQUAL(c.i_, 7);

  Utilities::CastValue c2 = c;
  BOOST_CHECK_EQUAL(c2.getTag(), Utilities::CastValue::INT);
  BOOST_CHECK_EQUAL(c2.i_, 7);

  Utilities::CastValue c3(c);
  BOOST_CHECK_EQUAL(c3.getTag(), Utilities::CastValue::INT);
  BOOST_CHECK_EQUAL(c3.i_, 7);
}

BOOST_AUTO_TEST_CASE(castString)
{
  Utilities::CastValue c;
  Utilities::castString(string("19"), string("iNT"), c);
  BOOST_CHECK_EQUAL(c.getTag(), Utilities::CastValue::INT);
  BOOST_CHECK_EQUAL(c.i_, 19);
  Utilities::castString(string("tRuE"), string("bOOl"), c);
  BOOST_CHECK_EQUAL(c.getTag(), Utilities::CastValue::BOOL);
  BOOST_CHECK_EQUAL(c.b_, true);
  Utilities::castString(string("False"), string("STRing"), c);
  BOOST_CHECK_EQUAL(c.getTag(), Utilities::CastValue::BOOL);
  BOOST_CHECK_EQUAL(c.b_, false);
  Utilities::castString(string("hello"), string("stRIng"), c);
  BOOST_CHECK_EQUAL(c.getTag(), Utilities::CastValue::STRING);
  BOOST_CHECK_EQUAL(c.s_, "hello");
  Utilities::castString(string("world"), string("String"), c);
  BOOST_CHECK_EQUAL(c.getTag(), Utilities::CastValue::STRING);
  BOOST_CHECK_EQUAL(c.s_, "world");
  Utilities::castString(string("35.35"), string("float"), c);
  BOOST_CHECK_EQUAL(c.getTag(), Utilities::CastValue::FLOAT);
  BOOST_CHECK_CLOSE(c.f_, (float)35.35, 1e-6);
}

BOOST_AUTO_TEST_CASE(updateParameters)
{
  vector<map<string,string>> parameters({
    {
      {"name", "param1"},
      {"type", "int"},
      {"value", "23"},
      {"description", "param1 description"},
      {"tags", "tag1,tag2,tag3"}
    },
    {
      {"name", "param2"},
      {"type", "float"},
      {"value", "3.14"},
      {"tags", "tag4"}
    },
    {
      {"name", "param3"},
      {"type", "bool"},
      {"value", "false"}
    },
    {
      {"name", "param4"},
      {"type", "bool"},
      {"value", "true"}
    },
    {
      {"name", "param5"},
      {"type", "string"},
      {"value", "false"}
    },
    {
      {"name", "param6"},
      {"type", "string"},
      {"value", "true"}
    },
    {
      {"name", "param7"},
      {"type", "string"},
      {"value", "simply a string"}
    },
    {
      {"name", "param8"},
      {"value", "simply a string"}
    },
    {
      {"name", "param9"},
      {"value", "45"}
    },
    {
      {"name", "param10"},
      {"value", "1.27"}
    },
    {
      {"name", "param11"},
      {"value", "false"}
    },
    {
      {"name", "param12"},
      {"value", "true"}
    },
    {
      {"name", "param13"},
      {"description", "foobar"},
      {"tags", "tag5"}
    }
  });

  OpenMS::Param param;
  param.setValue("param1", 10);
  param.setValue("param2", 10.1);
  param.setValue("param3", "true");
  param.setValue("param4", "false");
  param.setValue("param5", "true");
  param.setValue("param6", "false");
  param.setValue("param7", "abc");
  param.setValue("param8", "def");
  param.setValue("param9", 11);
  param.setValue("param10", 11.1);
  param.setValue("param11", "true");
  param.setValue("param12", "false");
  param.setValue("param13", 44);

  Utilities::updateParameters(param, parameters);
  BOOST_CHECK_EQUAL(static_cast<int>(param.getValue("param1")), 23);
  BOOST_CHECK_EQUAL(param.getDescription("param1"), "param1 description");
  BOOST_CHECK_EQUAL(param.hasTag("param1", "tag1"), true);
  BOOST_CHECK_EQUAL(param.hasTag("param1", "tag2"), true);
  BOOST_CHECK_EQUAL(param.hasTag("param1", "tag3"), true);
  BOOST_CHECK_EQUAL(param.hasTag("param1", "tag4"), false);

  BOOST_CHECK_CLOSE(static_cast<double>(param.getValue("param2")), (float)3.14, 1e-6);
  BOOST_CHECK_EQUAL(param.getDescription("param2"), "");
  BOOST_CHECK_EQUAL(param.hasTag("param2", "tag1"), false);
  BOOST_CHECK_EQUAL(param.hasTag("param2", "tag4"), true);

  BOOST_CHECK_EQUAL(param.getValue("param3").toBool(), false);
  BOOST_CHECK_EQUAL(param.getDescription("param3"), "");

  BOOST_CHECK_EQUAL(param.getValue("param4").toBool(), true);
  BOOST_CHECK_EQUAL(param.getValue("param5").toBool(), false);
  BOOST_CHECK_EQUAL(param.getValue("param6").toBool(), true);
  BOOST_CHECK_EQUAL(param.getValue("param7").toString(), "simply a string");
  BOOST_CHECK_EQUAL(param.getValue("param8").toString(), "simply a string");
  BOOST_CHECK_EQUAL(static_cast<int>(param.getValue("param9")), 45);
  BOOST_CHECK_CLOSE(static_cast<double>(param.getValue("param10")), (float)1.27, 1e-6);
  BOOST_CHECK_EQUAL(param.getValue("param11").toBool(), false);
  BOOST_CHECK_EQUAL(param.getValue("param12").toBool(), true);

  BOOST_CHECK_EQUAL(static_cast<int>(param.getValue("param13")), 44);
  BOOST_CHECK_EQUAL(param.getDescription("param13"), "foobar");
  BOOST_CHECK_EQUAL(param.hasTag("param13", "tag5"), true);
}

BOOST_AUTO_TEST_CASE(parseString)
{
  Utilities::CastValue c;

  Utilities::parseString("", c);
  BOOST_CHECK_EQUAL(c.getTag(), Utilities::CastValue::STRING);
  BOOST_CHECK_EQUAL(c.s_, "");

  Utilities::parseString(" foo ", c);
  BOOST_CHECK_EQUAL(c.getTag(), Utilities::CastValue::STRING);
  BOOST_CHECK_EQUAL(c.s_, "foo");

  Utilities::parseString("   34  ", c);
  BOOST_CHECK_EQUAL(c.getTag(), Utilities::CastValue::INT);
  BOOST_CHECK_EQUAL(c.i_, 34);

  Utilities::parseString(" 7.7  ", c);
  BOOST_CHECK_EQUAL(c.getTag(), Utilities::CastValue::FLOAT);
  BOOST_CHECK_CLOSE(c.f_, (float)7.7, 1e-6);

  Utilities::parseString("  false", c);
  BOOST_CHECK_EQUAL(c.getTag(), Utilities::CastValue::BOOL);
  BOOST_CHECK_EQUAL(c.b_, false);

  Utilities::parseString("true   ", c);
  BOOST_CHECK_EQUAL(c.getTag(), Utilities::CastValue::BOOL);
  BOOST_CHECK_EQUAL(c.b_, true);

  Utilities::parseString("[ 1,   2,    3 ]", c);
  BOOST_CHECK_EQUAL(c.getTag(), Utilities::CastValue::INT_LIST);
  BOOST_CHECK_EQUAL(c.il_[0], 1);
  BOOST_CHECK_EQUAL(c.il_[1], 2);
  BOOST_CHECK_EQUAL(c.il_[2], 3);

  Utilities::parseString("[   1 ,  2.2  ,     3.3]", c);
  BOOST_CHECK_EQUAL(c.getTag(), Utilities::CastValue::FLOAT_LIST);
  BOOST_CHECK_CLOSE(c.fl_[0], (float)1, 1e-6);
  BOOST_CHECK_CLOSE(c.fl_[1], (float)2.2, 1e-6);
  BOOST_CHECK_CLOSE(c.fl_[2], (float)3.3, 1e-6);

  Utilities::parseString("[   false ,  false  ,     true]", c);
  BOOST_CHECK_EQUAL(c.getTag(), Utilities::CastValue::BOOL_LIST);
  BOOST_CHECK_EQUAL(c.bl_[0], false);
  BOOST_CHECK_EQUAL(c.bl_[1], false);
  BOOST_CHECK_EQUAL(c.bl_[2], true);

  Utilities::parseString("[   \"first string\" ,  \"second string\"  ,     \"third string\"]", c);
  BOOST_CHECK_EQUAL(c.getTag(), Utilities::CastValue::STRING_LIST);
  BOOST_CHECK_EQUAL(c.sl_[0], "first string");
  BOOST_CHECK_EQUAL(c.sl_[1], "second string");
  BOOST_CHECK_EQUAL(c.sl_[2], "third string");
}

BOOST_AUTO_TEST_CASE(parseList)
{
  const string floats = "[1.1,-2.1,+3.1,4]";
  std::regex re_float_number("[+-]?\\d+(?:\\.\\d+)?"); // copied from .cpp implementation
  Utilities::CastValue c;
  c = std::vector<float>();
  Utilities::parseList(floats, re_float_number, c);
  BOOST_CHECK_CLOSE(c.fl_[0], (float)1.1, 1e-6);
  BOOST_CHECK_CLOSE(c.fl_[1], (float)-2.1, 1e-6);
  BOOST_CHECK_CLOSE(c.fl_[2], (float)3.1, 1e-6);
  BOOST_CHECK_CLOSE(c.fl_[3], (float)4, 1e-6);

  const string ints = "[1,-2,+3,4]";
  std::regex re_integer_number("[+-]?\\d+"); // copied from .cpp implementation
  c = std::vector<int>();
  Utilities::parseList(ints, re_integer_number, c);
  BOOST_CHECK_EQUAL(c.il_[0], 1);
  BOOST_CHECK_EQUAL(c.il_[1], -2);
  BOOST_CHECK_EQUAL(c.il_[2], 3);
  BOOST_CHECK_EQUAL(c.il_[3], 4);

  const string bools = "[true,false,TRuE,fAlSe,TRUE,FALSE]";
  std::regex re_bool("true|false", std::regex::icase); // copied from .cpp implementation
  c = std::vector<bool>();
  Utilities::parseList(bools, re_bool, c);
  BOOST_CHECK_EQUAL(c.bl_[0], true);
  BOOST_CHECK_EQUAL(c.bl_[1], false);
  BOOST_CHECK_EQUAL(c.bl_[2], true);
  BOOST_CHECK_EQUAL(c.bl_[3], false);
  BOOST_CHECK_EQUAL(c.bl_[4], true);
  BOOST_CHECK_EQUAL(c.bl_[5], false);

  const string strings = "[\"foo\",\"bar\",\"foobar\"]";
  std::regex re_s("\"([^,]+)\""); // copied from .cpp implementation
  c = std::vector<std::string>();
  Utilities::parseList(strings, re_s, c);
  BOOST_CHECK_EQUAL(c.sl_[0], string("foo"));
  BOOST_CHECK_EQUAL(c.sl_[1], string("bar"));
  BOOST_CHECK_EQUAL(c.sl_[2], string("foobar"));

  c = 2; // to set tag to INT
  BOOST_CHECK_THROW(Utilities::parseList(strings, re_s, c), std::invalid_argument);
}

BOOST_AUTO_TEST_SUITE_END()
