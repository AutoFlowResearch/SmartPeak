// TODO: Add copyright

#include <SmartPeak/test_config.h>

#define BOOST_TEST_MODULE ApplicationProcessor test suite
#include <boost/test/included/unit_test.hpp>
#include <SmartPeak/core/ApplicationProcessor.h>

using namespace SmartPeak;
using namespace std;

BOOST_AUTO_TEST_SUITE(applicationprocessor)

BOOST_AUTO_TEST_CASE(buildcommandsfromids)
{
  ApplicationHandler state;
  BuildCommandsFromNames buildCommandsFromNames(state);
  std::vector<ApplicationHandler::Command> methods;

  methods = buildCommandsFromNames(std::string(""));
  BOOST_CHECK_EQUAL(methods.size(), 0);

  methods = buildCommandsFromNames(std::string("     "));
  BOOST_CHECK_EQUAL(methods.size(), 0);

  methods = buildCommandsFromNames(std::string("LOAD_RAW_DATA LOAD_FEATURES PICK_FEATURES"));
  BOOST_CHECK_EQUAL(methods.size(), 3);
  BOOST_CHECK_EQUAL(methods.at(0).getName(), "LOAD_RAW_DATA");
  BOOST_CHECK_EQUAL(methods.at(1).getName(), "LOAD_FEATURES");
  BOOST_CHECK_EQUAL(methods.at(2).getName(), "PICK_FEATURES");

  methods = buildCommandsFromNames(std::string("LOAD_RAW_DATA PLOT_FEATURES LOAD_FEATURES")); // no plotting processor yet
  BOOST_CHECK_EQUAL(methods.size(), 2);
  BOOST_CHECK_EQUAL(methods.at(0).getName(), "LOAD_RAW_DATA");
  BOOST_CHECK_EQUAL(methods.at(1).getName(), "LOAD_FEATURES");

  methods = buildCommandsFromNames(std::string("55 87"));
  BOOST_CHECK_EQUAL(methods.size(), 0);
}

BOOST_AUTO_TEST_SUITE_END()
