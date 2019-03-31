/**TODO:  Add copyright*/

#define BOOST_TEST_MODULE AppWindow test suite 
#include <boost/test/included/unit_test.hpp>
#include <SmartPeak/ui/AppWindow.h>

using namespace SmartPeak;
using namespace std;

BOOST_AUTO_TEST_SUITE(appWindow)

BOOST_AUTO_TEST_CASE(constructor) 
{
  AppWindow* ptr = nullptr;
  AppWindow* nullPointer = nullptr;
  ptr = new AppWindow();
  BOOST_CHECK_NE(ptr, nullPointer);
}

BOOST_AUTO_TEST_CASE(destructor) 
{
  AppWindow* ptr = nullptr;
  ptr = new AppWindow();
  delete ptr;
}

BOOST_AUTO_TEST_SUITE_END()