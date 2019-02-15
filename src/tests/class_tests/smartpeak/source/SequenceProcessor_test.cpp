// TODO: Add copyright

#include <SmartPeak/test_config.h>

#define BOOST_TEST_MODULE SequenceProcessor test suite
#include <boost/test/included/unit_test.hpp>
#include <SmartPeak/core/SequenceProcessor.h>
#include <SmartPeak/core/Filenames.h>

using namespace SmartPeak;
using namespace std;

Filenames generateTestFilenames()
{
  const std::string dir = SMARTPEAK_GET_TEST_DATA_PATH("");
  Filenames filenames;
  filenames.sequence_csv_i                     = dir + "SequenceParser_sequence_1.csv";
  filenames.parameters_csv_i                   = dir + "RawDataProcessor_params_1_core.csv";
  filenames.traML_csv_i                        = dir + "OpenMSFile_traML_1.csv";
  filenames.featureFilterComponents_csv_i      = dir + "OpenMSFile_mrmfeatureqccomponents_1.csv";
  filenames.featureFilterComponentGroups_csv_i = dir + "OpenMSFile_mrmfeatureqccomponentgroups_1.csv";
  filenames.featureQCComponents_csv_i          = dir + "OpenMSFile_mrmfeatureqccomponents_1.csv";
  filenames.featureQCComponentGroups_csv_i     = dir + "OpenMSFile_mrmfeatureqccomponentgroups_1.csv";
  filenames.quantitationMethods_csv_i          = dir + "OpenMSFile_quantitationMethods_1.csv";
  filenames.standardsConcentrations_csv_i      = dir + "OpenMSFile_standardsConcentrations_1.csv";
  return filenames;
}

BOOST_AUTO_TEST_SUITE(sequenceprocessor)

BOOST_AUTO_TEST_CASE(createSequence)
{
  SequenceHandler sequenceHandler;
  SequenceProcessor::createSequence(sequenceHandler, generateTestFilenames(), ",", false);
  BOOST_CHECK_EQUAL(sequenceHandler.getSequence().size(), 6);
  const InjectionHandler& injection = sequenceHandler.getSequence()[0];
  BOOST_CHECK_EQUAL(injection.getMetaData().getSampleName(), "170808_Jonathan_yeast_Sacc1_1x");
  BOOST_CHECK_EQUAL(injection.getMetaData().getSampleGroupName(), "Test01");
  BOOST_CHECK_EQUAL(injection.getRawData().getTargetedExperiment().getTransitions().size(), 324);
  BOOST_CHECK_EQUAL(injection.getRawData().getTargetedExperiment().getTransitions()[0].getPeptideRef(), "arg-L");
  BOOST_CHECK_EQUAL(injection.getRawData().getFeatureFilter().component_qcs.size(), 324);
  BOOST_CHECK_EQUAL(injection.getRawData().getFeatureFilter().component_qcs[0].component_name, "arg-L.arg-L_1.Heavy");
  BOOST_CHECK_EQUAL(injection.getRawData().getFeatureQC().component_qcs.size(), 324);
  BOOST_CHECK_EQUAL(injection.getRawData().getFeatureQC().component_qcs[0].component_name, "arg-L.arg-L_1.Heavy");
  BOOST_CHECK_EQUAL(injection.getRawData().getQuantitationMethods().size(), 107);
  BOOST_CHECK_EQUAL(injection.getRawData().getQuantitationMethods()[0].getComponentName(), "23dpg.23dpg_1.Light");
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
  meta_data1.acq_method_name = "6";
  meta_data1.inj_volume = 7.0;
  meta_data1.inj_volume_units = "8";
  meta_data1.batch_name = "9";

  MetaDataHandler meta_data2;
  meta_data2.setFilename("file2");
  meta_data2.setSampleName("sample2");
  meta_data2.setSampleGroupName("sample");
  meta_data2.setSequenceSegmentName("sequence_segment1");
  meta_data2.setSampleType(MetaDataHandler::SampleType::Standard);
  meta_data2.acq_method_name = "6";
  meta_data2.inj_volume = 7.0;
  meta_data2.inj_volume_units = "8";
  meta_data2.batch_name = "9";

  MetaDataHandler meta_data3;
  meta_data3.setFilename("file3");
  meta_data3.setSampleName("sample3");
  meta_data3.setSampleGroupName("sample");
  meta_data3.setSequenceSegmentName("sequence_segment2");
  meta_data3.setSampleType(MetaDataHandler::SampleType::Unknown);
  meta_data3.acq_method_name = "6";
  meta_data3.inj_volume = 7.0;
  meta_data3.inj_volume_units = "8";
  meta_data3.batch_name = "9";

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
  meta_data1.acq_method_name = "6";
  meta_data1.inj_volume = 7.0;
  meta_data1.inj_volume_units = "8";
  meta_data1.batch_name = "9";

  MetaDataHandler meta_data2;
  meta_data2.setFilename("file2");
  meta_data2.setSampleName("sample2");
  meta_data2.setSampleGroupName("sample");
  meta_data2.setSequenceSegmentName("sequence_segment1");
  meta_data2.setSampleType(MetaDataHandler::SampleType::Standard);
  meta_data2.acq_method_name = "6";
  meta_data2.inj_volume = 7.0;
  meta_data2.inj_volume_units = "8";
  meta_data2.batch_name = "9";

  MetaDataHandler meta_data3;
  meta_data3.setFilename("file3");
  meta_data3.setSampleName("sample3");
  meta_data3.setSampleGroupName("sample");
  meta_data3.setSequenceSegmentName("sequence_segment2");
  meta_data3.setSampleType(MetaDataHandler::SampleType::Unknown);
  meta_data3.acq_method_name = "6";
  meta_data3.inj_volume = 7.0;
  meta_data3.inj_volume_units = "8";
  meta_data3.batch_name = "9";

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
  SequenceHandler sequenceHandler;
  SequenceProcessor::createSequence(sequenceHandler, generateTestFilenames(), ",", false);
  const vector<RawDataProcessor::RawDataProcMethod> raw_data_processing_methods = { RawDataProcessor::LOAD_RAW_DATA };
  const RawDataHandler& rawDataHandler0 = sequenceHandler.getSequence()[0].getRawData();
  BOOST_CHECK_EQUAL(rawDataHandler0.getExperiment().getChromatograms().size(), 0); // empty (not loaded, yet)

  std::map<std::string, Filenames> dynamic_filenames;
  const std::string path = SMARTPEAK_GET_TEST_DATA_PATH("");
  for (const InjectionHandler& injection : sequenceHandler.getSequence()) {
    const std::string key = injection.getMetaData().getInjectionName();
    dynamic_filenames[key] = Filenames::getDefaultDynamicFilenames(
      path + "mzML/",
      path + "features/",
      path + "features/",
      injection.getMetaData().getSampleName(),
      key
    );
  }

  SequenceProcessor::processSequence(sequenceHandler, dynamic_filenames, std::set<std::string>(), raw_data_processing_methods);
  BOOST_CHECK_EQUAL(sequenceHandler.getSequence().size(), 6);
  BOOST_CHECK_EQUAL(rawDataHandler0.getExperiment().getChromatograms().size(), 340); // loaded
}

BOOST_AUTO_TEST_CASE(processSequenceSegments)
{
  SequenceHandler sequenceHandler;
  SequenceProcessor::createSequence(sequenceHandler, generateTestFilenames(), ",", false);
  const vector<SequenceSegmentProcessor::SeqSegProcMethod> raw_data_processing_methods =
    { SequenceSegmentProcessor::CALCULATE_CALIBRATION };

  std::map<std::string, Filenames> dynamic_filenames;
  const std::string path = SMARTPEAK_GET_TEST_DATA_PATH("");
  for (const SequenceSegmentHandler& sequence_segment : sequenceHandler.getSequenceSegments()) {
    const std::string key = sequence_segment.getSequenceSegmentName();
    dynamic_filenames[key] = Filenames::getDefaultDynamicFilenames(
      path + "mzML/",
      path + "features/",
      path + "features/",
      key,
      key
    );
  }

  SequenceProcessor::processSequenceSegments(sequenceHandler, dynamic_filenames, std::set<std::string>(), raw_data_processing_methods);
  BOOST_CHECK_EQUAL(sequenceHandler.getSequenceSegments().size(), 1);

  const std::vector<OpenMS::AbsoluteQuantitationMethod>& AQMs = sequenceHandler.getSequenceSegments()[0].getQuantitationMethods();

  BOOST_CHECK_EQUAL(AQMs.size(), 107);

  BOOST_CHECK_EQUAL(AQMs[0].getComponentName(), "23dpg.23dpg_1.Light");
  BOOST_CHECK_EQUAL(AQMs[0].getISName(), "23dpg.23dpg_1.Heavy");
  BOOST_CHECK_EQUAL(AQMs[0].getFeatureName(), "peak_apex_int");
  BOOST_CHECK_CLOSE(static_cast<double>(AQMs[0].getTransformationModelParams().getValue("slope")), 2.429728323, 1e-6);
  BOOST_CHECK_CLOSE(static_cast<double>(AQMs[0].getTransformationModelParams().getValue("intercept")), -0.091856745000000004, 1e-6);
  BOOST_CHECK_EQUAL(AQMs[0].getNPoints(), 4);
  BOOST_CHECK_CLOSE(static_cast<double>(AQMs[0].getCorrelationCoefficient()), 0.98384694900000003, 1e-6);
  BOOST_CHECK_CLOSE(static_cast<double>(AQMs[0].getLLOQ()), 0.25, 1e-6);
  BOOST_CHECK_CLOSE(static_cast<double>(AQMs[0].getULOQ()), 2.5, 1e-6);

  BOOST_CHECK_EQUAL(AQMs[1].getComponentName(), "35cgmp.35cgmp_1.Light");
  BOOST_CHECK_EQUAL(AQMs[1].getISName(), "camp.camp_1.Heavy");
  BOOST_CHECK_EQUAL(AQMs[1].getFeatureName(), "peak_apex_int");
  BOOST_CHECK_CLOSE(static_cast<double>(AQMs[1].getTransformationModelParams().getValue("slope")), 6.5645316830000002, 1e-6);
  BOOST_CHECK_CLOSE(static_cast<double>(AQMs[1].getTransformationModelParams().getValue("intercept")), -0.0015584049999999999, 1e-6);
  BOOST_CHECK_EQUAL(AQMs[1].getNPoints(), 10);
  BOOST_CHECK_CLOSE(static_cast<double>(AQMs[1].getCorrelationCoefficient()), 0.99739781999999999, 1e-6);
  BOOST_CHECK_CLOSE(static_cast<double>(AQMs[1].getLLOQ()), 0.0002, 1e-6);
  BOOST_CHECK_CLOSE(static_cast<double>(AQMs[1].getULOQ()), 1.0, 1e-6);

  BOOST_CHECK_EQUAL(AQMs[2].getComponentName(), "6pgc.6pgc_1.Light");
  BOOST_CHECK_EQUAL(AQMs[2].getISName(), "6pgc.6pgc_1.Heavy");
  BOOST_CHECK_EQUAL(AQMs[2].getFeatureName(), "peak_apex_int");
  BOOST_CHECK_CLOSE(static_cast<double>(AQMs[2].getTransformationModelParams().getValue("slope")), 66.39342173, 1e-6);
  BOOST_CHECK_CLOSE(static_cast<double>(AQMs[2].getTransformationModelParams().getValue("intercept")), -0.14264795499999999, 1e-6);
  BOOST_CHECK_EQUAL(AQMs[2].getNPoints(), 7);
  BOOST_CHECK_CLOSE(static_cast<double>(AQMs[2].getCorrelationCoefficient()), 0.99547012000000001, 1e-6);
  BOOST_CHECK_CLOSE(static_cast<double>(AQMs[2].getLLOQ()), 0.008, 1e-6);
  BOOST_CHECK_CLOSE(static_cast<double>(AQMs[2].getULOQ()), 0.8, 1e-6);
}

BOOST_AUTO_TEST_SUITE_END()
