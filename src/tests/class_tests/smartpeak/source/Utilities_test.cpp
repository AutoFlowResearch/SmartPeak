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
#include <filesystem>
#include <sys/stat.h>
#include <SmartPeak/core/Utilities.h>
#include <OpenMS/ANALYSIS/OPENSWATH/MRMFeatureFinderScoring.h>

using namespace SmartPeak;
using namespace std;
namespace fs = std::filesystem;

const unsigned int nb_files_in_data_directory = 82;

TEST(utilities, castString)
{
  CastValue c;
  Utilities::castString(string("19"), string("iNT"), c);
  EXPECT_TRUE(c.getTag() == CastValue::Type::INT);
  EXPECT_EQ(c.i_, 19);
  Utilities::castString(string("tRuE"), string("bOOl"), c);
  EXPECT_TRUE(c.getTag() == CastValue::Type::BOOL);
  EXPECT_TRUE(c.b_);
  Utilities::castString(string("False"), string("STRing"), c);
  EXPECT_TRUE(c.getTag() == CastValue::Type::BOOL);
  EXPECT_FALSE(c.b_);
  Utilities::castString(string("hello"), string("stRIng"), c);
  EXPECT_TRUE(c.getTag() == CastValue::Type::STRING);
  EXPECT_STREQ(c.s_.c_str(), "hello");
  Utilities::castString(string("world"), string("String"), c);
  EXPECT_TRUE(c.getTag() == CastValue::Type::STRING);
  EXPECT_STREQ(c.s_.c_str(), "world");
  Utilities::castString(string("35.35"), string("float"), c);
  EXPECT_TRUE(c.getTag() == CastValue::Type::FLOAT);
  EXPECT_NEAR(c.f_, (float)35.35, 1e-6);
}

TEST(utilities, setUserParameters)
{
  OpenMS::MRMFeatureFinderScoring feature;
  auto param = feature.getParameters();
  // default value is 1 for add_up_spectra
  EXPECT_EQ(int(param.getValue("add_up_spectra")), 1);

  // user set parameter
  map<std::string, vector<map<string, string>>> feat_params_struct1({
  {"MRMFeatureFinderScoring", {
    { {"name", "add_up_spectra"}, {"type", "int"}, {"value", "42"} },
  }}
  });
  ParameterSet feat_params1(feat_params_struct1);
  Utilities::setUserParameters(feature, feat_params1);
  EXPECT_EQ(int(feature.getParameters().getValue("add_up_spectra")), 42);

  // user set parameter - alias name
  map<std::string, vector<map<string, string>>> feat_params_struct2({
  {"AliasName", {
    { {"name", "add_up_spectra"}, {"type", "int"}, {"value", "43"} },
  }}
  });
  ParameterSet feat_params2(feat_params_struct2);
  Utilities::setUserParameters(feature, feat_params2, "AliasName");
  EXPECT_EQ(int(feature.getParameters().getValue("add_up_spectra")), 43);

  // not matching parameter
  map<std::string, vector<map<string, string>>> feat_params_struct3({
  {"NotMatching", {
    { {"name", "add_up_spectra"}, {"type", "int"}, {"value", "43"} },
  }}
  });
  ParameterSet feat_params3(feat_params_struct3);
  Utilities::setUserParameters(feature, feat_params3);
  EXPECT_EQ(int(feature.getParameters().getValue("add_up_spectra")), 43);
}

TEST(utilities, updateParameters)
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
  FunctionParameters function_parameters("test", parameters);

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

  Utilities::updateParameters(param, function_parameters);
  EXPECT_EQ(static_cast<int>(param.getValue("param1")), 23);
  EXPECT_STREQ(param.getDescription("param1").c_str(), "param1 description");
  EXPECT_TRUE(param.hasTag("param1", "tag1"));
  EXPECT_TRUE(param.hasTag("param1", "tag2"));
  EXPECT_TRUE(param.hasTag("param1", "tag3"));
  EXPECT_FALSE(param.hasTag("param1", "tag4"));

  EXPECT_NEAR(static_cast<double>(param.getValue("param2")), (float)3.14, 1e-6);
  EXPECT_STREQ(param.getDescription("param2").c_str(), "");
  EXPECT_FALSE(param.hasTag("param2", "tag1"));
  EXPECT_TRUE(param.hasTag("param2", "tag4"));

  EXPECT_FALSE(param.getValue("param3").toBool());
  EXPECT_STREQ(param.getDescription("param3").c_str(), "");

  EXPECT_TRUE(param.getValue("param4").toBool());
  EXPECT_FALSE(param.getValue("param5").toBool());
  EXPECT_TRUE(param.getValue("param6").toBool());
  EXPECT_STREQ(param.getValue("param7").toString().c_str(), "simply a string");
  EXPECT_STREQ(param.getValue("param8").toString().c_str(), "simply a string");
  EXPECT_EQ(static_cast<int>(param.getValue("param9")), 45);
  EXPECT_NEAR(static_cast<double>(param.getValue("param10")), (float)1.27, 1e-6);
  EXPECT_FALSE(param.getValue("param11").toBool());
  EXPECT_TRUE(param.getValue("param12").toBool());

  EXPECT_EQ(static_cast<int>(param.getValue("param13")), 44);
  EXPECT_STREQ(param.getDescription("param13").c_str(), "foobar");
  EXPECT_TRUE(param.hasTag("param13", "tag5"));
}

TEST(utilities, parseString)
{
  CastValue c;

  Utilities::parseString("", c);
  EXPECT_TRUE(c.getTag() == CastValue::Type::STRING);
  EXPECT_STREQ(c.s_.c_str(), "");

  Utilities::parseString(" foo ", c);
  EXPECT_TRUE(c.getTag() == CastValue::Type::STRING);
  EXPECT_STREQ(c.s_.c_str(), "foo");

  Utilities::parseString("   34  ", c);
  EXPECT_TRUE(c.getTag() == CastValue::Type::INT);
  EXPECT_EQ(c.i_, 34);

  Utilities::parseString(" 7.7  ", c);
  EXPECT_TRUE(c.getTag() == CastValue::Type::FLOAT);
  EXPECT_NEAR(c.f_, (float)7.7, 1e-6);

  Utilities::parseString("  false", c);
  EXPECT_TRUE(c.getTag() == CastValue::Type::BOOL);
  EXPECT_FALSE(c.b_);

  Utilities::parseString("true   ", c);
  EXPECT_TRUE(c.getTag() == CastValue::Type::BOOL);
  EXPECT_TRUE(c.b_);

  Utilities::parseString("[ 1,   2,    3 ]", c);
  EXPECT_TRUE(c.getTag() == CastValue::Type::INT_LIST);
  EXPECT_EQ(c.il_[0], 1);
  EXPECT_EQ(c.il_[1], 2);
  EXPECT_EQ(c.il_[2], 3);

  Utilities::parseString("[   1 ,  2.2  ,     3.3]", c);
  EXPECT_TRUE(c.getTag() == CastValue::Type::FLOAT_LIST);
  EXPECT_NEAR(c.fl_[0], (float)1, 1e-6);
  EXPECT_NEAR(c.fl_[1], (float)2.2, 1e-6);
  EXPECT_NEAR(c.fl_[2], (float)3.3, 1e-6);

  Utilities::parseString("[   false ,  false  ,     true]", c);
  EXPECT_TRUE(c.getTag() == CastValue::Type::BOOL_LIST);
  EXPECT_FALSE(c.bl_[0]);
  EXPECT_FALSE(c.bl_[1]);
  EXPECT_TRUE(c.bl_[2]);

  Utilities::parseString("[   \"first string\" ,  \"second string\"  ,     \"third string\"]", c);
  EXPECT_TRUE(c.getTag() == CastValue::Type::STRING_LIST);
  EXPECT_STREQ(c.sl_[0].c_str(), "first string");
  EXPECT_STREQ(c.sl_[1].c_str(), "second string");
  EXPECT_STREQ(c.sl_[2].c_str(), "third string");
}

TEST(utilities, parseString_bigString)
{
  std::vector<bool> bool_list;
  std::ostringstream os;
  os << "[";
  std::string sep;
  for (int i = 0; i < 1000; ++i)
  {
    os << sep;
    os << "false";
    bool_list.push_back(false);
    sep = ",";
  }
  os << "]";
  std::string big_string = os.str();
  CastValue c;

  Utilities::parseString(big_string, c);
  ASSERT_EQ(c.getTag(), CastValue::Type::BOOL_LIST);
  EXPECT_EQ(c.bl_ , bool_list);
}

TEST(utilities, isList)
{
  std::regex re_integer_number("[+-]?\\d+");
  EXPECT_TRUE(Utilities::isList("1,2,3", re_integer_number));
  EXPECT_FALSE(Utilities::isList("1,3.14,3", re_integer_number));
  EXPECT_FALSE(Utilities::isList("", re_integer_number));

  std::regex re_float_number("[+-]?\\d+(?:\\.\\d+)?"); // can match also integers: check for integer before checking for float
  EXPECT_TRUE(Utilities::isList("1,2,3", re_float_number));
  EXPECT_TRUE(Utilities::isList("1,3.14,3", re_float_number));
  EXPECT_FALSE(Utilities::isList("1,one,two", re_float_number));

  std::regex re_bool("true|false", std::regex::icase);
  EXPECT_TRUE(Utilities::isList("false,TRUE,fAlse", re_bool));
  EXPECT_FALSE(Utilities::isList("1,one,two", re_bool));

  std::regex re_s("[\"']([^,]+)[\"']");
  EXPECT_TRUE(Utilities::isList("\"1\",\"one\",\"two\"", re_s));
  EXPECT_FALSE(Utilities::isList("false,TRUE,fAlse", re_s));
}

TEST(utilities, parseList)
{
  const string floats = "[1.1,-2.1,+3.1,4]";
  std::regex re_float_number("[+-]?\\d+(?:\\.\\d+)?"); // copied from .cpp implementation
  CastValue c;
  c = std::vector<float>();
  Utilities::parseList(floats, re_float_number, c);
  EXPECT_NEAR(c.fl_[0], (float)1.1, 1e-6);
  EXPECT_NEAR(c.fl_[1], (float)-2.1, 1e-6);
  EXPECT_NEAR(c.fl_[2], (float)3.1, 1e-6);
  EXPECT_NEAR(c.fl_[3], (float)4, 1e-6);

  const string ints = "[1,-2,+3,4]";
  std::regex re_integer_number("[+-]?\\d+"); // copied from .cpp implementation
  c = std::vector<int>();
  Utilities::parseList(ints, re_integer_number, c);
  EXPECT_EQ(c.il_[0], 1);
  EXPECT_EQ(c.il_[1], -2);
  EXPECT_EQ(c.il_[2], 3);
  EXPECT_EQ(c.il_[3], 4);

  const string bools = "[true,false,TRuE,fAlSe,TRUE,FALSE]";
  std::regex re_bool("true|false", std::regex::icase); // copied from .cpp implementation
  c = std::vector<bool>();
  Utilities::parseList(bools, re_bool, c);
  EXPECT_TRUE(c.bl_[0]);
  EXPECT_FALSE(c.bl_[1]);
  EXPECT_TRUE(c.bl_[2]);
  EXPECT_FALSE(c.bl_[3]);
  EXPECT_TRUE(c.bl_[4]);
  EXPECT_FALSE(c.bl_[5]);

  const string strings = "[\"foo\",\"bar\",\"foobar\"]";
  std::regex re_s("\"([^,]+)\""); // copied from .cpp implementation
  c = std::vector<std::string>();
  Utilities::parseList(strings, re_s, c);
  EXPECT_STREQ(c.sl_[0].c_str(), string("foo").c_str());
  EXPECT_STREQ(c.sl_[1].c_str(), string("bar").c_str());
  EXPECT_STREQ(c.sl_[2].c_str(), string("foobar").c_str());

  c = 2; // to set tag to INT
  EXPECT_THROW(Utilities::parseList(strings, re_s, c), std::invalid_argument);
}

TEST(utilities, calculateValidationMetrics)
{
  const vector<int> predicted = {0, 1, 1, 0, 1, 0, 1, 0, 1, 1};
  const vector<int> actual    = {0, 0, 1, 0, 0, 1, 1, 0, 1, 1};
  const std::map<std::string, float> metrics = Utilities::calculateValidationMetrics(actual, predicted);
  EXPECT_NEAR(metrics.at("accuracy"), static_cast<float>(0.7), 1e-3);
  EXPECT_NEAR(metrics.at("recall"), static_cast<float>(0.8), 1e-3);
  EXPECT_NEAR(metrics.at("precision"), static_cast<float>(0.66666666), 1e-3);
}

TEST(utilities, computeConfusionMatrix)
{
  const vector<int> predicted = {0, 1, 1, 0, 1, 0, 1, 0, 1, 1};
  const vector<int> actual    = {0, 0, 1, 0, 0, 1, 1, 0, 1, 1};
  const std::array<size_t, 4> conf = Utilities::computeConfusionMatrix(actual, predicted);
  EXPECT_EQ(conf[0], 4); // TP
  EXPECT_EQ(conf[1], 2); // FP
  EXPECT_EQ(conf[2], 1); // FN
  EXPECT_EQ(conf[3], 3); // TN
}

TEST(utilities, trimString)
{
  std::string s;

  s = Utilities::trimString(""); // empty string
  EXPECT_STREQ(s.c_str(), "");

  s = Utilities::trimString(" \f\n\r\t\v \f\n\r\t\v \f\n\r\t\v"); // only whitespaces
  EXPECT_STREQ(s.c_str(), "");

  s = Utilities::trimString("this is a string"); // no whitespaces
  EXPECT_STREQ(s.c_str(), "this is a string");

  s = Utilities::trimString(" \f\n\r\t\vwhitespaces on BOTH sides\v\t\r\n\f ");
  EXPECT_STREQ(s.c_str(), "whitespaces on BOTH sides");

  s = Utilities::trimString(" \f\n\r\t\vonly on the LEFT side");
  EXPECT_STREQ(s.c_str(), "only on the LEFT side");

  s = Utilities::trimString("only on the RIGHT side\v\t\r\n\f ");
  EXPECT_STREQ(s.c_str(), "only on the RIGHT side");

  s = Utilities::trimString("             let's do one more\t\t\t ");
  EXPECT_STREQ(s.c_str(), "let's do one more");

  s = Utilities::trimString("AEIOU but this will stay UOIEA", "AEIOU "); // with whitespaces argument
  EXPECT_STREQ(s.c_str(), "but this will stay");

  s = Utilities::trimString(" \t nothing will be removed\v", ""); // with empty whitespaces argument
  EXPECT_STREQ(s.c_str(), " \t nothing will be removed\v");
}

TEST(utilities, extractSelectorParameters)
{
  FunctionParameters params("params", {
    { {"name", "nn_thresholds"}, {"type", "list"}, {"value", "[4,4]"} },
    { {"name", "locality_weights"}, {"type", "list"}, {"value", "[False,False,False,True]"} },
    { {"name", "select_transition_groups"}, {"type", "list"}, {"value", "[True,True,True,True]"} },
    { {"name", "segment_window_lengths"}, {"type", "list"}, {"value", "[8,-1]"} },
    { {"name", "segment_step_lengths"}, {"type", "list"}, {"value", "[4,-1]"} },
    // { {"name", "select_highest_counts"}, {"type", "list"}, {"value", "[False,False,False,False]"} },
    { {"name", "variable_types"}, {"type", "list"}, {"value", "['integer','integer','integer','integer']"} },
    { {"name", "optimal_thresholds"}, {"type", "list"}, {"value", "[0.5,0.5,0.5,0.5]"} }
  });

  FunctionParameters score_weights("score_weights", {
    { {"name", "var_log_sn_score"}, {"type", "string"}, {"value", "lambda score: 1/score"} },
    { {"name", "peak_apices_sum"}, {"type", "string"}, {"value", "lambda score: 1/log10(score)"} }
  });

  std::vector<OpenMS::MRMFeatureSelector::SelectorParameters> v =
    Utilities::extractSelectorParameters(params, score_weights);

  const OpenMS::MRMFeatureSelector::SelectorParameters* p = &v[0];

  EXPECT_EQ(v.size(), 2);

  EXPECT_EQ(p->nn_threshold, 4);
  EXPECT_FALSE(p->locality_weight);
  EXPECT_TRUE(p->select_transition_group);
  EXPECT_EQ(p->segment_window_length, 8);
  EXPECT_EQ(p->segment_step_length, 4);
  EXPECT_TRUE(p->variable_type == OpenMS::MRMFeatureSelector::VariableType::INTEGER);
  EXPECT_NEAR(p->optimal_threshold, 0.5, 1e-6);
  EXPECT_EQ(p->score_weights.size(), 2);
  EXPECT_TRUE(p->score_weights.at("var_log_sn_score") == OpenMS::MRMFeatureSelector::LambdaScore::INVERSE);
  EXPECT_TRUE(p->score_weights.at("peak_apices_sum") == OpenMS::MRMFeatureSelector::LambdaScore::INVERSE_LOG10);

  ++p;
  EXPECT_EQ(p->nn_threshold, 4);
  EXPECT_FALSE(p->locality_weight);
  EXPECT_TRUE(p->select_transition_group);
  EXPECT_EQ(p->segment_window_length, -1);
  EXPECT_EQ(p->segment_step_length, -1);
  EXPECT_TRUE(p->variable_type == OpenMS::MRMFeatureSelector::VariableType::INTEGER);
  EXPECT_NEAR(p->optimal_threshold, 0.5, 1e-6);
  EXPECT_EQ(p->score_weights.size(), 2);
  EXPECT_TRUE(p->score_weights.at("var_log_sn_score") == OpenMS::MRMFeatureSelector::LambdaScore::INVERSE);
  EXPECT_TRUE(p->score_weights.at("peak_apices_sum") == OpenMS::MRMFeatureSelector::LambdaScore::INVERSE_LOG10);
}

TEST(utilities, endsWith)
{
  const vector<string> names { "1.csv", "2.featureXML", "3.CSV", "4.FeatureXML", "5.another" };

  EXPECT_TRUE(Utilities::endsWith(names[0], "csv")); // default argument case
  EXPECT_TRUE(Utilities::endsWith(names[0], "csv"));
  EXPECT_FALSE(Utilities::endsWith(names[0], "CSV"));
  EXPECT_FALSE(Utilities::endsWith(names[0], "Csv"));
  EXPECT_FALSE(Utilities::endsWith(names[0], "CSV"));
  EXPECT_FALSE(Utilities::endsWith(names[0], "Csv"));

  EXPECT_TRUE(Utilities::endsWith(names[1], "featureXML")); // default argument case
  EXPECT_TRUE(Utilities::endsWith(names[1], "featureXML"));
  EXPECT_FALSE(Utilities::endsWith(names[1], "FEATUREXML"));
  EXPECT_FALSE(Utilities::endsWith(names[1], "FeatureXML"));
  EXPECT_TRUE(Utilities::endsWith(names[1], "FEATUREXML", false));
  EXPECT_TRUE(Utilities::endsWith(names[1], "FeatureXML", false));

  EXPECT_FALSE(Utilities::endsWith(names[2], "csv")); // default argument case
  EXPECT_FALSE(Utilities::endsWith(names[2], "csv", true));
  EXPECT_TRUE(Utilities::endsWith(names[2], "CSV", true));
  EXPECT_FALSE(Utilities::endsWith(names[2], "Csv", true));
  EXPECT_TRUE(Utilities::endsWith(names[2], "CSV", false));
  EXPECT_TRUE(Utilities::endsWith(names[2], "Csv", false));

  EXPECT_FALSE(Utilities::endsWith(names[3].c_str(), "featureXML")); // default argument case
  EXPECT_FALSE(Utilities::endsWith(names[3], "featureXML", true));
  EXPECT_FALSE(Utilities::endsWith(names[3], "FEATUREXML", true));
  EXPECT_TRUE(Utilities::endsWith(names[3], "FeatureXML", true));
  EXPECT_TRUE(Utilities::endsWith(names[3], "FEATUREXML", false));
  EXPECT_TRUE(Utilities::endsWith(names[3], "FeatureXML", false));

  EXPECT_FALSE(Utilities::endsWith(names[4], "does_not_end_with_this", false));
}

TEST(utilities, getFolderContents)
{
  const std::string pathname = SMARTPEAK_GET_TEST_DATA_PATH("");
  const std::array<std::vector<std::string>, 4> c = Utilities::getFolderContents(pathname, false);

  // number of items in the pathname, taking .gitignore into account
  EXPECT_EQ(c[0].size(), nb_files_in_data_directory);
  EXPECT_EQ(c[1].size(), nb_files_in_data_directory);
  EXPECT_EQ(c[2].size(), nb_files_in_data_directory);
  EXPECT_EQ(c[3].size(), nb_files_in_data_directory);

  //EXPECT_STREQ(c[0][0], "OpenMSFile_ChromeleonFile_10ug.txt");
 #ifdef _WIN32
   // NOTE: depending on the build machine...
   //EXPECT_STREQ(c[1][0], "774620"); // file size
 #else
  //EXPECT_STREQ(c[1][0], "86299"); // file size
 #endif
  //EXPECT_STREQ(c[2][0], ".txt");

  //EXPECT_STREQ(c[0][48], "RawDataProcessor_serumTest_accurateMassSearch.featureXML");
  //EXPECT_STREQ(c[1][48], "78174");
  //EXPECT_STREQ(c[2][48], ".featureXML");
}


TEST(utilities, getParentPath)
{
  EXPECT_STREQ(Utilities::getParentPath("").c_str(), "");
  EXPECT_STREQ(Utilities::getParentPath("/").c_str(), "/");
  EXPECT_STREQ(Utilities::getParentPath("/home").c_str(), "/");
  EXPECT_STREQ(Utilities::getParentPath("/home/file.txt").c_str(), "/home");
  EXPECT_STREQ(Utilities::getParentPath("/home/user/Downloads").c_str(), "/home/user");
  EXPECT_STREQ(Utilities::getParentPath("//home///user//Downloads").c_str(), "//home///user");
  EXPECT_STREQ(Utilities::getParentPath("/home/user/Downloads/").c_str(), "/home/user/Downloads");
  EXPECT_STREQ(Utilities::getParentPath("/home/user/Downloads and a space").c_str(), "/home/user");
  EXPECT_STREQ(Utilities::getParentPath("C:/Users/user/Downloads").c_str(), "C:/Users/user");
#ifdef _WIN32
  EXPECT_STREQ(Utilities::getParentPath("D:").c_str(), "D:");
  EXPECT_STREQ(Utilities::getParentPath("D:/").c_str(), "D:/");
  EXPECT_STREQ(Utilities::getParentPath("D://").c_str(), "D://");
  EXPECT_STREQ(Utilities::getParentPath("D://///").c_str(), "D://///");
  EXPECT_STREQ(Utilities::getParentPath("E:/home/user/Downloads").c_str(), "E:/home/user");
  EXPECT_STREQ(Utilities::getParentPath("E://home///user//Downloads").c_str(), "E://home///user");
  EXPECT_STREQ(Utilities::getParentPath("E:/home/user/Downloads and a space").c_str(), "E:/home/user");
  EXPECT_STREQ(Utilities::getParentPath("E:/home/user/Downloads/").c_str(), "E:/home/user/Downloads");
#endif
}

TEST(utilities, sortPairs)
{
  const std::vector<size_t> indices { 1, 0, 3, 2, 5, 4 };

  std::vector<std::string> v { "second", "first", "4th", "3rd", "6th", "5th" };
  Utilities::sortPairs(indices, v);
  EXPECT_STREQ(v[0].c_str(), "first");
  EXPECT_STREQ(v[1].c_str(), "second");
  EXPECT_STREQ(v[2].c_str(), "3rd");
  EXPECT_STREQ(v[3].c_str(), "4th");
  EXPECT_STREQ(v[4].c_str(), "5th");
  EXPECT_STREQ(v[5].c_str(), "6th");

  std::vector<int> w { 11, 22, 33, 44, 55, 66 };
  Utilities::sortPairs(indices, w);
  EXPECT_EQ(w[0], 22);
  EXPECT_EQ(w[1], 11);
  EXPECT_EQ(w[2], 44);
  EXPECT_EQ(w[3], 33);
  EXPECT_EQ(w[4], 66);
  EXPECT_EQ(w[5], 55);

  std::vector<float> t;
  Utilities::sortPairs({}, t); // should not throw
}

TEST(utilities, is_less_than_icase)
{
  auto& f = Utilities::is_less_than_icase;
  EXPECT_FALSE(f("abc", "abc"));
  EXPECT_FALSE(f("abc", "ABC"));
  EXPECT_FALSE(f("ABC", "abc"));
  EXPECT_TRUE(f("home", "z"));
  EXPECT_FALSE(f("proc", "dev"));
  EXPECT_TRUE(f("dev", "proc"));
  EXPECT_TRUE(f("boot", "grub"));
}


TEST(utilities, getDirectoryInfo)
{
  std::tuple<float, uintmax_t> directory_info;
  const std::string path = SMARTPEAK_GET_TEST_DATA_PATH("");
  Utilities::getDirectoryInfo(path, directory_info);
  EXPECT_EQ(std::get<1>(directory_info), nb_files_in_data_directory);
}

void set_env_var_(const std::string& name, const std::string& value)
{
#ifdef _WIN32
  if (value.empty())
    _putenv((name + "=").c_str());
  else
    _putenv((name + "=" + value).c_str());
#else
  if (value.empty())
    unsetenv(name.c_str());
  else
    setenv(name.c_str(), value.c_str(), 1);
#endif
}

TEST(utilities, getLogFilepath)
{
  auto& get_log_file_path = Utilities::getLogFilepath;
  // Test default behaviour
  // Test doesn't assume that any env variable is set
  {
    auto dirpath = std::string{ SMARTPEAK_GET_TEST_DATA_PATH("logs") };
    auto filepath = std::filesystem::path{};
    auto dir_created = false;

    // Only HOME is set
    set_env_var_("SMARTPEAK_LOGS", "");
    set_env_var_("LOCALAPPDATA", "");
    set_env_var_("HOME", dirpath);
    std::tie(filepath, dir_created) = get_log_file_path("file");
    EXPECT_TRUE(fs::exists(filepath));
    EXPECT_TRUE(fs::exists(dirpath));
    EXPECT_TRUE(dir_created) << "Log directory with path '" << dirpath << "' is not created";
    fs::remove_all(dirpath);

    // Only LOCALAPPDATA is set
    set_env_var_("SMARTPEAK_LOGS", "");
    set_env_var_("LOCALAPPDATA", dirpath);
    set_env_var_("HOME", "");
    std::tie(filepath, dir_created) = get_log_file_path("file");
    EXPECT_TRUE(fs::exists(filepath));
    EXPECT_TRUE(fs::exists(dirpath));
    EXPECT_TRUE(dir_created) << "Log directory with path '" << dirpath << "' is not created";
    fs::remove_all(dirpath);
  }
  // All env variables unset
  {
    set_env_var_("SMARTPEAK_LOGS", "");
    set_env_var_("LOCALAPPDATA", "");
    set_env_var_("HOME", "");
    EXPECT_THROW(get_log_file_path("filename"), std::runtime_error);
  }
  // SMARTPEAK_LOGS has higher priority
  {
    auto logpath = std::string{ SMARTPEAK_GET_TEST_DATA_PATH("logs") };
    auto otherpath = std::string{ SMARTPEAK_GET_TEST_DATA_PATH("otherlogs") };
    auto filepath = std::filesystem::path{};
    auto dir_created = false;

    set_env_var_("SMARTPEAK_LOGS", logpath);
    set_env_var_("LOCALAPPDATA", otherpath);
    set_env_var_("HOME", otherpath);

    std::tie(filepath, dir_created) = get_log_file_path("file");
    EXPECT_TRUE(fs::exists(filepath));
    EXPECT_TRUE(fs::exists(logpath));
    EXPECT_TRUE(!fs::exists(otherpath));
    EXPECT_TRUE(dir_created) << "Log directory with path '" << logpath << "' is not created";
    fs::remove_all(logpath);
  }
}

TEST(utilities, removeTrailing)
{
  std::string str1 = "234.0000";
  Utilities::removeTrailing(str1, ".00");
  EXPECT_STREQ(str1.c_str(), "234");
  
  std::string str2 = "234.01";
  Utilities::removeTrailing(str2, ".00");
  EXPECT_STREQ(str2.c_str(), "234.01");
  
  std::string str3 = "234.00";
  Utilities::removeTrailing(str3, ".00");
  EXPECT_STREQ(str3.c_str(), "234");
}

TEST(utilities, prepareFileParameter)
{
  map<std::string, vector<map<string, string>>> param_struct1({
  {"test",
    {
      {
        {"name", "existing_relative_path"},
        {"type", "string"},
        {"value", "SequenceParser_sequence_1.csv"}
      },
      {
        {"name", "non_existing_relative_path"},
        {"type", "string"},
        {"value", "non_existing_file.csv"}
      },
      {
        {"name", "absolute_path"},
        {"type", "string"},
        {"value", SMARTPEAK_GET_TEST_DATA_PATH("SequenceParser_sequence_1.csv")}
      }
    }
  }});
  ParameterSet parameter_set(param_struct1);
  std::filesystem::path main_path = std::filesystem::path(SMARTPEAK_GET_TEST_DATA_PATH(""));

  Utilities::prepareFileParameter(parameter_set, "test", "existing_relative_path", main_path);
  EXPECT_EQ(parameter_set.findParameter("test", "existing_relative_path")->getValueAsString(),
            (main_path / std::filesystem::path("SequenceParser_sequence_1.csv")).lexically_normal().generic_string());

  Utilities::prepareFileParameter(parameter_set, "test", "non_existing_relative_path", main_path);
  EXPECT_EQ(parameter_set.findParameter("test", "non_existing_relative_path")->getValueAsString(),
           "non_existing_file.csv");

  Utilities::prepareFileParameter(parameter_set, "test", "absolute_path", main_path);
  EXPECT_EQ(parameter_set.findParameter("test", "absolute_path")->getValueAsString(),
            SMARTPEAK_GET_TEST_DATA_PATH("SequenceParser_sequence_1.csv"));
}

TEST(utilities, prepareFileParameterList)
{
  map<std::string, vector<map<string, string>>> param_struct1({
  {"test",
    {
      {
        {"name", "existing_relative_path"},
        {"type", "list"},
        {"value", "['SequenceParser_sequence_1.csv','SequenceParser_sequence_1_semicolon.csv']"}
      },
      {
        {"name", "non_existing_relative_path"},
        {"type", "list"},
        {"value", "['non_existing_file.csv','non_existing_file2.csv']"}
      },
      {
        {"name", "absolute_path"},
        {"type", "list"},
        {"value", std::string("['") + SMARTPEAK_GET_TEST_DATA_PATH("SequenceParser_sequence_1.csv") + std::string("',")
                  + std::string("'") + SMARTPEAK_GET_TEST_DATA_PATH("SequenceParser_sequence_1_semicolon.csv") + std::string("']")}
      }
    }
  } });
  ParameterSet parameter_set(param_struct1);
  std::filesystem::path main_path = std::filesystem::path(SMARTPEAK_GET_TEST_DATA_PATH(""));

  Utilities::prepareFileParameterList(parameter_set, "test", "existing_relative_path", main_path);
  EXPECT_EQ(parameter_set.findParameter("test", "existing_relative_path")->getValueAsString(),
           std::string("['") + (main_path / std::filesystem::path("SequenceParser_sequence_1.csv")).lexically_normal().generic_string() + std::string("'")
          +std::string(",'") +(main_path / std::filesystem::path("SequenceParser_sequence_1_semicolon.csv")).lexically_normal().generic_string() + std::string("']"));

  Utilities::prepareFileParameter(parameter_set, "test", "non_existing_relative_path", main_path);
  EXPECT_EQ(parameter_set.findParameter("test", "non_existing_relative_path")->getValueAsString(),
    "['non_existing_file.csv','non_existing_file2.csv']");

  Utilities::prepareFileParameter(parameter_set, "test", "absolute_path", main_path);
  EXPECT_EQ(parameter_set.findParameter("test", "absolute_path")->getValueAsString(),
    std::string("['") + SMARTPEAK_GET_TEST_DATA_PATH("SequenceParser_sequence_1.csv") + std::string("',")
    + std::string("'") + SMARTPEAK_GET_TEST_DATA_PATH("SequenceParser_sequence_1_semicolon.csv") + std::string("']"));
}

TEST(utilities, sha256)
{
  std::string hashed_string = Utilities::sha256("test-string");
  std::string correct_hashed_string = "ffe65f1d98fafedea3514adc956c8ada5980c6c5d2552fd61f48401aefd5c00e";
  EXPECT_STREQ(hashed_string.c_str(), correct_hashed_string.c_str());
}

TEST(utilities, createServerSessionFile)
{
  std::filesystem::path tmp_dir = Utilities::createEmptyTempDirectory();
  Utilities::createServerSessionFile(tmp_dir);
  EXPECT_TRUE(std::filesystem::exists(tmp_dir/".serversession.ssi"));
}

TEST(utilities, writeToServerSessionFile)
{
  std::filesystem::path tmp_dir = Utilities::createEmptyTempDirectory();
  std::string usr_id_1 = "user-1";
  std::string dataset_name_1 = "dataset-1";
  std::string workflow_status_1 = "YES";
  std::string started_at_1 = "12-30-03_30-12-2021";
  std::string finished_at_1 = "13-30-03_30-12-2021";
  std::string path_to_exports_1 = "/path/to/exports/1/";
  std::string log_file_1 = "/path/to/log/file-1";
  
  std::string usr_id_2 = "user-2";
  std::string dataset_name_2 = "dataset-2";
  std::string workflow_status_2 = "YES";
  std::string started_at_2 = "12-30-03_25-12-2021";
  std::string finished_at_2 = "13-30-03_25-12-2021";
  std::string path_to_exports_2 = "/path/to/exports/2/";
  std::string log_file_2 = "/path/to/log/file-2";
  
  std::vector<std::string> expected = {
    "usr_id,dataset_name,workflow_status,started_at,finished_at,path_to_exports,log_file",
    "user-1,dataset-1,YES,12-30-03_30-12-2021,13-30-03_30-12-2021,/path/to/exports/1/,/path/to/log/file-1",
    "user-2,dataset-2,YES,12-30-03_25-12-2021,13-30-03_25-12-2021,/path/to/exports/2/,/path/to/log/file-2" };
  
  Utilities::writeToServerSessionFile(
    tmp_dir, usr_id_1, dataset_name_1, workflow_status_1,
    started_at_1, finished_at_1, path_to_exports_1, log_file_1);
  Utilities::writeToServerSessionFile(
    tmp_dir, usr_id_2, dataset_name_2, workflow_status_2,
    started_at_2, finished_at_2, path_to_exports_2, log_file_2);
  
  std::vector<std::string> results;
  if (std::filesystem::exists(tmp_dir/".serversession.ssi")) {
    std::ifstream serversession_file(tmp_dir/".serversession.ssi");
    for(std::string line; getline(serversession_file, line);) {
      results.push_back(line);
    }
  }
  
  EXPECT_EQ(expected.size(), results.size());
  for (int i = 0; i < results.size(); ++i) {
    EXPECT_STREQ(expected[i].c_str(), results[i].c_str());
  }
}

TEST(utilities, checkLastServerWorkflowRun)
{
  std::filesystem::path tmp_dir = Utilities::createEmptyTempDirectory();
  std::string usr_id_1 = "user-1";
  std::string dataset_name_1 = "dataset-1";
  std::string workflow_status_1 = "YES";
  std::string started_at_1 = "12-30-03_30-12-2021";
  std::string finished_at_1 = "13-30-03_30-12-2021";
  std::string path_to_exports_1 = "/path/to/exports/1/";
  std::string log_file_1 = "/path/to/log/file-1";
  
  Utilities::writeToServerSessionFile(
    tmp_dir, usr_id_1, dataset_name_1, workflow_status_1,
    started_at_1, finished_at_1, path_to_exports_1, log_file_1);
  
  EXPECT_TRUE(Utilities::checkLastServerWorkflowRun(tmp_dir, usr_id_1));
}

TEST(utilities, checkCSVHeader)
{
  std::string filename = SMARTPEAK_GET_TEST_DATA_PATH("SequenceParser_sequence_1_semicolon.csv");
  EXPECT_TRUE(Utilities::checkCSVHeader<';'>(filename, "sample_name", "sample_type", "sample_group_name"));
  EXPECT_FALSE(Utilities::checkCSVHeader<';'>(filename, "sample_name", "non_existing_column", "sample_group_name"));
}

TEST(utilities, hasBOMMarker)
{
  const string pathname_no_bom = SMARTPEAK_GET_TEST_DATA_PATH("FileReader_parameters.csv");
  EXPECT_FALSE(Utilities::hasBOMMarker(pathname_no_bom));

  const string pathname_utf8_bom = SMARTPEAK_GET_TEST_DATA_PATH("FileReader_parameters_bom_utf8.csv");
  EXPECT_TRUE(Utilities::hasBOMMarker(pathname_utf8_bom));

  const string pathname_16be_bom = SMARTPEAK_GET_TEST_DATA_PATH("FileReader_parameters_bom_16be.csv");
  EXPECT_TRUE(Utilities::hasBOMMarker(pathname_16be_bom));

  const string pathname_16le_bom = SMARTPEAK_GET_TEST_DATA_PATH("FileReader_parameters_bom_16le.csv");
  EXPECT_TRUE(Utilities::hasBOMMarker(pathname_16le_bom));

  const string pathname_32le_bom = SMARTPEAK_GET_TEST_DATA_PATH("FileReader_parameters_bom_32le.csv");
  EXPECT_TRUE(Utilities::hasBOMMarker(pathname_32le_bom));

  const string pathname_32be_bom = SMARTPEAK_GET_TEST_DATA_PATH("FileReader_parameters_bom_32be.csv");
  EXPECT_TRUE(Utilities::hasBOMMarker(pathname_32be_bom));
}


TEST(utilities, replaceAll)
{
  std::string str = "apple strawberry apple peach ";
  auto result1 = Utilities::replaceAll(str, "apple", "banana");
  EXPECT_EQ(result1, "banana strawberry banana peach ");
  auto result2 = Utilities::replaceAll(str, "not found", "banana");
  EXPECT_EQ(result2, "apple strawberry apple peach ");
}

TEST(utilities, OpenMSDataValueToCastValue)
{
  OpenMS::DataValue v_int(42);
  auto c_int = Utilities::OpenMSDataValueToCastValue(v_int);
  EXPECT_EQ(c_int.getTag(), CastValue::Type::INT);
  EXPECT_EQ(c_int.i_, 42);

  std::vector<int> int_list{ 42, 43, 44 };
  OpenMS::DataValue v_int_list(int_list);
  auto c_int_list = Utilities::OpenMSDataValueToCastValue(v_int_list);
  EXPECT_EQ(c_int_list.getTag(), CastValue::Type::INT_LIST);
  for (int i=0; i< c_int_list.il_.size(); ++i)
  {
    EXPECT_EQ(c_int_list.il_.at(i), int_list.at(i));
  }

  OpenMS::DataValue v_double(3.14);
  auto c_double = Utilities::OpenMSDataValueToCastValue(v_double);
  EXPECT_EQ(c_double.getTag(), CastValue::Type::FLOAT);
  EXPECT_FLOAT_EQ(c_double.f_, 3.14);

  std::vector<double> double_list{ 3.14, 13.14, 103.14 };
  OpenMS::DataValue v_double_list(double_list);
  auto c_double_list = Utilities::OpenMSDataValueToCastValue(double_list);
  EXPECT_EQ(c_double_list.getTag(), CastValue::Type::FLOAT_LIST);
  for (int i = 0; i < c_double_list.il_.size(); ++i)
  {
    EXPECT_FLOAT_EQ(c_double_list.fl_.at(i), double_list.at(i));
  }

  OpenMS::DataValue v_string("apple");
  auto c_string = Utilities::OpenMSDataValueToCastValue(v_string);
  EXPECT_EQ(c_string.getTag(), CastValue::Type::STRING);
  EXPECT_EQ(c_string.s_, "apple");

  std::vector<std::string> string_list{ "apple", "banana", "strawberry"};
  OpenMS::DataValue v_string_list(string_list);
  auto c_string_list = Utilities::OpenMSDataValueToCastValue(v_string_list);
  EXPECT_EQ(c_string_list.getTag(), CastValue::Type::STRING_LIST);
  for (int i = 0; i < c_string_list.sl_.size(); ++i)
  {
    EXPECT_EQ(c_string_list.sl_.at(i), string_list.at(i));
  }

  OpenMS::DataValue v_empty;
  auto c_empty = Utilities::OpenMSDataValueToCastValue(v_empty);
  EXPECT_EQ(c_empty.getTag(), CastValue::Type::UNINITIALIZED);
}