// TODO: Add copyright

// #include <SmartPeak/test_config.h>

#define BOOST_TEST_MODULE SequenceSegmentProcessor test suite
#include <boost/test/included/unit_test.hpp>
#include <SmartPeak/core/SequenceSegmentProcessor.h>

using namespace SmartPeak;
using namespace std;

BOOST_AUTO_TEST_SUITE(sequencesegmentprocessor)

BOOST_AUTO_TEST_CASE(checkSequenceSegmentProcessing)
{
  const std::vector<std::string> events1 = {
    "calculate_calibration",
    "calculate_carryover",
    "calculate_variability",
    "store_quantitation_methods",
    "load_quantitation_methods",
    "store_components_to_concentrations",
    "plot_calibrators"
  };
  BOOST_CHECK_EQUAL(SequenceSegmentProcessor::checkSequenceSegmentProcessing(events1), true);

  const std::vector<std::string> events2 = {
    "calculate_calibration",
    "carryover"
  };
  BOOST_CHECK_EQUAL(SequenceSegmentProcessor::checkSequenceSegmentProcessing(events2), false);
}

BOOST_AUTO_TEST_CASE(getDefaultSequenceSegmentProcessingWorkflow)
{
  std::vector<std::string> workflow;

  SequenceSegmentProcessor::getDefaultSequenceSegmentProcessingWorkflow(MetaDataHandler::SampleType::Unknown, workflow);
  BOOST_CHECK_EQUAL(workflow.empty(), true);

  SequenceSegmentProcessor::getDefaultSequenceSegmentProcessingWorkflow(MetaDataHandler::SampleType::Standard, workflow);
  BOOST_CHECK_EQUAL(workflow.size(), 1);
  BOOST_CHECK_EQUAL(workflow[0], "calculate_calibration");

  SequenceSegmentProcessor::getDefaultSequenceSegmentProcessingWorkflow(MetaDataHandler::SampleType::QC, workflow);
  BOOST_CHECK_EQUAL(workflow.size(), 1);
  BOOST_CHECK_EQUAL(workflow[0], "calculate_variability");

  SequenceSegmentProcessor::getDefaultSequenceSegmentProcessingWorkflow(MetaDataHandler::SampleType::Blank, workflow);
  BOOST_CHECK_EQUAL(workflow.empty(), true);

  SequenceSegmentProcessor::getDefaultSequenceSegmentProcessingWorkflow(MetaDataHandler::SampleType::DoubleBlank, workflow);
  BOOST_CHECK_EQUAL(workflow.empty(), true);

  SequenceSegmentProcessor::getDefaultSequenceSegmentProcessingWorkflow(MetaDataHandler::SampleType::Solvent, workflow);
  BOOST_CHECK_EQUAL(workflow.size(), 1);
  BOOST_CHECK_EQUAL(workflow[0], "calculate_carryover");
}

BOOST_AUTO_TEST_CASE(getSampleIndicesBySampleType)
{
  MetaDataHandler meta_data1;
  meta_data1.setFilename("file1");
  meta_data1.setSampleName("sample1");
  meta_data1.setSampleGroupName("sample");
  meta_data1.setSequenceSegmentName("sequence_segment");
  meta_data1.setSampleType(MetaDataHandler::SampleType::Unknown);
  OpenMS::FeatureMap featuremap1;

  MetaDataHandler meta_data2;
  meta_data2.setFilename("file2");
  meta_data2.setSampleName("sample2");
  meta_data2.setSampleGroupName("sample");
  meta_data2.setSequenceSegmentName("sequence_segment");
  meta_data2.setSampleType(MetaDataHandler::SampleType::Standard);
  OpenMS::FeatureMap featuremap2;

  MetaDataHandler meta_data3;
  meta_data3.setFilename("file3");
  meta_data3.setSampleName("sample3");
  meta_data3.setSampleGroupName("sample");
  meta_data3.setSequenceSegmentName("sequence_segment");
  meta_data3.setSampleType(MetaDataHandler::SampleType::Unknown);
  OpenMS::FeatureMap featuremap3;

  SequenceHandler sequenceHandler;
  sequenceHandler.addSampleToSequence(meta_data1, featuremap1);
  sequenceHandler.addSampleToSequence(meta_data2, featuremap2);
  sequenceHandler.addSampleToSequence(meta_data3, featuremap3);

  SequenceSegmentHandler sequenceSegmentHandler;

  sequenceSegmentHandler.setSampleIndices({0, 1, 2});

  std::vector<size_t> sample_indices;
  SequenceSegmentProcessor::getSampleIndicesBySampleType(
    sequenceSegmentHandler,
    sequenceHandler,
    MetaDataHandler::SampleType::Unknown,
    sample_indices
  );

  BOOST_CHECK_EQUAL(sample_indices.size(), 2);
  BOOST_CHECK_EQUAL(sample_indices[0], 0);
  BOOST_CHECK_EQUAL(sample_indices[1], 2);
}

BOOST_AUTO_TEST_SUITE_END()
