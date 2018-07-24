// TODO: Add copyright

#define BOOST_TEST_MODULE SampleGroupHandler test suite
#include <boost/test/included/unit_test.hpp>
#include <SmartPeak/core/SampleGroupHandler.h>

using namespace SmartPeak;
using namespace std;

BOOST_AUTO_TEST_SUITE(samplegrouphandler)

BOOST_AUTO_TEST_CASE(constructor)
{
  SampleGroupHandler* ptr = nullptr;
  SampleGroupHandler* nullPointer = nullptr;
  ptr = new SampleGroupHandler();
  BOOST_CHECK_NE(ptr, nullPointer);
}

BOOST_AUTO_TEST_CASE(destructor)
{
  SampleGroupHandler* ptr = nullptr;
  ptr = new SampleGroupHandler();
  delete ptr;
}

BOOST_AUTO_TEST_CASE(set_get_SampleGroupName)
{
  SampleGroupHandler sg;
  const string foo {"foo"};

  sg.setSampleGroupName(foo);

  const string name1 = sg.getSampleGroupName(); // testing copy getter
  BOOST_CHECK_EQUAL(name1, foo);

  const string bar {"bar"};
  sg.getSampleGroupName() = bar;

  string& name2 = sg.getSampleGroupName(); // testing reference getter
  BOOST_CHECK_EQUAL(name2, bar);
}

BOOST_AUTO_TEST_CASE(set_get_SampleIndices)
{
  SampleGroupHandler sg;
  vector<size_t> si1 {1, 3, 5};

  sg.setSampleIndices(si1);

  const vector<size_t> si2 = sg.getSampleIndices(); // testing copy getter
  BOOST_CHECK_EQUAL(si2.size(), 3);
  BOOST_CHECK_EQUAL(si2[0], 1);
  BOOST_CHECK_EQUAL(si2[1], 3);
  BOOST_CHECK_EQUAL(si2[2], 5);

  sg.getSampleIndices()[1] = 2;

  const vector<size_t>& si3 = sg.getSampleIndices(); // testing reference getter
  BOOST_CHECK_EQUAL(si3.size(), 3);
  BOOST_CHECK_EQUAL(si3[1], 2);
}

BOOST_AUTO_TEST_SUITE_END()
