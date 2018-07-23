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
  std::string name {"foo"};
  m1.setSampleName(name);

  raw.setMetaData(m1);

  MetaDataHandler m2 = raw.getMetaData(); // testing copy getter
  BOOST_CHECK_EQUAL(m2.getSampleName(), name);

  std::string group_name {"bar"};
  raw.getMetaData().setSampleGroupName(group_name); // testing reference getter

  MetaDataHandler& m3 = raw.getMetaData();
  BOOST_CHECK_EQUAL(m3.getSampleName(), name);
  BOOST_CHECK_EQUAL(m3.getSampleGroupName(), group_name);
}

BOOST_AUTO_TEST_SUITE_END()
