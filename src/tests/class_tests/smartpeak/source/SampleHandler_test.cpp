/**TODO: Add copyright*/

#define BOOST_TEST_MODULE SampleHandler test suite
#include <boost/test/included/unit_test.hpp>
#include <SmartPeak/core/SampleHandler.h>
#include <SmartPeak/core/MetaDataHandler.h>

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

BOOST_AUTO_TEST_CASE(getters_and_setters)
{
  SampleHandler sh;
  MetaDataHandler& mdh = sh.getMetaData();
  BOOST_CHECK_EQUAL(mdh.getSampleName(), "");
  mdh.setSampleName("1");
  sh.getMetaData() = mdh;
  MetaDataHandler& mdh2 = sh.getMetaData();
  BOOST_CHECK_EQUAL(mdh2.getSampleName(), "1");
  mdh2.setSampleName("10");
  sh.setMetaData(mdh2);
  MetaDataHandler& mdh3 = sh.getMetaData();
  BOOST_CHECK_EQUAL(mdh3.getSampleName(), "10");
  // TODO: implement getRawData() and setRawData() tests
}

BOOST_AUTO_TEST_SUITE_END()
