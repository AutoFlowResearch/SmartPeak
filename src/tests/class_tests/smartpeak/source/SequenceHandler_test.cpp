// TODO: Add copyright

#define BOOST_TEST_MODULE SequenceHandler test suite
#include <boost/test/included/unit_test.hpp>
#include <SmartPeak/core/SequenceHandler.h>

using namespace SmartPeak;
using namespace std;

BOOST_AUTO_TEST_SUITE(sequencehandler)

BOOST_AUTO_TEST_CASE(constructor)
{
  SequenceHandler* ptr = nullptr;
  SequenceHandler* nullPointer = nullptr;
  ptr = new SequenceHandler();
  BOOST_CHECK_NE(ptr, nullPointer);
}

BOOST_AUTO_TEST_CASE(destructor)
{
  SequenceHandler* ptr = nullptr;
  ptr = new SequenceHandler();
  delete ptr;
}

BOOST_AUTO_TEST_CASE(addSampleToSequence)
{
  MetaDataHandler meta_data1;
  meta_data1.setFilename("file1");
  meta_data1.setSampleName("sample1");
  meta_data1.setSampleGroupName("sample");
  meta_data1.setSequenceSegmentName("sequence_segment1");
  meta_data1.setSampleType(MetaDataHandler::SampleType::Unknown);
  OpenMS::FeatureMap featuremap1;

  MetaDataHandler meta_data2;
  meta_data2.setFilename("file2");
  meta_data2.setSampleName("sample2");
  meta_data2.setSampleGroupName("sample");
  meta_data2.setSequenceSegmentName("sequence_segment2");
  meta_data2.setSampleType(MetaDataHandler::SampleType::Unknown);
  OpenMS::FeatureMap featuremap2;
  featuremap2.setMetaValue("foo", "bar");

  MetaDataHandler meta_data3;
  meta_data3.setFilename("file3");
  meta_data3.setSampleName("sample3");
  meta_data3.setSampleGroupName("sample");
  meta_data3.setSequenceSegmentName("sequence_segment3");
  meta_data3.setSampleType(MetaDataHandler::SampleType::Unknown);
  OpenMS::FeatureMap featuremap3;

  SequenceHandler sequenceHandler;
  sequenceHandler.addSampleToSequence(meta_data1, featuremap1);
  sequenceHandler.addSampleToSequence(meta_data2, featuremap2);
  sequenceHandler.addSampleToSequence(meta_data3, featuremap3);

  BOOST_CHECK_EQUAL(sequenceHandler.index_to_sample_.size(), 3);
  BOOST_CHECK_EQUAL(sequenceHandler.sample_to_index_.size(), 3);
  BOOST_CHECK_EQUAL(sequenceHandler.index_to_sample_.at(1), "sample2");
  BOOST_CHECK_EQUAL(sequenceHandler.sample_to_index_.at("sample2"), 1);
  BOOST_CHECK_EQUAL(sequenceHandler.getSequence().size(), 3);
  BOOST_CHECK_EQUAL(sequenceHandler.getSequence()[0].getMetaData().getSequenceSegmentName(), "sequence_segment1");
  BOOST_CHECK_EQUAL(sequenceHandler.getSequence()[0].getMetaData().getSampleName(), "sample1");
  BOOST_CHECK_EQUAL(sequenceHandler.getSequence()[1].getMetaData().getSequenceSegmentName(), "sequence_segment2");
  BOOST_CHECK_EQUAL(sequenceHandler.getSequence()[1].getMetaData().getSampleGroupName(), "sample");
  BOOST_CHECK_EQUAL(sequenceHandler.getSequence()[1].getRawData().getFeatureMap().metaValueExists("foo"), true);
  BOOST_CHECK_EQUAL(sequenceHandler.getSequence()[1].getRawData().getFeatureMap().getMetaValue("foo"), "bar");
  BOOST_CHECK_EQUAL(sequenceHandler.getSequence()[2].getMetaData().getSampleType(), MetaDataHandler::SampleType::Unknown);
}

BOOST_AUTO_TEST_CASE(getMetaValue)
{
  SequenceHandler sequenceHandler;
  OpenMS::Feature feature;
  feature.setRT(16.0);
  OpenMS::Feature subordinate;
  subordinate.setMetaValue("calculated_concentration", 10.0);

  Utilities::CastValue result;
  SequenceHandler::getMetaValue(feature, subordinate, "RT", result);
  BOOST_CHECK_EQUAL(result.getTag(), Utilities::CastValue::Type::FLOAT);
  BOOST_CHECK_CLOSE(result.f_, 16.0, 1e-6);
  SequenceHandler::getMetaValue(feature, subordinate, "calculated_concentration", result);
  BOOST_CHECK_EQUAL(result.getTag(), Utilities::CastValue::Type::FLOAT);
  BOOST_CHECK_CLOSE(result.f_, 10.0, 1e-6);
  BOOST_CHECK_THROW(SequenceHandler::getMetaValue(feature, subordinate, "not_present", result), std::invalid_argument);
}

BOOST_AUTO_TEST_CASE(getSamplesInSequence)
{
  MetaDataHandler meta_data1;
  meta_data1.setFilename("file1");
  meta_data1.setSampleName("sample1");
  meta_data1.setSampleGroupName("sample");
  meta_data1.setSequenceSegmentName("sequence_segment");
  meta_data1.setSampleType(MetaDataHandler::SampleType::Unknown);

  MetaDataHandler meta_data2;
  meta_data2.setFilename("file2");
  meta_data2.setSampleName("sample2");
  meta_data2.setSampleGroupName("sample");
  meta_data2.setSequenceSegmentName("sequence_segment");
  meta_data2.setSampleType(MetaDataHandler::SampleType::Unknown);

  MetaDataHandler meta_data3;
  meta_data3.setFilename("file3");
  meta_data3.setSampleName("sample3");
  meta_data3.setSampleGroupName("sample");
  meta_data3.setSequenceSegmentName("sequence_segment");
  meta_data3.setSampleType(MetaDataHandler::SampleType::Unknown);

  OpenMS::FeatureMap featuremap;

  SequenceHandler sequenceHandler;
  sequenceHandler.addSampleToSequence(meta_data1, featuremap);
  sequenceHandler.addSampleToSequence(meta_data2, featuremap);
  sequenceHandler.addSampleToSequence(meta_data3, featuremap);

  const vector<string> sample_names = {"sample1", "foo", "sample3"};
  std::vector<SampleHandler> samples;

  sequenceHandler.getSamplesInSequence(sample_names, samples);
  BOOST_CHECK_EQUAL(samples.size(), 2);
  BOOST_CHECK_EQUAL(samples[0].getMetaData().getSampleName(), "sample1");
  BOOST_CHECK_EQUAL(samples[1].getMetaData().getSampleName(), "sample3");
}

// BOOST_AUTO_TEST_CASE(PLEASE_REPLACE_ME)
// {
//   SequenceHandler sequenceHandler;
// }

BOOST_AUTO_TEST_SUITE_END()
