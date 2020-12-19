// TODO: Add copyright

#include <SmartPeak/test_config.h>

#define BOOST_TEST_MODULE Utilities test suite
#include <boost/test/included/unit_test.hpp>
#include <SmartPeak/core/Utilities.h>

using namespace SmartPeak;
using namespace std;

BOOST_AUTO_TEST_SUITE(utilities)

BOOST_AUTO_TEST_CASE(castString)
{
  CastValue c;
  Utilities::castString(string("19"), string("iNT"), c);
  BOOST_CHECK_EQUAL(c.getTag() == CastValue::Type::INT, true);
  BOOST_CHECK_EQUAL(c.i_, 19);
  Utilities::castString(string("tRuE"), string("bOOl"), c);
  BOOST_CHECK_EQUAL(c.getTag() == CastValue::Type::BOOL, true);
  BOOST_CHECK_EQUAL(c.b_, true);
  Utilities::castString(string("False"), string("STRing"), c);
  BOOST_CHECK_EQUAL(c.getTag() == CastValue::Type::BOOL, true);
  BOOST_CHECK_EQUAL(c.b_, false);
  Utilities::castString(string("hello"), string("stRIng"), c);
  BOOST_CHECK_EQUAL(c.getTag() == CastValue::Type::STRING, true);
  BOOST_CHECK_EQUAL(c.s_, "hello");
  Utilities::castString(string("world"), string("String"), c);
  BOOST_CHECK_EQUAL(c.getTag() == CastValue::Type::STRING, true);
  BOOST_CHECK_EQUAL(c.s_, "world");
  Utilities::castString(string("35.35"), string("float"), c);
  BOOST_CHECK_EQUAL(c.getTag() == CastValue::Type::FLOAT, true);
  BOOST_CHECK_CLOSE(c.f_, (float)35.35, 1e-6);
}

BOOST_AUTO_TEST_CASE(updateParameters)
{
  vector<map<string,string>> parameters({
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
    },
    {
      {"name", "param3"},
      {"type", "bool"},
      {"value", "false"}
    },
    {
      {"name", "param4"},
      {"type", "bool"},
      {"value", "true"}
    },
    {
      {"name", "param5"},
      {"type", "string"},
      {"value", "false"}
    },
    {
      {"name", "param6"},
      {"type", "string"},
      {"value", "true"}
    },
    {
      {"name", "param7"},
      {"type", "string"},
      {"value", "simply a string"}
    },
    {
      {"name", "param8"},
      {"value", "simply a string"}
    },
    {
      {"name", "param9"},
      {"value", "45"}
    },
    {
      {"name", "param10"},
      {"value", "1.27"}
    },
    {
      {"name", "param11"},
      {"value", "false"}
    },
    {
      {"name", "param12"},
      {"value", "true"}
    },
    {
      {"name", "param13"},
      {"description", "foobar"},
      {"tags", "tag5"}
    }
  });

  OpenMS::Param param;
  param.setValue("param1", 10);
  param.setValue("param2", 10.1);
  param.setValue("param3", "true");
  param.setValue("param4", "false");
  param.setValue("param5", "true");
  param.setValue("param6", "false");
  param.setValue("param7", "abc");
  param.setValue("param8", "def");
  param.setValue("param9", 11);
  param.setValue("param10", 11.1);
  param.setValue("param11", "true");
  param.setValue("param12", "false");
  param.setValue("param13", 44);

  Utilities::updateParameters(param, parameters);
  BOOST_CHECK_EQUAL(static_cast<int>(param.getValue("param1")), 23);
  BOOST_CHECK_EQUAL(param.getDescription("param1"), "param1 description");
  BOOST_CHECK_EQUAL(param.hasTag("param1", "tag1"), true);
  BOOST_CHECK_EQUAL(param.hasTag("param1", "tag2"), true);
  BOOST_CHECK_EQUAL(param.hasTag("param1", "tag3"), true);
  BOOST_CHECK_EQUAL(param.hasTag("param1", "tag4"), false);

  BOOST_CHECK_CLOSE(static_cast<double>(param.getValue("param2")), (float)3.14, 1e-6);
  BOOST_CHECK_EQUAL(param.getDescription("param2"), "");
  BOOST_CHECK_EQUAL(param.hasTag("param2", "tag1"), false);
  BOOST_CHECK_EQUAL(param.hasTag("param2", "tag4"), true);

  BOOST_CHECK_EQUAL(param.getValue("param3").toBool(), false);
  BOOST_CHECK_EQUAL(param.getDescription("param3"), "");

  BOOST_CHECK_EQUAL(param.getValue("param4").toBool(), true);
  BOOST_CHECK_EQUAL(param.getValue("param5").toBool(), false);
  BOOST_CHECK_EQUAL(param.getValue("param6").toBool(), true);
  BOOST_CHECK_EQUAL(param.getValue("param7").toString(), "simply a string");
  BOOST_CHECK_EQUAL(param.getValue("param8").toString(), "simply a string");
  BOOST_CHECK_EQUAL(static_cast<int>(param.getValue("param9")), 45);
  BOOST_CHECK_CLOSE(static_cast<double>(param.getValue("param10")), (float)1.27, 1e-6);
  BOOST_CHECK_EQUAL(param.getValue("param11").toBool(), false);
  BOOST_CHECK_EQUAL(param.getValue("param12").toBool(), true);

  BOOST_CHECK_EQUAL(static_cast<int>(param.getValue("param13")), 44);
  BOOST_CHECK_EQUAL(param.getDescription("param13"), "foobar");
  BOOST_CHECK_EQUAL(param.hasTag("param13", "tag5"), true);
}

BOOST_AUTO_TEST_CASE(parseString)
{
  CastValue c;

  Utilities::parseString("", c);
  BOOST_CHECK_EQUAL(c.getTag() == CastValue::Type::STRING, true);
  BOOST_CHECK_EQUAL(c.s_, "");

  Utilities::parseString(" foo ", c);
  BOOST_CHECK_EQUAL(c.getTag() == CastValue::Type::STRING, true);
  BOOST_CHECK_EQUAL(c.s_, "foo");

  Utilities::parseString("   34  ", c);
  BOOST_CHECK_EQUAL(c.getTag() == CastValue::Type::INT, true);
  BOOST_CHECK_EQUAL(c.i_, 34);

  Utilities::parseString(" 7.7  ", c);
  BOOST_CHECK_EQUAL(c.getTag() == CastValue::Type::FLOAT, true);
  BOOST_CHECK_CLOSE(c.f_, (float)7.7, 1e-6);

  Utilities::parseString("  false", c);
  BOOST_CHECK_EQUAL(c.getTag() == CastValue::Type::BOOL, true);
  BOOST_CHECK_EQUAL(c.b_, false);

  Utilities::parseString("true   ", c);
  BOOST_CHECK_EQUAL(c.getTag() == CastValue::Type::BOOL, true);
  BOOST_CHECK_EQUAL(c.b_, true);

  Utilities::parseString("[ 1,   2,    3 ]", c);
  BOOST_CHECK_EQUAL(c.getTag() == CastValue::Type::INT_LIST, true);
  BOOST_CHECK_EQUAL(c.il_[0], 1);
  BOOST_CHECK_EQUAL(c.il_[1], 2);
  BOOST_CHECK_EQUAL(c.il_[2], 3);

  Utilities::parseString("[   1 ,  2.2  ,     3.3]", c);
  BOOST_CHECK_EQUAL(c.getTag() == CastValue::Type::FLOAT_LIST, true);
  BOOST_CHECK_CLOSE(c.fl_[0], (float)1, 1e-6);
  BOOST_CHECK_CLOSE(c.fl_[1], (float)2.2, 1e-6);
  BOOST_CHECK_CLOSE(c.fl_[2], (float)3.3, 1e-6);

  Utilities::parseString("[   false ,  false  ,     true]", c);
  BOOST_CHECK_EQUAL(c.getTag() == CastValue::Type::BOOL_LIST, true);
  BOOST_CHECK_EQUAL(c.bl_[0], false);
  BOOST_CHECK_EQUAL(c.bl_[1], false);
  BOOST_CHECK_EQUAL(c.bl_[2], true);

  Utilities::parseString("[   \"first string\" ,  \"second string\"  ,     \"third string\"]", c);
  BOOST_CHECK_EQUAL(c.getTag() == CastValue::Type::STRING_LIST, true);
  BOOST_CHECK_EQUAL(c.sl_[0], "first string");
  BOOST_CHECK_EQUAL(c.sl_[1], "second string");
  BOOST_CHECK_EQUAL(c.sl_[2], "third string");
}

BOOST_AUTO_TEST_CASE(parseList)
{
  const string floats = "[1.1,-2.1,+3.1,4]";
  std::regex re_float_number("[+-]?\\d+(?:\\.\\d+)?"); // copied from .cpp implementation
  CastValue c;
  c = std::vector<float>();
  Utilities::parseList(floats, re_float_number, c);
  BOOST_CHECK_CLOSE(c.fl_[0], (float)1.1, 1e-6);
  BOOST_CHECK_CLOSE(c.fl_[1], (float)-2.1, 1e-6);
  BOOST_CHECK_CLOSE(c.fl_[2], (float)3.1, 1e-6);
  BOOST_CHECK_CLOSE(c.fl_[3], (float)4, 1e-6);

  const string ints = "[1,-2,+3,4]";
  std::regex re_integer_number("[+-]?\\d+"); // copied from .cpp implementation
  c = std::vector<int>();
  Utilities::parseList(ints, re_integer_number, c);
  BOOST_CHECK_EQUAL(c.il_[0], 1);
  BOOST_CHECK_EQUAL(c.il_[1], -2);
  BOOST_CHECK_EQUAL(c.il_[2], 3);
  BOOST_CHECK_EQUAL(c.il_[3], 4);

  const string bools = "[true,false,TRuE,fAlSe,TRUE,FALSE]";
  std::regex re_bool("true|false", std::regex::icase); // copied from .cpp implementation
  c = std::vector<bool>();
  Utilities::parseList(bools, re_bool, c);
  BOOST_CHECK_EQUAL(c.bl_[0], true);
  BOOST_CHECK_EQUAL(c.bl_[1], false);
  BOOST_CHECK_EQUAL(c.bl_[2], true);
  BOOST_CHECK_EQUAL(c.bl_[3], false);
  BOOST_CHECK_EQUAL(c.bl_[4], true);
  BOOST_CHECK_EQUAL(c.bl_[5], false);

  const string strings = "[\"foo\",\"bar\",\"foobar\"]";
  std::regex re_s("\"([^,]+)\""); // copied from .cpp implementation
  c = std::vector<std::string>();
  Utilities::parseList(strings, re_s, c);
  BOOST_CHECK_EQUAL(c.sl_[0], string("foo"));
  BOOST_CHECK_EQUAL(c.sl_[1], string("bar"));
  BOOST_CHECK_EQUAL(c.sl_[2], string("foobar"));

  c = 2; // to set tag to INT
  BOOST_CHECK_THROW(Utilities::parseList(strings, re_s, c), std::invalid_argument);
}

BOOST_AUTO_TEST_CASE(calculateValidationMetrics)
{
  const vector<int> predicted = {0, 1, 1, 0, 1, 0, 1, 0, 1, 1};
  const vector<int> actual    = {0, 0, 1, 0, 0, 1, 1, 0, 1, 1};
  const std::map<std::string, float> metrics = Utilities::calculateValidationMetrics(actual, predicted);
  BOOST_CHECK_CLOSE(metrics.at("accuracy"), static_cast<float>(0.7), 1e-3);
  BOOST_CHECK_CLOSE(metrics.at("recall"), static_cast<float>(0.8), 1e-3);
  BOOST_CHECK_CLOSE(metrics.at("precision"), static_cast<float>(0.66666666), 1e-3);
}

BOOST_AUTO_TEST_CASE(computeConfusionMatrix)
{
  const vector<int> predicted = {0, 1, 1, 0, 1, 0, 1, 0, 1, 1};
  const vector<int> actual    = {0, 0, 1, 0, 0, 1, 1, 0, 1, 1};
  const std::array<size_t, 4> conf = Utilities::computeConfusionMatrix(actual, predicted);
  BOOST_CHECK_EQUAL(conf[0], 4); // TP
  BOOST_CHECK_EQUAL(conf[1], 2); // FP
  BOOST_CHECK_EQUAL(conf[2], 1); // FN
  BOOST_CHECK_EQUAL(conf[3], 3); // TN
}

BOOST_AUTO_TEST_CASE(trimString)
{
  std::string s;

  s = Utilities::trimString(""); // empty string
  BOOST_CHECK_EQUAL(s, "");

  s = Utilities::trimString(" \f\n\r\t\v \f\n\r\t\v \f\n\r\t\v"); // only whitespaces
  BOOST_CHECK_EQUAL(s, "");

  s = Utilities::trimString("this is a string"); // no whitespaces
  BOOST_CHECK_EQUAL(s, "this is a string");

  s = Utilities::trimString(" \f\n\r\t\vwhitespaces on BOTH sides\v\t\r\n\f ");
  BOOST_CHECK_EQUAL(s, "whitespaces on BOTH sides");

  s = Utilities::trimString(" \f\n\r\t\vonly on the LEFT side");
  BOOST_CHECK_EQUAL(s, "only on the LEFT side");

  s = Utilities::trimString("only on the RIGHT side\v\t\r\n\f ");
  BOOST_CHECK_EQUAL(s, "only on the RIGHT side");

  s = Utilities::trimString("             let's do one more\t\t\t ");
  BOOST_CHECK_EQUAL(s, "let's do one more");

  s = Utilities::trimString("AEIOU but this will stay UOIEA", "AEIOU "); // with whitespaces argument
  BOOST_CHECK_EQUAL(s, "but this will stay");

  s = Utilities::trimString(" \t nothing will be removed\v", ""); // with empty whitespaces argument
  BOOST_CHECK_EQUAL(s, " \t nothing will be removed\v");
}

BOOST_AUTO_TEST_CASE(extractSelectorParameters)
{
  const std::vector<std::map<std::string, std::string>> params = {
    { {"name", "nn_thresholds"}, {"type", "list"}, {"value", "[4,4]"} },
    { {"name", "locality_weights"}, {"type", "list"}, {"value", "[False,False,False,True]"} },
    { {"name", "select_transition_groups"}, {"type", "list"}, {"value", "[True,True,True,True]"} },
    { {"name", "segment_window_lengths"}, {"type", "list"}, {"value", "[8,-1]"} },
    { {"name", "segment_step_lengths"}, {"type", "list"}, {"value", "[4,-1]"} },
    // { {"name", "select_highest_counts"}, {"type", "list"}, {"value", "[False,False,False,False]"} },
    { {"name", "variable_types"}, {"type", "list"}, {"value", "['integer','integer','integer','integer']"} },
    { {"name", "optimal_thresholds"}, {"type", "list"}, {"value", "[0.5,0.5,0.5,0.5]"} }
  };

  const std::vector<std::map<std::string, std::string>> score_weights = {
    { {"name", "var_log_sn_score"}, {"type", "string"}, {"value", "lambda score: 1/score"} },
    { {"name", "peak_apices_sum"}, {"type", "string"}, {"value", "lambda score: 1/log10(score)"} }
  };

  std::vector<OpenMS::MRMFeatureSelector::SelectorParameters> v =
    Utilities::extractSelectorParameters(params, score_weights);

  const OpenMS::MRMFeatureSelector::SelectorParameters* p = &v[0];

  BOOST_CHECK_EQUAL(v.size(), 2);

  BOOST_CHECK_EQUAL(p->nn_threshold, 4);
  BOOST_CHECK_EQUAL(p->locality_weight, false);
  BOOST_CHECK_EQUAL(p->select_transition_group, true);
  BOOST_CHECK_EQUAL(p->segment_window_length, 8);
  BOOST_CHECK_EQUAL(p->segment_step_length, 4);
  BOOST_CHECK_EQUAL(p->variable_type == OpenMS::MRMFeatureSelector::VariableType::INTEGER, true);
  BOOST_CHECK_CLOSE(p->optimal_threshold, 0.5, 1e-6);
  BOOST_CHECK_EQUAL(p->score_weights.size(), 2);
  BOOST_CHECK_EQUAL(p->score_weights.at("var_log_sn_score") == OpenMS::MRMFeatureSelector::LambdaScore::INVERSE, true);
  BOOST_CHECK_EQUAL(p->score_weights.at("peak_apices_sum") == OpenMS::MRMFeatureSelector::LambdaScore::INVERSE_LOG10, true);

  ++p;
  BOOST_CHECK_EQUAL(p->nn_threshold, 4);
  BOOST_CHECK_EQUAL(p->locality_weight, false);
  BOOST_CHECK_EQUAL(p->select_transition_group, true);
  BOOST_CHECK_EQUAL(p->segment_window_length, -1);
  BOOST_CHECK_EQUAL(p->segment_step_length, -1);
  BOOST_CHECK_EQUAL(p->variable_type == OpenMS::MRMFeatureSelector::VariableType::INTEGER, true);
  BOOST_CHECK_CLOSE(p->optimal_threshold, 0.5, 1e-6);
  BOOST_CHECK_EQUAL(p->score_weights.size(), 2);
  BOOST_CHECK_EQUAL(p->score_weights.at("var_log_sn_score") == OpenMS::MRMFeatureSelector::LambdaScore::INVERSE, true);
  BOOST_CHECK_EQUAL(p->score_weights.at("peak_apices_sum") == OpenMS::MRMFeatureSelector::LambdaScore::INVERSE_LOG10, true);
}

BOOST_AUTO_TEST_CASE(endsWith)
{
  const vector<string> names { "1.csv", "2.featureXML", "3.CSV", "4.FeatureXML", "5.another" };

  BOOST_CHECK_EQUAL(Utilities::endsWith(names[0], "csv"), true); // default argument case
  BOOST_CHECK_EQUAL(Utilities::endsWith(names[0], "csv", true), true);
  BOOST_CHECK_EQUAL(Utilities::endsWith(names[0], "CSV", true), false);
  BOOST_CHECK_EQUAL(Utilities::endsWith(names[0], "Csv", true), false);
  BOOST_CHECK_EQUAL(Utilities::endsWith(names[0], "CSV", false), true);
  BOOST_CHECK_EQUAL(Utilities::endsWith(names[0], "Csv", false), true);

  BOOST_CHECK_EQUAL(Utilities::endsWith(names[1], "featureXML"), true); // default argument case
  BOOST_CHECK_EQUAL(Utilities::endsWith(names[1], "featureXML", true), true);
  BOOST_CHECK_EQUAL(Utilities::endsWith(names[1], "FEATUREXML", true), false);
  BOOST_CHECK_EQUAL(Utilities::endsWith(names[1], "FeatureXML", true), false);
  BOOST_CHECK_EQUAL(Utilities::endsWith(names[1], "FEATUREXML", false), true);
  BOOST_CHECK_EQUAL(Utilities::endsWith(names[1], "FeatureXML", false), true);

  BOOST_CHECK_EQUAL(Utilities::endsWith(names[2], "csv"), false); // default argument case
  BOOST_CHECK_EQUAL(Utilities::endsWith(names[2], "csv", true), false);
  BOOST_CHECK_EQUAL(Utilities::endsWith(names[2], "CSV", true), true);
  BOOST_CHECK_EQUAL(Utilities::endsWith(names[2], "Csv", true), false);
  BOOST_CHECK_EQUAL(Utilities::endsWith(names[2], "CSV", false), true);
  BOOST_CHECK_EQUAL(Utilities::endsWith(names[2], "Csv", false), true);

  BOOST_CHECK_EQUAL(Utilities::endsWith(names[3], "featureXML"), false); // default argument case
  BOOST_CHECK_EQUAL(Utilities::endsWith(names[3], "featureXML", true), false);
  BOOST_CHECK_EQUAL(Utilities::endsWith(names[3], "FEATUREXML", true), false);
  BOOST_CHECK_EQUAL(Utilities::endsWith(names[3], "FeatureXML", true), true);
  BOOST_CHECK_EQUAL(Utilities::endsWith(names[3], "FEATUREXML", false), true);
  BOOST_CHECK_EQUAL(Utilities::endsWith(names[3], "FeatureXML", false), true);

  BOOST_CHECK_EQUAL(Utilities::endsWith(names[4], "does_not_end_with_this", false), false);
}

BOOST_AUTO_TEST_CASE(getPathnameContent)
{
  const std::string pathname = SMARTPEAK_GET_TEST_DATA_PATH("");
  const std::array<std::vector<std::string>, 4> c = Utilities::getPathnameContent(pathname);

  // number of items in the pathname, taking .gitignore into account
  BOOST_CHECK_EQUAL(c[0].size(), 45);
  BOOST_CHECK_EQUAL(c[1].size(), 45);
  BOOST_CHECK_EQUAL(c[2].size(), 45);
  BOOST_CHECK_EQUAL(c[3].size(), 45);

  BOOST_CHECK_EQUAL(c[0][0], "170808_Jonathan_yeast_Sacc1_1x_1_FluxTest_1900-01-01_000000.featureXML");
 #ifdef _WIN32
   // NOTE: depending on the build machine...
   //BOOST_CHECK_EQUAL(c[1][0], "774620"); // file size
 #else
  BOOST_CHECK_EQUAL(c[1][0], "761937"); // file size
 #endif
  BOOST_CHECK_EQUAL(c[2][0], ".featureXML");

<<<<<<< HEAD
  BOOST_CHECK_EQUAL(c[0][43], "SequenceProcessor_sequence.csv");
  BOOST_CHECK_EQUAL(c[1][43], "1392"); // number of items within the folder
  BOOST_CHECK_EQUAL(c[2][43], ".csv");
=======
  BOOST_CHECK_EQUAL(c[0][44], "workflow_csv_files");
  BOOST_CHECK_EQUAL(c[1][44], "22"); // number of items within the folder
  BOOST_CHECK_EQUAL(c[2][44], "Directory");
>>>>>>> develop
}

BOOST_AUTO_TEST_CASE(getParentPathname)
{
#ifdef _WIN32
  BOOST_CHECK_EQUAL(Utilities::getParentPathname("D://///"), "D:/");
  BOOST_CHECK_EQUAL(Utilities::getParentPathname("D:\\"), "D:");
  BOOST_CHECK_EQUAL(Utilities::getParentPathname("D:/"), "D:");
  BOOST_CHECK_EQUAL(Utilities::getParentPathname("D:"), "");
  BOOST_CHECK_EQUAL(Utilities::getParentPathname("E:/home/user/docs"), "E:/home/user");
  BOOST_CHECK_EQUAL(Utilities::getParentPathname("E://home///user//docs"), "E://home///user");
  BOOST_CHECK_EQUAL(Utilities::getParentPathname("E:/home/user/docs and a space"), "E:/home/user");
  BOOST_CHECK_EQUAL(Utilities::getParentPathname("E:/home/user/docs/"), "E:/home/user/docs");
  BOOST_CHECK_EQUAL(Utilities::getParentPathname("//home///user//docs"), "//home///user");
#else
  BOOST_CHECK_EQUAL(Utilities::getParentPathname(""), "");
  BOOST_CHECK_EQUAL(Utilities::getParentPathname("/"), "");
  BOOST_CHECK_EQUAL(Utilities::getParentPathname("/home/user/docs and a space"), "/home/user");
  BOOST_CHECK_EQUAL(Utilities::getParentPathname("/home/user/docs/"), "/home/user/docs");
  BOOST_CHECK_EQUAL(Utilities::getParentPathname("/home/user/docs"), "/home/user");
  BOOST_CHECK_EQUAL(Utilities::getParentPathname("/home"), "/");
  BOOST_CHECK_EQUAL(Utilities::getParentPathname("/home/file.txt"), "/home");
#endif
}

BOOST_AUTO_TEST_CASE(sortPairs)
{
  const std::vector<size_t> indices { 1, 0, 3, 2, 5, 4 };

  std::vector<std::string> v { "second", "first", "4th", "3rd", "6th", "5th" };
  Utilities::sortPairs(indices, v);
  BOOST_CHECK_EQUAL(v[0], "first");
  BOOST_CHECK_EQUAL(v[1], "second");
  BOOST_CHECK_EQUAL(v[2], "3rd");
  BOOST_CHECK_EQUAL(v[3], "4th");
  BOOST_CHECK_EQUAL(v[4], "5th");
  BOOST_CHECK_EQUAL(v[5], "6th");

  std::vector<int> w { 11, 22, 33, 44, 55, 66 };
  Utilities::sortPairs(indices, w);
  BOOST_CHECK_EQUAL(w[0], 22);
  BOOST_CHECK_EQUAL(w[1], 11);
  BOOST_CHECK_EQUAL(w[2], 44);
  BOOST_CHECK_EQUAL(w[3], 33);
  BOOST_CHECK_EQUAL(w[4], 66);
  BOOST_CHECK_EQUAL(w[5], 55);

  std::vector<float> t;
  Utilities::sortPairs({}, t); // should not throw
}

BOOST_AUTO_TEST_CASE(is_less_than_icase)
{
  auto& f = Utilities::is_less_than_icase;
  BOOST_CHECK_EQUAL(f("abc", "abc"), false);
  BOOST_CHECK_EQUAL(f("abc", "ABC"), false);
  BOOST_CHECK_EQUAL(f("ABC", "abc"), false);
  BOOST_CHECK_EQUAL(f("home", "z"), true);
  BOOST_CHECK_EQUAL(f("proc", "dev"), false);
  BOOST_CHECK_EQUAL(f("dev", "proc"), true);
  BOOST_CHECK_EQUAL(f("boot", "grub"), true);
}

BOOST_AUTO_TEST_CASE(directorySize)
{
  const std::string path = SMARTPEAK_GET_TEST_DATA_PATH("");
  auto& f = Utilities::directorySize;
  BOOST_CHECK_EQUAL(f(path), 45);
  BOOST_CHECK_EQUAL(f(path + "/workflow_csv_files"), 22);
  BOOST_CHECK_EQUAL(f(path + "/mzML"), 6);
}

BOOST_AUTO_TEST_SUITE_END()
