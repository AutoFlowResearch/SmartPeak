// TODO: Add copyright

 #include <SmartPeak/test_config.h>

#define BOOST_TEST_MODULE SampleGroupProcessor test suite
#include <boost/test/included/unit_test.hpp>
#include <SmartPeak/core/SampleGroupProcessor.h>

using namespace SmartPeak;
using namespace std;

BOOST_AUTO_TEST_SUITE(sequencesegmentprocessor)

/**
  MergeInjections Tests
*/
BOOST_AUTO_TEST_CASE(constructorMergeInjections)
{
  MergeInjections* ptrMergeInjections = nullptr;
  MergeInjections* nullPointerMergeInjections = nullptr;
  BOOST_CHECK_EQUAL(ptrMergeInjections, nullPointerMergeInjections);
}

BOOST_AUTO_TEST_CASE(destructorMergeInjections)
{
  MergeInjections* ptrMergeInjections = nullptr;
  ptrMergeInjections = new MergeInjections();
  delete ptrMergeInjections;
}

BOOST_AUTO_TEST_CASE(gettersMergeInjections)
{
  MergeInjections processor;

  BOOST_CHECK_EQUAL(processor.getID(), 15);
  BOOST_CHECK_EQUAL(processor.getName(), "CALCULATE_CALIBRATION");
}

BOOST_AUTO_TEST_CASE(processMergeInjections)
{
  // Pre-requisites: set up the parameters and data structures for testing
  const map<string, vector<map<string, string>>> absquant_params = {{"AbsoluteQuantitation", {
    {
      {"name", "min_points"},
      {"value", "4"}
    },
    {
      {"name", "max_bias"},
      {"value", "30.0"}
    },
    {
      {"name", "min_correlation_coefficient"},
      {"value", "0.9"}
    },
    {
      {"name", "max_iters"},
      {"value", "100"}
    },
    {
      {"name", "outlier_detection_method"},
      {"value", "iter_jackknife"}
    },
    {
      {"name", "use_chauvenet"},
      {"value", "false"}
    }
  }}};

}

BOOST_AUTO_TEST_SUITE_END()
