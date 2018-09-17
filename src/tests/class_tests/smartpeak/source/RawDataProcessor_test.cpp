// TODO: Add copyright

#include <SmartPeak/test_config.h>

#define BOOST_TEST_MODULE RawDataHandler test suite
#include <boost/test/included/unit_test.hpp>
#include <SmartPeak/core/RawDataHandler.h>
#include <SmartPeak/io/OpenMSFile.h>

using namespace SmartPeak;
using namespace std;

void load_data(
  std::map<std::string, std::vector<std::map<std::string, std::string>>>& params_1,
  std::map<std::string, std::vector<std::map<std::string, std::string>>>& params_2
)
{
  const string pathname1 = SMARTPEAK_GET_TEST_DATA_PATH("RawDataProcessor_params_1_core.csv");
  const string pathname2 = SMARTPEAK_GET_TEST_DATA_PATH("RawDataProcessor_params_2.csv");
  RawDataHandler rawDataHandler;
  OpenMSFile::readRawDataProcessingParameters(rawDataHandler, pathname1, ",");
  params_1 = rawDataHandler.getParameters();
  rawDataHandler.clear();
  OpenMSFile::readRawDataProcessingParameters(rawDataHandler, pathname2, ",");
  params_2 = rawDataHandler.getParameters();
}

BOOST_AUTO_TEST_SUITE(rawdataprocessor)

BOOST_AUTO_TEST_CASE(pickFeatures)
{

}

BOOST_AUTO_TEST_SUITE_END()
