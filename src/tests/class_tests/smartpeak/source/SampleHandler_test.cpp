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

BOOST_AUTO_TEST_CASE(set_get_MetaData)
{
  SampleHandler sampleHandler;

  MetaDataHandler mdh1;
  mdh1.setSampleName("1");

  sampleHandler.setMetaData(mdh1);

  const MetaDataHandler mdh2 = sampleHandler.getMetaData();
  BOOST_CHECK_EQUAL(mdh2.getSampleName(), "1");

  mdh1.setSampleGroupName("2");
  sampleHandler.getMetaData() = mdh1;

  const MetaDataHandler& mdh3 = sampleHandler.getMetaData();
  BOOST_CHECK_EQUAL(mdh3.getSampleName(), "1");
  BOOST_CHECK_EQUAL(mdh3.getSampleGroupName(), "2");
}

BOOST_AUTO_TEST_CASE(set_get_RawData)
{
  SampleHandler sampleHandler;

  OpenMS::FeatureMap f1;
  f1.setIdentifier("1");
  RawDataHandler rdh1;
  rdh1.setFeatureMap(f1);

  sampleHandler.setRawData(rdh1);

  const RawDataHandler rdh2 = sampleHandler.getRawData();
  BOOST_CHECK_EQUAL(rdh2.getFeatureMap().getIdentifier(), "1");

  f1.setIdentifier("2");
  rdh1.setFeatureMap(f1);
  sampleHandler.getRawData() = rdh1;

  const RawDataHandler& rdh3 = sampleHandler.getRawData();
  BOOST_CHECK_EQUAL(rdh3.getFeatureMap().getIdentifier(), "2");
}

BOOST_AUTO_TEST_CASE(clear)
{
  SampleHandler sampleHandler;

  MetaDataHandler mdh1;
  OpenMS::FeatureMap f1;
  RawDataHandler rdh1;
  mdh1.setSampleName("1");
  f1.setIdentifier("1");
  rdh1.setFeatureMap(f1);
  sampleHandler.setMetaData(mdh1);
  sampleHandler.setRawData(rdh1);

  BOOST_CHECK_EQUAL(sampleHandler.getMetaData().getSampleName(), "1");
  BOOST_CHECK_EQUAL(sampleHandler.getRawData().getFeatureMap().getIdentifier(), "1");

  sampleHandler.clear();

  BOOST_CHECK_EQUAL(sampleHandler.getMetaData().getSampleName(), "");
  // TODO: remove following line when clear() is fully implemented
  // BOOST_CHECK_EQUAL(sampleHandler.getRawData().getFeatureMap().getIdentifier(), "");
}

BOOST_AUTO_TEST_SUITE_END()
