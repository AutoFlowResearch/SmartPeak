/**TODO:  Add copyright*/

#define BOOST_TEST_MODULE AppWindow test suite 
#include <boost/test/included/unit_test.hpp>
#include <SmartPeak/ui/AppWindow.h>
#include <SmartPeak/core/AppState.h>

using namespace SmartPeak;
using namespace std;

BOOST_AUTO_TEST_SUITE(appWindow)

BOOST_AUTO_TEST_CASE(constructor) 
{
  AppWindow* ptr = nullptr;
  AppWindow* nullPointer = nullptr;
  AppState state;
  ptr = new AppWindow(state);
  BOOST_CHECK_NE(ptr, nullPointer);
}

BOOST_AUTO_TEST_CASE(destructor) 
{
  AppWindow* ptr = nullptr;
  AppState state;
  ptr = new AppWindow(state);
  delete ptr;
}

BOOST_AUTO_TEST_SUITE_END()