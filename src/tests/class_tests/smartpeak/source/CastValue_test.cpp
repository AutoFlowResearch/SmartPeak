// TODO: Add copyright

#define BOOST_TEST_MODULE CastValue test suite
#include <boost/test/included/unit_test.hpp>
#include <SmartPeak/core/CastValue.h>

using namespace SmartPeak;
using namespace std;

BOOST_AUTO_TEST_SUITE(castvalue)

BOOST_AUTO_TEST_CASE(castValue_constructor_copyConstructor)
{
  CastValue c;
  BOOST_CHECK_EQUAL(c.getTag(), CastValue::UNINITIALIZED);
  BOOST_CHECK_EQUAL(c.b_, false);

  c = 7;

  BOOST_CHECK_EQUAL(c.getTag(), CastValue::INT);
  BOOST_CHECK_EQUAL(c.i_, 7);

  CastValue c2 = c;
  BOOST_CHECK_EQUAL(c2.getTag(), CastValue::INT);
  BOOST_CHECK_EQUAL(c2.i_, 7);

  CastValue c3(c);
  BOOST_CHECK_EQUAL(c3.getTag(), CastValue::INT);
  BOOST_CHECK_EQUAL(c3.i_, 7);
}

BOOST_AUTO_TEST_SUITE_END()
