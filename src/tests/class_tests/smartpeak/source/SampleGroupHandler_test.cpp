// TODO: Add copyright

#define BOOST_TEST_MODULE SampleGroupHandler test suite
#include <boost/test/included/unit_test.hpp>
#include <SmartPeak/core/SampleGroupHandler.h>

using namespace SmartPeak;
using namespace std;

BOOST_AUTO_TEST_SUITE(samplegrouphandler)

BOOST_AUTO_TEST_CASE(constructor)
{
  SampleGroupHandler* ptr = nullptr;
  SampleGroupHandler* nullPointer = nullptr;
	ptr = new SampleGroupHandler();
  BOOST_CHECK_NE(ptr, nullPointer);
}

BOOST_AUTO_TEST_CASE(destructor)
{
  SampleGroupHandler* ptr = nullptr;
	ptr = new SampleGroupHandler();
  delete ptr;
}

BOOST_AUTO_TEST_SUITE_END()
