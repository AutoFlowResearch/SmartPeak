// TODO: Add copyright

#define BOOST_TEST_MODULE FileOpenMS test suite
#include <boost/test/included/unit_test.hpp>
#include <SmartPeak/core/FileOpenMS.h>

using namespace SmartPeak;
using namespace std;

BOOST_AUTO_TEST_SUITE(fileopenms)

BOOST_AUTO_TEST_CASE(constructor)
{
  FileOpenMS* ptr = nullptr;
  FileOpenMS* nullPointer = nullptr;
  ptr = new FileOpenMS();
  BOOST_CHECK_NE(ptr, nullPointer);
}

BOOST_AUTO_TEST_CASE(destructor)
{
  FileOpenMS* ptr = nullptr;
  ptr = new FileOpenMS();
  delete ptr;
}

BOOST_AUTO_TEST_SUITE_END()
