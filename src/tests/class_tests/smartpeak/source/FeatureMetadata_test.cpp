// TODO: Add copyright

#define BOOST_TEST_MODULE FeatureMetadata test suite
#include <boost/test/included/unit_test.hpp>
#include <SmartPeak/core/FeatureMetadata.h>

using namespace SmartPeak;
using namespace std;

BOOST_AUTO_TEST_SUITE(featuremetadata)

BOOST_AUTO_TEST_CASE(check_sizes)
{
  BOOST_CHECK_EQUAL(metadataToString.size(), FeatureMetadataSize);
}

BOOST_AUTO_TEST_SUITE_END()
