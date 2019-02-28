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
  meta_data1.acq_method_name = "6";
  meta_data1.inj_volume = 7.0;
  meta_data1.inj_volume_units = "8";
  meta_data1.batch_name = "9";
  OpenMS::FeatureMap featuremap1;

  MetaDataHandler meta_data2;
  meta_data2.setFilename("file2");
  meta_data2.setSampleName("sample2");
  meta_data2.setSampleGroupName("sample");
  meta_data2.setSequenceSegmentName("sequence_segment2");
  meta_data2.setSampleType(MetaDataHandler::SampleType::Unknown);
  meta_data2.acq_method_name = "6";
  meta_data2.inj_volume = 7.0;
  meta_data2.inj_volume_units = "8";
  meta_data2.batch_name = "9";
  OpenMS::FeatureMap featuremap2;
  featuremap2.setMetaValue("foo", "bar");

  MetaDataHandler meta_data3;
  meta_data3.setFilename("file3");
  meta_data3.setSampleName("sample3");
  meta_data3.setSampleGroupName("sample");
  meta_data3.setSequenceSegmentName("sequence_segment3");
  meta_data3.setSampleType(MetaDataHandler::SampleType::Unknown);
  meta_data3.acq_method_name = "6";
  meta_data3.inj_volume = 7.0;
  meta_data3.inj_volume_units = "8";
  meta_data3.batch_name = "9";
  OpenMS::FeatureMap featuremap3;

  SequenceHandler sequenceHandler;
  sequenceHandler.addSampleToSequence(meta_data1, featuremap1);
  sequenceHandler.addSampleToSequence(meta_data2, featuremap2);
  sequenceHandler.addSampleToSequence(meta_data3, featuremap3);

  BOOST_CHECK_EQUAL(sequenceHandler.index_to_sample_.size(), 3);
  BOOST_CHECK_EQUAL(sequenceHandler.sample_to_index_.size(), 3);
  BOOST_CHECK_EQUAL(sequenceHandler.index_to_sample_.at(1), "sample2_-1_9_1900-01-01_000000");
  BOOST_CHECK_EQUAL(sequenceHandler.sample_to_index_.at("sample2_-1_9_1900-01-01_000000"), 1);
  BOOST_CHECK_EQUAL(sequenceHandler.getSequence().size(), 3);
  BOOST_CHECK_EQUAL(sequenceHandler.getSequence()[0].getMetaData().getSequenceSegmentName(), "sequence_segment1");
  BOOST_CHECK_EQUAL(sequenceHandler.getSequence()[0].getMetaData().getSampleName(), "sample1");
  BOOST_CHECK_EQUAL(sequenceHandler.getSequence()[1].getMetaData().getSequenceSegmentName(), "sequence_segment2");
  BOOST_CHECK_EQUAL(sequenceHandler.getSequence()[1].getMetaData().getSampleGroupName(), "sample");
  BOOST_CHECK_EQUAL(sequenceHandler.getSequence()[1].getRawData().getFeatureMap().metaValueExists("foo"), true);
  BOOST_CHECK_EQUAL(sequenceHandler.getSequence()[1].getRawData().getFeatureMap().getMetaValue("foo"), "bar");
  BOOST_CHECK_EQUAL(sequenceHandler.getSequence()[1].getRawData().getFeatureMapHistory().metaValueExists("foo"), true);
  BOOST_CHECK_EQUAL(sequenceHandler.getSequence()[1].getRawData().getFeatureMapHistory().getMetaValue("foo"), "bar");
  BOOST_CHECK_EQUAL(sequenceHandler.getSequence()[2].getMetaData().getSampleType(), MetaDataHandler::SampleType::Unknown);
}

BOOST_AUTO_TEST_CASE(getMetaValue)
{
  SequenceHandler sequenceHandler;
  OpenMS::Feature feature;
  feature.setRT(16.0);
  feature.setIntensity(1.0e4);
  OpenMS::Feature subordinate;
  subordinate.setIntensity(1.0e2);
  subordinate.setMetaValue("calculated_concentration", 10.0);

  Utilities::CastValue result;

  result = SequenceHandler::getMetaValue(feature, subordinate, "RT");
  BOOST_CHECK_EQUAL(result.getTag(), Utilities::CastValue::Type::FLOAT);
  BOOST_CHECK_CLOSE(result.f_, 16.0, 1e-6);

  result = SequenceHandler::getMetaValue(feature, subordinate, "intensity");
  BOOST_CHECK_EQUAL(result.getTag(), Utilities::CastValue::Type::FLOAT);
  BOOST_CHECK_CLOSE(result.f_, 1.0e4, 1e-6);

  result = SequenceHandler::getMetaValue(feature, subordinate, "peak_area");
  BOOST_CHECK_EQUAL(result.getTag(), Utilities::CastValue::Type::FLOAT);
  BOOST_CHECK_CLOSE(result.f_, 1.0e2, 1e-6);

  result = SequenceHandler::getMetaValue(feature, subordinate, "calculated_concentration");
  BOOST_CHECK_EQUAL(result.getTag(), Utilities::CastValue::Type::FLOAT);
  BOOST_CHECK_CLOSE(result.f_, 10.0, 1e-6);

  result = SequenceHandler::getMetaValue(feature, subordinate, "absent_meta_value");
  BOOST_CHECK_EQUAL(result.getTag(), Utilities::CastValue::Type::STRING);
  BOOST_CHECK_EQUAL(result.s_, "");
}

BOOST_AUTO_TEST_CASE(getSamplesInSequence)
{
  MetaDataHandler meta_data1;
  meta_data1.setFilename("file1");
  meta_data1.setSampleName("sample1");
  meta_data1.setSampleGroupName("sample");
  meta_data1.setSequenceSegmentName("sequence_segment");
  meta_data1.setSampleType(MetaDataHandler::SampleType::Unknown);
  meta_data1.acq_method_name = "6";
  meta_data1.inj_volume = 7.0;
  meta_data1.inj_volume_units = "8";
  meta_data1.batch_name = "9";

  MetaDataHandler meta_data2;
  meta_data2.setFilename("file2");
  meta_data2.setSampleName("sample2");
  meta_data2.setSampleGroupName("sample");
  meta_data2.setSequenceSegmentName("sequence_segment");
  meta_data2.setSampleType(MetaDataHandler::SampleType::Unknown);
  meta_data2.acq_method_name = "6";
  meta_data2.inj_volume = 7.0;
  meta_data2.inj_volume_units = "8";
  meta_data2.batch_name = "9";

  MetaDataHandler meta_data3;
  meta_data3.setFilename("file3");
  meta_data3.setSampleName("sample3");
  meta_data3.setSampleGroupName("sample");
  meta_data3.setSequenceSegmentName("sequence_segment");
  meta_data3.setSampleType(MetaDataHandler::SampleType::Unknown);
  meta_data3.acq_method_name = "6";
  meta_data3.inj_volume = 7.0;
  meta_data3.inj_volume_units = "8";
  meta_data3.batch_name = "9";

  OpenMS::FeatureMap featuremap;

  SequenceHandler sequenceHandler;
  sequenceHandler.addSampleToSequence(meta_data1, featuremap);
  sequenceHandler.addSampleToSequence(meta_data2, featuremap);
  sequenceHandler.addSampleToSequence(meta_data3, featuremap);

  const set<string> injection_names = {
    "sample1_-1_9_1900-01-01_000000",
    "foo",
    "sample3_-1_9_1900-01-01_000000"
  };

  const std::vector<InjectionHandler> samples = sequenceHandler.getSamplesInSequence(injection_names);

  BOOST_CHECK_EQUAL(samples.size(), 2);
  BOOST_CHECK_EQUAL(samples[0].getMetaData().getSampleName(), "sample1");
  BOOST_CHECK_EQUAL(samples[1].getMetaData().getSampleName(), "sample3");
}

// BOOST_AUTO_TEST_CASE(PLEASE_REPLACE_ME)
// {
//   SequenceHandler sequenceHandler;
// }

BOOST_AUTO_TEST_SUITE_END()
