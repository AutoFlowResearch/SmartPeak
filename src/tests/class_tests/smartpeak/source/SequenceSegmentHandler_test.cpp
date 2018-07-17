// TODO: Add copyright

#define BOOST_TEST_MODULE SequenceSegmentHandler test suite 
#include <boost/test/included/unit_test.hpp>
#include <SmartPeak/core/SequenceSegmentHandler.h>

using namespace SmartPeak;
using namespace std;

BOOST_AUTO_TEST_SUITE(sequencesegmenthandler)

BOOST_AUTO_TEST_CASE(constructor) 
{
  SequenceSegmentHandler* ptr = nullptr;
  SequenceSegmentHandler* nullPointer = nullptr;
	ptr = new SequenceSegmentHandler();
  BOOST_CHECK_NE(ptr, nullPointer);
}

BOOST_AUTO_TEST_CASE(destructor) 
{
  SequenceSegmentHandler* ptr = nullptr;
	ptr = new SequenceSegmentHandler();
  delete ptr;
}

BOOST_AUTO_TEST_SUITE_END()
