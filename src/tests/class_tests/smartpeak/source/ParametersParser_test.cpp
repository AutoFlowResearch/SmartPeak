// --------------------------------------------------------------------------
//   SmartPeak -- Fast and Accurate CE-, GC- and LC-MS(/MS) Data Processing
// --------------------------------------------------------------------------
// Copyright The SmartPeak Team -- Novo Nordisk Foundation 
// Center for Biosustainability, Technical University of Denmark 2018-2021.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL ANY OF THE AUTHORS OR THE CONTRIBUTING
// INSTITUTIONS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
// OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
// WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
// OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
// ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// --------------------------------------------------------------------------
// $Maintainer: Douglas McCloskey $
// $Authors: Douglas McCloskey $
// --------------------------------------------------------------------------

#include <SmartPeak/test_config.h>

#define BOOST_TEST_MODULE ParametersParser test suite
#include <boost/test/included/unit_test.hpp>
#include <SmartPeak/io/ParametersParser.h>

using namespace SmartPeak;
using namespace std;

BOOST_AUTO_TEST_SUITE(parameterparser)

BOOST_AUTO_TEST_CASE(read)
{
  const string pathname = SMARTPEAK_GET_TEST_DATA_PATH("FileReader_parameters.csv");
  ParameterSet parameters;
  ParametersParser::read(pathname, parameters);
  BOOST_CHECK_EQUAL(parameters.size(), 3);

  const string func1 {"func1"};

  BOOST_REQUIRE(parameters.count(func1));
  auto function_parameters = parameters.at(func1);
  BOOST_REQUIRE(function_parameters.size() == 3);
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
  BOOST_REQUIRE(function_parameters.size() == 2);
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
  BOOST_REQUIRE(function_parameters.size() == 1);
  BOOST_CHECK_EQUAL(function_parameters[0].getName(), "param1");
  BOOST_CHECK_EQUAL(function_parameters[0].getType(), "bool");
  BOOST_CHECK_EQUAL(function_parameters[0].getValueAsString(), "false");
  BOOST_CHECK_EQUAL(function_parameters[0].getDescription(), "");
}

BOOST_AUTO_TEST_CASE(write)
{
  const string pathname = SMARTPEAK_GET_TEST_DATA_PATH("FileReader_parameters.csv");
  ParameterSet parameters;
  ParametersParser::read(pathname, parameters);
  BOOST_CHECK_EQUAL(parameters.size(), 3);

  const string func1{ "func1" };

  BOOST_REQUIRE(parameters.count(func1));
  auto function_parameters = parameters.at(func1);
  BOOST_REQUIRE(function_parameters.size() == 3);
  auto parameter1 = function_parameters.findParameter("param1");
  BOOST_REQUIRE(parameter1);
  BOOST_CHECK_EQUAL(parameter1->getName(), "param1");
  BOOST_CHECK_EQUAL(parameter1->getType(), "int");
  BOOST_CHECK_EQUAL(parameter1->getValueAsString(), "-1");
  BOOST_CHECK_EQUAL(parameter1->getDescription(), "");

  // change values, add parameters
  parameter1->setTags({ "one", "two", "three" });
  parameter1->setValueFromString("42");
  parameter1->setDescription("a description");
  FunctionParameters new_params("new_params", {
    { {"name", "nn_thresholds"}, {"type", "list"}, {"value", "[4,4]"} },
    { {"name", "locality_weights"}, {"type", "list"}, {"value", "[False,False,False,True]"} },
    { {"name", "select_transition_groups"}, {"type", "list"}, {"value", "[True,True,True,True]"} },
    { {"name", "segment_window_lengths"}, {"type", "list"}, {"value", "[8,-1]"} },
    { {"name", "segment_step_lengths"}, {"type", "list"}, {"value", "[4,-1]"} },
    { {"name", "variable_types"}, {"type", "list"}, {"value", "['integer','integer','integer','integer']"} },
    { {"name", "optimal_thresholds"}, {"type", "list"}, {"value", "[0.5,0.5,0.5,0.5]"} }
    });
  parameters.addFunctionParameters(new_params);

  // save
  std::string written_filename = std::tmpnam(nullptr);
  ParametersParser::write(written_filename, parameters);

  // read back
  ParameterSet written_parameters;
  ParametersParser::read(written_filename, written_parameters);
  BOOST_CHECK_EQUAL(written_parameters.size(), 4);
  BOOST_REQUIRE(written_parameters.count(func1));
  auto written_function_parameters = written_parameters.at(func1);
  auto written_parameter1 = function_parameters.findParameter("param1");
  BOOST_CHECK_EQUAL(written_parameter1->getName(), "param1");
  BOOST_CHECK_EQUAL(written_parameter1->getType(), "int");
  BOOST_CHECK_EQUAL(written_parameter1->getValueAsString(), "42");
  BOOST_CHECK_EQUAL(written_parameter1->getDescription(), "a description");
  BOOST_REQUIRE(written_parameters.count("new_params"));
  auto written_function_parameters_new = written_parameters.at("new_params");
  auto written_parameter_new = written_function_parameters_new.findParameter("segment_window_lengths");
  BOOST_CHECK_EQUAL(written_parameter_new->getName(), "segment_window_lengths");
  BOOST_CHECK_EQUAL(written_parameter_new->getType(), "int_list");
  BOOST_CHECK_EQUAL(written_parameter_new->getValueAsString(), "[8,-1]");
  BOOST_CHECK_EQUAL(written_parameter_new->getDescription(), "");
}

BOOST_AUTO_TEST_SUITE_END()
