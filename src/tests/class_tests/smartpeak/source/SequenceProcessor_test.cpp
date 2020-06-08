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
  filenames.sequence_csv_i                     = dir + "SequenceProcessor_sequence.csv";
  filenames.parameters_csv_i                   = dir + "RawDataProcessor_params_1_core.csv";
  filenames.traML_csv_i                        = dir + "OpenMSFile_traML_1.csv";
  filenames.featureFilterComponents_csv_i      = dir + "OpenMSFile_mrmfeatureqccomponents_1.csv";
  filenames.featureFilterComponentGroups_csv_i = dir + "OpenMSFile_mrmfeatureqccomponentgroups_1.csv";
  filenames.featureQCComponents_csv_i          = dir + "OpenMSFile_mrmfeatureqccomponents_1.csv";
  filenames.featureQCComponentGroups_csv_i     = dir + "OpenMSFile_mrmfeatureqccomponentgroups_1.csv";
  filenames.featureRSDFilterComponents_csv_i = dir + "OpenMSFile_mrmfeatureqccomponents_1.csv";
  filenames.featureRSDFilterComponentGroups_csv_i = dir + "OpenMSFile_mrmfeatureqccomponentgroups_1.csv";
  filenames.featureRSDQCComponents_csv_i = dir + "OpenMSFile_mrmfeatureqccomponents_1.csv";
  filenames.featureRSDQCComponentGroups_csv_i = dir + "OpenMSFile_mrmfeatureqccomponentgroups_1.csv";
  filenames.featureBackgroundFilterComponents_csv_i = dir + "OpenMSFile_mrmfeatureqccomponents_1.csv";
  filenames.featureBackgroundFilterComponentGroups_csv_i = dir + "OpenMSFile_mrmfeatureqccomponentgroups_1.csv";
  filenames.featureBackgroundQCComponents_csv_i = dir + "OpenMSFile_mrmfeatureqccomponents_1.csv";
  filenames.featureBackgroundQCComponentGroups_csv_i = dir + "OpenMSFile_mrmfeatureqccomponentgroups_1.csv";
  filenames.quantitationMethods_csv_i          = dir + "OpenMSFile_quantitationMethods_1.csv";
  filenames.standardsConcentrations_csv_i      = dir + "OpenMSFile_standardsConcentrations_1.csv";
  return filenames;
}

BOOST_AUTO_TEST_SUITE(sequenceprocessor)

BOOST_AUTO_TEST_CASE(createSequence)
{
  SequenceHandler sequenceHandler;
  CreateSequence cs(sequenceHandler);
  cs.filenames        = generateTestFilenames();
  cs.delimiter        = ",";
  cs.checkConsistency = false;
  cs.process();

  // Test initialization of the sequence
  BOOST_CHECK_EQUAL(sequenceHandler.getSequence().size(), 6);
  InjectionHandler& injection0 = sequenceHandler.getSequence()[0];
  BOOST_CHECK_EQUAL(injection0.getMetaData().getSampleName(), "170808_Jonathan_yeast_Sacc1_1x");
  BOOST_CHECK_EQUAL(injection0.getMetaData().getSampleGroupName(), "Test01");
  BOOST_CHECK_EQUAL(injection0.getRawData().getMetaData().getSampleName(), "170808_Jonathan_yeast_Sacc1_1x");
  BOOST_CHECK_EQUAL(injection0.getRawData().getParameters().size(), 18);
  BOOST_CHECK_EQUAL(injection0.getRawData().getParameters().at("MRMFeatureFinderScoring")[0].at("name"), "stop_report_after_feature");
  BOOST_CHECK_EQUAL(injection0.getRawData().getTargetedExperiment().getTransitions().size(), 324);
  BOOST_CHECK_EQUAL(injection0.getRawData().getTargetedExperiment().getTransitions()[0].getPeptideRef(), "arg-L");
  BOOST_CHECK_EQUAL(injection0.getRawData().getFeatureFilter().component_qcs.size(), 324);
  BOOST_CHECK_EQUAL(injection0.getRawData().getFeatureFilter().component_qcs[0].component_name, "arg-L.arg-L_1.Heavy");
  BOOST_CHECK_EQUAL(injection0.getRawData().getFeatureQC().component_qcs.size(), 324);
  BOOST_CHECK_EQUAL(injection0.getRawData().getFeatureQC().component_qcs[0].component_name, "arg-L.arg-L_1.Heavy");
  BOOST_CHECK_EQUAL(injection0.getRawData().getFeatureRSDFilter().component_qcs.size(), 324);
  BOOST_CHECK_EQUAL(injection0.getRawData().getFeatureRSDFilter().component_qcs[0].component_name, "arg-L.arg-L_1.Heavy");
  BOOST_CHECK_EQUAL(injection0.getRawData().getFeatureRSDQC().component_qcs.size(), 324);
  BOOST_CHECK_EQUAL(injection0.getRawData().getFeatureRSDQC().component_qcs[0].component_name, "arg-L.arg-L_1.Heavy");
  BOOST_CHECK_EQUAL(injection0.getRawData().getFeatureBackgroundFilter().component_qcs.size(), 324);
  BOOST_CHECK_EQUAL(injection0.getRawData().getFeatureBackgroundFilter().component_qcs[0].component_name, "arg-L.arg-L_1.Heavy");
  BOOST_CHECK_EQUAL(injection0.getRawData().getFeatureBackgroundQC().component_qcs.size(), 324);
  BOOST_CHECK_EQUAL(injection0.getRawData().getFeatureBackgroundQC().component_qcs[0].component_name, "arg-L.arg-L_1.Heavy");
  BOOST_CHECK_EQUAL(injection0.getRawData().getQuantitationMethods().size(), 107);
  BOOST_CHECK_EQUAL(injection0.getRawData().getQuantitationMethods()[0].getComponentName(), "23dpg.23dpg_1.Light");
  InjectionHandler& injection5 = sequenceHandler.getSequence()[5];
  BOOST_CHECK_EQUAL(injection5.getMetaData().getSampleName(), "170808_Jonathan_yeast_Yarr3_1x");
  BOOST_CHECK_EQUAL(injection5.getMetaData().getSampleGroupName(), "Test01");
  BOOST_CHECK_EQUAL(injection5.getRawData().getMetaData().getSampleName(), "170808_Jonathan_yeast_Yarr3_1x");
  BOOST_CHECK_EQUAL(injection5.getRawData().getParameters().size(), 18);
  BOOST_CHECK_EQUAL(injection5.getRawData().getParameters().at("MRMFeatureFinderScoring")[0].at("name"), "stop_report_after_feature");
  BOOST_CHECK_EQUAL(injection5.getRawData().getTargetedExperiment().getTransitions().size(), 324);
  BOOST_CHECK_EQUAL(injection5.getRawData().getTargetedExperiment().getTransitions()[0].getPeptideRef(), "arg-L");
  BOOST_CHECK_EQUAL(injection5.getRawData().getFeatureFilter().component_qcs.size(), 324);
  BOOST_CHECK_EQUAL(injection5.getRawData().getFeatureFilter().component_qcs[0].component_name, "arg-L.arg-L_1.Heavy");
  BOOST_CHECK_EQUAL(injection5.getRawData().getFeatureQC().component_qcs.size(), 324);
  BOOST_CHECK_EQUAL(injection5.getRawData().getFeatureQC().component_qcs[0].component_name, "arg-L.arg-L_1.Heavy");
  BOOST_CHECK_EQUAL(injection5.getRawData().getFeatureRSDFilter().component_qcs.size(), 324);
  BOOST_CHECK_EQUAL(injection5.getRawData().getFeatureRSDFilter().component_qcs[0].component_name, "arg-L.arg-L_1.Heavy");
  BOOST_CHECK_EQUAL(injection5.getRawData().getFeatureRSDQC().component_qcs.size(), 324);
  BOOST_CHECK_EQUAL(injection5.getRawData().getFeatureRSDQC().component_qcs[0].component_name, "arg-L.arg-L_1.Heavy");
  BOOST_CHECK_EQUAL(injection5.getRawData().getFeatureBackgroundFilter().component_qcs.size(), 324);
  BOOST_CHECK_EQUAL(injection5.getRawData().getFeatureBackgroundFilter().component_qcs[0].component_name, "arg-L.arg-L_1.Heavy");
  BOOST_CHECK_EQUAL(injection5.getRawData().getFeatureBackgroundQC().component_qcs.size(), 324);
  BOOST_CHECK_EQUAL(injection5.getRawData().getFeatureBackgroundQC().component_qcs[0].component_name, "arg-L.arg-L_1.Heavy");
  BOOST_CHECK_EQUAL(injection5.getRawData().getQuantitationMethods().size(), 107);
  BOOST_CHECK_EQUAL(injection5.getRawData().getQuantitationMethods()[0].getComponentName(), "23dpg.23dpg_1.Light");
  BOOST_CHECK_EQUAL(sequenceHandler.getSequenceSegments().size(), 1);
  BOOST_CHECK_EQUAL(sequenceHandler.getSequenceSegments()[0].getQuantitationMethods().size(), 107);
  BOOST_CHECK_EQUAL(sequenceHandler.getSequenceSegments()[0].getQuantitationMethods()[0].getComponentName(), "23dpg.23dpg_1.Light");
  BOOST_CHECK_EQUAL(sequenceHandler.getSequenceSegments()[0].getFeatureFilter().component_qcs.size(), 324);
  BOOST_CHECK_EQUAL(sequenceHandler.getSequenceSegments()[0].getFeatureFilter().component_qcs[0].component_name, "arg-L.arg-L_1.Heavy");
  BOOST_CHECK_EQUAL(sequenceHandler.getSequenceSegments()[0].getFeatureQC().component_qcs.size(), 324);
  BOOST_CHECK_EQUAL(sequenceHandler.getSequenceSegments()[0].getFeatureQC().component_qcs[0].component_name, "arg-L.arg-L_1.Heavy");
  BOOST_CHECK_EQUAL(sequenceHandler.getSequenceSegments()[0].getFeatureRSDFilter().component_qcs.size(), 324);
  BOOST_CHECK_EQUAL(sequenceHandler.getSequenceSegments()[0].getFeatureRSDFilter().component_qcs[0].component_name, "arg-L.arg-L_1.Heavy");
  BOOST_CHECK_EQUAL(sequenceHandler.getSequenceSegments()[0].getFeatureRSDQC().component_qcs.size(), 324);
  BOOST_CHECK_EQUAL(sequenceHandler.getSequenceSegments()[0].getFeatureRSDQC().component_qcs[0].component_name, "arg-L.arg-L_1.Heavy");
  BOOST_CHECK_EQUAL(sequenceHandler.getSequenceSegments()[0].getFeatureBackgroundFilter().component_qcs.size(), 324);
  BOOST_CHECK_EQUAL(sequenceHandler.getSequenceSegments()[0].getFeatureBackgroundFilter().component_qcs[0].component_name, "arg-L.arg-L_1.Heavy");
  BOOST_CHECK_EQUAL(sequenceHandler.getSequenceSegments()[0].getFeatureBackgroundQC().component_qcs.size(), 324);
  BOOST_CHECK_EQUAL(sequenceHandler.getSequenceSegments()[0].getFeatureBackgroundQC().component_qcs[0].component_name, "arg-L.arg-L_1.Heavy");

  // Test non-shared resources
  injection0.getMetaData().setSampleName("modified");
  BOOST_CHECK_EQUAL(injection0.getRawData().getMetaData().getSampleName(), "modified");
  BOOST_CHECK_EQUAL(injection5.getMetaData().getSampleName(), "170808_Jonathan_yeast_Yarr3_1x");

  // Test shared resources between all raw data handlers
  injection0.getRawData().getParameters().at("MRMFeatureFinderScoring")[0].at("name") = "modified";
  BOOST_CHECK_EQUAL(injection0.getRawData().getParameters().at("MRMFeatureFinderScoring")[0].at("name"), "modified");
  BOOST_CHECK_EQUAL(injection5.getRawData().getParameters().at("MRMFeatureFinderScoring")[0].at("name"), "modified");
  auto transitions = injection0.getRawData().getTargetedExperiment().getTransitions();
  transitions[0].setPeptideRef("arg-L-mod");
  injection0.getRawData().getTargetedExperiment().setTransitions(transitions);
  BOOST_CHECK_EQUAL(injection0.getRawData().getTargetedExperiment().getTransitions()[0].getPeptideRef(), "arg-L-mod");
  BOOST_CHECK_EQUAL(injection5.getRawData().getTargetedExperiment().getTransitions()[0].getPeptideRef(), "arg-L-mod");

  // Test shared resources between sequence segment handlers
  injection0.getRawData().getQuantitationMethods()[0].setComponentName("23dpg.23dpg_1.Light-mod");
  BOOST_CHECK_EQUAL(injection0.getRawData().getQuantitationMethods()[0].getComponentName(), "23dpg.23dpg_1.Light-mod");
  BOOST_CHECK_EQUAL(injection5.getRawData().getQuantitationMethods()[0].getComponentName(), "23dpg.23dpg_1.Light-mod");
  BOOST_CHECK_EQUAL(sequenceHandler.getSequenceSegments()[0].getQuantitationMethods()[0].getComponentName(), "23dpg.23dpg_1.Light-mod");
  injection0.getRawData().getFeatureFilter().component_qcs[0].component_name = "arg-L.arg-L_1.Heavy-mod";
  BOOST_CHECK_EQUAL(injection0.getRawData().getFeatureFilter().component_qcs[0].component_name, "arg-L.arg-L_1.Heavy-mod");
  BOOST_CHECK_EQUAL(injection5.getRawData().getFeatureFilter().component_qcs[0].component_name, "arg-L.arg-L_1.Heavy-mod");
  BOOST_CHECK_EQUAL(sequenceHandler.getSequenceSegments()[0].getFeatureFilter().component_qcs[0].component_name, "arg-L.arg-L_1.Heavy-mod");
  injection0.getRawData().getFeatureQC().component_qcs[0].component_name = "arg-L.arg-L_1.Heavy-modified";
  BOOST_CHECK_EQUAL(injection0.getRawData().getFeatureQC().component_qcs[0].component_name, "arg-L.arg-L_1.Heavy-modified");
  BOOST_CHECK_EQUAL(injection5.getRawData().getFeatureQC().component_qcs[0].component_name, "arg-L.arg-L_1.Heavy-modified");
  BOOST_CHECK_EQUAL(sequenceHandler.getSequenceSegments()[0].getFeatureQC().component_qcs[0].component_name, "arg-L.arg-L_1.Heavy-modified");
  injection0.getRawData().getFeatureRSDFilter().component_qcs[0].component_name = "trp-L.trp-L_1.Heavy-mod";
  BOOST_CHECK_EQUAL(injection0.getRawData().getFeatureRSDFilter().component_qcs[0].component_name, "trp-L.trp-L_1.Heavy-mod");
  BOOST_CHECK_EQUAL(injection5.getRawData().getFeatureRSDFilter().component_qcs[0].component_name, "trp-L.trp-L_1.Heavy-mod");
  BOOST_CHECK_EQUAL(sequenceHandler.getSequenceSegments()[0].getFeatureRSDFilter().component_qcs[0].component_name, "trp-L.trp-L_1.Heavy-mod");
  injection0.getRawData().getFeatureRSDQC().component_qcs[0].component_name = "trp-L.trp-L_1.Heavy-modified";
  BOOST_CHECK_EQUAL(injection0.getRawData().getFeatureRSDQC().component_qcs[0].component_name, "trp-L.trp-L_1.Heavy-modified");
  BOOST_CHECK_EQUAL(injection5.getRawData().getFeatureRSDQC().component_qcs[0].component_name, "trp-L.trp-L_1.Heavy-modified");
  BOOST_CHECK_EQUAL(sequenceHandler.getSequenceSegments()[0].getFeatureRSDQC().component_qcs[0].component_name, "trp-L.trp-L_1.Heavy-modified");
  injection0.getRawData().getFeatureBackgroundFilter().component_qcs[0].component_name = "ala-L.ala-L_1.Heavy-mod";
  BOOST_CHECK_EQUAL(injection0.getRawData().getFeatureBackgroundFilter().component_qcs[0].component_name, "ala-L.ala-L_1.Heavy-mod");
  BOOST_CHECK_EQUAL(injection5.getRawData().getFeatureBackgroundFilter().component_qcs[0].component_name, "ala-L.ala-L_1.Heavy-mod");
  BOOST_CHECK_EQUAL(sequenceHandler.getSequenceSegments()[0].getFeatureBackgroundFilter().component_qcs[0].component_name, "ala-L.ala-L_1.Heavy-mod");
  injection0.getRawData().getFeatureBackgroundQC().component_qcs[0].component_name = "ala-L.ala-L_1.Heavy-modified";
  BOOST_CHECK_EQUAL(injection0.getRawData().getFeatureBackgroundQC().component_qcs[0].component_name, "ala-L.ala-L_1.Heavy-modified");
  BOOST_CHECK_EQUAL(injection5.getRawData().getFeatureBackgroundQC().component_qcs[0].component_name, "ala-L.ala-L_1.Heavy-modified");
  BOOST_CHECK_EQUAL(sequenceHandler.getSequenceSegments()[0].getFeatureBackgroundQC().component_qcs[0].component_name, "ala-L.ala-L_1.Heavy-modified");

  sequenceHandler.clear();
  Filenames filenames { generateTestFilenames() };
  filenames.sequence_csv_i = SMARTPEAK_GET_TEST_DATA_PATH("SequenceProcessor_empty_sequence.csv");

  cs.filenames = filenames;
  cs.process();

  BOOST_CHECK_EQUAL(sequenceHandler.getSequence().size(), 0);
}

BOOST_AUTO_TEST_CASE(processSequence)
{
  SequenceHandler sequenceHandler;
  CreateSequence cs(sequenceHandler);
  cs.filenames        = generateTestFilenames();
  cs.delimiter        = ",";
  cs.checkConsistency = false;
  cs.process();

  const vector<std::shared_ptr<RawDataProcessor>> raw_data_processing_methods = { std::shared_ptr<RawDataProcessor>(new LoadRawData()) };
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

  BOOST_CHECK_EQUAL(sequenceHandler.getSequence().size(), dynamic_filenames.size());

  ProcessSequence ps(sequenceHandler);
  ps.filenames                     = dynamic_filenames;
  ps.raw_data_processing_methods_I = raw_data_processing_methods;
  ps.process();

  BOOST_CHECK_EQUAL(sequenceHandler.getSequence().size(), 6);
  BOOST_CHECK_EQUAL(rawDataHandler0.getExperiment().getChromatograms().size(), 340); // loaded

  SmartPeak::SequenceProcessorMultithread spMT1(sequenceHandler.getSequence(),
    dynamic_filenames,
    raw_data_processing_methods);

  SmartPeak::SequenceProcessorMultithread spMT2(sequenceHandler.getSequence(),
    dynamic_filenames,
    raw_data_processing_methods);

  const unsigned int max_threads = std::thread::hardware_concurrency();

  if (max_threads != 0 && 4 <= max_threads) {
    BOOST_CHECK_EQUAL(spMT1.getNumWorkers(4), 3);
    BOOST_CHECK_EQUAL(spMT2.getNumWorkers(3), 2);
  }
  else {
    BOOST_CHECK_EQUAL(spMT1.getNumWorkers(8), 1);
    BOOST_CHECK_EQUAL(spMT2.getNumWorkers(3), 1);
  }
}

BOOST_AUTO_TEST_CASE(processSequenceSegments)
{
  SequenceHandler sequenceHandler;
  CreateSequence cs(sequenceHandler);
  cs.filenames        = generateTestFilenames();
  cs.delimiter        = ",";
  cs.checkConsistency = false;
  cs.process();

  const vector<std::shared_ptr<SequenceSegmentProcessor>> sequence_segment_processing_methods =
    { std::shared_ptr<SequenceSegmentProcessor>(new CalculateCalibration()) };

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

  ProcessSequenceSegments pss(sequenceHandler);
  pss.filenames                             = dynamic_filenames;
  pss.sequence_segment_processing_methods_I = sequence_segment_processing_methods;
  pss.process();

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
