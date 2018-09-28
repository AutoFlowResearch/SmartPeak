// TODO: Add copyright

#include <SmartPeak/test_config.h>

#define BOOST_TEST_MODULE SequenceProcessor test suite
#include <boost/test/included/unit_test.hpp>
#include <SmartPeak/core/SequenceProcessor.h>

using namespace SmartPeak;
using namespace std;

BOOST_AUTO_TEST_SUITE(sequenceprocessor)

BOOST_AUTO_TEST_CASE(createSequence)
{
  const map<string,string> filenames = {
    {"sequence_csv_i", SMARTPEAK_GET_TEST_DATA_PATH("SequenceParser_sequence_1.csv")},
    {"parameters_csv_i", SMARTPEAK_GET_TEST_DATA_PATH("RawDataProcessor_params_1_core.csv")},
    {"traML_csv_i", SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_traML_1.csv")},
    {"featureFilterComponents_csv_i", SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_mrmfeatureqccomponents_1.csv")},
    {"featureFilterComponentGroups_csv_i", SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_mrmfeatureqccomponentgroups_1.csv")},
    {"featureQCComponents_csv_i", SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_mrmfeatureqccomponents_1.csv")},
    {"featureQCComponentGroups_csv_i", SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_mrmfeatureqccomponentgroups_1.csv")},
    {"quantitationMethods_csv_i", SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_quantitationMethods_1.csv")},
    {"standardsConcentrations_csv_i", SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_standardsConcentrations_1.csv")}
  };
  SequenceHandler sequenceHandler;
  sequenceHandler.setFilenames(filenames);
  SequenceProcessor::createSequence(sequenceHandler, ",");
  BOOST_CHECK_EQUAL(sequenceHandler.getSequence().size(), 6);
  const SampleHandler& sample = sequenceHandler.getSequence()[0];
  BOOST_CHECK_EQUAL(sample.getMetaData().getSampleName(), "170808_Jonathan_yeast_Sacc1_1x");
  BOOST_CHECK_EQUAL(sample.getMetaData().getSampleGroupName(), "Test01");
  BOOST_CHECK_EQUAL(sample.getRawData().getTargetedExperiment().getTransitions().size(), 324);
  BOOST_CHECK_EQUAL(sample.getRawData().getTargetedExperiment().getTransitions()[0].getPeptideRef(), "arg-L");
  BOOST_CHECK_EQUAL(sample.getRawData().getFeatureFilter().component_qcs.size(), 324);
  BOOST_CHECK_EQUAL(sample.getRawData().getFeatureFilter().component_qcs[0].component_name, "arg-L.arg-L_1.Heavy");
  BOOST_CHECK_EQUAL(sample.getRawData().getFeatureQC().component_qcs.size(), 324);
  BOOST_CHECK_EQUAL(sample.getRawData().getFeatureQC().component_qcs[0].component_name, "arg-L.arg-L_1.Heavy");
  BOOST_CHECK_EQUAL(sample.getRawData().getQuantitationMethods().size(), 107);
  BOOST_CHECK_EQUAL(sample.getRawData().getQuantitationMethods()[0].getComponentName(), "23dpg.23dpg_1.Light");
  BOOST_CHECK_EQUAL(sequenceHandler.getSequenceSegments().size(), 1);
  BOOST_CHECK_EQUAL(sequenceHandler.getSequenceSegments()[0].getQuantitationMethods().size(), 107);
  BOOST_CHECK_EQUAL(sequenceHandler.getSequenceSegments()[0].getQuantitationMethods()[0].getComponentName(), "23dpg.23dpg_1.Light");
}

BOOST_AUTO_TEST_CASE(addRawDataHandlerToSequence)
{
  MetaDataHandler meta_data1;
  meta_data1.setFilename("file1");
  meta_data1.setSampleName("sample1");
  meta_data1.setSampleGroupName("sample");
  meta_data1.setSequenceSegmentName("sequence_segment1");
  meta_data1.setSampleType(MetaDataHandler::SampleType::Unknown);

  MetaDataHandler meta_data2;
  meta_data2.setFilename("file2");
  meta_data2.setSampleName("sample2");
  meta_data2.setSampleGroupName("sample");
  meta_data2.setSequenceSegmentName("sequence_segment1");
  meta_data2.setSampleType(MetaDataHandler::SampleType::Standard);

  MetaDataHandler meta_data3;
  meta_data3.setFilename("file3");
  meta_data3.setSampleName("sample3");
  meta_data3.setSampleGroupName("sample");
  meta_data3.setSequenceSegmentName("sequence_segment2");
  meta_data3.setSampleType(MetaDataHandler::SampleType::Unknown);

  OpenMS::FeatureMap featuremap1;

  SequenceHandler sequenceHandler;
  sequenceHandler.addSampleToSequence(meta_data1, featuremap1);
  sequenceHandler.addSampleToSequence(meta_data2, featuremap1);
  sequenceHandler.addSampleToSequence(meta_data3, featuremap1);

  RawDataHandler rawDataHandler;
  OpenMS::FeatureMap featuremap2;
  featuremap2.setMetaValue("foo", "bar");
  rawDataHandler.setFeatureMap(featuremap2);

  SequenceProcessor::addRawDataHandlerToSequence(sequenceHandler, rawDataHandler);

  BOOST_CHECK_EQUAL(sequenceHandler.getSequence().size(), 3);
  BOOST_CHECK_EQUAL(sequenceHandler.getSequence()[0].getRawData().getFeatureMap().metaValueExists("foo"), true);
  BOOST_CHECK_EQUAL(sequenceHandler.getSequence()[0].getRawData().getFeatureMap().getMetaValue("foo"), "bar");
  BOOST_CHECK_EQUAL(sequenceHandler.getSequence()[1].getRawData().getFeatureMap().metaValueExists("foo"), true);
  BOOST_CHECK_EQUAL(sequenceHandler.getSequence()[1].getRawData().getFeatureMap().getMetaValue("foo"), "bar");
  BOOST_CHECK_EQUAL(sequenceHandler.getSequence()[2].getRawData().getFeatureMap().metaValueExists("foo"), true);
  BOOST_CHECK_EQUAL(sequenceHandler.getSequence()[2].getRawData().getFeatureMap().getMetaValue("foo"), "bar");
}

BOOST_AUTO_TEST_CASE(segmentSamplesInSequence)
{
  MetaDataHandler meta_data1;
  meta_data1.setFilename("file1");
  meta_data1.setSampleName("sample1");
  meta_data1.setSampleGroupName("sample");
  meta_data1.setSequenceSegmentName("sequence_segment1");
  meta_data1.setSampleType(MetaDataHandler::SampleType::Unknown);

  MetaDataHandler meta_data2;
  meta_data2.setFilename("file2");
  meta_data2.setSampleName("sample2");
  meta_data2.setSampleGroupName("sample");
  meta_data2.setSequenceSegmentName("sequence_segment1");
  meta_data2.setSampleType(MetaDataHandler::SampleType::Standard);

  MetaDataHandler meta_data3;
  meta_data3.setFilename("file3");
  meta_data3.setSampleName("sample3");
  meta_data3.setSampleGroupName("sample");
  meta_data3.setSequenceSegmentName("sequence_segment2");
  meta_data3.setSampleType(MetaDataHandler::SampleType::Unknown);

  OpenMS::FeatureMap featuremap;

  SequenceHandler sequenceHandler;
  sequenceHandler.addSampleToSequence(meta_data1, featuremap);
  sequenceHandler.addSampleToSequence(meta_data2, featuremap);
  sequenceHandler.addSampleToSequence(meta_data3, featuremap);

  OpenMS::AbsoluteQuantitationMethod aqm;
  aqm.setComponentName("Test");
  SequenceSegmentHandler sequenceSegmentHandler;
  sequenceSegmentHandler.setQuantitationMethods({aqm});

  SequenceProcessor::segmentSamplesInSequence(sequenceHandler, sequenceSegmentHandler);

  BOOST_CHECK_EQUAL(sequenceHandler.getSequenceSegments().size(), 2);
  BOOST_CHECK_EQUAL(sequenceHandler.getSequenceSegments()[0].getSampleIndices().size(), 2);
  BOOST_CHECK_EQUAL(sequenceHandler.getSequenceSegments()[0].getSampleIndices()[0], 0);
  BOOST_CHECK_EQUAL(sequenceHandler.getSequenceSegments()[0].getSampleIndices()[1], 1);
  BOOST_CHECK_EQUAL(sequenceHandler.getSequenceSegments()[1].getSampleIndices().size(), 1);
  BOOST_CHECK_EQUAL(sequenceHandler.getSequenceSegments()[1].getSampleIndices()[0], 2);
  BOOST_CHECK_EQUAL(sequenceHandler.getSequenceSegments()[0].getQuantitationMethods().size(), 1);
  BOOST_CHECK_EQUAL(sequenceHandler.getSequenceSegments()[0].getQuantitationMethods()[0].getComponentName(), "Test");
}

BOOST_AUTO_TEST_CASE(processSequence)
{
  const map<string,string> filenames = {
    {"sequence_csv_i", SMARTPEAK_GET_TEST_DATA_PATH("SequenceParser_sequence_1.csv")},
    {"parameters_csv_i", SMARTPEAK_GET_TEST_DATA_PATH("RawDataProcessor_params_1_core.csv")},
    {"traML_csv_i", SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_traML_1.csv")},
    {"featureFilterComponents_csv_i", SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_mrmfeatureqccomponents_1.csv")},
    {"featureFilterComponentGroups_csv_i", SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_mrmfeatureqccomponentgroups_1.csv")},
    {"featureQCComponents_csv_i", SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_mrmfeatureqccomponents_1.csv")},
    {"featureQCComponentGroups_csv_i", SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_mrmfeatureqccomponentgroups_1.csv")},
    {"quantitationMethods_csv_i", SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_quantitationMethods_1.csv")},
    {"standardsConcentrations_csv_i", SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_standardsConcentrations_1.csv")}
  };
  SequenceHandler sequenceHandler;
  sequenceHandler.setFilenames(filenames);
  sequenceHandler.setDirDynamic(SMARTPEAK_GET_TEST_DATA_PATH(""));
  SequenceProcessor::createSequence(sequenceHandler, ",");
  const vector<string> raw_data_processing_methods = { "load_raw_data" };
  const RawDataHandler& rawDataHandler0 = sequenceHandler.getSequence()[0].getRawData();
  BOOST_CHECK_EQUAL(rawDataHandler0.getExperiment().getChromatograms().size(), 0);
  SequenceProcessor::processSequence(sequenceHandler, std::vector<std::string>(), raw_data_processing_methods);
  BOOST_CHECK_EQUAL(sequenceHandler.getSequence().size(), 6);
  BOOST_CHECK_EQUAL(rawDataHandler0.getExperiment().getChromatograms().size(), 340);
}

BOOST_AUTO_TEST_CASE(processSequenceSegments)
{
  // const map<string,string> filenames = {
  //   {"sequence_csv_i", SMARTPEAK_GET_TEST_DATA_PATH("SequenceParser_sequence_1.csv")},
  //   {"parameters_csv_i", SMARTPEAK_GET_TEST_DATA_PATH("RawDataProcessor_params_1_core.csv")},
  //   {"traML_csv_i", SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_traML_1.csv")},
  //   {"featureFilterComponents_csv_i", SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_mrmfeatureqccomponents_1.csv")},
  //   {"featureFilterComponentGroups_csv_i", SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_mrmfeatureqccomponentgroups_1.csv")},
  //   {"featureQCComponents_csv_i", SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_mrmfeatureqccomponents_1.csv")},
  //   {"featureQCComponentGroups_csv_i", SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_mrmfeatureqccomponentgroups_1.csv")},
  //   {"quantitationMethods_csv_i", SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_quantitationMethods_1.csv")},
  //   {"standardsConcentrations_csv_i", SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_standardsConcentrations_1.csv")}
  // };
  // SequenceHandler sequenceHandler;
  // sequenceHandler.setFilenames(filenames);
  // sequenceHandler.setDirDynamic(SMARTPEAK_GET_TEST_DATA_PATH(""));
  // SequenceProcessor::createSequence(sequenceHandler, ",");
  // SequenceProcessor::processSequenceSegments(sequenceHandler);
  // // TODO: add checks
}

BOOST_AUTO_TEST_SUITE_END()
