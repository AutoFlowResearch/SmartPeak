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
  // TODO type override
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

BOOST_AUTO_TEST_SUITE_END()
