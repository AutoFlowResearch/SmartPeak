/**TODO:  Add copyright*/

#define BOOST_TEST_MODULE Helloworld test suite
#include <boost/test/included/unit_test.hpp>
#include <SmartPeak/core/Helloworld.h>

using namespace SmartPeak;
using namespace std;

BOOST_AUTO_TEST_SUITE(helloworld)

BOOST_AUTO_TEST_CASE(constructor)
{
  Helloworld* ptr = nullptr;
  Helloworld* nullPointer = nullptr;
  ptr = new Helloworld();
  BOOST_CHECK_NE(ptr, nullPointer);
}

BOOST_AUTO_TEST_CASE(destructor)
{
  Helloworld* ptr = nullptr;
  ptr = new Helloworld();
  delete ptr;
}

BOOST_AUTO_TEST_CASE(addNumbers)
{
  Helloworld hw;
  double test = hw.addNumbers(2.0, 2.0);
  BOOST_CHECK_EQUAL(test, 4.0);
}

BOOST_AUTO_TEST_SUITE_END()
