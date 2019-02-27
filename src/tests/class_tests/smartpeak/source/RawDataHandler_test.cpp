// TODO: Add copyright

#define BOOST_TEST_MODULE RawDataHandler test suite
#include <boost/test/included/unit_test.hpp>
#include <SmartPeak/core/RawDataHandler.h>
#include <SmartPeak/io/OpenMSFile.h>

using namespace SmartPeak;
using namespace std;

BOOST_AUTO_TEST_SUITE(rawdatahandler)

BOOST_AUTO_TEST_CASE(constructor)
{
  RawDataHandler* ptr = nullptr;
  RawDataHandler* nullPointer = nullptr;
  ptr = new RawDataHandler();
  BOOST_CHECK_NE(ptr, nullPointer);
}

BOOST_AUTO_TEST_CASE(destructor)
{
  RawDataHandler* ptr = nullptr;
  ptr = new RawDataHandler();
  delete ptr;
}

BOOST_AUTO_TEST_CASE(set_get_FeatureMap)
{
  RawDataHandler rawDataHandler;

  OpenMS::FeatureMap f1;
  f1.setMetaValue("name", "foo");

  rawDataHandler.setFeatureMap(f1);

  const OpenMS::FeatureMap& f2 = rawDataHandler.getFeatureMap(); // testing const getter
  BOOST_CHECK_EQUAL(f2.metaValueExists("name"), true);
  BOOST_CHECK_EQUAL(f2.getMetaValue("name"), "foo");

  const OpenMS::FeatureMap& f2_annotated = rawDataHandler.getFeatureMapHistory(); // testing annotated feature map const getter
  BOOST_CHECK_EQUAL(f2_annotated.metaValueExists("name"), true);
  BOOST_CHECK_EQUAL(f2_annotated.getMetaValue("name"), "foo");

  rawDataHandler.getFeatureMap().setMetaValue("name2", "bar"); // testing non-const getter

  const OpenMS::FeatureMap& f3 = rawDataHandler.getFeatureMap();
  BOOST_CHECK_EQUAL(f3.metaValueExists("name"), true);
  BOOST_CHECK_EQUAL(f3.getMetaValue("name"), "foo");
  BOOST_CHECK_EQUAL(f3.metaValueExists("name2"), true);
  BOOST_CHECK_EQUAL(f3.getMetaValue("name2"), "bar");

  BOOST_CHECK_EQUAL(f2_annotated.metaValueExists("name"), true); // check that the annotated feature map has not been changed
  BOOST_CHECK_EQUAL(f2_annotated.getMetaValue("name"), "foo");

  rawDataHandler.getFeatureMapHistory().setMetaValue("name2", "bar"); // testing annotated feature map non-const getter

  const OpenMS::FeatureMap& f3_annotated = rawDataHandler.getFeatureMap();

  BOOST_CHECK_EQUAL(f3_annotated.metaValueExists("name"), true);
  BOOST_CHECK_EQUAL(f3_annotated.getMetaValue("name"), "foo");
  BOOST_CHECK_EQUAL(f3_annotated.metaValueExists("name2"), true);
  BOOST_CHECK_EQUAL(f3_annotated.getMetaValue("name2"), "bar");
}

BOOST_AUTO_TEST_CASE(set_get_FeatureMapHistory)
{
  RawDataHandler rawDataHandler;

  OpenMS::FeatureMap f1;
  f1.setMetaValue("name", "foo");

  rawDataHandler.setFeatureMapHistory(f1);

  const OpenMS::FeatureMap& f2 = rawDataHandler.getFeatureMapHistory(); // testing const getter
  BOOST_CHECK_EQUAL(f2.metaValueExists("name"), true);
  BOOST_CHECK_EQUAL(f2.getMetaValue("name"), "foo");

  rawDataHandler.getFeatureMapHistory().setMetaValue("name2", "bar"); // testing non-const getter

  const OpenMS::FeatureMap& f3 = rawDataHandler.getFeatureMapHistory();
  BOOST_CHECK_EQUAL(f3.metaValueExists("name"), true);
  BOOST_CHECK_EQUAL(f3.getMetaValue("name"), "foo");
  BOOST_CHECK_EQUAL(f3.metaValueExists("name2"), true);
  BOOST_CHECK_EQUAL(f3.getMetaValue("name2"), "bar");
}

BOOST_AUTO_TEST_CASE(set_get_MetaData)
{
  RawDataHandler rawDataHandler;

  MetaDataHandler m1;
  const string name {"foo"};
  m1.setSampleName(name);

  rawDataHandler.setMetaData(m1);

  const MetaDataHandler& m2 = rawDataHandler.getMetaData(); // testing const getter
  BOOST_CHECK_EQUAL(m2.getSampleName(), name);

  const string group_name {"bar"};
  rawDataHandler.getMetaData().setSampleGroupName(group_name); // testing non-const getter

  const MetaDataHandler& m3 = rawDataHandler.getMetaData();
  BOOST_CHECK_EQUAL(m3.getSampleName(), name);
  BOOST_CHECK_EQUAL(m3.getSampleGroupName(), group_name);
}

BOOST_AUTO_TEST_CASE(set_get_Parameters)
{
  RawDataHandler rawDataHandler;

  map<string, string> m1 = {
    {"name", "stop_report_after_feature"},
    {"value", "-1"}
  };
  vector<map<string, string>> v1;
  v1.push_back(m1);
  map<string, vector<map<string, string>>> parameters1;
  const string name1 {"MRMFeatureFinderScoring"};
  parameters1.insert({name1, v1});

  rawDataHandler.setParameters(parameters1);

  const map<string, vector<map<string, string>>>& parameters2 = rawDataHandler.getParameters();
  BOOST_CHECK_EQUAL(parameters2.count(name1), 1);
  BOOST_CHECK_EQUAL(parameters2.at(name1).size(), 1);
  BOOST_CHECK_EQUAL(parameters2.at(name1)[0].count("name"), 1);
  BOOST_CHECK_EQUAL(parameters2.at(name1)[0].at("name"), "stop_report_after_feature");
  BOOST_CHECK_EQUAL(parameters2.at(name1)[0].count("value"), 1);
  BOOST_CHECK_EQUAL(parameters2.at(name1)[0].at("value"), "-1");

  rawDataHandler.getParameters().at(name1)[0].insert({"type", "int"});

  const map<string, vector<map<string, string>>>& parameters3 = rawDataHandler.getParameters();
  BOOST_CHECK_EQUAL(parameters3.count(name1), 1);
  BOOST_CHECK_EQUAL(parameters3.at(name1).size(), 1);
  BOOST_CHECK_EQUAL(parameters3.at(name1)[0].count("name"), 1);
  BOOST_CHECK_EQUAL(parameters3.at(name1)[0].at("name"), "stop_report_after_feature");
  BOOST_CHECK_EQUAL(parameters3.at(name1)[0].count("value"), 1);
  BOOST_CHECK_EQUAL(parameters3.at(name1)[0].at("value"), "-1");
  BOOST_CHECK_EQUAL(parameters3.at(name1)[0].count("type"), 1);
  BOOST_CHECK_EQUAL(parameters3.at(name1)[0].at("type"), "int");
}

BOOST_AUTO_TEST_CASE(set_get_QuantitationMethods)
{
  RawDataHandler rawDataHandler;

  OpenMS::AbsoluteQuantitationMethod aqm;
  const string name {"foo"};
  aqm.setComponentName(name);

  vector<OpenMS::AbsoluteQuantitationMethod> AQMs1;
  AQMs1.push_back(aqm);

  rawDataHandler.setQuantitationMethods(AQMs1);

  const vector<OpenMS::AbsoluteQuantitationMethod>& AQMs2 = rawDataHandler.getQuantitationMethods(); // testing const getter
  BOOST_CHECK_EQUAL(AQMs2.size(), 1);
  BOOST_CHECK_EQUAL(AQMs2[0].getComponentName(), name);

  const string feature_name {"bar"};
  rawDataHandler.getQuantitationMethods()[0].setFeatureName(feature_name); // testing non-const getter

  const vector<OpenMS::AbsoluteQuantitationMethod>& AQMs3 = rawDataHandler.getQuantitationMethods();
  BOOST_CHECK_EQUAL(AQMs3[0].getComponentName(), name);
  BOOST_CHECK_EQUAL(AQMs3[0].getFeatureName(), feature_name);
}

BOOST_AUTO_TEST_CASE(set_get_FeatureFilter)
{
  RawDataHandler rawDataHandler;

  OpenMS::MRMFeatureQC::ComponentQCs qc;
  const string name {"foo"};
  qc.component_name = name;

  OpenMS::MRMFeatureQC fqc1;
  fqc1.component_qcs.push_back(qc);

  rawDataHandler.setFeatureFilter(fqc1);

  const OpenMS::MRMFeatureQC& fqc2 = rawDataHandler.getFeatureFilter(); // testing const getter
  BOOST_CHECK_EQUAL(fqc2.component_qcs.size(), 1);
  BOOST_CHECK_EQUAL(fqc2.component_qcs[0].component_name, name);

  const double rt_low {4.0};
  rawDataHandler.getFeatureFilter().component_qcs[0].retention_time_l = rt_low; // testing non-const getter

  const OpenMS::MRMFeatureQC& fqc3 = rawDataHandler.getFeatureFilter();
  BOOST_CHECK_EQUAL(fqc3.component_qcs.size(), 1);
  BOOST_CHECK_EQUAL(fqc3.component_qcs[0].component_name, name);
  BOOST_CHECK_EQUAL(fqc3.component_qcs[0].retention_time_l, rt_low);
}

BOOST_AUTO_TEST_CASE(set_get_FeatureQC)
{
  RawDataHandler rawDataHandler;

  OpenMS::MRMFeatureQC::ComponentQCs qc;
  const string name {"foo"};
  qc.component_name = name;

  OpenMS::MRMFeatureQC fqc1;
  fqc1.component_qcs.push_back(qc);

  rawDataHandler.setFeatureQC(fqc1);

  const OpenMS::MRMFeatureQC& fqc2 = rawDataHandler.getFeatureQC(); // testing const getter
  BOOST_CHECK_EQUAL(fqc2.component_qcs.size(), 1);
  BOOST_CHECK_EQUAL(fqc2.component_qcs[0].component_name, name);

  const double rt_low {4.0};
  rawDataHandler.getFeatureQC().component_qcs[0].retention_time_l = rt_low; // testing non-const getter

  const OpenMS::MRMFeatureQC& fqc3 = rawDataHandler.getFeatureQC();
  BOOST_CHECK_EQUAL(fqc3.component_qcs.size(), 1);
  BOOST_CHECK_EQUAL(fqc3.component_qcs[0].component_name, name);
  BOOST_CHECK_EQUAL(fqc3.component_qcs[0].retention_time_l, rt_low);
}

BOOST_AUTO_TEST_CASE(set_get_Experiment)
{
  RawDataHandler rawDataHandler;

  OpenMS::MSExperiment experiment;
  experiment.setMetaValue("name", "foo");

  rawDataHandler.setExperiment(experiment);

  const OpenMS::MSExperiment& experiment2 = rawDataHandler.getExperiment(); // testing const getter
  BOOST_CHECK_EQUAL(experiment2.metaValueExists("name"), true);
  BOOST_CHECK_EQUAL(experiment2.getMetaValue("name"), "foo");

  rawDataHandler.getExperiment().setMetaValue("name2", "bar"); // testing non-const getter

  const OpenMS::MSExperiment& experiment3 = rawDataHandler.getExperiment();
  BOOST_CHECK_EQUAL(experiment3.metaValueExists("name"), true);
  BOOST_CHECK_EQUAL(experiment3.getMetaValue("name"), "foo");
  BOOST_CHECK_EQUAL(experiment3.metaValueExists("name2"), true);
  BOOST_CHECK_EQUAL(experiment3.getMetaValue("name2"), "bar");
}

BOOST_AUTO_TEST_CASE(set_get_ChromatogramMap)
{
  RawDataHandler rawDataHandler;

  OpenMS::MSExperiment chromatogram_map;
  chromatogram_map.setMetaValue("name", "foo");

  rawDataHandler.setChromatogramMap(chromatogram_map);

  const OpenMS::MSExperiment& chromatogram_map2 = rawDataHandler.getChromatogramMap(); // testing const getter
  BOOST_CHECK_EQUAL(chromatogram_map2.metaValueExists("name"), true);
  BOOST_CHECK_EQUAL(chromatogram_map2.getMetaValue("name"), "foo");

  rawDataHandler.getChromatogramMap().setMetaValue("name2", "bar"); // testing non-const getter

  const OpenMS::MSExperiment& chromatogram_map3 = rawDataHandler.getChromatogramMap();
  BOOST_CHECK_EQUAL(chromatogram_map3.metaValueExists("name"), true);
  BOOST_CHECK_EQUAL(chromatogram_map3.getMetaValue("name"), "foo");
  BOOST_CHECK_EQUAL(chromatogram_map3.metaValueExists("name2"), true);
  BOOST_CHECK_EQUAL(chromatogram_map3.getMetaValue("name2"), "bar");
}

BOOST_AUTO_TEST_CASE(set_get_ReferenceData)
{
  RawDataHandler rawDataHandler;
  std::vector<std::map<std::string, Utilities::CastValue>> data;
  Utilities::CastValue c("bar");

  map<string, Utilities::CastValue> m /*= {{"foo", c}}*/;
  m.emplace("foo", c);
  c = "bar2";
  m.emplace("foo2", c);
  data.push_back(m);

  rawDataHandler.setReferenceData(data);

  const std::vector<std::map<std::string, Utilities::CastValue>>& data2 = rawDataHandler.getReferenceData();

  BOOST_CHECK_EQUAL(data2.size(), 1);
  BOOST_CHECK_EQUAL(data2[0].count("foo"), 1);
  BOOST_CHECK_EQUAL(data2[0].at("foo").s_, "bar");
  BOOST_CHECK_EQUAL(data2[0].count("foo2"), 1);
  BOOST_CHECK_EQUAL(data2[0].at("foo2").s_, "bar2");

  c = "bar3";

  rawDataHandler.getReferenceData().push_back({{"foo3", c}});

  const std::vector<std::map<std::string, Utilities::CastValue>>& data3 = rawDataHandler.getReferenceData();

  BOOST_CHECK_EQUAL(data3.size(), 2);
  BOOST_CHECK_EQUAL(data3[0].count("foo"), 1);
  BOOST_CHECK_EQUAL(data3[0].at("foo").s_, "bar");
  BOOST_CHECK_EQUAL(data3[0].count("foo2"), 1);
  BOOST_CHECK_EQUAL(data3[0].at("foo2").s_, "bar2");
  BOOST_CHECK_EQUAL(data3[1].count("foo3"), 1);
  BOOST_CHECK_EQUAL(data3[1].at("foo3").s_, "bar3");
}

BOOST_AUTO_TEST_CASE(clear)
{
  RawDataHandler rawDataHandler;

  OpenMS::FeatureMap f1;
  f1.setMetaValue("name", "foo");
  rawDataHandler.setFeatureMap(f1);

  MetaDataHandler m1;
  m1.setSampleName("foo");
  rawDataHandler.setMetaData(m1);

  OpenMS::AbsoluteQuantitationMethod aqm;
  aqm.setComponentName("foo");
  vector<OpenMS::AbsoluteQuantitationMethod> AQMs1 = { aqm };
  rawDataHandler.setQuantitationMethods(AQMs1);

  OpenMS::MRMFeatureQC::ComponentQCs qc;
  qc.component_name = "foo";
  OpenMS::MRMFeatureQC fqc1;
  fqc1.component_qcs.push_back(qc);
  rawDataHandler.setFeatureFilter(fqc1);

  rawDataHandler.setFeatureQC(fqc1);

  OpenMS::FeatureMap featureMap_v = f1;
  rawDataHandler.setFeatureMapHistory(featureMap_v);

  OpenMS::MSExperiment experiment;
  experiment.setMetaValue("name", "foo");
  rawDataHandler.setExperiment(experiment);

  OpenMS::MSExperiment chromatogram_map;
  chromatogram_map.setMetaValue("name", "foo");
  rawDataHandler.setChromatogramMap(chromatogram_map);

  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureMap().getMetaValue("name"), "foo");
  BOOST_CHECK_EQUAL(rawDataHandler.getMetaData().getSampleName(), "foo");
  BOOST_CHECK_EQUAL(rawDataHandler.getQuantitationMethods().front().getComponentName(), "foo");
  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureFilter().component_qcs.front().component_name, "foo");
  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureQC().component_qcs.front().component_name, "foo");
  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureMapHistory().getMetaValue("name"), "foo");
  BOOST_CHECK_EQUAL(rawDataHandler.getExperiment().getMetaValue("name"), "foo");
  BOOST_CHECK_EQUAL(rawDataHandler.getChromatogramMap().getMetaValue("name"), "foo");
}

BOOST_AUTO_TEST_CASE(updateFeatureMapHistory)
{
  map<string, vector<map<string, string>>> params_1;
  map<string, vector<map<string, string>>> params_2;
  load_data(params_1, params_2);

  OpenMS::FeatureMap f1, f1_annotated;

  const string traML_csv_i = SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_traML_1.csv");
  OpenMSFile::loadTraML(rawDataHandler, traML_csv_i, "csv");

  const string mzML_i = SMARTPEAK_GET_TEST_DATA_PATH("RawDataProcessor_mzML_1.mzML");
  OpenMSFile::loadMSExperiment(rawDataHandler, mzML_i, params_1.at("MRMMapping"));

  RawDataProcessor::extractMetaData(rawDataHandler);

  const string featureXML_o1 = SMARTPEAK_GET_TEST_DATA_PATH("RawDataProcessor_test_1_core_RawDataProcessor.featureXML");
  OpenMSFile::loadFeatureMap(rawDataHandler, featureXML_o1);

  RawDataProcessor::saveCurrentFeatureMapToHistory(rawDataHandler);

  const string featureXML_o3 = SMARTPEAK_GET_TEST_DATA_PATH("RawDataProcessor_test_3_core_RawDataProcessor.featureXML");
  OpenMSFile::loadFeatureMap(rawDataHandler, featureXML_o3);

  RawDataProcessor::updateFeatureMapHistory(rawDataHandler);

  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureMap().size(), 481);
  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureMap()[0].getSubordinates().size(), 3);

  const OpenMS::Feature& subordinate0 = rawDataHandler.getFeatureMap()[0].getSubordinates()[0];
  BOOST_CHECK_EQUAL(subordinate0.getMetaValue("used_").toBool(), true);
  BOOST_CHECK_EQUAL(subordinate0.getMetaValue("native_id").toString(), "23dpg.23dpg_1.Heavy");

  const OpenMS::Feature& subordinate1 = rawDataHandler.getFeatureMap()[50].getSubordinates()[0];
  BOOST_CHECK_EQUAL(subordinate1.getMetaValue("used_").toBool(), false);
  BOOST_CHECK_EQUAL(subordinate1.getMetaValue("native_id").toString(), "accoa.accoa_1.Heavy");
}

BOOST_AUTO_TEST_SUITE_END()
