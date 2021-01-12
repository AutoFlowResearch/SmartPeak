// TODO: Add copyright

#include <SmartPeak/test_config.h>

#define BOOST_TEST_MODULE MRMFeatureValidator test suite
#include <boost/test/included/unit_test.hpp>
#include <SmartPeak/algorithm/MRMFeatureValidator.h>

#include <OpenMS/FORMAT/FeatureXMLFile.h>
#include <SmartPeak/io/FileReader.h>
#include <SmartPeak/core/RawDataHandler.h>
#include <SmartPeak/core/RawDataProcessor.h>

using namespace SmartPeak;
using namespace std;

BOOST_AUTO_TEST_SUITE(mrmfeaturevalidator)

BOOST_AUTO_TEST_CASE(validate_MRMFeatures)
{
  Filenames filenames;
  filenames.referenceData_csv_i = SMARTPEAK_GET_TEST_DATA_PATH("MRMFeatureValidator_referenceData_1.csv");
  const string featureXML_i = SMARTPEAK_GET_TEST_DATA_PATH("MRMFeatureValidator_test_1_algorithm_MRMFeatureValidator.featureXML");
  const string filename_params = SMARTPEAK_GET_TEST_DATA_PATH("MRMFeatureValidator_test_pyTOPP_MRMFeatureValidator_params.csv");

  ParameterSet params;
  FileReader::parseOpenMSParams(filename_params, params); // it is assumed "," as delimiter
  BOOST_CHECK_EQUAL(params.size(), 1);
  BOOST_CHECK_EQUAL(params.count("MRMFeatureValidator.validate_MRMFeatures"), 1);
  BOOST_CHECK_EQUAL(params["MRMFeatureValidator.validate_MRMFeatures"].size(), 1);

  RawDataHandler rawDataHandler;
  LoadValidationData loadValidationData;
  loadValidationData.process(rawDataHandler, {}, filenames);
  const std::vector<std::map<std::string, CastValue>>& ref_data = rawDataHandler.getReferenceData();

  BOOST_CHECK_EQUAL(ref_data.size(), 179);

  OpenMS::FeatureXMLFile featurexml;
  OpenMS::FeatureMap featureMap;
  featurexml.load(featureXML_i, featureMap);

  OpenMS::FeatureMap output_validated;
  std::map<std::string, float> validation_metrics;
  const float Tr_window = std::stof(params.at("MRMFeatureValidator.validate_MRMFeatures")[0].getValueAsString());

  MetaDataHandler mdh;
  mdh.sample_name = "150601_0_BloodProject01_PLT_QC_Broth-1"; // info taken from .csv file
  mdh.inj_number = 1;
  mdh.batch_name = "BloodProject01";
  mdh.setAcquisitionDateAndTimeFromString("09-06-2015 17:14", "%m-%d-%Y %H:%M");

  MRMFeatureValidator::validate_MRMFeatures(
    ref_data,
    featureMap,
    mdh.getInjectionName(),
    output_validated,
    validation_metrics,
    Tr_window
  );

  BOOST_CHECK_CLOSE(validation_metrics.at("accuracy"), 0.987096786, 1e-3);
  BOOST_CHECK_CLOSE(validation_metrics.at("recall"), 1.0, 1e-3);
  BOOST_CHECK_CLOSE(validation_metrics.at("precision"), 0.987096786, 1e-3);
}

BOOST_AUTO_TEST_SUITE_END()
