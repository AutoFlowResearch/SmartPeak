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
  BOOST_CHECK_EQUAL(c.s, "");

  c = 7;

  Utilities::CastValue c2 = c;
  BOOST_CHECK_EQUAL(c2.getTag(), Utilities::CastValue::INT);
  BOOST_CHECK_EQUAL(c2.i, 7);
}

BOOST_AUTO_TEST_CASE(castString)
{
  Utilities::CastValue c;
  Utilities::castString(string("19"), string("iNT"), c);
  BOOST_CHECK_EQUAL(c.getTag(), Utilities::CastValue::INT);
  BOOST_CHECK_EQUAL(c.i, 19);
  Utilities::castString(string("tRuE"), string("bOOl"), c);
  BOOST_CHECK_EQUAL(c.getTag(), Utilities::CastValue::BOOL);
  BOOST_CHECK_EQUAL(c.b, true);
  Utilities::castString(string("False"), string("STRing"), c);
  BOOST_CHECK_EQUAL(c.getTag(), Utilities::CastValue::BOOL);
  BOOST_CHECK_EQUAL(c.b, false);
  Utilities::castString(string("hello"), string("stRIng"), c);
  BOOST_CHECK_EQUAL(c.getTag(), Utilities::CastValue::STRING);
  BOOST_CHECK_EQUAL(c.s, "hello");
  Utilities::castString(string("world"), string("String"), c);
  BOOST_CHECK_EQUAL(c.getTag(), Utilities::CastValue::STRING);
  BOOST_CHECK_EQUAL(c.s, "world");
  Utilities::castString(string("35.35"), string("float"), c);
  BOOST_CHECK_EQUAL(c.getTag(), Utilities::CastValue::FLOAT);
  BOOST_CHECK_CLOSE(c.f, (float)35.35, 1e-6);
}

BOOST_AUTO_TEST_CASE(updateParameters)
{
}

BOOST_AUTO_TEST_CASE(parseString)
{
  // const vector<string> sl = {"a", "b", "c"};
  // Utilities::CastValue c;
  // Utilities::castString(string("35.35"), string("float"), c);
  // BOOST_CHECK_EQUAL(c.getTag(), Utilities::CastValue::FLOAT);
  // BOOST_CHECK_CLOSE(c.f, (float)35.35, 1e-6);
}

BOOST_AUTO_TEST_CASE(parseList)
{
  const string floats = "[1.1,-2.1,+3.1,4]";
  std::regex re_float_number("[+-]?\\d+(?:\\.\\d+)?"); // copied from .cpp implementation
  Utilities::CastValue c;
  c = std::vector<float>();
  Utilities::parseList(floats, re_float_number, c);
  BOOST_CHECK_CLOSE(c.fl[0], (float)1.1, 1e-6);
  BOOST_CHECK_CLOSE(c.fl[1], (float)-2.1, 1e-6);
  BOOST_CHECK_CLOSE(c.fl[2], (float)3.1, 1e-6);
  BOOST_CHECK_CLOSE(c.fl[3], (float)4, 1e-6);

  const string ints = "[1,-2,+3,4]";
  std::regex re_integer_number("[+-]?\\d+"); // copied from .cpp implementation
  c = std::vector<int>();
  Utilities::parseList(ints, re_integer_number, c);
  BOOST_CHECK_EQUAL(c.il[0], 1);
  BOOST_CHECK_EQUAL(c.il[1], -2);
  BOOST_CHECK_EQUAL(c.il[2], 3);
  BOOST_CHECK_EQUAL(c.il[3], 4);

  const string bools = "[true,false,TRuE,fAlSe,TRUE,FALSE]";
  std::regex re_bool("true|false", std::regex::icase); // copied from .cpp implementation
  c = std::vector<bool>();
  Utilities::parseList(bools, re_bool, c);
  BOOST_CHECK_EQUAL(c.bl[0], true);
  BOOST_CHECK_EQUAL(c.bl[1], false);
  BOOST_CHECK_EQUAL(c.bl[2], true);
  BOOST_CHECK_EQUAL(c.bl[3], false);
  BOOST_CHECK_EQUAL(c.bl[4], true);
  BOOST_CHECK_EQUAL(c.bl[5], false);

  const string strings = "[\"foo\",\"bar\",\"foobar\"]";
  std::regex re_s("[^,\\[\\]\"]+"); // copied from .cpp implementation
  c = std::vector<std::string>();
  Utilities::parseList(strings, re_s, c);
  BOOST_CHECK_EQUAL(c.sl[0], string("foo"));
  BOOST_CHECK_EQUAL(c.sl[1], string("bar"));
  BOOST_CHECK_EQUAL(c.sl[2], string("foobar"));

  // c = 2; // to set tag to INT
  // BOOST_CHECK_THROW(Utilities::parseList(strings, re_s, c), std::invalid_argument());
}

BOOST_AUTO_TEST_SUITE_END()
