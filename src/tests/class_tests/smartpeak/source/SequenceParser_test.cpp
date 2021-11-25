// --------------------------------------------------------------------------
//   SmartPeak -- Fast and Accurate CE-, GC- and LC-MS(/MS) Data Processing
// --------------------------------------------------------------------------
// Copyright The SmartPeak Team -- Novo Nordisk Foundation 
// Center for Biosustainability, Technical University of Denmark 2018-2021.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL ANY OF THE AUTHORS OR THE CONTRIBUTING
// INSTITUTIONS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
// OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
// WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
// OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
// ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// --------------------------------------------------------------------------
// $Maintainer: Douglas McCloskey, Ahmed Khalil $
// $Authors: Douglas McCloskey $
// --------------------------------------------------------------------------

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <SmartPeak/test_config.h>
#include <SmartPeak/io/SequenceParser.h>
#include <SmartPeak/core/MetaDataHandler.h>
#include <SmartPeak/core/SampleType.h>
#include <SmartPeak/core/ApplicationHandler.h>
#include <SmartPeak/core/RawDataProcessors/LoadFeatures.h>

using namespace SmartPeak;
using namespace std;

struct SequenceParserFixture : public ::testing::Test
{
  /* ctor/dtor */
  SequenceParserFixture()
  {
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
      metaDataHandler.setReplicateGroupName("replicate_group_name");
      metaDataHandler.inj_number = inj_num;
      metaDataHandler.acq_method_name = "6";
      metaDataHandler.inj_volume = 7.0;
      metaDataHandler.inj_volume_units = "8";
      metaDataHandler.batch_name = "FluxTest";
      metaDataHandler.scan_polarity = "negative";
      metaDataHandler.scan_mass_high = 2000;
      metaDataHandler.scan_mass_low = 60;

      Filenames filenames;
      filenames.setFullPath("featureXML_i", SMARTPEAK_GET_TEST_DATA_PATH(metaDataHandler.getInjectionName() + ".featureXML"));
      RawDataHandler rawDataHandler;
      LoadFeatures loadFeatures;
      loadFeatures.process(rawDataHandler, {}, filenames);

      sequence_handler_.addSampleToSequence(metaDataHandler, rawDataHandler.getFeatureMap());
    }
  }

  ~SequenceParserFixture() {}

public:
  SequenceHandler sequence_handler_;
};

TEST(Sequenceparser, readSequenceFile)
{
  string pathname = SMARTPEAK_GET_TEST_DATA_PATH("SequenceParser_sequence_1.csv");
  SequenceHandler sequenceHandler;
  SequenceParser::readSequenceFile(sequenceHandler, pathname, ",");
  const vector<InjectionHandler>& sequence1 = sequenceHandler.getSequence();
  EXPECT_EQ(sequence1.size(), 4);
  EXPECT_STREQ(sequence1[0].getMetaData().getSampleName().c_str(), "170808_Jonathan_yeast_Sacc2_1x");
  EXPECT_STREQ(sequence1[0].getMetaData().getSampleGroupName().c_str(), "Test01");
  EXPECT_STREQ(sequence1[0].getMetaData().getSequenceSegmentName().c_str(), "Group01");
  EXPECT_TRUE(sequence1[0].getMetaData().getSampleType() == SampleType::Unknown);
  EXPECT_STREQ(sequence1[0].getMetaData().getFilename().c_str(), "/home/user/code/test/data/mzML/170808_Jonathan_yeast_Sacc1_1x.mzML");
  EXPECT_STREQ(sequence1[0].getMetaData().acq_method_name.c_str(), "LCMS");
  EXPECT_NEAR(sequence1[0].getMetaData().inj_volume, 10.0, 1e-3);
  EXPECT_STREQ(sequence1[0].getMetaData().inj_volume_units.c_str(), "uL");
  EXPECT_STREQ(sequence1[0].getMetaData().batch_name.c_str(), "FluxTest");
  EXPECT_EQ(sequence1[0].getMetaData().inj_number, 2);
  EXPECT_STREQ(sequence1[0].getMetaData().scan_polarity.c_str(), "positive");
  EXPECT_EQ(sequence1[0].getMetaData().scan_mass_low, 60);
  EXPECT_EQ(sequence1[0].getMetaData().scan_mass_high, 2000);
  EXPECT_STREQ(sequence1[0].getMetaData().getAcquisitionDateAndTimeAsString().c_str(), "2015-07-07_153300");
  EXPECT_STREQ(sequence1[3].getMetaData().getSampleName().c_str(), "170808_Jonathan_yeast_Yarr2_1x");
  EXPECT_STREQ(sequence1[3].getMetaData().getSampleGroupName().c_str(), "Test01");
  EXPECT_STREQ(sequence1[3].getMetaData().getSequenceSegmentName().c_str(), "Group01");
  EXPECT_TRUE(sequence1[3].getMetaData().getSampleType() == SampleType::Unknown);
  EXPECT_STREQ(sequence1[3].getMetaData().getFilename().c_str(), "/home/user/code/test/data/mzML/170808_Jonathan_yeast_Sacc1_1x.mzML");
  EXPECT_STREQ(sequence1[3].getMetaData().acq_method_name.c_str(), "LCMS");
  EXPECT_NEAR(sequence1[3].getMetaData().inj_volume, 10.0, 1e-3);
  EXPECT_STREQ(sequence1[3].getMetaData().inj_volume_units.c_str(), "uL");
  EXPECT_STREQ(sequence1[3].getMetaData().batch_name.c_str(), "FluxTest");
  EXPECT_EQ(sequence1[3].getMetaData().inj_number, 5);
  EXPECT_STREQ(sequence1[3].getMetaData().scan_polarity.c_str(), "negative");
  EXPECT_EQ(sequence1[3].getMetaData().scan_mass_low, 60);
  EXPECT_EQ(sequence1[3].getMetaData().scan_mass_high, 2000);
  EXPECT_STREQ(sequence1[3].getMetaData().getAcquisitionDateAndTimeAsString().c_str(), "2015-07-07_153300");

  sequenceHandler.clear();
  pathname = SMARTPEAK_GET_TEST_DATA_PATH("SequenceParser_sequence_1_semicolon.csv");
  SequenceParser::readSequenceFile(sequenceHandler, pathname, ";");
  const vector<InjectionHandler>& sequence2 = sequenceHandler.getSequence();
  EXPECT_EQ(sequence2.size(), 5);
  EXPECT_STREQ(sequence2[0].getMetaData().getSampleName().c_str(), "170808_Jonathan_yeast_Sacc1_1x");
  EXPECT_STREQ(sequence2[0].getMetaData().getSampleGroupName().c_str(), "Test01");
  EXPECT_STREQ(sequence2[0].getMetaData().getSequenceSegmentName().c_str(), "Group01");
  EXPECT_TRUE(sequence2[0].getMetaData().getSampleType() == SampleType::Unknown);
  EXPECT_STREQ(sequence2[0].getMetaData().getFilename().c_str(), "/home/user/code/test/data/mzML/170808_Jonathan_yeast_Sacc1_1x.mzML");
  EXPECT_STREQ(sequence2[0].getMetaData().acq_method_name.c_str(), "LCMS");
  EXPECT_NEAR(sequence2[0].getMetaData().inj_volume, 10.0, 1e-3);
  EXPECT_STREQ(sequence2[0].getMetaData().inj_volume_units.c_str(), "uL");
  EXPECT_STREQ(sequence2[0].getMetaData().batch_name.c_str(), "FluxTest");
  EXPECT_EQ(sequence2[0].getMetaData().inj_number, 1);
  EXPECT_STREQ(sequence2[0].getMetaData().getAcquisitionDateAndTimeAsString().c_str(), "2015-07-07_153300");
  EXPECT_STREQ(sequence2[4].getMetaData().getSampleName().c_str(), "170808_Jonathan_yeast_Yarr2_1x");
  EXPECT_STREQ(sequence2[4].getMetaData().getSampleGroupName().c_str(), "Test01");
  EXPECT_STREQ(sequence2[4].getMetaData().getSequenceSegmentName().c_str(), "Group01");
  EXPECT_TRUE(sequence2[4].getMetaData().getSampleType() == SampleType::Unknown);
  EXPECT_STREQ(sequence2[4].getMetaData().getFilename().c_str(), "170808_Jonathan_yeast_Yarr2_1x_5_FluxTest_2015-07-07_153300"); // No default provided
  EXPECT_STREQ(sequence2[4].getMetaData().acq_method_name.c_str(), "LCMS");
  EXPECT_NEAR(sequence2[4].getMetaData().inj_volume, 10.0, 1e-3);
  EXPECT_STREQ(sequence2[4].getMetaData().inj_volume_units.c_str(), "uL");
  EXPECT_STREQ(sequence2[4].getMetaData().batch_name.c_str(), "FluxTest");
  EXPECT_EQ(sequence2[4].getMetaData().inj_number, 5);
  EXPECT_STREQ(sequence2[4].getMetaData().getAcquisitionDateAndTimeAsString().c_str(), "2015-07-07_153300");

  sequenceHandler.clear();
  pathname = SMARTPEAK_GET_TEST_DATA_PATH("SequenceParser_sequence_1_tab.csv");
  SequenceParser::readSequenceFile(sequenceHandler, pathname, "\t");
  const vector<InjectionHandler>& sequence3 = sequenceHandler.getSequence();
  
  EXPECT_EQ(sequence3.size(), 5);
  EXPECT_STREQ(sequence3[0].getMetaData().getSampleName().c_str(), "170808_Jonathan_yeast_Sacc1_1x");
  EXPECT_STREQ(sequence3[0].getMetaData().getSampleGroupName().c_str(), "Test01");
  EXPECT_STREQ(sequence3[0].getMetaData().getSequenceSegmentName().c_str(), "Group01");
  EXPECT_TRUE(sequence3[0].getMetaData().getSampleType() == SampleType::Unknown);
  EXPECT_STREQ(sequence3[0].getMetaData().getFilename().c_str(), "/home/user/code/test/data/mzML/170808_Jonathan_yeast_Sacc1_1x.mzML");
  EXPECT_STREQ(sequence3[0].getMetaData().acq_method_name.c_str(), "LCMS");
  EXPECT_NEAR(sequence3[0].getMetaData().inj_volume, 10.0, 1e-3);
  EXPECT_STREQ(sequence3[0].getMetaData().inj_volume_units.c_str(), "uL");
  EXPECT_STREQ(sequence3[0].getMetaData().batch_name.c_str(), "FluxTest");
  EXPECT_EQ(sequence3[0].getMetaData().inj_number, 1);
  EXPECT_STREQ(sequence3[0].getMetaData().getAcquisitionDateAndTimeAsString().c_str(), "2015-07-07_153300");
  EXPECT_STREQ(sequence3[4].getMetaData().getSampleName().c_str(), "170808_Jonathan_yeast_Yarr2_1x");
  EXPECT_STREQ(sequence3[4].getMetaData().getSampleGroupName().c_str(), "Test01");
  EXPECT_STREQ(sequence3[4].getMetaData().getSequenceSegmentName().c_str(), "Group01");
  EXPECT_TRUE(sequence3[4].getMetaData().getSampleType() == SampleType::Unknown);
  EXPECT_STREQ(sequence3[4].getMetaData().getFilename().c_str(), "170808_Jonathan_yeast_Yarr2_1x_5_FluxTest_2015-07-07_153300"); // No default provided
  EXPECT_STREQ(sequence3[4].getMetaData().acq_method_name.c_str(), "LCMS");
  EXPECT_NEAR(sequence3[4].getMetaData().inj_volume, 10.0, 1e-3);
  EXPECT_STREQ(sequence3[4].getMetaData().inj_volume_units.c_str(), "uL");
  EXPECT_STREQ(sequence3[4].getMetaData().batch_name.c_str(), "FluxTest");
  EXPECT_EQ(sequence3[4].getMetaData().inj_number, 5);
  EXPECT_STREQ(sequence3[4].getMetaData().getAcquisitionDateAndTimeAsString().c_str(), "2015-07-07_153300");
  sequenceHandler.clear();
  pathname = SMARTPEAK_GET_TEST_DATA_PATH("SequenceParser_sequence_with_NA.csv");
  SequenceParser::readSequenceFile(sequenceHandler, pathname, ",");
  const vector<InjectionHandler>& sequence4 = sequenceHandler.getSequence();
  EXPECT_EQ(sequence4.size(), 4);
  EXPECT_STREQ(sequence4[0].getMetaData().getSampleName().c_str(), "170808_Jonathan_yeast_Sacc2_1x");
  EXPECT_STREQ(sequence4[0].getMetaData().getSampleGroupName().c_str(), "Test01");
  EXPECT_STREQ(sequence4[0].getMetaData().getSequenceSegmentName().c_str(), "Group01");
  EXPECT_TRUE(sequence4[0].getMetaData().getSampleType() == SampleType::Unknown);
  EXPECT_STREQ(sequence4[0].getMetaData().getFilename().c_str(), "/home/user/code/test/data/mzML/170808_Jonathan_yeast_Sacc1_1x.mzML");
  EXPECT_STREQ(sequence4[0].getMetaData().acq_method_name.c_str(), "LCMS");
  EXPECT_NEAR(sequence4[0].getMetaData().inj_volume, 10.0, 1e-3);
  EXPECT_STREQ(sequence4[0].getMetaData().inj_volume_units.c_str(), "uL");
  EXPECT_STREQ(sequence4[0].getMetaData().batch_name.c_str(), "FluxTest");
  EXPECT_EQ(sequence4[0].getMetaData().inj_number, 2);
  EXPECT_STREQ(sequence4[0].getMetaData().scan_polarity.c_str(), "positive");
  EXPECT_EQ(sequence4[0].getMetaData().scan_mass_low, 60);
  EXPECT_EQ(sequence4[0].getMetaData().scan_mass_high, 2000);
  EXPECT_STREQ(sequence4[0].getMetaData().getAcquisitionDateAndTimeAsString().c_str(), "2015-07-07_153300");
  EXPECT_STREQ(sequence4[3].getMetaData().getSampleName().c_str(), "170808_Jonathan_yeast_Yarr2_1x");
  EXPECT_STREQ(sequence4[3].getMetaData().getSampleGroupName().c_str(), "Test01");
  EXPECT_STREQ(sequence4[3].getMetaData().getSequenceSegmentName().c_str(), "Group01");
  EXPECT_TRUE(sequence4[3].getMetaData().getSampleType() == SampleType::Unknown);
  EXPECT_STREQ(sequence4[3].getMetaData().getFilename().c_str(), "/home/user/code/test/data/mzML/170808_Jonathan_yeast_Sacc1_1x.mzML");
  EXPECT_STREQ(sequence4[3].getMetaData().acq_method_name.c_str(), "LCMS");
  EXPECT_NEAR(sequence4[3].getMetaData().inj_volume, 10.0, 1e-3);
  EXPECT_STREQ(sequence4[3].getMetaData().inj_volume_units.c_str(), "uL");
  EXPECT_STREQ(sequence4[3].getMetaData().batch_name.c_str(), "FluxTest");
  EXPECT_EQ(sequence4[3].getMetaData().inj_number, 5);
  EXPECT_STREQ(sequence4[3].getMetaData().scan_polarity.c_str(), "negative");
  EXPECT_EQ(sequence4[3].getMetaData().scan_mass_low, 60);
  EXPECT_EQ(sequence4[3].getMetaData().scan_mass_high, 2000);
  EXPECT_STREQ(sequence4[3].getMetaData().getAcquisitionDateAndTimeAsString().c_str(), "2015-07-07_153300");
}

TEST_F(SequenceParserFixture, makeDataTableFromMetaValue)
{
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

  SequenceParser::makeDataTableFromMetaValue(sequence_handler_, data_out, headers_out, meta_data, sample_types, std::set<std::string>(), std::set<std::string>(), std::set<std::string>());

  EXPECT_EQ(data_out.size(), 1657);
  EXPECT_STREQ(data_out.at(0).at(0).c_str(), "170808_Jonathan_yeast_Sacc1_1x");
  EXPECT_STREQ(data_out.at(0).at(1).c_str(), "Unknown");
  EXPECT_STREQ(data_out.at(0).at(2).c_str(), "23dpg");
  EXPECT_STREQ(data_out.at(0).at(4).c_str(), "23dpg.23dpg_1.Heavy");
  EXPECT_STREQ(data_out.at(0).at(23).c_str(), std::to_string(235.0).c_str());
  EXPECT_STREQ(data_out.at(0).at(24).c_str(), std::to_string(3.52866193485212).c_str());
  EXPECT_STREQ(data_out.at(0).at(27).c_str(), std::to_string(15.605367).c_str());
  EXPECT_STREQ(data_out.at(0).at(28).c_str(), std::to_string(15.836817).c_str());
  EXPECT_EQ(headers_out.size(), 29);
  EXPECT_STREQ(headers_out[0].c_str(), "sample_name");
  EXPECT_STREQ(headers_out[1].c_str(), "sample_type");
  EXPECT_STREQ(headers_out[2].c_str(), "component_group_name");
  EXPECT_STREQ(headers_out[3].c_str(), "replicate_group_name");
  EXPECT_STREQ(headers_out[4].c_str(), "component_name");
  EXPECT_STREQ(headers_out[5].c_str(), "batch_name");
  EXPECT_STREQ(headers_out[6].c_str(), "rack_number");
  EXPECT_STREQ(headers_out[7].c_str(), "plate_number");
  EXPECT_STREQ(headers_out[8].c_str(), "pos_number");
  EXPECT_STREQ(headers_out[9].c_str(), "inj_number");
  EXPECT_STREQ(headers_out[10].c_str(), "dilution_factor");
  EXPECT_STREQ(headers_out[11].c_str(), "inj_volume");
  EXPECT_STREQ(headers_out[12].c_str(), "inj_volume_units");
  EXPECT_STREQ(headers_out[13].c_str(), "operator_name");
  EXPECT_STREQ(headers_out[14].c_str(), "acq_method_name");
  EXPECT_STREQ(headers_out[15].c_str(), "proc_method_name");
  EXPECT_STREQ(headers_out[16].c_str(), "original_filename");
  EXPECT_STREQ(headers_out[17].c_str(), "acquisition_date_and_time");
  EXPECT_STREQ(headers_out[18].c_str(), "scan_polarity");
  EXPECT_STREQ(headers_out[19].c_str(), "scan_mass_low");
  EXPECT_STREQ(headers_out[20].c_str(), "scan_mass_high");
  EXPECT_STREQ(headers_out[21].c_str(), "injection_name");
  EXPECT_STREQ(headers_out[22].c_str(), "used_");
  // metadata
  EXPECT_STREQ(headers_out[23].c_str(), "peak_apex_int");
  EXPECT_STREQ(headers_out[24].c_str(), "logSN");
  EXPECT_STREQ(headers_out[25].c_str(), "QC_transition_message");
  EXPECT_STREQ(headers_out[26].c_str(), "QC_transition_group_message");
  EXPECT_STREQ(headers_out[27].c_str(), "leftWidth");
  EXPECT_STREQ(headers_out[28].c_str(), "rightWidth");

  SequenceParser::makeDataTableFromMetaValue(sequence_handler_, data_out, headers_out, std::vector<std::string>({ "leftWidth" }), sample_types,
    std::set<std::string>({ "170808_Jonathan_yeast_Sacc1_1x" }), std::set<std::string>({ "23dpg" }), std::set<std::string>({ "23dpg.23dpg_1.Light" }));

  EXPECT_EQ(data_out.size(), 1);
  EXPECT_STREQ(data_out.at(0).at(0).c_str(), "170808_Jonathan_yeast_Sacc1_1x");
  EXPECT_STREQ(data_out.at(0).at(1).c_str(), "Unknown");
  EXPECT_STREQ(data_out.at(0).at(2).c_str(), "23dpg");
  EXPECT_STREQ(data_out.at(0).at(4).c_str(), "23dpg.23dpg_1.Light");
  EXPECT_STREQ(data_out.at(0).at(23).c_str(), std::to_string(15.605367).c_str());
  EXPECT_EQ(headers_out.size(), 24);
  EXPECT_STREQ(headers_out[0].c_str(), "sample_name");
  EXPECT_STREQ(headers_out[1].c_str(), "sample_type");
  EXPECT_STREQ(headers_out[2].c_str(), "component_group_name");
  EXPECT_STREQ(headers_out[3].c_str(), "replicate_group_name");
  EXPECT_STREQ(headers_out[4].c_str(), "component_name");
  EXPECT_STREQ(headers_out[5].c_str(), "batch_name");
  EXPECT_STREQ(headers_out[6].c_str(), "rack_number");
  EXPECT_STREQ(headers_out[7].c_str(), "plate_number");
  EXPECT_STREQ(headers_out[8].c_str(), "pos_number");
  EXPECT_STREQ(headers_out[9].c_str(), "inj_number");
  EXPECT_STREQ(headers_out[10].c_str(), "dilution_factor");
  EXPECT_STREQ(headers_out[11].c_str(), "inj_volume");
  EXPECT_STREQ(headers_out[12].c_str(), "inj_volume_units");
  EXPECT_STREQ(headers_out[13].c_str(), "operator_name");
  EXPECT_STREQ(headers_out[14].c_str(), "acq_method_name");
  EXPECT_STREQ(headers_out[15].c_str(), "proc_method_name");
  EXPECT_STREQ(headers_out[16].c_str(), "original_filename");
  EXPECT_STREQ(headers_out[17].c_str(), "acquisition_date_and_time");
  EXPECT_STREQ(headers_out[18].c_str(), "scan_polarity");
  EXPECT_STREQ(headers_out[19].c_str(), "scan_mass_low");
  EXPECT_STREQ(headers_out[20].c_str(), "scan_mass_high");
  EXPECT_STREQ(headers_out[21].c_str(), "injection_name");
  EXPECT_STREQ(headers_out[22].c_str(), "used_");
  // metadata
  EXPECT_STREQ(headers_out[23].c_str(), "leftWidth");

  // write sequence to output
  // const std::string pathname_output = SMARTPEAK_GET_TEST_DATA_PATH("output/SequenceParser_writeDataTableFromMetaValue.csv");
  // SequenceParser::writeDataTableFromMetaValue(sequenceHandler, pathname_output, meta_data, sample_types);
}

TEST_F(SequenceParserFixture, makeDataMatrixFromMetaValue)
{
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

  SequenceParser::makeDataMatrixFromMetaValue(sequence_handler_,
                                              data_out,
                                              columns_out,
                                              rows_out,
                                              meta_data,
                                              sample_types,
                                              std::set<std::string>(),
                                              std::set<std::string>(),
                                              std::set<std::string>());

  EXPECT_EQ(columns_out.size(), 6);
  EXPECT_STREQ(columns_out(0).c_str(), "170808_Jonathan_yeast_Sacc1_1x");
  EXPECT_EQ(rows_out.dimension(0), 912);
  EXPECT_EQ(rows_out.dimension(1), 3);
  EXPECT_STREQ(rows_out(0,1).c_str(), "23dpg");
  EXPECT_NEAR(data_out(1,0), 15.6053667, 1e-3);
  EXPECT_NEAR(data_out(rows_out.dimension(0)-1,columns_out.size()-1), 1.66744995, 1e-3);

  SequenceParser::makeDataMatrixFromMetaValue(sequence_handler_,
                                              data_out,
                                              columns_out,
                                              rows_out,
                                              std::vector<std::string>({ "leftWidth" }),
                                              sample_types,
                                              std::set<std::string>({ "170808_Jonathan_yeast_Sacc1_1x" }),
                                              std::set<std::string>({ "23dpg" }),
                                              std::set<std::string>({ "23dpg.23dpg_1.Light" }));

  EXPECT_EQ(columns_out.size(), 1);
  EXPECT_STREQ(columns_out(0).c_str(), "170808_Jonathan_yeast_Sacc1_1x");
  EXPECT_EQ(rows_out.dimension(0), 1);
  EXPECT_EQ(rows_out.dimension(1), 3);
  EXPECT_STREQ(rows_out(0, 1).c_str(), "23dpg");
  EXPECT_NEAR(data_out(0, 0), 15.6053667, 1e-3);
}

TEST_F(SequenceParserFixture, writeDataMatrixFromMetaValue)
{
  Eigen::Tensor<float, 2> data_out;
  Eigen::Tensor<std::string, 1> columns_out;
  Eigen::Tensor<std::string, 2> rows_out;

  const vector<FeatureMetadata> meta_data = {
    FeatureMetadata::calculated_concentration,
  };

  const set<SampleType> sample_types = { SampleType::Unknown };

  std::string written_filename = std::tmpnam(nullptr);
  SequenceParser::writeDataMatrixFromMetaValue(sequence_handler_, written_filename, meta_data, sample_types);

  // read back the file
  io::CSVReader<9, io::trim_chars<>, io::no_quote_escape<','>> in(written_filename);

  const std::string s_component_name{ "component_name" };
  const std::string s_component_group_name{ "component_group_name" };
  const std::string s_meta_value{ "meta_value" };
  const std::string s_injection_1{ "170808_Jonathan_yeast_Sacc1_1x" };
  const std::string s_injection_2{ "170808_Jonathan_yeast_Sacc2_1x" };
  const std::string s_injection_3{ "170808_Jonathan_yeast_Sacc3_1x" };
  const std::string s_injection_4{ "170808_Jonathan_yeast_Yarr1_1x" };
  const std::string s_injection_5{ "170808_Jonathan_yeast_Yarr2_1x" };
  const std::string s_injection_6{ "170808_Jonathan_yeast_Yarr3_1x" };

  in.read_header(
    io::ignore_no_column,
    s_component_name,
    s_component_group_name,
    s_meta_value,
    s_injection_1,
    s_injection_2,
    s_injection_3,
    s_injection_4,
    s_injection_5,
    s_injection_6
    );

  std::string component_name;
  std::string component_group_name;
  std::string meta_value;
  std::string injection_1;
  std::string injection_2;
  std::string injection_3;
  std::string injection_4;
  std::string injection_5;
  std::string injection_6;
  for (int i = 0; i < 27; ++i)  // we will check row 27 (previous are 0-values)
  {
    in.read_row(
      component_name,
      component_group_name,
      meta_value,
      injection_1,
      injection_2,
      injection_3,
      injection_4,
      injection_5,
      injection_6
    );
  }

  EXPECT_STREQ(component_name.c_str(), "accoa.accoa_1.Light");
  EXPECT_STREQ(component_group_name.c_str(), "accoa");
  EXPECT_STREQ(meta_value.c_str(), "calculated_concentration");
  EXPECT_STREQ(injection_1.c_str(), "1.284786");
  EXPECT_STREQ(injection_2.c_str(), "2.345204");
  EXPECT_STREQ(injection_3.c_str(), "2.027345");
  EXPECT_STREQ(injection_4.c_str(), "2.128750");
  EXPECT_STREQ(injection_5.c_str(), "0.782144");
  EXPECT_STREQ(injection_6.c_str(), "2.478645");
}

TEST(SequenceHandler, makeSequenceSmartPeak)
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
    metaDataHandler.setReplicateGroupName("replicate_group_name");
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

  SequenceParser::makeSequenceFileSmartPeak(sequenceHandler, data_out, headers_out);

  EXPECT_EQ(data_out.size(), 6);
  EXPECT_TRUE(data_out.at(0).size() == 21);
  EXPECT_STREQ(data_out.at(0).at(0).c_str(), "170808_Jonathan_yeast_Sacc1_1x");
  EXPECT_STREQ(data_out.at(0).at(1).c_str(), "sample_group");
  EXPECT_STREQ(data_out.at(0).at(2).c_str(), "sequence_segment");
  EXPECT_STREQ(data_out.at(0).at(3).c_str(), "replicate_group_name");
  EXPECT_STREQ(data_out.at(0).at(4).c_str(), "Unknown");
  EXPECT_STREQ(data_out.at(0).at(5).c_str(), "170808_Jonathan_yeast_Sacc1_1x_1_FluxTest_2020-01-01_171400");
  EXPECT_STREQ(data_out.at(0).at(6).c_str(), "");
  EXPECT_STREQ(data_out.at(0).at(7).c_str(), "4");
  EXPECT_STREQ(data_out.at(0).at(8).c_str(), "3");
  EXPECT_STREQ(data_out.at(0).at(9).c_str(), "1");
  EXPECT_STREQ(data_out.at(0).at(10).c_str(), "1");
  EXPECT_STREQ(data_out.at(0).at(11).c_str(), "8.000000");
  EXPECT_STREQ(data_out.at(0).at(12).c_str(), "RapidRIP");
  EXPECT_STREQ(data_out.at(0).at(13).c_str(), "");
  EXPECT_STREQ(data_out.at(0).at(14).c_str(), "2020-01-01_171400");
  EXPECT_STREQ(data_out.at(0).at(15).c_str(), "7.000000");
  EXPECT_STREQ(data_out.at(0).at(16).c_str(), "8");
  EXPECT_STREQ(data_out.at(0).at(17).c_str(), "FluxTest");
  EXPECT_STREQ(data_out.at(0).at(18).c_str(), "negative");
  EXPECT_STREQ(data_out.at(0).at(19).c_str(), "60.000000");
  EXPECT_STREQ(data_out.at(0).at(20).c_str(), "2000.000000");

  EXPECT_EQ(headers_out.size(), 21);
  EXPECT_STREQ(headers_out.at(0).c_str(), "sample_name");
  EXPECT_STREQ(headers_out.at(1).c_str(), "sample_group_name");
  EXPECT_STREQ(headers_out.at(2).c_str(), "sequence_segment_name");
  EXPECT_STREQ(headers_out.at(3).c_str(), "replicate_group_name");
  EXPECT_STREQ(headers_out.at(4).c_str(), "sample_type");
  EXPECT_STREQ(headers_out.at(5).c_str(), "original_filename");
  EXPECT_STREQ(headers_out.at(6).c_str(), "proc_method_name");
  EXPECT_STREQ(headers_out.at(7).c_str(), "rack_number");
  EXPECT_STREQ(headers_out.at(8).c_str(), "plate_number");
  EXPECT_STREQ(headers_out.at(9).c_str(), "pos_number");
  EXPECT_STREQ(headers_out.at(10).c_str(), "inj_number");
  EXPECT_STREQ(headers_out.at(11).c_str(), "dilution_factor");
  EXPECT_STREQ(headers_out.at(12).c_str(), "acq_method_name");
  EXPECT_STREQ(headers_out.at(13).c_str(), "operator_name");
  EXPECT_STREQ(headers_out.at(14).c_str(), "acquisition_date_and_time");
  EXPECT_STREQ(headers_out.at(15).c_str(), "inj_volume");
  EXPECT_STREQ(headers_out.at(16).c_str(), "inj_volume_units");
  EXPECT_STREQ(headers_out.at(17).c_str(), "batch_name");
  EXPECT_STREQ(headers_out.at(18).c_str(), "scan_polarity");
  EXPECT_STREQ(headers_out.at(19).c_str(), "scan_mass_low");
  EXPECT_STREQ(headers_out.at(20).c_str(), "scan_mass_high");
}

TEST(SequenceHandler, makeSequenceFileAnalyst)
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
    metaDataHandler.setReplicateGroupName("replicate_group_name");
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

  EXPECT_EQ(data_out.size(), 6);
  EXPECT_STREQ(data_out.at(0).at(0).c_str(), "170808_Jonathan_yeast_Sacc1_1x_1_FluxTest_2020-01-01_171400");
  EXPECT_STREQ(data_out.at(0).at(1).c_str(), "170808_Jonathan_yeast_Sacc1_1x");
  EXPECT_STREQ(data_out.at(0).at(2).c_str(), "");
  EXPECT_STREQ(data_out.at(0).at(3).c_str(), "RapidRIP.dam");
  EXPECT_STREQ(data_out.at(0).at(4).c_str(), "none");
  EXPECT_STREQ(data_out.at(0).at(5).c_str(), "CStk1-04");
  EXPECT_STREQ(data_out.at(0).at(6).c_str(), "MT96_or_VT54");
  EXPECT_STREQ(data_out.at(0).at(7).c_str(), "1");
  EXPECT_STREQ(data_out.at(0).at(8).c_str(), "7.000000");
  EXPECT_STREQ(data_out.at(0).at(9).c_str(), "8.000000");
  EXPECT_STREQ(data_out.at(0).at(10).c_str(), "0");
  EXPECT_STREQ(data_out.at(0).at(11).c_str(), "Unknown");
  EXPECT_STREQ(data_out.at(0).at(12).c_str(), "4");
  EXPECT_STREQ(data_out.at(0).at(13).c_str(), "3");
  EXPECT_STREQ(data_out.at(0).at(14).c_str(), "2020-01-01_171400");
  EXPECT_STREQ(data_out.at(0).at(15).c_str(), "2020-01-01_171400\\170808_Jonathan_yeast_Sacc1_1x_1_FluxTest_2020-01-01_171400");
  EXPECT_EQ(headers_out.size(), 16);
  EXPECT_STREQ(headers_out.at(0).c_str(), "% header=SampleName");
  EXPECT_STREQ(headers_out.at(1).c_str(), "SampleID");
  EXPECT_STREQ(headers_out.at(2).c_str(), "Comments");
  EXPECT_STREQ(headers_out.at(3).c_str(), "AcqMethod");
  EXPECT_STREQ(headers_out.at(4).c_str(), "ProcMethod");
  EXPECT_STREQ(headers_out.at(5).c_str(), "RackCode");
  EXPECT_STREQ(headers_out.at(6).c_str(), "PlateCode");
  EXPECT_STREQ(headers_out.at(7).c_str(), "VialPos");
  EXPECT_STREQ(headers_out.at(8).c_str(), "SmplInjVol");
  EXPECT_STREQ(headers_out.at(9).c_str(), "DilutFact");
  EXPECT_STREQ(headers_out.at(10).c_str(), "WghtToVol");
  EXPECT_STREQ(headers_out.at(11).c_str(), "Type");
  EXPECT_STREQ(headers_out.at(12).c_str(), "RackPos");
  EXPECT_STREQ(headers_out.at(13).c_str(), "PlatePos");
  EXPECT_STREQ(headers_out.at(14).c_str(), "SetName");
  EXPECT_STREQ(headers_out.at(15).c_str(), "OutputFile");
}

TEST(SequenceHandler, makeSequenceFileMasshunter)
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

  EXPECT_EQ(data_out.size(), 6);
  EXPECT_STREQ(data_out.at(0).at(0).c_str(), "170808_Jonathan_yeast_Sacc1_1x");
  EXPECT_STREQ(data_out.at(0).at(1).c_str(), "1");
  EXPECT_STREQ(data_out.at(0).at(2).c_str(), "D:\\DATA\\TODO");
  EXPECT_STREQ(data_out.at(0).at(3).c_str(), "RapidRIP.M");
  EXPECT_STREQ(data_out.at(0).at(4).c_str(), "D:\\DATA\\TODO");
  EXPECT_STREQ(data_out.at(0).at(5).c_str(), "2020-01-01_171400\\170808_Jonathan_yeast_Sacc1_1x_1_FluxTest_2020-01-01_171400");
  EXPECT_STREQ(data_out.at(0).at(6).c_str(), "Unknown");
  EXPECT_STREQ(data_out.at(0).at(7).c_str(), "");
  EXPECT_STREQ(data_out.at(0).at(8).c_str(), "8.000000");
  EXPECT_STREQ(data_out.at(0).at(9).c_str(), "7.000000");
  EXPECT_STREQ(data_out.at(0).at(10).c_str(), "Rack 4");
  EXPECT_STREQ(data_out.at(0).at(11).c_str(), "");
  EXPECT_EQ(headers_out.size(), 12);
  EXPECT_STREQ(headers_out.at(0).c_str(), "Name");
  EXPECT_STREQ(headers_out.at(1).c_str(), "Vial");
  EXPECT_STREQ(headers_out.at(2).c_str(), "Method Path");
  EXPECT_STREQ(headers_out.at(3).c_str(), "Mathod File");
  EXPECT_STREQ(headers_out.at(4).c_str(), "Data Path");
  EXPECT_STREQ(headers_out.at(5).c_str(), "Data File");
  EXPECT_STREQ(headers_out.at(6).c_str(), "Type");
  EXPECT_STREQ(headers_out.at(7).c_str(), "Level");
  EXPECT_STREQ(headers_out.at(8).c_str(), "Dil.");
  EXPECT_STREQ(headers_out.at(9).c_str(), "Vol.");
  EXPECT_STREQ(headers_out.at(10).c_str(), "Tray Name");
  EXPECT_STREQ(headers_out.at(11).c_str(), "Comment");
}

TEST(SequenceHandler, makeSequenceFileXcalibur)
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
    metaDataHandler.setReplicateGroupName("replicate_group_name");
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

  SequenceParser::makeSequenceFileXcalibur(sequenceHandler, data_out, headers_out);

  EXPECT_EQ(data_out.size(), 6);
  EXPECT_STREQ(data_out.at(0).at(0).c_str(), "Unknown");
  EXPECT_STREQ(data_out.at(0).at(1).c_str(), "2020-01-01_171400\\170808_Jonathan_yeast_Sacc1_1x_1_FluxTest_2020-01-01_171400");
  EXPECT_STREQ(data_out.at(0).at(2).c_str(), "1");
  EXPECT_STREQ(data_out.at(0).at(3).c_str(), "D:\\DATA\\TODO");
  EXPECT_STREQ(data_out.at(0).at(4).c_str(), "RapidRIP");
  EXPECT_STREQ(data_out.at(0).at(5).c_str(), "");
  EXPECT_STREQ(data_out.at(0).at(6).c_str(), "");
  EXPECT_STREQ(data_out.at(0).at(7).c_str(), "R:TODO1");
  EXPECT_STREQ(data_out.at(0).at(8).c_str(), "7.000000");
  EXPECT_STREQ(data_out.at(0).at(9).c_str(), "");
  EXPECT_STREQ(data_out.at(0).at(10).c_str(), "0");
  EXPECT_STREQ(data_out.at(0).at(11).c_str(), "0");
  EXPECT_STREQ(data_out.at(0).at(12).c_str(), "0");
  EXPECT_STREQ(data_out.at(0).at(13).c_str(), "8.000000");
  EXPECT_STREQ(data_out.at(0).at(14).c_str(), "");
  EXPECT_STREQ(data_out.at(0).at(15).c_str(), "");
  EXPECT_STREQ(data_out.at(0).at(16).c_str(), "");
  EXPECT_STREQ(data_out.at(0).at(17).c_str(), "");
  EXPECT_STREQ(data_out.at(0).at(18).c_str(), "");
  EXPECT_STREQ(data_out.at(0).at(19).c_str(), "");
  EXPECT_STREQ(data_out.at(0).at(20).c_str(), "170808_Jonathan_yeast_Sacc1_1x");
  EXPECT_EQ(headers_out.size(), 21);
  EXPECT_STREQ(headers_out.at(0).c_str(), "Sample Type");
  EXPECT_STREQ(headers_out.at(1).c_str(), "File Name");
  EXPECT_STREQ(headers_out.at(2).c_str(), "Sample ID");
  EXPECT_STREQ(headers_out.at(3).c_str(), "Path");
  EXPECT_STREQ(headers_out.at(4).c_str(), "Instrument Method");
  EXPECT_STREQ(headers_out.at(5).c_str(), "Process Method");
  EXPECT_STREQ(headers_out.at(6).c_str(), "Calibration File");
  EXPECT_STREQ(headers_out.at(7).c_str(), "Position");
  EXPECT_STREQ(headers_out.at(8).c_str(), "Inj Vol");
  EXPECT_STREQ(headers_out.at(9).c_str(), "Level");
  EXPECT_STREQ(headers_out.at(10).c_str(), "Sample Wt");
  EXPECT_STREQ(headers_out.at(11).c_str(), "Sample Vol");
  EXPECT_STREQ(headers_out.at(12).c_str(), "ISTD Amt");
  EXPECT_STREQ(headers_out.at(13).c_str(), "Dil Factor");
  EXPECT_STREQ(headers_out.at(14).c_str(), "L1 Study");
  EXPECT_STREQ(headers_out.at(15).c_str(), "L2 Client");
  EXPECT_STREQ(headers_out.at(16).c_str(), "L3 Laboratory");
  EXPECT_STREQ(headers_out.at(17).c_str(), "L4 Company");
  EXPECT_STREQ(headers_out.at(18).c_str(), "L5 Phone");
  EXPECT_STREQ(headers_out.at(19).c_str(), "Comment");
  EXPECT_STREQ(headers_out.at(20).c_str(), "Sample Name");
}

/* StoreSequenceFileAnalyst */
TEST_F(SequenceParserFixture, StoreSequenceFileAnalyst_ProcessFailsOnEmptySequence)
{
  // Default ApplicationHandler has a default SequenceHandler with empty sequence
  ApplicationHandler ah;
  StoreSequenceFileAnalyst cmd;
  auto store = cmd.onFilePicked("test", &ah);
  EXPECT_TRUE(!store);
}

TEST_F(SequenceParserFixture, StoreSequenceFileAnalyst_ProcessSucceedsOnNonEmptySequence)
{
  ApplicationHandler ah;
  ah.sequenceHandler_ = sequence_handler_;
  StoreSequenceFileAnalyst cmd;
  std::string filename = std::tmpnam(nullptr);
  EXPECT_TRUE(cmd.onFilePicked(filename, &ah));
}

/* StoreSequenceFileMasshunter */
TEST_F(SequenceParserFixture, StoreSequenceFileMasshunter_ProcessFailsOnEmptySequence)
{
  // Default ApplicationHandler has a default SequenceHandler with empty sequence
  ApplicationHandler ah;
  StoreSequenceFileMasshunter cmd;
  auto store = cmd.onFilePicked("test", &ah);
  EXPECT_TRUE(!store);
}

TEST_F(SequenceParserFixture, StoreSequenceFileMasshunter_ProcessSucceedsOnNonEmptySequence)
{
  ApplicationHandler ah;
  ah.sequenceHandler_ = sequence_handler_;
  StoreSequenceFileMasshunter cmd;
  std::string filename = std::tmpnam(nullptr);
  EXPECT_TRUE(cmd.onFilePicked(filename, &ah));
}

/* StoreSequenceFileXcalibur */
TEST(SequenceHandler, StoreSequenceFileXcalibur_ProcessFailsOnEmptySequence)
{
  // Default ApplicationHandler has a default SequenceHandler with empty sequence
  ApplicationHandler ah;
  StoreSequenceFileXcalibur cmd;
  auto store = cmd.onFilePicked("test", &ah);
  EXPECT_TRUE(!store);
}

TEST_F(SequenceParserFixture, StoreSequenceFileXcalibur_ProcessSucceedsOnNonEmptySequence)
{
  ApplicationHandler ah;
  ah.sequenceHandler_ = sequence_handler_;
  StoreSequenceFileXcalibur cmd;
  std::string filename = std::tmpnam(nullptr);
  EXPECT_TRUE(cmd.onFilePicked(filename, &ah));
}
