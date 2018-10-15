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

  const std::string pathname_output = SMARTPEAK_GET_TEST_DATA_PATH("SequenceParser_write_dataTableFromMetaValue.csv");
  SequenceParser::write_dataTableFromMetaValue(sequenceHandler, pathname_output, meta_data, sample_types);
  // TODO: not really testing the output of the write
}

BOOST_AUTO_TEST_CASE(makeDataMatrixFromMetaValue)
{
  SequenceHandler sequenceHandler;

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

  std::vector<std::vector<float>> data_out;
  std::vector<std::string> columns_out;
  std::vector<SequenceParser::Row> rows_out;

  const set<string> meta_data = {"calculated_concentration"};
  const set<MetaDataHandler::SampleType> sample_types = {MetaDataHandler::SampleType::Unknown};

  SequenceParser::makeDataMatrixFromMetaValue(sequenceHandler, data_out, columns_out, rows_out, meta_data, sample_types);

  BOOST_CHECK_EQUAL(columns_out.size(), 6);
  BOOST_CHECK_EQUAL(columns_out[0], "170808_Jonathan_yeast_Sacc1_1x");
  BOOST_CHECK_EQUAL(rows_out.size(), 28);
  BOOST_CHECK_EQUAL(rows_out[0].component_group_name, "accoa");
  BOOST_CHECK_CLOSE(data_out.front().front(), 1.28478575, 1e-3);
  BOOST_CHECK_CLOSE(data_out.back().back(), 1.57220089, 1e-3);

  const std::string pathname_output = SMARTPEAK_GET_TEST_DATA_PATH("SequenceParser_write_dataMatrixFromMetaValue.csv");
  SequenceParser::write_dataMatrixFromMetaValue(sequenceHandler, pathname_output);
  // TODO: not really testing the output of the write
}

BOOST_AUTO_TEST_SUITE_END()
