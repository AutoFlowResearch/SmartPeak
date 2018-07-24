// TODO: Add copyright

#define BOOST_TEST_MODULE RawDataHandler test suite
#include <boost/test/included/unit_test.hpp>
#include <SmartPeak/core/RawDataHandler.h>

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

BOOST_AUTO_TEST_CASE(set_or_get_FeatureMap)
{
  RawDataHandler rawDataHandler;

  OpenMS::FeatureMap f1;
  f1.setMetaValue("name", "foo");

  rawDataHandler.setFeatureMap(f1);

  const OpenMS::FeatureMap f2 = rawDataHandler.getFeatureMap(); // testing copy getter
  BOOST_CHECK_EQUAL(f2.metaValueExists("name"), true);
  BOOST_CHECK_EQUAL(f2.getMetaValue("name"), "foo");

  rawDataHandler.getFeatureMap().setMetaValue("name2", "bar"); // testing reference getter

  const OpenMS::FeatureMap& f3 = rawDataHandler.getFeatureMap();
  BOOST_CHECK_EQUAL(f3.metaValueExists("name"), true);
  BOOST_CHECK_EQUAL(f3.getMetaValue("name"), "foo");
  BOOST_CHECK_EQUAL(f3.metaValueExists("name2"), true);
  BOOST_CHECK_EQUAL(f3.getMetaValue("name2"), "bar");
}

BOOST_AUTO_TEST_CASE(set_or_get_MetaData)
{
  RawDataHandler rawDataHandler;

  MetaDataHandler m1;
  const string name {"foo"};
  m1.setSampleName(name);

  rawDataHandler.setMetaData(m1);

  const MetaDataHandler m2 = rawDataHandler.getMetaData(); // testing copy getter
  BOOST_CHECK_EQUAL(m2.getSampleName(), name);

  const string group_name {"bar"};
  rawDataHandler.getMetaData().setSampleGroupName(group_name); // testing reference getter

  const MetaDataHandler& m3 = rawDataHandler.getMetaData();
  BOOST_CHECK_EQUAL(m3.getSampleName(), name);
  BOOST_CHECK_EQUAL(m3.getSampleGroupName(), group_name);
}

BOOST_AUTO_TEST_CASE(set_or_get_Parameters)
{
  map<string, vector<map<string, string>>> parameters;
  RawDataHandler rawDataHandler;
  // TODO: missing tests
  // https://github.com/dmccloskey/SmartPeak2/issues/51#issuecomment-406987883
}

BOOST_AUTO_TEST_CASE(set_or_get_QuantitationMethods)
{
  RawDataHandler rawDataHandler;

  OpenMS::AbsoluteQuantitationMethod aqm;
  const string name {"foo"};
  aqm.setComponentName(name);

  vector<OpenMS::AbsoluteQuantitationMethod> AQMs1;
  AQMs1.push_back(aqm);

  rawDataHandler.setQuantitationMethods(AQMs1);

  const vector<OpenMS::AbsoluteQuantitationMethod> AQMs2 = rawDataHandler.getQuantitationMethods(); // testing copy getter
  BOOST_CHECK_EQUAL(AQMs2.size(), 1);
  BOOST_CHECK_EQUAL(AQMs2[0].getComponentName(), name);

  const string feature_name {"bar"};
  rawDataHandler.getQuantitationMethods()[0].setFeatureName(feature_name); // testing reference getter

  const vector<OpenMS::AbsoluteQuantitationMethod>& AQMs3 = rawDataHandler.getQuantitationMethods();
  BOOST_CHECK_EQUAL(AQMs3[0].getComponentName(), name);
  BOOST_CHECK_EQUAL(AQMs3[0].getFeatureName(), feature_name);
}

BOOST_AUTO_TEST_CASE(set_or_get_FeatureFilter)
{
  RawDataHandler rawDataHandler;

  OpenMS::MRMFeatureQC::ComponentQCs qc;
  const string name {"foo"};
  qc.component_name = name;

  OpenMS::MRMFeatureQC fqc1;
  fqc1.component_qcs.push_back(qc);

  rawDataHandler.setFeatureFilter(fqc1);

  const OpenMS::MRMFeatureQC fqc2 = rawDataHandler.getFeatureFilter(); // testing copy getter
  BOOST_CHECK_EQUAL(fqc2.component_qcs.size(), 1);
  BOOST_CHECK_EQUAL(fqc2.component_qcs[0].component_name, name);

  const double rt_low {4.0};
  rawDataHandler.getFeatureFilter().component_qcs[0].retention_time_l = rt_low; // testing reference getter

  const OpenMS::MRMFeatureQC& fqc3 = rawDataHandler.getFeatureFilter();
  BOOST_CHECK_EQUAL(fqc3.component_qcs.size(), 1);
  BOOST_CHECK_EQUAL(fqc3.component_qcs[0].component_name, name);
  BOOST_CHECK_EQUAL(fqc3.component_qcs[0].retention_time_l, rt_low);
}

BOOST_AUTO_TEST_CASE(set_or_get_FeatureQC)
{
  RawDataHandler rawDataHandler;

  OpenMS::MRMFeatureQC::ComponentQCs qc;
  const string name {"foo"};
  qc.component_name = name;

  OpenMS::MRMFeatureQC fqc1;
  fqc1.component_qcs.push_back(qc);

  rawDataHandler.setFeatureQC(fqc1);

  const OpenMS::MRMFeatureQC fqc2 = rawDataHandler.getFeatureQC(); // testing copy getter
  BOOST_CHECK_EQUAL(fqc2.component_qcs.size(), 1);
  BOOST_CHECK_EQUAL(fqc2.component_qcs[0].component_name, name);

  const double rt_low {4.0};
  rawDataHandler.getFeatureQC().component_qcs[0].retention_time_l = rt_low; // testing reference getter

  const OpenMS::MRMFeatureQC& fqc3 = rawDataHandler.getFeatureQC();
  BOOST_CHECK_EQUAL(fqc3.component_qcs.size(), 1);
  BOOST_CHECK_EQUAL(fqc3.component_qcs[0].component_name, name);
  BOOST_CHECK_EQUAL(fqc3.component_qcs[0].retention_time_l, rt_low);
}

BOOST_AUTO_TEST_CASE(set_or_get_FeatureMapHistory)
{
  RawDataHandler rawDataHandler;

  OpenMS::FeatureMap f;
  f.setMetaValue("name", "foo");
  vector<OpenMS::FeatureMap> f1;
  f1.push_back(f);

  rawDataHandler.setFeatureMapHistory(f1);

  const vector<OpenMS::FeatureMap> f2 = rawDataHandler.getFeatureMapHistory(); // testing copy getter
  BOOST_CHECK_EQUAL(f2.size(), 1);
  BOOST_CHECK_EQUAL(f2[0].metaValueExists("name"), true);
  BOOST_CHECK_EQUAL(f2[0].getMetaValue("name"), "foo");

  rawDataHandler.getFeatureMapHistory()[0].setMetaValue("name2", "bar"); // testing reference getter

  const vector<OpenMS::FeatureMap>& f3 = rawDataHandler.getFeatureMapHistory();
  BOOST_CHECK_EQUAL(f3.size(), 1);
  BOOST_CHECK_EQUAL(f3[0].metaValueExists("name"), true);
  BOOST_CHECK_EQUAL(f3[0].getMetaValue("name"), "foo");
  BOOST_CHECK_EQUAL(f3[0].metaValueExists("name2"), true);
  BOOST_CHECK_EQUAL(f3[0].getMetaValue("name2"), "bar");
}

BOOST_AUTO_TEST_SUITE_END()
