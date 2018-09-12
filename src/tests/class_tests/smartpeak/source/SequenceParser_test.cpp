// TODO: Add copyright

#include <SmartPeak/test_config.h>

#define BOOST_TEST_MODULE SequenceParser test suite
#include <boost/test/included/unit_test.hpp>
#include <SmartPeak/io/SequenceParser.h>

using namespace SmartPeak;
using namespace std;

BOOST_AUTO_TEST_SUITE(sequenceparser)

BOOST_AUTO_TEST_CASE(castValue_constructor_copyConstructor)
{
  const string pathname = SMARTPEAK_GET_TEST_DATA_PATH("SequenceParser_sequence_1.csv");
  SequenceHandler sequenceHandler;
  SequenceParser::readSequenceFile(sequenceHandler, pathname);
//   BOOST_CHECK_EQUAL(c.getTag(), Utilities::CastValue::UNKNOWN);
//   BOOST_CHECK_EQUAL(c.s_, "");
  const string output_path = SMARTPEAK_GET_TEST_DATA_PATH("write_dataMatrixFromMetaValue_out.csv");
  SequenceParser::write_dataMatrixFromMetaValue(sequenceHandler, output_path);
}

BOOST_AUTO_TEST_SUITE_END()
