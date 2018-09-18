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
  filename = filename.substr(filename.find("src/tests")); // otherwise it would contain /home/username/SmartPeak2/
  BOOST_CHECK_EQUAL(filename, "src/tests/class_tests/smartpeak/data/RawDataProcessor_mzML_1.mzML");
  BOOST_CHECK_EQUAL(rawDataHandler.getMetaData().getSampleName(), "150601_0_BloodProject01_PLT_QC_Broth-1");
}

BOOST_AUTO_TEST_CASE(pickFeatures)
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
  RawDataProcessor::pickFeatures(rawDataHandler, params_1.at("MRMFeatureFinderScoring"));

  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureMap().size(), 12);

  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureMap()[0].getSubordinates().size(), 3);

  const OpenMS::Feature& subordinate1 = rawDataHandler.getFeatureMap()[0].getSubordinates()[0];
  BOOST_CHECK_CLOSE(static_cast<double>(subordinate1.getMetaValue("peak_apex_int")), 646692.0, 1e-6);
  BOOST_CHECK_EQUAL(subordinate1.getMetaValue("native_id").toString(), "Hexose_Pool_fru_glc-D.Hexose_Pool_fru_glc-D_1.Heavy");
  BOOST_CHECK_CLOSE(static_cast<double>(subordinate1.getRT()), 58.003782501697543, 1e-6);

  // const OpenMS::Feature& subordinate1 = rawDataHandler.getFeatureMap()[0].getSubordinates()[0];
  // BOOST_CHECK_CLOSE(static_cast<double>(subordinate1.getMetaValue("peak_apex_int")), 266403.0, 1e-6);
  // BOOST_CHECK_EQUAL(subordinate1.getMetaValue("native_id").toString(), "23dpg.23dpg_1.Heavy");
  // BOOST_CHECK_CLOSE(static_cast<double>(subordinate1.getRT()), 953.665693772912, 1e-6);

  // const OpenMS::Feature& subordinate2 = rawDataHandler.getFeatureMap()[50].getSubordinates()[0];
  // BOOST_CHECK_CLOSE(static_cast<double>(subordinate2.getMetaValue("peak_apex_int")), 0.0, 1e-6);
  // BOOST_CHECK_EQUAL(subordinate2.getMetaValue("native_id").toString(), "accoa.accoa_1.Heavy");
  // BOOST_CHECK_CLOSE(static_cast<double>(subordinate2.getRT()), 1067.5447296543123, 1e-6);
}

BOOST_AUTO_TEST_CASE(filterFeatures)
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

  const string featureXML_o = SMARTPEAK_GET_TEST_DATA_PATH("RawDataProcessor_test_1_core_RawDataProcessor.featureXML");

  OpenMSFile::loadFeatureMap(rawDataHandler, featureXML_o);

  const string featureFilterComponents_csv_i = SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_mrmfeatureqccomponents_1.csv");
  const string featureFilterComponentGroups_csv_i = SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_mrmfeatureqccomponentgroups_1.csv");

  OpenMSFile::loadFeatureFilter(rawDataHandler, featureFilterComponents_csv_i, featureFilterComponentGroups_csv_i);

  RawDataProcessor::filterFeatures(rawDataHandler, params_1.at("MRMFeatureFilter.filter_MRMFeatures"));

  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureMap().size(), 329);

  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureMap()[0].getSubordinates().size(), 3);

  const OpenMS::Feature& subordinate1 = rawDataHandler.getFeatureMap()[0].getSubordinates()[0];
  BOOST_CHECK_CLOSE(static_cast<double>(subordinate1.getMetaValue("peak_apex_int")), 266403.0, 1e-6);
  BOOST_CHECK_EQUAL(subordinate1.getMetaValue("native_id").toString(), "23dpg.23dpg_1.Heavy");
  BOOST_CHECK_CLOSE(static_cast<double>(subordinate1.getRT()), 953.66569377291205, 1e-6);

  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureMap()[50].getSubordinates().size(), 2);

  const OpenMS::Feature& subordinate2 = rawDataHandler.getFeatureMap()[50].getSubordinates()[0];
  BOOST_CHECK_CLOSE(static_cast<double>(subordinate2.getMetaValue("peak_apex_int")), 49333.0, 1e-6);
  BOOST_CHECK_EQUAL(subordinate2.getMetaValue("native_id").toString(), "arg-L.arg-L_1.Heavy");
  BOOST_CHECK_CLOSE(static_cast<double>(subordinate2.getRT()), 46.652168337345103, 1e-6);
}


BOOST_AUTO_TEST_CASE(selectFeatures)
{
// TODO: Uncomment once MRMFeatureSelector is ready

//   map<string, vector<map<string, string>>> params_1;
//   map<string, vector<map<string, string>>> params_2;
//   load_data(params_1, params_2);
//   RawDataHandler rawDataHandler;

//   const string traML_csv_i = SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_traML_1.csv");
//   OpenMSFile::loadTraML(rawDataHandler, traML_csv_i, "csv");

//   const string mzML_i = SMARTPEAK_GET_TEST_DATA_PATH("RawDataProcessor_mzML_1.mzML");
//   OpenMSFile::loadMSExperiment(rawDataHandler, mzML_i, params_1.at("MRMMapping"));

//   RawDataProcessor::extractMetaData(rawDataHandler);

//   const string featureXML_o = SMARTPEAK_GET_TEST_DATA_PATH("RawDataProcessor_test_2_core_RawDataProcessor.featureXML");

//   OpenMSFile::loadFeatureMap(rawDataHandler, featureXML_o);

//   RawDataProcessor::selectFeatures(
//     rawDataHandler,
//     params_1.at("MRMFeatureSelector.select_MRMFeatures_qmip"),
//     params_1.at("MRMFeatureSelector.schedule_MRMFeatures_qmip")
//   );

//   BOOST_CHECK_EQUAL(rawDataHandler.getFeatureMap().size(), 329);

//   BOOST_CHECK_EQUAL(rawDataHandler.getFeatureMap()[0].getSubordinates().size(), 3);

//   const OpenMS::Feature& subordinate1 = rawDataHandler.getFeatureMap()[0].getSubordinates()[0];
//   BOOST_CHECK_CLOSE(static_cast<double>(subordinate1.getMetaValue("peak_apex_int")), 266403.0, 1e-6);
//   BOOST_CHECK_EQUAL(subordinate1.getMetaValue("native_id").toString(), "23dpg.23dpg_1.Heavy");
//   BOOST_CHECK_CLOSE(static_cast<double>(subordinate1.getRT()), 953.66569377291205, 1e-6);

//   BOOST_CHECK_EQUAL(rawDataHandler.getFeatureMap()[50].getSubordinates().size(), 2);

//   const OpenMS::Feature& subordinate2 = rawDataHandler.getFeatureMap()[50].getSubordinates()[0];
//   BOOST_CHECK_CLOSE(static_cast<double>(subordinate2.getMetaValue("peak_apex_int")), 49333.0, 1e-6);
//   BOOST_CHECK_EQUAL(subordinate2.getMetaValue("native_id").toString(), "arg-L.arg-L_1.Heavy");
//   BOOST_CHECK_CLOSE(static_cast<double>(subordinate2.getRT()), 46.652168337345103, 1e-6);
}

BOOST_AUTO_TEST_CASE(validateFeatures)
{
// TODO: Uncomment once MRMFeatureValidator is ready
}

BOOST_AUTO_TEST_CASE(plotFeatures)
{
// TODO: Uncomment once FeaturePlotter is ready
}

BOOST_AUTO_TEST_CASE(quantifyComponents)
{
  const string quantitationMethods_csv_i = SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_quantitationMethods_1.csv");
  SequenceSegmentHandler sequenceSegmentHandler_IO;
  OpenMSFile::loadQuantitationMethods(sequenceSegmentHandler_IO, quantitationMethods_csv_i);

  RawDataHandler rawDataHandler;
  rawDataHandler.setQuantitationMethods(sequenceSegmentHandler_IO.getQuantitationMethods());

  const string featureXML_o = SMARTPEAK_GET_TEST_DATA_PATH("RawDataProcessor_test_3_core_RawDataProcessor.featureXML");
  OpenMSFile::loadFeatureMap(rawDataHandler, featureXML_o);

  RawDataProcessor::quantifyComponents(rawDataHandler);


  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureMap().size(), 98);

  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureMap()[0].getSubordinates().size(), 3);

  const OpenMS::Feature& sub = rawDataHandler.getFeatureMap()[0].getSubordinates()[1];

  BOOST_CHECK_EQUAL(sub.getMetaValue("native_id").toString(), "23dpg.23dpg_1.Light");
  BOOST_CHECK_CLOSE(static_cast<double>(sub.getMetaValue("calculated_concentration")), 0.44335812456518986, 1e-6);
  BOOST_CHECK_EQUAL(sub.getMetaValue("concentration_units").toString(), "uM");
}

BOOST_AUTO_TEST_CASE(checkFeatures)
{
  map<string, vector<map<string, string>>> params_1;
  map<string, vector<map<string, string>>> params_2;
  load_data(params_1, params_2);
  RawDataHandler rawDataHandler;

  const string traML_csv_i = SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_traML_1.csv");
  OpenMSFile::loadTraML(rawDataHandler, traML_csv_i, "csv");

  const string featureXML_o = SMARTPEAK_GET_TEST_DATA_PATH("RawDataProcessor_test_3_core_RawDataProcessor.featureXML");
  OpenMSFile::loadFeatureMap(rawDataHandler, featureXML_o);

  const string featureFilterComponents_csv_i = SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_mrmfeatureqccomponents_1.csv");
  const string featureFilterComponentGroups_csv_i = SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_mrmfeatureqccomponentgroups_1.csv");

  OpenMSFile::loadFeatureQC(rawDataHandler, featureFilterComponents_csv_i, featureFilterComponentGroups_csv_i);

  RawDataProcessor::checkFeatures(rawDataHandler, params_1.at("MRMFeatureFilter.filter_MRMFeatures.qc"));

  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureMap().size(), 98);

  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureMap()[0].getSubordinates().size(), 3);

  const OpenMS::Feature& sub = rawDataHandler.getFeatureMap()[0].getSubordinates()[1];
  BOOST_CHECK_EQUAL(sub.getMetaValue("native_id").toString(), "23dpg.23dpg_1.Light");
  BOOST_CHECK_EQUAL(sub.getMetaValue("QC_transition_pass").toString(), "1");
}

BOOST_AUTO_TEST_SUITE_END()
