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
// $Authors: Douglas McCloskey, Pasquale Domenico Colaianni $
// --------------------------------------------------------------------------

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <SmartPeak/test_config.h>
#include <SmartPeak/core/Parameters.h>
#include <OpenMS/ANALYSIS/OPENSWATH/MRMFeatureFinderScoring.h>

using namespace SmartPeak;
using namespace std;

TEST(ParameterSet, ParameterSet_createFromStruct)
{
  map<std::string, vector<map<string, string>>> param_struct1({
  {"func1", {
      {
        {"name", "param1"},
        {"type", "int"},
        {"value", "23"},
        {"description", "param1 description"},
        {"tags", "tag1,tag2,tag3"}
      },
      {
        {"name", "param2"},
        {"type", "float"},
        {"value", "3.14"},
        {"tags", "tag4"}
      }
    }},
  {"func2", {
      {
        {"name", "param3"},
        {"type", "bool"},
        {"value", "true"}
      },
      {
        {"name", "param4"},
        {"type", "string"},
        {"value", "false"}
      }
    }}
  });
  ParameterSet parameter_set(param_struct1);

  EXPECT_TRUE(parameter_set.count("func1"));
  auto func1 = parameter_set["func1"];
  auto param1 = func1.findParameter("param1");
  EXPECT_TRUE(param1);
  EXPECT_STREQ(param1->getType().c_str(), "int");
  EXPECT_STREQ(param1->getValueAsString().c_str(), "23");
  EXPECT_STREQ(param1->getDescription().c_str(), "param1 description");
  EXPECT_TRUE(param1->getTags().size()==3);
  EXPECT_STREQ(param1->getTags()[0].c_str(), "tag1");
  EXPECT_STREQ(param1->getTags()[1].c_str(), "tag2");
  EXPECT_STREQ(param1->getTags()[2].c_str(), "tag3");
  auto param2 = func1.findParameter("param2");
  EXPECT_TRUE(param2);
  EXPECT_STREQ(param2->getType().c_str(), "float");
  EXPECT_STREQ(param2->getValueAsString().c_str(), "3.14");
  EXPECT_STREQ(param2->getDescription().c_str(), "");
  EXPECT_TRUE(param2->getTags().size() == 1);
  EXPECT_STREQ(param2->getTags()[0].c_str(), "tag4");

  EXPECT_TRUE(parameter_set.count("func2"));
  auto func2 = parameter_set["func2"];
  auto param3 = func2.findParameter("param3");
  EXPECT_TRUE(param3);
  EXPECT_STREQ(param3->getType().c_str(), "bool");
  EXPECT_STREQ(param3->getValueAsString().c_str(), "true");
  EXPECT_STREQ(param3->getDescription().c_str(), "");
  auto param4 = func2.findParameter("param4");
  EXPECT_TRUE(param4);
  EXPECT_STREQ(param4->getType().c_str(), "bool");
  EXPECT_STREQ(param4->getValueAsString().c_str(), "false");
  EXPECT_STREQ(param4->getDescription().c_str(), "");
}

TEST(ParameterSet, ParameterSet_createFromOpenMS)
{
  OpenMS::MRMFeatureFinderScoring oms_params;
  ParameterSet parameter_set({ oms_params });

  EXPECT_TRUE(parameter_set.count("MRMFeatureFinderScoring"));
  auto func = parameter_set["MRMFeatureFinderScoring"];

  auto param = func.findParameter("TransitionGroupPicker:stop_after_feature");
  EXPECT_TRUE(param);
  EXPECT_STREQ(param->getType().c_str(), "int");
  EXPECT_STREQ(param->getValueAsString().c_str(), "-1");
  EXPECT_STREQ(param->getDescription().c_str(), "Stop finding after feature (ordered by intensity; -1 means do not stop).");

  param = func.findParameter("TransitionGroupPicker:peak_integration");
  EXPECT_TRUE(param);
  EXPECT_STREQ(param->getType().c_str(), "string");
  EXPECT_STREQ(param->getValueAsString().c_str(), "original");
  EXPECT_STREQ(param->getRestrictionsAsString().c_str(), "[original,smoothed]");

  param = func.findParameter("add_up_spectra");
  EXPECT_TRUE(param);
  EXPECT_STREQ(param->getType().c_str(), "int");
  EXPECT_STREQ(param->getValueAsString().c_str(), "1");
  EXPECT_STREQ(param->getRestrictionsAsString().c_str(), "min:1");
}

TEST(ParameterSet, ParameterSet_merge)
{
  map<std::string, vector<map<string, string>>> param_struct1({
  {"func1", {
    { {"name", "param1"}, {"type", "int"}, {"value", "23"}, {"description", "param1 description"}, {"tags", "tag1,tag2,tag3"} },
    { {"name", "param2"}, {"type", "float"}, {"value", "3.14"}, {"tags", "tag4"} }
  }},
  {"func2", { 
    { {"name", "param3"}, {"type", "bool"}, {"value", "true"} },
  }}
  });

  map<std::string, vector<map<string, string>>> param_struct2({
  {"func1", {
    { {"name", "param1"}, {"type", "int"}, {"value", "23"}, {"description", "param1 description"}, {"tags", "tag1,tag2,tag3"} },
    { {"name", "param2"}, {"type", "string"}, {"value", "override"}, {"tags", "tag4"} }
  }},
  {"func2", {
    { {"name", "param4_merged"}, {"type", "string"}, {"value", "merged"} }
  }},
  {"func3", {
    { {"name", "param5_merged"}, {"type", "int"}, {"value", "42"} },
  }}
  });

  ParameterSet parameter_set1(param_struct1);
  ParameterSet parameter_set2(param_struct2);
  parameter_set1.merge(parameter_set2);

  EXPECT_TRUE(parameter_set1.count("func1"));
  auto func1 = parameter_set1["func1"];
  auto param1 = func1.findParameter("param1");
  EXPECT_EQ(func1.size(), 2);
  EXPECT_TRUE(param1);
  EXPECT_STREQ(param1->getType().c_str(), "int");
  EXPECT_STREQ(param1->getValueAsString().c_str(), "23");
  EXPECT_STREQ(param1->getDescription().c_str(), "param1 description");
  EXPECT_TRUE(param1->getTags().size() == 3);
  EXPECT_STREQ(param1->getTags()[0].c_str(), "tag1");
  EXPECT_STREQ(param1->getTags()[1].c_str(), "tag2");
  EXPECT_STREQ(param1->getTags()[2].c_str(), "tag3");
  auto param2 = func1.findParameter("param2");
  EXPECT_TRUE(param2);
  EXPECT_STREQ(param2->getType().c_str(), "float");
  EXPECT_STREQ(param2->getValueAsString().c_str(), "3.14");
  EXPECT_STREQ(param2->getDescription().c_str(), "");
  EXPECT_TRUE(param2->getTags().size() == 1);
  EXPECT_STREQ(param2->getTags()[0].c_str(), "tag4");

  EXPECT_TRUE(parameter_set1.count("func2"));
  auto func2 = parameter_set1["func2"];
  auto param3 = func2.findParameter("param3");
  EXPECT_TRUE(param3);
  EXPECT_STREQ(param3->getType().c_str(), "bool");
  EXPECT_STREQ(param3->getValueAsString().c_str(), "true");
  auto param4 = func2.findParameter("param4_merged");
  EXPECT_TRUE(param4);
  EXPECT_STREQ(param4->getType().c_str(), "string");
  EXPECT_STREQ(param4->getValueAsString().c_str(), "merged");

  EXPECT_TRUE(parameter_set1.count("func3"));
  auto func3 = parameter_set1["func3"];
  auto param5 = func3.findParameter("param5_merged");
  EXPECT_TRUE(param5);
  EXPECT_STREQ(param5->getType().c_str(), "int");
  EXPECT_STREQ(param5->getValueAsString().c_str(), "42");
}

TEST(ParameterSet, ParameterSet_setAsSchema)
{
  map<std::string, vector<map<string, string>>> param_struct1({
  {"func1", {
    { {"name", "invalid_param_int"}, {"type", "int"}, {"value", "23"},  {"description", "invalid_param_int description"}, {"tags", "tag1,tag2,tag3"} },
    { {"name", "invalid_string"}, {"type", "string"}, {"value", "invalid_string"} },
    { {"name", "valid_param_float"}, {"type", "float"}, {"value", "42.0"}, {"min","0"}, {"max","10"}},
    { {"name", "valid_string"}, {"type", "string"}, {"value", "valid_string1"} },
    { {"name", "type_override"}, {"type", "string"}, {"value", "type_override"} }
  }},
  {"func2", {
    { {"name", "param3"}, {"type", "bool"}, {"value", "true"} },
    { {"name", "param3"}, {"type", "bool"}, {"value", "true"} },
  }}
  });
  map<std::string, vector<map<string, string>>> param_schema({
  {"func1", {
    { {"name", "invalid_param_int"}, {"type", "int"}, {"value", "23"}, {"description", "invalid_param_int scheme description"}, {"tags", "scheme_tag1,scheme_tag2,scheme_tag3"}, {"min","0"}, {"max","10"} },
    { {"name", "invalid_string"}, {"type", "string"}, {"value", "valid_string1"}, {"tags", "tag4"}, {"valid_strings","[\"valid_string1\",\"valid_string2\",\"valid_string3\"]"} },
    { {"name", "valid_param_float"}, {"type", "float"}, {"value", "10"}, {"description", "param1 description"}, {"tags", "scheme_tag1"}, {"min","0"}, {"max","100"} },
    { {"name", "valid_string"}, {"type", "string"}, {"value", "valid_string1"}, {"tags", "tag4"}, {"valid_strings","[\"valid_string1\",\"valid_string2\",\"valid_string3\"]"} },
    { {"name", "unused_param"}, {"type", "string"}, {"value", "unused_param"}, {"tags", "tag4"} },
    { {"name", "type_override"}, {"type", "int"}, {"value", "42"} }
  }},
  {"func2", {
    { {"name", "param4_merged"}, {"type", "string"}, {"value", "merged"} }
  }},
  {"func3", {
    { {"name", "param5_merged"}, {"type", "int"}, {"value", "42"} },
  }}
  });

  ParameterSet parameter_set1(param_struct1);
  ParameterSet parameter_set_schema(param_schema);
  parameter_set_schema.setAsSchema(true);
  parameter_set1.merge(parameter_set_schema);

  EXPECT_TRUE(parameter_set1.count("func1"));
  auto func1 = parameter_set1["func1"];
  EXPECT_EQ(func1.size(), 6);

  auto param = func1.findParameter("invalid_param_int");
  EXPECT_TRUE(param);
  EXPECT_STREQ(param->getType().c_str(), "int");
  EXPECT_STREQ(param->getValueAsString().c_str(), "23");
  EXPECT_STREQ(param->getDescription().c_str(), "invalid_param_int scheme description");
  EXPECT_TRUE(param->getTags().size() == 3);
  EXPECT_STREQ(param->getTags()[0].c_str(), "scheme_tag1");
  EXPECT_STREQ(param->getTags()[1].c_str(), "scheme_tag2");
  EXPECT_STREQ(param->getTags()[2].c_str(), "scheme_tag3");
  EXPECT_FALSE(param->isValid());
  EXPECT_FALSE(param->isSchema());

  param = func1.findParameter("invalid_string");
  EXPECT_TRUE(param);
  EXPECT_STREQ(param->getType().c_str(), "string");
  EXPECT_STREQ(param->getValueAsString().c_str(), "invalid_string");
  EXPECT_TRUE(param->getTags().size() == 1);
  EXPECT_STREQ(param->getTags()[0].c_str(), "tag4");
  EXPECT_FALSE(param->isValid());
  EXPECT_FALSE(param->isSchema());

  param = func1.findParameter("valid_param_float");
  EXPECT_TRUE(param);
  EXPECT_STREQ(param->getType().c_str(), "float");
  EXPECT_STREQ(param->getValueAsString().c_str(), "42");
  EXPECT_TRUE(param->getTags().size() == 1);
  EXPECT_STREQ(param->getTags()[0].c_str(), "scheme_tag1");
  EXPECT_TRUE(param->isValid());
  EXPECT_FALSE(param->isSchema());

  param = func1.findParameter("valid_string");
  EXPECT_TRUE(param);
  EXPECT_STREQ(param->getType().c_str(), "string");
  EXPECT_STREQ(param->getValueAsString().c_str(), "valid_string1");
  EXPECT_TRUE(param->getTags().size() == 1);
  EXPECT_STREQ(param->getTags()[0].c_str(), "tag4");
  EXPECT_TRUE(param->isValid());
  EXPECT_FALSE(param->isSchema());

  param = func1.findParameter("unused_param");
  EXPECT_TRUE(param);
  EXPECT_STREQ(param->getType().c_str(), "string");
  EXPECT_STREQ(param->getValueAsString().c_str(), "unused_param");
  EXPECT_TRUE(param->isValid());
  EXPECT_TRUE(param->isSchema());

  param = func1.findParameter("type_override");
  EXPECT_TRUE(param);
  EXPECT_STREQ(param->getType().c_str(), "string");
  EXPECT_STREQ(param->getValueAsString().c_str(), "type_override");
  EXPECT_FALSE(param->isValid());
  EXPECT_FALSE(param->isSchema());
}

TEST(ParameterSet, ParameterSet_findParameter)
{
  map<std::string, vector<map<string, string>>> param_struct1({
  {"one_function", {
    { {"name", "one_parameter"}, {"type", "string"}, {"value", "valid_string1"} },
  }}});

  ParameterSet parameter_set1(param_struct1);
  auto param = parameter_set1.findParameter("one_function", "one_parameter");
  EXPECT_TRUE(param);
  EXPECT_TRUE(param->getName() == "one_parameter");
  EXPECT_TRUE(param->getValueAsString() == "valid_string1");
  EXPECT_TRUE(param->getType() == "string");

  // non existing functions / parameters
  EXPECT_TRUE(!parameter_set1.findParameter("one_function", "non_existing_parameter"));
  EXPECT_TRUE(!parameter_set1.findParameter("non_existing_function", "one_parameter"));
  EXPECT_TRUE(!parameter_set1.findParameter("non_existing_function", "one_parameter"));
}

TEST(ParameterSet, ParameterSet_addFunctionParameters)
{
  map<std::string, vector<map<string, string>>> param_struct1({
  {"one_function", {
    { {"name", "one_parameter"}, {"type", "string"}, {"value", "valid_string1"} },
  }} });
  ParameterSet parameter_set1(param_struct1);

  // Nominal usage
  vector<map<string, string>> param_struct2({
  {
    { {"name", "another_parameter"}, {"type", "string"}, {"value", "valid_string2"} },
  } });
  FunctionParameters function_parameter("another_function", param_struct2);
  parameter_set1.addFunctionParameters(function_parameter);
  auto param = parameter_set1.findParameter("another_function", "another_parameter");
  EXPECT_TRUE(param);
  EXPECT_TRUE(param->getName() == "another_parameter");
  EXPECT_TRUE(param->getValueAsString() == "valid_string2");
  EXPECT_TRUE(param->getType() == "string");

  // Add an already existing FunctionParameter - replace.
  vector<map<string, string>> param_struct3({
  {
    { {"name", "another_parameter_replace"}, {"type", "string"}, {"value", "valid_string3"} },
  } });
  FunctionParameters function_parameter2("another_function", param_struct3);
  parameter_set1.addFunctionParameters(function_parameter2);
  EXPECT_TRUE(!parameter_set1.findParameter("another_function", "another_parameter"));
  param = parameter_set1.findParameter("another_function", "another_parameter_replace");
  EXPECT_TRUE(param);
  EXPECT_TRUE(param->getName() == "another_parameter_replace");
  EXPECT_TRUE(param->getValueAsString() == "valid_string3");
  EXPECT_TRUE(param->getType() == "string");
}

TEST(ParameterSet, ParameterSet_equal)
{
  map<std::string, vector<map<string, string>>> param_struct1({
  {"one_function", {
    { {"name", "one_parameter"}, {"type", "string"}, {"value", "valid_string1"} },
    { {"name", "another_parameter"}, {"type", "string"}, {"value", "valid_string1"} },
  }} });
  ParameterSet parameter_set1(param_struct1);

  map<std::string, vector<map<string, string>>> param_struct2({
  {"one_function", {
    { {"name", "one_parameter"}, {"type", "string"}, {"value", "valid_string1"} },
    { {"name", "another_parameter"}, {"type", "string"}, {"value", "valid_string1"} },
  }} });
  ParameterSet parameter_set2(param_struct2);

  EXPECT_TRUE(parameter_set1 == parameter_set2);

  map<std::string, vector<map<string, string>>> param_struct3({
  {"one_function", {
    { {"name", "one_parameter"}, {"type", "string"}, {"value", "valid_string1"} },
    { {"name", "another_parameter"}, {"type", "string"}, {"value", "valid_string1"} },
    { {"name", "and_another_parameter"}, {"type", "string"}, {"value", "valid_string1"} },
  }} });
  ParameterSet parameter_set3(param_struct3);

  EXPECT_FALSE(parameter_set1 == parameter_set3);
}

TEST(ParameterSet, ParameterSet_accessors)
{
  map<std::string, vector<map<string, string>>> param_struct1({
  {"func1", {
    { {"name", "invalid_param_int"}, {"type", "int"}, {"value", "23"},  {"description", "invalid_param_int description"}, {"tags", "tag1,tag2,tag3"} },
    { {"name", "invalid_string"}, {"type", "string"}, {"value", "invalid_string"} },
    { {"name", "valid_param_float"}, {"type", "float"}, {"value", "42.0"}, {"min","0"}, {"max","10"}},
    { {"name", "valid_string"}, {"type", "string"}, {"value", "valid_string1"} },
    { {"name", "type_override"}, {"type", "string"}, {"value", "type_override"} }
  }},
  {"func2", {
    { {"name", "param3"}, {"type", "bool"}, {"value", "true"} },
    { {"name", "param3"}, {"type", "bool"}, {"value", "true"} },
  }}
  });
  ParameterSet parameter_set1(param_struct1);

  EXPECT_TRUE(!parameter_set1.empty());
  EXPECT_EQ(parameter_set1.size(), 2);
  EXPECT_EQ(parameter_set1.count("non_existing_function"), 0);
  EXPECT_EQ(parameter_set1.count("func1"), 1);
  EXPECT_STREQ(parameter_set1.at("func1").getFunctionName().c_str(), "func1");
  EXPECT_STREQ(parameter_set1["func1"].getFunctionName().c_str(), "func1");
  std::set<std::string> function_names;
  for (const auto& f : parameter_set1)
  {
    function_names.insert(f.second.getFunctionName());
  }
  std::set<std::string> expected{ "func1","func2" };
  EXPECT_THAT(function_names, ::testing::ContainerEq(expected));

  parameter_set1.clear();
  EXPECT_TRUE(parameter_set1.empty());
  EXPECT_EQ(parameter_set1.size(), 0);
  EXPECT_EQ(parameter_set1.count("func1"), 0);
}

TEST(ParameterSet, FunctionParameter_constructWithName)
{
  FunctionParameters function_parameter("function");

  EXPECT_STREQ(function_parameter.getFunctionName().c_str(), "function");
  EXPECT_EQ(function_parameter.size(), 0);
}

TEST(ParameterSet, FunctionParameter_constructFromStruct)
{
  vector<map<string, string>> param_struct1({
  {
    { {"name", "parameter1"}, {"type", "string"}, {"value", "valid_string1"} },
    { {"name", "parameter2"}, {"type", "string"}, {"value", "valid_string2"} },
  } });
  FunctionParameters function_parameter("function", param_struct1);

  EXPECT_STREQ(function_parameter.getFunctionName().c_str(), "function");
  EXPECT_EQ(function_parameter.size(), 2);
}

TEST(ParameterSet, FunctionParameter_constructFromOpenMS)
{
  OpenMS::MRMFeatureFinderScoring oms_params;
  FunctionParameters function_parameter(oms_params);

  EXPECT_STREQ(function_parameter.getFunctionName().c_str(), "MRMFeatureFinderScoring");

  auto param = function_parameter.findParameter("TransitionGroupPicker:stop_after_feature");
  EXPECT_TRUE(param);
  EXPECT_STREQ(param->getType().c_str(), "int");
  EXPECT_STREQ(param->getValueAsString().c_str(), "-1");
  EXPECT_STREQ(param->getDescription().c_str(), "Stop finding after feature (ordered by intensity; -1 means do not stop).");

  param = function_parameter.findParameter("TransitionGroupPicker:peak_integration");
  EXPECT_TRUE(param);
  EXPECT_STREQ(param->getType().c_str(), "string");
  EXPECT_STREQ(param->getValueAsString().c_str(), "original");
  EXPECT_STREQ(param->getRestrictionsAsString().c_str(), "[original,smoothed]");

  param = function_parameter.findParameter("add_up_spectra");
  EXPECT_TRUE(param);
  EXPECT_STREQ(param->getType().c_str(), "int");
  EXPECT_STREQ(param->getValueAsString().c_str(), "1");
  EXPECT_STREQ(param->getRestrictionsAsString().c_str(), "min:1");
}

TEST(ParameterSet, FunctionParameter_findParameter)
{
  vector<map<string, string>> param_struct1({
  {
    { {"name", "parameter1"}, {"type", "string"}, {"value", "valid_string1"} },
    { {"name", "parameter2"}, {"type", "string"}, {"value", "valid_string2"} },
  } });
  FunctionParameters function_parameter("function", param_struct1);

  auto param = function_parameter.findParameter("parameter1");
  EXPECT_TRUE(param);
  EXPECT_STREQ(param->getName().c_str(), "parameter1");

  param = function_parameter.findParameter("parameter2");
  EXPECT_TRUE(param);
  EXPECT_STREQ(param->getName().c_str(), "parameter2");

  param = function_parameter.findParameter("non_existing_parameter");
  EXPECT_TRUE(!param);
}

TEST(ParameterSet, FunctionParameter_addParameter)
{
  vector<map<string, string>> param_struct1({
  {
    { {"name", "parameter1"}, {"type", "string"}, {"value", "valid_string1"} },
  } });
  FunctionParameters function_parameter("function", param_struct1);

  map<string, string > param_struct2(
    { {"name", "parameter2"}, { "type", "string" }, { "value", "valid_string2" } });
  Parameter new_param(param_struct2);

  function_parameter.addParameter(new_param);
  EXPECT_EQ(function_parameter.size(), 2);
  auto param = function_parameter.findParameter("parameter2");
  EXPECT_TRUE(param);
  EXPECT_STREQ(param->getName().c_str(), "parameter2");
  EXPECT_STREQ(param->getValueAsString().c_str(), "valid_string2");

  // addParameter with an existing parameter does not replace.
  map<string, string > param_struct3(
    { {"name", "parameter2"}, { "type", "int" }, { "value", "42" } });
  Parameter new_param2(param_struct3);
  function_parameter.addParameter(new_param2);
  EXPECT_EQ(function_parameter.size(), 2);
  param = function_parameter.findParameter("parameter2");
  EXPECT_TRUE(param);
  EXPECT_STREQ(param->getName().c_str(), "parameter2");
  EXPECT_STREQ(param->getValueAsString().c_str(), "valid_string2");
}

TEST(ParameterSet, FunctionParameter_removeParameter)
{
  vector<map<string, string>> param_struct1({
  {
    { {"name", "parameter1"}, {"type", "string"}, {"value", "valid_string1"} },
    { {"name", "parameter2"}, {"type", "string"}, {"value", "valid_string2"} },
    { {"name", "parameter3"}, {"type", "string"}, {"value", "valid_string3"} },
  } });
  FunctionParameters function_parameter("function", param_struct1);

  EXPECT_EQ(function_parameter.size(), 3);

  function_parameter.removeParameter("parameter2");
  EXPECT_EQ(function_parameter.size(), 2);
  auto param = function_parameter.findParameter("parameter1");
  EXPECT_TRUE(param);
  EXPECT_STREQ(param->getName().c_str(), "parameter1");
  EXPECT_STREQ(param->getValueAsString().c_str(), "valid_string1");
  param = function_parameter.findParameter("parameter2");
  EXPECT_EQ(param, nullptr);
  param = function_parameter.findParameter("parameter3");
  EXPECT_TRUE(param);
  EXPECT_STREQ(param->getName().c_str(), "parameter3");
  EXPECT_STREQ(param->getValueAsString().c_str(), "valid_string3");
  param = function_parameter.findParameter("parameter3");

  function_parameter.removeParameter("non_existing_parameter");
  EXPECT_EQ(function_parameter.size(), 2);
}

TEST(ParameterSet, FunctionParameter_merge)
{
  vector<map<string, string>> param_struct1({
  {
    { {"name", "parameter1"}, {"type", "string"}, {"value", "valid_string1"} },
  } });
  FunctionParameters function_parameter1("function", param_struct1);

  vector<map<string, string>> param_struct2({
  {
    { {"name", "parameter1"}, {"type", "string"}, {"value", "merge_valid_string1"} },
    { {"name", "parameter2"}, {"type", "string"}, {"value", "merge_valid_string2"} },
  } });
  FunctionParameters function_parameter2("function", param_struct2);

  function_parameter1.merge(function_parameter2);

  EXPECT_EQ(function_parameter1.size(), 2);
  
  auto param = function_parameter1.findParameter("parameter1");
  EXPECT_TRUE(param);
  EXPECT_STREQ(param->getName().c_str(), "parameter1");
  EXPECT_STREQ(param->getValueAsString().c_str(), "valid_string1");
  EXPECT_TRUE(!param->getSchema());
  EXPECT_STREQ(param->getDefaultValueAsString().c_str(), "valid_string1");
  
  param = function_parameter1.findParameter("parameter2");
  EXPECT_TRUE(param);
  EXPECT_STREQ(param->getName().c_str(), "parameter2");
  EXPECT_STREQ(param->getValueAsString().c_str(), "merge_valid_string2");
  EXPECT_TRUE(!param->getSchema());
  EXPECT_STREQ(param->getDefaultValueAsString().c_str(), "merge_valid_string2");
}

TEST(ParameterSet, FunctionParameter_equal)
{
  vector<map<string, string>> param_struct1({
  {
    { {"name", "parameter1"}, {"type", "string"}, {"value", "string1"} },
    { {"name", "parameter2"}, {"type", "string"}, {"value", "string2"} },
  } });
  FunctionParameters function_parameter1("function", param_struct1);

  vector<map<string, string>> param_struct2({
  {
    { {"name", "parameter1"}, {"type", "string"}, {"value", "string1"} },
    { {"name", "parameter2"}, {"type", "string"}, {"value", "string2"} },
  } });
  FunctionParameters function_parameter2("function", param_struct2);
  
  EXPECT_TRUE(function_parameter1 == function_parameter2);

  vector<map<string, string>> param_struct3({
  {
    { {"name", "parameter1"}, {"type", "string"}, {"value", "string1"} },
    { {"name", "parameter3"}, {"type", "string"}, {"value", "string3"} },
  } });
  FunctionParameters function_parameter3("function", param_struct3);

  EXPECT_FALSE(function_parameter1 == function_parameter3);
}

TEST(ParameterSet, FunctionParameter_setAsSchema)
{
  vector<map<string, string>> param_struct1({
  {
    { {"name", "parameter1"}, {"type", "string"}, {"value", "valid_string1"} },
  } });
  FunctionParameters function_parameter1("function", param_struct1);

  vector<map<string, string>> param_struct2({
  {
    { {"name", "parameter1"}, {"type", "string"}, {"value", "merge_valid_string1"} },
    { {"name", "parameter2"}, {"type", "string"}, {"value", "merge_valid_string2"} },
  } });
  FunctionParameters function_parameter2("function", param_struct2);
  function_parameter2.setAsSchema(true);

  function_parameter1.merge(function_parameter2);

  EXPECT_EQ(function_parameter1.size(), 2);

  auto param = function_parameter1.findParameter("parameter1");
  EXPECT_TRUE(param);
  EXPECT_STREQ(param->getName().c_str(), "parameter1");
  EXPECT_STREQ(param->getValueAsString().c_str(), "valid_string1");
  EXPECT_TRUE(param->getSchema());
  EXPECT_STREQ(param->getDefaultValueAsString().c_str(), "merge_valid_string1");

  param = function_parameter1.findParameter("parameter2");
  EXPECT_TRUE(param);
  EXPECT_STREQ(param->getName().c_str(), "parameter2");
  EXPECT_STREQ(param->getValueAsString().c_str(), "merge_valid_string2");
  EXPECT_TRUE(!param->getSchema());
  EXPECT_STREQ(param->getDefaultValueAsString().c_str(), "merge_valid_string2");
}

TEST(ParameterSet, FunctionParameter_accessors)
{
  vector<map<string, string>> param_struct1({
  {
    { {"name", "parameter1"}, {"type", "string"}, {"value", "merge_valid_string1"} },
    { {"name", "parameter2"}, {"type", "string"}, {"value", "merge_valid_string2"} },
  } });
  FunctionParameters function_parameter1("function", param_struct1);

  EXPECT_EQ(function_parameter1.size(), 2);
  EXPECT_TRUE(!function_parameter1.empty());
  Parameter& front = function_parameter1.front();
  EXPECT_STREQ(front.getName().c_str(), "parameter1");
  EXPECT_STREQ(function_parameter1[1].getName().c_str(), "parameter2");
  EXPECT_STREQ(function_parameter1.at(1).getName().c_str(), "parameter2");
  std::set<std::string> parameter_names;
  for (const auto& f : function_parameter1)
  {
    parameter_names.insert(f.getName());
  }
  std::set<std::string> expected{ "parameter1","parameter2" };
  EXPECT_THAT(parameter_names, ::testing::ContainerEq(expected));
}

TEST(ParameterSet, Parameter_constructorNonValue)
{
  Parameter param("test");
  EXPECT_STREQ(param.getName().c_str(), "test");
  EXPECT_STREQ(param.getType().c_str(), "uninitialized");
  EXPECT_TRUE(param.isValid());
  EXPECT_STREQ(param.getValueAsString().c_str(), "false");
  EXPECT_STREQ(param.getDescription().c_str(), "");
  EXPECT_TRUE(!param.getSchema());
  const auto& tags = param.getTags();
  EXPECT_EQ(tags.size(), 0);
  EXPECT_STREQ(param.getRestrictionsAsString().c_str(), "");
  EXPECT_TRUE(!param.isSchema());
}

TEST(ParameterSet, Parameter_constructorFromCastValue)
{
  // we use implicit casts
  Parameter param_int("param_int", 42);
  EXPECT_STREQ(param_int.getName().c_str(), "param_int");
  EXPECT_STREQ(param_int.getType().c_str(), "int");
  EXPECT_TRUE(param_int.isValid());
  EXPECT_STREQ(param_int.getValueAsString().c_str(), "42");
  EXPECT_STREQ(param_int.getDescription().c_str(), "");
  EXPECT_TRUE(!param_int.getSchema());
  EXPECT_EQ(param_int.getTags().size(), 0);
  EXPECT_STREQ(param_int.getRestrictionsAsString().c_str(), "");
  EXPECT_TRUE(!param_int.isSchema());

  Parameter param_long("param_long", 42l);
  EXPECT_STREQ(param_long.getName().c_str(), "param_long");
  EXPECT_STREQ(param_long.getType().c_str(), "long");
  EXPECT_TRUE(param_long.isValid());
  EXPECT_STREQ(param_long.getValueAsString().c_str(), "42");
  EXPECT_STREQ(param_long.getDescription().c_str(), "");
  EXPECT_TRUE(!param_long.getSchema());
  EXPECT_EQ(param_long.getTags().size(), 0);
  EXPECT_STREQ(param_long.getRestrictionsAsString().c_str(), "");
  EXPECT_TRUE(!param_long.isSchema());

  Parameter param_float("param_float", 3.14f);
  EXPECT_STREQ(param_float.getName().c_str(), "param_float");
  EXPECT_STREQ(param_float.getType().c_str(), "float");
  EXPECT_TRUE(param_float.isValid());
  EXPECT_STREQ(param_float.getValueAsString().c_str(), "3.14");
  EXPECT_STREQ(param_float.getDescription().c_str(), "");
  EXPECT_TRUE(!param_float.getSchema());
  EXPECT_EQ(param_float.getTags().size(), 0);
  EXPECT_STREQ(param_float.getRestrictionsAsString().c_str(), "");
  EXPECT_TRUE(!param_float.isSchema());

  Parameter param_bool("param_bool", true);
  EXPECT_STREQ(param_bool.getName().c_str(), "param_bool");
  EXPECT_STREQ(param_bool.getType().c_str(), "bool");
  EXPECT_TRUE(param_bool.isValid());
  EXPECT_STREQ(param_bool.getValueAsString().c_str(), "true");
  EXPECT_STREQ(param_bool.getDescription().c_str(), "");
  EXPECT_TRUE(!param_bool.getSchema());
  EXPECT_EQ(param_bool.getTags().size(), 0);
  EXPECT_STREQ(param_bool.getRestrictionsAsString().c_str(), "");
  EXPECT_TRUE(!param_bool.isSchema());

  Parameter param_string("param_string", "test");
  EXPECT_STREQ(param_string.getName().c_str(), "param_string");
  EXPECT_STREQ(param_string.getType().c_str(), "string");
  EXPECT_TRUE(param_string.isValid());
  EXPECT_STREQ(param_string.getValueAsString().c_str(), "test");
  EXPECT_STREQ(param_string.getDescription().c_str(), "");
  EXPECT_TRUE(!param_string.getSchema());
  EXPECT_EQ(param_string.getTags().size(), 0);
  EXPECT_STREQ(param_string.getRestrictionsAsString().c_str(), "");
  EXPECT_TRUE(!param_string.isSchema());
}

TEST(ParameterSet, Parameter_constructorFromStruct)
{
  std::map<std::string, std::string> param_struct_int = { {"name", "param_int"} , {"value", "42"}, {"type", "int"} };
  Parameter param_int(param_struct_int);
  EXPECT_STREQ(param_int.getName().c_str(), "param_int");
  EXPECT_STREQ(param_int.getType().c_str(), "int");
  EXPECT_TRUE(param_int.isValid());
  EXPECT_STREQ(param_int.getValueAsString().c_str(), "42");
  EXPECT_STREQ(param_int.getDescription().c_str(), "");
  EXPECT_TRUE(!param_int.getSchema());
  EXPECT_EQ(param_int.getTags().size(), 0);
  EXPECT_STREQ(param_int.getRestrictionsAsString().c_str(), "");
  EXPECT_TRUE(!param_int.isSchema());

  std::map<std::string, std::string> param_struct_long = { {"name", "param_long"} , {"value", "42"}, {"type", "long"} };
  Parameter param_long(param_struct_long);
  EXPECT_STREQ(param_long.getName().c_str(), "param_long");
  EXPECT_STREQ(param_long.getType().c_str(), "long");
  EXPECT_TRUE(param_long.isValid());
  EXPECT_STREQ(param_long.getValueAsString().c_str(), "42");
  EXPECT_STREQ(param_long.getDescription().c_str(), "");
  EXPECT_TRUE(!param_long.getSchema());
  EXPECT_EQ(param_long.getTags().size(), 0);
  EXPECT_STREQ(param_long.getRestrictionsAsString().c_str(), "");
  EXPECT_TRUE(!param_long.isSchema());

  std::map<std::string, std::string> param_struct_float = { {"name", "param_float"} , {"value", "42"}, {"type", "float"} };
  Parameter param_float(param_struct_float);
  EXPECT_STREQ(param_float.getName().c_str(), "param_float");
  EXPECT_STREQ(param_float.getType().c_str(), "float");
  EXPECT_TRUE(param_float.isValid());
  EXPECT_STREQ(param_float.getValueAsString().c_str(), "42");
  EXPECT_STREQ(param_float.getDescription().c_str(), "");
  EXPECT_TRUE(!param_float.getSchema());
  EXPECT_EQ(param_float.getTags().size(), 0);
  EXPECT_STREQ(param_float.getRestrictionsAsString().c_str(), "");
  EXPECT_TRUE(!param_float.isSchema());

  std::map<std::string, std::string> param_struct_bool = { {"name", "param_bool"} , {"value", "true"}, {"type", "bool"} };
  Parameter param_bool(param_struct_bool);
  EXPECT_STREQ(param_bool.getName().c_str(), "param_bool");
  EXPECT_STREQ(param_bool.getType().c_str(), "bool");
  EXPECT_TRUE(param_bool.isValid());
  EXPECT_STREQ(param_bool.getValueAsString().c_str(), "true");
  EXPECT_STREQ(param_bool.getDescription().c_str(), "");
  EXPECT_TRUE(!param_bool.getSchema());
  EXPECT_EQ(param_bool.getTags().size(), 0);
  EXPECT_STREQ(param_bool.getRestrictionsAsString().c_str(), "");
  EXPECT_TRUE(!param_bool.isSchema());

  std::map<std::string, std::string> param_struct_string = { {"name", "param_string"} , {"value", "test"}, {"type", "string"} };
  Parameter param_string(param_struct_string);
  EXPECT_STREQ(param_string.getName().c_str(), "param_string");
  EXPECT_STREQ(param_string.getType().c_str(), "string");
  EXPECT_TRUE(param_string.isValid());
  EXPECT_STREQ(param_string.getValueAsString().c_str(), "test");
  EXPECT_STREQ(param_string.getDescription().c_str(), "");
  EXPECT_TRUE(!param_string.getSchema());
  EXPECT_EQ(param_string.getTags().size(), 0);
  EXPECT_STREQ(param_string.getRestrictionsAsString().c_str(), "");
  EXPECT_TRUE(!param_string.isSchema());

  // no type specifier
  std::map<std::string, std::string> param_struct_int2 = { {"name", "param_int2"} , {"value", "42"} };
  Parameter param_int2(param_struct_int2);
  EXPECT_STREQ(param_int2.getName().c_str(), "param_int2");
  EXPECT_STREQ(param_int2.getType().c_str(), "int");
  EXPECT_TRUE(param_int2.isValid());
  EXPECT_STREQ(param_int2.getValueAsString().c_str(), "42");
  EXPECT_STREQ(param_int2.getDescription().c_str(), "");
  EXPECT_TRUE(!param_int2.getSchema());
  EXPECT_EQ(param_int2.getTags().size(), 0);
  EXPECT_STREQ(param_int2.getRestrictionsAsString().c_str(), "");
  EXPECT_TRUE(!param_int2.isSchema());

  std::map<std::string, std::string> param_struct_float2 = { {"name", "param_float2"} , {"value", "3.14"} };
  Parameter param_float2(param_struct_float2);
  EXPECT_STREQ(param_float2.getName().c_str(), "param_float2");
  EXPECT_STREQ(param_float2.getType().c_str(), "float");
  EXPECT_TRUE(param_float2.isValid());
  EXPECT_STREQ(param_float2.getValueAsString().c_str(), "3.14");
  EXPECT_STREQ(param_float2.getDescription().c_str(), "");
  EXPECT_TRUE(!param_float2.getSchema());
  EXPECT_EQ(param_float2.getTags().size(), 0);
  EXPECT_STREQ(param_float2.getRestrictionsAsString().c_str(), "");
  EXPECT_TRUE(!param_float2.isSchema());

  std::map<std::string, std::string> param_struct_bool2 = { {"name", "param_bool2"} , {"value", "true"} };
  Parameter param_bool2(param_struct_bool2);
  EXPECT_STREQ(param_bool2.getName().c_str(), "param_bool2");
  EXPECT_STREQ(param_bool2.getType().c_str(), "bool");
  EXPECT_TRUE(param_bool2.isValid());
  EXPECT_STREQ(param_bool2.getValueAsString().c_str(), "true");
  EXPECT_STREQ(param_bool2.getDescription().c_str(), "");
  EXPECT_TRUE(!param_bool2.getSchema());
  EXPECT_EQ(param_bool2.getTags().size(), 0);
  EXPECT_STREQ(param_bool2.getRestrictionsAsString().c_str(), "");
  EXPECT_TRUE(!param_bool2.isSchema());

  std::map<std::string, std::string> param_struct_string2 = { {"name", "param_string2"} , {"value", "test"} };
  Parameter param_string2(param_struct_string2);
  EXPECT_STREQ(param_string2.getName().c_str(), "param_string2");
  EXPECT_STREQ(param_string2.getType().c_str(), "string");
  EXPECT_TRUE(param_string2.isValid());
  EXPECT_STREQ(param_string2.getValueAsString().c_str(), "test");
  EXPECT_STREQ(param_string2.getDescription().c_str(), "");
  EXPECT_TRUE(!param_string2.getSchema());
  EXPECT_EQ(param_string2.getTags().size(), 0);
  EXPECT_STREQ(param_string2.getRestrictionsAsString().c_str(), "");
  EXPECT_TRUE(!param_string2.isSchema());

  // unknown type
  std::map<std::string, std::string> param_struct_invalid = { {"name", "param_int"} , {"value", "42"}, {"type", "invalid_type"} };
  Parameter param_invalid(param_struct_invalid);
  EXPECT_STREQ(param_invalid.getName().c_str(), "param_int");
  EXPECT_STREQ(param_invalid.getType().c_str(), "unknown");
  EXPECT_TRUE(param_invalid.isValid());
  EXPECT_STREQ(param_invalid.getValueAsString().c_str(), "42");
  EXPECT_STREQ(param_invalid.getDescription().c_str(), "");
  EXPECT_TRUE(!param_invalid.getSchema());
  EXPECT_EQ(param_invalid.getTags().size(), 0);
  EXPECT_STREQ(param_invalid.getRestrictionsAsString().c_str(), "");
  EXPECT_TRUE(!param_invalid.isSchema());

  // invalid values
  std::map<std::string, std::string> param_struct_empty_value = { {"name", "param_string"} , {"value", ""}, {"type", "int"} };
  try {
    Parameter p(param_struct_empty_value);
    FAIL() << "Expected std::exception";
  }
  catch (std::exception const& err) {
  }
  catch (...) {
    FAIL() << "Expected std::exception";
  }

  std::map<std::string, std::string> param_struct_wrong_min = { {"name", "param_string"} , {"value", "42"}, {"type", "int"}, {"min", "test"} };
  Parameter param_wrong_min(param_struct_wrong_min);
  EXPECT_EQ(param_wrong_min.getRestrictionsAsString(), "");

  std::map<std::string, std::string> param_struct_wrong_max = { {"name", "param_string"} , {"value", "42"}, {"type", "int"}, {"max", "test"} };
  Parameter param_wrong_max(param_struct_wrong_max);
  EXPECT_EQ(param_wrong_max.getRestrictionsAsString(), "");
}

TEST(ParameterSet, Parameter_constructFromParamEntry)
{
  OpenMS::Param::ParamEntry param_entry_int("test_int", 42, "test_int_description");
  Parameter param_int(param_entry_int);
  EXPECT_EQ(param_int.getType(), "int");
  EXPECT_EQ(param_int.getValueAsString(), "42");
  EXPECT_EQ(param_int.getName(), "test_int");
  EXPECT_EQ(param_int.getDescription(), "test_int_description");

  OpenMS::Param::ParamEntry param_entry_float("test_float", 3.14, "test_float_description");
  Parameter param_float(param_entry_float);
  EXPECT_EQ(param_float.getType(), "float");
  EXPECT_EQ(param_float.getValueAsString(), "3.14");
  EXPECT_EQ(param_float.getName(), "test_float");
  EXPECT_EQ(param_float.getDescription(), "test_float_description");

  OpenMS::Param::ParamEntry param_entry_string("test_string", "test", "test_string_description");
  Parameter param_string(param_entry_string);
  EXPECT_EQ(param_string.getType(), "string");
  EXPECT_EQ(param_string.getValueAsString(), "test");
  EXPECT_EQ(param_string.getName(), "test_string");
  EXPECT_EQ(param_string.getDescription(), "test_string_description");

  OpenMS::Param::ParamEntry param_entry_int_list("test_int_list", std::vector<int>({ 42, 43, 44 }), "test_int_list_description");
  Parameter param_int_list(param_entry_int_list);
  EXPECT_EQ(param_int_list.getType(), "int_list");
  EXPECT_EQ(param_int_list.getValueAsString(), "[42,43,44]");
  EXPECT_EQ(param_int_list.getName(), "test_int_list");
  EXPECT_EQ(param_int_list.getDescription(), "test_int_list_description");

  OpenMS::Param::ParamEntry param_entry_float_list("test_float_list", std::vector<double>({ 3.14, 3.15, 3.16 }), "test_float_list_description");
  Parameter param_float_list(param_entry_float_list);
  EXPECT_EQ(param_float_list.getType(), "float_list");
  EXPECT_EQ(param_float_list.getValueAsString(), "[3.14,3.15,3.16]");
  EXPECT_EQ(param_float_list.getName(), "test_float_list");
  EXPECT_EQ(param_float_list.getDescription(), "test_float_list_description");

  OpenMS::Param::ParamEntry param_entry_string_list("test_string_list", std::vector<std::string>({ "test0", "test1", "test2" }), "test_string_list_description");
  Parameter param_string_list(param_entry_string_list);
  EXPECT_EQ(param_string_list.getType(), "string_list");
  EXPECT_EQ(param_string_list.getValueAsString(), "[\'test0\',\'test1\',\'test2\']");
  EXPECT_EQ(param_string_list.getName(), "test_string_list");
  EXPECT_EQ(param_string_list.getDescription(), "test_string_list_description");
}

TEST(ParameterSet, Parameter_type)
{
  // we use implicit casts
  Parameter param_int("param_int", 42);
  EXPECT_STREQ(param_int.getType().c_str(), "int");
  EXPECT_TRUE(param_int.isValid());

  Parameter param_long("param_long", 42l);
  EXPECT_STREQ(param_long.getType().c_str(), "long");

  Parameter param_float("param_float", 3.14f);
  EXPECT_STREQ(param_float.getType().c_str(), "float");

  Parameter param_bool("param_bool", true);
  EXPECT_STREQ(param_bool.getType().c_str(), "bool");

  Parameter param_string("param_string", "test");
  EXPECT_STREQ(param_string.getType().c_str(), "string");
}

TEST(ParameterSet, Parameter_isValid)
{
  // This also test setConstraintsList and setConstraintsMinMax
  Parameter param_int("param_int", 10);

  EXPECT_TRUE(param_int.isValid());
  auto p_min_int = std::make_shared<CastValue>(0);
  auto p_max_int = std::make_shared<CastValue>(50);
  param_int.setConstraintsMinMax(p_min_int, p_max_int);
  EXPECT_TRUE(param_int.isValid());
  param_int.setValueFromString("100");
  EXPECT_TRUE(!param_int.isValid());

  param_int.setValueFromString("10");
  auto p_min_float = std::make_shared<CastValue>(0.0f);
  auto p_max_float = std::make_shared<CastValue>(50.0f);
  param_int.setConstraintsMinMax(p_min_float, p_max_float);
  EXPECT_TRUE(param_int.isValid());
  param_int.setValueFromString("100");
  EXPECT_TRUE(!param_int.isValid());
  
  Parameter param_string("param_string", "one");
  auto list_string = std::make_shared<std::vector<CastValue>>();
  list_string->push_back("one");
  list_string->push_back("two");
  list_string->push_back("three");
  param_string.setConstraintsList(list_string);
  EXPECT_TRUE(param_string.isValid());
  param_string.setValueFromString("four");
  EXPECT_TRUE(!param_string.isValid());

  // List of other types than strings should fail
  auto list_bool = std::make_shared<std::vector<CastValue>>();
  list_bool->push_back(true);
  list_bool->push_back(false);
  list_bool->push_back(true);
  param_string.setConstraintsList(list_bool);
  EXPECT_TRUE(!param_string.isValid());

  // if Value is not string, it should fail
  param_string.setValueFromString("42");
  param_string.setConstraintsList(list_string);
  EXPECT_TRUE(!param_string.isValid());
}

TEST(ParameterSet, Parameter_getValidStrings)
{
  Parameter param_string("param_string", "one");
  auto list_string = std::make_shared<std::vector<CastValue>>();
  list_string->push_back("one");
  list_string->push_back("two");
  list_string->push_back("three");
  param_string.setConstraintsList(list_string);
  std::vector<std::string> read_strings;
  for (const auto& cast_value : param_string.getValidStrings())
  {
    read_strings.push_back(cast_value);
  }
  std::vector<std::string> expected{ "one", "two", "three" };
  EXPECT_THAT(read_strings, ::testing::ContainerEq(expected));

  // tests using schema
  Parameter param_string_schema("param_string", "four");
  auto list_string_schema = std::make_shared<std::vector<CastValue>>();
  list_string_schema->push_back("four");
  list_string_schema->push_back("five");
  list_string_schema->push_back("six");
  param_string_schema.setConstraintsList(list_string_schema);
  param_string_schema.setAsSchema(true);

  read_strings.clear();
  param_string.setSchema(param_string_schema);
  for (const auto& cast_value : param_string.getValidStrings(false))
  {
    read_strings.push_back(cast_value);
  }
  EXPECT_THAT(read_strings, ::testing::ContainerEq(expected));

  read_strings.clear();
  param_string.setSchema(param_string_schema);
  for (const auto& cast_value : param_string.getValidStrings(true))
  {
    read_strings.push_back(cast_value);
  }
  std::vector<std::string> expected_schema{ "four", "five", "six" };
  EXPECT_THAT(read_strings, ::testing::ContainerEq(expected_schema));
}

TEST(ParameterSet, Parameter_name)
{
  Parameter param_int("param_int", 10);
  EXPECT_STREQ(param_int.getName().c_str(), "param_int");
  param_int.setName("new_name");
  EXPECT_STREQ(param_int.getName().c_str(), "new_name");
}

TEST(ParameterSet, Parameter_setValueFromString)
{
  Parameter param("param_int", 10);
  EXPECT_STREQ(param.getValueAsString().c_str(), "10");
  EXPECT_STREQ(param.getType().c_str(), "int");
  
  param.setValueFromString("true");
  EXPECT_STREQ(param.getValueAsString().c_str(), "true");
  EXPECT_STREQ(param.getType().c_str(), "bool");
  param.setValueFromString("True");
  EXPECT_STREQ(param.getValueAsString().c_str(), "true");
  EXPECT_STREQ(param.getType().c_str(), "bool");
  param.setValueFromString("TRUE");
  EXPECT_STREQ(param.getValueAsString().c_str(), "true");
  EXPECT_STREQ(param.getType().c_str(), "bool");
  param.setValueFromString("false");
  EXPECT_STREQ(param.getValueAsString().c_str(), "false");
  EXPECT_STREQ(param.getType().c_str(), "bool");
  param.setValueFromString("False");
  EXPECT_STREQ(param.getValueAsString().c_str(), "false");
  EXPECT_STREQ(param.getType().c_str(), "bool");
  param.setValueFromString("FALSE");
  EXPECT_STREQ(param.getValueAsString().c_str(), "false");
  EXPECT_STREQ(param.getType().c_str(), "bool");

  param.setValueFromString("test");
  EXPECT_STREQ(param.getValueAsString().c_str(), "test");
  EXPECT_STREQ(param.getType().c_str(), "string");

  param.setValueFromString("3.14");
  EXPECT_STREQ(param.getValueAsString().c_str(), "3.14");
  EXPECT_STREQ(param.getType().c_str(), "float");

  param.setValueFromString("42");
  EXPECT_STREQ(param.getValueAsString().c_str(), "42");
  EXPECT_STREQ(param.getType().c_str(), "int");

  param.setValueFromString("[1, 2, 3]");
  EXPECT_STREQ(param.getValueAsString().c_str(), "[1,2,3]");
  EXPECT_STREQ(param.getType().c_str(), "int_list");

  param.setValueFromString("[1.1, 2.2, 3.3]");
  EXPECT_STREQ(param.getValueAsString().c_str(), "[1.1,2.2,3.3]");
  EXPECT_STREQ(param.getType().c_str(), "float_list");

  param.setValueFromString("[true, FALSE, True]");
  EXPECT_STREQ(param.getValueAsString().c_str(), "[true,false,true]");
  EXPECT_STREQ(param.getType().c_str(), "bool_list");

  param.setValueFromString(" ['one','two', 'three']  ");
  EXPECT_STREQ(param.getValueAsString().c_str(), "['one','two','three']");
  EXPECT_STREQ(param.getType().c_str(), "string_list");

  param.setValueFromString(" [non_quoted_string1, non_quoted_string2, non_quoted_string3]  ");
  EXPECT_STREQ(param.getValueAsString().c_str(), "[]");
  EXPECT_STREQ(param.getType().c_str(), "string_list");

  param.setValueFromString(" non_bracket_string1, non_bracket_string2, non_bracket_string3  ");
  EXPECT_STREQ(param.getValueAsString().c_str(), "non_bracket_string1, non_bracket_string2, non_bracket_string3");
  EXPECT_STREQ(param.getType().c_str(), "string");

  // disable automatic type change when setting the value
  param.setValueFromString("3.14");
  EXPECT_STREQ(param.getType().c_str(), "float");
  param.setValueFromString("42", false);
  EXPECT_STREQ(param.getValueAsString().c_str(), "42");
  EXPECT_STREQ(param.getType().c_str(), "float");

  param.setValueFromString("test");
  EXPECT_STREQ(param.getType().c_str(), "string");
  param.setValueFromString("42", false);
  EXPECT_STREQ(param.getValueAsString().c_str(), "42");
  EXPECT_STREQ(param.getType().c_str(), "string");

  param.setValueFromString("test");
  EXPECT_STREQ(param.getType().c_str(), "string");
  param.setValueFromString("false", false);
  EXPECT_STREQ(param.getValueAsString().c_str(), "false");
  EXPECT_STREQ(param.getType().c_str(), "string");

  param.setValueFromString("test");
  EXPECT_STREQ(param.getType().c_str(), "string");
  param.setValueFromString("[1, 2, 3]", false);
  EXPECT_STREQ(param.getValueAsString().c_str(), "[1, 2, 3]");
  EXPECT_STREQ(param.getType().c_str(), "string");

  param.setValueFromString("42");
  EXPECT_STREQ(param.getType().c_str(), "int");
  param.setValueFromString("3.14", false);
  EXPECT_STREQ(param.getValueAsString().c_str(), "42");
  EXPECT_STREQ(param.getType().c_str(), "int");
}

TEST(ParameterSet, Parameter_description)
{
  Parameter param_int("param_int", 10);
  EXPECT_STREQ(param_int.getDescription().c_str(), "");
  param_int.setDescription("new_description");
  EXPECT_STREQ(param_int.getDescription().c_str(), "new_description");

  Parameter schema_param_int("param_int", 50);
  schema_param_int.setDescription("description from schema");
  schema_param_int.setAsSchema(true);
  param_int.setSchema(schema_param_int);
  EXPECT_STREQ(param_int.getDescription().c_str(), "description from schema");
  EXPECT_STREQ(param_int.getDescription(false).c_str(), "new_description");
}

TEST(ParameterSet, Parameter_schema)
{
  Parameter param_int("param_int", 10);
  EXPECT_TRUE(!param_int.getSchema());
  EXPECT_TRUE(!param_int.isSchema());
  Parameter schema_param_int("param_int", 50);
  EXPECT_TRUE(!schema_param_int.isSchema());
  schema_param_int.setAsSchema(true);
  EXPECT_TRUE(schema_param_int.isSchema());
  param_int.setSchema(schema_param_int);
  auto schema = param_int.getSchema();
  EXPECT_TRUE(schema);
  EXPECT_STREQ(schema->getName().c_str(), "param_int");
  EXPECT_TRUE(schema->isSchema());
}

TEST(ParameterSet, Parameter_tags)
{
  Parameter param_int("param_int", 10);
  EXPECT_EQ(param_int.getTags().size(), 0);

  std::vector<std::string> tags = { "one", "two", "three" };
  param_int.setTags(tags);
  EXPECT_EQ(param_int.getTags().size(), 3);
  std::vector<std::string> read_tags;
  for (const auto& t : param_int.getTags())
  {
    read_tags.push_back(t);
  }
  std::vector<std::string> expected{ "one", "two", "three" };
  EXPECT_THAT(read_tags, ::testing::ContainerEq(expected));
}

TEST(ParameterSet, Parameter_defaultvalue)
{
  Parameter param_int("param_int", 10);
  EXPECT_STREQ(param_int.getDefaultValueAsString().c_str(), "10");
  Parameter schema_param_int("param_int", 50);
  schema_param_int.setAsSchema(true);
  param_int.setSchema(schema_param_int);
  EXPECT_STREQ(param_int.getDefaultValueAsString().c_str(), "50");
}

TEST(ParameterSet, Parameter_equal)
{
  Parameter param_int_1("param_int", 10);
  Parameter param_int_2("param_int", 10);
  EXPECT_TRUE(param_int_1 == param_int_2);
  Parameter param_int_3("param_int", 42);
  EXPECT_FALSE(param_int_1 == param_int_3);
  Parameter param_int_4("param_different_name", 10);
  EXPECT_FALSE(param_int_1 == param_int_4);
  Parameter param_int_5("param_int", 10);
  param_int_5.setTags({ "tag1","tag2" });
  EXPECT_FALSE(param_int_1 == param_int_5);
  param_int_1.setDescription("description");
  EXPECT_FALSE(param_int_1 == param_int_2);
  param_int_2.setDescription("description");
  EXPECT_TRUE(param_int_1 == param_int_2);
  auto c_min = std::make_shared<CastValue>(5);
  auto c_max = std::make_shared<CastValue>(25);
  param_int_1.setConstraintsMinMax(c_min, c_max);
  EXPECT_FALSE(param_int_1 == param_int_2);
  param_int_2.setConstraintsMinMax(c_min, c_max);
  EXPECT_TRUE(param_int_1 == param_int_2);

  std::vector<int> list_int_1{ 1, 2, 3 };
  Parameter param_list_int_1("param_list_int_1", list_int_1);
  std::vector<int> list_int_2{ 1, 2, 3 };
  Parameter param_list_int_2("param_list_int_1", list_int_2);
  EXPECT_TRUE(param_list_int_1 == param_list_int_2);
  std::vector<int> list_int_3{ 1, 2, 3 };
  Parameter param_list_int_3("param_list_int_1", list_int_3);
  EXPECT_TRUE(param_list_int_1 == param_list_int_3);

}

TEST(ParameterSet, Parameter_isInList)
{
  Parameter param("param_int", 10);
  EXPECT_FALSE(param.isInList(42));
  EXPECT_FALSE(param.isInList("one"));

  param.setValueFromString("[1, 2, 3]");
  EXPECT_STREQ(param.getValueAsString().c_str(), "[1,2,3]");
  EXPECT_TRUE(param.isInList(2));
  EXPECT_FALSE(param.isInList(42));

  param.setValueFromString("[FAlSe, FALSE, false]");
  EXPECT_TRUE(param.isInList(false));
  EXPECT_FALSE(param.isInList(true));

  param.setValueFromString(" ['one','two', 'three']  ");
  EXPECT_STREQ(param.getValueAsString().c_str(), "['one','two','three']");
  EXPECT_STREQ(param.getType().c_str(), "string_list");
  EXPECT_TRUE(param.isInList("three"));
  EXPECT_FALSE(param.isInList("four"));
}

TEST(ParameterSet, Parameter_addToList)
{
  Parameter param("param_int", 10);
  param.addToList(42);
  EXPECT_STREQ(param.getValueAsString().c_str(), "10");

  param.setValueFromString("[1, 2, 3]");
  param.addToList(4);
  EXPECT_STREQ(param.getValueAsString().c_str(), "[1,2,3,4]");

  param.setValueFromString("[1.1, 2.2, 3.3]");
  param.addToList(4.4f);
  EXPECT_STREQ(param.getValueAsString().c_str(), "[1.1,2.2,3.3,4.4]");

  param.setValueFromString("[true, FALSE, True]");
  param.addToList(false);
  EXPECT_STREQ(param.getValueAsString().c_str(), "[true,false,true,false]");

  param.setValueFromString(" ['one','two', 'three']  ");
  param.addToList("four");
  EXPECT_STREQ(param.getValueAsString().c_str(), "['one','two','three','four']");
}

TEST(ParameterSet, Parameter_removeFromList)
{
  Parameter param("param_int", 10);
  param.removeFromList(42);
  EXPECT_STREQ(param.getValueAsString().c_str(), "10");

  param.setValueFromString("[1, 2, 3]");
  param.removeFromList(2);
  EXPECT_STREQ(param.getValueAsString().c_str(), "[1,3]");

  param.setValueFromString("[true, FALSE, True]");
  param.removeFromList(true);
  EXPECT_STREQ(param.getValueAsString().c_str(), "[false]");

  param.setValueFromString(" ['one','two', 'three']  ");
  param.removeFromList("one");
  EXPECT_STREQ(param.getValueAsString().c_str(), "['two','three']");
}
