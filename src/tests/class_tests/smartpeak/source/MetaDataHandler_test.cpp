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
  BOOST_CHECK_EQUAL(metaDataHandler.getSampleType(), "");
  BOOST_CHECK_EQUAL(metaDataHandler.getSequenceSegmentName(), "");
  BOOST_CHECK_EQUAL(metaDataHandler.getFilename(), "");
  
  // test setters
  metaDataHandler.setSampleName("1");
  metaDataHandler.setSampleGroupName("2");
  metaDataHandler.setSampleType("3");
  metaDataHandler.setSequenceSegmentName("4");
  metaDataHandler.setFilename("5");
  BOOST_CHECK_EQUAL(metaDataHandler.getSampleName(), "1");
  BOOST_CHECK_EQUAL(metaDataHandler.getSampleGroupName(), "2");
  BOOST_CHECK_EQUAL(metaDataHandler.getSampleType(), "3");
  BOOST_CHECK_EQUAL(metaDataHandler.getSequenceSegmentName(), "4");
  BOOST_CHECK_EQUAL(metaDataHandler.getFilename(), "5");

  // test clear
  metaDataHandler.clear();
  BOOST_CHECK_EQUAL(metaDataHandler.getSampleName(), "");
  BOOST_CHECK_EQUAL(metaDataHandler.getSampleGroupName(), "");
  BOOST_CHECK_EQUAL(metaDataHandler.getSampleType(), "");
  BOOST_CHECK_EQUAL(metaDataHandler.getSequenceSegmentName(), "");
  BOOST_CHECK_EQUAL(metaDataHandler.getFilename(), "");

}

BOOST_AUTO_TEST_CASE(checkSampleType) 
{
  MetaDataHandler metaDataHandler;

  std::vector<std::string> sample_types_valid = {
    "Unknown", "Standard", "QC", "Blank", "Double Blank", "Solvent"};  
  for (const std::string& sample_type: sample_types_valid)
    BOOST_CHECK(metaDataHandler.checkSampleType(sample_type));

  std::vector<std::string> sample_types_invalid = {
    "BLANK", ""};  
  for (const std::string& sample_type: sample_types_invalid)
    BOOST_CHECK(!metaDataHandler.checkSampleType(sample_type));
}

BOOST_AUTO_TEST_SUITE_END()