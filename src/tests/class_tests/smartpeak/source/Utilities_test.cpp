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
  BOOST_CHECK_EQUAL(c.tag, Utilities::CastValue::UNKNOWN);
  BOOST_CHECK_EQUAL(c.s, "");

  c.setData(7);

  Utilities::CastValue c2 = c;
  BOOST_CHECK_EQUAL(c2.tag, Utilities::CastValue::INT);
  BOOST_CHECK_EQUAL(c2.i, 7);
}

BOOST_AUTO_TEST_CASE(castString)
{
  Utilities::CastValue c;
  Utilities::castString(string("19"), string("iNT"), c);
  BOOST_CHECK_EQUAL(c.tag, Utilities::CastValue::INT);
  BOOST_CHECK_EQUAL(c.i, 19);
  Utilities::castString(string("tRuE"), string("bOOl"), c);
  BOOST_CHECK_EQUAL(c.tag, Utilities::CastValue::BOOL);
  BOOST_CHECK_EQUAL(c.b, true);
  Utilities::castString(string("False"), string("STRing"), c);
  BOOST_CHECK_EQUAL(c.tag, Utilities::CastValue::BOOL);
  BOOST_CHECK_EQUAL(c.b, false);
  Utilities::castString(string("hello"), string("stRIng"), c);
  BOOST_CHECK_EQUAL(c.tag, Utilities::CastValue::STRING);
  BOOST_CHECK_EQUAL(c.s, "hello");
  Utilities::castString(string("world"), string("String"), c);
  BOOST_CHECK_EQUAL(c.tag, Utilities::CastValue::STRING);
  BOOST_CHECK_EQUAL(c.s, "world");
  Utilities::castString(string("35.35"), string("float"), c);
  BOOST_CHECK_EQUAL(c.tag, Utilities::CastValue::FLOAT);
  BOOST_CHECK_CLOSE(c.f, (float)35.35, 1e-6);
}

BOOST_AUTO_TEST_CASE(updateParameters)
{
}

BOOST_AUTO_TEST_CASE(parseString)
{
}

BOOST_AUTO_TEST_CASE(parseList)
{
}

BOOST_AUTO_TEST_SUITE_END()
