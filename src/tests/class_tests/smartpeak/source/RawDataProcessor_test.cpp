// TODO: Add copyright

#include <SmartPeak/test_config.h>

#define BOOST_TEST_MODULE RawDataHandler test suite
#include <boost/test/included/unit_test.hpp>
#include <SmartPeak/core/RawDataProcessor.h>
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

BOOST_AUTO_TEST_CASE(extractMetaData)
{
  map<string, vector<map<string, string>>> params_1;
  map<string, vector<map<string, string>>> params_2;
  load_data(params_1, params_2);
  RawDataHandler rawDataHandler;

  const string traML_csv_i = SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_traML_1.csv");
  OpenMSFile::loadTraML(rawDataHandler, traML_csv_i, "csv");

  const string mzML_i = SMARTPEAK_GET_TEST_DATA_PATH("RawDataProcessor_mzML_1.mzML");
  OpenMSFile::loadMSExperiment(rawDataHandler, mzML_i, params_1.at("MRMMapping"));

  RawDataProcessor::extractMetaData(rawDataHandler);

  string filename = rawDataHandler.getMetaData().getFilename();
  filename = filename.substr(filename.find("src/tests"));
  BOOST_CHECK_EQUAL(filename, "src/tests/class_tests/smartpeak/data/RawDataProcessor_mzML_1.mzML");
  BOOST_CHECK_EQUAL(rawDataHandler.getMetaData().getSampleName(), "150601_0_BloodProject01_PLT_QC_Broth-1");
}

// BOOST_AUTO_TEST_CASE(pickFeatures)
// {
//   map<string, vector<map<string, string>>> params_1;
//   map<string, vector<map<string, string>>> params_2;
//   const string traML_csv_i = SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_traML_1.csv");
//   RawDataHandler rawDataHandler;
//   OpenMSFile::loadTraML(rawDataHandler, traML_csv_i, "csv");
//   const string mzML_i = SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_traML_1.csv");
//   OpenMSFile::loadMSExperiment(rawDataHandler, mzML_i, params_1.at("MRMMapping"));
//   // TODO: incomplete
// }

BOOST_AUTO_TEST_SUITE_END()
