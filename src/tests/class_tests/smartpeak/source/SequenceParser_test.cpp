// TODO: Add copyright

#include <SmartPeak/test_config.h>

#define BOOST_TEST_MODULE SequenceParser test suite
#include <boost/test/included/unit_test.hpp>
#include <SmartPeak/io/SequenceParser.h>
#include <SmartPeak/core/MetaDataHandler.h>
#include <SmartPeak/core/RawDataProcessor.h>
#include <SmartPeak/core/SampleType.h>

using namespace SmartPeak;
using namespace std;

BOOST_AUTO_TEST_SUITE(sequenceparser)

BOOST_AUTO_TEST_CASE(readSequenceFile)
{
  string pathname = SMARTPEAK_GET_TEST_DATA_PATH("SequenceParser_sequence_1.csv");
  SequenceHandler sequenceHandler;
  SequenceParser::readSequenceFile(sequenceHandler, pathname, ",");
  const vector<InjectionHandler>& sequence1 = sequenceHandler.getSequence();
  BOOST_CHECK_EQUAL(sequence1.size(), 4);
  BOOST_CHECK_EQUAL(sequence1[0].getMetaData().getSampleName(), "170808_Jonathan_yeast_Sacc2_1x");
  BOOST_CHECK_EQUAL(sequence1[0].getMetaData().getSampleGroupName(), "Test01");
  BOOST_CHECK_EQUAL(sequence1[0].getMetaData().getSequenceSegmentName(), "Group01");
  BOOST_CHECK_EQUAL(sequence1[0].getMetaData().getSampleType() == SampleType::Unknown, true);
  BOOST_CHECK_EQUAL(sequence1[0].getMetaData().getFilename(), "/home/user/code/test/data/mzML/170808_Jonathan_yeast_Sacc1_1x.mzML");
  BOOST_CHECK_EQUAL(sequence1[0].getMetaData().acq_method_name, "LCMS");
  BOOST_CHECK_CLOSE(sequence1[0].getMetaData().inj_volume, 10.0, 1e-3);
  BOOST_CHECK_EQUAL(sequence1[0].getMetaData().inj_volume_units, "uL");
  BOOST_CHECK_EQUAL(sequence1[0].getMetaData().batch_name, "FluxTest");
  BOOST_CHECK_EQUAL(sequence1[0].getMetaData().inj_number, 2);
  BOOST_CHECK_EQUAL(sequence1[0].getMetaData().scan_polarity, "positive");
  BOOST_CHECK_EQUAL(sequence1[0].getMetaData().scan_mass_low, 60);
  BOOST_CHECK_EQUAL(sequence1[0].getMetaData().scan_mass_high, 2000);
  BOOST_CHECK_EQUAL(sequence1[0].getMetaData().getAcquisitionDateAndTimeAsString(), "2015-07-07_153300");
  BOOST_CHECK_EQUAL(sequence1[3].getMetaData().getSampleName(), "170808_Jonathan_yeast_Yarr2_1x");
  BOOST_CHECK_EQUAL(sequence1[3].getMetaData().getSampleGroupName(), "Test01");
  BOOST_CHECK_EQUAL(sequence1[3].getMetaData().getSequenceSegmentName(), "Group01");
  BOOST_CHECK_EQUAL(sequence1[3].getMetaData().getSampleType() == SampleType::Unknown, true);
  BOOST_CHECK_EQUAL(sequence1[3].getMetaData().getFilename(), "/home/user/code/test/data/mzML/170808_Jonathan_yeast_Sacc1_1x.mzML");
  BOOST_CHECK_EQUAL(sequence1[3].getMetaData().acq_method_name, "LCMS");
  BOOST_CHECK_CLOSE(sequence1[3].getMetaData().inj_volume, 10.0, 1e-3);
  BOOST_CHECK_EQUAL(sequence1[3].getMetaData().inj_volume_units, "uL");
  BOOST_CHECK_EQUAL(sequence1[3].getMetaData().batch_name, "FluxTest");
  BOOST_CHECK_EQUAL(sequence1[3].getMetaData().inj_number, 5);
  BOOST_CHECK_EQUAL(sequence1[3].getMetaData().scan_polarity, "negative");
  BOOST_CHECK_EQUAL(sequence1[3].getMetaData().scan_mass_low, 60);
  BOOST_CHECK_EQUAL(sequence1[3].getMetaData().scan_mass_high, 2000);
  BOOST_CHECK_EQUAL(sequence1[3].getMetaData().getAcquisitionDateAndTimeAsString(), "2015-07-07_153300");

  sequenceHandler.clear();
  pathname = SMARTPEAK_GET_TEST_DATA_PATH("SequenceParser_sequence_1_semicolon.csv");
  SequenceParser::readSequenceFile(sequenceHandler, pathname, ";");
  const vector<InjectionHandler>& sequence2 = sequenceHandler.getSequence();
  BOOST_CHECK_EQUAL(sequence2.size(), 5);
  BOOST_CHECK_EQUAL(sequence2[0].getMetaData().getSampleName(), "170808_Jonathan_yeast_Sacc1_1x");
  BOOST_CHECK_EQUAL(sequence2[0].getMetaData().getSampleGroupName(), "Test01");
  BOOST_CHECK_EQUAL(sequence2[0].getMetaData().getSequenceSegmentName(), "Group01");
  BOOST_CHECK_EQUAL(sequence2[0].getMetaData().getSampleType() == SampleType::Unknown, true);
  BOOST_CHECK_EQUAL(sequence2[0].getMetaData().getFilename(), "/home/user/code/test/data/mzML/170808_Jonathan_yeast_Sacc1_1x.mzML");
  BOOST_CHECK_EQUAL(sequence2[0].getMetaData().acq_method_name, "LCMS");
  BOOST_CHECK_CLOSE(sequence2[0].getMetaData().inj_volume, 10.0, 1e-3);
  BOOST_CHECK_EQUAL(sequence2[0].getMetaData().inj_volume_units, "uL");
  BOOST_CHECK_EQUAL(sequence2[0].getMetaData().batch_name, "FluxTest");
  BOOST_CHECK_EQUAL(sequence2[0].getMetaData().inj_number, 1);
  BOOST_CHECK_EQUAL(sequence2[0].getMetaData().getAcquisitionDateAndTimeAsString(), "2015-07-07_153300");
  BOOST_CHECK_EQUAL(sequence2[4].getMetaData().getSampleName(), "170808_Jonathan_yeast_Yarr2_1x");
  BOOST_CHECK_EQUAL(sequence2[4].getMetaData().getSampleGroupName(), "Test01");
  BOOST_CHECK_EQUAL(sequence2[4].getMetaData().getSequenceSegmentName(), "Group01");
  BOOST_CHECK_EQUAL(sequence2[4].getMetaData().getSampleType() == SampleType::Unknown, true);
  BOOST_CHECK_EQUAL(sequence2[4].getMetaData().getFilename(), "170808_Jonathan_yeast_Yarr2_1x_-1_FluxTest_1900-01-01_000000"); // No default provided
  BOOST_CHECK_EQUAL(sequence2[4].getMetaData().acq_method_name, "LCMS");
  BOOST_CHECK_CLOSE(sequence2[4].getMetaData().inj_volume, 10.0, 1e-3);
  BOOST_CHECK_EQUAL(sequence2[4].getMetaData().inj_volume_units, "uL");
  BOOST_CHECK_EQUAL(sequence2[4].getMetaData().batch_name, "FluxTest");
  BOOST_CHECK_EQUAL(sequence2[4].getMetaData().inj_number, 5);
  BOOST_CHECK_EQUAL(sequence2[4].getMetaData().getAcquisitionDateAndTimeAsString(), "2015-07-07_153300");

  sequenceHandler.clear();
  pathname = SMARTPEAK_GET_TEST_DATA_PATH("SequenceParser_sequence_1_tab.csv");
  SequenceParser::readSequenceFile(sequenceHandler, pathname, "\t");
  const vector<InjectionHandler>& sequence3 = sequenceHandler.getSequence();
  BOOST_CHECK_EQUAL(sequence3.size(), 5);
  BOOST_CHECK_EQUAL(sequence3[0].getMetaData().getSampleName(), "170808_Jonathan_yeast_Sacc1_1x");
  BOOST_CHECK_EQUAL(sequence3[0].getMetaData().getSampleGroupName(), "Test01");
  BOOST_CHECK_EQUAL(sequence3[0].getMetaData().getSequenceSegmentName(), "Group01");
  BOOST_CHECK_EQUAL(sequence3[0].getMetaData().getSampleType() == SampleType::Unknown, true);
  BOOST_CHECK_EQUAL(sequence3[0].getMetaData().getFilename(), "/home/user/code/test/data/mzML/170808_Jonathan_yeast_Sacc1_1x.mzML");
  BOOST_CHECK_EQUAL(sequence3[0].getMetaData().acq_method_name, "LCMS");
  BOOST_CHECK_CLOSE(sequence3[0].getMetaData().inj_volume, 10.0, 1e-3);
  BOOST_CHECK_EQUAL(sequence3[0].getMetaData().inj_volume_units, "uL");
  BOOST_CHECK_EQUAL(sequence3[0].getMetaData().batch_name, "FluxTest");
  BOOST_CHECK_EQUAL(sequence3[0].getMetaData().inj_number, 1);
  BOOST_CHECK_EQUAL(sequence3[0].getMetaData().getAcquisitionDateAndTimeAsString(), "2015-07-07_153300");
  BOOST_CHECK_EQUAL(sequence3[4].getMetaData().getSampleName(), "170808_Jonathan_yeast_Yarr2_1x");
  BOOST_CHECK_EQUAL(sequence3[4].getMetaData().getSampleGroupName(), "Test01");
  BOOST_CHECK_EQUAL(sequence3[4].getMetaData().getSequenceSegmentName(), "Group01");
  BOOST_CHECK_EQUAL(sequence3[4].getMetaData().getSampleType() == SampleType::Unknown, true);
  BOOST_CHECK_EQUAL(sequence3[4].getMetaData().getFilename(), "170808_Jonathan_yeast_Yarr2_1x_-1_FluxTest_1900-01-01_000000"); // No default provided
  BOOST_CHECK_EQUAL(sequence3[4].getMetaData().acq_method_name, "LCMS");
  BOOST_CHECK_CLOSE(sequence3[4].getMetaData().inj_volume, 10.0, 1e-3);
  BOOST_CHECK_EQUAL(sequence3[4].getMetaData().inj_volume_units, "uL");
  BOOST_CHECK_EQUAL(sequence3[4].getMetaData().batch_name, "FluxTest");
  BOOST_CHECK_EQUAL(sequence3[4].getMetaData().inj_number, 5);
  BOOST_CHECK_EQUAL(sequence3[4].getMetaData().getAcquisitionDateAndTimeAsString(), "2015-07-07_153300");
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

  int inj_num = 0;
  for (const string& sample_name : sample_names) {
    ++inj_num;
    MetaDataHandler metaDataHandler;
    metaDataHandler.setSampleName(sample_name);
    metaDataHandler.setFilename(sample_name + ".mzML");
    metaDataHandler.setSampleType(SampleType::Unknown);
    metaDataHandler.setSampleGroupName("sample_group");
    metaDataHandler.setSequenceSegmentName("sequence_segment");
    metaDataHandler.inj_number = inj_num;
    metaDataHandler.acq_method_name = "6";
    metaDataHandler.inj_volume = 7.0;
    metaDataHandler.inj_volume_units = "8";
    metaDataHandler.batch_name = "FluxTest";
    metaDataHandler.scan_polarity = "negative";
    metaDataHandler.scan_mass_high = 2000;
    metaDataHandler.scan_mass_low = 60;

    Filenames filenames;
    filenames.featureXML_i = SMARTPEAK_GET_TEST_DATA_PATH(metaDataHandler.getInjectionName() + ".featureXML");
    RawDataHandler rawDataHandler;
    LoadFeatures loadFeatures;
    loadFeatures.process(rawDataHandler, {}, filenames);

    sequenceHandler.addSampleToSequence(metaDataHandler, rawDataHandler.getFeatureMap());
  }

  vector<vector<string>> data_out;
  vector<string> headers_out;
  const vector<string> meta_data {
    "peak_apex_int",
    "logSN",
    "QC_transition_message",
    "QC_transition_group_message",
    "leftWidth",
    "rightWidth",
  };
  const set<SampleType> sample_types = {SampleType::Unknown};

  SequenceParser::makeDataTableFromMetaValue(sequenceHandler, data_out, headers_out, meta_data, sample_types, std::set<std::string>(), std::set<std::string>(), std::set<std::string>());

  BOOST_CHECK_EQUAL(data_out.size(), 1657);
  BOOST_CHECK_EQUAL(data_out.at(0).at(0), "170808_Jonathan_yeast_Sacc1_1x");
  BOOST_CHECK_EQUAL(data_out.at(0).at(1), "Unknown");
  BOOST_CHECK_EQUAL(data_out.at(0).at(2), "23dpg");
  BOOST_CHECK_EQUAL(data_out.at(0).at(3), "23dpg.23dpg_1.Heavy");
  BOOST_CHECK_EQUAL(data_out.at(0).at(22), std::to_string(235.0));
  BOOST_CHECK_EQUAL(data_out.at(0).at(23), std::to_string(3.52866193485212));
  BOOST_CHECK_EQUAL(data_out.at(0).at(26), std::to_string(15.605367));
  BOOST_CHECK_EQUAL(data_out.at(0).at(27), std::to_string(15.836817));
  BOOST_CHECK_EQUAL(headers_out.size(), 28);
  BOOST_CHECK_EQUAL(headers_out[0], "sample_name");
  BOOST_CHECK_EQUAL(headers_out[1], "sample_type");
  BOOST_CHECK_EQUAL(headers_out[2], "component_group_name");
  BOOST_CHECK_EQUAL(headers_out[3], "component_name");
  BOOST_CHECK_EQUAL(headers_out[4], "batch_name");
  BOOST_CHECK_EQUAL(headers_out[5], "rack_number");
  BOOST_CHECK_EQUAL(headers_out[6], "plate_number");
  BOOST_CHECK_EQUAL(headers_out[7], "pos_number");
  BOOST_CHECK_EQUAL(headers_out[8], "inj_number");
  BOOST_CHECK_EQUAL(headers_out[9], "dilution_factor");
  BOOST_CHECK_EQUAL(headers_out[10], "inj_volume");
  BOOST_CHECK_EQUAL(headers_out[11], "inj_volume_units");
  BOOST_CHECK_EQUAL(headers_out[12], "operator_name");
  BOOST_CHECK_EQUAL(headers_out[13], "acq_method_name");
  BOOST_CHECK_EQUAL(headers_out[14], "proc_method_name");
  BOOST_CHECK_EQUAL(headers_out[15], "original_filename");
  BOOST_CHECK_EQUAL(headers_out[16], "acquisition_date_and_time");
  BOOST_CHECK_EQUAL(headers_out[17], "scan_polarity");
  BOOST_CHECK_EQUAL(headers_out[18], "scan_mass_low");
  BOOST_CHECK_EQUAL(headers_out[19], "scan_mass_high");
  BOOST_CHECK_EQUAL(headers_out[20], "injection_name");
  BOOST_CHECK_EQUAL(headers_out[21], "used_");
  // metadata
  BOOST_CHECK_EQUAL(headers_out[22], "peak_apex_int");
  BOOST_CHECK_EQUAL(headers_out[23], "logSN");
  BOOST_CHECK_EQUAL(headers_out[24], "QC_transition_message");
  BOOST_CHECK_EQUAL(headers_out[25], "QC_transition_group_message");
  BOOST_CHECK_EQUAL(headers_out[26], "leftWidth");
  BOOST_CHECK_EQUAL(headers_out[27], "rightWidth");

  SequenceParser::makeDataTableFromMetaValue(sequenceHandler, data_out, headers_out, std::vector<std::string>({ "leftWidth" }), sample_types,
    std::set<std::string>({ "170808_Jonathan_yeast_Sacc1_1x" }), std::set<std::string>({ "23dpg" }), std::set<std::string>({ "23dpg.23dpg_1.Light" }));

  BOOST_CHECK_EQUAL(data_out.size(), 1);
  BOOST_CHECK_EQUAL(data_out.at(0).at(0), "170808_Jonathan_yeast_Sacc1_1x");
  BOOST_CHECK_EQUAL(data_out.at(0).at(1), "Unknown");
  BOOST_CHECK_EQUAL(data_out.at(0).at(2), "23dpg");
  BOOST_CHECK_EQUAL(data_out.at(0).at(3), "23dpg.23dpg_1.Light");
  BOOST_CHECK_EQUAL(data_out.at(0).at(22), std::to_string(15.605367));
  BOOST_CHECK_EQUAL(headers_out.size(), 23);
  BOOST_CHECK_EQUAL(headers_out[0], "sample_name");
  BOOST_CHECK_EQUAL(headers_out[1], "sample_type");
  BOOST_CHECK_EQUAL(headers_out[2], "component_group_name");
  BOOST_CHECK_EQUAL(headers_out[3], "component_name");
  BOOST_CHECK_EQUAL(headers_out[4], "batch_name");
  BOOST_CHECK_EQUAL(headers_out[5], "rack_number");
  BOOST_CHECK_EQUAL(headers_out[6], "plate_number");
  BOOST_CHECK_EQUAL(headers_out[7], "pos_number");
  BOOST_CHECK_EQUAL(headers_out[8], "inj_number");
  BOOST_CHECK_EQUAL(headers_out[9], "dilution_factor");
  BOOST_CHECK_EQUAL(headers_out[10], "inj_volume");
  BOOST_CHECK_EQUAL(headers_out[11], "inj_volume_units");
  BOOST_CHECK_EQUAL(headers_out[12], "operator_name");
  BOOST_CHECK_EQUAL(headers_out[13], "acq_method_name");
  BOOST_CHECK_EQUAL(headers_out[14], "proc_method_name");
  BOOST_CHECK_EQUAL(headers_out[15], "original_filename");
  BOOST_CHECK_EQUAL(headers_out[16], "acquisition_date_and_time");
  BOOST_CHECK_EQUAL(headers_out[17], "scan_polarity");
  BOOST_CHECK_EQUAL(headers_out[18], "scan_mass_low");
  BOOST_CHECK_EQUAL(headers_out[19], "scan_mass_high");
  BOOST_CHECK_EQUAL(headers_out[20], "injection_name");
  BOOST_CHECK_EQUAL(headers_out[21], "used_");
  // metadata
  BOOST_CHECK_EQUAL(headers_out[22], "leftWidth");

  // write sequence to output
  // const std::string pathname_output = SMARTPEAK_GET_TEST_DATA_PATH("output/SequenceParser_writeDataTableFromMetaValue.csv");
  // SequenceParser::writeDataTableFromMetaValue(sequenceHandler, pathname_output, meta_data, sample_types);
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

  int inj_num = 0;
  for (const string& sample_name : sample_names) {
    ++inj_num;
    MetaDataHandler metaDataHandler;
    metaDataHandler.setSampleName(sample_name);
    metaDataHandler.setFilename(sample_name + ".mzML");
    metaDataHandler.setSampleType(SampleType::Unknown);
    metaDataHandler.setSampleGroupName("sample_group");
    metaDataHandler.setSequenceSegmentName("sequence_segment");
    metaDataHandler.acq_method_name = "6";
    metaDataHandler.inj_number = inj_num;
    metaDataHandler.inj_volume = 7.0;
    metaDataHandler.inj_volume_units = "8";
    metaDataHandler.batch_name = "FluxTest";
    metaDataHandler.scan_polarity = "negative";
    metaDataHandler.scan_mass_high = 2000;
    metaDataHandler.scan_mass_low = 60;

    Filenames filenames;
    filenames.featureXML_i = SMARTPEAK_GET_TEST_DATA_PATH(metaDataHandler.getInjectionName() + ".featureXML");
    RawDataHandler rawDataHandler;
    LoadFeatures loadFeatures;
    loadFeatures.process(rawDataHandler, {}, filenames);

    sequenceHandler.addSampleToSequence(metaDataHandler, rawDataHandler.getFeatureMap());
  }

  Eigen::Tensor<float, 2> data_out;
  Eigen::Tensor<std::string, 1> columns_out;
  Eigen::Tensor<std::string, 2> rows_out;

  const vector<string> meta_data = {
    "calculated_concentration",
    "leftWidth",
    "rightWidth"
  };
  // const vector<string> meta_data = {"calculated_concentration"};
  const set<SampleType> sample_types = {SampleType::Unknown};

  SequenceParser::makeDataMatrixFromMetaValue(sequenceHandler, data_out, columns_out, rows_out, meta_data, sample_types, std::set<std::string>(), std::set<std::string>(), std::set<std::string>());

  BOOST_CHECK_EQUAL(columns_out.size(), 6);
  BOOST_CHECK_EQUAL(columns_out(0), "170808_Jonathan_yeast_Sacc1_1x");
  BOOST_CHECK_EQUAL(rows_out.dimension(0), 636);
  BOOST_CHECK_EQUAL(rows_out.dimension(1), 3);
  BOOST_CHECK_EQUAL(rows_out(0,1), "23dpg");
  BOOST_CHECK_CLOSE(data_out(0,0), 15.6053667, 1e-3);
  BOOST_CHECK_CLOSE(data_out(rows_out.dimension(0)-1,columns_out.size()-1), 1.66744995, 1e-3);

  SequenceParser::makeDataMatrixFromMetaValue(sequenceHandler, data_out, columns_out, rows_out, std::vector<std::string>({ "leftWidth" }), sample_types,
    std::set<std::string>({ "170808_Jonathan_yeast_Sacc1_1x" }), std::set<std::string>({ "23dpg" }), std::set<std::string>({ "23dpg.23dpg_1.Light" }));

  BOOST_CHECK_EQUAL(columns_out.size(), 1);
  BOOST_CHECK_EQUAL(columns_out(0), "170808_Jonathan_yeast_Sacc1_1x");
  BOOST_CHECK_EQUAL(rows_out.dimension(0), 1);
  BOOST_CHECK_EQUAL(rows_out.dimension(1), 3);
  BOOST_CHECK_EQUAL(rows_out(0, 1), "23dpg");
  BOOST_CHECK_CLOSE(data_out(0, 0), 15.6053667, 1e-3);

  // write sequence to output
  // const std::string pathname_output = SMARTPEAK_GET_TEST_DATA_PATH("output/SequenceParser_writeDataMatrixFromMetaValue.csv");
  // SequenceParser::writeDataMatrixFromMetaValue(sequenceHandler, pathname_output);
}

BOOST_AUTO_TEST_CASE(makeSequenceFileAnalyst)
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

  int inj_num = 0;
  for (const string& sample_name : sample_names) {
    ++inj_num;
    MetaDataHandler metaDataHandler;
    metaDataHandler.setSampleName(sample_name);
    metaDataHandler.setSampleType(SampleType::Unknown);
    metaDataHandler.setSampleGroupName("sample_group");
    metaDataHandler.setSequenceSegmentName("sequence_segment");
    metaDataHandler.plate_number = 3;
    metaDataHandler.rack_number = 4;
    metaDataHandler.pos_number = inj_num;
    metaDataHandler.dilution_factor = 8.0;
    metaDataHandler.setAcquisitionDateAndTimeFromString("01-01-2020 17:14:00", "%m-%d-%Y %H:%M:%S");
    metaDataHandler.inj_number = inj_num;
    metaDataHandler.acq_method_name = "RapidRIP";
    metaDataHandler.inj_volume = 7.0;
    metaDataHandler.inj_volume_units = "8";
    metaDataHandler.batch_name = "FluxTest";
    metaDataHandler.scan_polarity = "negative";
    metaDataHandler.scan_mass_high = 2000;
    metaDataHandler.scan_mass_low = 60;
    metaDataHandler.setFilename(metaDataHandler.getInjectionName());

    sequenceHandler.addSampleToSequence(metaDataHandler, OpenMS::FeatureMap());
  }

  vector<vector<string>> data_out;
  vector<string> headers_out;

  SequenceParser::makeSequenceFileAnalyst(sequenceHandler, data_out, headers_out);

  BOOST_CHECK_EQUAL(data_out.size(), 6);
  BOOST_CHECK_EQUAL(data_out.at(0).at(0), "170808_Jonathan_yeast_Sacc1_1x");
  BOOST_CHECK_EQUAL(data_out.at(0).at(1), "sample_group");
  BOOST_CHECK_EQUAL(data_out.at(0).at(2), "");
  BOOST_CHECK_EQUAL(data_out.at(0).at(3), "RapidRIP.dam");
  BOOST_CHECK_EQUAL(data_out.at(0).at(4), "none");
  BOOST_CHECK_EQUAL(data_out.at(0).at(5), "CStk1-4");
  BOOST_CHECK_EQUAL(data_out.at(0).at(6), "MT96_or_VT54");
  BOOST_CHECK_EQUAL(data_out.at(0).at(7), "1");
  BOOST_CHECK_EQUAL(data_out.at(0).at(8), "7.000000");
  BOOST_CHECK_EQUAL(data_out.at(0).at(9), "8.000000");
  BOOST_CHECK_EQUAL(data_out.at(0).at(10), "0");
  BOOST_CHECK_EQUAL(data_out.at(0).at(11), "Unknown");
  BOOST_CHECK_EQUAL(data_out.at(0).at(12), "4");
  BOOST_CHECK_EQUAL(data_out.at(0).at(13), "3");
  BOOST_CHECK_EQUAL(data_out.at(0).at(14), "2020-01-01_171400");
  BOOST_CHECK_EQUAL(data_out.at(0).at(15), "2020-01-01_171400\\170808_Jonathan_yeast_Sacc1_1x_1_FluxTest_2020-01-01_171400");
  BOOST_CHECK_EQUAL(headers_out.size(), 16);
  BOOST_CHECK_EQUAL(headers_out.at(0), "% header=SampleName");
  BOOST_CHECK_EQUAL(headers_out.at(1), "SampleID");
  BOOST_CHECK_EQUAL(headers_out.at(2), "Comments");
  BOOST_CHECK_EQUAL(headers_out.at(3), "AcqMethod");
  BOOST_CHECK_EQUAL(headers_out.at(4), "ProcMethod");
  BOOST_CHECK_EQUAL(headers_out.at(5), "RackCode");
  BOOST_CHECK_EQUAL(headers_out.at(6), "PlateCode");
  BOOST_CHECK_EQUAL(headers_out.at(7), "VialPos");
  BOOST_CHECK_EQUAL(headers_out.at(8), "SmplInjVol");
  BOOST_CHECK_EQUAL(headers_out.at(9), "DilutFact");
  BOOST_CHECK_EQUAL(headers_out.at(10), "WghtToVol");
  BOOST_CHECK_EQUAL(headers_out.at(11), "Type");
  BOOST_CHECK_EQUAL(headers_out.at(12), "RackPos");
  BOOST_CHECK_EQUAL(headers_out.at(13), "PlatePos");
  BOOST_CHECK_EQUAL(headers_out.at(14), "SetName");
  BOOST_CHECK_EQUAL(headers_out.at(15), "OutputFile");
}

BOOST_AUTO_TEST_CASE(makeSequenceFileMasshunter)
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

  int inj_num = 0;
  for (const string& sample_name : sample_names) {
    ++inj_num;
    MetaDataHandler metaDataHandler;
    metaDataHandler.setSampleName(sample_name);
    metaDataHandler.setSampleType(SampleType::Unknown);
    metaDataHandler.setSampleGroupName("sample_group");
    metaDataHandler.setSequenceSegmentName("sequence_segment");
    metaDataHandler.plate_number = 3;
    metaDataHandler.rack_number = 4;
    metaDataHandler.pos_number = inj_num;
    metaDataHandler.dilution_factor = 8.0;
    metaDataHandler.setAcquisitionDateAndTimeFromString("01-01-2020 17:14:00", "%m-%d-%Y %H:%M:%S");
    metaDataHandler.inj_number = inj_num;
    metaDataHandler.acq_method_name = "RapidRIP";
    metaDataHandler.inj_volume = 7.0;
    metaDataHandler.inj_volume_units = "8";
    metaDataHandler.batch_name = "FluxTest";
    metaDataHandler.scan_polarity = "negative";
    metaDataHandler.scan_mass_high = 2000;
    metaDataHandler.scan_mass_low = 60;
    metaDataHandler.setFilename(metaDataHandler.getInjectionName());

    sequenceHandler.addSampleToSequence(metaDataHandler, OpenMS::FeatureMap());
  }

  vector<vector<string>> data_out;
  vector<string> headers_out;

  SequenceParser::makeSequenceFileMasshunter(sequenceHandler, data_out, headers_out);

  BOOST_CHECK_EQUAL(data_out.size(), 6);
  BOOST_CHECK_EQUAL(data_out.at(0).at(0), "170808_Jonathan_yeast_Sacc1_1x");
  BOOST_CHECK_EQUAL(data_out.at(0).at(1), "1");
  BOOST_CHECK_EQUAL(data_out.at(0).at(2), "D:\\DATA\\TODO");
  BOOST_CHECK_EQUAL(data_out.at(0).at(3), "RapidRIP.M");
  BOOST_CHECK_EQUAL(data_out.at(0).at(4), "D:\\DATA\\TODO");
  BOOST_CHECK_EQUAL(data_out.at(0).at(5), "2020-01-01_171400\\170808_Jonathan_yeast_Sacc1_1x_1_FluxTest_2020-01-01_171400");
  BOOST_CHECK_EQUAL(data_out.at(0).at(6), "Unknown");
  BOOST_CHECK_EQUAL(data_out.at(0).at(7), "");
  BOOST_CHECK_EQUAL(data_out.at(0).at(8), "8.000000");
  BOOST_CHECK_EQUAL(data_out.at(0).at(9), "7.000000");
  BOOST_CHECK_EQUAL(data_out.at(0).at(10), "Rack 4");
  BOOST_CHECK_EQUAL(data_out.at(0).at(11), "");
  BOOST_CHECK_EQUAL(headers_out.size(), 12);
  BOOST_CHECK_EQUAL(headers_out.at(0), "Name");
  BOOST_CHECK_EQUAL(headers_out.at(1), "Vial");
  BOOST_CHECK_EQUAL(headers_out.at(2), "Method Path");
  BOOST_CHECK_EQUAL(headers_out.at(3), "Mathod File");
  BOOST_CHECK_EQUAL(headers_out.at(4), "Data Path");
  BOOST_CHECK_EQUAL(headers_out.at(5), "Data File");
  BOOST_CHECK_EQUAL(headers_out.at(6), "Type");
  BOOST_CHECK_EQUAL(headers_out.at(7), "Level");
  BOOST_CHECK_EQUAL(headers_out.at(8), "Dil.");
  BOOST_CHECK_EQUAL(headers_out.at(9), "Vol.");
  BOOST_CHECK_EQUAL(headers_out.at(10), "Tray Name");
  BOOST_CHECK_EQUAL(headers_out.at(11), "Comment");
}

BOOST_AUTO_TEST_SUITE_END()
