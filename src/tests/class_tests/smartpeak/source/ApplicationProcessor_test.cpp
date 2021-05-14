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

#define BOOST_TEST_MODULE ApplicationProcessor test suite
#include <boost/test/included/unit_test.hpp>
#include <SmartPeak/core/ApplicationProcessor.h>
#include <SmartPeak/core/SequenceProcessor.h>
#include <SmartPeak/core/Filenames.h>
#include <filesystem>

using namespace SmartPeak;
using namespace std;

struct ApplicationProcessorFixture 
{
  /* ctor/dtor */
  ApplicationProcessorFixture() 
  {
    datapath_ = SMARTPEAK_GET_TEST_DATA_PATH("");
    auto workflow = std::filesystem::path{ datapath_ } / std::filesystem::path{ "workflow_csv_files" };

    filenames_ = Filenames::getDefaultStaticFilenames(workflow.string());
    filenames_.referenceData_csv_i = (std::filesystem::path{ datapath_ } / std::filesystem::path{ "MRMFeatureValidator_referenceData_1.csv" }).string();

    // Injections sequence:
    std::vector<InjectionHandler> seq;
    {
      seq.push_back(InjectionHandler{});
      seq.push_back(InjectionHandler{});
    }
    ah_.sequenceHandler_.setSequence(seq);
    // Sequence segments:
    std::vector<SequenceSegmentHandler> seq_seg;
    {
      seq_seg.push_back(SequenceSegmentHandler{});
      seq_seg.push_back(SequenceSegmentHandler{});
    }
    ah_.sequenceHandler_.setSequenceSegments(seq_seg);
    // Sample groups:
    std::vector<SampleGroupHandler> sample_groups;
    {
      sample_groups.push_back(SampleGroupHandler{});
      sample_groups.push_back(SampleGroupHandler{});
    }
    ah_.sequenceHandler_.setSampleGroups(sample_groups);
  }

  ~ApplicationProcessorFixture() {}

public:
  ApplicationHandler ah_;
  std::string datapath_;
  Filenames filenames_;
};

BOOST_FIXTURE_TEST_SUITE(ApplicationProcessor, ApplicationProcessorFixture)

/* CreateCommand */
BOOST_AUTO_TEST_CASE(CreateCommand_GetName)
{
  CreateCommand cmd{ ah_ };
  BOOST_CHECK_EQUAL(cmd.getName(), "CreateCommand");
}

BOOST_AUTO_TEST_CASE(CreateCommand_ProcessFailsOnEmptyCommand)
{
  CreateCommand cmd{ ah_ };
  cmd.name_ = "";
  auto created = cmd.process();
  BOOST_CHECK(!created);
}

BOOST_AUTO_TEST_CASE(CreateCommand_ProcessFailsOnWrongCommandName)
{
  CreateCommand cmd{ ah_ };
  cmd.name_ = "BAD_COMMAND_NAME";
  auto created = cmd.process();
  BOOST_CHECK(!created);
}

BOOST_AUTO_TEST_CASE(CreateCommand_ProcessSucceedsOnValidCommandName)
{
  CreateCommand cmd{ ah_ };
  // Raw data method
  {
    cmd.name_ = "LOAD_RAW_DATA";
    auto created = cmd.process();
    BOOST_CHECK(created);
  }
  // Sequence Segment Method
  {
    cmd.name_ = "LOAD_FEATURE_QCS";
    auto created = cmd.process();
    BOOST_CHECK(created);
  }
  // Sample Group Method
  {
    cmd.name_ = "MERGE_INJECTIONS";
    auto created = cmd.process();
    BOOST_CHECK(created);
  }
}

/* BuildCommandsFromNames */
BOOST_AUTO_TEST_CASE(BuildCommandsFromNames_Process)
{
  ApplicationHandler application_handler;
  BuildCommandsFromNames buildCommandsFromNames(application_handler);
  std::vector<ApplicationHandler::Command> methods;

  buildCommandsFromNames.names_ = {};
  BOOST_CHECK(buildCommandsFromNames.process());
  BOOST_CHECK_EQUAL(buildCommandsFromNames.commands_.size(), 0);

  buildCommandsFromNames.names_ = { "LOAD_RAW_DATA", "LOAD_FEATURES", "PICK_MRM_FEATURES" };
  BOOST_CHECK(buildCommandsFromNames.process());
  BOOST_CHECK_EQUAL(buildCommandsFromNames.commands_.size(), 3);
  BOOST_CHECK_EQUAL(buildCommandsFromNames.commands_.at(0).getName(), "LOAD_RAW_DATA");
  BOOST_CHECK_EQUAL(buildCommandsFromNames.commands_.at(1).getName(), "LOAD_FEATURES");
  BOOST_CHECK_EQUAL(buildCommandsFromNames.commands_.at(2).getName(), "PICK_MRM_FEATURES");

  buildCommandsFromNames.names_ = { "LOAD_RAW_DATA", "PLOT_FEATURES", "LOAD_FEATURES" }; // no plotting processor yet
  BOOST_CHECK(!buildCommandsFromNames.process());
  BOOST_CHECK_EQUAL(buildCommandsFromNames.commands_.size(), 2);
  BOOST_CHECK_EQUAL(buildCommandsFromNames.commands_.at(0).getName(), "LOAD_RAW_DATA");
  BOOST_CHECK_EQUAL(buildCommandsFromNames.commands_.at(1).getName(), "LOAD_FEATURES");

  buildCommandsFromNames.names_ = { "55", "87" };
  BOOST_CHECK(!buildCommandsFromNames.process());
  BOOST_CHECK_EQUAL(buildCommandsFromNames.commands_.size(), 0);
}

BOOST_AUTO_TEST_CASE(BuildCommandsFromNames_GetName)
{
  BuildCommandsFromNames cmd{ ah_ };
  BOOST_CHECK_EQUAL(cmd.getName(), "BuildCommandsFromNames");
}

/* LoadSessionFromSequence */
BOOST_AUTO_TEST_CASE(LoadSessionFromSequence_GetName)
{
  LoadSessionFromSequence cmd{ ah_ };
  BOOST_CHECK_EQUAL(cmd.getName(), "LoadSessionFromSequence");
}

BOOST_AUTO_TEST_CASE(LoadSessionFromSequence_ProcessSucceedsOnCorrectPath)
{
  LoadSessionFromSequence cmd{ ah_ };
  {
    cmd.pathname_ = filenames_.sequence_csv_i;
  }
  auto loaded = cmd.process();
  BOOST_CHECK(loaded);
}

/* LoadSequenceParameters */
BOOST_AUTO_TEST_CASE(LoadSequenceParameters_GetName)
{
  LoadSequenceParameters cmd{ ah_ };
  BOOST_CHECK_EQUAL(cmd.getName(), "LoadSequenceParameters");
}

BOOST_AUTO_TEST_CASE(LoadSequenceParameters_ProcessFailsOnEmptySequence)
{
  // Default ApplicationHandler has a default SequenceHnadler with empty sequence
  ApplicationHandler ah;
  LoadSequenceParameters cmd{ ah };
  BOOST_CHECK(!cmd.process());
}

BOOST_AUTO_TEST_CASE(LoadSequenceParameters_ProcessSucceedsOnNonEmptySequenceAndCorrectPath)
{
  LoadSequenceParameters cmd{ ah_ };
  {
    cmd.pathname_ = filenames_.parameters_csv_i;
  }
  auto loaded = cmd.process();
  BOOST_CHECK(loaded);
}

/* LoadSequenceTransitions */
BOOST_AUTO_TEST_CASE(LoadSequenceTransitions_GetName)
{
  LoadSequenceTransitions cmd{ ah_ };
  BOOST_CHECK_EQUAL(cmd.getName(), "LoadSequenceTransitions");
}

BOOST_AUTO_TEST_CASE(LoadSequenceTransitions_ProcessFailsOnEmptySequence)
{
  // Default ApplicationHandler has a default SequenceHnadler with empty sequence
  ApplicationHandler ah;
  LoadSequenceTransitions cmd{ ah };
  auto loaded = cmd.process();
  BOOST_CHECK(!loaded);
}

BOOST_AUTO_TEST_CASE(LoadSequenceTransitions_ProcessSucceedsOnNonEmptySequenceAndCorrectPath)
{
  LoadSequenceTransitions cmd{ ah_ };
  {
    cmd.pathname_ = filenames_.traML_csv_i;
  }
  auto loaded = cmd.process();
  BOOST_CHECK(loaded);
}

/* LoadSequenceValidationData */
BOOST_AUTO_TEST_CASE(LoadSequenceValidationData_GetName)
{
  LoadSequenceValidationData cmd{ ah_ };
  BOOST_CHECK_EQUAL(cmd.getName(), "LoadSequenceValidationData");
}

BOOST_AUTO_TEST_CASE(LoadSequenceValidationData_ProcessFailsOnEmptySequence)
{
  // Default ApplicationHandler has a default SequenceHnadler with empty sequence
  ApplicationHandler ah;
  LoadSequenceValidationData cmd{ ah };
  auto loaded = cmd.process();
  BOOST_CHECK(!loaded);
}

BOOST_AUTO_TEST_CASE(LoadSequenceValidationData_ProcessSucceedsOnNonEmptySequenceAndCorrectPath)
{
  LoadSequenceValidationData cmd{ ah_ };
  {
    cmd.pathname_ = filenames_.referenceData_csv_i;
  }
  auto loaded = cmd.process();
  BOOST_CHECK(loaded);
}

/* LoadSequenceSegmentQuantitationMethods */
BOOST_AUTO_TEST_CASE(LoadSequenceSegmentQuantitationMethods_GetName)
{
  LoadSequenceSegmentQuantitationMethods cmd{ ah_ };
  BOOST_CHECK_EQUAL(cmd.getName(), "LoadSequenceSegmentQuantitationMethods");
}

BOOST_AUTO_TEST_CASE(LoadSequenceSegmentQuantitationMethods_ProcessFailsOnEmptySequence)
{
  // Default ApplicationHandler has a default SequenceHnadler with empty sequence
  ApplicationHandler ah;
  LoadSequenceSegmentQuantitationMethods cmd{ ah };
  auto loaded = cmd.process();
  BOOST_CHECK(!loaded);
}

BOOST_AUTO_TEST_CASE(LoadSequenceSegmentQuantitationMethods_ProcessSucceedsOnNonEmptySequenceAndCorrectPath)
{
  LoadSequenceSegmentQuantitationMethods cmd{ ah_ };
  {
    cmd.pathname_ = filenames_.quantitationMethods_csv_i;
  }
  auto loaded = cmd.process();
  BOOST_CHECK(loaded);
}

/* LoadSequenceSegmentStandardsConcentrations */
BOOST_AUTO_TEST_CASE(LoadSequenceSegmentStandardsConcentrations_GetName)
{
  LoadSequenceSegmentStandardsConcentrations cmd{ ah_ };
  BOOST_CHECK_EQUAL(cmd.getName(), "LoadSequenceSegmentStandardsConcentrations");
}

BOOST_AUTO_TEST_CASE(LoadSequenceSegmentStandardsConcentrations_ProcessFailsOnEmptySequence)
{
  // Default ApplicationHandler has a default SequenceHnadler with empty sequence
  ApplicationHandler ah;
  LoadSequenceSegmentStandardsConcentrations cmd{ ah };
  auto loaded = cmd.process();
  BOOST_CHECK(!loaded);
}

BOOST_AUTO_TEST_CASE(LoadSequenceSegmentStandardsConcentrations_ProcessSucceedsOnNonEmptySequenceAndCorrectPath)
{
  LoadSequenceSegmentStandardsConcentrations cmd{ ah_ };
  {
    cmd.pathname_ = filenames_.standardsConcentrations_csv_i;
  }
  auto loaded = cmd.process();
  BOOST_CHECK(loaded);
}

/* LoadSequenceSegmentFeatureFilterComponents */
BOOST_AUTO_TEST_CASE(LoadSequenceSegmentFeatureFilterComponents_GetName)
{
  LoadSequenceSegmentFeatureFilterComponents cmd{ ah_ };
  BOOST_CHECK_EQUAL(cmd.getName(), "LoadSequenceSegmentFeatureFilterComponents");
}

BOOST_AUTO_TEST_CASE(LoadSequenceSegmentFeatureFilterComponents_ProcessFailsOnEmptySequence)
{
  // Default ApplicationHandler has a default SequenceHnadler with empty sequence
  ApplicationHandler ah;
  LoadSequenceSegmentFeatureFilterComponents cmd{ ah };
  auto loaded = cmd.process();
  BOOST_CHECK(!loaded);
}

BOOST_AUTO_TEST_CASE(LoadSequenceSegmentFeatureFilterComponents_ProcessSucceedsOnNonEmptySequenceAndCorrectPath)
{
  LoadSequenceSegmentFeatureFilterComponents cmd{ ah_ };
  {
    cmd.pathname_ = filenames_.featureFilterComponents_csv_i;
  }
  auto loaded = cmd.process();
  BOOST_CHECK(loaded);
}

/* LoadSequenceSegmentFeatureFilterComponentGroups */
BOOST_AUTO_TEST_CASE(LoadSequenceSegmentFeatureFilterComponentGroups_GetName)
{
  LoadSequenceSegmentFeatureFilterComponentGroups cmd{ ah_ };
  BOOST_CHECK_EQUAL(cmd.getName(), "LoadSequenceSegmentFeatureFilterComponentGroups");
}

BOOST_AUTO_TEST_CASE(LoadSequenceSegmentFeatureFilterComponentGroups_ProcessFailsOnEmptySequence)
{
  // Default ApplicationHandler has a default SequenceHnadler with empty sequence
  ApplicationHandler ah;
  LoadSequenceSegmentFeatureFilterComponentGroups cmd{ ah };
  auto loaded = cmd.process();
  BOOST_CHECK(!loaded);
}

BOOST_AUTO_TEST_CASE(LoadSequenceSegmentFeatureFilterComponentGroups_ProcessSucceedsOnNonEmptySequenceAndCorrectPath)
{
  LoadSequenceSegmentFeatureFilterComponentGroups cmd{ ah_ };
  {
    cmd.pathname_ = filenames_.featureFilterComponentGroups_csv_i;
  }
  auto loaded = cmd.process();
  BOOST_CHECK(loaded);
}

/* LoadSequenceSegmentFeatureQCComponents */
BOOST_AUTO_TEST_CASE(LoadSequenceSegmentFeatureQCComponents_GetName)
{
  LoadSequenceSegmentFeatureQCComponents cmd{ ah_ };
  BOOST_CHECK_EQUAL(cmd.getName(), "LoadSequenceSegmentFeatureQCComponents");
}

BOOST_AUTO_TEST_CASE(LoadSequenceSegmentFeatureQCComponents_ProcessFailsOnEmptySequence)
{
  // Default ApplicationHandler has a default SequenceHnadler with empty sequence
  ApplicationHandler ah;
  LoadSequenceSegmentFeatureQCComponents cmd{ ah };
  auto loaded = cmd.process();
  BOOST_CHECK(!loaded);
}

BOOST_AUTO_TEST_CASE(LoadSequenceSegmentFeatureQCComponents_ProcessSucceedsOnNonEmptySequenceAndCorrectPath)
{
  LoadSequenceSegmentFeatureQCComponents cmd{ ah_ };
  {
    cmd.pathname_ = filenames_.featureQCComponents_csv_i;
  }
  auto loaded = cmd.process();
  BOOST_CHECK(loaded);
}

/* LoadSequenceSegmentFeatureQCComponentGroups */
BOOST_AUTO_TEST_CASE(LoadSequenceSegmentFeatureQCComponentGroups_GetName)
{
  LoadSequenceSegmentFeatureQCComponentGroups cmd{ ah_ };
  BOOST_CHECK_EQUAL(cmd.getName(), "LoadSequenceSegmentFeatureQCComponentGroups");
}

BOOST_AUTO_TEST_CASE(LoadSequenceSegmentFeatureQCComponentGroups_ProcessFailsOnEmptySequence)
{
  // Default ApplicationHandler has a default SequenceHnadler with empty sequence
  ApplicationHandler ah;
  LoadSequenceSegmentFeatureQCComponentGroups cmd{ ah };
  auto loaded = cmd.process();
  BOOST_CHECK(!loaded);
}

BOOST_AUTO_TEST_CASE(LoadSequenceSegmentFeatureQCComponentGroups_ProcessSucceedsOnNonEmptySequenceAndCorrectPath)
{
  LoadSequenceSegmentFeatureQCComponentGroups cmd{ ah_ };
  {
    cmd.pathname_ = filenames_.featureQCComponentGroups_csv_i;
  }
  auto loaded = cmd.process();
  BOOST_CHECK(loaded);
}

/* LoadSequenceSegmentFeatureRSDFilterComponents */
BOOST_AUTO_TEST_CASE(LoadSequenceSegmentFeatureRSDFilterComponents_GetName)
{
  LoadSequenceSegmentFeatureRSDFilterComponents cmd{ ah_ };
  BOOST_CHECK_EQUAL(cmd.getName(), "LoadSequenceSegmentFeatureRSDFilterComponents");
}

BOOST_AUTO_TEST_CASE(LoadSequenceSegmentFeatureRSDFilterComponents_ProcessFailsOnEmptySequence)
{
  // Default ApplicationHandler has a default SequenceHnadler with empty sequence
  ApplicationHandler ah;
  LoadSequenceSegmentFeatureRSDFilterComponents cmd{ ah };
  auto loaded = cmd.process();
  BOOST_CHECK(!loaded);
}

BOOST_AUTO_TEST_CASE(LoadSequenceSegmentFeatureRSDFilterComponents_ProcessSucceedsOnNonEmptySequenceAndCorrectPath)
{
  LoadSequenceSegmentFeatureRSDFilterComponents cmd{ ah_ };
  {
    cmd.pathname_ = filenames_.featureRSDFilterComponents_csv_i;
  }
  auto loaded = cmd.process();
  BOOST_CHECK(loaded);
}

/* LoadSequenceSegmentFeatureRSDFilterComponentGroups */
BOOST_AUTO_TEST_CASE(LoadSequenceSegmentFeatureRSDFilterComponentGroups_GetName)
{
  LoadSequenceSegmentFeatureRSDFilterComponentGroups cmd{ ah_ };
  BOOST_CHECK_EQUAL(cmd.getName(), "LoadSequenceSegmentFeatureRSDFilterComponentGroups");
}

BOOST_AUTO_TEST_CASE(LoadSequenceSegmentFeatureRSDFilterComponentGroups_ProcessFailsOnEmptySequence)
{
  // Default ApplicationHandler has a default SequenceHnadler with empty sequence
  ApplicationHandler ah;
  LoadSequenceSegmentFeatureRSDFilterComponentGroups cmd{ ah };
  auto loaded = cmd.process();
  BOOST_CHECK(!loaded);
}

BOOST_AUTO_TEST_CASE(LoadSequenceSegmentFeatureRSDFilterComponentGroups_ProcessSucceedsOnNonEmptySequenceAndCorrectPath)
{
  LoadSequenceSegmentFeatureRSDFilterComponentGroups cmd{ ah_ };
  {
    cmd.pathname_ = filenames_.featureRSDFilterComponentGroups_csv_i;
  }
  auto loaded = cmd.process();
  BOOST_CHECK(loaded);
}

/* LoadSequenceSegmentFeatureRSDQCComponents */
BOOST_AUTO_TEST_CASE(LoadSequenceSegmentFeatureRSDQCComponents_GetName)
{
  LoadSequenceSegmentFeatureRSDQCComponents cmd{ ah_ };
  BOOST_CHECK_EQUAL(cmd.getName(), "LoadSequenceSegmentFeatureRSDQCComponents");
}

BOOST_AUTO_TEST_CASE(LoadSequenceSegmentFeatureRSDQCComponents_ProcessFailsOnEmptySequence)
{
  // Default ApplicationHandler has a default SequenceHnadler with empty sequence
  ApplicationHandler ah;
  LoadSequenceSegmentFeatureRSDQCComponents cmd{ ah };
  auto loaded = cmd.process();
  BOOST_CHECK(!loaded);
}

BOOST_AUTO_TEST_CASE(LoadSequenceSegmentFeatureRSDQCComponents_ProcessSucceedsOnNonEmptySequenceAndCorrectPath)
{
  LoadSequenceSegmentFeatureRSDQCComponents cmd{ ah_ };
  {
    cmd.pathname_ = filenames_.featureRSDQCComponents_csv_i;
  }
  auto loaded = cmd.process();
  BOOST_CHECK(loaded);
}

/* LoadSequenceSegmentFeatureRSDQCComponentGroups */
BOOST_AUTO_TEST_CASE(LoadSequenceSegmentFeatureRSDQCComponentGroups_GetName)
{
  LoadSequenceSegmentFeatureRSDQCComponentGroups cmd{ ah_ };
  BOOST_CHECK_EQUAL(cmd.getName(), "LoadSequenceSegmentFeatureRSDQCComponentGroups");
}

BOOST_AUTO_TEST_CASE(LoadSequenceSegmentFeatureRSDQCComponentGroups_ProcessFailsOnEmptySequence)
{
  // Default ApplicationHandler has a default SequenceHnadler with empty sequence
  ApplicationHandler ah;
  LoadSequenceSegmentFeatureRSDQCComponentGroups cmd{ ah };
  auto loaded = cmd.process();
  BOOST_CHECK(!loaded);
}

BOOST_AUTO_TEST_CASE(LoadSequenceSegmentFeatureRSDQCComponentGroups_ProcessSucceedsOnNonEmptySequenceAndCorrectPath)
{
  LoadSequenceSegmentFeatureRSDQCComponentGroups cmd{ ah_ };
  {
    cmd.pathname_ = filenames_.featureRSDQCComponentGroups_csv_i;
  }
  auto loaded = cmd.process();
  BOOST_CHECK(loaded);
}

/* LoadSequenceSegmentFeatureBackgroundFilterComponents */
BOOST_AUTO_TEST_CASE(LoadSequenceSegmentFeatureBackgroundFilterComponents_GetName)
{
  LoadSequenceSegmentFeatureBackgroundFilterComponents cmd{ ah_ };
  BOOST_CHECK_EQUAL(cmd.getName(), "LoadSequenceSegmentFeatureBackgroundFilterComponents");
}

BOOST_AUTO_TEST_CASE(LoadSequenceSegmentFeatureBackgroundFilterComponents_ProcessFailsOnEmptySequence)
{
  // Default ApplicationHandler has a default SequenceHnadler with empty sequence
  ApplicationHandler ah;
  LoadSequenceSegmentFeatureBackgroundFilterComponents cmd{ ah };
  auto loaded = cmd.process();
  BOOST_CHECK(!loaded);
}

BOOST_AUTO_TEST_CASE(LoadSequenceSegmentFeatureBackgroundFilterComponents_ProcessSucceedsOnNonEmptySequenceAndCorrectPath)
{
  LoadSequenceSegmentFeatureBackgroundFilterComponents cmd{ ah_ };
  {
    cmd.pathname_ = filenames_.featureBackgroundFilterComponents_csv_i;
  }
  auto loaded = cmd.process();
  BOOST_CHECK(loaded);
}

/* LoadSequenceSegmentFeatureBackgroundFilterComponentGroups */
BOOST_AUTO_TEST_CASE(LoadSequenceSegmentFeatureBackgroundFilterComponentGroups_GetName)
{
  LoadSequenceSegmentFeatureBackgroundFilterComponentGroups cmd{ ah_ };
  BOOST_CHECK_EQUAL(cmd.getName(), "LoadSequenceSegmentFeatureBackgroundFilterComponentGroups");
}

BOOST_AUTO_TEST_CASE(LoadSequenceSegmentFeatureBackgroundFilterComponentGroups_ProcessFailsOnEmptySequence)
{
  // Default ApplicationHandler has a default SequenceHnadler with empty sequence
  ApplicationHandler ah;
  LoadSequenceSegmentFeatureBackgroundFilterComponentGroups cmd{ ah };
  auto loaded = cmd.process();
  BOOST_CHECK(!loaded);
}

BOOST_AUTO_TEST_CASE(LoadSequenceSegmentFeatureBackgroundFilterComponentGroups_ProcessSucceedsOnNonEmptySequenceAndCorrectPath)
{
  LoadSequenceSegmentFeatureBackgroundFilterComponentGroups cmd{ ah_ };
  {
    cmd.pathname_ = filenames_.featureBackgroundFilterComponentGroups_csv_i;
  }
  auto loaded = cmd.process();
  BOOST_CHECK(loaded);
}

/* LoadSequenceSegmentFeatureBackgroundQCComponents */
BOOST_AUTO_TEST_CASE(LoadSequenceSegmentFeatureBackgroundQCComponents_GetName)
{
  LoadSequenceSegmentFeatureBackgroundQCComponents cmd{ ah_ };
  BOOST_CHECK_EQUAL(cmd.getName(), "LoadSequenceSegmentFeatureBackgroundQCComponents");
}

BOOST_AUTO_TEST_CASE(LoadSequenceSegmentFeatureBackgroundQCComponents_ProcessFailsOnEmptySequence)
{
  // Default ApplicationHandler has a default SequenceHnadler with empty sequence
  ApplicationHandler ah;
  LoadSequenceSegmentFeatureBackgroundQCComponents cmd{ ah };
  auto loaded = cmd.process();
  BOOST_CHECK(!loaded);
}

BOOST_AUTO_TEST_CASE(LoadSequenceSegmentFeatureBackgroundQCComponents_ProcessSucceedsOnNonEmptySequenceAndCorrectPath)
{
  LoadSequenceSegmentFeatureBackgroundQCComponents cmd{ ah_ };
  {
    cmd.pathname_ = filenames_.featureBackgroundQCComponents_csv_i;
  }
  auto loaded = cmd.process();
  BOOST_CHECK(loaded);
}

/* LoadSequenceSegmentFeatureBackgroundQCComponentGroups */
BOOST_AUTO_TEST_CASE(LoadSequenceSegmentFeatureBackgroundQCComponentGroups_GetName)
{
  LoadSequenceSegmentFeatureBackgroundQCComponentGroups cmd{ ah_ };
  BOOST_CHECK_EQUAL(cmd.getName(), "LoadSequenceSegmentFeatureBackgroundQCComponentGroups");
}

BOOST_AUTO_TEST_CASE(LoadSequenceSegmentFeatureBackgroundQCComponentGroups_ProcessFailsOnEmptySequence)
{
  // Default ApplicationHandler has a default SequenceHnadler with empty sequence
  ApplicationHandler ah;
  LoadSequenceSegmentFeatureBackgroundQCComponentGroups cmd{ ah };
  auto loaded = cmd.process();
  BOOST_CHECK(!loaded);
}

BOOST_AUTO_TEST_CASE(LoadSequenceSegmentFeatureBackgroundQCComponentGroups_ProcessSucceedsOnNonEmptySequenceAndCorrectPath)
{
  LoadSequenceSegmentFeatureBackgroundQCComponentGroups cmd{ ah_ };
  {
    cmd.pathname_ = filenames_.featureBackgroundQCComponentGroups_csv_i;
  }
  auto loaded = cmd.process();
  BOOST_CHECK(loaded);
}

/* StoreSequenceFileAnalyst */
BOOST_AUTO_TEST_CASE(StoreSequenceFileAnalyst_GetName)
{
  StoreSequenceFileAnalyst cmd{ ah_ };
  BOOST_CHECK_EQUAL(cmd.getName(), "StoreSequenceFileAnalyst");
}

BOOST_AUTO_TEST_CASE(StoreSequenceFileAnalyst_ProcessFailsOnEmptySequence)
{
  // Default ApplicationHandler has a default SequenceHnadler with empty sequence
  ApplicationHandler ah;
  StoreSequenceFileAnalyst cmd{ ah };
  auto store = cmd.process();
  BOOST_CHECK(!store);
}

BOOST_AUTO_TEST_CASE(StoreSequenceFileAnalyst_ProcessSucceedsOnNonEmptySequence)
{
  StoreSequenceFileAnalyst cmd{ ah_ };
  auto store = cmd.process();
  BOOST_CHECK(store);
}

/* StoreSequenceFileMasshunter */
BOOST_AUTO_TEST_CASE(StoreSequenceFileMasshunter_GetName)
{
  StoreSequenceFileMasshunter cmd{ ah_ };
  BOOST_CHECK_EQUAL(cmd.getName(), "StoreSequenceFileMasshunter");
}

BOOST_AUTO_TEST_CASE(StoreSequenceFileMasshunter_ProcessFailsOnEmptySequence)
{
  // Default ApplicationHandler has a default SequenceHnadler with empty sequence
  ApplicationHandler ah;
  StoreSequenceFileMasshunter cmd{ ah };
  auto store = cmd.process();
  BOOST_CHECK(!store);
}

BOOST_AUTO_TEST_CASE(StoreSequenceFileMasshunter_ProcessSucceedsOnNonEmptySequence)
{
  StoreSequenceFileMasshunter cmd{ ah_ };
  auto store = cmd.process();
  BOOST_CHECK(store);
}

/* StoreSequenceFileXcalibur */
BOOST_AUTO_TEST_CASE(StoreSequenceFileXcalibur_GetName)
{
  StoreSequenceFileXcalibur cmd{ ah_ };
  BOOST_CHECK_EQUAL(cmd.getName(), "StoreSequenceFileXcalibur");
}

BOOST_AUTO_TEST_CASE(StoreSequenceFileXcalibur_ProcessFailsOnEmptySequence)
{
  // Default ApplicationHandler has a default SequenceHnadler with empty sequence
  ApplicationHandler ah;
  StoreSequenceFileXcalibur cmd{ ah };
  auto store = cmd.process();
  BOOST_CHECK(!store);
}

BOOST_AUTO_TEST_CASE(StoreSequenceFileXcalibur_ProcessSucceedsOnNonEmptySequence)
{
  StoreSequenceFileXcalibur cmd{ ah_ };
  auto store = cmd.process();
  BOOST_CHECK(store);
}

/* SetRawDataPathname */
BOOST_AUTO_TEST_CASE(SetRawDataPathname_GetName)
{
  SetRawDataPathname cmd{ ah_ };
  BOOST_CHECK_EQUAL(cmd.getName(), "SetRawDataPathname");
}

BOOST_AUTO_TEST_CASE(SetRawDataPathname_ProcessSetsPath)
{
  SetRawDataPathname cmd{ ah_ };
  {
    cmd.pathname_ = "path/to/directory";
  }
  auto set = cmd.process();
  BOOST_CHECK(set);
  BOOST_CHECK_EQUAL(ah_.mzML_dir_, cmd.pathname_);
}

/* SetInputFeaturesPathname */
BOOST_AUTO_TEST_CASE(SetInputFeaturesPathname_GetName)
{
  SetInputFeaturesPathname cmd{ ah_ };
  BOOST_CHECK_EQUAL(cmd.getName(), "SetInputFeaturesPathname");
}

BOOST_AUTO_TEST_CASE(SetInputFeaturesPathname_ProcessSetsPath)
{
  SetInputFeaturesPathname cmd{ ah_ };
  {
    cmd.pathname_ = "path/to/directory";
  }
  auto set = cmd.process();
  BOOST_CHECK(set);
  BOOST_CHECK_EQUAL(ah_.features_in_dir_, cmd.pathname_);
}

/* SetOutputFeaturesPathname */
BOOST_AUTO_TEST_CASE(SetOutputFeaturesPathname_GetName)
{
  SetOutputFeaturesPathname cmd{ ah_ };
  BOOST_CHECK_EQUAL(cmd.getName(), "SetOutputFeaturesPathname");
}

BOOST_AUTO_TEST_CASE(SetOutputFeaturesPathname_ProcessSetsPath)
{
  SetOutputFeaturesPathname cmd{ ah_ };
  {
    cmd.pathname_ = "path/to/directory";
  }
  auto set = cmd.process();
  BOOST_CHECK(set);
  BOOST_CHECK_EQUAL(ah_.features_out_dir_, cmd.pathname_);
}

BOOST_AUTO_TEST_CASE(StoreSequenceWorkflow1)
{
  namespace fs = std::filesystem;
  ApplicationHandler application_handler;
  std::vector<std::string> command_names = {
    "LOAD_RAW_DATA",
    "MAP_CHROMATOGRAMS",
    "EXTRACT_CHROMATOGRAM_WINDOWS",
    "ZERO_CHROMATOGRAM_BASELINE",
    "PICK_MRM_FEATURES",
    "QUANTIFY_FEATURES",
    "CHECK_FEATURES",
    "SELECT_FEATURES",
    "STORE_FEATURES"
  };
  application_handler.sequenceHandler_.setWorkflow(command_names);
  StoreSequenceWorkflow application_processor(application_handler);
  BOOST_CHECK_EQUAL(application_processor.getName(), "StoreSequenceWorkflow");
  application_processor.pathname_ = (SMARTPEAK_GET_TEST_DATA_PATH("ApplicationProcessor_workflow.csv"));
  //application_processor.pathname_ = std::tmpnam(nullptr);
  BOOST_REQUIRE(application_processor.process());
  // compare with reference file
  const string reference_filename = SMARTPEAK_GET_TEST_DATA_PATH("ApplicationProcessor_workflow.csv");
  std::ifstream created_if(application_processor.pathname_);
  std::ifstream reference_if(reference_filename);
  std::istream_iterator<char> created_is(created_if), created_end;
  std::istream_iterator<char> reference_is(reference_if), reference_end;
  BOOST_CHECK_EQUAL_COLLECTIONS(created_is, created_end, reference_is, reference_end);
}

BOOST_AUTO_TEST_CASE(LoadSequenceWorkflow1)
{
  ApplicationHandler application_handler;
  LoadSequenceWorkflow application_processor(application_handler);
  BOOST_CHECK_EQUAL(application_processor.getName(), "LoadSequenceWorkflow");
  application_processor.pathname_ = SMARTPEAK_GET_TEST_DATA_PATH("ApplicationProcessor_workflow.csv");
  BOOST_REQUIRE(application_processor.process());
  const auto& commands = application_handler.sequenceHandler_.getWorkflow();
  std::vector<std::string> expected_command_names = {
    "LOAD_RAW_DATA",
    "MAP_CHROMATOGRAMS",
    "EXTRACT_CHROMATOGRAM_WINDOWS",
    "ZERO_CHROMATOGRAM_BASELINE",
    "PICK_MRM_FEATURES",
    "QUANTIFY_FEATURES",
    "CHECK_FEATURES",
    "SELECT_FEATURES",
    "STORE_FEATURES"
  };
  BOOST_REQUIRE(commands.size() == expected_command_names.size());
  for (auto i=0; i< expected_command_names.size(); ++i)
  {
    BOOST_CHECK_EQUAL(expected_command_names[i], commands[i]);
  }
}

BOOST_AUTO_TEST_SUITE_END()
