// TODO: Add copyright

#include <SmartPeak/test_config.h>

#define BOOST_TEST_MODULE RawDataHandler test suite
#include <boost/test/included/unit_test.hpp>
#include <SmartPeak/core/RawDataHandler.h>
#include <SmartPeak/io/OpenMSFile.h>

using namespace SmartPeak;
using namespace std;

void load_data()
{
  RawDataHandler rawDataHandler;
  string filename_params = SMARTPEAK_GET_TEST_DATA_PATH("RawDataProcessor_params_1_core.csv");
}

BOOST_AUTO_TEST_SUITE(rawdataprocessor)

BOOST_AUTO_TEST_CASE(pickFeatures)
{

}

BOOST_AUTO_TEST_SUITE_END()
