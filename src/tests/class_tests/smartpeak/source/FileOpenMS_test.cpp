// TODO: Add copyright

#define BOOST_TEST_MODULE OpenMSFile test suite
#include <boost/test/included/unit_test.hpp>
#include <SmartPeak/core/OpenMSFile.h>

using namespace SmartPeak;
using namespace std;

BOOST_AUTO_TEST_SUITE(fileopenms)

BOOST_AUTO_TEST_CASE(constructor)
{
  OpenMSFile* ptr = nullptr;
  OpenMSFile* nullPointer = nullptr;
  ptr = new OpenMSFile();
  BOOST_CHECK_NE(ptr, nullPointer);
}

BOOST_AUTO_TEST_CASE(destructor)
{
  OpenMSFile* ptr = nullptr;
  ptr = new OpenMSFile();
  delete ptr;
}

BOOST_AUTO_TEST_SUITE_END()
