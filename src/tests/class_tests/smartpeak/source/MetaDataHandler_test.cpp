/**TODO:  Add copyright*/

#define BOOST_TEST_MODULE MetaDataHandler test suite
#include <boost/test/included/unit_test.hpp>
#include <SmartPeak/core/MetaDataHandler.h>

using namespace SmartPeak;
using namespace std;

BOOST_AUTO_TEST_SUITE(metadatahandler)

BOOST_AUTO_TEST_CASE(constructor)
{
  MetaDataHandler* ptr = nullptr;
  MetaDataHandler* nullPointer = nullptr;
  ptr = new MetaDataHandler();
  BOOST_CHECK_NE(ptr, nullPointer);
}

BOOST_AUTO_TEST_CASE(destructor)
{
  MetaDataHandler* ptr = nullptr;
  ptr = new MetaDataHandler();
  delete ptr;
}

BOOST_AUTO_TEST_CASE(gettersAndSettersAndClear)
{
  MetaDataHandler metaDataHandler;

  // test defaults
  BOOST_CHECK_EQUAL(metaDataHandler.getSampleName(), "");
  BOOST_CHECK_EQUAL(metaDataHandler.getSampleGroupName(), "");
  BOOST_CHECK_EQUAL((int)metaDataHandler.getSampleType(), (int)MetaDataHandler::SampleType::Unknown);
  BOOST_CHECK_EQUAL(metaDataHandler.getSequenceSegmentName(), "");
  BOOST_CHECK_EQUAL(metaDataHandler.getFilename(), "");

  // test setters
  metaDataHandler.setSampleName("1");
  metaDataHandler.setSampleGroupName("2");
  metaDataHandler.setSampleType(MetaDataHandler::SampleType::QC);
  metaDataHandler.setSequenceSegmentName("4");
  metaDataHandler.setFilename("5");
  BOOST_CHECK_EQUAL(metaDataHandler.getSampleName(), "1");
  BOOST_CHECK_EQUAL(metaDataHandler.getSampleGroupName(), "2");
  BOOST_CHECK_EQUAL((int)metaDataHandler.getSampleType(), (int)MetaDataHandler::SampleType::QC);
  BOOST_CHECK_EQUAL(metaDataHandler.getSequenceSegmentName(), "4");
  BOOST_CHECK_EQUAL(metaDataHandler.getFilename(), "5");

  // test clear
  metaDataHandler.clear();
  BOOST_CHECK_EQUAL(metaDataHandler.getSampleName(), "");
  BOOST_CHECK_EQUAL(metaDataHandler.getSampleGroupName(), "");
  BOOST_CHECK_EQUAL((int)metaDataHandler.getSampleType(), (int)MetaDataHandler::SampleType::Unknown);
  BOOST_CHECK_EQUAL(metaDataHandler.getSequenceSegmentName(), "");
  BOOST_CHECK_EQUAL(metaDataHandler.getFilename(), "");

}

BOOST_AUTO_TEST_CASE(validateMetaData)
{
  MetaDataHandler m;

  m.setSampleName("1");
  m.setSampleGroupName("2");
  m.setSequenceSegmentName("4");
  m.setFilename("5");
  BOOST_CHECK_EQUAL(MetaDataHandler::validateMetaData(m), true);

  m.setSampleType(MetaDataHandler::SampleType::Unknown);
  BOOST_CHECK_EQUAL(MetaDataHandler::validateMetaData(m), true);
  m.setSampleType(MetaDataHandler::SampleType::Standard);
  BOOST_CHECK_EQUAL(MetaDataHandler::validateMetaData(m), true);
  m.setSampleType(MetaDataHandler::SampleType::QC);
  BOOST_CHECK_EQUAL(MetaDataHandler::validateMetaData(m), true);
  m.setSampleType(MetaDataHandler::SampleType::Blank);
  BOOST_CHECK_EQUAL(MetaDataHandler::validateMetaData(m), true);
  m.setSampleType(MetaDataHandler::SampleType::DoubleBlank);
  BOOST_CHECK_EQUAL(MetaDataHandler::validateMetaData(m), true);
  m.setSampleType(MetaDataHandler::SampleType::Solvent);
  BOOST_CHECK_EQUAL(MetaDataHandler::validateMetaData(m), true);

  BOOST_CHECK_EQUAL(MetaDataHandler::SampleTypeToString(MetaDataHandler::SampleType::Unknown), "Unknown");
  BOOST_CHECK_EQUAL(MetaDataHandler::SampleTypeToString(MetaDataHandler::SampleType::Standard), "Standard");
  BOOST_CHECK_EQUAL(MetaDataHandler::SampleTypeToString(MetaDataHandler::SampleType::QC), "QC");
  BOOST_CHECK_EQUAL(MetaDataHandler::SampleTypeToString(MetaDataHandler::SampleType::Blank), "Blank");
  BOOST_CHECK_EQUAL(MetaDataHandler::SampleTypeToString(MetaDataHandler::SampleType::DoubleBlank), "Double Blank");
  BOOST_CHECK_EQUAL(MetaDataHandler::SampleTypeToString(MetaDataHandler::SampleType::Solvent), "Solvent");
}

BOOST_AUTO_TEST_SUITE_END()