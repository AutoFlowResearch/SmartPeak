// TODO: Add copyright

#include <SmartPeak/test_config.h>

#define BOOST_TEST_MODULE RawDataProcessor test suite
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
  BOOST_CHECK_EQUAL(params_1.size(), 14);
  BOOST_CHECK_EQUAL(params_2.size(), 15);
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

  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureMap().size(), 481);

  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureMap()[0].getSubordinates().size(), 3);

  const OpenMS::Feature& subordinate1 = rawDataHandler.getFeatureMap()[0].getSubordinates()[0];
  BOOST_CHECK_CLOSE(static_cast<double>(subordinate1.getMetaValue("peak_apex_int")), 266403.0, 1e-6);
  BOOST_CHECK_EQUAL(subordinate1.getMetaValue("native_id").toString(), "23dpg.23dpg_1.Heavy");
  BOOST_CHECK_CLOSE(static_cast<double>(subordinate1.getRT()), 953.665693772912, 1e-6);

  const OpenMS::Feature& subordinate2 = rawDataHandler.getFeatureMap()[50].getSubordinates()[0];
  BOOST_CHECK_CLOSE(static_cast<double>(subordinate2.getMetaValue("peak_apex_int")), 0.0, 1e-6);
  BOOST_CHECK_EQUAL(subordinate2.getMetaValue("native_id").toString(), "accoa.accoa_1.Heavy");
  BOOST_CHECK_CLOSE(static_cast<double>(subordinate2.getRT()), 1067.5447296543123, 1e-6);
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

BOOST_AUTO_TEST_CASE(getDefaultRawDataProcessingWorkflow)
{
  std::vector<std::string> workflow;

  RawDataProcessor::getDefaultRawDataProcessingWorkflow(
    MetaDataHandler::SampleType::Unknown,
    workflow
  );

  BOOST_CHECK_EQUAL(workflow.size(), 6);
  BOOST_CHECK_EQUAL(workflow[0], "load_raw_data");
  BOOST_CHECK_EQUAL(workflow[1], "pick_features");
  BOOST_CHECK_EQUAL(workflow[2], "filter_features");
  BOOST_CHECK_EQUAL(workflow[3], "select_features");
  BOOST_CHECK_EQUAL(workflow[4], "quantify_features");
  BOOST_CHECK_EQUAL(workflow[5], "check_features");

  RawDataProcessor::getDefaultRawDataProcessingWorkflow(
    MetaDataHandler::SampleType::DoubleBlank,
    workflow
  );

  BOOST_CHECK_EQUAL(workflow.size(), 5);
  BOOST_CHECK_EQUAL(workflow[0], "load_raw_data");
  BOOST_CHECK_EQUAL(workflow[1], "pick_features");
  BOOST_CHECK_EQUAL(workflow[2], "filter_features");
  BOOST_CHECK_EQUAL(workflow[3], "select_features");
  BOOST_CHECK_EQUAL(workflow[4], "check_features");
}

BOOST_AUTO_TEST_CASE(checkRawDataProcessingWorkflow)
{
  const std::vector<std::string> raw_data_processing1 = {"load_raw_data", "quantify_features"};
  const std::vector<std::string> raw_data_processing2 = {"fake_event", "another_fake"};

  const bool result1 = RawDataProcessor::checkRawDataProcessingWorkflow(raw_data_processing1);
  const bool result2 = RawDataProcessor::checkRawDataProcessingWorkflow(raw_data_processing2);

  BOOST_CHECK_EQUAL(result1, true);
  BOOST_CHECK_EQUAL(result2, false);
}

BOOST_AUTO_TEST_CASE(processRawData)
{
  map<string, vector<map<string, string>>> params_1;
  map<string, vector<map<string, string>>> params_2;
  load_data(params_1, params_2);
  RawDataHandler rawDataHandler;

  const string traML_csv_i = SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_traML_1.csv");
  OpenMSFile::loadTraML(rawDataHandler, traML_csv_i, "csv");

  const string featureFilterComponents_csv_i = SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_mrmfeatureqccomponents_1.csv");
  const string featureFilterComponentGroups_csv_i = SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_mrmfeatureqccomponentgroups_1.csv");

  OpenMSFile::loadFeatureFilter(rawDataHandler, featureFilterComponents_csv_i, featureFilterComponentGroups_csv_i);

  const string quantitationMethods_csv_i = SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_quantitationMethods_1.csv");
  SequenceSegmentHandler sequenceSegmentHandler_IO;
  OpenMSFile::loadQuantitationMethods(sequenceSegmentHandler_IO, quantitationMethods_csv_i);
  rawDataHandler.setQuantitationMethods(sequenceSegmentHandler_IO.getQuantitationMethods());

  OpenMSFile::loadFeatureQC(rawDataHandler, featureFilterComponents_csv_i, featureFilterComponentGroups_csv_i);

  std::vector<std::string> raw_data_processing_events;

  RawDataProcessor::getDefaultRawDataProcessingWorkflow(
    MetaDataHandler::SampleType::Unknown,
    raw_data_processing_events
  );

  const string mzML_i = SMARTPEAK_GET_TEST_DATA_PATH("RawDataProcessor_mzML_1.mzML");
  const std::map<std::string, std::string> filenames = {{"mzML_i", mzML_i}};

  map<string, vector<map<string, string>>>::iterator it = params_1.find("ChromatogramExtractor");
  if (it != params_1.end()) {
    params_1.erase(it);
  }
  params_1.emplace("ChromatogramExtractor", vector<map<string, string>>());

  for (const std::string& event : raw_data_processing_events) {
    // TODO: update if-condition when selectFeatures() and/or validateFeatures() is implemented
    if (event == "select_features" || event == "validate_features")
      continue;
    RawDataProcessor::processRawData(rawDataHandler, event, params_1, filenames);
  }

  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureMap().size(), 329);
  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureMap()[0].getSubordinates().size(), 3);

  const OpenMS::Feature& subordinate0 = rawDataHandler.getFeatureMap()[0].getSubordinates()[0];
  BOOST_CHECK_CLOSE(static_cast<double>(subordinate0.getMetaValue("peak_apex_int")), 266403.0, 1e-6);
  BOOST_CHECK_EQUAL(subordinate0.getMetaValue("native_id").toString(), "23dpg.23dpg_1.Heavy");
  BOOST_CHECK_CLOSE(static_cast<double>(subordinate0.getRT()), 953.665693772912, 1e-6);

  const OpenMS::Feature& subordinate1 = rawDataHandler.getFeatureMap()[0].getSubordinates()[1];
  BOOST_CHECK_EQUAL(subordinate1.getMetaValue("QC_transition_pass").toString(), "1");
  BOOST_CHECK_CLOSE(static_cast<double>(subordinate1.getMetaValue("calculated_concentration")), 0.44335812456518986, 1e-6);
  BOOST_CHECK_EQUAL(subordinate1.getMetaValue("concentration_units").toString(), "uM");

  const OpenMS::Feature& subordinate2 = rawDataHandler.getFeatureMap()[8].getSubordinates()[0];
  BOOST_CHECK_CLOSE(static_cast<double>(subordinate2.getMetaValue("peak_apex_int")), 163.0, 1e-6);
  BOOST_CHECK_EQUAL(subordinate2.getMetaValue("native_id").toString(), "35cgmp.35cgmp_1.Heavy");
  BOOST_CHECK_CLOSE(static_cast<double>(subordinate2.getRT()), 628.59474989938735, 1e-6);

  const OpenMS::Feature& subordinate3 = rawDataHandler.getFeatureMap()[49].getSubordinates()[0];
  BOOST_CHECK_CLOSE(static_cast<double>(subordinate3.getMetaValue("peak_apex_int")), 53935.99999999999, 1e-6);
  BOOST_CHECK_EQUAL(subordinate3.getMetaValue("native_id").toString(), "amp.amp_1.Heavy");
  BOOST_CHECK_CLOSE(static_cast<double>(subordinate3.getRT()), 733.9804012544155, 1e-6);

  const OpenMS::Feature& subordinate4 = rawDataHandler.getFeatureMap()[200].getSubordinates()[0]; // this is [49][0] in python tests
  BOOST_CHECK_CLOSE(static_cast<double>(subordinate4.getMetaValue("peak_apex_int")), 198161.0, 1e-6);
  BOOST_CHECK_EQUAL(subordinate4.getMetaValue("native_id").toString(), "glutacon.glutacon_1.Heavy");
  BOOST_CHECK_CLOSE(static_cast<double>(subordinate4.getRT()), 752.7960037236212, 1e-6);
}

BOOST_AUTO_TEST_CASE(annotateUsedFeatures)
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

  const string featureXML_o1 = SMARTPEAK_GET_TEST_DATA_PATH("RawDataProcessor_test_1_core_RawDataProcessor.featureXML");
  OpenMSFile::loadFeatureMap(rawDataHandler, featureXML_o1);

  RawDataProcessor::saveCurrentFeatureMapToHistory(rawDataHandler);

  const string featureXML_o3 = SMARTPEAK_GET_TEST_DATA_PATH("RawDataProcessor_test_3_core_RawDataProcessor.featureXML");
  OpenMSFile::loadFeatureMap(rawDataHandler, featureXML_o3);

  RawDataProcessor::annotateUsedFeatures(rawDataHandler);

  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureMap().size(), 481);
  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureMap()[0].getSubordinates().size(), 3);

  const OpenMS::Feature& subordinate0 = rawDataHandler.getFeatureMap()[0].getSubordinates()[0];
  BOOST_CHECK_EQUAL(subordinate0.getMetaValue("used_").toBool(), true);
  BOOST_CHECK_EQUAL(subordinate0.getMetaValue("native_id").toString(), "23dpg.23dpg_1.Heavy");

  const OpenMS::Feature& subordinate1 = rawDataHandler.getFeatureMap()[50].getSubordinates()[0];
  BOOST_CHECK_EQUAL(subordinate1.getMetaValue("used_").toBool(), false);
  BOOST_CHECK_EQUAL(subordinate1.getMetaValue("native_id").toString(), "accoa.accoa_1.Heavy");
}

BOOST_AUTO_TEST_CASE(saveCurrentFeatureMapToHistory)
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

  const string featureXML_o1 = SMARTPEAK_GET_TEST_DATA_PATH("RawDataProcessor_test_1_core_RawDataProcessor.featureXML");
  OpenMSFile::loadFeatureMap(rawDataHandler, featureXML_o1);

  RawDataProcessor::saveCurrentFeatureMapToHistory(rawDataHandler);

  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureMapHistory().size(), 1);
  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureMapHistory().back().size(), 481);

  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureMapHistory().back()[0].getSubordinates().size(), 3);

  const OpenMS::Feature& subordinate1 = rawDataHandler.getFeatureMapHistory().back()[0].getSubordinates()[0];
  BOOST_CHECK_CLOSE(static_cast<double>(subordinate1.getMetaValue("peak_apex_int")), 266403.0, 1e-6);
  BOOST_CHECK_EQUAL(subordinate1.getMetaValue("native_id").toString(), "23dpg.23dpg_1.Heavy");
  BOOST_CHECK_CLOSE(static_cast<double>(subordinate1.getRT()), 953.665693772912, 1e-6);

  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureMapHistory().back()[50].getSubordinates().size(), 2);

  const OpenMS::Feature& subordinate2 = rawDataHandler.getFeatureMapHistory().back()[50].getSubordinates()[0];
  BOOST_CHECK_CLOSE(static_cast<double>(subordinate2.getMetaValue("peak_apex_int")), 0.0, 1e-6);
  BOOST_CHECK_EQUAL(subordinate2.getMetaValue("native_id").toString(), "accoa.accoa_1.Heavy");
  BOOST_CHECK_CLOSE(static_cast<double>(subordinate2.getRT()), 1067.54472965431, 1e-6);
}

BOOST_AUTO_TEST_SUITE_END()
