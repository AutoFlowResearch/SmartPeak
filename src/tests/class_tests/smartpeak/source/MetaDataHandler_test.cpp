/**TODO:  Add copyright*/

#define BOOST_TEST_MODULE MetaDataHandler test suite 
#include <boost/test/included/unit_test.hpp>
#include <SmartPeak/core/MetaDataHandler.h>

using namespace SmartPeak;
using namespace std;

BOOST_AUTO_TEST_SUITE(rawdatahandler)

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

BOOST_AUTO_TEST_CASE(gettersAndSetters) 
{
  MetaDataHandler metaDataHandler;
  // None
}

BOOST_AUTO_TEST_CASE(checkSampleType) 
{
  MetaDataHandler metaDataHandler;

  std::vector<std::string> sample_types_valid = {
    "Unknown", "Standard", "QC", "Blank", "Double Blank", "Solvent", "BLANK", ""};  
  for (const std::string& sample_type: sample_types_valid)
    BOOST_CHECK(metaDataHandler.checkSampleType(sample_type));

  std::vector<std::string> sample_types_invalid = {
    "BLANK", ""};  
  for (const std::string& sample_type: sample_types_invalid)
    BOOST_CHECK(!metaDataHandler.checkSampleType(sample_type));
}

BOOST_AUTO_TEST_SUITE_END()