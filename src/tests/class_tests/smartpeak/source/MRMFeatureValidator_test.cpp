// TODO: Add copyright

#include <SmartPeak/test_config.h>

#define BOOST_TEST_MODULE MRMFeatureValidator test suite
#include <boost/test/included/unit_test.hpp>
#include <SmartPeak/core/MRMFeatureValidator.h>

#ifndef CSV_IO_NO_THREAD
#define CSV_IO_NO_THREAD
#endif
#include <SmartPeak/io/csv.h>
#include <OpenMS/FORMAT/FeatureXMLFile.h>
#include <SmartPeak/io/FileReader.h>

using namespace SmartPeak;
using namespace std;

BOOST_AUTO_TEST_SUITE(mrmfeaturevalidator)

BOOST_AUTO_TEST_CASE(validate_MRMFeatures)
{

  const string referenceData_csv_i = SMARTPEAK_GET_TEST_DATA_PATH("MRMFeatureValidator_referenceData_1.csv");
  const string featureXML_i = SMARTPEAK_GET_TEST_DATA_PATH("MRMFeatureValidator_test_1_algorithm_MRMFeatureValidator.featureXML");
  const string filename_params = SMARTPEAK_GET_TEST_DATA_PATH("MRMFeatureValidator_test_pyTOPP_MRMFeatureValidator_params.csv");

  io::CSVReader<2, io::trim_chars<>> in(referenceData_csv_i);
  in.read_header(io::ignore_extra_column, "component_name", "retention_time");
  map<string,float> reference_data;
  std::string component_name;
  float retention_time;
  while(in.read_row(component_name, retention_time)) {
    reference_data.emplace(component_name, retention_time);
  }

  OpenMS::FeatureXMLFile featurexml;
  OpenMS::FeatureMap featureMap;
  featurexml.load(featureXML_i, featureMap);

  std::map<std::string,std::vector<std::map<std::string,std::string>>> params;
  FileReader::parseOpenMSParams(filename_params, params); // it is assumed "," as delimiter


  OpenMS::FeatureMap output_filtered;
  std::map<std::string, float> validation_metrics;

  const float Tr_window = std::stof(params.at("MRMFeatureValidator.validate_MRMFeatures").at(0).at("value"));

  MRMFeatureValidator::validate_MRMFeatures(
    reference_data,
    featureMap,
    output_filtered,
    validation_metrics,
    Tr_window
  );

  BOOST_CHECK_CLOSE(validation_metrics.at("accuracy"), static_cast<float>(0.98709677419354835), 1e-6);
  BOOST_CHECK_CLOSE(validation_metrics.at("recall"), static_cast<float>(0.98709677419354835), 1e-6);
  BOOST_CHECK_CLOSE(validation_metrics.at("precision"), static_cast<float>(1.0), 1e-6);
}

BOOST_AUTO_TEST_SUITE_END()
