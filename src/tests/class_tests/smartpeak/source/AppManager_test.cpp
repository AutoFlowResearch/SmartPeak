// TODO: Add copyright

#include <SmartPeak/test_config.h>

#define BOOST_TEST_MODULE AppManager test suite
#include <boost/test/included/unit_test.hpp>
#include <SmartPeak/ui/AppManager.h>

using namespace SmartPeak;
using namespace std;

BOOST_AUTO_TEST_SUITE(appmanager)

BOOST_AUTO_TEST_CASE(constructor)
{
  AppManager* ptr = nullptr;
  AppManager* nullPointer = nullptr;
  ptr = new AppManager();
  BOOST_CHECK_NE(ptr, nullPointer);
}

BOOST_AUTO_TEST_CASE(destructor)
{
  AppManager* ptr = new AppManager();
  delete ptr;
}

BOOST_AUTO_TEST_SUITE_END()
