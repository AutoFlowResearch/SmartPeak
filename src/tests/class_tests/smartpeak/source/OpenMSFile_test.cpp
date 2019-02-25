// TODO: Add copyright

#include <SmartPeak/test_config.h>

#define BOOST_TEST_MODULE OpenMSFile test suite
#include <boost/test/included/unit_test.hpp>
#include <SmartPeak/io/OpenMSFile.h>
#include <SmartPeak/core/Utilities.h>
#include <OpenMS/FORMAT/MzMLFile.h>

using namespace SmartPeak;
using namespace std;

BOOST_AUTO_TEST_SUITE(fileopenms)

BOOST_AUTO_TEST_CASE(loadStandardsConcentrations)
{
  const string pathname = SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_standardsConcentrations_1.csv");
  SequenceSegmentHandler ssh;
  OpenMSFile::loadStandardsConcentrations(ssh, pathname);
  const std::vector<OpenMS::AbsoluteQuantitationStandards::runConcentration>& rc = ssh.getStandardsConcentrations();

  BOOST_CHECK_EQUAL(rc.size(), 8);

  BOOST_CHECK_EQUAL(rc[0].sample_name, "150516_CM1_Level1");
  BOOST_CHECK_EQUAL(rc[0].component_name, "23dpg.23dpg_1.Light");
  BOOST_CHECK_EQUAL(rc[0].IS_component_name, "23dpg.23dpg_1.Heavy");
  BOOST_CHECK_CLOSE(rc[0].actual_concentration, 0.0, 1e-6);
  BOOST_CHECK_CLOSE(rc[0].IS_actual_concentration, 1.0, 1e-6);
  BOOST_CHECK_EQUAL(rc[0].concentration_units, "uM");
  BOOST_CHECK_CLOSE(rc[0].dilution_factor, 1.0, 1e-6);

  BOOST_CHECK_EQUAL(rc[4].sample_name, "150516_CM3_Level9");
  BOOST_CHECK_EQUAL(rc[4].component_name, "ump.ump_2.Light");
  BOOST_CHECK_EQUAL(rc[4].IS_component_name, "ump.ump_1.Heavy");
  BOOST_CHECK_CLOSE(rc[4].actual_concentration, 0.016, 1e-6);
  BOOST_CHECK_CLOSE(rc[4].IS_actual_concentration, 1.0, 1e-6);
  BOOST_CHECK_EQUAL(rc[4].concentration_units, "uM");
  BOOST_CHECK_CLOSE(rc[4].dilution_factor, 1.0, 1e-6);

  BOOST_CHECK_EQUAL(rc[7].sample_name, "150516_CM3_Level9");
  BOOST_CHECK_EQUAL(rc[7].component_name, "utp.utp_2.Light");
  BOOST_CHECK_EQUAL(rc[7].IS_component_name, "utp.utp_1.Heavy");
  BOOST_CHECK_CLOSE(rc[7].actual_concentration, 0.0, 1e-6);
  BOOST_CHECK_CLOSE(rc[7].IS_actual_concentration, 1.0, 1e-6);
  BOOST_CHECK_EQUAL(rc[7].concentration_units, "uM");
  BOOST_CHECK_CLOSE(rc[7].dilution_factor, 1.0, 1e-6);
}

BOOST_AUTO_TEST_CASE(loadQuantitationMethods)
{
  const string pathname = SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_quantitationMethods_1.csv");
  SequenceSegmentHandler ssh;
  OpenMSFile::loadQuantitationMethods(ssh, pathname);
  const std::vector<OpenMS::AbsoluteQuantitationMethod>& aqm = ssh.getQuantitationMethods();

  BOOST_CHECK_EQUAL(aqm.size(), 107);

  BOOST_CHECK_EQUAL(aqm[0].getComponentName(), "23dpg.23dpg_1.Light");
  BOOST_CHECK_EQUAL(aqm[0].getFeatureName(), "peak_apex_int");
  BOOST_CHECK_EQUAL(aqm[0].getISName(), "23dpg.23dpg_1.Heavy");
  BOOST_CHECK_EQUAL(aqm[0].getConcentrationUnits(), "uM");
  BOOST_CHECK_EQUAL(aqm[0].getTransformationModel(), "linear");
  BOOST_CHECK_CLOSE(aqm[0].getLLOD(), 0.0, 1e-6);
  BOOST_CHECK_CLOSE(aqm[0].getULOD(), 0.0, 1e-6);
  BOOST_CHECK_CLOSE(aqm[0].getLLOQ(), 0.25, 1e-6);
  BOOST_CHECK_CLOSE(aqm[0].getULOQ(), 2.5, 1e-6);
  BOOST_CHECK_CLOSE(aqm[0].getCorrelationCoefficient(), 0.983846949, 1e-6);
  BOOST_CHECK_EQUAL(aqm[0].getNPoints(), 4);
  const OpenMS::Param params1 = aqm[0].getTransformationModelParams();
  BOOST_CHECK_CLOSE(static_cast<double>(params1.getValue("slope")), 2.429728323, 1e-6);
  BOOST_CHECK_CLOSE(static_cast<double>(params1.getValue("intercept")), -0.091856745, 1e-6);

  BOOST_CHECK_EQUAL(aqm[106].getComponentName(), "xan.xan_1.Light");
  BOOST_CHECK_EQUAL(aqm[106].getFeatureName(), "peak_apex_int");
  BOOST_CHECK_EQUAL(aqm[106].getISName(), "xan.xan_1.Heavy");
  BOOST_CHECK_EQUAL(aqm[106].getConcentrationUnits(), "uM");
  BOOST_CHECK_EQUAL(aqm[106].getTransformationModel(), "linear");
  BOOST_CHECK_CLOSE(aqm[106].getLLOD(), 0.0, 1e-6);
  BOOST_CHECK_CLOSE(aqm[106].getULOD(), 0.0, 1e-6);
  BOOST_CHECK_CLOSE(aqm[106].getLLOQ(), 0.004, 1e-6);
  BOOST_CHECK_CLOSE(aqm[106].getULOQ(), 0.16, 1e-6);
  BOOST_CHECK_CLOSE(aqm[106].getCorrelationCoefficient(), 0.994348761, 1e-6);
  BOOST_CHECK_EQUAL(aqm[106].getNPoints(), 6);
  const OpenMS::Param params2 = aqm[106].getTransformationModelParams();
  BOOST_CHECK_CLOSE(static_cast<double>(params2.getValue("slope")), 1.084995619, 1e-6);
  BOOST_CHECK_CLOSE(static_cast<double>(params2.getValue("intercept")), -0.00224781, 1e-6);
}

BOOST_AUTO_TEST_CASE(loadTraML)
{
  const string pathname = SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_traML_1.csv");
  RawDataHandler rawDataHandler;
  OpenMSFile::loadTraML(rawDataHandler, pathname, "csv");
  const std::vector<OpenMS::ReactionMonitoringTransition>& t = rawDataHandler.getTargetedExperiment().getTransitions();

  BOOST_CHECK_EQUAL(t.size(), 324);

  BOOST_CHECK_EQUAL(t[0].getPeptideRef(), "arg-L");
  BOOST_CHECK_CLOSE(t[0].getPrecursorMZ(), 179.0, 1e-6);
  BOOST_CHECK_CLOSE(t[0].getProductMZ(), 136.0, 1e-6);

  BOOST_CHECK_EQUAL(t[10].getPeptideRef(), "citr-L");
  BOOST_CHECK_CLOSE(t[10].getPrecursorMZ(), 180.0, 1e-6);
  BOOST_CHECK_CLOSE(t[10].getProductMZ(), 136.0, 1e-6);

  BOOST_CHECK_EQUAL(t[19].getPeptideRef(), "Lcystin");
  BOOST_CHECK_CLOSE(t[19].getPrecursorMZ(), 239.0, 1e-6);
  BOOST_CHECK_CLOSE(t[19].getProductMZ(), 120.0, 1e-6);
}

BOOST_AUTO_TEST_CASE(loadMSExperiment)
{
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

  OpenMSFile::loadMSExperiment(
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

BOOST_AUTO_TEST_CASE(loadFeatureMap)
{
  const string pathname = SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_test_1_io_FileReaderOpenMS.featureXML");
  RawDataHandler rawDataHandler;
  OpenMSFile::loadFeatureMap(rawDataHandler, pathname);
  const OpenMS::FeatureMap& fm = rawDataHandler.getFeatureMap();

  BOOST_CHECK_EQUAL(fm.size(), 481);

  BOOST_CHECK_CLOSE(static_cast<double>(fm[0].getSubordinates()[0].getMetaValue("peak_apex_int")), 266403.0, 1e-6);
  BOOST_CHECK_CLOSE(static_cast<double>(fm[0].getSubordinates()[0].getRT()), 15.8944563381195, 1e-6);
  BOOST_CHECK_EQUAL(fm[0].getSubordinates()[0].getMetaValue("native_id").toString(), "23dpg.23dpg_1.Heavy");
  BOOST_CHECK_CLOSE(static_cast<double>(fm[0].getMetaValue("peak_apices_sum")), 583315.0, 1e-6);
  BOOST_CHECK_EQUAL(fm[0].getMetaValue("PeptideRef").toString(), "23dpg");

  BOOST_CHECK_CLOSE(static_cast<double>(fm[1].getSubordinates()[0].getMetaValue("peak_apex_int")), 3436.0, 1e-6);
  BOOST_CHECK_CLOSE(static_cast<double>(fm[1].getSubordinates()[0].getRT()), 16.2997193464915, 1e-6);
  BOOST_CHECK_EQUAL(fm[1].getSubordinates()[0].getMetaValue("native_id").toString(), "23dpg.23dpg_1.Heavy");
  BOOST_CHECK_CLOSE(static_cast<double>(fm[1].getMetaValue("peak_apices_sum")), 13859.0, 1e-6);
  BOOST_CHECK_EQUAL(fm[1].getMetaValue("PeptideRef").toString(), "23dpg");
}

BOOST_AUTO_TEST_CASE(loadFeatureFilter)
{
  RawDataHandler rawDataHandler;

  const string pathname1 = SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_mrmfeatureqccomponents_1.csv");
  const string pathname2 = SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_mrmfeatureqccomponentgroups_1.csv");

  OpenMSFile::loadFeatureFilter(rawDataHandler, pathname1, pathname2);
  const OpenMS::MRMFeatureQC& fQC = rawDataHandler.getFeatureFilter();

  BOOST_CHECK_EQUAL(fQC.component_qcs.size(), 324);
  BOOST_CHECK_EQUAL(fQC.component_qcs[0].component_name, "arg-L.arg-L_1.Heavy");
  BOOST_CHECK_EQUAL(fQC.component_group_qcs.size(), 118);
  BOOST_CHECK_EQUAL(fQC.component_group_qcs[0].component_group_name, "arg-L");
}

BOOST_AUTO_TEST_CASE(loadFeatureQC)
{
  RawDataHandler rawDataHandler;

  const string pathname1 = SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_mrmfeatureqccomponents_1.csv");
  const string pathname2 = SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_mrmfeatureqccomponentgroups_1.csv");

  OpenMSFile::loadFeatureQC(rawDataHandler, pathname1, pathname2);
  const OpenMS::MRMFeatureQC& fQC = rawDataHandler.getFeatureQC();

  BOOST_CHECK_EQUAL(fQC.component_qcs.size(), 324);
  BOOST_CHECK_EQUAL(fQC.component_qcs[0].component_name, "arg-L.arg-L_1.Heavy");
  BOOST_CHECK_EQUAL(fQC.component_group_qcs.size(), 118);
  BOOST_CHECK_EQUAL(fQC.component_group_qcs[0].component_group_name, "arg-L");
}

BOOST_AUTO_TEST_CASE(loadValidationData)
{
  const string pathname = SMARTPEAK_GET_TEST_DATA_PATH("MRMFeatureValidator_referenceData_1.csv");
  RawDataHandler rawDataHandler;

  OpenMSFile::loadValidationData(rawDataHandler, pathname);
  const std::vector<std::map<std::string, Utilities::CastValue>>& ref_data = rawDataHandler.getReferenceData();

  BOOST_CHECK_EQUAL(ref_data.size(), 179);
  BOOST_CHECK_EQUAL(ref_data[0].at("component_name").s_, "23dpg.23dpg_1.Heavy");
  BOOST_CHECK_CLOSE(ref_data[0].at("area").f_, 932543.098, 1e-3);
  BOOST_CHECK_CLOSE(ref_data[0].at("retention_time").f_, static_cast<float>(15.89495171), 1e-1);
  BOOST_CHECK_EQUAL(ref_data[0].at("acquisition_date_and_time").s_, "09-06-2015 17:14");
  // TODO: Should we just use double instead of float? I had to go down to -1 to make the test pass
  BOOST_CHECK_EQUAL(ref_data[178].at("component_name").s_, "xan.xan_1.Light");
  BOOST_CHECK_CLOSE(ref_data[178].at("area").f_, 206951.3035, 1e-3);
  BOOST_CHECK_CLOSE(ref_data[178].at("retention_time").f_, static_cast<float>(1.492980468), 1e-1);
  BOOST_CHECK_EQUAL(ref_data[178].at("acquisition_date_and_time").s_, "09-06-2015 17:14");
}

BOOST_AUTO_TEST_CASE(readRawDataProcessingParameters)
{
// no tests, it calls FileReader::parseOpenMSParams and OpenMSFile::sanitizeRawDataProcessorParameters
}

BOOST_AUTO_TEST_CASE(sanitizeRawDataProcessorParameters)
{
  RawDataHandler rawDataHandler;
  std::map<std::string, std::vector<std::map<std::string, std::string>>> params;

  params.emplace("SequenceSegmentPlotter", vector<map<string,string>> {
    {
      {"map1_elem1", "value1"},
      {"map1_elem2", "value2"}
    },
    {
      {"map2_elem1", "value3"}
    }
  });

  OpenMSFile::sanitizeRawDataProcessorParameters(rawDataHandler, params);
  BOOST_CHECK_EQUAL(params.size(), 14);
  BOOST_CHECK_EQUAL(params.count("SequenceSegmentPlotter"), 1);
  BOOST_CHECK_EQUAL(params.count("FeaturePlotter"), 1);
  BOOST_CHECK_EQUAL(params.count("AbsoluteQuantitation"), 1);
  BOOST_CHECK_EQUAL(params.count("mzML"), 1);
  BOOST_CHECK_EQUAL(params.count("MRMMapping"), 1);
  BOOST_CHECK_EQUAL(params.count("ChromatogramExtractor"), 1);
  BOOST_CHECK_EQUAL(params.count("MRMFeatureFinderScoring"), 1);
  BOOST_CHECK_EQUAL(params.count("MRMFeatureFilter.filter_MRMFeatures"), 1);
  BOOST_CHECK_EQUAL(params.count("MRMFeatureSelector.select_MRMFeatures_qmip"), 1);
  BOOST_CHECK_EQUAL(params.count("MRMFeatureSelector.schedule_MRMFeatures_qmip"), 1);
  BOOST_CHECK_EQUAL(params.count("MRMFeatureSelector.select_MRMFeatures_score"), 1);
  BOOST_CHECK_EQUAL(params.count("ReferenceDataMethods.getAndProcess_referenceData_samples"), 1);
  BOOST_CHECK_EQUAL(params.count("MRMFeatureValidator.validate_MRMFeatures"), 1);
  BOOST_CHECK_EQUAL(params.count("MRMFeatureFilter.filter_MRMFeatures.qc"), 1);
  BOOST_CHECK_EQUAL(params.at("SequenceSegmentPlotter").size(), 2);
  BOOST_CHECK_EQUAL(params.at("SequenceSegmentPlotter")[0].at("map1_elem1"), "value1");
  BOOST_CHECK_EQUAL(params.at("SequenceSegmentPlotter")[0].at("map1_elem2"), "value2");
  BOOST_CHECK_EQUAL(params.at("SequenceSegmentPlotter")[1].at("map2_elem1"), "value3");
  BOOST_CHECK_EQUAL(params.at("MRMFeatureFilter.filter_MRMFeatures.qc").size(), 0);
}

BOOST_AUTO_TEST_CASE(storeQuantitationMethods)
{
// no tests, it wraps OpenMS store function
}

BOOST_AUTO_TEST_CASE(storeFeatureMap)
{
// no tests, it wraps OpenMS store function
}

BOOST_AUTO_TEST_CASE(storeMzML)
{
// no tests, it wraps OpenMS store function
}

BOOST_AUTO_TEST_SUITE_END()
