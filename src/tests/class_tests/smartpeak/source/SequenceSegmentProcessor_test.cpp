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

BOOST_AUTO_TEST_SUITE_END()
