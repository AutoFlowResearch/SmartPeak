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

BOOST_AUTO_TEST_CASE(getters_setters)
{
  MetaDataHandler metaDataHandler;

  // test defaults
  BOOST_CHECK_EQUAL(metaDataHandler.getSampleName(), "");
  BOOST_CHECK_EQUAL(metaDataHandler.getSampleGroupName(), "");
  BOOST_CHECK_EQUAL((int)metaDataHandler.getSampleType(), (int)MetaDataHandler::SampleType::Unknown);
  BOOST_CHECK_EQUAL(metaDataHandler.getSequenceSegmentName(), "");
  BOOST_CHECK_EQUAL(metaDataHandler.getFilename(), "");

  // test setters
  metaDataHandler.setSampleName("this is a sample name");
  metaDataHandler.setSampleGroupName("this is a sample GROUP name");
  metaDataHandler.setSampleType(MetaDataHandler::SampleType::QC);
  metaDataHandler.setSequenceSegmentName("this is a SEQUENCE segment name");
  metaDataHandler.setFilename("some filename");
  BOOST_CHECK_EQUAL(metaDataHandler.getSampleName(), "this is a sample name");
  BOOST_CHECK_EQUAL(metaDataHandler.getSampleGroupName(), "this is a sample GROUP name");
  BOOST_CHECK_EQUAL((int)metaDataHandler.getSampleType(), (int)MetaDataHandler::SampleType::QC);
  BOOST_CHECK_EQUAL(metaDataHandler.getSequenceSegmentName(), "this is a SEQUENCE segment name");
  BOOST_CHECK_EQUAL(metaDataHandler.getFilename(), "some filename");
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

  m.setSampleType(MetaDataHandler::SampleType::Unrecognized);
  BOOST_CHECK_EQUAL(MetaDataHandler::validateMetaData(m), false);
}

BOOST_AUTO_TEST_CASE(clear)
{
  MetaDataHandler m;

  m.setSampleName("1");
  m.setSampleGroupName("2");
  m.setSequenceSegmentName("3");
  m.setFilename("4");
  m.setSampleType(MetaDataHandler::SampleType::QC);

  m.clear();

  BOOST_CHECK_EQUAL(m.getSampleName(), "");
  BOOST_CHECK_EQUAL(m.getSampleGroupName(), "");
  BOOST_CHECK_EQUAL((int)m.getSampleType(), (int)MetaDataHandler::SampleType::Unknown);
  BOOST_CHECK_EQUAL(m.getSequenceSegmentName(), "");
  BOOST_CHECK_EQUAL(m.getFilename(), "");
}

BOOST_AUTO_TEST_CASE(SampleTypeToString)
{
  BOOST_CHECK_EQUAL(MetaDataHandler::SampleTypeToString(MetaDataHandler::SampleType::Unknown), "Unknown");
  BOOST_CHECK_EQUAL(MetaDataHandler::SampleTypeToString(MetaDataHandler::SampleType::Standard), "Standard");
  BOOST_CHECK_EQUAL(MetaDataHandler::SampleTypeToString(MetaDataHandler::SampleType::QC), "QC");
  BOOST_CHECK_EQUAL(MetaDataHandler::SampleTypeToString(MetaDataHandler::SampleType::Blank), "Blank");
  BOOST_CHECK_EQUAL(MetaDataHandler::SampleTypeToString(MetaDataHandler::SampleType::DoubleBlank), "Double Blank");
  BOOST_CHECK_EQUAL(MetaDataHandler::SampleTypeToString(MetaDataHandler::SampleType::Solvent), "Solvent");
  BOOST_CHECK_EQUAL(MetaDataHandler::SampleTypeToString(MetaDataHandler::SampleType::Unrecognized), "Unrecognized");
}

BOOST_AUTO_TEST_SUITE_END()
