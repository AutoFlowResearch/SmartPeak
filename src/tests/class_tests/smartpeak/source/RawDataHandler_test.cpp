/**TODO:  Add copyright*/

#define BOOST_TEST_MODULE RawDataHandler test suite
#include <boost/test/included/unit_test.hpp>
#include <SmartPeak/core/RawDataHandler.h>

using namespace SmartPeak;
using namespace std;

BOOST_AUTO_TEST_SUITE(rawdatahandler)

BOOST_AUTO_TEST_CASE(constructor)
{
  RawDataHandler* ptr = nullptr;
  RawDataHandler* nullPointer = nullptr;
	ptr = new RawDataHandler();
  BOOST_CHECK_NE(ptr, nullPointer);
}

BOOST_AUTO_TEST_CASE(destructor)
{
  RawDataHandler* ptr = nullptr;
	ptr = new RawDataHandler();
  delete ptr;
}

BOOST_AUTO_TEST_CASE(gettersAndSetters)
{
  RawDataHandler rawDataHandler;
  // None
}

BOOST_AUTO_TEST_SUITE_END()