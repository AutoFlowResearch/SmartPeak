/**TODO:  Add copyright*/

#define BOOST_TEST_MODULE AppWindow test suite 
#include <boost/test/included/unit_test.hpp>
#include <SmartPeak/ui/AppWindow.h>
#include <SmartPeak/core/AppState.h>
#include <SmartPeak/ui/GuiAppender.h>

using namespace SmartPeak;
using namespace std;

BOOST_AUTO_TEST_SUITE(appWindow)

BOOST_AUTO_TEST_CASE(constructor) 
{
  AppWindow* ptr = nullptr;
  AppWindow* nullPointer = nullptr;
  AppState state;
  GuiAppender guiAppender;
  ptr = new AppWindow(state, guiAppender);
  BOOST_CHECK_NE(ptr, nullPointer);
  delete ptr;
}

BOOST_AUTO_TEST_SUITE_END()