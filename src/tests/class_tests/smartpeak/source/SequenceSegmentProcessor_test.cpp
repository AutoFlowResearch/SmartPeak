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

BOOST_AUTO_TEST_SUITE_END()
