// TODO: Add copyright

#include <SmartPeak/test_config.h>

#define BOOST_TEST_MODULE FileReader test suite
#include <boost/test/included/unit_test.hpp>
#include <SmartPeak/io/FileReader.h>

using namespace SmartPeak;
using namespace std;

BOOST_AUTO_TEST_SUITE(filereader)

BOOST_AUTO_TEST_CASE(parseopenmsparams)
{
  const string pathname = SMARTPEAK_GET_TEST_DATA_PATH("FileReader_parameters.csv");
  ParameterSet parameters;
  FileReader::parseOpenMSParams(pathname, parameters);
  BOOST_CHECK_EQUAL(parameters.size(), 3);

  const string func1 {"func1"};

  BOOST_REQUIRE(parameters.count(func1));
  auto function_parameters = parameters.at(func1);
  BOOST_REQUIRE(function_parameters.size(), 3);
  BOOST_CHECK_EQUAL(function_parameters[0].getName(), "param1");
  BOOST_CHECK_EQUAL(function_parameters[0].getType(), "int");
  BOOST_CHECK_EQUAL(function_parameters[0].getValueAsString(), "-1");
  BOOST_CHECK_EQUAL(function_parameters[0].getDescription(), "");

  BOOST_CHECK_EQUAL(function_parameters[1].getName(), "param2");
  BOOST_CHECK_EQUAL(function_parameters[1].getType(), "float");
  BOOST_CHECK_EQUAL(function_parameters[1].getValueAsString(), "-1");
  BOOST_CHECK_EQUAL(function_parameters[1].getDescription(), "a description");

  BOOST_CHECK_EQUAL(function_parameters[2].getName(), "param3");
  BOOST_CHECK_EQUAL(function_parameters[2].getType(), "float");
  BOOST_CHECK_EQUAL(function_parameters[2].getValueAsString(), "-1");
  BOOST_CHECK_EQUAL(function_parameters[2].getDescription(), "a quoted description, containing a comma");

  const string func2 {"func2"};

  BOOST_REQUIRE(parameters.count(func2));
  function_parameters = parameters.at(func2);
  BOOST_REQUIRE(function_parameters.size(), 2);
  BOOST_CHECK_EQUAL(function_parameters[0].getName(), "param1");
  BOOST_CHECK_EQUAL(function_parameters[0].getType(), "float");
  BOOST_CHECK_EQUAL(function_parameters[0].getValueAsString(), "0.5");
  BOOST_CHECK_EQUAL(function_parameters[0].getDescription(), "");

  BOOST_CHECK_EQUAL(function_parameters[1].getName(), "param2");
  BOOST_CHECK_EQUAL(function_parameters[1].getType(), "bool");
  BOOST_CHECK_EQUAL(function_parameters[1].getValueAsString(), "false");
  BOOST_CHECK_EQUAL(function_parameters[1].getDescription(), "");

  const string func3 {"func3"};

  BOOST_REQUIRE(parameters.count(func3));
  function_parameters = parameters.at(func3);
  BOOST_REQUIRE(function_parameters.size(), 1);
  BOOST_CHECK_EQUAL(function_parameters[0].getName(), "param1");
  BOOST_CHECK_EQUAL(function_parameters[0].getType(), "bool");
  BOOST_CHECK_EQUAL(function_parameters[0].getValueAsString(), "false");
  BOOST_CHECK_EQUAL(function_parameters[0].getDescription(), "");
}

BOOST_AUTO_TEST_SUITE_END()
