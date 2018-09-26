// TODO: Add copyright

// #include <SmartPeak/test_config.h>

#define BOOST_TEST_MODULE SequenceProcessor test suite
#include <boost/test/included/unit_test.hpp>
#include <SmartPeak/core/SequenceProcessor.h>

using namespace SmartPeak;
using namespace std;

BOOST_AUTO_TEST_SUITE(sequenceprocessor)

BOOST_AUTO_TEST_CASE(createSequence)
{
}

BOOST_AUTO_TEST_CASE(addRawDataHandlerToSequence)
{
}

BOOST_AUTO_TEST_CASE(segmentSamplesInSequence)
{
  MetaDataHandler meta_data1;
  meta_data1.setFilename("file1");
  meta_data1.setSampleName("sample1");
  meta_data1.setSampleGroupName("sample");
  meta_data1.setSequenceSegmentName("sequence_segment1");
  meta_data1.setSampleType(MetaDataHandler::SampleType::Unknown);

  MetaDataHandler meta_data2;
  meta_data2.setFilename("file2");
  meta_data2.setSampleName("sample2");
  meta_data2.setSampleGroupName("sample");
  meta_data2.setSequenceSegmentName("sequence_segment1");
  meta_data2.setSampleType(MetaDataHandler::SampleType::Standard);

  MetaDataHandler meta_data3;
  meta_data3.setFilename("file3");
  meta_data3.setSampleName("sample3");
  meta_data3.setSampleGroupName("sample");
  meta_data3.setSequenceSegmentName("sequence_segment2");
  meta_data3.setSampleType(MetaDataHandler::SampleType::Unknown);

  OpenMS::FeatureMap featuremap;

  SequenceHandler sequenceHandler;
  sequenceHandler.addSampleToSequence(meta_data1, featuremap);
  sequenceHandler.addSampleToSequence(meta_data2, featuremap);
  sequenceHandler.addSampleToSequence(meta_data3, featuremap);

  OpenMS::AbsoluteQuantitationMethod aqm;
  aqm.setComponentName("Test");
  SequenceSegmentHandler sequenceSegmentHandler;
  sequenceSegmentHandler.setQuantitationMethods({aqm});

  SequenceProcessor::segmentSamplesInSequence(sequenceHandler, sequenceSegmentHandler);

  BOOST_CHECK_EQUAL(sequenceHandler.getSequenceSegments().size(), 2);
  BOOST_CHECK_EQUAL(sequenceHandler.getSequenceSegments()[0].getSampleIndices().size(), 2);
  BOOST_CHECK_EQUAL(sequenceHandler.getSequenceSegments()[0].getSampleIndices()[0], 0);
  BOOST_CHECK_EQUAL(sequenceHandler.getSequenceSegments()[0].getSampleIndices()[1], 1);
  BOOST_CHECK_EQUAL(sequenceHandler.getSequenceSegments()[1].getSampleIndices().size(), 1);
  BOOST_CHECK_EQUAL(sequenceHandler.getSequenceSegments()[1].getSampleIndices()[0], 2);
  BOOST_CHECK_EQUAL(sequenceHandler.getSequenceSegments()[0].getQuantitationMethods().size(), 1);
  BOOST_CHECK_EQUAL(sequenceHandler.getSequenceSegments()[0].getQuantitationMethods()[0].getComponentName(), "Test");
}

BOOST_AUTO_TEST_CASE(processSequence)
{
}

BOOST_AUTO_TEST_CASE(processSequenceSegments)
{
}

BOOST_AUTO_TEST_SUITE_END()
