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
// $Maintainer: Douglas McCloskey, Ahmed Khalil $
// $Authors: Douglas McCloskey $
// --------------------------------------------------------------------------

#include <gtest/gtest.h>
#include <SmartPeak/test_config.h>
#include <SmartPeak/io/ParametersParser.h>

using namespace SmartPeak;
using namespace std;

TEST(ParametersParser, read)
{
  const string pathname = SMARTPEAK_GET_TEST_DATA_PATH("FileReader_parameters.csv");
  ParameterSet parameters;
  ParametersParser::read(pathname, parameters);
  EXPECT_EQ(parameters.size(), 3);

  const string func1 {"func1"};

  ASSERT_TRUE(parameters.count(func1));
  auto function_parameters = parameters.at(func1);
  ASSERT_TRUE(function_parameters.size() == 3);
  EXPECT_STREQ(function_parameters[0].getName().c_str(), "param1");
  EXPECT_STREQ(function_parameters[0].getType().c_str(), "int");
  EXPECT_STREQ(function_parameters[0].getValueAsString().c_str(), "-1");
  EXPECT_STREQ(function_parameters[0].getDescription().c_str(), "");

  EXPECT_STREQ(function_parameters[1].getName().c_str(), "param2");
  EXPECT_STREQ(function_parameters[1].getType().c_str(), "float");
  EXPECT_STREQ(function_parameters[1].getValueAsString().c_str(), "-1");
  EXPECT_STREQ(function_parameters[1].getDescription().c_str(), "a description");

  EXPECT_STREQ(function_parameters[2].getName().c_str(), "param3");
  EXPECT_STREQ(function_parameters[2].getType().c_str(), "float");
  EXPECT_STREQ(function_parameters[2].getValueAsString().c_str(), "-1");
  EXPECT_STREQ(function_parameters[2].getDescription().c_str(), "a quoted description, containing a comma");

  const string func2 {"func2"};

  ASSERT_TRUE(parameters.count(func2));
  function_parameters = parameters.at(func2);
  ASSERT_TRUE(function_parameters.size() == 2);
  EXPECT_STREQ(function_parameters[0].getName().c_str(), "param1");
  EXPECT_STREQ(function_parameters[0].getType().c_str(), "float");
  EXPECT_STREQ(function_parameters[0].getValueAsString().c_str(), "0.5");
  EXPECT_STREQ(function_parameters[0].getDescription().c_str(), "");

  EXPECT_STREQ(function_parameters[1].getName().c_str(), "param2");
  EXPECT_STREQ(function_parameters[1].getType().c_str(), "bool");
  EXPECT_STREQ(function_parameters[1].getValueAsString().c_str(), "false");
  EXPECT_STREQ(function_parameters[1].getDescription().c_str(), "");

  const string func3 {"func3"};

  ASSERT_TRUE(parameters.count(func3));
  function_parameters = parameters.at(func3);
  ASSERT_TRUE(function_parameters.size() == 1);
  EXPECT_STREQ(function_parameters[0].getName().c_str(), "param1");
  EXPECT_STREQ(function_parameters[0].getType().c_str(), "bool");
  EXPECT_STREQ(function_parameters[0].getValueAsString().c_str(), "false");
  EXPECT_STREQ(function_parameters[0].getDescription().c_str(), "");
}

TEST(ParametersParser, read_invalid)
{
  const string pathname = SMARTPEAK_GET_TEST_DATA_PATH("FileReader_parameters_invalid.csv");
  ParameterSet parameters;
  try {
    ParametersParser::read(pathname, parameters);
    FAIL() << "Expected std::exception";
  }
  catch (std::exception const& err) {
  }
  catch (...) {
    FAIL() << "Expected std::exception";
  }
}

TEST(ParametersParser, write)
{
  const string pathname = SMARTPEAK_GET_TEST_DATA_PATH("FileReader_parameters.csv");
  ParameterSet parameters;
  ParametersParser::read(pathname, parameters);
  ASSERT_EQ(parameters.size(), 3);

  const string func1{ "func1" };

  ASSERT_TRUE(parameters.count(func1));
  auto function_parameters = parameters.at(func1);
  ASSERT_TRUE(function_parameters.size() == 3);
  auto parameter1 = function_parameters.findParameter("param1");
  ASSERT_TRUE(parameter1);
  EXPECT_STREQ(parameter1->getName().c_str(), "param1");
  EXPECT_STREQ(parameter1->getType().c_str(), "int");
  EXPECT_STREQ(parameter1->getValueAsString().c_str(), "-1");
  EXPECT_STREQ(parameter1->getDescription().c_str(), "");

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
  EXPECT_EQ(written_parameters.size(), 4);
  ASSERT_TRUE(written_parameters.count(func1));
  auto written_function_parameters = written_parameters.at(func1);
  auto written_parameter1 = function_parameters.findParameter("param1");
  EXPECT_STREQ(written_parameter1->getName().c_str(), "param1");
  EXPECT_STREQ(written_parameter1->getType().c_str(), "int");
  EXPECT_STREQ(written_parameter1->getValueAsString().c_str(), "42");
  EXPECT_STREQ(written_parameter1->getDescription().c_str(), "a description");
  ASSERT_TRUE(written_parameters.count("new_params"));
  auto written_function_parameters_new = written_parameters.at("new_params");
  auto written_parameter_new = written_function_parameters_new.findParameter("segment_window_lengths");
  EXPECT_STREQ(written_parameter_new->getName().c_str(), "segment_window_lengths");
  EXPECT_STREQ(written_parameter_new->getType().c_str(), "int_list");
  EXPECT_STREQ(written_parameter_new->getValueAsString().c_str(), "[8,-1]");
  EXPECT_STREQ(written_parameter_new->getDescription().c_str(), "");
}
