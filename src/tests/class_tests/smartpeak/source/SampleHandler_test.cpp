/**TODO: Add copyright*/

#define BOOST_TEST_MODULE SampleHandler test suite 
#include <boost/test/included/unit_test.hpp>
#include <SmartPeak/core/SampleHandler.h>

using namespace SmartPeak;
using namespace std;

BOOST_AUTO_TEST_SUITE(samplehandler)

BOOST_AUTO_TEST_CASE(constructor) 
{
  SampleHandler* ptr = nullptr;
  SampleHandler* nullPointer = nullptr;
  ptr = new SampleHandler();
  BOOST_CHECK_NE(ptr, nullPointer);
}

BOOST_AUTO_TEST_CASE(destructor) 
{
  SampleHandler* ptr = nullptr;
  ptr = new SampleHandler();
  delete ptr;
}

BOOST_AUTO_TEST_SUITE_END()
