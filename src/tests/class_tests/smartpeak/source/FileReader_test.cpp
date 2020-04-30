// TODO: Add copyright

#include <SmartPeak/test_config.h>

#define BOOST_TEST_MODULE FileReader test suite
#include <boost/test/included/unit_test.hpp>
#include <SmartPeak/io/FileReader.h>
#include <OpenMS/FORMAT/FileHandler.h>
#include <OpenMS/FORMAT/MzMLFile.h>
#include <OpenMS/ANALYSIS/OPENSWATH/ChromatogramExtractor.h>
#include <OpenMS/ANALYSIS/TARGETED/MRMMapping.h>
#include <OpenMS/KERNEL/SpectrumHelper.h>

using namespace SmartPeak;
using namespace std;

BOOST_AUTO_TEST_SUITE(filereader)

BOOST_AUTO_TEST_CASE(parseopenmsparams)
{
  const string pathname = SMARTPEAK_GET_TEST_DATA_PATH("FileReader_parameters.csv");
  map<string,vector<map<string,string>>> parameters;
  FileReader::parseOpenMSParams(pathname, parameters);
  BOOST_CHECK_EQUAL(parameters.size(), 3);

  const string func1 {"func1"};

  BOOST_CHECK_EQUAL(parameters[func1].size(), 3);

  BOOST_CHECK_EQUAL(parameters[func1][0].count("name"), 1);
  BOOST_CHECK_EQUAL(parameters[func1][0].count("type"), 1);
  BOOST_CHECK_EQUAL(parameters[func1][0].count("value"), 1);
  BOOST_CHECK_EQUAL(parameters[func1][0].count("description"), 1);
  BOOST_CHECK_EQUAL(parameters[func1][0]["name"], "param1");
  BOOST_CHECK_EQUAL(parameters[func1][0]["type"], "int");
  BOOST_CHECK_EQUAL(parameters[func1][0]["value"], "-1");
  BOOST_CHECK_EQUAL(parameters[func1][0]["description"], "");

  BOOST_CHECK_EQUAL(parameters[func1][1].count("name"), 1);
  BOOST_CHECK_EQUAL(parameters[func1][1].count("type"), 1);
  BOOST_CHECK_EQUAL(parameters[func1][1].count("value"), 1);
  BOOST_CHECK_EQUAL(parameters[func1][1].count("description"), 1);
  BOOST_CHECK_EQUAL(parameters[func1][1]["name"], "param2");
  BOOST_CHECK_EQUAL(parameters[func1][1]["type"], "float");
  BOOST_CHECK_EQUAL(parameters[func1][1]["value"], "-1");
  BOOST_CHECK_EQUAL(parameters[func1][1]["description"], "a description");

  BOOST_CHECK_EQUAL(parameters[func1][2].count("name"), 1);
  BOOST_CHECK_EQUAL(parameters[func1][2].count("type"), 1);
  BOOST_CHECK_EQUAL(parameters[func1][2].count("value"), 1);
  BOOST_CHECK_EQUAL(parameters[func1][2].count("description"), 1);
  BOOST_CHECK_EQUAL(parameters[func1][2]["name"], "param3");
  BOOST_CHECK_EQUAL(parameters[func1][2]["type"], "float");
  BOOST_CHECK_EQUAL(parameters[func1][2]["value"], "-1");
  BOOST_CHECK_EQUAL(parameters[func1][2]["description"], "a quoted description, containing a comma");

  BOOST_CHECK_EQUAL(parameters[func1][2].count("name"), 1);
  BOOST_CHECK_EQUAL(parameters[func1][2].count("type"), 1);
  BOOST_CHECK_EQUAL(parameters[func1][2].count("value"), 1);
  BOOST_CHECK_EQUAL(parameters[func1][2].count("description"), 1);
  BOOST_CHECK_EQUAL(parameters[func1][2]["name"], "param3");
  BOOST_CHECK_EQUAL(parameters[func1][2]["type"], "float");
  BOOST_CHECK_EQUAL(parameters[func1][2]["value"], "-1");
  BOOST_CHECK_EQUAL(parameters[func1][2]["description"], "a quoted description, containing a comma");

  const string func2 {"func2"};

  BOOST_CHECK_EQUAL(parameters[func2].size(), 2);

  BOOST_CHECK_EQUAL(parameters[func2][0].count("name"), 1);
  BOOST_CHECK_EQUAL(parameters[func2][0].count("type"), 1);
  BOOST_CHECK_EQUAL(parameters[func2][0].count("value"), 1);
  BOOST_CHECK_EQUAL(parameters[func2][0].count("description"), 1);
  BOOST_CHECK_EQUAL(parameters[func2][0]["name"], "param1");
  BOOST_CHECK_EQUAL(parameters[func2][0]["type"], "float");
  BOOST_CHECK_EQUAL(parameters[func2][0]["value"], "0.5");
  BOOST_CHECK_EQUAL(parameters[func2][0]["description"], "");

  BOOST_CHECK_EQUAL(parameters[func2][1].count("name"), 1);
  BOOST_CHECK_EQUAL(parameters[func2][1].count("type"), 1);
  BOOST_CHECK_EQUAL(parameters[func2][1].count("value"), 1);
  BOOST_CHECK_EQUAL(parameters[func2][1].count("description"), 1);
  BOOST_CHECK_EQUAL(parameters[func2][1]["name"], "param2");
  BOOST_CHECK_EQUAL(parameters[func2][1]["type"], "bool");
  BOOST_CHECK_EQUAL(parameters[func2][1]["value"], "FALSE");
  BOOST_CHECK_EQUAL(parameters[func2][1]["description"], "");

  const string func3 {"func3"};

  BOOST_CHECK_EQUAL(parameters[func3].size(), 1);
  
  BOOST_CHECK_EQUAL(parameters[func3][0].count("name"), 1);
  BOOST_CHECK_EQUAL(parameters[func3][0].count("type"), 1);
  BOOST_CHECK_EQUAL(parameters[func3][0].count("value"), 1);
  BOOST_CHECK_EQUAL(parameters[func3][0].count("description"), 1);
  BOOST_CHECK_EQUAL(parameters[func3][0]["name"], "param1");
  BOOST_CHECK_EQUAL(parameters[func3][0]["type"], "bool");
  BOOST_CHECK_EQUAL(parameters[func3][0]["value"], "FALSE");
  BOOST_CHECK_EQUAL(parameters[func3][0]["description"], "");
}

BOOST_AUTO_TEST_SUITE_END()
