// TODO: Add copyright

#include <SmartPeak/test_config.h>

#define BOOST_TEST_MODULE Filenames test suite
#include <boost/test/included/unit_test.hpp>
#include <SmartPeak/core/Filenames.h>

using namespace SmartPeak;
using namespace std;

BOOST_AUTO_TEST_SUITE(filenames)

BOOST_AUTO_TEST_CASE(filenames_getDefaultStaticFilenames)
{
  Filenames filenames = Filenames::getDefaultStaticFilenames("/home/user");
  BOOST_CHECK_EQUAL(filenames.sequence_csv_i, "/home/user/sequence.csv");
  BOOST_CHECK_EQUAL(filenames.parameters_csv_i, "/home/user/parameters.csv");
  BOOST_CHECK_EQUAL(filenames.traML_csv_i, "/home/user/traML.csv");
  BOOST_CHECK_EQUAL(filenames.featureFilterComponents_csv_i, "/home/user/featureFilterComponents.csv");
  BOOST_CHECK_EQUAL(filenames.featureFilterComponentGroups_csv_i, "/home/user/featureFilterComponentGroups.csv");
  BOOST_CHECK_EQUAL(filenames.featureQCComponents_csv_i, "/home/user/featureQCComponents.csv");
  BOOST_CHECK_EQUAL(filenames.featureQCComponentGroups_csv_i, "/home/user/featureQCComponentGroups.csv");
  BOOST_CHECK_EQUAL(filenames.featureRSDFilterComponents_csv_i, "/home/user/featureRSDFilterComponents.csv");
  BOOST_CHECK_EQUAL(filenames.featureRSDFilterComponentGroups_csv_i, "/home/user/featureRSDFilterComponentGroups.csv");
  BOOST_CHECK_EQUAL(filenames.featureRSDQCComponents_csv_i, "/home/user/featureRSDQCComponents.csv");
  BOOST_CHECK_EQUAL(filenames.featureRSDQCComponentGroups_csv_i, "/home/user/featureRSDQCComponentGroups.csv");
  BOOST_CHECK_EQUAL(filenames.featureBackgroundFilterComponents_csv_i, "/home/user/featureBackgroundFilterComponents.csv");
  BOOST_CHECK_EQUAL(filenames.featureBackgroundFilterComponentGroups_csv_i, "/home/user/featureBackgroundFilterComponentGroups.csv");
  BOOST_CHECK_EQUAL(filenames.featureBackgroundQCComponents_csv_i, "/home/user/featureBackgroundQCComponents.csv");
  BOOST_CHECK_EQUAL(filenames.featureBackgroundQCComponentGroups_csv_i, "/home/user/featureBackgroundQCComponentGroups.csv");
  BOOST_CHECK_EQUAL(filenames.featureRSDEstimationComponents_csv_i, "/home/user/featureRSDEstimationComponents.csv");
  BOOST_CHECK_EQUAL(filenames.featureRSDEstimationComponentGroups_csv_i, "/home/user/featureRSDEstimationComponentGroups.csv");
  BOOST_CHECK_EQUAL(filenames.featureRSDQCComponents_csv_i, "/home/user/featureRSDQCComponents.csv");
  BOOST_CHECK_EQUAL(filenames.featureRSDQCComponentGroups_csv_i, "/home/user/featureRSDQCComponentGroups.csv");
  BOOST_CHECK_EQUAL(filenames.quantitationMethods_csv_i, "/home/user/quantitationMethods.csv");
  BOOST_CHECK_EQUAL(filenames.standardsConcentrations_csv_i, "/home/user/standardsConcentrations.csv");
  BOOST_CHECK_EQUAL(filenames.referenceData_csv_i, "/home/user/referenceData.csv");
  BOOST_CHECK_EQUAL(filenames.mzML_i, "");
  BOOST_CHECK_EQUAL(filenames.mzTab_o, "");
  BOOST_CHECK_EQUAL(filenames.mzTab_i, "");
  BOOST_CHECK_EQUAL(filenames.featureXML_o, "");
  BOOST_CHECK_EQUAL(filenames.featureXMLSampleGroup_o, "");
  BOOST_CHECK_EQUAL(filenames.featureXML_i, "");
  BOOST_CHECK_EQUAL(filenames.featureXMLSampleGroup_i, "");
  BOOST_CHECK_EQUAL(filenames.featureFilterComponents_csv_o, "");
  BOOST_CHECK_EQUAL(filenames.featureFilterComponentGroups_csv_o, "");
  BOOST_CHECK_EQUAL(filenames.featureQCComponents_csv_o, "");
  BOOST_CHECK_EQUAL(filenames.featureQCComponentGroups_csv_o, "");
  BOOST_CHECK_EQUAL(filenames.featureRSDFilterComponents_csv_o, "");
  BOOST_CHECK_EQUAL(filenames.featureRSDFilterComponentGroups_csv_o, "");
  BOOST_CHECK_EQUAL(filenames.featureRSDQCComponents_csv_o, "");
  BOOST_CHECK_EQUAL(filenames.featureRSDQCComponentGroups_csv_o, "");
  BOOST_CHECK_EQUAL(filenames.featureBackgroundFilterComponents_csv_o, "");
  BOOST_CHECK_EQUAL(filenames.featureBackgroundFilterComponentGroups_csv_o, "");
  BOOST_CHECK_EQUAL(filenames.featureBackgroundQCComponents_csv_o, "");
  BOOST_CHECK_EQUAL(filenames.featureBackgroundQCComponentGroups_csv_o, "");
  BOOST_CHECK_EQUAL(filenames.featureRSDEstimationComponents_csv_o, "");
  BOOST_CHECK_EQUAL(filenames.featureRSDEstimationComponentGroups_csv_o, "");
  BOOST_CHECK_EQUAL(filenames.featureBackgroundEstimationComponents_csv_o, "");
  BOOST_CHECK_EQUAL(filenames.featureBackgroundEstimationComponentGroups_csv_o, "");
  BOOST_CHECK_EQUAL(filenames.quantitationMethods_csv_o, "");
  BOOST_CHECK_EQUAL(filenames.componentsToConcentrations_csv_o, "");
  BOOST_CHECK_EQUAL(filenames.pivotTable_csv_o, "/home/user/PivotTable.csv");
  BOOST_CHECK_EQUAL(filenames.featureDB_csv_o, "/home/user/FeatureDB.csv");
}

BOOST_AUTO_TEST_CASE(filenames_getDefaultDynamicFilenames)
{
  Filenames filenames = Filenames::getDefaultDynamicFilenames(
    "/home/user/mzML",
    "/home/user/featuresIn", 
    "/home/user/featuresOut", 
    "mzMLIn",
    "injIn",
    "injOut",
    "sampleIn",
    "sampleOut");
  BOOST_CHECK_EQUAL(filenames.sequence_csv_i, "");
  BOOST_CHECK_EQUAL(filenames.parameters_csv_i, "");
  BOOST_CHECK_EQUAL(filenames.workflow_csv_i, "");
  BOOST_CHECK_EQUAL(filenames.traML_csv_i, "");
  BOOST_CHECK_EQUAL(filenames.featureFilterComponents_csv_i, "");
  BOOST_CHECK_EQUAL(filenames.featureFilterComponentGroups_csv_i, "");
  BOOST_CHECK_EQUAL(filenames.featureQCComponents_csv_i, "");
  BOOST_CHECK_EQUAL(filenames.featureQCComponentGroups_csv_i, "");
  BOOST_CHECK_EQUAL(filenames.featureRSDFilterComponents_csv_i, "");
  BOOST_CHECK_EQUAL(filenames.featureRSDFilterComponentGroups_csv_i, "");
  BOOST_CHECK_EQUAL(filenames.featureRSDQCComponents_csv_i, "");
  BOOST_CHECK_EQUAL(filenames.featureRSDQCComponentGroups_csv_i, "");
  BOOST_CHECK_EQUAL(filenames.featureBackgroundFilterComponents_csv_i, "");
  BOOST_CHECK_EQUAL(filenames.featureBackgroundFilterComponentGroups_csv_i, "");
  BOOST_CHECK_EQUAL(filenames.featureBackgroundQCComponents_csv_i, "");
  BOOST_CHECK_EQUAL(filenames.featureBackgroundQCComponentGroups_csv_i, "");
  BOOST_CHECK_EQUAL(filenames.featureRSDEstimationComponents_csv_i, "");
  BOOST_CHECK_EQUAL(filenames.featureRSDEstimationComponentGroups_csv_i, "");
  BOOST_CHECK_EQUAL(filenames.featureBackgroundEstimationComponents_csv_i, "");
  BOOST_CHECK_EQUAL(filenames.featureBackgroundEstimationComponentGroups_csv_i, "");
  BOOST_CHECK_EQUAL(filenames.quantitationMethods_csv_i, "");
  BOOST_CHECK_EQUAL(filenames.standardsConcentrations_csv_i, "");
  BOOST_CHECK_EQUAL(filenames.referenceData_csv_i, "");
  BOOST_CHECK_EQUAL(filenames.mzML_i, "/home/user/mzML/mzMLIn.mzML");
  BOOST_CHECK_EQUAL(filenames.mzTab_o, "/home/user/featuresOut/injOut.mzTab");
  BOOST_CHECK_EQUAL(filenames.mzTab_i, "/home/user/featuresIn/injIn.mzTab");
  BOOST_CHECK_EQUAL(filenames.featureXML_o, "/home/user/featuresOut/injOut.featureXML");
  BOOST_CHECK_EQUAL(filenames.featureXMLSampleGroup_o, "/home/user/featuresOut/sampleOut.featureXML");
  BOOST_CHECK_EQUAL(filenames.featureXML_i, "/home/user/featuresIn/injIn.featureXML");
  BOOST_CHECK_EQUAL(filenames.featureXMLSampleGroup_i, "/home/user/featuresIn/sampleIn.featureXML");
  BOOST_CHECK_EQUAL(filenames.featureFilterComponents_csv_o, "/home/user/featuresOut/injOut_featureFilterComponents.csv");
  BOOST_CHECK_EQUAL(filenames.featureFilterComponentGroups_csv_o, "/home/user/featuresOut/injOut_featureFilterComponentGroups.csv");
  BOOST_CHECK_EQUAL(filenames.featureQCComponents_csv_o, "/home/user/featuresOut/injOut_featureQCComponents.csv");
  BOOST_CHECK_EQUAL(filenames.featureQCComponentGroups_csv_o, "/home/user/featuresOut/injOut_featureQCComponentGroups.csv");
  BOOST_CHECK_EQUAL(filenames.featureRSDFilterComponents_csv_o, "/home/user/featuresOut/injOut_featureRSDFilterComponents.csv");
  BOOST_CHECK_EQUAL(filenames.featureRSDFilterComponentGroups_csv_o, "/home/user/featuresOut/injOut_featureRSDFilterComponentGroups.csv");
  BOOST_CHECK_EQUAL(filenames.featureRSDQCComponents_csv_o, "/home/user/featuresOut/injOut_featureRSDQCComponents.csv");
  BOOST_CHECK_EQUAL(filenames.featureRSDQCComponentGroups_csv_o, "/home/user/featuresOut/injOut_featureRSDQCComponentGroups.csv");
  BOOST_CHECK_EQUAL(filenames.featureBackgroundFilterComponents_csv_o, "/home/user/featuresOut/injOut_featureBackgroundFilterComponents.csv");
  BOOST_CHECK_EQUAL(filenames.featureBackgroundFilterComponentGroups_csv_o, "/home/user/featuresOut/injOut_featureBackgroundFilterComponentGroups.csv");
  BOOST_CHECK_EQUAL(filenames.featureBackgroundQCComponents_csv_o, "/home/user/featuresOut/injOut_featureBackgroundQCComponents.csv");
  BOOST_CHECK_EQUAL(filenames.featureBackgroundQCComponentGroups_csv_o, "/home/user/featuresOut/injOut_featureBackgroundQCComponentGroups.csv");
  BOOST_CHECK_EQUAL(filenames.featureRSDEstimationComponents_csv_o, "/home/user/featuresOut/injOut_featureRSDEstimationComponents.csv");
  BOOST_CHECK_EQUAL(filenames.featureRSDEstimationComponentGroups_csv_o, "/home/user/featuresOut/injOut_featureRSDEstimationComponentGroups.csv");
  BOOST_CHECK_EQUAL(filenames.featureBackgroundEstimationComponents_csv_o, "/home/user/featuresOut/injOut_featureBackgroundEstimationComponents.csv");
  BOOST_CHECK_EQUAL(filenames.featureBackgroundEstimationComponentGroups_csv_o, "/home/user/featuresOut/injOut_featureBackgroundEstimationComponentGroups.csv");
  BOOST_CHECK_EQUAL(filenames.quantitationMethods_csv_o, "/home/user/featuresOut/injOut_quantitationMethods.csv");
  BOOST_CHECK_EQUAL(filenames.componentsToConcentrations_csv_o, "/home/user/featuresOut/injOut_componentsToConcentrations.csv");
  BOOST_CHECK_EQUAL(filenames.pivotTable_csv_o, "");
  BOOST_CHECK_EQUAL(filenames.featureDB_csv_o, "");
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
  BOOST_CHECK_EQUAL(filenames.featureRSDFilterComponents_csv_i, "");
  BOOST_CHECK_EQUAL(filenames.featureRSDFilterComponentGroups_csv_i, "");
  BOOST_CHECK_EQUAL(filenames.featureRSDQCComponents_csv_i, "");
  BOOST_CHECK_EQUAL(filenames.featureRSDQCComponentGroups_csv_i, "");
  BOOST_CHECK_EQUAL(filenames.featureBackgroundFilterComponents_csv_i, "");
  BOOST_CHECK_EQUAL(filenames.featureBackgroundFilterComponentGroups_csv_i, "");
  BOOST_CHECK_EQUAL(filenames.featureBackgroundQCComponents_csv_i, "");
  BOOST_CHECK_EQUAL(filenames.featureBackgroundQCComponentGroups_csv_i, "");
  BOOST_CHECK_EQUAL(filenames.featureRSDEstimationComponents_csv_i, "");
  BOOST_CHECK_EQUAL(filenames.featureRSDEstimationComponentGroups_csv_i, "");
  BOOST_CHECK_EQUAL(filenames.featureBackgroundEstimationComponents_csv_i, "");
  BOOST_CHECK_EQUAL(filenames.featureBackgroundEstimationComponentGroups_csv_i, "");
  BOOST_CHECK_EQUAL(filenames.quantitationMethods_csv_i, "");
  BOOST_CHECK_EQUAL(filenames.standardsConcentrations_csv_i, "");
  BOOST_CHECK_EQUAL(filenames.referenceData_csv_i, "");
  BOOST_CHECK_EQUAL(filenames.mzML_i, "");
  BOOST_CHECK_EQUAL(filenames.mzTab_o, "");
  BOOST_CHECK_EQUAL(filenames.mzTab_i, "");
  BOOST_CHECK_EQUAL(filenames.featureXML_o, "");
  BOOST_CHECK_EQUAL(filenames.featureXMLSampleGroup_o, "");
  BOOST_CHECK_EQUAL(filenames.featureXML_i, "");
  BOOST_CHECK_EQUAL(filenames.featureXMLSampleGroup_i, "");
  BOOST_CHECK_EQUAL(filenames.featureFilterComponents_csv_o, "");
  BOOST_CHECK_EQUAL(filenames.featureFilterComponentGroups_csv_o, "");
  BOOST_CHECK_EQUAL(filenames.featureQCComponents_csv_o, "");
  BOOST_CHECK_EQUAL(filenames.featureQCComponentGroups_csv_o, "");
  BOOST_CHECK_EQUAL(filenames.featureRSDFilterComponents_csv_o, "");
  BOOST_CHECK_EQUAL(filenames.featureRSDFilterComponentGroups_csv_o, "");
  BOOST_CHECK_EQUAL(filenames.featureRSDQCComponents_csv_o, "");
  BOOST_CHECK_EQUAL(filenames.featureRSDQCComponentGroups_csv_o, "");
  BOOST_CHECK_EQUAL(filenames.featureBackgroundFilterComponents_csv_o, "");
  BOOST_CHECK_EQUAL(filenames.featureBackgroundFilterComponentGroups_csv_o, "");
  BOOST_CHECK_EQUAL(filenames.featureBackgroundQCComponents_csv_o, "");
  BOOST_CHECK_EQUAL(filenames.featureBackgroundQCComponentGroups_csv_o, "");
  BOOST_CHECK_EQUAL(filenames.featureRSDEstimationComponents_csv_o, "");
  BOOST_CHECK_EQUAL(filenames.featureRSDEstimationComponentGroups_csv_o, "");
  BOOST_CHECK_EQUAL(filenames.featureBackgroundEstimationComponents_csv_o, "");
  BOOST_CHECK_EQUAL(filenames.featureBackgroundEstimationComponentGroups_csv_o, "");
  BOOST_CHECK_EQUAL(filenames.quantitationMethods_csv_o, "");
  BOOST_CHECK_EQUAL(filenames.componentsToConcentrations_csv_o, "");
  BOOST_CHECK_EQUAL(filenames.pivotTable_csv_o, "");
  BOOST_CHECK_EQUAL(filenames.featureDB_csv_o, "");
}

BOOST_AUTO_TEST_CASE(clear2)
{
  Filenames filenames = Filenames::getDefaultDynamicFilenames(
    "/home/user/mzML",
    "/home/user/featuresIn",
    "/home/user/featuresOut",
    "mzMLIn",
    "injIn",
    "injOut",
    "sampleIn",
    "sampleOut");
  filenames.clear();
  BOOST_CHECK_EQUAL(filenames.sequence_csv_i, "");
  BOOST_CHECK_EQUAL(filenames.parameters_csv_i, "");
  BOOST_CHECK_EQUAL(filenames.traML_csv_i, "");
  BOOST_CHECK_EQUAL(filenames.featureFilterComponents_csv_i, "");
  BOOST_CHECK_EQUAL(filenames.featureFilterComponentGroups_csv_i, "");
  BOOST_CHECK_EQUAL(filenames.featureQCComponents_csv_i, "");
  BOOST_CHECK_EQUAL(filenames.featureQCComponentGroups_csv_i, "");
  BOOST_CHECK_EQUAL(filenames.featureRSDQCComponents_csv_i, "");
  BOOST_CHECK_EQUAL(filenames.featureRSDQCComponentGroups_csv_i, "");
  BOOST_CHECK_EQUAL(filenames.featureBackgroundFilterComponents_csv_i, "");
  BOOST_CHECK_EQUAL(filenames.featureBackgroundFilterComponentGroups_csv_i, "");
  BOOST_CHECK_EQUAL(filenames.featureBackgroundQCComponents_csv_i, "");
  BOOST_CHECK_EQUAL(filenames.featureBackgroundQCComponentGroups_csv_i, "");
  BOOST_CHECK_EQUAL(filenames.featureRSDEstimationComponents_csv_i, "");
  BOOST_CHECK_EQUAL(filenames.featureRSDEstimationComponentGroups_csv_i, "");
  BOOST_CHECK_EQUAL(filenames.featureBackgroundEstimationComponents_csv_i, "");
  BOOST_CHECK_EQUAL(filenames.featureBackgroundEstimationComponentGroups_csv_i, "");
  BOOST_CHECK_EQUAL(filenames.quantitationMethods_csv_i, "");
  BOOST_CHECK_EQUAL(filenames.standardsConcentrations_csv_i, "");
  BOOST_CHECK_EQUAL(filenames.referenceData_csv_i, "");
  BOOST_CHECK_EQUAL(filenames.mzML_i, "");
  BOOST_CHECK_EQUAL(filenames.mzTab_o, "");
  BOOST_CHECK_EQUAL(filenames.mzTab_i, "");
  BOOST_CHECK_EQUAL(filenames.featureXML_o, "");
  BOOST_CHECK_EQUAL(filenames.featureXMLSampleGroup_o, "");
  BOOST_CHECK_EQUAL(filenames.featureXML_i, "");
  BOOST_CHECK_EQUAL(filenames.featureXMLSampleGroup_i, "");
  BOOST_CHECK_EQUAL(filenames.featureFilterComponents_csv_o, "");
  BOOST_CHECK_EQUAL(filenames.featureFilterComponentGroups_csv_o, "");
  BOOST_CHECK_EQUAL(filenames.featureQCComponents_csv_o, "");
  BOOST_CHECK_EQUAL(filenames.featureQCComponentGroups_csv_o, "");
  BOOST_CHECK_EQUAL(filenames.featureRSDQCComponents_csv_o, "");
  BOOST_CHECK_EQUAL(filenames.featureRSDQCComponentGroups_csv_o, "");
  BOOST_CHECK_EQUAL(filenames.featureBackgroundFilterComponents_csv_o, "");
  BOOST_CHECK_EQUAL(filenames.featureBackgroundFilterComponentGroups_csv_o, "");
  BOOST_CHECK_EQUAL(filenames.featureBackgroundQCComponents_csv_o, "");
  BOOST_CHECK_EQUAL(filenames.featureBackgroundQCComponentGroups_csv_o, "");
  BOOST_CHECK_EQUAL(filenames.featureRSDEstimationComponents_csv_o, "");
  BOOST_CHECK_EQUAL(filenames.featureRSDEstimationComponentGroups_csv_o, "");
  BOOST_CHECK_EQUAL(filenames.featureBackgroundEstimationComponents_csv_o, "");
  BOOST_CHECK_EQUAL(filenames.featureBackgroundEstimationComponentGroups_csv_o, "");
  BOOST_CHECK_EQUAL(filenames.quantitationMethods_csv_o, "");
  BOOST_CHECK_EQUAL(filenames.componentsToConcentrations_csv_o, "");
  BOOST_CHECK_EQUAL(filenames.pivotTable_csv_o, "");
  BOOST_CHECK_EQUAL(filenames.featureDB_csv_o, "");
}

BOOST_AUTO_TEST_SUITE_END()
