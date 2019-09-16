// TODO: Add copyright

#include <SmartPeak/test_config.h>

#define BOOST_TEST_MODULE AppStateProcessor test suite
#include <boost/test/included/unit_test.hpp>
#include <SmartPeak/core/AppStateProcessor.h>

using namespace SmartPeak;
using namespace std;

BOOST_AUTO_TEST_SUITE(appstateprocessor)

BOOST_AUTO_TEST_CASE(buildcommandsfromids)
{
  AppState state;
  BuildCommandsFromIds buildCommandsFromIds(state);
  std::vector<AppState::Command> methods;

  methods = buildCommandsFromIds(std::string(""));
  BOOST_CHECK_EQUAL(methods.size(), 0);

  methods = buildCommandsFromIds(std::string("     "));
  BOOST_CHECK_EQUAL(methods.size(), 0);

  methods = buildCommandsFromIds(std::string("1 2 3"));
  BOOST_CHECK_EQUAL(methods.size(), 3);
  BOOST_CHECK_EQUAL(methods.at(0).getID(), 1);
  BOOST_CHECK_EQUAL(methods.at(1).getID(), 2);
  BOOST_CHECK_EQUAL(methods.at(2).getID(), 3);

  methods = buildCommandsFromIds(std::string("1 10 2")); // no plotting processor yet, "10" skips
  BOOST_CHECK_EQUAL(methods.size(), 2);
  BOOST_CHECK_EQUAL(methods.at(0).getID(), 1);
  BOOST_CHECK_EQUAL(methods.at(1).getID(), 2);

  methods = buildCommandsFromIds(std::string("1 18 16")); // "18" and higher don't exist
  BOOST_CHECK_EQUAL(methods.size(), 2);
  BOOST_CHECK_EQUAL(methods.at(0).getID(), 1);
  BOOST_CHECK_EQUAL(methods.at(1).getID(), 16);

  methods = buildCommandsFromIds(std::string("55 87"));
  BOOST_CHECK_EQUAL(methods.size(), 0);
}

BOOST_AUTO_TEST_SUITE_END()
