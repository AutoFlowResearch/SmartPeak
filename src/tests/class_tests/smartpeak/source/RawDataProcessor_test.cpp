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
  const string pathname1 = SMARTPEAK_GET_TEST_DATA_PATH("RawDataProcessor_params_1_core_tmpFix.csv");
  const string pathname2 = SMARTPEAK_GET_TEST_DATA_PATH("RawDataProcessor_params_2_tmpFix.csv");
  RawDataHandler rawDataHandler;
  OpenMSFile::readRawDataProcessingParameters(rawDataHandler, pathname1, ",");
  params_1 = rawDataHandler.getParameters();
  rawDataHandler.clear();
  OpenMSFile::readRawDataProcessingParameters(rawDataHandler, pathname2, ",");
  params_2 = rawDataHandler.getParameters();
}

BOOST_AUTO_TEST_SUITE(rawdataprocessor)

/**
  LoadRawData Tests
*/
BOOST_AUTO_TEST_CASE(constructorLoadRawData)
{
  LoadRawData* ptrLoadRawData = nullptr;
  LoadRawData* nullPointerLoadRawData = nullptr;
  BOOST_CHECK_EQUAL(ptrLoadRawData, nullPointerLoadRawData);
}

BOOST_AUTO_TEST_CASE(destructorLoadRawData)
{
  LoadRawData* ptrLoadRawData = nullptr;
  ptrLoadRawData = new LoadRawData();
  delete ptrLoadRawData;
}

BOOST_AUTO_TEST_CASE(gettersLoadRawData)
{
  LoadRawData processor;

  BOOST_CHECK_EQUAL(processor.getID(), 1);
  BOOST_CHECK_EQUAL(processor.getName(), "LOAD_RAW_DATA");
}

BOOST_AUTO_TEST_CASE(processorLoadRawData)
{
  LoadRawData processor;

  // TODO: add more tests once loadMSExperiment is split
  RawDataHandler rawDataHandler;
  std::vector<std::map<std::string, std::string>> mzML_params_I = {
    {
      {"name", "zero_baseline"},
      {"type", "bool"},
      {"value", "false"}
    }
  };

  const string pathname1 = SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_baseline_correction.mzML");

  processor.process(
    rawDataHandler,
    pathname1,
    std::vector<std::map<std::string, std::string>>(),
    std::vector<std::map<std::string, std::string>>(),
    mzML_params_I
  );

  const vector<OpenMS::MSChromatogram>& chromatograms1 = rawDataHandler.getExperiment().getChromatograms();

  BOOST_CHECK_EQUAL(chromatograms1.size(), 2);

  BOOST_CHECK_CLOSE(chromatograms1[0][0].getIntensity(), 3.0, 1e-3);
  BOOST_CHECK_CLOSE(chromatograms1[0][1].getIntensity(), 4.0, 1e-3);
  BOOST_CHECK_CLOSE(chromatograms1[0][2].getIntensity(), 6.0, 1e-3);
  BOOST_CHECK_CLOSE(chromatograms1[0][3].getIntensity(), 19.0, 1e-3);
  BOOST_CHECK_CLOSE(chromatograms1[0][4].getIntensity(), 1.0, 1e-3);

  BOOST_CHECK_CLOSE(chromatograms1[1][0].getIntensity(), 3.0, 1e-3);
  BOOST_CHECK_CLOSE(chromatograms1[1][1].getIntensity(), 4.0, 1e-3);
  BOOST_CHECK_CLOSE(chromatograms1[1][2].getIntensity(), -2.0, 1e-3);
  BOOST_CHECK_CLOSE(chromatograms1[1][3].getIntensity(), -8.0, 1e-3);
  BOOST_CHECK_CLOSE(chromatograms1[1][4].getIntensity(), 1.0, 1e-3);

  mzML_params_I.at(0).at("value") = "true";
  rawDataHandler.clear();

  OpenMSFile::loadMSExperiment(
    rawDataHandler,
    pathname1,
    std::vector<std::map<std::string, std::string>>(),
    std::vector<std::map<std::string, std::string>>(),
    mzML_params_I
  );

  const vector<OpenMS::MSChromatogram>& chromatograms2 = rawDataHandler.getExperiment().getChromatograms();

  BOOST_CHECK_EQUAL(chromatograms2.size(), 2);

  BOOST_CHECK_CLOSE(chromatograms2[0][0].getIntensity(), 2.0, 1e-3);
  BOOST_CHECK_CLOSE(chromatograms2[0][1].getIntensity(), 3.0, 1e-3);
  BOOST_CHECK_CLOSE(chromatograms2[0][2].getIntensity(), 5.0, 1e-3);
  BOOST_CHECK_CLOSE(chromatograms2[0][3].getIntensity(), 18.0, 1e-3);
  BOOST_CHECK_CLOSE(chromatograms2[0][4].getIntensity(), 0.0, 1e-3);

  BOOST_CHECK_CLOSE(chromatograms2[1][0].getIntensity(), 11.0, 1e-3);
  BOOST_CHECK_CLOSE(chromatograms2[1][1].getIntensity(), 12.0, 1e-3);
  BOOST_CHECK_CLOSE(chromatograms2[1][2].getIntensity(), 6.0, 1e-3);
  BOOST_CHECK_CLOSE(chromatograms2[1][3].getIntensity(), 0.0, 1e-3);
  BOOST_CHECK_CLOSE(chromatograms2[1][4].getIntensity(), 9.0, 1e-3);

  mzML_params_I.at(0).at("value") = "false";
  mzML_params_I.push_back(
    {
      {"name", "format"},
      {"type", "string"},
      {"value", "ChromeleonFile"}
    }
  );
  rawDataHandler.clear();

  const string pathname2 = SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_ChromeleonFile_10ug.txt");

  OpenMSFile::loadMSExperiment(
    rawDataHandler,
    pathname2,
    std::vector<std::map<std::string, std::string>>(),
    std::vector<std::map<std::string, std::string>>(),
    mzML_params_I
  );

  const vector<OpenMS::MSChromatogram>& chromatograms3 = rawDataHandler.getExperiment().getChromatograms();

  BOOST_CHECK_EQUAL(chromatograms3.size(), 1);
  BOOST_CHECK_EQUAL(chromatograms3[0].size(), 3301);

  BOOST_CHECK_CLOSE(chromatograms3[0][0].getIntensity(), 0.0, 1e-3);
  BOOST_CHECK_CLOSE(chromatograms3[0][600].getIntensity(), -0.503815, 1e-3);
  BOOST_CHECK_CLOSE(chromatograms3[0][1200].getIntensity(), -0.666694, 1e-3);
  BOOST_CHECK_CLOSE(chromatograms3[0][1800].getIntensity(), -0.232843, 1e-3);
  BOOST_CHECK_CLOSE(chromatograms3[0][2400].getIntensity(), -0.223644, 1e-3);
  BOOST_CHECK_CLOSE(chromatograms3[0][3300].getIntensity(), 0.126958, 1e-3);

  mzML_params_I.at(0).at("value") = "true";
  rawDataHandler.clear();

  OpenMSFile::loadMSExperiment(
    rawDataHandler,
    pathname2,
    std::vector<std::map<std::string, std::string>>(),
    std::vector<std::map<std::string, std::string>>(),
    mzML_params_I
  );

  const vector<OpenMS::MSChromatogram>& chromatograms4 = rawDataHandler.getExperiment().getChromatograms();

  BOOST_CHECK_EQUAL(chromatograms4.size(), 1);
  BOOST_CHECK_EQUAL(chromatograms4[0].size(), 3301);

  BOOST_CHECK_CLOSE(chromatograms4[0][0].getIntensity(), 1.004634, 1e-3);
  BOOST_CHECK_CLOSE(chromatograms4[0][600].getIntensity(), 0.500819, 1e-3);
  BOOST_CHECK_CLOSE(chromatograms4[0][1200].getIntensity(), 0.33794, 1e-3);
  BOOST_CHECK_CLOSE(chromatograms4[0][1800].getIntensity(), 0.771791, 1e-3);
  BOOST_CHECK_CLOSE(chromatograms4[0][2400].getIntensity(), 0.78099, 1e-3);
  BOOST_CHECK_CLOSE(chromatograms4[0][3300].getIntensity(), 1.131592, 1e-3);
}

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

  LoadRawData::extractMetaData(rawDataHandler);

  const MetaDataHandler& metaDataHandler = rawDataHandler.getMetaData();

  string filename = metaDataHandler.getFilename();
  filename = filename.substr(filename.find("src/tests")); // otherwise it would contain /home/username/SmartPeak2/
  BOOST_CHECK_EQUAL(filename, "src/tests/class_tests/smartpeak/data/RawDataProcessor_mzML_1.mzML");
  BOOST_CHECK_EQUAL(metaDataHandler.getSampleName(), "150601_0_BloodProject01_PLT_QC_Broth-1");

  BOOST_CHECK_EQUAL(metaDataHandler.proc_method_name, "Analyst");
  BOOST_CHECK_EQUAL(metaDataHandler.instrument, "QTRAP 5500");
  // BOOST_CHECK_EQUAL(rawDataHandler.acq_method_name, "");
  BOOST_CHECK_EQUAL(metaDataHandler.operator_name, "");
  BOOST_CHECK_EQUAL(metaDataHandler.acquisition_date_and_time.tm_mday, 10);
  BOOST_CHECK_EQUAL(metaDataHandler.acquisition_date_and_time.tm_mon, 06);
  BOOST_CHECK_EQUAL(metaDataHandler.acquisition_date_and_time.tm_year, 2015);
  BOOST_CHECK_EQUAL(metaDataHandler.acquisition_date_and_time.tm_hour, 01);
  BOOST_CHECK_EQUAL(metaDataHandler.acquisition_date_and_time.tm_min, 14);
  BOOST_CHECK_EQUAL(metaDataHandler.acquisition_date_and_time.tm_sec, 10);
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

  const OpenMS::Feature& subordinate1 = rawDataHandler.getFeatureMap()[0].getSubordinates()[0];  // feature_map
  BOOST_CHECK_CLOSE(static_cast<double>(subordinate1.getMetaValue("peak_apex_int")), 266403.0, 1e-6);
  BOOST_CHECK_EQUAL(subordinate1.getMetaValue("native_id").toString(), "23dpg.23dpg_1.Heavy");
  BOOST_CHECK_CLOSE(static_cast<double>(subordinate1.getRT()), 953.665693772912, 1e-6);

  const OpenMS::Feature& subordinate2 = rawDataHandler.getFeatureMap()[50].getSubordinates()[0];
  BOOST_CHECK_CLOSE(static_cast<double>(subordinate2.getMetaValue("peak_apex_int")), 0.0, 1e-6);
  BOOST_CHECK_EQUAL(subordinate2.getMetaValue("native_id").toString(), "accoa.accoa_1.Heavy");
  BOOST_CHECK_CLOSE(static_cast<double>(subordinate2.getRT()), 1067.5447296543123, 1e-6);

  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureMapHistory().size(), 481);
  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureMapHistory()[0].getSubordinates().size(), 3);

  const OpenMS::Feature& hsubordinate1 = rawDataHandler.getFeatureMapHistory()[0].getSubordinates()[0]; // feature_map_history
  BOOST_CHECK_CLOSE(static_cast<double>(hsubordinate1.getMetaValue("peak_apex_int")), 266403.0, 1e-6);
  BOOST_CHECK_EQUAL(hsubordinate1.getMetaValue("native_id").toString(), "23dpg.23dpg_1.Heavy");
  BOOST_CHECK_CLOSE(static_cast<double>(hsubordinate1.getRT()), 953.665693772912, 1e-6);
  BOOST_CHECK(hsubordinate1.getMetaValue("used_").toBool());

  const OpenMS::Feature& hsubordinate2 = rawDataHandler.getFeatureMapHistory()[50].getSubordinates()[0];
  BOOST_CHECK_CLOSE(static_cast<double>(hsubordinate2.getMetaValue("peak_apex_int")), 0.0, 1e-6);
  BOOST_CHECK_EQUAL(hsubordinate2.getMetaValue("native_id").toString(), "accoa.accoa_1.Heavy");
  BOOST_CHECK_CLOSE(static_cast<double>(hsubordinate2.getRT()), 1067.5447296543123, 1e-6);
  BOOST_CHECK(hsubordinate2.getMetaValue("used_").toBool());
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

  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureMap().size(), 329); // Test feature_map
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

  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureMapHistory().size(), 481); // Test feature_map_history
  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureMapHistory()[0].getSubordinates().size(), 3);

  const OpenMS::Feature& hsubordinate1 = rawDataHandler.getFeatureMapHistory()[0].getSubordinates()[0];
  BOOST_CHECK_CLOSE(static_cast<double>(hsubordinate1.getMetaValue("peak_apex_int")), 266403.0, 1e-6);
  BOOST_CHECK_EQUAL(hsubordinate1.getMetaValue("native_id").toString(), "23dpg.23dpg_1.Heavy");
  BOOST_CHECK_CLOSE(static_cast<double>(hsubordinate1.getRT()), 953.665693772912, 1e-6);
  BOOST_CHECK(hsubordinate1.getMetaValue("used_").toBool());

  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureMapHistory()[50].getSubordinates().size(), 2);

  const OpenMS::Feature& hsubordinate2 = rawDataHandler.getFeatureMapHistory()[50].getSubordinates()[0];
  BOOST_CHECK_CLOSE(static_cast<double>(hsubordinate2.getMetaValue("peak_apex_int")), 0.0, 1e-6);
  BOOST_CHECK_EQUAL(hsubordinate2.getMetaValue("native_id").toString(), "accoa.accoa_1.Heavy");
  BOOST_CHECK_CLOSE(static_cast<double>(hsubordinate2.getRT()), 1067.5447296543123, 1e-6);
  BOOST_CHECK(hsubordinate1.getMetaValue("used_").toBool());
}

BOOST_AUTO_TEST_CASE(selectFeatures)
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

  const string featureXML_o = SMARTPEAK_GET_TEST_DATA_PATH("RawDataProcessor_test_2_core_RawDataProcessor.featureXML");
  OpenMSFile::loadFeatureMap(rawDataHandler, featureXML_o);

  RawDataProcessor::selectFeatures(
    rawDataHandler,
    params_1.at("MRMFeatureSelector.select_MRMFeatures_qmip"),
    params_1.at("MRMFeatureSelector.schedule_MRMFeatures_qmip")
  );

  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureMap().size(), 114); // Test feature_map
  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureMap()[0].getSubordinates().size(), 3);

  const OpenMS::Feature& subordinate1 = rawDataHandler.getFeatureMap()[0].getSubordinates()[0];
  BOOST_CHECK_CLOSE(static_cast<double>(subordinate1.getMetaValue("peak_apex_int")), 266403.0, 1e-6);
  BOOST_CHECK_EQUAL(subordinate1.getMetaValue("native_id").toString(), "23dpg.23dpg_1.Heavy");
  BOOST_CHECK_CLOSE(static_cast<double>(subordinate1.getRT()), 953.66569377291205, 1e-6);

  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureMap()[50].getSubordinates().size(), 2);

  const OpenMS::Feature& subordinate2 = rawDataHandler.getFeatureMap()[50].getSubordinates()[0];
  BOOST_CHECK_CLOSE(static_cast<double>(subordinate2.getMetaValue("peak_apex_int")), 4132.0, 1e-6);
  BOOST_CHECK_EQUAL(subordinate2.getMetaValue("native_id").toString(), "f6p.f6p_1.Heavy");
  BOOST_CHECK_CLOSE(static_cast<double>(subordinate2.getRT()), 439.165833, 1e-6);

  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureMapHistory().size(), 329); // Test feature_map_history
  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureMapHistory()[0].getSubordinates().size(), 3);

  const OpenMS::Feature& hsubordinate1 = rawDataHandler.getFeatureMapHistory()[0].getSubordinates()[0];
  BOOST_CHECK_CLOSE(static_cast<double>(hsubordinate1.getMetaValue("peak_apex_int")), 266403.0, 1e-6);
  BOOST_CHECK_EQUAL(hsubordinate1.getMetaValue("native_id").toString(), "23dpg.23dpg_1.Heavy");
  BOOST_CHECK_CLOSE(static_cast<double>(hsubordinate1.getRT()), 953.66569377291205, 1e-6);
  BOOST_CHECK(hsubordinate1.getMetaValue("used_").toBool());

  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureMapHistory()[50].getSubordinates().size(), 2);

  const OpenMS::Feature& hsubordinate2 = rawDataHandler.getFeatureMapHistory()[50].getSubordinates()[0];
  BOOST_CHECK_CLOSE(static_cast<double>(hsubordinate2.getMetaValue("peak_apex_int")), 49333.0, 1e-6);
  BOOST_CHECK_EQUAL(hsubordinate2.getMetaValue("native_id").toString(), "arg-L.arg-L_1.Heavy");
  BOOST_CHECK_CLOSE(static_cast<double>(hsubordinate2.getRT()), 46.652168337345103, 1e-6);
  BOOST_CHECK(hsubordinate1.getMetaValue("used_").toBool());
}

BOOST_AUTO_TEST_CASE(validateFeatures)
{
  map<string, vector<map<string, string>>> params_1;
  map<string, vector<map<string, string>>> params_2;
  load_data(params_1, params_2);
  RawDataHandler rawDataHandler;

  const string featureXML_o = SMARTPEAK_GET_TEST_DATA_PATH("RawDataProcessor_test_3_core_RawDataProcessor.featureXML");
  OpenMSFile::loadFeatureMap(rawDataHandler, featureXML_o);

  const string referenceData_csv_i = SMARTPEAK_GET_TEST_DATA_PATH("MRMFeatureValidator_referenceData_1.csv");
  OpenMSFile::loadValidationData(rawDataHandler, referenceData_csv_i);

  MetaDataHandler& mdh = rawDataHandler.getMetaData();
  mdh.sample_name = "150601_0_BloodProject01_PLT_QC_Broth-1"; // info taken from .csv file
  mdh.inj_number = 1;
  mdh.batch_name = "BloodProject01";
  mdh.setAcquisitionDateAndTimeFromString("09-06-2015 17:14", "%m-%d-%Y %H:%M");

  RawDataProcessor::validateFeatures(rawDataHandler, params_1.at("MRMFeatureValidator.validate_MRMFeatures"), true);

  const std::map<std::string, float>& validation_metrics = rawDataHandler.getValidationMetrics();
  // Confusion matrix: [TP, FP, FN, TN] = [0, 155, 0, 0]
  BOOST_CHECK_CLOSE(validation_metrics.at("accuracy"), 0.0, 1e-3);
  BOOST_CHECK_CLOSE(validation_metrics.at("precision"), 0.0, 1e-3);
  BOOST_CHECK_EQUAL(std::isnan(validation_metrics.at("recall")), true);
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

  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureMapHistory().size(), 98);
  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureMapHistory()[0].getSubordinates().size(), 3);

  const OpenMS::Feature& hsub = rawDataHandler.getFeatureMapHistory()[0].getSubordinates()[1];
  BOOST_CHECK_EQUAL(hsub.getMetaValue("native_id").toString(), "23dpg.23dpg_1.Light");
  BOOST_CHECK_CLOSE(static_cast<double>(hsub.getMetaValue("calculated_concentration")), 0.44335812456518986, 1e-6);
  BOOST_CHECK_EQUAL(hsub.getMetaValue("concentration_units").toString(), "uM");
  BOOST_CHECK(hsub.getMetaValue("used_").toBool());
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

  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureMapHistory().size(), 98);
  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureMapHistory()[0].getSubordinates().size(), 3);

  const OpenMS::Feature& hsub = rawDataHandler.getFeatureMapHistory()[0].getSubordinates()[1];
  BOOST_CHECK_EQUAL(hsub.getMetaValue("native_id").toString(), "23dpg.23dpg_1.Light");
  BOOST_CHECK_EQUAL(hsub.getMetaValue("QC_transition_pass").toString(), "1");
  BOOST_CHECK(hsub.getMetaValue("used_").toBool());
}

BOOST_AUTO_TEST_CASE(getDefaultRawDataProcessingWorkflow)
{
  std::vector<RawDataProcessor::RawDataProcMethod> workflow;

  workflow = RawDataProcessor::getDefaultRawDataProcessingWorkflow(
    MetaDataHandler::SampleType::Unknown
  );

  BOOST_CHECK_EQUAL(workflow.size(), 6);
  BOOST_CHECK_EQUAL(workflow[0], RawDataProcessor::LOAD_RAW_DATA);
  BOOST_CHECK_EQUAL(workflow[1], RawDataProcessor::PICK_FEATURES);
  BOOST_CHECK_EQUAL(workflow[2], RawDataProcessor::FILTER_FEATURES);
  BOOST_CHECK_EQUAL(workflow[3], RawDataProcessor::SELECT_FEATURES);
  BOOST_CHECK_EQUAL(workflow[4], RawDataProcessor::QUANTIFY_FEATURES);
  BOOST_CHECK_EQUAL(workflow[5], RawDataProcessor::CHECK_FEATURES);

  workflow = RawDataProcessor::getDefaultRawDataProcessingWorkflow(
    MetaDataHandler::SampleType::DoubleBlank
  );

  BOOST_CHECK_EQUAL(workflow.size(), 5);
  BOOST_CHECK_EQUAL(workflow[0], RawDataProcessor::LOAD_RAW_DATA);
  BOOST_CHECK_EQUAL(workflow[1], RawDataProcessor::PICK_FEATURES);
  BOOST_CHECK_EQUAL(workflow[2], RawDataProcessor::FILTER_FEATURES);
  BOOST_CHECK_EQUAL(workflow[3], RawDataProcessor::SELECT_FEATURES);
  BOOST_CHECK_EQUAL(workflow[4], RawDataProcessor::CHECK_FEATURES);
}

BOOST_AUTO_TEST_CASE(process)
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

  std::vector<RawDataProcessor::RawDataProcMethod> raw_data_processing_events;

  raw_data_processing_events = RawDataProcessor::getDefaultRawDataProcessingWorkflow(
    MetaDataHandler::SampleType::Unknown
  );

  Filenames filenames;
  filenames.mzML_i = SMARTPEAK_GET_TEST_DATA_PATH("RawDataProcessor_mzML_1.mzML");

  map<string, vector<map<string, string>>>::iterator it = params_1.find("ChromatogramExtractor");
  if (it != params_1.end()) {
    params_1.erase(it);
  }
  params_1.emplace("ChromatogramExtractor", vector<map<string, string>>());

  for (const RawDataProcessor::RawDataProcMethod event : raw_data_processing_events) {
    RawDataProcessor::process(rawDataHandler, event, params_1, filenames);
  }

  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureMap().size(), 114); // test feature_map
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
  BOOST_CHECK_CLOSE(static_cast<double>(subordinate2.getMetaValue("peak_apex_int")), 2780997.0, 1e-6);
  BOOST_CHECK_EQUAL(subordinate2.getMetaValue("native_id").toString(), "Pool_2pg_3pg.Pool_2pg_3pg_1.Heavy");
  BOOST_CHECK_CLOSE(static_cast<double>(subordinate2.getRT()), 832.69577069044112, 1e-6);

  const OpenMS::Feature& subordinate3 = rawDataHandler.getFeatureMap()[49].getSubordinates()[0];
  BOOST_CHECK_CLOSE(static_cast<double>(subordinate3.getMetaValue("peak_apex_int")), 217684.0, 1e-6);
  BOOST_CHECK_EQUAL(subordinate3.getMetaValue("native_id").toString(), "f1p.f1p_1.Heavy");
  BOOST_CHECK_CLOSE(static_cast<double>(subordinate3.getRT()), 602.45962461757676, 1e-6);

  const OpenMS::Feature& subordinate4 = rawDataHandler.getFeatureMap()[113].getSubordinates()[0]; // this is [49][0] in python tests
  BOOST_CHECK_CLOSE(static_cast<double>(subordinate4.getMetaValue("peak_apex_int")), 4066240.0, 1e-6);
  BOOST_CHECK_EQUAL(subordinate4.getMetaValue("native_id").toString(), "xan.xan_1.Heavy");
  BOOST_CHECK_CLOSE(static_cast<double>(subordinate4.getRT()), 89.418783654689776, 1e-6);

  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureMapHistory().size(), 481); // test feature_map_history
  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureMapHistory()[0].getSubordinates().size(), 3);

  const OpenMS::Feature& hsubordinate1 = rawDataHandler.getFeatureMapHistory()[0].getSubordinates()[1];
  BOOST_CHECK_EQUAL(hsubordinate1.getMetaValue("native_id").toString(), "23dpg.23dpg_1.Light");
  BOOST_CHECK_CLOSE(static_cast<double>(hsubordinate1.getMetaValue("calculated_concentration")), 0.44335812456518986, 1e-6);
  BOOST_CHECK_EQUAL(hsubordinate1.getMetaValue("concentration_units").toString(), "uM");
  BOOST_CHECK_CLOSE(static_cast<double>(hsubordinate1.getRT()), 953.665693772912, 1e-6);
  BOOST_CHECK_EQUAL(hsubordinate1.getMetaValue("QC_transition_pass").toString(), "1");
  BOOST_CHECK(hsubordinate1.getMetaValue("used_").toBool());

  const OpenMS::Feature& hsubordinate2 = rawDataHandler.getFeatureMapHistory()[50].getSubordinates()[0];
  BOOST_CHECK_CLOSE(static_cast<double>(hsubordinate2.getMetaValue("peak_apex_int")), 0.0, 1e-6);
  BOOST_CHECK_EQUAL(hsubordinate2.getMetaValue("native_id").toString(), "accoa.accoa_1.Heavy");
  BOOST_CHECK_CLOSE(static_cast<double>(hsubordinate2.getRT()), 1067.5447296543123, 1e-6);
  BOOST_CHECK(!hsubordinate2.getMetaValue("used_").toBool());
}

BOOST_AUTO_TEST_SUITE_END()
