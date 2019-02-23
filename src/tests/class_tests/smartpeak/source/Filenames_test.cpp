// TODO: Add copyright

#include <SmartPeak/test_config.h>

#define BOOST_TEST_MODULE Filenames test suite
#include <boost/test/included/unit_test.hpp>
#include <SmartPeak/core/Filenames.h>

using namespace SmartPeak;
using namespace std;

BOOST_AUTO_TEST_SUITE(filenames)

BOOST_AUTO_TEST_CASE(getDefaultStaticFilenames)
{
  Filenames filenames = Filenames::getDefaultStaticFilenames("/home/user");
  BOOST_CHECK_EQUAL(filenames.sequence_csv_i, "/home/user/sequence.csv");
  BOOST_CHECK_EQUAL(filenames.parameters_csv_i, "/home/user/parameters.csv");
  BOOST_CHECK_EQUAL(filenames.traML_csv_i, "/home/user/traML.csv");
  BOOST_CHECK_EQUAL(filenames.featureFilterComponents_csv_i, "/home/user/featureFilterComponents.csv");
  BOOST_CHECK_EQUAL(filenames.featureFilterComponentGroups_csv_i, "/home/user/featureFilterComponentGroups.csv");
  BOOST_CHECK_EQUAL(filenames.featureQCComponents_csv_i, "/home/user/featureQCComponents.csv");
  BOOST_CHECK_EQUAL(filenames.featureQCComponentGroups_csv_i, "/home/user/featureQCComponentGroups.csv");
  BOOST_CHECK_EQUAL(filenames.quantitationMethods_csv_i, "/home/user/quantitationMethods.csv");
  BOOST_CHECK_EQUAL(filenames.standardsConcentrations_csv_i, "/home/user/standardsConcentrations.csv");
  BOOST_CHECK_EQUAL(filenames.referenceData_csv_i, "/home/user/referenceData.csv");
  BOOST_CHECK_EQUAL(filenames.mzML_i, "");
  BOOST_CHECK_EQUAL(filenames.featureXML_o, "");
  BOOST_CHECK_EQUAL(filenames.feature_csv_o, "");
  BOOST_CHECK_EQUAL(filenames.featureXML_i, "");
  BOOST_CHECK_EQUAL(filenames.features_pdf_o, "");
  BOOST_CHECK_EQUAL(filenames.quantitationMethods_csv_o, "");
  BOOST_CHECK_EQUAL(filenames.componentsToConcentrations_csv_o, "");
  BOOST_CHECK_EQUAL(filenames.sequenceSummary_csv_o, "/home/user/SequenceSummary.csv");
  BOOST_CHECK_EQUAL(filenames.featureSummary_csv_o, "/home/user/FeatureSummary.csv");
}

BOOST_AUTO_TEST_CASE(getDefaultDynamicFilenames)
{
  Filenames filenames = Filenames::getDefaultDynamicFilenames(
    "/home/user/mzML",
    "/home/user/featuresIn", 
    "/home/user/featuresOut", 
    "injIn",
    "injOut");
  BOOST_CHECK_EQUAL(filenames.sequence_csv_i, "");
  BOOST_CHECK_EQUAL(filenames.parameters_csv_i, "");
  BOOST_CHECK_EQUAL(filenames.traML_csv_i, "");
  BOOST_CHECK_EQUAL(filenames.featureFilterComponents_csv_i, "");
  BOOST_CHECK_EQUAL(filenames.featureFilterComponentGroups_csv_i, "");
  BOOST_CHECK_EQUAL(filenames.featureQCComponents_csv_i, "");
  BOOST_CHECK_EQUAL(filenames.featureQCComponentGroups_csv_i, "");
  BOOST_CHECK_EQUAL(filenames.quantitationMethods_csv_i, "");
  BOOST_CHECK_EQUAL(filenames.standardsConcentrations_csv_i, "");
  BOOST_CHECK_EQUAL(filenames.referenceData_csv_i, "");
  BOOST_CHECK_EQUAL(filenames.mzML_i, "/home/user/mzML/injIn.mzML");
  BOOST_CHECK_EQUAL(filenames.featureXML_o, "/home/user/featuresOut/injOut.featureXML");
  BOOST_CHECK_EQUAL(filenames.feature_csv_o, "/home/user/featuresOut/injOut.csv");
  BOOST_CHECK_EQUAL(filenames.featureXML_i, "/home/user/featuresIn/injOut.featureXML");
  BOOST_CHECK_EQUAL(filenames.features_pdf_o, "/home/user/featuresOut/injOut");
  BOOST_CHECK_EQUAL(filenames.quantitationMethods_csv_o, "/home/user/featuresOut/injOut_quantitationMethods.csv");
  BOOST_CHECK_EQUAL(filenames.componentsToConcentrations_csv_o, "/home/user/featuresOut/injOut_componentsToConcentrations.csv");
  BOOST_CHECK_EQUAL(filenames.sequenceSummary_csv_o, "");
  BOOST_CHECK_EQUAL(filenames.featureSummary_csv_o, "");
}

BOOST_AUTO_TEST_CASE(clear1)
{
  Filenames filenames = Filenames::getDefaultStaticFilenames("/home/user");
  filenames.clear();
  BOOST_CHECK_EQUAL(filenames.sequence_csv_i, "");
  BOOST_CHECK_EQUAL(filenames.parameters_csv_i, "");
  BOOST_CHECK_EQUAL(filenames.traML_csv_i, "");
  BOOST_CHECK_EQUAL(filenames.featureFilterComponents_csv_i, "");
  BOOST_CHECK_EQUAL(filenames.featureFilterComponentGroups_csv_i, "");
  BOOST_CHECK_EQUAL(filenames.featureQCComponents_csv_i, "");
  BOOST_CHECK_EQUAL(filenames.featureQCComponentGroups_csv_i, "");
  BOOST_CHECK_EQUAL(filenames.quantitationMethods_csv_i, "");
  BOOST_CHECK_EQUAL(filenames.standardsConcentrations_csv_i, "");
  BOOST_CHECK_EQUAL(filenames.referenceData_csv_i, "");
  BOOST_CHECK_EQUAL(filenames.mzML_i, "");
  BOOST_CHECK_EQUAL(filenames.featureXML_o, "");
  BOOST_CHECK_EQUAL(filenames.feature_csv_o, "");
  BOOST_CHECK_EQUAL(filenames.featureXML_i, "");
  BOOST_CHECK_EQUAL(filenames.features_pdf_o, "");
  BOOST_CHECK_EQUAL(filenames.quantitationMethods_csv_o, "");
  BOOST_CHECK_EQUAL(filenames.componentsToConcentrations_csv_o, "");
  BOOST_CHECK_EQUAL(filenames.sequenceSummary_csv_o, "");
  BOOST_CHECK_EQUAL(filenames.featureSummary_csv_o, "");
}

BOOST_AUTO_TEST_CASE(clear2)
{
  Filenames filenames = Filenames::getDefaultDynamicFilenames(
    "/home/user/mzML",
    "/home/user/featuresIn",
    "/home/user/featuresOut",
    "injIn",
    "injOut");
  filenames.clear();
  BOOST_CHECK_EQUAL(filenames.sequence_csv_i, "");
  BOOST_CHECK_EQUAL(filenames.parameters_csv_i, "");
  BOOST_CHECK_EQUAL(filenames.traML_csv_i, "");
  BOOST_CHECK_EQUAL(filenames.featureFilterComponents_csv_i, "");
  BOOST_CHECK_EQUAL(filenames.featureFilterComponentGroups_csv_i, "");
  BOOST_CHECK_EQUAL(filenames.featureQCComponents_csv_i, "");
  BOOST_CHECK_EQUAL(filenames.featureQCComponentGroups_csv_i, "");
  BOOST_CHECK_EQUAL(filenames.quantitationMethods_csv_i, "");
  BOOST_CHECK_EQUAL(filenames.standardsConcentrations_csv_i, "");
  BOOST_CHECK_EQUAL(filenames.referenceData_csv_i, "");
  BOOST_CHECK_EQUAL(filenames.mzML_i, "");
  BOOST_CHECK_EQUAL(filenames.featureXML_o, "");
  BOOST_CHECK_EQUAL(filenames.feature_csv_o, "");
  BOOST_CHECK_EQUAL(filenames.featureXML_i, "");
  BOOST_CHECK_EQUAL(filenames.features_pdf_o, "");
  BOOST_CHECK_EQUAL(filenames.quantitationMethods_csv_o, "");
  BOOST_CHECK_EQUAL(filenames.componentsToConcentrations_csv_o, "");
  BOOST_CHECK_EQUAL(filenames.sequenceSummary_csv_o, "");
  BOOST_CHECK_EQUAL(filenames.featureSummary_csv_o, "");
}

BOOST_AUTO_TEST_SUITE_END()
