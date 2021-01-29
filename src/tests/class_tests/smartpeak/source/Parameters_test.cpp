// TODO: Add copyright

#include <SmartPeak/test_config.h>

#define BOOST_TEST_MODULE Parameters test suite
#include <boost/test/included/unit_test.hpp>
#include <SmartPeak/core/Parameters.h>
#include <OpenMS/ANALYSIS/OPENSWATH/MRMFeatureFinderScoring.h>

using namespace SmartPeak;
using namespace std;

BOOST_AUTO_TEST_SUITE(parameters)

BOOST_AUTO_TEST_CASE(ParameterSet_createFromStruct)
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

  BOOST_REQUIRE(parameter_set.count("func1"));
  auto func1 = parameter_set["func1"];
  auto param1 = func1.findParameter("param1");
  BOOST_REQUIRE(param1);
  BOOST_CHECK_EQUAL(param1->getType(), "int");
  BOOST_CHECK_EQUAL(param1->getValueAsString(), "23");
  BOOST_CHECK_EQUAL(param1->getDescription(), "param1 description");
  BOOST_REQUIRE(param1->getTags().size()==3);
  BOOST_CHECK_EQUAL(param1->getTags()[0], "tag1");
  BOOST_CHECK_EQUAL(param1->getTags()[1], "tag2");
  BOOST_CHECK_EQUAL(param1->getTags()[2], "tag3");
  auto param2 = func1.findParameter("param2");
  BOOST_REQUIRE(param2);
  BOOST_CHECK_EQUAL(param2->getType(), "float");
  BOOST_CHECK_EQUAL(param2->getValueAsString(), "3.14");
  BOOST_CHECK_EQUAL(param2->getDescription(), "");
  BOOST_REQUIRE(param2->getTags().size() == 1);
  BOOST_CHECK_EQUAL(param2->getTags()[0], "tag4");

  BOOST_REQUIRE(parameter_set.count("func2"));
  auto func2 = parameter_set["func2"];
  auto param3 = func2.findParameter("param3");
  BOOST_REQUIRE(param3);
  BOOST_CHECK_EQUAL(param3->getType(), "bool");
  BOOST_CHECK_EQUAL(param3->getValueAsString(), "true");
  BOOST_CHECK_EQUAL(param3->getDescription(), "");
  auto param4 = func2.findParameter("param4");
  BOOST_REQUIRE(param4);
  BOOST_CHECK_EQUAL(param4->getType(), "bool");
  BOOST_CHECK_EQUAL(param4->getValueAsString(), "false");
  BOOST_CHECK_EQUAL(param4->getDescription(), "");
}

BOOST_AUTO_TEST_CASE(ParameterSet_createFromOpenMS)
{
  OpenMS::MRMFeatureFinderScoring oms_params;
  ParameterSet parameter_set({ oms_params });

  BOOST_REQUIRE(parameter_set.count("MRMFeatureFinderScoring"));
  auto func = parameter_set["MRMFeatureFinderScoring"];

  auto param = func.findParameter("TransitionGroupPicker:stop_after_feature");
  BOOST_REQUIRE(param);
  BOOST_CHECK_EQUAL(param->getType(), "int");
  BOOST_CHECK_EQUAL(param->getValueAsString(), "-1");
  BOOST_CHECK_EQUAL(param->getDescription(), "Stop finding after feature (ordered by intensity; -1 means do not stop).");

  param = func.findParameter("TransitionGroupPicker:peak_integration");
  BOOST_REQUIRE(param);
  BOOST_CHECK_EQUAL(param->getType(), "string");
  BOOST_CHECK_EQUAL(param->getValueAsString(), "original");
  BOOST_CHECK_EQUAL(param->getRestrictionsAsString(), "[original,smoothed]");

  param = func.findParameter("add_up_spectra");
  BOOST_REQUIRE(param);
  BOOST_CHECK_EQUAL(param->getType(), "int");
  BOOST_CHECK_EQUAL(param->getValueAsString(), "1");
  BOOST_CHECK_EQUAL(param->getRestrictionsAsString(), "min:1");
}

BOOST_AUTO_TEST_CASE(ParameterSet_merge)
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

  BOOST_REQUIRE(parameter_set1.count("func1"));
  auto func1 = parameter_set1["func1"];
  auto param1 = func1.findParameter("param1");
  BOOST_CHECK_EQUAL(func1.size(), 2);
  BOOST_REQUIRE(param1);
  BOOST_CHECK_EQUAL(param1->getType(), "int");
  BOOST_CHECK_EQUAL(param1->getValueAsString(), "23");
  BOOST_CHECK_EQUAL(param1->getDescription(), "param1 description");
  BOOST_REQUIRE(param1->getTags().size() == 3);
  BOOST_CHECK_EQUAL(param1->getTags()[0], "tag1");
  BOOST_CHECK_EQUAL(param1->getTags()[1], "tag2");
  BOOST_CHECK_EQUAL(param1->getTags()[2], "tag3");
  auto param2 = func1.findParameter("param2");
  BOOST_REQUIRE(param2);
  BOOST_CHECK_EQUAL(param2->getType(), "float");
  BOOST_CHECK_EQUAL(param2->getValueAsString(), "3.14");
  BOOST_CHECK_EQUAL(param2->getDescription(), "");
  BOOST_REQUIRE(param2->getTags().size() == 1);
  BOOST_CHECK_EQUAL(param2->getTags()[0], "tag4");

  BOOST_REQUIRE(parameter_set1.count("func2"));
  auto func2 = parameter_set1["func2"];
  auto param3 = func2.findParameter("param3");
  BOOST_REQUIRE(param3);
  BOOST_CHECK_EQUAL(param3->getType(), "bool");
  BOOST_CHECK_EQUAL(param3->getValueAsString(), "true");
  auto param4 = func2.findParameter("param4_merged");
  BOOST_REQUIRE(param4);
  BOOST_CHECK_EQUAL(param4->getType(), "string");
  BOOST_CHECK_EQUAL(param4->getValueAsString(), "merged");

  BOOST_REQUIRE(parameter_set1.count("func3"));
  auto func3 = parameter_set1["func3"];
  auto param5 = func3.findParameter("param5_merged");
  BOOST_REQUIRE(param5);
  BOOST_CHECK_EQUAL(param5->getType(), "int");
  BOOST_CHECK_EQUAL(param5->getValueAsString(), "42");
}

BOOST_AUTO_TEST_CASE(ParameterSet_setAsSchema)
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

  BOOST_REQUIRE(parameter_set1.count("func1"));
  auto func1 = parameter_set1["func1"];
  BOOST_CHECK_EQUAL(func1.size(), 6);

  auto param = func1.findParameter("invalid_param_int");
  BOOST_REQUIRE(param);
  BOOST_CHECK_EQUAL(param->getType(), "int");
  BOOST_CHECK_EQUAL(param->getValueAsString(), "23");
  BOOST_CHECK_EQUAL(param->getDescription(), "invalid_param_int scheme description");
  BOOST_REQUIRE(param->getTags().size() == 3);
  BOOST_CHECK_EQUAL(param->getTags()[0], "scheme_tag1");
  BOOST_CHECK_EQUAL(param->getTags()[1], "scheme_tag2");
  BOOST_CHECK_EQUAL(param->getTags()[2], "scheme_tag3");
  BOOST_CHECK_EQUAL(param->isValid(), false);
  BOOST_CHECK_EQUAL(param->isSchema(), false);

  param = func1.findParameter("invalid_string");
  BOOST_REQUIRE(param);
  BOOST_CHECK_EQUAL(param->getType(), "string");
  BOOST_CHECK_EQUAL(param->getValueAsString(), "invalid_string");
  BOOST_REQUIRE(param->getTags().size() == 1);
  BOOST_CHECK_EQUAL(param->getTags()[0], "tag4");
  BOOST_CHECK_EQUAL(param->isValid(), false);
  BOOST_CHECK_EQUAL(param->isSchema(), false);

  param = func1.findParameter("valid_param_float");
  BOOST_REQUIRE(param);
  BOOST_CHECK_EQUAL(param->getType(), "float");
  BOOST_CHECK_EQUAL(param->getValueAsString(), "42");
  BOOST_REQUIRE(param->getTags().size() == 1);
  BOOST_CHECK_EQUAL(param->getTags()[0], "scheme_tag1");
  BOOST_CHECK_EQUAL(param->isValid(), true);
  BOOST_CHECK_EQUAL(param->isSchema(), false);

  param = func1.findParameter("valid_string");
  BOOST_REQUIRE(param);
  BOOST_CHECK_EQUAL(param->getType(), "string");
  BOOST_CHECK_EQUAL(param->getValueAsString(), "valid_string1");
  BOOST_REQUIRE(param->getTags().size() == 1);
  BOOST_CHECK_EQUAL(param->getTags()[0], "tag4");
  BOOST_CHECK_EQUAL(param->isValid(), true);
  BOOST_CHECK_EQUAL(param->isSchema(), false);

  param = func1.findParameter("unused_param");
  BOOST_REQUIRE(param);
  BOOST_CHECK_EQUAL(param->getType(), "string");
  BOOST_CHECK_EQUAL(param->getValueAsString(), "unused_param");
  BOOST_CHECK_EQUAL(param->isValid(), true);
  BOOST_CHECK_EQUAL(param->isSchema(), true);

  param = func1.findParameter("type_override");
  BOOST_REQUIRE(param);
  BOOST_CHECK_EQUAL(param->getType(), "string");
  BOOST_CHECK_EQUAL(param->getValueAsString(), "type_override");
  BOOST_CHECK_EQUAL(param->isValid(), false);
  BOOST_CHECK_EQUAL(param->isSchema(), false);
}

BOOST_AUTO_TEST_CASE(ParameterSet_findParameter)
{
  map<std::string, vector<map<string, string>>> param_struct1({
  {"one_function", {
    { {"name", "one_parameter"}, {"type", "string"}, {"value", "valid_string1"} },
  }}});

  ParameterSet parameter_set1(param_struct1);
  auto param = parameter_set1.findParameter("one_function", "one_parameter");
  BOOST_REQUIRE(param);
  BOOST_CHECK(param->getName() == "one_parameter");
  BOOST_CHECK(param->getValueAsString() == "valid_string1");
  BOOST_CHECK(param->getType() == "string");

  // non existing functions / parameters
  BOOST_CHECK(!parameter_set1.findParameter("one_function", "non_existing_parameter"));
  BOOST_CHECK(!parameter_set1.findParameter("non_existing_function", "one_parameter"));
  BOOST_CHECK(!parameter_set1.findParameter("non_existing_function", "one_parameter"));
}

BOOST_AUTO_TEST_CASE(ParameterSet_addFunctionParameters)
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
  BOOST_REQUIRE(param);
  BOOST_CHECK(param->getName() == "another_parameter");
  BOOST_CHECK(param->getValueAsString() == "valid_string2");
  BOOST_CHECK(param->getType() == "string");

  // Add an already existing FunctionParameter - replace.
  vector<map<string, string>> param_struct3({
  {
    { {"name", "another_parameter_replace"}, {"type", "string"}, {"value", "valid_string3"} },
  } });
  FunctionParameters function_parameter2("another_function", param_struct3);
  parameter_set1.addFunctionParameters(function_parameter2);
  BOOST_CHECK(!parameter_set1.findParameter("another_function", "another_parameter"));
  param = parameter_set1.findParameter("another_function", "another_parameter_replace");
  BOOST_REQUIRE(param);
  BOOST_CHECK(param->getName() == "another_parameter_replace");
  BOOST_CHECK(param->getValueAsString() == "valid_string3");
  BOOST_CHECK(param->getType() == "string");
}

BOOST_AUTO_TEST_CASE(ParameterSet_accessors)
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

  BOOST_CHECK(!parameter_set1.empty());
  BOOST_CHECK_EQUAL(parameter_set1.size(), 2);
  BOOST_CHECK_EQUAL(parameter_set1.count("non_existing_function"), 0);
  BOOST_CHECK_EQUAL(parameter_set1.count("func1"), 1);
  BOOST_CHECK_EQUAL(parameter_set1.at("func1").getFunctionName(), "func1");
  BOOST_CHECK_EQUAL(parameter_set1["func1"].getFunctionName(), "func1");
  std::set<std::string> function_names;
  for (const auto& f : parameter_set1)
  {
    function_names.insert(f.second.getFunctionName());
  }
  std::set<std::string> expected{ "func1","func2" };
  BOOST_CHECK_EQUAL_COLLECTIONS(function_names.begin(), function_names.end(), expected.begin(), expected.end());

  parameter_set1.clear();
  BOOST_CHECK(parameter_set1.empty());
  BOOST_CHECK_EQUAL(parameter_set1.size(), 0);
  BOOST_CHECK_EQUAL(parameter_set1.count("func1"), 0);
}

BOOST_AUTO_TEST_CASE(FunctionParameter_constructWithName)
{
  FunctionParameters function_parameter("function");

  BOOST_CHECK_EQUAL(function_parameter.getFunctionName(), "function");
  BOOST_CHECK_EQUAL(function_parameter.size(), 0);
}

BOOST_AUTO_TEST_CASE(FunctionParameter_constructFromStruct)
{
  vector<map<string, string>> param_struct1({
  {
    { {"name", "parameter1"}, {"type", "string"}, {"value", "valid_string1"} },
    { {"name", "parameter2"}, {"type", "string"}, {"value", "valid_string2"} },
  } });
  FunctionParameters function_parameter("function", param_struct1);

  BOOST_CHECK_EQUAL(function_parameter.getFunctionName(), "function");
  BOOST_CHECK_EQUAL(function_parameter.size(), 2);
}

BOOST_AUTO_TEST_CASE(FunctionParameter_constructFromOpenMS)
{
  OpenMS::MRMFeatureFinderScoring oms_params;
  FunctionParameters function_parameter(oms_params);

  BOOST_CHECK_EQUAL(function_parameter.getFunctionName(), "MRMFeatureFinderScoring");

  auto param = function_parameter.findParameter("TransitionGroupPicker:stop_after_feature");
  BOOST_REQUIRE(param);
  BOOST_CHECK_EQUAL(param->getType(), "int");
  BOOST_CHECK_EQUAL(param->getValueAsString(), "-1");
  BOOST_CHECK_EQUAL(param->getDescription(), "Stop finding after feature (ordered by intensity; -1 means do not stop).");

  param = function_parameter.findParameter("TransitionGroupPicker:peak_integration");
  BOOST_REQUIRE(param);
  BOOST_CHECK_EQUAL(param->getType(), "string");
  BOOST_CHECK_EQUAL(param->getValueAsString(), "original");
  BOOST_CHECK_EQUAL(param->getRestrictionsAsString(), "[original,smoothed]");

  param = function_parameter.findParameter("add_up_spectra");
  BOOST_REQUIRE(param);
  BOOST_CHECK_EQUAL(param->getType(), "int");
  BOOST_CHECK_EQUAL(param->getValueAsString(), "1");
  BOOST_CHECK_EQUAL(param->getRestrictionsAsString(), "min:1");
}

BOOST_AUTO_TEST_CASE(FunctionParameter_findParameter)
{
  vector<map<string, string>> param_struct1({
  {
    { {"name", "parameter1"}, {"type", "string"}, {"value", "valid_string1"} },
    { {"name", "parameter2"}, {"type", "string"}, {"value", "valid_string2"} },
  } });
  FunctionParameters function_parameter("function", param_struct1);

  auto param = function_parameter.findParameter("parameter1");
  BOOST_REQUIRE(param);
  BOOST_CHECK_EQUAL(param->getName(), "parameter1");

  param = function_parameter.findParameter("parameter2");
  BOOST_REQUIRE(param);
  BOOST_CHECK_EQUAL(param->getName(), "parameter2");

  param = function_parameter.findParameter("non_existing_parameter");
  BOOST_CHECK(!param);
}

BOOST_AUTO_TEST_CASE(FunctionParameter_addParameter)
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
  BOOST_CHECK(function_parameter.size(), 2);
  auto param = function_parameter.findParameter("parameter2");
  BOOST_REQUIRE(param);
  BOOST_CHECK_EQUAL(param->getName(), "parameter2");
  BOOST_CHECK_EQUAL(param->getValueAsString(), "valid_string2");

  // addParameter with an existing parameter does not replace.
  map<string, string > param_struct3(
    { {"name", "parameter2"}, { "type", "int" }, { "value", "42" } });
  Parameter new_param2(param_struct3);
  function_parameter.addParameter(new_param2);
  BOOST_CHECK(function_parameter.size(), 2);
  param = function_parameter.findParameter("parameter2");
  BOOST_REQUIRE(param);
  BOOST_CHECK_EQUAL(param->getName(), "parameter2");
  BOOST_CHECK_EQUAL(param->getValueAsString(), "valid_string2");
}

BOOST_AUTO_TEST_CASE(FunctionParameter_merge)
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

  BOOST_CHECK(function_parameter1.size(), 2);
  
  auto param = function_parameter1.findParameter("parameter1");
  BOOST_REQUIRE(param);
  BOOST_CHECK_EQUAL(param->getName(), "parameter1");
  BOOST_CHECK_EQUAL(param->getValueAsString(), "valid_string1");
  BOOST_REQUIRE(!param->getSchema());
  BOOST_REQUIRE_EQUAL(param->getDefaultValueAsString(), "valid_string1");
  
  param = function_parameter1.findParameter("parameter2");
  BOOST_REQUIRE(param);
  BOOST_CHECK_EQUAL(param->getName(), "parameter2");
  BOOST_CHECK_EQUAL(param->getValueAsString(), "merge_valid_string2");
  BOOST_REQUIRE(!param->getSchema());
  BOOST_REQUIRE_EQUAL(param->getDefaultValueAsString(), "merge_valid_string2");
}

BOOST_AUTO_TEST_CASE(FunctionParameter_setAsSchema)
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

  BOOST_CHECK(function_parameter1.size(), 2);

  auto param = function_parameter1.findParameter("parameter1");
  BOOST_REQUIRE(param);
  BOOST_CHECK_EQUAL(param->getName(), "parameter1");
  BOOST_CHECK_EQUAL(param->getValueAsString(), "valid_string1");
  BOOST_REQUIRE(param->getSchema());
  BOOST_REQUIRE_EQUAL(param->getDefaultValueAsString(), "merge_valid_string1");

  param = function_parameter1.findParameter("parameter2");
  BOOST_REQUIRE(param);
  BOOST_CHECK_EQUAL(param->getName(), "parameter2");
  BOOST_CHECK_EQUAL(param->getValueAsString(), "merge_valid_string2");
  BOOST_REQUIRE(!param->getSchema());
  BOOST_REQUIRE_EQUAL(param->getDefaultValueAsString(), "merge_valid_string2");
}

BOOST_AUTO_TEST_CASE(FunctionParameter_accessors)
{
  vector<map<string, string>> param_struct1({
  {
    { {"name", "parameter1"}, {"type", "string"}, {"value", "merge_valid_string1"} },
    { {"name", "parameter2"}, {"type", "string"}, {"value", "merge_valid_string2"} },
  } });
  FunctionParameters function_parameter1("function", param_struct1);

  BOOST_CHECK_EQUAL(function_parameter1.size(), 2);
  BOOST_CHECK(!function_parameter1.empty());
  Parameter& front = function_parameter1.front();
  BOOST_CHECK_EQUAL(front.getName(), "parameter1");
  BOOST_CHECK_EQUAL(function_parameter1[1].getName(), "parameter2");
  BOOST_CHECK_EQUAL(function_parameter1.at(1).getName(), "parameter2");
  std::set<std::string> parameter_names;
  for (const auto& f : function_parameter1)
  {
    parameter_names.insert(f.getName());
  }
  std::set<std::string> expected{ "parameter1","parameter2" };
  BOOST_CHECK_EQUAL_COLLECTIONS(parameter_names.begin(), parameter_names.end(), expected.begin(), expected.end());
}

BOOST_AUTO_TEST_CASE(Parameter_constructorNonValue)
{
  Parameter param("test");
  BOOST_CHECK_EQUAL(param.getName(), "test");
  BOOST_CHECK_EQUAL(param.getType(), "uninitialized");
  BOOST_CHECK(param.isValid());
  BOOST_CHECK_EQUAL(param.getValueAsString(), "false");
  BOOST_CHECK_EQUAL(param.getDescription(), "");
  BOOST_CHECK(!param.getSchema());
  const auto& tags = param.getTags();
  BOOST_CHECK_EQUAL(tags.size(), 0);
  BOOST_CHECK_EQUAL(param.getRestrictionsAsString(), "");
  BOOST_CHECK(!param.isSchema());
}

BOOST_AUTO_TEST_CASE(Parameter_constructorFromCastValue)
{
  // we use implicit casts
  Parameter param_int("param_int", 42);
  BOOST_CHECK_EQUAL(param_int.getName(), "param_int");
  BOOST_CHECK_EQUAL(param_int.getType(), "int");
  BOOST_CHECK(param_int.isValid());
  BOOST_CHECK_EQUAL(param_int.getValueAsString(), "42");
  BOOST_CHECK_EQUAL(param_int.getDescription(), "");
  BOOST_CHECK(!param_int.getSchema());
  BOOST_CHECK_EQUAL(param_int.getTags().size(), 0);
  BOOST_CHECK_EQUAL(param_int.getRestrictionsAsString(), "");
  BOOST_CHECK(!param_int.isSchema());

  Parameter param_long("param_long", 42l);
  BOOST_CHECK_EQUAL(param_long.getName(), "param_long");
  BOOST_CHECK_EQUAL(param_long.getType(), "long");
  BOOST_CHECK(param_long.isValid());
  BOOST_CHECK_EQUAL(param_long.getValueAsString(), "42");
  BOOST_CHECK_EQUAL(param_long.getDescription(), "");
  BOOST_CHECK(!param_long.getSchema());
  BOOST_CHECK_EQUAL(param_long.getTags().size(), 0);
  BOOST_CHECK_EQUAL(param_long.getRestrictionsAsString(), "");
  BOOST_CHECK(!param_long.isSchema());

  Parameter param_float("param_float", 3.14f);
  BOOST_CHECK_EQUAL(param_float.getName(), "param_float");
  BOOST_CHECK_EQUAL(param_float.getType(), "float");
  BOOST_CHECK(param_float.isValid());
  BOOST_CHECK_EQUAL(param_float.getValueAsString(), "3.14");
  BOOST_CHECK_EQUAL(param_float.getDescription(), "");
  BOOST_CHECK(!param_float.getSchema());
  BOOST_CHECK_EQUAL(param_float.getTags().size(), 0);
  BOOST_CHECK_EQUAL(param_float.getRestrictionsAsString(), "");
  BOOST_CHECK(!param_float.isSchema());

  Parameter param_bool("param_bool", true);
  BOOST_CHECK_EQUAL(param_bool.getName(), "param_bool");
  BOOST_CHECK_EQUAL(param_bool.getType(), "bool");
  BOOST_CHECK(param_bool.isValid());
  BOOST_CHECK_EQUAL(param_bool.getValueAsString(), "true");
  BOOST_CHECK_EQUAL(param_bool.getDescription(), "");
  BOOST_CHECK(!param_bool.getSchema());
  BOOST_CHECK_EQUAL(param_bool.getTags().size(), 0);
  BOOST_CHECK_EQUAL(param_bool.getRestrictionsAsString(), "");
  BOOST_CHECK(!param_bool.isSchema());

  Parameter param_string("param_string", "test");
  BOOST_CHECK_EQUAL(param_string.getName(), "param_string");
  BOOST_CHECK_EQUAL(param_string.getType(), "string");
  BOOST_CHECK(param_string.isValid());
  BOOST_CHECK_EQUAL(param_string.getValueAsString(), "test");
  BOOST_CHECK_EQUAL(param_string.getDescription(), "");
  BOOST_CHECK(!param_string.getSchema());
  BOOST_CHECK_EQUAL(param_string.getTags().size(), 0);
  BOOST_CHECK_EQUAL(param_string.getRestrictionsAsString(), "");
  BOOST_CHECK(!param_string.isSchema());
}

BOOST_AUTO_TEST_CASE(Parameter_constructorFromStruct)
{
  std::map<std::string, std::string> param_struct_int = { {"name", "param_int"} , {"value", "42"}, {"type", "int"} };
  Parameter param_int(param_struct_int);
  BOOST_CHECK_EQUAL(param_int.getName(), "param_int");
  BOOST_CHECK_EQUAL(param_int.getType(), "int");
  BOOST_CHECK(param_int.isValid());
  BOOST_CHECK_EQUAL(param_int.getValueAsString(), "42");
  BOOST_CHECK_EQUAL(param_int.getDescription(), "");
  BOOST_CHECK(!param_int.getSchema());
  BOOST_CHECK_EQUAL(param_int.getTags().size(), 0);
  BOOST_CHECK_EQUAL(param_int.getRestrictionsAsString(), "");
  BOOST_CHECK(!param_int.isSchema());

  std::map<std::string, std::string> param_struct_long = { {"name", "param_long"} , {"value", "42"}, {"type", "long"} };
  Parameter param_long(param_struct_long);
  BOOST_CHECK_EQUAL(param_long.getName(), "param_long");
  BOOST_CHECK_EQUAL(param_long.getType(), "long");
  BOOST_CHECK(param_long.isValid());
  BOOST_CHECK_EQUAL(param_long.getValueAsString(), "42");
  BOOST_CHECK_EQUAL(param_long.getDescription(), "");
  BOOST_CHECK(!param_long.getSchema());
  BOOST_CHECK_EQUAL(param_long.getTags().size(), 0);
  BOOST_CHECK_EQUAL(param_long.getRestrictionsAsString(), "");
  BOOST_CHECK(!param_long.isSchema());

  std::map<std::string, std::string> param_struct_float = { {"name", "param_float"} , {"value", "42"}, {"type", "float"} };
  Parameter param_float(param_struct_float);
  BOOST_CHECK_EQUAL(param_float.getName(), "param_float");
  BOOST_CHECK_EQUAL(param_float.getType(), "float");
  BOOST_CHECK(param_float.isValid());
  BOOST_CHECK_EQUAL(param_float.getValueAsString(), "42");
  BOOST_CHECK_EQUAL(param_float.getDescription(), "");
  BOOST_CHECK(!param_float.getSchema());
  BOOST_CHECK_EQUAL(param_float.getTags().size(), 0);
  BOOST_CHECK_EQUAL(param_float.getRestrictionsAsString(), "");
  BOOST_CHECK(!param_float.isSchema());

  std::map<std::string, std::string> param_struct_bool = { {"name", "param_bool"} , {"value", "true"}, {"type", "bool"} };
  Parameter param_bool(param_struct_bool);
  BOOST_CHECK_EQUAL(param_bool.getName(), "param_bool");
  BOOST_CHECK_EQUAL(param_bool.getType(), "bool");
  BOOST_CHECK(param_bool.isValid());
  BOOST_CHECK_EQUAL(param_bool.getValueAsString(), "true");
  BOOST_CHECK_EQUAL(param_bool.getDescription(), "");
  BOOST_CHECK(!param_bool.getSchema());
  BOOST_CHECK_EQUAL(param_bool.getTags().size(), 0);
  BOOST_CHECK_EQUAL(param_bool.getRestrictionsAsString(), "");
  BOOST_CHECK(!param_bool.isSchema());

  std::map<std::string, std::string> param_struct_string = { {"name", "param_string"} , {"value", "test"}, {"type", "string"} };
  Parameter param_string(param_struct_string);
  BOOST_CHECK_EQUAL(param_string.getName(), "param_string");
  BOOST_CHECK_EQUAL(param_string.getType(), "string");
  BOOST_CHECK(param_string.isValid());
  BOOST_CHECK_EQUAL(param_string.getValueAsString(), "test");
  BOOST_CHECK_EQUAL(param_string.getDescription(), "");
  BOOST_CHECK(!param_string.getSchema());
  BOOST_CHECK_EQUAL(param_string.getTags().size(), 0);
  BOOST_CHECK_EQUAL(param_string.getRestrictionsAsString(), "");
  BOOST_CHECK(!param_string.isSchema());

  // no type specifier
  std::map<std::string, std::string> param_struct_int2 = { {"name", "param_int2"} , {"value", "42"} };
  Parameter param_int2(param_struct_int2);
  BOOST_CHECK_EQUAL(param_int2.getName(), "param_int2");
  BOOST_CHECK_EQUAL(param_int2.getType(), "int");
  BOOST_CHECK(param_int2.isValid());
  BOOST_CHECK_EQUAL(param_int2.getValueAsString(), "42");
  BOOST_CHECK_EQUAL(param_int2.getDescription(), "");
  BOOST_CHECK(!param_int2.getSchema());
  BOOST_CHECK_EQUAL(param_int2.getTags().size(), 0);
  BOOST_CHECK_EQUAL(param_int2.getRestrictionsAsString(), "");
  BOOST_CHECK(!param_int2.isSchema());

  std::map<std::string, std::string> param_struct_float2 = { {"name", "param_float2"} , {"value", "3.14"} };
  Parameter param_float2(param_struct_float2);
  BOOST_CHECK_EQUAL(param_float2.getName(), "param_float2");
  BOOST_CHECK_EQUAL(param_float2.getType(), "float");
  BOOST_CHECK(param_float2.isValid());
  BOOST_CHECK_EQUAL(param_float2.getValueAsString(), "3.14");
  BOOST_CHECK_EQUAL(param_float2.getDescription(), "");
  BOOST_CHECK(!param_float2.getSchema());
  BOOST_CHECK_EQUAL(param_float2.getTags().size(), 0);
  BOOST_CHECK_EQUAL(param_float2.getRestrictionsAsString(), "");
  BOOST_CHECK(!param_float2.isSchema());

  std::map<std::string, std::string> param_struct_bool2 = { {"name", "param_bool2"} , {"value", "true"} };
  Parameter param_bool2(param_struct_bool2);
  BOOST_CHECK_EQUAL(param_bool2.getName(), "param_bool2");
  BOOST_CHECK_EQUAL(param_bool2.getType(), "bool");
  BOOST_CHECK(param_bool2.isValid());
  BOOST_CHECK_EQUAL(param_bool2.getValueAsString(), "true");
  BOOST_CHECK_EQUAL(param_bool2.getDescription(), "");
  BOOST_CHECK(!param_bool2.getSchema());
  BOOST_CHECK_EQUAL(param_bool2.getTags().size(), 0);
  BOOST_CHECK_EQUAL(param_bool2.getRestrictionsAsString(), "");
  BOOST_CHECK(!param_bool2.isSchema());

  std::map<std::string, std::string> param_struct_string2 = { {"name", "param_string2"} , {"value", "test"} };
  Parameter param_string2(param_struct_string2);
  BOOST_CHECK_EQUAL(param_string2.getName(), "param_string2");
  BOOST_CHECK_EQUAL(param_string2.getType(), "string");
  BOOST_CHECK(param_string2.isValid());
  BOOST_CHECK_EQUAL(param_string2.getValueAsString(), "test");
  BOOST_CHECK_EQUAL(param_string2.getDescription(), "");
  BOOST_CHECK(!param_string2.getSchema());
  BOOST_CHECK_EQUAL(param_string2.getTags().size(), 0);
  BOOST_CHECK_EQUAL(param_string2.getRestrictionsAsString(), "");
  BOOST_CHECK(!param_string2.isSchema());

  // unknown type
  std::map<std::string, std::string> param_struct_invalid = { {"name", "param_int"} , {"value", "42"}, {"type", "invalid_type"} };
  Parameter param_invalid(param_struct_invalid);
  BOOST_CHECK_EQUAL(param_invalid.getName(), "param_int");
  BOOST_CHECK_EQUAL(param_invalid.getType(), "unknown");
  BOOST_CHECK(param_invalid.isValid());
  BOOST_CHECK_EQUAL(param_invalid.getValueAsString(), "42");
  BOOST_CHECK_EQUAL(param_invalid.getDescription(), "");
  BOOST_CHECK(!param_invalid.getSchema());
  BOOST_CHECK_EQUAL(param_invalid.getTags().size(), 0);
  BOOST_CHECK_EQUAL(param_invalid.getRestrictionsAsString(), "");
  BOOST_CHECK(!param_invalid.isSchema());
}

BOOST_AUTO_TEST_CASE(Parameter_type)
{
  // we use implicit casts
  Parameter param_int("param_int", 42);
  BOOST_CHECK_EQUAL(param_int.getType(), "int");
  BOOST_CHECK(param_int.isValid());

  Parameter param_long("param_long", 42l);
  BOOST_CHECK_EQUAL(param_long.getType(), "long");

  Parameter param_float("param_float", 3.14f);
  BOOST_CHECK_EQUAL(param_float.getType(), "float");

  Parameter param_bool("param_bool", true);
  BOOST_CHECK_EQUAL(param_bool.getType(), "bool");

  Parameter param_string("param_string", "test");
  BOOST_CHECK_EQUAL(param_string.getType(), "string");
}

BOOST_AUTO_TEST_CASE(Parameter_isValid)
{
  // This also test setConstraintsList and setConstraintsMinMax
  Parameter param_int("param_int", 10);

  BOOST_CHECK(param_int.isValid());
  auto p_min_int = std::make_shared<CastValue>(0);
  auto p_max_int = std::make_shared<CastValue>(50);
  param_int.setConstraintsMinMax(p_min_int, p_max_int);
  BOOST_CHECK(param_int.isValid());
  param_int.setValueFromString("100");
  BOOST_CHECK(!param_int.isValid());

  param_int.setValueFromString("10");
  auto p_min_float = std::make_shared<CastValue>(0.0f);
  auto p_max_float = std::make_shared<CastValue>(50.0f);
  param_int.setConstraintsMinMax(p_min_float, p_max_float);
  BOOST_CHECK(param_int.isValid());
  param_int.setValueFromString("100");
  BOOST_CHECK(!param_int.isValid());
  
  Parameter param_string("param_string", "one");
  auto list_string = std::make_shared<std::vector<CastValue>>();
  list_string->push_back("one");
  list_string->push_back("two");
  list_string->push_back("three");
  param_string.setConstraintsList(list_string);
  BOOST_CHECK(param_string.isValid());
  param_string.setValueFromString("four");
  BOOST_CHECK(!param_string.isValid());

  // List of other types than strings should fail
  auto list_bool = std::make_shared<std::vector<CastValue>>();
  list_bool->push_back(true);
  list_bool->push_back(false);
  list_bool->push_back(true);
  param_string.setConstraintsList(list_bool);
  BOOST_CHECK(!param_string.isValid());

  // if Value is not string, it should fail
  param_string.setValueFromString("42");
  param_string.setConstraintsList(list_string);
  BOOST_CHECK(!param_string.isValid());
}

BOOST_AUTO_TEST_CASE(Parameter_name)
{
  Parameter param_int("param_int", 10);
  BOOST_CHECK_EQUAL(param_int.getName(), "param_int");
  param_int.setName("new_name");
  BOOST_CHECK_EQUAL(param_int.getName(), "new_name");
}

BOOST_AUTO_TEST_CASE(Parameter_setValueFromString)
{
  Parameter param("param_int", 10);
  BOOST_CHECK_EQUAL(param.getValueAsString(), "10");
  BOOST_CHECK_EQUAL(param.getType(), "int");
  
  param.setValueFromString("true");
  BOOST_CHECK_EQUAL(param.getValueAsString(), "true");
  BOOST_CHECK_EQUAL(param.getType(), "bool");
  param.setValueFromString("True");
  BOOST_CHECK_EQUAL(param.getValueAsString(), "true");
  BOOST_CHECK_EQUAL(param.getType(), "bool");
  param.setValueFromString("TRUE");
  BOOST_CHECK_EQUAL(param.getValueAsString(), "true");
  BOOST_CHECK_EQUAL(param.getType(), "bool");
  param.setValueFromString("false");
  BOOST_CHECK_EQUAL(param.getValueAsString(), "false");
  BOOST_CHECK_EQUAL(param.getType(), "bool");
  param.setValueFromString("False");
  BOOST_CHECK_EQUAL(param.getValueAsString(), "false");
  BOOST_CHECK_EQUAL(param.getType(), "bool");
  param.setValueFromString("FALSE");
  BOOST_CHECK_EQUAL(param.getValueAsString(), "false");
  BOOST_CHECK_EQUAL(param.getType(), "bool");

  param.setValueFromString("test");
  BOOST_CHECK_EQUAL(param.getValueAsString(), "test");
  BOOST_CHECK_EQUAL(param.getType(), "string");

  param.setValueFromString("3.14");
  BOOST_CHECK_EQUAL(param.getValueAsString(), "3.14");
  BOOST_CHECK_EQUAL(param.getType(), "float");

  param.setValueFromString("42");
  BOOST_CHECK_EQUAL(param.getValueAsString(), "42");
  BOOST_CHECK_EQUAL(param.getType(), "int");

  param.setValueFromString("[1, 2, 3]");
  BOOST_CHECK_EQUAL(param.getValueAsString(), "[1,2,3]");
  BOOST_CHECK_EQUAL(param.getType(), "int_list");

  param.setValueFromString("[1.1, 2.2, 3.3]");
  BOOST_CHECK_EQUAL(param.getValueAsString(), "[1.1,2.2,3.3]");
  BOOST_CHECK_EQUAL(param.getType(), "float_list");

  param.setValueFromString("[true, FALSE, True]");
  BOOST_CHECK_EQUAL(param.getValueAsString(), "[true,false,true]");
  BOOST_CHECK_EQUAL(param.getType(), "bool_list");

  param.setValueFromString(" ['one','two', 'three']  ");
  BOOST_CHECK_EQUAL(param.getValueAsString(), "['one','two','three']");
  BOOST_CHECK_EQUAL(param.getType(), "string_list");

  param.setValueFromString(" [non_quoted_string1, non_quoted_string2, non_quoted_string3]  ");
  BOOST_CHECK_EQUAL(param.getValueAsString(), "[]");
  BOOST_CHECK_EQUAL(param.getType(), "string_list");

  param.setValueFromString(" non_bracket_string1, non_bracket_string2, non_bracket_string3  ");
  BOOST_CHECK_EQUAL(param.getValueAsString(), "non_bracket_string1, non_bracket_string2, non_bracket_string3");
  BOOST_CHECK_EQUAL(param.getType(), "string");
}

BOOST_AUTO_TEST_CASE(Parameter_description)
{
  Parameter param_int("param_int", 10);
  BOOST_CHECK_EQUAL(param_int.getDescription(), "");
  param_int.setDescription("new_description");
  BOOST_CHECK_EQUAL(param_int.getDescription(), "new_description");

  Parameter schema_param_int("param_int", 50);
  schema_param_int.setDescription("description from schema");
  schema_param_int.setAsSchema(true);
  param_int.setSchema(schema_param_int);
  BOOST_CHECK_EQUAL(param_int.getDescription(), "description from schema");
  BOOST_CHECK_EQUAL(param_int.getDescription(false), "new_description");
}

BOOST_AUTO_TEST_CASE(Parameter_schema)
{
  Parameter param_int("param_int", 10);
  BOOST_CHECK(!param_int.getSchema());
  BOOST_CHECK(!param_int.isSchema());
  Parameter schema_param_int("param_int", 50);
  BOOST_CHECK(!schema_param_int.isSchema());
  schema_param_int.setAsSchema(true);
  BOOST_CHECK(schema_param_int.isSchema());
  param_int.setSchema(schema_param_int);
  auto schema = param_int.getSchema();
  BOOST_REQUIRE(schema);
  BOOST_CHECK_EQUAL(schema->getName(), "param_int");
  BOOST_CHECK(schema->isSchema());
}

BOOST_AUTO_TEST_CASE(Parameter_tags)
{
  Parameter param_int("param_int", 10);
  BOOST_CHECK_EQUAL(param_int.getTags().size(), 0);

  std::vector<std::string> tags = { "one", "two", "three" };
  param_int.setTags(tags);
  BOOST_CHECK_EQUAL(param_int.getTags().size(), 3);
  std::vector<std::string> read_tags;
  for (const auto& t : param_int.getTags())
  {
    read_tags.push_back(t);
  }
  std::vector<std::string> expected{ "one", "two", "three" };
  BOOST_CHECK_EQUAL_COLLECTIONS(read_tags.begin(), read_tags.end(), expected.begin(), expected.end());
}

BOOST_AUTO_TEST_CASE(Parameter_defaultvalue)
{
  Parameter param_int("param_int", 10);
  BOOST_CHECK_EQUAL(param_int.getDefaultValueAsString(), "10");
  Parameter schema_param_int("param_int", 50);
  schema_param_int.setAsSchema(true);
  param_int.setSchema(schema_param_int);
  BOOST_CHECK_EQUAL(param_int.getDefaultValueAsString(), "50");
}

BOOST_AUTO_TEST_SUITE_END()
