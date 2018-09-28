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

// BOOST_AUTO_TEST_CASE(makeDataMatrixFromMetaValue)
// {
//   const string pathname = SMARTPEAK_GET_TEST_DATA_PATH("SequenceParser_sequence_1.csv");
//   SequenceHandler sequenceHandler;
//   SequenceParser::readSequenceFile(sequenceHandler, pathname);

//   OpenMS::FeatureMap fm;

//   const vector<string> filenames_featureXML = {
//     "170808_Jonathan_yeast_Sacc1_1x.featureXML",
//     "170808_Jonathan_yeast_Sacc2_1x.featureXML",
//     "170808_Jonathan_yeast_Sacc3_1x.featureXML",
//     "170808_Jonathan_yeast_Yarr1_1x.featureXML",
//     "170808_Jonathan_yeast_Yarr2_1x.featureXML",
//     "170808_Jonathan_yeast_Yarr3_1x.featureXML"
//   };

//   vector<SampleHandler>& sequence = sequenceHandler.getSequence();
//   BOOST_CHECK_EQUAL(sequence.size(), filenames_featureXML.size());

//   for (size_t i = 0; i < sequence.size(); ++i) {
//     RawDataHandler rawDataHandler;
//     OpenMSFile::loadFeatureMap(rawDataHandler, SMARTPEAK_GET_TEST_DATA_PATH(filenames_featureXML[i]));
//     sequence[i].setRawData(rawDataHandler);
//   }

//   std::vector<std::vector<float>> data_out;
//   std::set<std::string> columns_out;
//   std::set<std::string> rows_out;

//   const string output_path = SMARTPEAK_GET_TEST_DATA_PATH("makeDataMatrixFromMetaValue.csv");
//   SequenceParser::makeDataMatrixFromMetaValue(sequenceHandler, data_out, columns_out, rows_out);

// }

// BOOST_AUTO_TEST_CASE(write_dataMatrixFromMetaValue)
// {
//   const string pathname = SMARTPEAK_GET_TEST_DATA_PATH("SequenceParser_sequence_1.csv");
//   SequenceHandler sequenceHandler;
//   SequenceParser::readSequenceFile(sequenceHandler, pathname);
//   const string output_path = SMARTPEAK_GET_TEST_DATA_PATH("write_dataMatrixFromMetaValue.csv");
//   SequenceParser::write_dataMatrixFromMetaValue(sequenceHandler, output_path);
// }

BOOST_AUTO_TEST_SUITE_END()
