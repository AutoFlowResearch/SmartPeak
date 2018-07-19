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
  BOOST_CHECK_EQUAL(mdh.getSampleGroupName(), "");
  BOOST_CHECK_EQUAL(mdh.getSampleType(), "");
  BOOST_CHECK_EQUAL(mdh.getSequenceSegmentName(), "");
  BOOST_CHECK_EQUAL(mdh.getFilename(), "");
  mdh.setSampleName("1");
  mdh.setSampleGroupName("2");
  mdh.setSampleType("3");
  mdh.setSequenceSegmentName("4");
  mdh.setFilename("5");
  sh.getMetaData() = mdh;
  MetaDataHandler& mdh2 = sh.getMetaData();
  BOOST_CHECK_EQUAL(mdh2.getSampleName(), "1");
  BOOST_CHECK_EQUAL(mdh2.getSampleGroupName(), "2");
  BOOST_CHECK_EQUAL(mdh2.getSampleType(), "3");
  BOOST_CHECK_EQUAL(mdh2.getSequenceSegmentName(), "4");
  BOOST_CHECK_EQUAL(mdh2.getFilename(), "5");
  mdh2.setSampleName("10");
  mdh2.setSampleGroupName("20");
  mdh2.setSampleType("30");
  mdh2.setSequenceSegmentName("40");
  mdh2.setFilename("50");
  sh.setMetaData(mdh2);
  MetaDataHandler& mdh3 = sh.getMetaData();
  BOOST_CHECK_EQUAL(mdh3.getSampleName(), "10");
  BOOST_CHECK_EQUAL(mdh3.getSampleGroupName(), "20");
  BOOST_CHECK_EQUAL(mdh3.getSampleType(), "30");
  BOOST_CHECK_EQUAL(mdh3.getSequenceSegmentName(), "40");
  BOOST_CHECK_EQUAL(mdh3.getFilename(), "50");
  // TODO: implement getRawData() and setRawData() tests
}

BOOST_AUTO_TEST_SUITE_END()
