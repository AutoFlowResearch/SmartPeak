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
  ApplicationHandler application_handler;
  BuildCommandsFromNames buildCommandsFromNames(application_handler);
  std::vector<ApplicationHandler::Command> methods;

  buildCommandsFromNames.names_ = std::string("");
  buildCommandsFromNames.process();
  BOOST_CHECK_EQUAL(buildCommandsFromNames.commands_.size(), 0);

  buildCommandsFromNames.names_ = std::string("     ");
  buildCommandsFromNames.process();
  BOOST_CHECK_EQUAL(buildCommandsFromNames.commands_.size(), 0);

  buildCommandsFromNames.names_ = std::string("LOAD_RAW_DATA LOAD_FEATURES PICK_FEATURES");
  buildCommandsFromNames.process();
  BOOST_CHECK_EQUAL(buildCommandsFromNames.commands_.size(), 3);
  BOOST_CHECK_EQUAL(buildCommandsFromNames.commands_.at(0).getName(), "LOAD_RAW_DATA");
  BOOST_CHECK_EQUAL(buildCommandsFromNames.commands_.at(1).getName(), "LOAD_FEATURES");
  BOOST_CHECK_EQUAL(buildCommandsFromNames.commands_.at(2).getName(), "PICK_FEATURES");

  buildCommandsFromNames.names_ = std::string("LOAD_RAW_DATA PLOT_FEATURES LOAD_FEATURES"); // no plotting processor yet
  buildCommandsFromNames.process();
  BOOST_CHECK_EQUAL(buildCommandsFromNames.commands_.size(), 2);
  BOOST_CHECK_EQUAL(buildCommandsFromNames.commands_.at(0).getName(), "LOAD_RAW_DATA");
  BOOST_CHECK_EQUAL(buildCommandsFromNames.commands_.at(1).getName(), "LOAD_FEATURES");

  buildCommandsFromNames.names_ = std::string("55 87");
  buildCommandsFromNames.process();
  BOOST_CHECK_EQUAL(buildCommandsFromNames.commands_.size(), 0);
}

BOOST_AUTO_TEST_SUITE_END()
