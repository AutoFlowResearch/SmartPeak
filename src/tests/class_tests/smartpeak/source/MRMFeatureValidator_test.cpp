// TODO: Add copyright

#include <SmartPeak/test_config.h>

#define BOOST_TEST_MODULE MRMFeatureValidator test suite
#include <boost/test/included/unit_test.hpp>
#include <SmartPeak/core/MRMFeatureValidator.h>

#include <OpenMS/FORMAT/FeatureXMLFile.h>
#include <SmartPeak/io/FileReader.h>
#include <SmartPeak/core/RawDataHandler.h>
#include <SmartPeak/io/OpenMSFile.h>

using namespace SmartPeak;
using namespace std;

BOOST_AUTO_TEST_SUITE(mrmfeaturevalidator)

BOOST_AUTO_TEST_CASE(validate_MRMFeatures)
{
  const string referenceData_csv_i = SMARTPEAK_GET_TEST_DATA_PATH("MRMFeatureValidator_referenceData_1.csv");
  const string featureXML_i = SMARTPEAK_GET_TEST_DATA_PATH("MRMFeatureValidator_test_1_algorithm_MRMFeatureValidator.featureXML");
  const string filename_params = SMARTPEAK_GET_TEST_DATA_PATH("MRMFeatureValidator_test_pyTOPP_MRMFeatureValidator_params.csv");

  std::map<std::string,std::vector<std::map<std::string,std::string>>> params;
  FileReader::parseOpenMSParams(filename_params, params); // it is assumed "," as delimiter
  BOOST_CHECK_EQUAL(params.size(), 1);
  BOOST_CHECK_EQUAL(params.count("MRMFeatureValidator.validate_MRMFeatures"), 1);
  BOOST_CHECK_EQUAL(params["MRMFeatureValidator.validate_MRMFeatures"].size(), 1);

  RawDataHandler rawDataHandler;
  OpenMSFile::loadValidationData(rawDataHandler, referenceData_csv_i);
  const std::vector<std::map<std::string, Utilities::CastValue>>& ref_data = rawDataHandler.getReferenceData();

  BOOST_CHECK_EQUAL(ref_data.size(), 179);

  OpenMS::FeatureXMLFile featurexml;
  OpenMS::FeatureMap featureMap;
  featurexml.load(featureXML_i, featureMap);

  OpenMS::FeatureMap output_validated;
  std::map<std::string, float> validation_metrics;
  const float Tr_window = std::stof(params.at("MRMFeatureValidator.validate_MRMFeatures").at(0).at("value"));

  MRMFeatureValidator::validate_MRMFeatures(
    ref_data,
    featureMap,
    "150601_0_BloodProject01_PLT_QC_Broth-1", // info taken from .csv file
    output_validated,
    validation_metrics,
    Tr_window
  );

  BOOST_CHECK_CLOSE(validation_metrics.at("accuracy"), 0.98709677419354835, 1e-3);
  BOOST_CHECK_CLOSE(validation_metrics.at("recall"), 1.0, 1e-3);
  BOOST_CHECK_CLOSE(validation_metrics.at("precision"), 0.98709677419354835, 1e-3);
}

BOOST_AUTO_TEST_SUITE_END()
