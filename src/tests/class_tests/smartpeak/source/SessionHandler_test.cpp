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
// $Maintainer: Douglas McCloskey $
// $Authors: Douglas McCloskey $
// --------------------------------------------------------------------------

#include <SmartPeak/test_config.h>
#define BOOST_TEST_MODULE SessionHandler test suite
#include <boost/test/included/unit_test.hpp>
#include <SmartPeak/core/SessionHandler.h>
#include <SmartPeak/core/SequenceProcessor.h>

using namespace SmartPeak;
using namespace std;

BOOST_AUTO_TEST_SUITE(SessionHandler1)

struct TestData {
  TestData(const bool& load_sequence = true, const bool& load_features = false, const bool& load_raw_data = false){
    // Handle the filenames
    const std::string pathname = SMARTPEAK_GET_TEST_DATA_PATH("workflow_csv_files");
    // Load the sequence
    if (load_sequence) {
      Filenames filenames_ = Filenames::getDefaultStaticFilenames(pathname);
      CreateSequence cs(sequenceHandler);
      cs.filenames_ = filenames_;
      cs.delimiter = ",";
      cs.checkConsistency = false;
      cs.process();
    }
    // Load the picked features
    if (load_features) {
      LoadFeatures loadFeatures;
      for (auto& injection : sequenceHandler.getSequence()) {
        Filenames filenames_ = Filenames::getDefaultDynamicFilenames(
          pathname + "/mzML",
          pathname + "/features",
          pathname + "/features",
          injection.getMetaData().getFilename(),
          injection.getMetaData().getInjectionName(),
          injection.getMetaData().getInjectionName(),
          injection.getMetaData().getSampleGroupName(),
          injection.getMetaData().getSampleGroupName());
        loadFeatures.process(injection.getRawData(), {}, filenames_);
      }
    }
    // Load the raw data
    if (load_raw_data) {
      std::map<std::string, std::vector<std::map<std::string, std::string>>> params;
      params.emplace("mzML", std::vector<std::map<std::string, std::string>>());
      params.emplace("ChromatogramExtractor", std::vector<std::map<std::string, std::string>>());
      LoadRawData loadRawData;
      for (auto& injection : sequenceHandler.getSequence()) {
        Filenames filenames_ = Filenames::getDefaultDynamicFilenames(
          pathname + "/mzML",
          pathname + "/features",
          pathname + "/features",
          injection.getMetaData().getFilename(),
          injection.getMetaData().getSampleName(),
          injection.getMetaData().getSampleName(),
          injection.getMetaData().getSampleGroupName(),
          injection.getMetaData().getSampleGroupName());
        loadRawData.process(injection.getRawData(), params, filenames_);
      }
    }
  }
  void changeSampleType(const SampleType& sample_type) {
    for (auto& injection : sequenceHandler.getSequence()) {
      injection.getMetaData().setSampleType(sample_type);
    }
  }
  SequenceHandler sequenceHandler;
};

BOOST_AUTO_TEST_CASE(setSequenceTable1)
{
  TestData testData;
  SessionHandler session_handler;
  session_handler.setSequenceTable(testData.sequenceHandler);
  BOOST_CHECK_EQUAL(session_handler.sequence_table_headers.size(), 11);
  BOOST_CHECK_EQUAL(session_handler.sequence_table_headers(0), "inj#");
  BOOST_CHECK_EQUAL(session_handler.sequence_table_headers(session_handler.sequence_table_headers.size() - 1), "acquisition_date_and_time");
  BOOST_CHECK_EQUAL(session_handler.sequence_table_body.dimension(0), 2);
  BOOST_CHECK_EQUAL(session_handler.sequence_table_body.dimension(1), 11);
  BOOST_CHECK_EQUAL(session_handler.sequence_table_body(0, 0), "1");
  BOOST_CHECK_EQUAL(session_handler.sequence_table_body(session_handler.sequence_table_body.dimension(0) - 1, session_handler.sequence_table_body.dimension(1) - 1), "1900-01-01_000000");

  session_handler.setInjectionExplorer();
  BOOST_CHECK_EQUAL(session_handler.injection_explorer_checkbox_headers.size(), 3);
  BOOST_CHECK_EQUAL(session_handler.injection_explorer_checkbox_headers(0), "workflow");
  BOOST_CHECK_EQUAL(session_handler.injection_explorer_checkbox_headers(session_handler.injection_explorer_checkbox_headers.size() - 1), "table");
  BOOST_CHECK_EQUAL(session_handler.injection_explorer_checkbox_body.dimension(0), 2);
  BOOST_CHECK_EQUAL(session_handler.injection_explorer_checkbox_body.dimension(1), 3);
  BOOST_CHECK(session_handler.injection_explorer_checkbox_body(0, 0));
  BOOST_CHECK(!session_handler.injection_explorer_checkbox_body(1, 1));
  BOOST_CHECK(session_handler.injection_explorer_checkbox_body(session_handler.injection_explorer_checkbox_body.dimension(0) - 1, session_handler.injection_explorer_checkbox_body.dimension(1) - 1));
}

BOOST_AUTO_TEST_CASE(setTransitionsTable1)
{
  TestData testData;
  SessionHandler session_handler; 
  session_handler.setTransitionsTable(testData.sequenceHandler);
  BOOST_CHECK_EQUAL(session_handler.transitions_table_headers.size(), 9);
  BOOST_CHECK_EQUAL(session_handler.transitions_table_headers(0), "transition_group");
  BOOST_CHECK_EQUAL(session_handler.transitions_table_headers(session_handler.transitions_table_headers.size() - 1), "detecting_transition");
  BOOST_CHECK_EQUAL(session_handler.transitions_table_body.dimension(0), 6);
  BOOST_CHECK_EQUAL(session_handler.transitions_table_body.dimension(1), 9);
  BOOST_CHECK_EQUAL(session_handler.transitions_table_body(0, 0), "arg-L");
  BOOST_CHECK_EQUAL(session_handler.transitions_table_body(session_handler.transitions_table_body.dimension(0) - 1, session_handler.transitions_table_body.dimension(1) - 1), "1");

  session_handler.setTransitionExplorer();
  BOOST_CHECK_EQUAL(session_handler.transition_explorer_checkbox_headers.size(), 2);
  BOOST_CHECK_EQUAL(session_handler.transition_explorer_checkbox_headers(0), "plot");
  BOOST_CHECK_EQUAL(session_handler.transition_explorer_checkbox_headers(session_handler.transition_explorer_checkbox_headers.size() - 1), "table");
  BOOST_CHECK_EQUAL(session_handler.transition_explorer_checkbox_body.dimension(0), 6);
  BOOST_CHECK_EQUAL(session_handler.transition_explorer_checkbox_body.dimension(1), 2);
  BOOST_CHECK(session_handler.transition_explorer_checkbox_body(0, 0));
  BOOST_CHECK(!session_handler.transition_explorer_checkbox_body(1, 0));
  BOOST_CHECK(session_handler.transition_explorer_checkbox_body(session_handler.transition_explorer_checkbox_body.dimension(0) - 1, session_handler.transition_explorer_checkbox_body.dimension(1) - 1));
}

BOOST_AUTO_TEST_CASE(setFeatureExplorer1)
{
  TestData testData;
  SessionHandler session_handler; 
  session_handler.setFeatureExplorer();
  BOOST_CHECK_EQUAL(session_handler.feature_explorer_headers.size(), 1);
  BOOST_CHECK_EQUAL(session_handler.feature_explorer_headers(0), "name");
  BOOST_CHECK_EQUAL(session_handler.feature_explorer_body.dimension(0), 20);
  BOOST_CHECK_EQUAL(session_handler.feature_explorer_body.dimension(1), 1);
  BOOST_CHECK_EQUAL(session_handler.feature_explorer_body(0, 0), "asymmetry_factor");
  BOOST_CHECK_EQUAL(session_handler.feature_explorer_body(session_handler.feature_explorer_body.dimension(0) - 1, session_handler.feature_explorer_body.dimension(1) - 1), "mz_error_Da");
  BOOST_CHECK_EQUAL(session_handler.feature_explorer_checkbox_headers.size(), 2);
  BOOST_CHECK_EQUAL(session_handler.feature_explorer_checkbox_headers(0), "plot");
  BOOST_CHECK_EQUAL(session_handler.feature_explorer_checkbox_headers(session_handler.feature_explorer_checkbox_headers.size() - 1), "table");
  BOOST_CHECK_EQUAL(session_handler.feature_explorer_checkbox_body.dimension(0), 20);
  BOOST_CHECK_EQUAL(session_handler.feature_explorer_checkbox_body.dimension(1), 2);
  BOOST_CHECK(!session_handler.feature_explorer_checkbox_body(0, 0));
  BOOST_CHECK(session_handler.feature_explorer_checkbox_body(2, 0));
  BOOST_CHECK(session_handler.feature_explorer_checkbox_body(session_handler.feature_explorer_checkbox_body.dimension(0) - 1, session_handler.feature_explorer_checkbox_body.dimension(1) - 1));
}

BOOST_AUTO_TEST_CASE(setSpectrumExplorer1)
{
  TestData testData(true, false, true);
  SessionHandler session_handler;
  session_handler.setSpectrumTable(testData.sequenceHandler);
  BOOST_CHECK_EQUAL(session_handler.spectrum_table_headers.size(), 8);
  BOOST_CHECK_EQUAL(session_handler.spectrum_table_headers(0), "native_id");
  BOOST_CHECK_EQUAL(session_handler.spectrum_table_headers(session_handler.spectrum_table_headers.size() - 1), "highest observed m/z");
  BOOST_CHECK_EQUAL(session_handler.spectrum_table_body.dimension(0), 1);
  BOOST_CHECK_EQUAL(session_handler.spectrum_table_body.dimension(1), 8);
  BOOST_CHECK_EQUAL(session_handler.spectrum_table_body(0, 0), "controllerType=0 controllerNumber=1 scan=1");
  BOOST_CHECK_EQUAL(session_handler.spectrum_table_body(session_handler.spectrum_table_body.dimension(0) - 1, session_handler.spectrum_table_body.dimension(1) - 1), "209.166053007435011");

  session_handler.setSpectrumExplorer();
  BOOST_CHECK_EQUAL(session_handler.spectrum_explorer_checkbox_headers.size(), 2);
  BOOST_CHECK_EQUAL(session_handler.spectrum_explorer_checkbox_headers(0), "plot");
  BOOST_CHECK_EQUAL(session_handler.spectrum_explorer_checkbox_headers(session_handler.spectrum_explorer_checkbox_headers.size() - 1), "table");
  BOOST_CHECK_EQUAL(session_handler.spectrum_explorer_checkbox_body.dimension(0), 1);
  BOOST_CHECK_EQUAL(session_handler.spectrum_explorer_checkbox_body.dimension(1), 2);
  BOOST_CHECK(session_handler.spectrum_explorer_checkbox_body(0, 0));
  BOOST_CHECK(session_handler.spectrum_explorer_checkbox_body(1, 0));
  BOOST_CHECK(session_handler.spectrum_explorer_checkbox_body(session_handler.spectrum_explorer_checkbox_body.dimension(0) - 1, session_handler.spectrum_explorer_checkbox_body.dimension(1) - 1));
}

BOOST_AUTO_TEST_CASE(setWorkflowTable1)
{
  TestData testData;
  //SessionHandler session_handler; session_handler.setWorkflowTable();
}

BOOST_AUTO_TEST_CASE(setParametersTable1)
{
  TestData testData;
  SessionHandler session_handler; 
  session_handler.setParametersTable(testData.sequenceHandler);
  BOOST_CHECK_EQUAL(session_handler.parameters_table_headers.size(), 4);
  BOOST_CHECK_EQUAL(session_handler.parameters_table_headers(0), "function");
  BOOST_CHECK_EQUAL(session_handler.parameters_table_headers(session_handler.parameters_table_headers.size() - 1), "value");
  BOOST_CHECK_EQUAL(session_handler.parameters_table_body.dimension(0), 106);
  BOOST_CHECK_EQUAL(session_handler.parameters_table_body.dimension(1), 4);
  BOOST_CHECK_EQUAL(session_handler.parameters_table_body(0, 0), "AbsoluteQuantitation");
  BOOST_CHECK_EQUAL(session_handler.parameters_table_body(session_handler.parameters_table_body.dimension(0) - 1, session_handler.parameters_table_body.dimension(1) - 1), "TRUE");
}

BOOST_AUTO_TEST_CASE(setQuantMethodTable1)
{
  TestData testData;
  SessionHandler session_handler; 
  session_handler.setQuantMethodTable(testData.sequenceHandler);
  BOOST_CHECK_EQUAL(session_handler.quant_method_table_headers.size(), 20);
  BOOST_CHECK_EQUAL(session_handler.quant_method_table_headers(0), "component_name");
  BOOST_CHECK_EQUAL(session_handler.quant_method_table_headers(session_handler.quant_method_table_headers.size() - 1), "transformation_model_param_y_datum_max");
  BOOST_CHECK_EQUAL(session_handler.quant_method_table_body.dimension(0), 10);
  BOOST_CHECK_EQUAL(session_handler.quant_method_table_body.dimension(1), 20);
  BOOST_CHECK_EQUAL(session_handler.quant_method_table_body(0, 0), "arg-L.arg-L_1.Light");
  BOOST_CHECK_EQUAL(session_handler.quant_method_table_body(session_handler.quant_method_table_body.dimension(0) - 1, session_handler.quant_method_table_body.dimension(1) - 1), "1000000000000000.000000");

}

BOOST_AUTO_TEST_CASE(setStdsConcsTable1)
{
  TestData testData;
  SessionHandler session_handler; 
  session_handler.setStdsConcsTable(testData.sequenceHandler);
  BOOST_CHECK_EQUAL(session_handler.stds_concs_table_headers.size(), 7);
  BOOST_CHECK_EQUAL(session_handler.stds_concs_table_headers(0), "sample_name");
  BOOST_CHECK_EQUAL(session_handler.stds_concs_table_headers(session_handler.stds_concs_table_headers.size() - 1), "dilution_factor");
  BOOST_CHECK_EQUAL(session_handler.stds_concs_table_body.dimension(0), 8);
  BOOST_CHECK_EQUAL(session_handler.stds_concs_table_body.dimension(1), 7);
  BOOST_CHECK_EQUAL(session_handler.stds_concs_table_body(0, 0), "150516_CM1_Level1");
  BOOST_CHECK_EQUAL(session_handler.stds_concs_table_body(session_handler.stds_concs_table_body.dimension(0) - 1, session_handler.stds_concs_table_body.dimension(1) - 1), "1.000000");

}

BOOST_AUTO_TEST_CASE(setComponentFiltersTable1)
{
  TestData testData;
  SessionHandler session_handler; 
  session_handler.setComponentFiltersTable(testData.sequenceHandler);
  BOOST_CHECK_EQUAL(session_handler.comp_filters_table_headers.size(), 11);
  BOOST_CHECK_EQUAL(session_handler.comp_filters_table_headers(0), "component_name");
  BOOST_CHECK_EQUAL(session_handler.comp_filters_table_headers(session_handler.comp_filters_table_headers.size() - 1), "metaValue_peak_apex_int_u");
  BOOST_CHECK_EQUAL(session_handler.comp_filters_table_body.dimension(0), 10);
  BOOST_CHECK_EQUAL(session_handler.comp_filters_table_body.dimension(1), 11);
  BOOST_CHECK_EQUAL(session_handler.comp_filters_table_body(0, 0), "arg-L.arg-L_1.Heavy");
  BOOST_CHECK_EQUAL(session_handler.comp_filters_table_body(session_handler.comp_filters_table_body.dimension(0) - 1, session_handler.comp_filters_table_body.dimension(1) - 1), "1000000000000.000000");

}

BOOST_AUTO_TEST_CASE(setComponentGroupFiltersTable1)
{
  TestData testData;
  SessionHandler session_handler; 
  session_handler.setComponentGroupFiltersTable(testData.sequenceHandler);
  BOOST_CHECK_EQUAL(session_handler.comp_group_filters_table_headers.size(), 24);
  BOOST_CHECK_EQUAL(session_handler.comp_group_filters_table_headers(0), "component_group_name");
  BOOST_CHECK_EQUAL(session_handler.comp_group_filters_table_headers(session_handler.comp_group_filters_table_headers.size() - 1), "ion_ratio_feature_name");
  BOOST_CHECK_EQUAL(session_handler.comp_group_filters_table_body.dimension(0), 10);
  BOOST_CHECK_EQUAL(session_handler.comp_group_filters_table_body.dimension(1), 24);
  BOOST_CHECK_EQUAL(session_handler.comp_group_filters_table_body(0, 0), "arg-L");
  BOOST_CHECK_EQUAL(session_handler.comp_group_filters_table_body(session_handler.comp_group_filters_table_body.dimension(0) - 1, session_handler.comp_group_filters_table_body.dimension(1) - 1), "");

}

BOOST_AUTO_TEST_CASE(setComponentQCsTable1)
{
  TestData testData;
  SessionHandler session_handler;
  session_handler.setComponentQCsTable(testData.sequenceHandler);
  BOOST_CHECK_EQUAL(session_handler.comp_qcs_table_headers.size(), 19);
  BOOST_CHECK_EQUAL(session_handler.comp_qcs_table_headers(0), "component_name");
  BOOST_CHECK_EQUAL(session_handler.comp_qcs_table_headers(session_handler.comp_qcs_table_headers.size() - 1), "metaValue_var_xcorr_shape_weighted_u");
  BOOST_CHECK_EQUAL(session_handler.comp_qcs_table_body.dimension(0), 10);
  BOOST_CHECK_EQUAL(session_handler.comp_qcs_table_body.dimension(1), 19);
  BOOST_CHECK_EQUAL(session_handler.comp_qcs_table_body(0, 0), "arg-L.arg-L_1.Heavy");
  BOOST_CHECK_EQUAL(session_handler.comp_qcs_table_body(session_handler.comp_qcs_table_body.dimension(0) - 1, session_handler.comp_qcs_table_body.dimension(1) - 1), "1000000000.000000");

}

BOOST_AUTO_TEST_CASE(setComponentGroupQCsTable1)
{
  TestData testData;
  SessionHandler session_handler;
  session_handler.setComponentGroupQCsTable(testData.sequenceHandler);
  BOOST_CHECK_EQUAL(session_handler.comp_group_qcs_table_headers.size(), 24);
  BOOST_CHECK_EQUAL(session_handler.comp_group_qcs_table_headers(0), "component_group_name");
  BOOST_CHECK_EQUAL(session_handler.comp_group_qcs_table_headers(session_handler.comp_group_qcs_table_headers.size() - 1), "ion_ratio_feature_name");
  BOOST_CHECK_EQUAL(session_handler.comp_group_qcs_table_body.dimension(0), 10);
  BOOST_CHECK_EQUAL(session_handler.comp_group_qcs_table_body.dimension(1), 24);
  BOOST_CHECK_EQUAL(session_handler.comp_group_qcs_table_body(0, 0), "arg-L");
  BOOST_CHECK_EQUAL(session_handler.comp_group_qcs_table_body(session_handler.comp_group_qcs_table_body.dimension(0) - 1, session_handler.comp_group_qcs_table_body.dimension(1) - 1), "");
}

BOOST_AUTO_TEST_CASE(setComponentRSDFiltersTable1)
{
  TestData testData;
  SessionHandler session_handler;
  session_handler.setComponentRSDFiltersTable(testData.sequenceHandler);
  BOOST_CHECK_EQUAL(session_handler.comp_rsd_filters_table_headers.size(), 11);
  BOOST_CHECK_EQUAL(session_handler.comp_rsd_filters_table_headers(0), "component_name");
  BOOST_CHECK_EQUAL(session_handler.comp_rsd_filters_table_headers(session_handler.comp_rsd_filters_table_headers.size() - 1), "metaValue_peak_apex_int_u");
  BOOST_CHECK_EQUAL(session_handler.comp_rsd_filters_table_body.dimension(0), 10);
  BOOST_CHECK_EQUAL(session_handler.comp_rsd_filters_table_body.dimension(1), 11);
  BOOST_CHECK_EQUAL(session_handler.comp_rsd_filters_table_body(0, 0), "arg-L.arg-L_1.Heavy");
  BOOST_CHECK_EQUAL(session_handler.comp_rsd_filters_table_body(session_handler.comp_rsd_filters_table_body.dimension(0) - 1, session_handler.comp_rsd_filters_table_body.dimension(1) - 1), "1000000000000.000000");

}

BOOST_AUTO_TEST_CASE(setComponentGroupRSDFiltersTable1)
{
  TestData testData;
  SessionHandler session_handler;
  session_handler.setComponentGroupRSDFiltersTable(testData.sequenceHandler);
  BOOST_CHECK_EQUAL(session_handler.comp_group_rsd_filters_table_headers.size(), 24);
  BOOST_CHECK_EQUAL(session_handler.comp_group_rsd_filters_table_headers(0), "component_group_name");
  BOOST_CHECK_EQUAL(session_handler.comp_group_rsd_filters_table_headers(session_handler.comp_group_rsd_filters_table_headers.size() - 1), "ion_ratio_feature_name");
  BOOST_CHECK_EQUAL(session_handler.comp_group_rsd_filters_table_body.dimension(0), 10);
  BOOST_CHECK_EQUAL(session_handler.comp_group_rsd_filters_table_body.dimension(1), 24);
  BOOST_CHECK_EQUAL(session_handler.comp_group_rsd_filters_table_body(0, 0), "arg-L");
  BOOST_CHECK_EQUAL(session_handler.comp_group_rsd_filters_table_body(session_handler.comp_group_rsd_filters_table_body.dimension(0) - 1, session_handler.comp_group_rsd_filters_table_body.dimension(1) - 1), "");

}

BOOST_AUTO_TEST_CASE(setComponentRSDQCsTable1)
{
  TestData testData;
  SessionHandler session_handler;
  session_handler.setComponentRSDQCsTable(testData.sequenceHandler);
  BOOST_CHECK_EQUAL(session_handler.comp_rsd_qcs_table_headers.size(), 19);
  BOOST_CHECK_EQUAL(session_handler.comp_rsd_qcs_table_headers(0), "component_name");
  BOOST_CHECK_EQUAL(session_handler.comp_rsd_qcs_table_headers(session_handler.comp_rsd_qcs_table_headers.size() - 1), "metaValue_var_xcorr_shape_weighted_u");
  BOOST_CHECK_EQUAL(session_handler.comp_rsd_qcs_table_body.dimension(0), 10);
  BOOST_CHECK_EQUAL(session_handler.comp_rsd_qcs_table_body.dimension(1), 19);
  BOOST_CHECK_EQUAL(session_handler.comp_rsd_qcs_table_body(0, 0), "arg-L.arg-L_1.Heavy");
  BOOST_CHECK_EQUAL(session_handler.comp_rsd_qcs_table_body(session_handler.comp_rsd_qcs_table_body.dimension(0) - 1, session_handler.comp_rsd_qcs_table_body.dimension(1) - 1), "1000000000.000000");

}

BOOST_AUTO_TEST_CASE(setComponentGroupRSDQCsTable1)
{
  TestData testData;
  SessionHandler session_handler;
  session_handler.setComponentGroupRSDQCsTable(testData.sequenceHandler);
  BOOST_CHECK_EQUAL(session_handler.comp_group_rsd_qcs_table_headers.size(), 24);
  BOOST_CHECK_EQUAL(session_handler.comp_group_rsd_qcs_table_headers(0), "component_group_name");
  BOOST_CHECK_EQUAL(session_handler.comp_group_rsd_qcs_table_headers(session_handler.comp_group_rsd_qcs_table_headers.size() - 1), "ion_ratio_feature_name");
  BOOST_CHECK_EQUAL(session_handler.comp_group_rsd_qcs_table_body.dimension(0), 10);
  BOOST_CHECK_EQUAL(session_handler.comp_group_rsd_qcs_table_body.dimension(1), 24);
  BOOST_CHECK_EQUAL(session_handler.comp_group_rsd_qcs_table_body(0, 0), "arg-L");
  BOOST_CHECK_EQUAL(session_handler.comp_group_rsd_qcs_table_body(session_handler.comp_group_rsd_qcs_table_body.dimension(0) - 1, session_handler.comp_group_rsd_qcs_table_body.dimension(1) - 1), "");
}

BOOST_AUTO_TEST_CASE(setComponentBackgroundFiltersTable1)
{
  TestData testData;
  SessionHandler session_handler;
  session_handler.setComponentBackgroundFiltersTable(testData.sequenceHandler);
  BOOST_CHECK_EQUAL(session_handler.comp_background_filters_table_headers.size(), 11);
  BOOST_CHECK_EQUAL(session_handler.comp_background_filters_table_headers(0), "component_name");
  BOOST_CHECK_EQUAL(session_handler.comp_background_filters_table_headers(session_handler.comp_background_filters_table_headers.size() - 1), "metaValue_peak_apex_int_u");
  BOOST_CHECK_EQUAL(session_handler.comp_background_filters_table_body.dimension(0), 10);
  BOOST_CHECK_EQUAL(session_handler.comp_background_filters_table_body.dimension(1), 11);
  BOOST_CHECK_EQUAL(session_handler.comp_background_filters_table_body(0, 0), "arg-L.arg-L_1.Heavy");
  BOOST_CHECK_EQUAL(session_handler.comp_background_filters_table_body(session_handler.comp_background_filters_table_body.dimension(0) - 1, session_handler.comp_background_filters_table_body.dimension(1) - 1), "1000000000000.000000");

}

BOOST_AUTO_TEST_CASE(setComponentGroupBackgroundFiltersTable1)
{
  TestData testData;
  SessionHandler session_handler;
  session_handler.setComponentGroupBackgroundFiltersTable(testData.sequenceHandler);
  BOOST_CHECK_EQUAL(session_handler.comp_group_background_filters_table_headers.size(), 24);
  BOOST_CHECK_EQUAL(session_handler.comp_group_background_filters_table_headers(0), "component_group_name");
  BOOST_CHECK_EQUAL(session_handler.comp_group_background_filters_table_headers(session_handler.comp_group_background_filters_table_headers.size() - 1), "ion_ratio_feature_name");
  BOOST_CHECK_EQUAL(session_handler.comp_group_background_filters_table_body.dimension(0), 10);
  BOOST_CHECK_EQUAL(session_handler.comp_group_background_filters_table_body.dimension(1), 24);
  BOOST_CHECK_EQUAL(session_handler.comp_group_background_filters_table_body(0, 0), "arg-L");
  BOOST_CHECK_EQUAL(session_handler.comp_group_background_filters_table_body(session_handler.comp_group_background_filters_table_body.dimension(0) - 1, session_handler.comp_group_background_filters_table_body.dimension(1) - 1), "");

}

BOOST_AUTO_TEST_CASE(setComponentBackgroundQCsTable1)
{
  TestData testData;
  SessionHandler session_handler;
  session_handler.setComponentBackgroundQCsTable(testData.sequenceHandler);
  BOOST_CHECK_EQUAL(session_handler.comp_background_qcs_table_headers.size(), 19);
  BOOST_CHECK_EQUAL(session_handler.comp_background_qcs_table_headers(0), "component_name");
  BOOST_CHECK_EQUAL(session_handler.comp_background_qcs_table_headers(session_handler.comp_background_qcs_table_headers.size() - 1), "metaValue_var_xcorr_shape_weighted_u");
  BOOST_CHECK_EQUAL(session_handler.comp_background_qcs_table_body.dimension(0), 10);
  BOOST_CHECK_EQUAL(session_handler.comp_background_qcs_table_body.dimension(1), 19);
  BOOST_CHECK_EQUAL(session_handler.comp_background_qcs_table_body(0, 0), "arg-L.arg-L_1.Heavy");
  BOOST_CHECK_EQUAL(session_handler.comp_background_qcs_table_body(session_handler.comp_background_qcs_table_body.dimension(0) - 1, session_handler.comp_background_qcs_table_body.dimension(1) - 1), "1000000000.000000");

}

BOOST_AUTO_TEST_CASE(setComponentGroupBackgroundQCsTable1)
{
  TestData testData;
  SessionHandler session_handler;
  session_handler.setComponentGroupBackgroundQCsTable(testData.sequenceHandler);
  BOOST_CHECK_EQUAL(session_handler.comp_group_background_qcs_table_headers.size(), 24);
  BOOST_CHECK_EQUAL(session_handler.comp_group_background_qcs_table_headers(0), "component_group_name");
  BOOST_CHECK_EQUAL(session_handler.comp_group_background_qcs_table_headers(session_handler.comp_group_background_qcs_table_headers.size() - 1), "ion_ratio_feature_name");
  BOOST_CHECK_EQUAL(session_handler.comp_group_background_qcs_table_body.dimension(0), 10);
  BOOST_CHECK_EQUAL(session_handler.comp_group_background_qcs_table_body.dimension(1), 24);
  BOOST_CHECK_EQUAL(session_handler.comp_group_background_qcs_table_body(0, 0), "arg-L");
  BOOST_CHECK_EQUAL(session_handler.comp_group_background_qcs_table_body(session_handler.comp_group_background_qcs_table_body.dimension(0) - 1, session_handler.comp_group_background_qcs_table_body.dimension(1) - 1), "");
}

BOOST_AUTO_TEST_CASE(setComponentRSDEstimationsTable1)
{
  TestData testData;
  testData.changeSampleType(SampleType::QC);
  const map<string, vector<map<string, string>>> params;
  Filenames filenames_;
  EstimateFeatureRSDs processor;
  processor.process(testData.sequenceHandler.getSequenceSegments().front(), testData.sequenceHandler, params, filenames_);
  SessionHandler session_handler;
  session_handler.setComponentRSDEstimationsTable(testData.sequenceHandler);
  BOOST_CHECK_EQUAL(session_handler.comp_rsd_estimations_table_headers.size(), 11);
  BOOST_CHECK_EQUAL(session_handler.comp_rsd_estimations_table_headers(0), "component_name");
  BOOST_CHECK_EQUAL(session_handler.comp_rsd_estimations_table_headers(session_handler.comp_rsd_estimations_table_headers.size() - 1), "metaValue_peak_apex_int_u");
  BOOST_CHECK_EQUAL(session_handler.comp_rsd_estimations_table_body.dimension(0), 10);
  BOOST_CHECK_EQUAL(session_handler.comp_rsd_estimations_table_body.dimension(1), 11);
  BOOST_CHECK_EQUAL(session_handler.comp_rsd_estimations_table_body(0, 0), "arg-L.arg-L_1.Heavy");
  BOOST_CHECK_EQUAL(session_handler.comp_rsd_estimations_table_body(session_handler.comp_rsd_estimations_table_body.dimension(0) - 1, session_handler.comp_rsd_estimations_table_body.dimension(1) - 1), "0.000000");

}

BOOST_AUTO_TEST_CASE(setComponentGroupRSDEstimationsTable1)
{
  TestData testData;
  testData.changeSampleType(SampleType::QC);
  const map<string, vector<map<string, string>>> params;
  Filenames filenames_;
  EstimateFeatureRSDs processor;
  processor.process(testData.sequenceHandler.getSequenceSegments().front(), testData.sequenceHandler, params, filenames_);
  SessionHandler session_handler;
  session_handler.setComponentGroupRSDEstimationsTable(testData.sequenceHandler);
  BOOST_CHECK_EQUAL(session_handler.comp_group_rsd_estimations_table_headers.size(), 24);
  BOOST_CHECK_EQUAL(session_handler.comp_group_rsd_estimations_table_headers(0), "component_group_name");
  BOOST_CHECK_EQUAL(session_handler.comp_group_rsd_estimations_table_headers(session_handler.comp_group_rsd_estimations_table_headers.size() - 1), "ion_ratio_feature_name");
  BOOST_CHECK_EQUAL(session_handler.comp_group_rsd_estimations_table_body.dimension(0), 10);
  BOOST_CHECK_EQUAL(session_handler.comp_group_rsd_estimations_table_body.dimension(1), 24);
  BOOST_CHECK_EQUAL(session_handler.comp_group_rsd_estimations_table_body(0, 0), "arg-L");
  BOOST_CHECK_EQUAL(session_handler.comp_group_rsd_estimations_table_body(session_handler.comp_group_rsd_estimations_table_body.dimension(0) - 1, session_handler.comp_group_rsd_estimations_table_body.dimension(1) - 1), "");

}

BOOST_AUTO_TEST_CASE(setComponentBackgroundEstimationsTable1)
{
  TestData testData;
  testData.changeSampleType(SampleType::Blank);
  const map<string, vector<map<string, string>>> params;
  Filenames filenames_;
  EstimateFeatureBackgroundInterferences processor;
  processor.process(testData.sequenceHandler.getSequenceSegments().front(), testData.sequenceHandler, params, filenames_);
  SessionHandler session_handler;
  session_handler.setComponentBackgroundEstimationsTable(testData.sequenceHandler);
  BOOST_CHECK_EQUAL(session_handler.comp_background_estimations_table_headers.size(), 11);
  BOOST_CHECK_EQUAL(session_handler.comp_background_estimations_table_headers(0), "component_name");
  BOOST_CHECK_EQUAL(session_handler.comp_background_estimations_table_headers(session_handler.comp_background_estimations_table_headers.size() - 1), "metaValue_peak_apex_int_u");
  BOOST_CHECK_EQUAL(session_handler.comp_background_estimations_table_body.dimension(0), 10);
  BOOST_CHECK_EQUAL(session_handler.comp_background_estimations_table_body.dimension(1), 11);
  BOOST_CHECK_EQUAL(session_handler.comp_background_estimations_table_body(0, 0), "arg-L.arg-L_1.Heavy");
  BOOST_CHECK_EQUAL(session_handler.comp_background_estimations_table_body(session_handler.comp_background_estimations_table_body.dimension(0) - 1, session_handler.comp_background_estimations_table_body.dimension(1) - 1), "0.000000");

}

BOOST_AUTO_TEST_CASE(setComponentGroupBackgroundEstimationsTable1)
{
  TestData testData;
  testData.changeSampleType(SampleType::Blank);
  const map<string, vector<map<string, string>>> params;
  Filenames filenames_;
  EstimateFeatureBackgroundInterferences processor;
  processor.process(testData.sequenceHandler.getSequenceSegments().front(), testData.sequenceHandler, params, filenames_);
  SessionHandler session_handler;
  session_handler.setComponentGroupBackgroundEstimationsTable(testData.sequenceHandler);
  BOOST_CHECK_EQUAL(session_handler.comp_group_background_estimations_table_headers.size(), 24);
  BOOST_CHECK_EQUAL(session_handler.comp_group_background_estimations_table_headers(0), "component_group_name");
  BOOST_CHECK_EQUAL(session_handler.comp_group_background_estimations_table_headers(session_handler.comp_group_background_estimations_table_headers.size() - 1), "ion_ratio_feature_name");
  BOOST_CHECK_EQUAL(session_handler.comp_group_background_estimations_table_body.dimension(0), 10);
  BOOST_CHECK_EQUAL(session_handler.comp_group_background_estimations_table_body.dimension(1), 24);
  BOOST_CHECK_EQUAL(session_handler.comp_group_background_estimations_table_body(0, 0), "arg-L");
  BOOST_CHECK_EQUAL(session_handler.comp_group_background_estimations_table_body(session_handler.comp_group_background_estimations_table_body.dimension(0) - 1, session_handler.comp_group_background_estimations_table_body.dimension(1) - 1), "");

}

BOOST_AUTO_TEST_CASE(sessionHandlerGetters1)
{
  TestData testData(true, false, true);
  SessionHandler session_handler;
  session_handler.setMinimalDataAndFilters(testData.sequenceHandler);
  // Dynamic headers and body
  BOOST_CHECK_EQUAL(session_handler.getInjectionExplorerHeader().size(), 2);
  BOOST_CHECK_EQUAL(session_handler.getInjectionExplorerHeader()(0), "inj#");
  BOOST_CHECK_EQUAL(session_handler.getInjectionExplorerHeader()(session_handler.getInjectionExplorerHeader().dimension(0) - 1), "sample_name");
  BOOST_CHECK_EQUAL(session_handler.getInjectionExplorerBody().dimension(0), 2);
  BOOST_CHECK_EQUAL(session_handler.getInjectionExplorerBody().dimension(1), 2);
  BOOST_CHECK_EQUAL(session_handler.getInjectionExplorerBody()(0,0), "1");
  BOOST_CHECK_EQUAL(session_handler.getInjectionExplorerBody()(session_handler.getInjectionExplorerBody().dimension(0) - 1,session_handler.getInjectionExplorerBody().dimension(1) - 1), "150516_CM1_Level10");
  BOOST_CHECK_EQUAL(session_handler.getTransitionExplorerHeader().size(), 2);
  BOOST_CHECK_EQUAL(session_handler.getTransitionExplorerHeader()(0), "transition_group");
  BOOST_CHECK_EQUAL(session_handler.getTransitionExplorerHeader()(session_handler.getTransitionExplorerHeader().dimension(0) - 1), "transition_name");
  BOOST_CHECK_EQUAL(session_handler.getTransitionExplorerBody().dimension(0), 6);
  BOOST_CHECK_EQUAL(session_handler.getTransitionExplorerBody().dimension(1), 2);
  BOOST_CHECK_EQUAL(session_handler.getTransitionExplorerBody()(0,0), "arg-L");
  BOOST_CHECK_EQUAL(session_handler.getTransitionExplorerBody()(session_handler.getTransitionExplorerBody().dimension(0) - 1, session_handler.getTransitionExplorerBody().dimension(1) - 1), "ser-L.ser-L_2.Light");
  BOOST_CHECK_EQUAL(session_handler.getSpectrumExplorerHeader().size(), 1);
  BOOST_CHECK_EQUAL(session_handler.getSpectrumExplorerHeader()(0), "native_id");
  BOOST_CHECK_EQUAL(session_handler.getSpectrumExplorerHeader()(session_handler.getSpectrumExplorerHeader().dimension(0) - 1), "native_id");
  BOOST_CHECK_EQUAL(session_handler.getSpectrumExplorerBody().dimension(0), 1);
  BOOST_CHECK_EQUAL(session_handler.getSpectrumExplorerBody().dimension(1), 1);
  BOOST_CHECK_EQUAL(session_handler.getSpectrumExplorerBody()(0, 0), "controllerType=0 controllerNumber=1 scan=1");
  BOOST_CHECK_EQUAL(session_handler.getSpectrumExplorerBody()(session_handler.getSpectrumExplorerBody().dimension(0) - 1, session_handler.getSpectrumExplorerBody().dimension(1) - 1), "controllerType=0 controllerNumber=1 scan=1");
  // N selected counts
  BOOST_CHECK_EQUAL(session_handler.getNSelectedSampleNamesTable(), 2);
  BOOST_CHECK_EQUAL(session_handler.getNSelectedSampleNamesPlot(), 1);
  BOOST_CHECK_EQUAL(session_handler.getNSelectedTransitionsTable(), 6);
  BOOST_CHECK_EQUAL(session_handler.getNSelectedTransitionsPlot(), 1);
  BOOST_CHECK_EQUAL(session_handler.getNSelectedFeatureMetaValuesTable(), 20);
  BOOST_CHECK_EQUAL(session_handler.getNSelectedFeatureMetaValuesPlot(), 1);
  // Selected string values
  BOOST_CHECK(session_handler.getSelectInjectionNamesWorkflow(testData.sequenceHandler) == std::set<std::string>({"150516_CM1_Level10_2_BatchName_1900-01-01_000000", "150516_CM1_Level1_1_BatchName_1900-01-01_000000"}));
  BOOST_CHECK(session_handler.getSelectSequenceSegmentNamesWorkflow(testData.sequenceHandler) == std::set<std::string>({ "segment1" }));
  BOOST_CHECK(session_handler.getSelectSampleGroupNamesWorkflow(testData.sequenceHandler) == std::set<std::string>({ "CM" }));
  BOOST_CHECK_EQUAL(session_handler.getSelectSampleNamesTable().size(), 2);
  BOOST_CHECK_EQUAL(session_handler.getSelectSampleNamesTable()(0), "150516_CM1_Level1");
  BOOST_CHECK_EQUAL(session_handler.getSelectSampleNamesTable()(session_handler.getSelectSampleNamesTable().dimension(0)-1), "150516_CM1_Level10");
  BOOST_CHECK_EQUAL(session_handler.getSelectSampleNamesPlot().size(), 2);
  BOOST_CHECK_EQUAL(session_handler.getSelectSampleNamesPlot()(0), "150516_CM1_Level1");
  BOOST_CHECK_EQUAL(session_handler.getSelectSampleNamesPlot()(session_handler.getSelectSampleNamesPlot().dimension(0) - 1), "");
  BOOST_CHECK_EQUAL(session_handler.getSelectTransitionsTable().size(), 6);
  BOOST_CHECK_EQUAL(session_handler.getSelectTransitionsTable()(0), "arg-L.arg-L_1.Light");
  BOOST_CHECK_EQUAL(session_handler.getSelectTransitionsTable()(session_handler.getSelectTransitionsTable().dimension(0) - 1), "ser-L.ser-L_2.Light");
  BOOST_CHECK_EQUAL(session_handler.getSelectTransitionGroupsTable().size(), 6);
  BOOST_CHECK_EQUAL(session_handler.getSelectTransitionGroupsTable()(0), "arg-L");
  BOOST_CHECK_EQUAL(session_handler.getSelectTransitionGroupsTable()(session_handler.getSelectTransitionGroupsTable().dimension(0) - 1), "ser-L");
  BOOST_CHECK_EQUAL(session_handler.getSelectTransitionsPlot().size(), 6);
  BOOST_CHECK_EQUAL(session_handler.getSelectTransitionsPlot()(0), "arg-L.arg-L_1.Light");
  BOOST_CHECK_EQUAL(session_handler.getSelectTransitionsPlot()(session_handler.getSelectTransitionsPlot().dimension(0) - 1), "");
  BOOST_CHECK_EQUAL(session_handler.getSelectTransitionGroupsPlot().size(), 6);
  BOOST_CHECK_EQUAL(session_handler.getSelectTransitionGroupsPlot()(0), "arg-L");
  BOOST_CHECK_EQUAL(session_handler.getSelectTransitionGroupsPlot()(session_handler.getSelectTransitionsPlot().dimension(0) - 1), "");
  BOOST_CHECK_EQUAL(session_handler.getSelectFeatureMetaValuesTable().size(), 20);
  BOOST_CHECK_EQUAL(session_handler.getSelectFeatureMetaValuesTable()(0), "asymmetry_factor");
  BOOST_CHECK_EQUAL(session_handler.getSelectFeatureMetaValuesTable()(session_handler.getSelectFeatureMetaValuesTable().dimension(0) - 1), "mz_error_Da");
  BOOST_CHECK_EQUAL(session_handler.getSelectFeatureMetaValuesPlot().size(), 20);
  BOOST_CHECK_EQUAL(session_handler.getSelectFeatureMetaValuesPlot()(0), "");
  BOOST_CHECK_EQUAL(session_handler.getSelectFeatureMetaValuesPlot()(session_handler.getSelectFeatureMetaValuesPlot().dimension(0) - 1), "");
  BOOST_CHECK_EQUAL(session_handler.getSelectSpectrumPlot().size(), 1);
  BOOST_CHECK_EQUAL(session_handler.getSelectSpectrumPlot()(0), "controllerType=0 controllerNumber=1 scan=1");
  BOOST_CHECK_EQUAL(session_handler.getSelectSpectrumPlot()(session_handler.getSelectSpectrumPlot().dimension(0) - 1), "controllerType=0 controllerNumber=1 scan=1");
  // Table filters
  BOOST_CHECK_EQUAL(session_handler.getSequenceTableFilters().size(), 2);
  BOOST_CHECK(session_handler.getSequenceTableFilters()(0));
  BOOST_CHECK(session_handler.getSequenceTableFilters()(session_handler.getSequenceTableFilters().dimension(0) - 1));
  BOOST_CHECK_EQUAL(session_handler.getTransitionsTableFilters().size(), 6);
  BOOST_CHECK(session_handler.getTransitionsTableFilters()(0));
  BOOST_CHECK(session_handler.getTransitionsTableFilters()(session_handler.getTransitionsTableFilters().dimension(0) - 1));
  BOOST_CHECK_EQUAL(session_handler.getSpectrumTableFilters().size(), 1);
  BOOST_CHECK(session_handler.getSpectrumTableFilters()(0));
  BOOST_CHECK(session_handler.getSpectrumTableFilters()(session_handler.getSpectrumTableFilters().dimension(0) - 1));
  session_handler.setQuantMethodTable(testData.sequenceHandler);
  BOOST_CHECK_EQUAL(session_handler.getQuantMethodsTableFilters().size(), 10);
  BOOST_CHECK(session_handler.getQuantMethodsTableFilters()(0));
  BOOST_CHECK(!session_handler.getQuantMethodsTableFilters()(session_handler.getQuantMethodsTableFilters().dimension(0) - 1));
  session_handler.setComponentFiltersTable(testData.sequenceHandler);
  BOOST_CHECK_EQUAL(session_handler.getComponentFiltersTableFilters().size(), 10);
  BOOST_CHECK(!session_handler.getComponentFiltersTableFilters()(0));
  BOOST_CHECK(session_handler.getComponentFiltersTableFilters()(session_handler.getComponentFiltersTableFilters().dimension(0) - 1));
  session_handler.setComponentQCsTable(testData.sequenceHandler);
  BOOST_CHECK_EQUAL(session_handler.getComponentQCsTableFilters().size(), 10);
  BOOST_CHECK(!session_handler.getComponentQCsTableFilters()(0));
  BOOST_CHECK(session_handler.getComponentQCsTableFilters()(session_handler.getComponentQCsTableFilters().dimension(0) - 1));
  session_handler.setComponentGroupFiltersTable(testData.sequenceHandler);
  BOOST_CHECK_EQUAL(session_handler.getComponentGroupFiltersTableFilters().size(), 10);
  BOOST_CHECK(session_handler.getComponentGroupFiltersTableFilters()(0));
  BOOST_CHECK(!session_handler.getComponentGroupFiltersTableFilters()(session_handler.getComponentGroupFiltersTableFilters().dimension(0) - 1));
  session_handler.setComponentGroupQCsTable(testData.sequenceHandler);
  BOOST_CHECK_EQUAL(session_handler.getComponentGroupQCsTableFilters().size(), 10);
  BOOST_CHECK(session_handler.getComponentGroupQCsTableFilters()(0));
  BOOST_CHECK(!session_handler.getComponentGroupQCsTableFilters()(session_handler.getComponentGroupQCsTableFilters().dimension(0) - 1));

  session_handler.setComponentRSDFiltersTable(testData.sequenceHandler);
  BOOST_CHECK_EQUAL(session_handler.getComponentRSDFiltersTableFilters().size(), 10);
  BOOST_CHECK(!session_handler.getComponentRSDFiltersTableFilters()(0));
  BOOST_CHECK(session_handler.getComponentRSDFiltersTableFilters()(session_handler.getComponentRSDFiltersTableFilters().dimension(0) - 1));
  session_handler.setComponentRSDQCsTable(testData.sequenceHandler);
  BOOST_CHECK_EQUAL(session_handler.getComponentRSDQCsTableFilters().size(), 10);
  BOOST_CHECK(!session_handler.getComponentRSDQCsTableFilters()(0));
  BOOST_CHECK(session_handler.getComponentRSDQCsTableFilters()(session_handler.getComponentRSDQCsTableFilters().dimension(0) - 1));
  session_handler.setComponentGroupRSDFiltersTable(testData.sequenceHandler);
  BOOST_CHECK_EQUAL(session_handler.getComponentGroupRSDFiltersTableFilters().size(), 10);
  BOOST_CHECK(session_handler.getComponentGroupRSDFiltersTableFilters()(0));
  BOOST_CHECK(!session_handler.getComponentGroupRSDFiltersTableFilters()(session_handler.getComponentGroupRSDFiltersTableFilters().dimension(0) - 1));
  session_handler.setComponentGroupRSDQCsTable(testData.sequenceHandler);
  BOOST_CHECK_EQUAL(session_handler.getComponentGroupRSDQCsTableFilters().size(), 10);
  BOOST_CHECK(session_handler.getComponentGroupRSDQCsTableFilters()(0));
  BOOST_CHECK(!session_handler.getComponentGroupRSDQCsTableFilters()(session_handler.getComponentGroupRSDQCsTableFilters().dimension(0) - 1));  
  session_handler.setComponentBackgroundFiltersTable(testData.sequenceHandler);
  BOOST_CHECK_EQUAL(session_handler.getComponentBackgroundFiltersTableFilters().size(), 10);
  BOOST_CHECK(!session_handler.getComponentBackgroundFiltersTableFilters()(0));
  BOOST_CHECK(session_handler.getComponentBackgroundFiltersTableFilters()(session_handler.getComponentBackgroundFiltersTableFilters().dimension(0) - 1));
  session_handler.setComponentBackgroundQCsTable(testData.sequenceHandler);
  BOOST_CHECK_EQUAL(session_handler.getComponentBackgroundQCsTableFilters().size(), 10);
  BOOST_CHECK(!session_handler.getComponentBackgroundQCsTableFilters()(0));
  BOOST_CHECK(session_handler.getComponentBackgroundQCsTableFilters()(session_handler.getComponentBackgroundQCsTableFilters().dimension(0) - 1));
  session_handler.setComponentGroupBackgroundFiltersTable(testData.sequenceHandler);
  BOOST_CHECK_EQUAL(session_handler.getComponentGroupBackgroundFiltersTableFilters().size(), 10);
  BOOST_CHECK(session_handler.getComponentGroupBackgroundFiltersTableFilters()(0));
  BOOST_CHECK(!session_handler.getComponentGroupBackgroundFiltersTableFilters()(session_handler.getComponentGroupBackgroundFiltersTableFilters().dimension(0) - 1));
  session_handler.setComponentGroupBackgroundQCsTable(testData.sequenceHandler);
  BOOST_CHECK_EQUAL(session_handler.getComponentGroupBackgroundQCsTableFilters().size(), 10);
  BOOST_CHECK(session_handler.getComponentGroupBackgroundQCsTableFilters()(0));
  BOOST_CHECK(!session_handler.getComponentGroupBackgroundQCsTableFilters()(session_handler.getComponentGroupBackgroundQCsTableFilters().dimension(0) - 1));

  testData.changeSampleType(SampleType::QC);
  const map<string, vector<map<string, string>>> params;
  Filenames filenames_;
  EstimateFeatureRSDs estimateFeatureRSDs;
  estimateFeatureRSDs.process(testData.sequenceHandler.getSequenceSegments().front(), testData.sequenceHandler, params, filenames_);
  session_handler.setComponentRSDEstimationsTable(testData.sequenceHandler);
  BOOST_CHECK_EQUAL(session_handler.getComponentRSDEstimationsTableFilters().size(), 10);
  BOOST_CHECK(!session_handler.getComponentRSDEstimationsTableFilters()(0));
  BOOST_CHECK(session_handler.getComponentRSDEstimationsTableFilters()(session_handler.getComponentRSDEstimationsTableFilters().dimension(0) - 1));
  session_handler.setComponentGroupRSDEstimationsTable(testData.sequenceHandler);
  BOOST_CHECK_EQUAL(session_handler.getComponentGroupRSDEstimationsTableFilters().size(), 10);
  BOOST_CHECK(session_handler.getComponentGroupRSDEstimationsTableFilters()(0));
  BOOST_CHECK(!session_handler.getComponentGroupRSDEstimationsTableFilters()(session_handler.getComponentGroupRSDEstimationsTableFilters().dimension(0) - 1));

  testData.changeSampleType(SampleType::Blank);
  EstimateFeatureBackgroundInterferences estimateFeatureBackgroundInterferences;
  estimateFeatureBackgroundInterferences.process(testData.sequenceHandler.getSequenceSegments().front(), testData.sequenceHandler, params, filenames_);
  session_handler.setComponentBackgroundEstimationsTable(testData.sequenceHandler);
  BOOST_CHECK_EQUAL(session_handler.getComponentBackgroundEstimationsTableFilters().size(), 10);
  BOOST_CHECK(!session_handler.getComponentBackgroundEstimationsTableFilters()(0));
  BOOST_CHECK(session_handler.getComponentBackgroundEstimationsTableFilters()(session_handler.getComponentBackgroundEstimationsTableFilters().dimension(0) - 1));
  session_handler.setComponentGroupBackgroundEstimationsTable(testData.sequenceHandler);
  BOOST_CHECK_EQUAL(session_handler.getComponentGroupBackgroundEstimationsTableFilters().size(), 10);
  BOOST_CHECK(session_handler.getComponentGroupBackgroundEstimationsTableFilters()(0));
  BOOST_CHECK(!session_handler.getComponentGroupBackgroundEstimationsTableFilters()(session_handler.getComponentGroupBackgroundEstimationsTableFilters().dimension(0) - 1));

}

BOOST_AUTO_TEST_CASE(setFeatureTable1)
{
  TestData testData(true, true);
  SessionHandler session_handler;
  session_handler.setFeatureTable(testData.sequenceHandler);
  BOOST_CHECK_EQUAL(session_handler.feature_table_headers.size(), 22);
  BOOST_CHECK_EQUAL(session_handler.feature_table_headers(0), "sample_name");
  BOOST_CHECK_EQUAL(session_handler.feature_table_headers(session_handler.feature_table_headers.size() - 1), "used_");
  BOOST_CHECK_EQUAL(session_handler.feature_table_body.dimension(0), 13);
  BOOST_CHECK_EQUAL(session_handler.feature_table_body.dimension(1), 22);
  BOOST_CHECK_EQUAL(session_handler.feature_table_body(0, 0), "150516_CM1_Level1");
  BOOST_CHECK_EQUAL(session_handler.feature_table_body(session_handler.feature_table_body.dimension(0) - 1, session_handler.feature_table_body.dimension(1) - 1), "true");
  session_handler.setMinimalDataAndFilters(testData.sequenceHandler);
  session_handler.setFeatureTable(testData.sequenceHandler);
  BOOST_CHECK_EQUAL(session_handler.feature_table_headers.size(), 23);
  BOOST_CHECK_EQUAL(session_handler.feature_table_headers(0), "sample_name");
  BOOST_CHECK_EQUAL(session_handler.feature_table_headers(session_handler.feature_table_headers.size() - 1), "calculated_concentration");
  BOOST_CHECK_EQUAL(session_handler.feature_table_body.dimension(0), 1);
  BOOST_CHECK_EQUAL(session_handler.feature_table_body.dimension(1), 23);
  BOOST_CHECK_EQUAL(session_handler.feature_table_body(0, 0), "150516_CM1_Level1");
  BOOST_CHECK_EQUAL(session_handler.feature_table_body(session_handler.feature_table_body.dimension(0) - 1, session_handler.feature_table_body.dimension(1) - 1), "");
}

BOOST_AUTO_TEST_CASE(setFeatureMatrix1)
{
  TestData testData;
  SessionHandler session_handler;
  session_handler.setFeatureMatrix(testData.sequenceHandler);
}
/*
BOOST_AUTO_TEST_CASE(resetChromatogramRange1)
{
  SessionHandler session_handler;
  session_handler.resetChromatogramRange();
  BOOST_CHECK_CLOSE(session_handler.chrom_time_range.first, 0.0, 1e-3);
  BOOST_CHECK_CLOSE(session_handler.chrom_time_range.second, 1800.0, 1e-3);
}

BOOST_AUTO_TEST_CASE(setChromatogramScatterPlot1)
{
  TestData testData;
  SessionHandler session_handler;
  session_handler.setChromatogramScatterPlot(testData.sequenceHandler);
}
*/

BOOST_AUTO_TEST_CASE(resetSpectrumRange1)
{
  SessionHandler session_handler;
  session_handler.resetSpectrumRange();
  BOOST_CHECK_CLOSE(session_handler.spec_mz_range.first, 0.0, 1e-3);
  BOOST_CHECK_CLOSE(session_handler.spec_mz_range.second, 2000.0, 1e-3);
}

BOOST_AUTO_TEST_CASE(setSpectrumScatterPlot1)
{
  TestData testData;
  SessionHandler session_handler;
  session_handler.setSpectrumScatterPlot(testData.sequenceHandler);
}

BOOST_AUTO_TEST_CASE(setCalibratorsScatterLinePlot1)
{
  TestData testData;
  SessionHandler session_handler;
  session_handler.setCalibratorsScatterLinePlot(testData.sequenceHandler);
}

BOOST_AUTO_TEST_SUITE_END()
