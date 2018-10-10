// TODO: Add copyright

#include <SmartPeak/test_config.h>

#define BOOST_TEST_MODULE SequenceParser test suite
#include <boost/test/included/unit_test.hpp>
#include <SmartPeak/io/SequenceParser.h>
#include <SmartPeak/io/OpenMSFile.h>
#include <SmartPeak/core/MetaDataHandler.h>

using namespace SmartPeak;
using namespace std;

BOOST_AUTO_TEST_SUITE(sequenceparser)

BOOST_AUTO_TEST_CASE(readSequenceFile)
{
  string pathname = SMARTPEAK_GET_TEST_DATA_PATH("SequenceParser_sequence_1.csv");
  SequenceHandler sequenceHandler;
  SequenceParser::readSequenceFile(sequenceHandler, pathname, ",");
  vector<SampleHandler>& sequence1 = sequenceHandler.getSequence();
  BOOST_CHECK_EQUAL(sequence1.size(), 6);
  BOOST_CHECK_EQUAL(sequence1[0].getMetaData().getSampleName(), "170808_Jonathan_yeast_Sacc1_1x");
  BOOST_CHECK_EQUAL(sequence1[0].getMetaData().getSampleGroupName(), "Test01");
  BOOST_CHECK_EQUAL(sequence1[0].getMetaData().getSequenceSegmentName(), "Group01");
  BOOST_CHECK_EQUAL(sequence1[0].getMetaData().getSampleType(), MetaDataHandler::SampleType::Unknown);
  BOOST_CHECK_EQUAL(sequence1[0].getMetaData().getFilename(), "/home/user/code/test/data/mzML/170808_Jonathan_yeast_Sacc1_1x.mzML");
  BOOST_CHECK_EQUAL(sequence1[5].getMetaData().getSampleName(), "170808_Jonathan_yeast_Yarr3_1x");
  BOOST_CHECK_EQUAL(sequence1[5].getMetaData().getSampleGroupName(), "Test01");
  BOOST_CHECK_EQUAL(sequence1[5].getMetaData().getSequenceSegmentName(), "Group01");
  BOOST_CHECK_EQUAL(sequence1[5].getMetaData().getSampleType(), MetaDataHandler::SampleType::Unknown);
  BOOST_CHECK_EQUAL(sequence1[5].getMetaData().getFilename(), "/home/user/code/test/data/mzML/170808_Jonathan_yeast_Sacc1_1x.mzML");

  sequenceHandler.clear();
  pathname = SMARTPEAK_GET_TEST_DATA_PATH("SequenceParser_sequence_1_semicolon.csv");
  SequenceParser::readSequenceFile(sequenceHandler, pathname, ";");
  vector<SampleHandler>& sequence2 = sequenceHandler.getSequence();
  BOOST_CHECK_EQUAL(sequence2.size(), 6);
  BOOST_CHECK_EQUAL(sequence2[0].getMetaData().getSampleName(), "170808_Jonathan_yeast_Sacc1_1x");
  BOOST_CHECK_EQUAL(sequence2[0].getMetaData().getSampleGroupName(), "Test01");
  BOOST_CHECK_EQUAL(sequence2[0].getMetaData().getSequenceSegmentName(), "Group01");
  BOOST_CHECK_EQUAL(sequence2[0].getMetaData().getSampleType(), MetaDataHandler::SampleType::Unknown);
  BOOST_CHECK_EQUAL(sequence2[0].getMetaData().getFilename(), "/home/user/code/test/data/mzML/170808_Jonathan_yeast_Sacc1_1x.mzML");
  BOOST_CHECK_EQUAL(sequence2[5].getMetaData().getSampleName(), "170808_Jonathan_yeast_Yarr3_1x");
  BOOST_CHECK_EQUAL(sequence2[5].getMetaData().getSampleGroupName(), "Test01");
  BOOST_CHECK_EQUAL(sequence2[5].getMetaData().getSequenceSegmentName(), "Group01");
  BOOST_CHECK_EQUAL(sequence2[5].getMetaData().getSampleType(), MetaDataHandler::SampleType::Unknown);
  BOOST_CHECK_EQUAL(sequence2[5].getMetaData().getFilename(), "/home/user/code/test/data/mzML/170808_Jonathan_yeast_Sacc1_1x.mzML");

  sequenceHandler.clear();
  pathname = SMARTPEAK_GET_TEST_DATA_PATH("SequenceParser_sequence_1_tab.csv");
  SequenceParser::readSequenceFile(sequenceHandler, pathname, "\t");
  vector<SampleHandler>& sequence3 = sequenceHandler.getSequence();
  BOOST_CHECK_EQUAL(sequence3.size(), 6);
  BOOST_CHECK_EQUAL(sequence3[0].getMetaData().getSampleName(), "170808_Jonathan_yeast_Sacc1_1x");
  BOOST_CHECK_EQUAL(sequence3[0].getMetaData().getSampleGroupName(), "Test01");
  BOOST_CHECK_EQUAL(sequence3[0].getMetaData().getSequenceSegmentName(), "Group01");
  BOOST_CHECK_EQUAL(sequence3[0].getMetaData().getSampleType(), MetaDataHandler::SampleType::Unknown);
  BOOST_CHECK_EQUAL(sequence3[0].getMetaData().getFilename(), "/home/user/code/test/data/mzML/170808_Jonathan_yeast_Sacc1_1x.mzML");
  BOOST_CHECK_EQUAL(sequence3[5].getMetaData().getSampleName(), "170808_Jonathan_yeast_Yarr3_1x");
  BOOST_CHECK_EQUAL(sequence3[5].getMetaData().getSampleGroupName(), "Test01");
  BOOST_CHECK_EQUAL(sequence3[5].getMetaData().getSequenceSegmentName(), "Group01");
  BOOST_CHECK_EQUAL(sequence3[5].getMetaData().getSampleType(), MetaDataHandler::SampleType::Unknown);
  BOOST_CHECK_EQUAL(sequence3[5].getMetaData().getFilename(), "/home/user/code/test/data/mzML/170808_Jonathan_yeast_Sacc1_1x.mzML");
}

BOOST_AUTO_TEST_CASE(makeDataTableFromMetaValue)
{
  SequenceHandler sequenceHandler;

  OpenMS::FeatureMap fm;

  const vector<string> sample_names = {
    "170808_Jonathan_yeast_Sacc1_1x",
    "170808_Jonathan_yeast_Sacc2_1x",
    "170808_Jonathan_yeast_Sacc3_1x",
    "170808_Jonathan_yeast_Yarr1_1x",
    "170808_Jonathan_yeast_Yarr2_1x",
    "170808_Jonathan_yeast_Yarr3_1x"
  };

  for (const string& sample_name : sample_names) {
    MetaDataHandler metaDataHandler;
    metaDataHandler.setSampleName(sample_name);
    metaDataHandler.setFilename(sample_name + ".mzML");
    metaDataHandler.setSampleType(MetaDataHandler::SampleType::Unknown);
    metaDataHandler.setSampleGroupName("sample_group");
    metaDataHandler.setSequenceSegmentName("sequence_segment");

    const string featureXML_i = SMARTPEAK_GET_TEST_DATA_PATH(sample_name + ".featureXML");
    RawDataHandler rawDataHandler;
    OpenMSFile::loadFeatureMap(rawDataHandler, featureXML_i);

    sequenceHandler.addSampleToSequence(metaDataHandler, rawDataHandler.getFeatureMap());
  }

  vector<SampleHandler>& sequence = sequenceHandler.getSequence();


  vector<map<string,string>> data_out;
  vector<string> headers_out;
  const set<string> meta_data = {"peak_apex_int", "logSN"};
  const set<MetaDataHandler::SampleType> sample_types = {MetaDataHandler::SampleType::Unknown};

  SequenceParser::makeDataTableFromMetaValue(sequenceHandler, data_out, headers_out, meta_data, sample_types);

  BOOST_CHECK_EQUAL(data_out.size(), 1657);
  BOOST_CHECK_EQUAL(data_out.at(0).at("sample_name"), "170808_Jonathan_yeast_Sacc1_1x");
  BOOST_CHECK_EQUAL(data_out.at(0).at("sample_type"), "Unknown");
  BOOST_CHECK_EQUAL(data_out.at(0).at("component_group_name"), "23dpg");
  BOOST_CHECK_EQUAL(data_out.at(0).at("component_name"), "23dpg.23dpg_1.Heavy");
  BOOST_CHECK_EQUAL(data_out.at(0).at("peak_apex_int"), std::to_string(235.0));
  BOOST_CHECK_EQUAL(data_out.at(0).at("logSN"), std::to_string(3.52866193485212));
  BOOST_CHECK_EQUAL(headers_out.size(), 6);
  BOOST_CHECK_EQUAL(headers_out[0], "sample_name");
  BOOST_CHECK_EQUAL(headers_out[1], "sample_type");
  BOOST_CHECK_EQUAL(headers_out[2], "component_group_name");
  BOOST_CHECK_EQUAL(headers_out[3], "component_name");
  BOOST_CHECK_EQUAL(headers_out[4], "logSN");
  BOOST_CHECK_EQUAL(headers_out[5], "peak_apex_int");
}

// BOOST_AUTO_TEST_CASE(write_dataMatrixFromMetaValue)
// {
//   const string pathname = SMARTPEAK_GET_TEST_DATA_PATH("SequenceParser_sequence_1.csv");
//   SequenceHandler sequenceHandler;
//   SequenceParser::readSequenceFile(sequenceHandler, pathname);
//   const string output_path = SMARTPEAK_GET_TEST_DATA_PATH("write_dataMatrixFromMetaValue.csv");
//   SequenceParser::write_dataMatrixFromMetaValue(sequenceHandler, output_path);
// }

BOOST_AUTO_TEST_SUITE_END()
