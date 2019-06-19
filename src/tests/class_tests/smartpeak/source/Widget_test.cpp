/**TODO:  Add copyright*/

#define BOOST_TEST_MODULE Widget test suite 
#include <boost/test/included/unit_test.hpp>
#include <SmartPeak/ui/Widget.h>

using namespace SmartPeak;
using namespace std;

BOOST_AUTO_TEST_SUITE(widget)

BOOST_AUTO_TEST_CASE(constructor) 
{
  Widget* ptr = nullptr;
  Widget* nullPointer = nullptr;
  ptr = new Widget();
  BOOST_CHECK_NE(ptr, nullPointer);
}

BOOST_AUTO_TEST_CASE(destructor) 
{
  Widget* ptr = nullptr;
  ptr = new Widget();
  delete ptr;
}

BOOST_AUTO_TEST_SUITE_END()