// --------------------------------------------------------------------------
//   SmartPeak -- Fast and Accurate CE-, GC- and LC-MS(/MS) Data Processing
// --------------------------------------------------------------------------
// Copyright The SmartPeak Team -- Novo Nordisk Foundation 
// Center for Biosustainability, Technical University of Denmark 2018-2021.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL ANY OF THE AUTHORS OR THE CONTRIBUTING
// INSTITUTIONS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
// OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
// WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
// OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
// ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// --------------------------------------------------------------------------
// $Maintainer: Douglas McCloskey $
// $Authors: Douglas McCloskey $
// --------------------------------------------------------------------------

#include <SmartPeak/test_config.h>

#define BOOST_TEST_MODULE MRMFeatureValidator test suite
#include <boost/test/included/unit_test.hpp>
#include <SmartPeak/algorithm/MRMFeatureValidator.h>

#include <OpenMS/FORMAT/FeatureXMLFile.h>
#include <SmartPeak/io/ParametersParser.h>
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
  ParametersParser::read(filename_params, params); // it is assumed "," as delimiter
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
  mdh.setSampleName("150601_0_BloodProject01_PLT_QC_Broth-1"); // info taken from .csv file
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
