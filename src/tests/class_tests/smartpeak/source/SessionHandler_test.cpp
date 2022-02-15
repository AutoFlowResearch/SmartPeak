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
#include <SmartPeak/test_config.h>
#include <SmartPeak/core/SessionHandler.h>
#include <SmartPeak/core/SequenceProcessor.h>
#include <SmartPeak/core/Utilities.h>
#include <SmartPeak/core/RawDataProcessors/LoadFeatures.h>
#include <SmartPeak/core/RawDataProcessors/LoadRawData.h>
#include <SmartPeak/core/SequenceSegmentProcessors/StoreFeatureRSDEstimations.h>
#include <SmartPeak/core/SequenceSegmentProcessors/EstimateFeatureBackgroundInterferences.h>
#include <SmartPeak/core/SequenceSegmentProcessors/EstimateFeatureRSDs.h>
#include <SmartPeak/core/ApplicationProcessors/LoadSession.h>

using namespace SmartPeak;
using namespace std;

struct TestData {
  TestData(const bool& load_sequence = true, const bool& load_features = false, const bool& load_raw_data = false){
    // Handle the filenames
    const std::string pathname = SMARTPEAK_GET_TEST_DATA_PATH("workflow_csv_files");
    // Load the sequence
    if (load_sequence) {
      application_handler.filenames_ = Utilities::buildFilenamesFromDirectory(application_handler, pathname);
      WorkflowManager workflow_manager;
      LoadSession cs(application_handler, workflow_manager);
      cs.filenames_ = application_handler.filenames_;
      cs.delimiter = ",";
      cs.checkConsistency = false;
      cs.process();
    }
    // Load the picked features
    if (load_features) {
      LoadFeatures loadFeatures;
      Filenames method_filenames;
      method_filenames.setTagValue(Filenames::Tag::MAIN_DIR, pathname);
      method_filenames.setTagValue(Filenames::Tag::MZML_INPUT_PATH, pathname + "/mzML");
      method_filenames.setTagValue(Filenames::Tag::FEATURES_INPUT_PATH, pathname + "/features");
      method_filenames.setTagValue(Filenames::Tag::FEATURES_OUTPUT_PATH, pathname + "/features");
      for (auto& injection : application_handler.sequenceHandler_.getSequence()) {
        Filenames filenames_ = method_filenames;
        filenames_.setTagValue(Filenames::Tag::INPUT_MZML_FILENAME, injection.getMetaData().getFilename());
        filenames_.setTagValue(Filenames::Tag::INPUT_INJECTION_NAME, injection.getMetaData().getInjectionName());
        filenames_.setTagValue(Filenames::Tag::OUTPUT_INJECTION_NAME, injection.getMetaData().getInjectionName());
        filenames_.setTagValue(Filenames::Tag::INPUT_GROUP_NAME, injection.getMetaData().getSampleGroupName());
        filenames_.setTagValue(Filenames::Tag::OUTPUT_GROUP_NAME, injection.getMetaData().getSampleGroupName());
        loadFeatures.process(injection.getRawData(), {}, filenames_);
      }
    }
    // Load the raw data
    if (load_raw_data) {
      ParameterSet params;
      params.addFunctionParameters(FunctionParameters("mzML"));
      params.addFunctionParameters(FunctionParameters("ChromatogramExtractor"));
      LoadRawData loadRawData;
      Filenames method_filenames;
      method_filenames.setTagValue(Filenames::Tag::MAIN_DIR, pathname);
      method_filenames.setTagValue(Filenames::Tag::MZML_INPUT_PATH, pathname + "/mzML");
      method_filenames.setTagValue(Filenames::Tag::FEATURES_INPUT_PATH, pathname + "/features");
      method_filenames.setTagValue(Filenames::Tag::FEATURES_OUTPUT_PATH, pathname + "/features");
      for (auto& injection : application_handler.sequenceHandler_.getSequence()) {
        Filenames filenames_ = method_filenames;
        filenames_.setTagValue(Filenames::Tag::INPUT_MZML_FILENAME, injection.getMetaData().getFilename());
        filenames_.setTagValue(Filenames::Tag::INPUT_INJECTION_NAME, injection.getMetaData().getInjectionName());
        filenames_.setTagValue(Filenames::Tag::OUTPUT_INJECTION_NAME, injection.getMetaData().getInjectionName());
        filenames_.setTagValue(Filenames::Tag::INPUT_GROUP_NAME, injection.getMetaData().getSampleGroupName());
        filenames_.setTagValue(Filenames::Tag::OUTPUT_GROUP_NAME, injection.getMetaData().getSampleGroupName());
        loadRawData.process(injection.getRawData(), params, filenames_);
      }
    }
  }
  void changeSampleType(const SampleType& sample_type) {
    for (auto& injection : application_handler.sequenceHandler_.getSequence()) {
      injection.getMetaData().setSampleType(sample_type);
    }
  }
  ApplicationHandler application_handler;
};

TEST(SessionHandler, setSequenceTable1)
{
  TestData testData;
  SessionHandler session_handler;
  session_handler.setSequenceTable(testData.application_handler.sequenceHandler_, session_handler.sequence_table);
  EXPECT_EQ(session_handler.sequence_table.headers_.size(), 12);
  EXPECT_STREQ(session_handler.sequence_table.headers_(0).c_str(), "inj#");
  EXPECT_STREQ(session_handler.sequence_table.headers_(session_handler.sequence_table.headers_.size() - 1).c_str(), "acquisition_date_and_time");
  EXPECT_EQ(session_handler.sequence_table.body_.dimension(0), 2);
  EXPECT_EQ(session_handler.sequence_table.body_.dimension(1), 12);
  const auto debug_to_remove = session_handler.sequence_table.body_(0, 0);
  EXPECT_STREQ(session_handler.sequence_table.body_(0, 0).c_str(), "1");
  EXPECT_STREQ(session_handler.sequence_table.body_(session_handler.sequence_table.body_.dimension(0) - 1, session_handler.sequence_table.body_.dimension(1) - 1).c_str(), "1900-01-01_000000");

  session_handler.setInjectionExplorer(session_handler.sequence_table);
  EXPECT_EQ(session_handler.injection_explorer_data.checkbox_headers.size(), 3);
  EXPECT_STREQ(session_handler.injection_explorer_data.checkbox_headers(0).c_str(), "workflow");
  EXPECT_STREQ(session_handler.injection_explorer_data.checkbox_headers(session_handler.injection_explorer_data.checkbox_headers.size() - 1).c_str(), "table");
  EXPECT_EQ(session_handler.injection_explorer_data.checkbox_body.dimension(0), 2);
  EXPECT_EQ(session_handler.injection_explorer_data.checkbox_body.dimension(1), 3);
  EXPECT_TRUE(session_handler.injection_explorer_data.checkbox_body(0, 0));
  EXPECT_TRUE(!session_handler.injection_explorer_data.checkbox_body(1, 1));
  EXPECT_TRUE(session_handler.injection_explorer_data.checkbox_body(session_handler.injection_explorer_data.checkbox_body.dimension(0) - 1, session_handler.injection_explorer_data.checkbox_body.dimension(1) - 1));
}

TEST(SessionHandler, setTransitionsTable1)
{
  TestData testData;
  SessionHandler session_handler; 
  session_handler.setTransitionsTable(testData.application_handler.sequenceHandler_, session_handler.transitions_table);
  EXPECT_EQ(session_handler.transitions_table.headers_.size(), 9);
  EXPECT_STREQ(session_handler.transitions_table.headers_(0).c_str(), "transition_group");
  EXPECT_STREQ(session_handler.transitions_table.headers_(session_handler.transitions_table.headers_.size() - 1).c_str(), "detecting_transition");
  EXPECT_EQ(session_handler.transitions_table.body_.dimension(0), 6);
  EXPECT_EQ(session_handler.transitions_table.body_.dimension(1), 9);
  EXPECT_STREQ(session_handler.transitions_table.body_(0, 0).c_str(), "arg-L");
  EXPECT_STREQ(session_handler.transitions_table.body_(session_handler.transitions_table.body_.dimension(0) - 1, session_handler.transitions_table.body_.dimension(1) - 1).c_str(), "1");

  session_handler.setTransitionExplorer(session_handler.transitions_table);
  EXPECT_EQ(session_handler.transition_explorer_data.checkbox_headers.size(), 2);
  EXPECT_STREQ(session_handler.transition_explorer_data.checkbox_headers(0).c_str(), "plot");
  EXPECT_STREQ(session_handler.transition_explorer_data.checkbox_headers(session_handler.transition_explorer_data.checkbox_headers.size() - 1).c_str(), "table");
  EXPECT_EQ(session_handler.transition_explorer_data.checkbox_body.dimension(0), 6);
  EXPECT_EQ(session_handler.transition_explorer_data.checkbox_body.dimension(1), 2);
  EXPECT_TRUE(!session_handler.transition_explorer_data.checkbox_body(0, 0));
  EXPECT_TRUE(!session_handler.transition_explorer_data.checkbox_body(1, 0));
  EXPECT_TRUE(session_handler.transition_explorer_data.checkbox_body(session_handler.transition_explorer_data.checkbox_body.dimension(0) - 1, session_handler.transition_explorer_data.checkbox_body.dimension(1) - 1));
}

TEST(SessionHandler, setFeatureExplorer1)
{
  TestData testData;
  SessionHandler session_handler; 
  session_handler.setFeatureExplorer();
  EXPECT_EQ(session_handler.feature_table.headers_.size(), 1);
  EXPECT_STREQ(session_handler.feature_table.headers_(0).c_str(), "name");
  EXPECT_EQ(session_handler.feature_table.body_.dimension(0), 22);
  EXPECT_EQ(session_handler.feature_table.body_.dimension(1), 1);
  EXPECT_STREQ(session_handler.feature_table.body_(0, 0).c_str(), "asymmetry_factor");
  EXPECT_STREQ(session_handler.feature_table.body_(session_handler.feature_table.body_.dimension(0) - 1, session_handler.feature_table.body_.dimension(1) - 1).c_str(), "absolute_difference");
  EXPECT_EQ(session_handler.feature_explorer_data.checkbox_headers.size(), 2);
  EXPECT_STREQ(session_handler.feature_explorer_data.checkbox_headers(0).c_str(), "plot");
  EXPECT_STREQ(session_handler.feature_explorer_data.checkbox_headers(session_handler.feature_explorer_data.checkbox_headers.size() - 1).c_str(), "table");
  EXPECT_EQ(session_handler.feature_explorer_data.checkbox_body.dimension(0), 22);
  EXPECT_EQ(session_handler.feature_explorer_data.checkbox_body.dimension(1), 2);
  EXPECT_TRUE(!session_handler.feature_explorer_data.checkbox_body(0, 0));
  EXPECT_FALSE(session_handler.feature_explorer_data.checkbox_body(2, 0));
  EXPECT_FALSE(session_handler.feature_explorer_data.checkbox_body(session_handler.feature_explorer_data.checkbox_body.dimension(0) - 1, session_handler.feature_explorer_data.checkbox_body.dimension(1) - 1));
}

TEST(SessionHandler, setSpectrumExplorer1)
{
  TestData testData(true, false, true);
  SessionHandler session_handler;
  session_handler.setSpectrumTable(testData.application_handler.sequenceHandler_, session_handler.spectrum_table);
  EXPECT_EQ(session_handler.spectrum_table.headers_.size(), 8);
  EXPECT_STREQ(session_handler.spectrum_table.headers_(0).c_str(), "native_id");
  EXPECT_STREQ(session_handler.spectrum_table.headers_(session_handler.spectrum_table.headers_.size() - 1).c_str(), "highest observed m/z");
  EXPECT_EQ(session_handler.spectrum_table.body_.dimension(0), 1);
  EXPECT_EQ(session_handler.spectrum_table.body_.dimension(1), 8);
  EXPECT_STREQ(session_handler.spectrum_table.body_(0, 0).c_str(), "controllerType=0 controllerNumber=1 scan=1");
  EXPECT_STREQ(session_handler.spectrum_table.body_(session_handler.spectrum_table.body_.dimension(0) - 1, session_handler.spectrum_table.body_.dimension(1) - 1).c_str(), "209.166053007435011");

  session_handler.setSpectrumExplorer(session_handler.spectrum_table);
  EXPECT_EQ(session_handler.spectrum_explorer_data.checkbox_headers.size(), 2);
  EXPECT_STREQ(session_handler.spectrum_explorer_data.checkbox_headers(0).c_str(), "plot");
  EXPECT_STREQ(session_handler.spectrum_explorer_data.checkbox_headers(session_handler.spectrum_explorer_data.checkbox_headers.size() - 1).c_str(), "table");
  EXPECT_EQ(session_handler.spectrum_explorer_data.checkbox_body.dimension(0), 1);
  EXPECT_EQ(session_handler.spectrum_explorer_data.checkbox_body.dimension(1), 2);
  EXPECT_TRUE(session_handler.spectrum_explorer_data.checkbox_body(0, 0));
  EXPECT_TRUE(session_handler.spectrum_explorer_data.checkbox_body(1, 0));
  EXPECT_TRUE(session_handler.spectrum_explorer_data.checkbox_body(session_handler.spectrum_explorer_data.checkbox_body.dimension(0) - 1, session_handler.spectrum_explorer_data.checkbox_body.dimension(1) - 1));
}

TEST(SessionHandler, setWorkflowTable1)
{
  TestData testData;
  //SessionHandler session_handler; session_handler.setWorkflowTable();
}

TEST(SessionHandler, setQuantMethodTable1)
{
  TestData testData;
  SessionHandler session_handler;
  SessionHandler::GenericTableData table_data;
  session_handler.setQuantMethodTable(testData.application_handler.sequenceHandler_, table_data);
  EXPECT_EQ(table_data.headers_.size(), 20);
  EXPECT_STREQ(table_data.headers_(0).c_str(), "component_name");
  EXPECT_STREQ(table_data.headers_(table_data.headers_.size() - 1).c_str(), "transformation_model_param_y_datum_max");
  EXPECT_EQ(table_data.body_.dimension(0), 10);
  EXPECT_EQ(table_data.body_.dimension(1), 20);
  EXPECT_STREQ(table_data.body_(0, 0).c_str(), "arg-L.arg-L_1.Light");
  EXPECT_STREQ(table_data.body_(table_data.body_.dimension(0) - 1, table_data.body_.dimension(1) - 1).c_str(), "1000000000000000.000000");
}

TEST(SessionHandler, setStdsConcsTable1)
{
  TestData testData;
  SessionHandler session_handler; 
  SessionHandler::GenericTableData table_data;
  session_handler.setStdsConcsTable(testData.application_handler.sequenceHandler_, table_data);
  EXPECT_EQ(table_data.headers_.size(), 7);
  EXPECT_STREQ(table_data.headers_(0).c_str(), "sample_name");
  EXPECT_STREQ(table_data.headers_(table_data.headers_.size() - 1).c_str(), "dilution_factor");
  EXPECT_EQ(table_data.body_.dimension(0), 8);
  EXPECT_EQ(table_data.body_.dimension(1), 7);
  EXPECT_STREQ(table_data.body_(0, 0).c_str(), "150516_CM1_Level1");
  EXPECT_STREQ(table_data.body_(table_data.body_.dimension(0) - 1, table_data.body_.dimension(1) - 1).c_str(), "1.000000");
}

TEST(SessionHandler, setComponentFiltersTable1)
{
  TestData testData;
  SessionHandler session_handler; 
  SessionHandler::GenericTableData table_data;
  session_handler.setComponentFiltersTable(testData.application_handler.sequenceHandler_, table_data);
  EXPECT_EQ(table_data.headers_.size(), 11);
  EXPECT_STREQ(table_data.headers_(0).c_str(), "component_name");
  EXPECT_STREQ(table_data.headers_(table_data.headers_.size() - 1).c_str(), "metaValue_peak_apex_int_u");
  EXPECT_EQ(table_data.body_.dimension(0), 10);
  EXPECT_EQ(table_data.body_.dimension(1), 11);
  EXPECT_STREQ(table_data.body_(0, 0).c_str(), "arg-L.arg-L_1.Heavy");
  EXPECT_STREQ(table_data.body_(table_data.body_.dimension(0) - 1, table_data.body_.dimension(1) - 1).c_str(), "1000000000000.000000");
}

TEST(SessionHandler, setComponentGroupFiltersTable1)
{
  TestData testData;
  SessionHandler session_handler; 
  SessionHandler::GenericTableData table_data;
  session_handler.setComponentGroupFiltersTable(testData.application_handler.sequenceHandler_, table_data);
  EXPECT_EQ(table_data.headers_.size(), 24);
  EXPECT_STREQ(table_data.headers_(0).c_str(), "component_group_name");
  EXPECT_STREQ(table_data.headers_(table_data.headers_.size() - 1).c_str(), "ion_ratio_feature_name");
  EXPECT_EQ(table_data.body_.dimension(0), 10);
  EXPECT_EQ(table_data.body_.dimension(1), 24);
  EXPECT_STREQ(table_data.body_(0, 0).c_str(), "arg-L");
  EXPECT_STREQ(table_data.body_(table_data.body_.dimension(0) - 1, table_data.body_.dimension(1) - 1).c_str(), "");
}

TEST(SessionHandler, setComponentQCsTable1)
{
  TestData testData;
  SessionHandler session_handler;
  SessionHandler::GenericTableData table_data;
  session_handler.setComponentQCsTable(testData.application_handler.sequenceHandler_, table_data);
  EXPECT_EQ(table_data.headers_.size(), 19);
  EXPECT_STREQ(table_data.headers_(0).c_str(), "component_name");
  EXPECT_STREQ(table_data.headers_(table_data.headers_.size() - 1).c_str(), "metaValue_var_xcorr_shape_weighted_u");
  EXPECT_EQ(table_data.body_.dimension(0), 10);
  EXPECT_EQ(table_data.body_.dimension(1), 19);
  EXPECT_STREQ(table_data.body_(0, 0).c_str(), "arg-L.arg-L_1.Heavy");
  EXPECT_STREQ(table_data.body_(table_data.body_.dimension(0) - 1, table_data.body_.dimension(1) - 1).c_str(), "1000000000.000000");
}

TEST(SessionHandler, setComponentGroupQCsTable1)
{
  TestData testData;
  SessionHandler session_handler;
  SessionHandler::GenericTableData table_data;
  session_handler.setComponentGroupQCsTable(testData.application_handler.sequenceHandler_, table_data);
  EXPECT_EQ(table_data.headers_.size(), 24);
  EXPECT_STREQ(table_data.headers_(0).c_str(), "component_group_name");
  EXPECT_STREQ(table_data.headers_(table_data.headers_.size() - 1).c_str(), "ion_ratio_feature_name");
  EXPECT_EQ(table_data.body_.dimension(0), 10);
  EXPECT_EQ(table_data.body_.dimension(1), 24);
  EXPECT_STREQ(table_data.body_(0, 0).c_str(), "arg-L");
  EXPECT_STREQ(table_data.body_(table_data.body_.dimension(0) - 1, table_data.body_.dimension(1) - 1).c_str(), "");
}

TEST(SessionHandler, setComponentRSDFiltersTable1)
{
  TestData testData;
  SessionHandler session_handler;
  SessionHandler::GenericTableData table_data;
  session_handler.setComponentRSDFiltersTable(testData.application_handler.sequenceHandler_, table_data);
  EXPECT_EQ(table_data.headers_.size(), 11);
  EXPECT_STREQ(table_data.headers_(0).c_str(), "component_name");
  EXPECT_STREQ(table_data.headers_(table_data.headers_.size() - 1).c_str(), "metaValue_peak_apex_int_u");
  EXPECT_EQ(table_data.body_.dimension(0), 10);
  EXPECT_EQ(table_data.body_.dimension(1), 11);
  EXPECT_STREQ(table_data.body_(0, 0).c_str(), "arg-L.arg-L_1.Heavy");
  EXPECT_STREQ(table_data.body_(table_data.body_.dimension(0) - 1, table_data.body_.dimension(1) - 1).c_str(), "1000000000000.000000");
}

TEST(SessionHandler, setComponentGroupRSDFiltersTable1)
{
  TestData testData;
  SessionHandler session_handler;
  SessionHandler::GenericTableData table_data;
  session_handler.setComponentGroupRSDFiltersTable(testData.application_handler.sequenceHandler_, table_data);
  EXPECT_EQ(table_data.headers_.size(), 24);
  EXPECT_STREQ(table_data.headers_(0).c_str(), "component_group_name");
  EXPECT_STREQ(table_data.headers_(table_data.headers_.size() - 1).c_str(), "ion_ratio_feature_name");
  EXPECT_EQ(table_data.body_.dimension(0), 10);
  EXPECT_EQ(table_data.body_.dimension(1), 24);
  EXPECT_STREQ(table_data.body_(0, 0).c_str(), "arg-L");
  EXPECT_STREQ(table_data.body_(table_data.body_.dimension(0) - 1, table_data.body_.dimension(1) - 1).c_str(), "");
}

TEST(SessionHandler, setComponentRSDQCsTable1)
{
  TestData testData;
  SessionHandler session_handler;
  SessionHandler::GenericTableData table_data;
  session_handler.setComponentRSDQCsTable(testData.application_handler.sequenceHandler_, table_data);
  EXPECT_EQ(table_data.headers_.size(), 19);
  EXPECT_STREQ(table_data.headers_(0).c_str(), "component_name");
  EXPECT_STREQ(table_data.headers_(table_data.headers_.size() - 1).c_str(), "metaValue_var_xcorr_shape_weighted_u");
  EXPECT_EQ(table_data.body_.dimension(0), 10);
  EXPECT_EQ(table_data.body_.dimension(1), 19);
  EXPECT_STREQ(table_data.body_(0, 0).c_str(), "arg-L.arg-L_1.Heavy");
  EXPECT_STREQ(table_data.body_(table_data.body_.dimension(0) - 1, table_data.body_.dimension(1) - 1).c_str(), "1000000000.000000");
}

TEST(SessionHandler, setComponentGroupRSDQCsTable1)
{
  TestData testData;
  SessionHandler session_handler;
  SessionHandler::GenericTableData table_data;
  session_handler.setComponentGroupRSDQCsTable(testData.application_handler.sequenceHandler_, table_data);
  EXPECT_EQ(table_data.headers_.size(), 24);
  EXPECT_STREQ(table_data.headers_(0).c_str(), "component_group_name");
  EXPECT_STREQ(table_data.headers_(table_data.headers_.size() - 1).c_str(), "ion_ratio_feature_name");
  EXPECT_EQ(table_data.body_.dimension(0), 10);
  EXPECT_EQ(table_data.body_.dimension(1), 24);
  EXPECT_STREQ(table_data.body_(0, 0).c_str(), "arg-L");
  EXPECT_STREQ(table_data.body_(table_data.body_.dimension(0) - 1, table_data.body_.dimension(1) - 1).c_str(), "");
}

TEST(SessionHandler, setComponentBackgroundFiltersTable1)
{
  TestData testData;
  SessionHandler session_handler;
  SessionHandler::GenericTableData table_data;
  session_handler.setComponentBackgroundFiltersTable(testData.application_handler.sequenceHandler_, table_data);
  EXPECT_EQ(table_data.headers_.size(), 11);
  EXPECT_STREQ(table_data.headers_(0).c_str(), "component_name");
  EXPECT_STREQ(table_data.headers_(table_data.headers_.size() - 1).c_str(), "metaValue_peak_apex_int_u");
  EXPECT_EQ(table_data.body_.dimension(0), 10);
  EXPECT_EQ(table_data.body_.dimension(1), 11);
  EXPECT_STREQ(table_data.body_(0, 0).c_str(), "arg-L.arg-L_1.Heavy");
  EXPECT_STREQ(table_data.body_(table_data.body_.dimension(0) - 1, table_data.body_.dimension(1) - 1).c_str(), "1000000000000.000000");
}

TEST(SessionHandler, setComponentGroupBackgroundFiltersTable1)
{
  TestData testData;
  SessionHandler session_handler;
  SessionHandler::GenericTableData table_data;
  session_handler.setComponentGroupBackgroundFiltersTable(testData.application_handler.sequenceHandler_, table_data);
  EXPECT_EQ(table_data.headers_.size(), 24);
  EXPECT_STREQ(table_data.headers_(0).c_str(), "component_group_name");
  EXPECT_STREQ(table_data.headers_(table_data.headers_.size() - 1).c_str(), "ion_ratio_feature_name");
  EXPECT_EQ(table_data.body_.dimension(0), 10);
  EXPECT_EQ(table_data.body_.dimension(1), 24);
  EXPECT_STREQ(table_data.body_(0, 0).c_str(), "arg-L");
  EXPECT_STREQ(table_data.body_(table_data.body_.dimension(0) - 1, table_data.body_.dimension(1) - 1).c_str(), "");
}

TEST(SessionHandler, setComponentBackgroundQCsTable1)
{
  TestData testData;
  SessionHandler session_handler;
  SessionHandler::GenericTableData table_data;
  session_handler.setComponentBackgroundQCsTable(testData.application_handler.sequenceHandler_, table_data);
  EXPECT_EQ(table_data.headers_.size(), 19);
  EXPECT_STREQ(table_data.headers_(0).c_str(), "component_name");
  EXPECT_STREQ(table_data.headers_(table_data.headers_.size() - 1).c_str(), "metaValue_var_xcorr_shape_weighted_u");
  EXPECT_EQ(table_data.body_.dimension(0), 10);
  EXPECT_EQ(table_data.body_.dimension(1), 19);
  EXPECT_STREQ(table_data.body_(0, 0).c_str(), "arg-L.arg-L_1.Heavy");
  EXPECT_STREQ(table_data.body_(table_data.body_.dimension(0) - 1, table_data.body_.dimension(1) - 1).c_str(), "1000000000.000000");
}

TEST(SessionHandler, setComponentGroupBackgroundQCsTable1)
{
  TestData testData;
  SessionHandler session_handler;
  SessionHandler::GenericTableData table_data;
  session_handler.setComponentGroupBackgroundQCsTable(testData.application_handler.sequenceHandler_, table_data);
  EXPECT_EQ(table_data.headers_.size(), 24);
  EXPECT_STREQ(table_data.headers_(0).c_str(), "component_group_name");
  EXPECT_STREQ(table_data.headers_(table_data.headers_.size() - 1).c_str(), "ion_ratio_feature_name");
  EXPECT_EQ(table_data.body_.dimension(0), 10);
  EXPECT_EQ(table_data.body_.dimension(1), 24);
  EXPECT_STREQ(table_data.body_(0, 0).c_str(), "arg-L");
  EXPECT_STREQ(table_data.body_(table_data.body_.dimension(0) - 1, table_data.body_.dimension(1) - 1).c_str(), "");
}

TEST(SessionHandler, setComponentRSDEstimationsTable1)
{
  TestData testData;
  testData.changeSampleType(SampleType::QC);
  const ParameterSet params;
  Filenames filenames_;
  EstimateFeatureRSDs processor;
  processor.process(testData.application_handler.sequenceHandler_.getSequenceSegments().front(), testData.application_handler.sequenceHandler_, params, filenames_);
  SessionHandler session_handler;
  SessionHandler::GenericTableData table_data;
  session_handler.setComponentRSDEstimationsTable(testData.application_handler.sequenceHandler_, table_data);
  EXPECT_EQ(table_data.headers_.size(), 11);
  EXPECT_STREQ(table_data.headers_(0).c_str(), "component_name");
  EXPECT_STREQ(table_data.headers_(table_data.headers_.size() - 1).c_str(), "metaValue_peak_apex_int_u");
  EXPECT_EQ(table_data.body_.dimension(0), 10);
  EXPECT_EQ(table_data.body_.dimension(1), 11);
  EXPECT_STREQ(table_data.body_(0, 0).c_str(), "arg-L.arg-L_1.Heavy");
  EXPECT_STREQ(table_data.body_(table_data.body_.dimension(0) - 1, table_data.body_.dimension(1) - 1).c_str(), "0.000000");
}

TEST(SessionHandler, setComponentGroupRSDEstimationsTable1)
{
  TestData testData;
  testData.changeSampleType(SampleType::QC);
  const ParameterSet params;
  Filenames filenames_;
  EstimateFeatureRSDs processor;
  processor.process(testData.application_handler.sequenceHandler_.getSequenceSegments().front(), testData.application_handler.sequenceHandler_, params, filenames_);
  SessionHandler session_handler;
  SessionHandler::GenericTableData table_data;
  session_handler.setComponentGroupRSDEstimationsTable(testData.application_handler.sequenceHandler_, table_data);
  EXPECT_EQ(table_data.headers_.size(), 24);
  EXPECT_STREQ(table_data.headers_(0).c_str(), "component_group_name");
  EXPECT_STREQ(table_data.headers_(table_data.headers_.size() - 1).c_str(), "ion_ratio_feature_name");
  EXPECT_EQ(table_data.body_.dimension(0), 10);
  EXPECT_EQ(table_data.body_.dimension(1), 24);
  EXPECT_STREQ(table_data.body_(0, 0).c_str(), "arg-L");
  EXPECT_STREQ(table_data.body_(table_data.body_.dimension(0) - 1, table_data.body_.dimension(1) - 1).c_str(), "");
}

TEST(SessionHandler, setComponentBackgroundEstimationsTable1)
{
  TestData testData;
  testData.changeSampleType(SampleType::Blank);
  const ParameterSet params;
  Filenames filenames_;
  EstimateFeatureBackgroundInterferences processor;
  processor.process(testData.application_handler.sequenceHandler_.getSequenceSegments().front(), testData.application_handler.sequenceHandler_, params, filenames_);
  SessionHandler session_handler;
  SessionHandler::GenericTableData table_data;
  session_handler.setComponentBackgroundEstimationsTable(testData.application_handler.sequenceHandler_, table_data);
  EXPECT_EQ(table_data.headers_.size(), 11);
  EXPECT_STREQ(table_data.headers_(0).c_str(), "component_name");
  EXPECT_STREQ(table_data.headers_(table_data.headers_.size() - 1).c_str(), "metaValue_peak_apex_int_u");
  EXPECT_EQ(table_data.body_.dimension(0), 10);
  EXPECT_EQ(table_data.body_.dimension(1), 11);
  EXPECT_STREQ(table_data.body_(0, 0).c_str(), "arg-L.arg-L_1.Heavy");
  EXPECT_STREQ(table_data.body_(table_data.body_.dimension(0) - 1, table_data.body_.dimension(1) - 1).c_str(), "0.000000");
}

TEST(SessionHandler, setComponentGroupBackgroundEstimationsTable1)
{
  TestData testData;
  testData.changeSampleType(SampleType::Blank);
  const ParameterSet params;
  Filenames filenames_;
  EstimateFeatureBackgroundInterferences processor;
  processor.process(testData.application_handler.sequenceHandler_.getSequenceSegments().front(), testData.application_handler.sequenceHandler_, params, filenames_);
  SessionHandler session_handler;
  SessionHandler::GenericTableData table_data;
  session_handler.setComponentGroupBackgroundEstimationsTable(testData.application_handler.sequenceHandler_, table_data);
  EXPECT_EQ(table_data.headers_.size(), 24);
  EXPECT_STREQ(table_data.headers_(0).c_str(), "component_group_name");
  EXPECT_STREQ(table_data.headers_(table_data.headers_.size() - 1).c_str(), "ion_ratio_feature_name");
  EXPECT_EQ(table_data.body_.dimension(0), 10);
  EXPECT_EQ(table_data.body_.dimension(1), 24);
  EXPECT_STREQ(table_data.body_(0, 0).c_str(), "arg-L");
  EXPECT_STREQ(table_data.body_(table_data.body_.dimension(0) - 1, table_data.body_.dimension(1) - 1).c_str(), "");
}

TEST(SessionHandler, sessionHandlerGetters1)
{
  TestData testData(true, false, true);
  SessionHandler session_handler;
  session_handler.setMinimalDataAndFilters(testData.application_handler.sequenceHandler_);
  // Dynamic headers and body
  EXPECT_EQ(session_handler.getInjectionExplorerHeader().size(), 2);
  EXPECT_STREQ(session_handler.getInjectionExplorerHeader()(0).c_str(), "inj#");
  EXPECT_STREQ(session_handler.getInjectionExplorerHeader()(session_handler.getInjectionExplorerHeader().dimension(0) - 1).c_str(), "sample_name");
  EXPECT_EQ(session_handler.getInjectionExplorerBody().dimension(0), 2);
  EXPECT_EQ(session_handler.getInjectionExplorerBody().dimension(1), 2);
  EXPECT_STREQ(session_handler.getInjectionExplorerBody()(0,0).c_str(), "1");
  EXPECT_STREQ(session_handler.getInjectionExplorerBody()(session_handler.getInjectionExplorerBody().dimension(0) - 1,session_handler.getInjectionExplorerBody().dimension(1) - 1).c_str(), "150516_CM1_Level10");
  EXPECT_EQ(session_handler.getTransitionExplorerHeader().size(), 2);
  EXPECT_STREQ(session_handler.getTransitionExplorerHeader()(0).c_str(), "transition_group");
  EXPECT_STREQ(session_handler.getTransitionExplorerHeader()(session_handler.getTransitionExplorerHeader().dimension(0) - 1).c_str(), "transition_name");
  EXPECT_EQ(session_handler.getTransitionExplorerBody().dimension(0), 6);
  EXPECT_EQ(session_handler.getTransitionExplorerBody().dimension(1), 2);
  EXPECT_STREQ(session_handler.getTransitionExplorerBody()(0,0).c_str(), "arg-L");
  EXPECT_STREQ(session_handler.getTransitionExplorerBody()(session_handler.getTransitionExplorerBody().dimension(0) - 1, session_handler.getTransitionExplorerBody().dimension(1) - 1).c_str(), "ser-L.ser-L_2.Light");
  EXPECT_EQ(session_handler.getSpectrumExplorerHeader().size(), 1);
  EXPECT_STREQ(session_handler.getSpectrumExplorerHeader()(0).c_str(), "native_id");
  EXPECT_STREQ(session_handler.getSpectrumExplorerHeader()(session_handler.getSpectrumExplorerHeader().dimension(0) - 1).c_str(), "native_id");
  EXPECT_EQ(session_handler.getSpectrumExplorerBody().dimension(0), 1);
  EXPECT_EQ(session_handler.getSpectrumExplorerBody().dimension(1), 1);
  EXPECT_STREQ(session_handler.getSpectrumExplorerBody()(0, 0).c_str(), "controllerType=0 controllerNumber=1 scan=1");
  EXPECT_STREQ(session_handler.getSpectrumExplorerBody()(session_handler.getSpectrumExplorerBody().dimension(0) - 1, session_handler.getSpectrumExplorerBody().dimension(1) - 1).c_str(), "controllerType=0 controllerNumber=1 scan=1");
  // N selected counts
  EXPECT_EQ(session_handler.getNSelectedSampleNamesTable(), 2);
  EXPECT_EQ(session_handler.getNSelectedSampleNamesPlot(), 0);
  EXPECT_EQ(session_handler.getNSelectedTransitionsTable(), 6);
  EXPECT_EQ(session_handler.getNSelectedTransitionsPlot(), 0);
  EXPECT_EQ(session_handler.getNSelectedFeatureMetaValuesTable(), 0);
  EXPECT_EQ(session_handler.getNSelectedFeatureMetaValuesPlot(), 0);
  // Selected string values
  EXPECT_TRUE(session_handler.getSelectInjectionNamesWorkflow(testData.application_handler.sequenceHandler_) == std::set<std::string>({"150516_CM1_Level10_2_BatchName_1900-01-01_000000", "150516_CM1_Level1_1_BatchName_1900-01-01_000000"}));
  EXPECT_TRUE(session_handler.getSelectSequenceSegmentNamesWorkflow(testData.application_handler.sequenceHandler_) == std::set<std::string>({ "segment1" }));
  EXPECT_TRUE(session_handler.getSelectSampleGroupNamesWorkflow(testData.application_handler.sequenceHandler_) == std::set<std::string>({ "CM" }));
  EXPECT_EQ(session_handler.getSelectSampleNamesTable().size(), 2);
  EXPECT_STREQ(session_handler.getSelectSampleNamesTable()(0).c_str(), "150516_CM1_Level1");
  EXPECT_STREQ(session_handler.getSelectSampleNamesTable()(session_handler.getSelectSampleNamesTable().dimension(0)-1).c_str(), "150516_CM1_Level10");
  EXPECT_EQ(session_handler.getSelectSampleNamesPlot().size(), 2);
  EXPECT_STREQ(session_handler.getSelectSampleNamesPlot()(0).c_str(), "");
  EXPECT_STREQ(session_handler.getSelectSampleNamesPlot()(session_handler.getSelectSampleNamesPlot().dimension(0) - 1).c_str(), "");
  EXPECT_EQ(session_handler.getSelectTransitionsTable().size(), 6);
  EXPECT_STREQ(session_handler.getSelectTransitionsTable()(0).c_str(), "arg-L.arg-L_1.Light");
  EXPECT_STREQ(session_handler.getSelectTransitionsTable()(session_handler.getSelectTransitionsTable().dimension(0) - 1).c_str(), "ser-L.ser-L_2.Light");
  EXPECT_EQ(session_handler.getSelectTransitionGroupsTable().size(), 6);
  EXPECT_STREQ(session_handler.getSelectTransitionGroupsTable()(0).c_str(), "arg-L");
  EXPECT_STREQ(session_handler.getSelectTransitionGroupsTable()(session_handler.getSelectTransitionGroupsTable().dimension(0) - 1).c_str(), "ser-L");
  EXPECT_EQ(session_handler.getSelectTransitionsPlot().size(), 6);
  EXPECT_STREQ(session_handler.getSelectTransitionsPlot()(0).c_str(), "");
  EXPECT_STREQ(session_handler.getSelectTransitionsPlot()(session_handler.getSelectTransitionsPlot().dimension(0) - 1).c_str(), "");
  EXPECT_EQ(session_handler.getSelectTransitionGroupsPlot().size(), 6);
  EXPECT_STREQ(session_handler.getSelectTransitionGroupsPlot()(0).c_str(), "");
  EXPECT_STREQ(session_handler.getSelectTransitionGroupsPlot()(session_handler.getSelectTransitionsPlot().dimension(0) - 1).c_str(), "");
  EXPECT_EQ(session_handler.getSelectFeatureMetaValuesTable().size(), 22);
  EXPECT_STREQ(session_handler.feature_table.body_(0, 0).c_str(), "asymmetry_factor");
  EXPECT_STREQ(session_handler.feature_table.body_(session_handler.feature_table.body_.dimension(0) - 1, 0).c_str(), "absolute_difference");
  EXPECT_EQ(session_handler.getSelectFeatureMetaValuesPlot().size(), 22);
  EXPECT_STREQ(session_handler.getSelectFeatureMetaValuesPlot()(0).c_str(), "");
  EXPECT_STREQ(session_handler.getSelectFeatureMetaValuesPlot()(session_handler.getSelectFeatureMetaValuesPlot().dimension(0) - 1).c_str(), "");
  EXPECT_EQ(session_handler.getSelectSpectrumPlot().size(), 1);
  EXPECT_STREQ(session_handler.getSelectSpectrumPlot()(0).c_str(), "controllerType=0 controllerNumber=1 scan=1");
  EXPECT_STREQ(session_handler.getSelectSpectrumPlot()(session_handler.getSelectSpectrumPlot().dimension(0) - 1).c_str(), "controllerType=0 controllerNumber=1 scan=1");
  // Table filters
  EXPECT_EQ(session_handler.getSequenceTableFilters().size(), 2);
  EXPECT_TRUE(session_handler.getSequenceTableFilters()(0));
  EXPECT_TRUE(session_handler.getSequenceTableFilters()(session_handler.getSequenceTableFilters().dimension(0) - 1));
  EXPECT_EQ(session_handler.getTransitionsTableFilters().size(), 6);
  EXPECT_TRUE(session_handler.getTransitionsTableFilters()(0));
  EXPECT_TRUE(session_handler.getTransitionsTableFilters()(session_handler.getTransitionsTableFilters().dimension(0) - 1));
  EXPECT_EQ(session_handler.getSpectrumTableFilters().size(), 1);
  EXPECT_TRUE(session_handler.getSpectrumTableFilters()(0));
  EXPECT_TRUE(session_handler.getSpectrumTableFilters()(session_handler.getSpectrumTableFilters().dimension(0) - 1));
  SessionHandler::GenericTableData table_data;
  session_handler.setQuantMethodTable(testData.application_handler.sequenceHandler_, table_data);
  EXPECT_EQ(session_handler.getFiltersTable(table_data.body_).size(), 10);
  EXPECT_TRUE(session_handler.getFiltersTable(table_data.body_)(0));
  EXPECT_TRUE(!session_handler.getFiltersTable(table_data.body_)(session_handler.getFiltersTable(table_data.body_).dimension(0) - 1));
  table_data.clear();
  session_handler.setComponentFiltersTable(testData.application_handler.sequenceHandler_, table_data);
  EXPECT_EQ(session_handler.getFiltersTable(table_data.body_).size(), 10);
  EXPECT_TRUE(!session_handler.getFiltersTable(table_data.body_)(0));
  EXPECT_TRUE(session_handler.getFiltersTable(table_data.body_)(session_handler.getFiltersTable(table_data.body_).dimension(0) - 1));
  table_data.clear();
  session_handler.setComponentQCsTable(testData.application_handler.sequenceHandler_, table_data);
  EXPECT_EQ(session_handler.getFiltersTable(table_data.body_).size(), 10);
  EXPECT_TRUE(!session_handler.getFiltersTable(table_data.body_)(0));
  EXPECT_TRUE(session_handler.getFiltersTable(table_data.body_)(session_handler.getFiltersTable(table_data.body_).dimension(0) - 1));
  table_data.clear();
  session_handler.setComponentGroupFiltersTable(testData.application_handler.sequenceHandler_, table_data);
  EXPECT_EQ(session_handler.getGroupFiltersTable(table_data.body_).size(), 10);
  EXPECT_TRUE(session_handler.getGroupFiltersTable(table_data.body_)(0));
  EXPECT_TRUE(!session_handler.getGroupFiltersTable(table_data.body_)(session_handler.getGroupFiltersTable(table_data.body_).dimension(0) - 1));
  table_data.clear();
  session_handler.setComponentGroupQCsTable(testData.application_handler.sequenceHandler_, table_data);
  EXPECT_EQ(session_handler.getGroupFiltersTable(table_data.body_).size(), 10);
  EXPECT_TRUE(session_handler.getGroupFiltersTable(table_data.body_)(0));
  EXPECT_TRUE(!session_handler.getGroupFiltersTable(table_data.body_)(session_handler.getGroupFiltersTable(table_data.body_).dimension(0) - 1));
  table_data.clear();
  session_handler.setComponentRSDFiltersTable(testData.application_handler.sequenceHandler_, table_data);
  EXPECT_EQ(session_handler.getFiltersTable(table_data.body_).size(), 10);
  EXPECT_TRUE(!session_handler.getFiltersTable(table_data.body_)(0));
  EXPECT_TRUE(session_handler.getFiltersTable(table_data.body_)(session_handler.getFiltersTable(table_data.body_).dimension(0) - 1));
  table_data.clear();
  session_handler.setComponentRSDQCsTable(testData.application_handler.sequenceHandler_, table_data);
  EXPECT_EQ(session_handler.getFiltersTable(table_data.body_).size(), 10);
  EXPECT_TRUE(!session_handler.getFiltersTable(table_data.body_)(0));
  EXPECT_TRUE(session_handler.getFiltersTable(table_data.body_)(session_handler.getFiltersTable(table_data.body_).dimension(0) - 1));
  table_data.clear();
  session_handler.setComponentGroupRSDFiltersTable(testData.application_handler.sequenceHandler_, table_data);
  EXPECT_EQ(session_handler.getGroupFiltersTable(table_data.body_).size(), 10);
  EXPECT_TRUE(session_handler.getGroupFiltersTable(table_data.body_)(0));
  EXPECT_TRUE(!session_handler.getGroupFiltersTable(table_data.body_)(session_handler.getGroupFiltersTable(table_data.body_).dimension(0) - 1));
  table_data.clear();
  session_handler.setComponentGroupRSDQCsTable(testData.application_handler.sequenceHandler_, table_data);
  EXPECT_EQ(session_handler.getGroupFiltersTable(table_data.body_).size(), 10);
  EXPECT_TRUE(session_handler.getGroupFiltersTable(table_data.body_)(0));
  EXPECT_TRUE(!session_handler.getGroupFiltersTable(table_data.body_)(session_handler.getGroupFiltersTable(table_data.body_).dimension(0) - 1));
  table_data.clear();
  session_handler.setComponentBackgroundFiltersTable(testData.application_handler.sequenceHandler_, table_data);
  EXPECT_EQ(session_handler.getFiltersTable(table_data.body_).size(), 10);
  EXPECT_TRUE(!session_handler.getFiltersTable(table_data.body_)(0));
  EXPECT_TRUE(session_handler.getFiltersTable(table_data.body_)(session_handler.getFiltersTable(table_data.body_).dimension(0) - 1));
  table_data.clear();
  session_handler.setComponentBackgroundQCsTable(testData.application_handler.sequenceHandler_, table_data);
  EXPECT_EQ(session_handler.getFiltersTable(table_data.body_).size(), 10);
  EXPECT_TRUE(!session_handler.getFiltersTable(table_data.body_)(0));
  EXPECT_TRUE(session_handler.getFiltersTable(table_data.body_)(session_handler.getFiltersTable(table_data.body_).dimension(0) - 1));
  table_data.clear();
  session_handler.setComponentGroupBackgroundFiltersTable(testData.application_handler.sequenceHandler_, table_data);
  EXPECT_EQ(session_handler.getGroupFiltersTable(table_data.body_).size(), 10);
  EXPECT_TRUE(session_handler.getGroupFiltersTable(table_data.body_)(0));
  EXPECT_TRUE(!session_handler.getGroupFiltersTable(table_data.body_)(session_handler.getGroupFiltersTable(table_data.body_).dimension(0) - 1));
  table_data.clear();
  session_handler.setComponentGroupBackgroundQCsTable(testData.application_handler.sequenceHandler_, table_data);
  EXPECT_EQ(session_handler.getGroupFiltersTable(table_data.body_).size(), 10);
  EXPECT_TRUE(session_handler.getGroupFiltersTable(table_data.body_)(0));
  EXPECT_TRUE(!session_handler.getGroupFiltersTable(table_data.body_)(session_handler.getGroupFiltersTable(table_data.body_).dimension(0) - 1));

  testData.changeSampleType(SampleType::QC);
  const ParameterSet params;
  Filenames filenames_;
  EstimateFeatureRSDs estimateFeatureRSDs;
  estimateFeatureRSDs.process(testData.application_handler.sequenceHandler_.getSequenceSegments().front(), testData.application_handler.sequenceHandler_, params, filenames_);
  table_data.clear();
  session_handler.setComponentRSDEstimationsTable(testData.application_handler.sequenceHandler_, table_data);
  EXPECT_EQ(session_handler.getFiltersTable(table_data.body_).size(), 10);
  EXPECT_TRUE(!session_handler.getFiltersTable(table_data.body_)(0));
  EXPECT_TRUE(session_handler.getFiltersTable(table_data.body_)(session_handler.getFiltersTable(table_data.body_).dimension(0) - 1));
  table_data.clear();
  session_handler.setComponentGroupRSDEstimationsTable(testData.application_handler.sequenceHandler_, table_data);
  EXPECT_EQ(session_handler.getGroupFiltersTable(table_data.body_).size(), 10);
  EXPECT_TRUE(session_handler.getGroupFiltersTable(table_data.body_)(0));
  EXPECT_TRUE(!session_handler.getGroupFiltersTable(table_data.body_)(session_handler.getGroupFiltersTable(table_data.body_).dimension(0) - 1));

  testData.changeSampleType(SampleType::Blank);
  EstimateFeatureBackgroundInterferences estimateFeatureBackgroundInterferences;
  estimateFeatureBackgroundInterferences.process(testData.application_handler.sequenceHandler_.getSequenceSegments().front(), testData.application_handler.sequenceHandler_, params, filenames_);
  table_data.clear();
  session_handler.setComponentBackgroundEstimationsTable(testData.application_handler.sequenceHandler_, table_data);
  EXPECT_EQ(session_handler.getFiltersTable(table_data.body_).size(), 10);
  EXPECT_TRUE(!session_handler.getFiltersTable(table_data.body_)(0));
  EXPECT_TRUE(session_handler.getFiltersTable(table_data.body_)(session_handler.getFiltersTable(table_data.body_).dimension(0) - 1));
  table_data.clear();
  session_handler.setComponentGroupBackgroundEstimationsTable(testData.application_handler.sequenceHandler_, table_data);
  EXPECT_EQ(session_handler.getGroupFiltersTable(table_data.body_).size(), 10);
  EXPECT_TRUE(session_handler.getGroupFiltersTable(table_data.body_)(0));
  EXPECT_TRUE(!session_handler.getGroupFiltersTable(table_data.body_)(session_handler.getGroupFiltersTable(table_data.body_).dimension(0) - 1));
}

TEST(SessionHandler, setFeatureTable1)
{
  TestData testData(true, true);
  SessionHandler session_handler;
  SessionHandler::GenericTableData table_data;
  session_handler.setFeatureTable(testData.application_handler.sequenceHandler_, table_data);
  EXPECT_EQ(table_data.headers_.size(), 23);
  EXPECT_STREQ(table_data.headers_(0).c_str(), "sample_name");
  EXPECT_STREQ(table_data.headers_(table_data.headers_.size() - 1).c_str(), "used_");
  EXPECT_EQ(table_data.body_.dimension(0), 13);
  EXPECT_EQ(table_data.body_.dimension(1), 23);
  EXPECT_STREQ(table_data.body_(0, 0).c_str(), "150516_CM1_Level1");
  EXPECT_STREQ(table_data.body_(table_data.body_.dimension(0) - 1, table_data.body_.dimension(1) - 1).c_str(), "true");
  session_handler.setMinimalDataAndFilters(testData.application_handler.sequenceHandler_);
  session_handler.setFeatureTable(testData.application_handler.sequenceHandler_, table_data);
  EXPECT_EQ(table_data.headers_.size(), 23);
  EXPECT_STREQ(table_data.headers_(0).c_str(), "sample_name");
  EXPECT_STREQ(table_data.headers_(table_data.headers_.size() - 1).c_str(), "used_");
  EXPECT_EQ(table_data.body_.dimension(0), 13);
  EXPECT_EQ(table_data.body_.dimension(1), 23);
  EXPECT_STREQ(table_data.body_(0, 0).c_str(), "150516_CM1_Level1");
  EXPECT_STREQ(table_data.body_(table_data.body_.dimension(0) - 1, table_data.body_.dimension(1) - 1).c_str(), "true");
}

TEST(SessionHandler, setFeatureMatrix1)
{
  TestData testData;
  SessionHandler session_handler;
  session_handler.setFeatureMatrix(testData.application_handler.sequenceHandler_);
}
TEST(SessionHandler, getSpectrumScatterPlot1)
{
  TestData testData;
  SessionHandler session_handler;
  SessionHandler::GraphVizData result;
  const std::pair<float, float> range = std::make_pair(0, 1800);
  const std::set<std::string> sample_names;
  const std::set<std::string> component_names;
  session_handler.getChromatogramScatterPlot(testData.application_handler.sequenceHandler_, result, range, sample_names, component_names);
  EXPECT_FALSE(result.points_overflow_);
}
TEST(SessionHandler, setSpectrumScatterPlot1)
{
  TestData testData;
  SessionHandler session_handler;
  SessionHandler::GraphVizData result;
  const std::pair<float, float> range = std::make_pair(0, 2000);
  const std::set<std::string> sample_names;
  const std::set<std::string> scan_names;
  const std::set<std::string> component_group_names;
  session_handler.getSpectrumScatterPlot(testData.application_handler.sequenceHandler_, result, range, sample_names, scan_names, component_group_names);
  EXPECT_FALSE(result.points_overflow_);
}
TEST(SessionHandler, setCalibratorsScatterLinePlot1)
{
  TestData testData;
  SessionHandler session_handler;
  SessionHandler::CalibrationData calibrator_data;
  session_handler.setCalibratorsScatterLinePlot(testData.application_handler.sequenceHandler_, calibrator_data);
  EXPECT_EQ(calibrator_data.conc_fit_data.size(), 0);
  EXPECT_EQ(calibrator_data.matching_points_.concentrations_.size(), 0);
  EXPECT_EQ(calibrator_data.matching_points_.features_.size(), 0);
  EXPECT_EQ(calibrator_data.outlier_points_.concentrations_.size(), 0);
  EXPECT_EQ(calibrator_data.outlier_points_.features_.size(), 0);
  EXPECT_EQ(calibrator_data.excluded_points_.concentrations_.size(), 0);
  EXPECT_EQ(calibrator_data.excluded_points_.features_.size(), 0);  EXPECT_EQ(calibrator_data.series_names.size(), 0);
  EXPECT_EQ(calibrator_data.x_axis_title, std::string(""));
  EXPECT_EQ(calibrator_data.y_axis_title, std::string(""));
}
TEST(SessionHandler, getHeatMap)
{
  TestData testData;
  SessionHandler session_handler;
  SessionHandler::HeatMapData result;
  std::string feature_name = "calculated_concentration";
  session_handler.getHeatMap(testData.application_handler.sequenceHandler_, result, feature_name);
  EXPECT_EQ(result.feat_heatmap_col_labels.size(), 0);
  EXPECT_EQ(result.feat_heatmap_data.size(), 0);
  EXPECT_EQ(result.feat_heatmap_row_labels.size(), 0);
  EXPECT_STREQ(result.feat_heatmap_x_axis_title.c_str(), "Injections");
  EXPECT_STREQ(result.feat_heatmap_y_axis_title.c_str(), "Transitions");
  EXPECT_NEAR(result.feat_value_max_, std::numeric_limits<float>::min(), 1e-6);
  EXPECT_NEAR(result.feat_value_min_, std::numeric_limits<float>::max(), 1e-6);
  EXPECT_STREQ(result.selected_feature_.c_str(), "calculated_concentration");
  EXPECT_EQ(result.selected_sample_names_.size(), 0);
  EXPECT_EQ(result.selected_transitions_.size(), 0);
  EXPECT_EQ(result.selected_transition_groups_.size(), 0);
}

TEST(SessionHandler, GraphVizData)
{
  SessionHandler::GraphVizData graph_viz_data;
  EXPECT_TRUE(graph_viz_data.series_names_area_.empty());
  EXPECT_TRUE(graph_viz_data.x_data_area_.empty());
  EXPECT_TRUE(graph_viz_data.y_data_area_.empty());
  EXPECT_TRUE(graph_viz_data.series_names_scatter_.empty());
  EXPECT_TRUE(graph_viz_data.x_data_scatter_.empty());
  EXPECT_TRUE(graph_viz_data.y_data_scatter_.empty());
  EXPECT_STREQ(graph_viz_data.x_axis_title_.c_str(), "");
  EXPECT_STREQ(graph_viz_data.y_axis_title_.c_str(), "");
  EXPECT_TRUE(graph_viz_data.z_data_area_.empty());
  EXPECT_FALSE(graph_viz_data.z_axis_title_);
  EXPECT_NEAR(graph_viz_data.x_min_, 0.0f, 1e-6);
  EXPECT_NEAR(graph_viz_data.x_max_, 0.0f, 1e-6);
  EXPECT_NEAR(graph_viz_data.y_min_, 0.0f, 1e-6);
  EXPECT_NEAR(graph_viz_data.y_max_, 0.0f, 1e-6);
  EXPECT_FALSE(graph_viz_data.points_overflow_);
  EXPECT_EQ(graph_viz_data.nb_points_, 0);
  EXPECT_EQ(graph_viz_data.max_nb_points_, 0);

  graph_viz_data.reset("x_axis", "y_axis", "z_axis", 13);
  EXPECT_TRUE(graph_viz_data.series_names_area_.empty());
  EXPECT_TRUE(graph_viz_data.x_data_area_.empty());
  EXPECT_TRUE(graph_viz_data.y_data_area_.empty());
  EXPECT_TRUE(graph_viz_data.series_names_scatter_.empty());
  EXPECT_TRUE(graph_viz_data.x_data_scatter_.empty());
  EXPECT_TRUE(graph_viz_data.y_data_scatter_.empty());
  EXPECT_STREQ(graph_viz_data.x_axis_title_.c_str(), "x_axis");
  EXPECT_STREQ(graph_viz_data.y_axis_title_.c_str(), "y_axis");
  EXPECT_TRUE(graph_viz_data.z_data_area_.empty());
  EXPECT_STREQ(graph_viz_data.z_axis_title_->c_str(), "z_axis");
  EXPECT_NEAR(graph_viz_data.x_min_, 1000000.0f, 1e-6);
  EXPECT_NEAR(graph_viz_data.x_max_, 0.0f, 1e-6);
  EXPECT_NEAR(graph_viz_data.y_min_, 1000000.0f, 1e-6);
  EXPECT_NEAR(graph_viz_data.y_max_, 0.0f, 1e-6);
  EXPECT_FALSE(graph_viz_data.points_overflow_);
  EXPECT_EQ(graph_viz_data.nb_points_, 0);
  EXPECT_EQ(graph_viz_data.max_nb_points_, 13);

  std::vector<float> data_x_1 = { 1.0f, 10.0f, 5.0f };
  std::vector<float> data_y_1 = { 101.0f, 110.0f, 105.0f };
  std::vector<float> data_x_2 = { 201.0f, 210.0f, 205.0f };
  std::vector<float> data_y_2 = { 301.0f, 310.0f, 305.0f };

  EXPECT_TRUE(graph_viz_data.addData(data_x_1, data_y_1, "data1"));
  EXPECT_TRUE(graph_viz_data.addData(data_x_2, data_y_2, "data2"));
  EXPECT_TRUE(std::equal(graph_viz_data.series_names_area_.begin(), graph_viz_data.series_names_area_.end(), 
              std::vector<std::string>({ "data1", "data2" }).begin()));
  EXPECT_TRUE(std::equal(graph_viz_data.x_data_area_.begin(), graph_viz_data.x_data_area_.end(),
    std::vector<std::vector<float>>({ { 1.0f, 10.0f, 5.0f }, { 201.0f, 210.0f, 205.0f } }).begin()));
  EXPECT_TRUE(std::equal(graph_viz_data.y_data_area_.begin(), graph_viz_data.y_data_area_.end(),
    std::vector<std::vector<float>>({ { 101.0f, 110.0f, 105.0f }, { 301.0f, 310.0f, 305.0f } }).begin()));
  EXPECT_TRUE(graph_viz_data.series_names_scatter_.empty());
  EXPECT_TRUE(graph_viz_data.x_data_scatter_.empty());
  EXPECT_TRUE(graph_viz_data.y_data_scatter_.empty());
  EXPECT_STREQ(graph_viz_data.x_axis_title_.c_str(), "x_axis");
  EXPECT_STREQ(graph_viz_data.y_axis_title_.c_str(), "y_axis");
  EXPECT_TRUE(graph_viz_data.z_data_area_.empty());
  EXPECT_STREQ(graph_viz_data.z_axis_title_->c_str(), "z_axis");
  EXPECT_NEAR(graph_viz_data.x_min_, 1.0f, 1e-6);
  EXPECT_NEAR(graph_viz_data.x_max_, 210.0f, 1e-6);
  EXPECT_NEAR(graph_viz_data.y_min_, 101.0f, 1e-6);
  EXPECT_NEAR(graph_viz_data.y_max_, 310.0f, 1e-6);
  EXPECT_FALSE(graph_viz_data.points_overflow_);
  EXPECT_EQ(graph_viz_data.nb_points_, 6);
  EXPECT_EQ(graph_viz_data.max_nb_points_, 13);

  std::vector<float> scatter_data_x_1 = { 401.0f, 410.0f, 405.0f };
  std::vector<float> scatter_data_y_1 = { 501.0f, 510.0f, 505.0f };
  std::vector<float> scatter_data_x_2 = { 601.0f, 610.0f, 605.0f };
  std::vector<float> scatter_data_y_2 = { 701.0f, 710.0f, 705.0f };
  EXPECT_TRUE(graph_viz_data.addScatterData(scatter_data_x_1, scatter_data_y_1, "scatter_data1"));
  EXPECT_TRUE(graph_viz_data.addScatterData(scatter_data_x_2, scatter_data_y_2, "scatter_data2"));
  EXPECT_TRUE(std::equal(graph_viz_data.series_names_area_.begin(), graph_viz_data.series_names_area_.end(),
    std::vector<std::string>({ "data1", "data2" }).begin()));
  EXPECT_TRUE(std::equal(graph_viz_data.x_data_area_.begin(), graph_viz_data.x_data_area_.end(),
    std::vector<std::vector<float>>({ { 1.0f, 10.0f, 5.0f }, { 201.0f, 210.0f, 205.0f } }).begin()));
  EXPECT_TRUE(std::equal(graph_viz_data.y_data_area_.begin(), graph_viz_data.y_data_area_.end(),
    std::vector<std::vector<float>>({ { 101.0f, 110.0f, 105.0f }, { 301.0f, 310.0f, 305.0f } }).begin()));
  EXPECT_STREQ(graph_viz_data.x_axis_title_.c_str(), "x_axis");
  EXPECT_STREQ(graph_viz_data.y_axis_title_.c_str(), "y_axis");
  EXPECT_TRUE(graph_viz_data.z_data_area_.empty());
  EXPECT_STREQ(graph_viz_data.z_axis_title_->c_str(), "z_axis");
  EXPECT_TRUE(std::equal(graph_viz_data.series_names_scatter_.begin(), graph_viz_data.series_names_scatter_.end(),
    std::vector<std::string>({ "scatter_data1", "scatter_data2" }).begin()));
  EXPECT_TRUE(std::equal(graph_viz_data.x_data_scatter_.begin(), graph_viz_data.x_data_scatter_.end(),
    std::vector<std::vector<float>>({ { 401.0f, 410.0f, 405.0f }, { 601.0f, 610.0f, 605.0f } }).begin()));
  EXPECT_TRUE(std::equal(graph_viz_data.y_data_scatter_.begin(), graph_viz_data.y_data_scatter_.end(),
    std::vector<std::vector<float>>({ { 501.0f, 510.0f, 505.0f }, { 701.0f, 710.0f, 705.0f } }).begin()));
  EXPECT_NEAR(graph_viz_data.x_min_, 1.0f, 1e-6);
  EXPECT_NEAR(graph_viz_data.x_max_, 610.0f, 1e-6);
  EXPECT_NEAR(graph_viz_data.y_min_, 101.0f, 1e-6);
  EXPECT_NEAR(graph_viz_data.y_max_, 710.0f, 1e-6);
  EXPECT_FALSE(graph_viz_data.points_overflow_);
  EXPECT_EQ(graph_viz_data.nb_points_, 12);
  EXPECT_EQ(graph_viz_data.max_nb_points_, 13);

  // overflow
  EXPECT_FALSE(graph_viz_data.addScatterData(scatter_data_x_1, scatter_data_y_1, "scatter_data1"));
  EXPECT_FALSE(graph_viz_data.addScatterData(scatter_data_x_2, scatter_data_y_2, "scatter_data2"));
  EXPECT_TRUE(std::equal(graph_viz_data.series_names_area_.begin(), graph_viz_data.series_names_area_.end(),
    std::vector<std::string>({ "data1", "data2" }).begin()));
  EXPECT_TRUE(std::equal(graph_viz_data.x_data_area_.begin(), graph_viz_data.x_data_area_.end(),
    std::vector<std::vector<float>>({ { 1.0f, 10.0f, 5.0f }, { 201.0f, 210.0f, 205.0f } }).begin()));
  EXPECT_TRUE(std::equal(graph_viz_data.y_data_area_.begin(), graph_viz_data.y_data_area_.end(),
    std::vector<std::vector<float>>({ { 101.0f, 110.0f, 105.0f }, { 301.0f, 310.0f, 305.0f } }).begin()));
  EXPECT_STREQ(graph_viz_data.x_axis_title_.c_str(), "x_axis");
  EXPECT_STREQ(graph_viz_data.y_axis_title_.c_str(), "y_axis");
  EXPECT_TRUE(graph_viz_data.z_data_area_.empty());
  EXPECT_STREQ(graph_viz_data.z_axis_title_->c_str(), "z_axis");
  EXPECT_TRUE(std::equal(graph_viz_data.series_names_scatter_.begin(), graph_viz_data.series_names_scatter_.end(),
    std::vector<std::string>({ "scatter_data1", "scatter_data2" }).begin()));
  EXPECT_TRUE(std::equal(graph_viz_data.x_data_scatter_.begin(), graph_viz_data.x_data_scatter_.end(),
    std::vector<std::vector<float>>({ { 401.0f, 410.0f, 405.0f }, { 601.0f, 610.0f, 605.0f } }).begin()));
  EXPECT_TRUE(std::equal(graph_viz_data.y_data_scatter_.begin(), graph_viz_data.y_data_scatter_.end(),
    std::vector<std::vector<float>>({ { 501.0f, 510.0f, 505.0f }, { 701.0f, 710.0f, 705.0f } }).begin()));
  EXPECT_NEAR(graph_viz_data.x_min_, 1.0f, 1e-6);
  EXPECT_NEAR(graph_viz_data.x_max_, 610.0f, 1e-6);
  EXPECT_NEAR(graph_viz_data.y_min_, 101.0f, 1e-6);
  EXPECT_NEAR(graph_viz_data.y_max_, 710.0f, 1e-6);
  EXPECT_TRUE(graph_viz_data.points_overflow_);
  EXPECT_EQ(graph_viz_data.nb_points_, 12);
  EXPECT_EQ(graph_viz_data.max_nb_points_, 13);
}
