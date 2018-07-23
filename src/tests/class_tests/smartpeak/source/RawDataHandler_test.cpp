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
  RawDataHandler raw;

  OpenMS::FeatureMap f1;
  f1.setMetaValue("name", "foo");

  raw.setFeatureMap(f1);

  OpenMS::FeatureMap f2 = raw.getFeatureMap(); // testing copy getter
  BOOST_CHECK_EQUAL(f2.metaValueExists("name"), true);
  BOOST_CHECK_EQUAL(f2.getMetaValue("name"), "foo");

  raw.getFeatureMap().setMetaValue("name2", "bar"); // testing reference getter

  OpenMS::FeatureMap& f3 = raw.getFeatureMap();
  BOOST_CHECK_EQUAL(f3.metaValueExists("name"), true);
  BOOST_CHECK_EQUAL(f3.getMetaValue("name"), "foo");
  BOOST_CHECK_EQUAL(f3.metaValueExists("name2"), true);
  BOOST_CHECK_EQUAL(f3.getMetaValue("name2"), "bar");
}

BOOST_AUTO_TEST_CASE(set_or_get_MetaData)
{
  RawDataHandler raw;

  MetaDataHandler m1;
  string name {"foo"};
  m1.setSampleName(name);

  raw.setMetaData(m1);

  MetaDataHandler m2 = raw.getMetaData(); // testing copy getter
  BOOST_CHECK_EQUAL(m2.getSampleName(), name);

  string group_name {"bar"};
  raw.getMetaData().setSampleGroupName(group_name); // testing reference getter

  MetaDataHandler& m3 = raw.getMetaData();
  BOOST_CHECK_EQUAL(m3.getSampleName(), name);
  BOOST_CHECK_EQUAL(m3.getSampleGroupName(), group_name);
}

BOOST_AUTO_TEST_CASE(set_or_get_Parameters)
{
  map<string, vector<map<string, string>>> parameters;
  RawDataHandler raw;
  // TODO: missing tests
  // https://github.com/dmccloskey/SmartPeak2/issues/51#issuecomment-406987883
}

BOOST_AUTO_TEST_CASE(set_or_get_QuantitationMethods)
{
  RawDataHandler raw;

  OpenMS::AbsoluteQuantitationMethod aqm;
  string name {"foo"};
  aqm.setComponentName(name);

  vector<OpenMS::AbsoluteQuantitationMethod> AQMs1;
  AQMs1.push_back(aqm);

  raw.setQuantitationMethods(AQMs1);

  vector<OpenMS::AbsoluteQuantitationMethod> AQMs2 = raw.getQuantitationMethods(); // testing copy getter
  BOOST_CHECK_EQUAL(AQMs2.size(), 1);
  BOOST_CHECK_EQUAL(AQMs2[0].getComponentName(), name);

  string feature_name {"bar"};
  raw.getQuantitationMethods()[0].setFeatureName(feature_name); // testing reference getter

  vector<OpenMS::AbsoluteQuantitationMethod>& AQMs3 = raw.getQuantitationMethods();
  BOOST_CHECK_EQUAL(AQMs3[0].getComponentName(), name);
  BOOST_CHECK_EQUAL(AQMs3[0].getFeatureName(), feature_name);
}

BOOST_AUTO_TEST_CASE(set_or_get_FeatureFilter)
{
  RawDataHandler raw;

  OpenMS::MRMFeatureQC::ComponentQCs qc;
  string name {"foo"};
  qc.component_name = name;

  OpenMS::MRMFeatureQC fqc1;
  fqc1.component_qcs.push_back(qc);

  raw.setFeatureFilter(fqc1);

  OpenMS::MRMFeatureQC fqc2 = raw.getFeatureFilter(); // testing copy getter
  BOOST_CHECK_EQUAL(fqc2.component_qcs.size(), 1);
  BOOST_CHECK_EQUAL(fqc2.component_qcs[0].component_name, name);

  const double rt_low {4.0};
  raw.getFeatureFilter().component_qcs[0].retention_time_l = rt_low; // testing reference getter

  OpenMS::MRMFeatureQC& fqc3 = raw.getFeatureFilter();
  BOOST_CHECK_EQUAL(fqc3.component_qcs.size(), 1);
  BOOST_CHECK_EQUAL(fqc3.component_qcs[0].component_name, name);
  BOOST_CHECK_EQUAL(fqc3.component_qcs[0].retention_time_l, rt_low);
}

BOOST_AUTO_TEST_SUITE_END()
