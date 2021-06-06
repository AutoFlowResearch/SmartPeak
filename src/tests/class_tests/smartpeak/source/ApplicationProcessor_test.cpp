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
#include <SmartPeak/core/ApplicationProcessor.h>
#include <SmartPeak/core/SequenceProcessor.h>
#include <SmartPeak/core/Filenames.h>
#include <filesystem>

using namespace SmartPeak;
using namespace std;

struct ApplicationProcessorFixture : public ::testing::Test
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

//BOOST_FIXTURE_TEST_SUITE(ApplicationProcessor, ApplicationProcessorFixture)

/* CreateCommand */
TEST_F(ApplicationProcessorFixture, CreateCommand_GetName)
{
  CreateCommand cmd{ ah_ };
  EXPECT_STREQ(cmd.getName().c_str(), "CreateCommand");
}

TEST_F(ApplicationProcessorFixture, CreateCommand_ProcessFailsOnEmptyCommand)
{
  CreateCommand cmd{ ah_ };
  cmd.name_ = "";
  auto created = cmd.process();
  EXPECT_TRUE(!created);
}

TEST_F(ApplicationProcessorFixture, CreateCommand_ProcessFailsOnWrongCommandName)
{
  CreateCommand cmd{ ah_ };
  cmd.name_ = "BAD_COMMAND_NAME";
  auto created = cmd.process();
  EXPECT_TRUE(!created);
}

TEST_F(ApplicationProcessorFixture, CreateCommand_ProcessSucceedsOnValidCommandName)
{
  CreateCommand cmd{ ah_ };
  // Raw data method
  {
    cmd.name_ = "LOAD_RAW_DATA";
    auto created = cmd.process();
    EXPECT_TRUE(created);
  }
  // Sequence Segment Method
  {
    cmd.name_ = "LOAD_FEATURE_QCS";
    auto created = cmd.process();
    EXPECT_TRUE(created);
  }
  // Sample Group Method
  {
    cmd.name_ = "MERGE_INJECTIONS";
    auto created = cmd.process();
    EXPECT_TRUE(created);
  }
}

/* BuildCommandsFromNames */
TEST_F(ApplicationProcessorFixture, BuildCommandsFromNames_Process)
{
  ApplicationHandler application_handler;
  BuildCommandsFromNames buildCommandsFromNames(application_handler);
  std::vector<ApplicationHandler::Command> methods;

  buildCommandsFromNames.names_ = {};
  EXPECT_TRUE(buildCommandsFromNames.process());
  EXPECT_EQ(buildCommandsFromNames.commands_.size(), 0);

  buildCommandsFromNames.names_ = { "LOAD_RAW_DATA", "LOAD_FEATURES", "PICK_MRM_FEATURES" };
  EXPECT_TRUE(buildCommandsFromNames.process());
  EXPECT_EQ(buildCommandsFromNames.commands_.size(), 3);
  EXPECT_STREQ(buildCommandsFromNames.commands_.at(0).getName().c_str(), "LOAD_RAW_DATA");
  EXPECT_STREQ(buildCommandsFromNames.commands_.at(1).getName().c_str(), "LOAD_FEATURES");
  EXPECT_STREQ(buildCommandsFromNames.commands_.at(2).getName().c_str(), "PICK_MRM_FEATURES");

  buildCommandsFromNames.names_ = { "LOAD_RAW_DATA", "PLOT_FEATURES", "LOAD_FEATURES" }; // no plotting processor yet
  EXPECT_TRUE(!buildCommandsFromNames.process());
  EXPECT_EQ(buildCommandsFromNames.commands_.size(), 2);
  EXPECT_STREQ(buildCommandsFromNames.commands_.at(0).getName().c_str(), "LOAD_RAW_DATA");
  EXPECT_STREQ(buildCommandsFromNames.commands_.at(1).getName().c_str(), "LOAD_FEATURES");

  buildCommandsFromNames.names_ = { "55", "87" };
  EXPECT_TRUE(!buildCommandsFromNames.process());
  EXPECT_EQ(buildCommandsFromNames.commands_.size(), 0);
}

TEST_F(ApplicationProcessorFixture, BuildCommandsFromNames_GetName)
{
  BuildCommandsFromNames cmd{ ah_ };
  EXPECT_STREQ(cmd.getName().c_str(), "BuildCommandsFromNames");
}

/* LoadSessionFromSequence */
TEST_F(ApplicationProcessorFixture, LoadSessionFromSequence_GetName)
{
  LoadSessionFromSequence cmd{ ah_ };
  EXPECT_STREQ(cmd.getName().c_str(), "LoadSessionFromSequence");
}

TEST_F(ApplicationProcessorFixture, LoadSessionFromSequence_ProcessSucceedsOnCorrectPath)
{
  LoadSessionFromSequence cmd{ ah_ };
  {
    cmd.pathname_ = filenames_.sequence_csv_i;
  }
  auto loaded = cmd.process();
  EXPECT_TRUE(loaded);
}

/* LoadSequenceParameters */
TEST_F(ApplicationProcessorFixture, LoadSequenceParameters_GetName)
{
  LoadSequenceParameters cmd{ ah_ };
  EXPECT_STREQ(cmd.getName().c_str(), "LoadSequenceParameters");
}

TEST_F(ApplicationProcessorFixture, LoadSequenceParameters_ProcessFailsOnEmptySequence)
{
  // Default ApplicationHandler has a default SequenceHnadler with empty sequence
  ApplicationHandler ah;
  LoadSequenceParameters cmd{ ah };
  EXPECT_TRUE(!cmd.process());
}

TEST_F(ApplicationProcessorFixture, LoadSequenceParameters_ProcessSucceedsOnNonEmptySequenceAndCorrectPath)
{
  LoadSequenceParameters cmd{ ah_ };
  {
    cmd.pathname_ = filenames_.parameters_csv_i;
  }
  auto loaded = cmd.process();
  EXPECT_TRUE(loaded);
}

/* LoadSequenceTransitions */
TEST_F(ApplicationProcessorFixture, LoadSequenceTransitions_GetName)
{
  LoadSequenceTransitions cmd{ ah_ };
  EXPECT_STREQ(cmd.getName().c_str(), "LoadSequenceTransitions");
}

TEST_F(ApplicationProcessorFixture, LoadSequenceTransitions_ProcessFailsOnEmptySequence)
{
  // Default ApplicationHandler has a default SequenceHnadler with empty sequence
  ApplicationHandler ah;
  LoadSequenceTransitions cmd{ ah };
  auto loaded = cmd.process();
  EXPECT_TRUE(!loaded);
}

TEST_F(ApplicationProcessorFixture, LoadSequenceTransitions_ProcessSucceedsOnNonEmptySequenceAndCorrectPath)
{
  LoadSequenceTransitions cmd{ ah_ };
  {
    cmd.pathname_ = filenames_.traML_csv_i;
  }
  auto loaded = cmd.process();
  EXPECT_TRUE(loaded);
}

/* LoadSequenceValidationData */
TEST_F(ApplicationProcessorFixture, LoadSequenceValidationData_GetName)
{
  LoadSequenceValidationData cmd{ ah_ };
  EXPECT_STREQ(cmd.getName().c_str(), "LoadSequenceValidationData");
}

TEST_F(ApplicationProcessorFixture, LoadSequenceValidationData_ProcessFailsOnEmptySequence)
{
  // Default ApplicationHandler has a default SequenceHnadler with empty sequence
  ApplicationHandler ah;
  LoadSequenceValidationData cmd{ ah };
  auto loaded = cmd.process();
  EXPECT_TRUE(!loaded);
}

TEST_F(ApplicationProcessorFixture, LoadSequenceValidationData_ProcessSucceedsOnNonEmptySequenceAndCorrectPath)
{
  LoadSequenceValidationData cmd{ ah_ };
  {
    cmd.pathname_ = filenames_.referenceData_csv_i;
  }
  auto loaded = cmd.process();
  EXPECT_TRUE(loaded);
}

/* LoadSequenceSegmentQuantitationMethods */
TEST_F(ApplicationProcessorFixture, LoadSequenceSegmentQuantitationMethods_GetName)
{
  LoadSequenceSegmentQuantitationMethods cmd{ ah_ };
  EXPECT_STREQ(cmd.getName().c_str(), "LoadSequenceSegmentQuantitationMethods");
}

TEST_F(ApplicationProcessorFixture, LoadSequenceSegmentQuantitationMethods_ProcessFailsOnEmptySequence)
{
  // Default ApplicationHandler has a default SequenceHnadler with empty sequence
  ApplicationHandler ah;
  LoadSequenceSegmentQuantitationMethods cmd{ ah };
  auto loaded = cmd.process();
  EXPECT_TRUE(!loaded);
}

TEST_F(ApplicationProcessorFixture, LoadSequenceSegmentQuantitationMethods_ProcessSucceedsOnNonEmptySequenceAndCorrectPath)
{
  LoadSequenceSegmentQuantitationMethods cmd{ ah_ };
  {
    cmd.pathname_ = filenames_.quantitationMethods_csv_i;
  }
  auto loaded = cmd.process();
  EXPECT_TRUE(loaded);
}

/* LoadSequenceSegmentStandardsConcentrations */
TEST_F(ApplicationProcessorFixture, LoadSequenceSegmentStandardsConcentrations_GetName)
{
  LoadSequenceSegmentStandardsConcentrations cmd{ ah_ };
  EXPECT_STREQ(cmd.getName().c_str(), "LoadSequenceSegmentStandardsConcentrations");
}

TEST_F(ApplicationProcessorFixture, LoadSequenceSegmentStandardsConcentrations_ProcessFailsOnEmptySequence)
{
  // Default ApplicationHandler has a default SequenceHnadler with empty sequence
  ApplicationHandler ah;
  LoadSequenceSegmentStandardsConcentrations cmd{ ah };
  auto loaded = cmd.process();
  EXPECT_TRUE(!loaded);
}

TEST_F(ApplicationProcessorFixture, LoadSequenceSegmentStandardsConcentrations_ProcessSucceedsOnNonEmptySequenceAndCorrectPath)
{
  LoadSequenceSegmentStandardsConcentrations cmd{ ah_ };
  {
    cmd.pathname_ = filenames_.standardsConcentrations_csv_i;
  }
  auto loaded = cmd.process();
  EXPECT_TRUE(loaded);
}

/* LoadSequenceSegmentFeatureFilterComponents */
TEST_F(ApplicationProcessorFixture, LoadSequenceSegmentFeatureFilterComponents_GetName)
{
  LoadSequenceSegmentFeatureFilterComponents cmd{ ah_ };
  EXPECT_STREQ(cmd.getName().c_str(), "LoadSequenceSegmentFeatureFilterComponents");
}

TEST_F(ApplicationProcessorFixture, LoadSequenceSegmentFeatureFilterComponents_ProcessFailsOnEmptySequence)
{
  // Default ApplicationHandler has a default SequenceHnadler with empty sequence
  ApplicationHandler ah;
  LoadSequenceSegmentFeatureFilterComponents cmd{ ah };
  auto loaded = cmd.process();
  EXPECT_TRUE(!loaded);
}

TEST_F(ApplicationProcessorFixture, LoadSequenceSegmentFeatureFilterComponents_ProcessSucceedsOnNonEmptySequenceAndCorrectPath)
{
  LoadSequenceSegmentFeatureFilterComponents cmd{ ah_ };
  {
    cmd.pathname_ = filenames_.featureFilterComponents_csv_i;
  }
  auto loaded = cmd.process();
  EXPECT_TRUE(loaded);
}

/* LoadSequenceSegmentFeatureFilterComponentGroups */
TEST_F(ApplicationProcessorFixture, LoadSequenceSegmentFeatureFilterComponentGroups_GetName)
{
  LoadSequenceSegmentFeatureFilterComponentGroups cmd{ ah_ };
  EXPECT_STREQ(cmd.getName().c_str(), "LoadSequenceSegmentFeatureFilterComponentGroups");
}

TEST_F(ApplicationProcessorFixture, LoadSequenceSegmentFeatureFilterComponentGroups_ProcessFailsOnEmptySequence)
{
  // Default ApplicationHandler has a default SequenceHnadler with empty sequence
  ApplicationHandler ah;
  LoadSequenceSegmentFeatureFilterComponentGroups cmd{ ah };
  auto loaded = cmd.process();
  EXPECT_TRUE(!loaded);
}

TEST_F(ApplicationProcessorFixture, LoadSequenceSegmentFeatureFilterComponentGroups_ProcessSucceedsOnNonEmptySequenceAndCorrectPath)
{
  LoadSequenceSegmentFeatureFilterComponentGroups cmd{ ah_ };
  {
    cmd.pathname_ = filenames_.featureFilterComponentGroups_csv_i;
  }
  auto loaded = cmd.process();
  EXPECT_TRUE(loaded);
}

/* LoadSequenceSegmentFeatureQCComponents */
TEST_F(ApplicationProcessorFixture, LoadSequenceSegmentFeatureQCComponents_GetName)
{
  LoadSequenceSegmentFeatureQCComponents cmd{ ah_ };
  EXPECT_STREQ(cmd.getName().c_str(), "LoadSequenceSegmentFeatureQCComponents");
}

TEST_F(ApplicationProcessorFixture, LoadSequenceSegmentFeatureQCComponents_ProcessFailsOnEmptySequence)
{
  // Default ApplicationHandler has a default SequenceHnadler with empty sequence
  ApplicationHandler ah;
  LoadSequenceSegmentFeatureQCComponents cmd{ ah };
  auto loaded = cmd.process();
  EXPECT_TRUE(!loaded);
}

TEST_F(ApplicationProcessorFixture, LoadSequenceSegmentFeatureQCComponents_ProcessSucceedsOnNonEmptySequenceAndCorrectPath)
{
  LoadSequenceSegmentFeatureQCComponents cmd{ ah_ };
  {
    cmd.pathname_ = filenames_.featureQCComponents_csv_i;
  }
  auto loaded = cmd.process();
  EXPECT_TRUE(loaded);
}

/* LoadSequenceSegmentFeatureQCComponentGroups */
TEST_F(ApplicationProcessorFixture, LoadSequenceSegmentFeatureQCComponentGroups_GetName)
{
  LoadSequenceSegmentFeatureQCComponentGroups cmd{ ah_ };
  EXPECT_STREQ(cmd.getName().c_str(), "LoadSequenceSegmentFeatureQCComponentGroups");
}

TEST_F(ApplicationProcessorFixture, LoadSequenceSegmentFeatureQCComponentGroups_ProcessFailsOnEmptySequence)
{
  // Default ApplicationHandler has a default SequenceHnadler with empty sequence
  ApplicationHandler ah;
  LoadSequenceSegmentFeatureQCComponentGroups cmd{ ah };
  auto loaded = cmd.process();
  EXPECT_TRUE(!loaded);
}

TEST_F(ApplicationProcessorFixture, LoadSequenceSegmentFeatureQCComponentGroups_ProcessSucceedsOnNonEmptySequenceAndCorrectPath)
{
  LoadSequenceSegmentFeatureQCComponentGroups cmd{ ah_ };
  {
    cmd.pathname_ = filenames_.featureQCComponentGroups_csv_i;
  }
  auto loaded = cmd.process();
  EXPECT_TRUE(loaded);
}

/* LoadSequenceSegmentFeatureRSDFilterComponents */
TEST_F(ApplicationProcessorFixture, LoadSequenceSegmentFeatureRSDFilterComponents_GetName)
{
  LoadSequenceSegmentFeatureRSDFilterComponents cmd{ ah_ };
  EXPECT_STREQ(cmd.getName().c_str(), "LoadSequenceSegmentFeatureRSDFilterComponents");
}

TEST_F(ApplicationProcessorFixture, LoadSequenceSegmentFeatureRSDFilterComponents_ProcessFailsOnEmptySequence)
{
  // Default ApplicationHandler has a default SequenceHnadler with empty sequence
  ApplicationHandler ah;
  LoadSequenceSegmentFeatureRSDFilterComponents cmd{ ah };
  auto loaded = cmd.process();
  EXPECT_TRUE(!loaded);
}

TEST_F(ApplicationProcessorFixture, LoadSequenceSegmentFeatureRSDFilterComponents_ProcessSucceedsOnNonEmptySequenceAndCorrectPath)
{
  LoadSequenceSegmentFeatureRSDFilterComponents cmd{ ah_ };
  {
    cmd.pathname_ = filenames_.featureRSDFilterComponents_csv_i;
  }
  auto loaded = cmd.process();
  EXPECT_TRUE(loaded);
}

/* LoadSequenceSegmentFeatureRSDFilterComponentGroups */
TEST_F(ApplicationProcessorFixture, LoadSequenceSegmentFeatureRSDFilterComponentGroups_GetName)
{
  LoadSequenceSegmentFeatureRSDFilterComponentGroups cmd{ ah_ };
  EXPECT_STREQ(cmd.getName().c_str(), "LoadSequenceSegmentFeatureRSDFilterComponentGroups");
}

TEST_F(ApplicationProcessorFixture, LoadSequenceSegmentFeatureRSDFilterComponentGroups_ProcessFailsOnEmptySequence)
{
  // Default ApplicationHandler has a default SequenceHnadler with empty sequence
  ApplicationHandler ah;
  LoadSequenceSegmentFeatureRSDFilterComponentGroups cmd{ ah };
  auto loaded = cmd.process();
  EXPECT_TRUE(!loaded);
}

TEST_F(ApplicationProcessorFixture, LoadSequenceSegmentFeatureRSDFilterComponentGroups_ProcessSucceedsOnNonEmptySequenceAndCorrectPath)
{
  LoadSequenceSegmentFeatureRSDFilterComponentGroups cmd{ ah_ };
  {
    cmd.pathname_ = filenames_.featureRSDFilterComponentGroups_csv_i;
  }
  auto loaded = cmd.process();
  EXPECT_TRUE(loaded);
}

/* LoadSequenceSegmentFeatureRSDQCComponents */
TEST_F(ApplicationProcessorFixture, LoadSequenceSegmentFeatureRSDQCComponents_GetName)
{
  LoadSequenceSegmentFeatureRSDQCComponents cmd{ ah_ };
  EXPECT_STREQ(cmd.getName().c_str(), "LoadSequenceSegmentFeatureRSDQCComponents");
}

TEST_F(ApplicationProcessorFixture, LoadSequenceSegmentFeatureRSDQCComponents_ProcessFailsOnEmptySequence)
{
  // Default ApplicationHandler has a default SequenceHnadler with empty sequence
  ApplicationHandler ah;
  LoadSequenceSegmentFeatureRSDQCComponents cmd{ ah };
  auto loaded = cmd.process();
  EXPECT_TRUE(!loaded);
}

TEST_F(ApplicationProcessorFixture, LoadSequenceSegmentFeatureRSDQCComponents_ProcessSucceedsOnNonEmptySequenceAndCorrectPath)
{
  LoadSequenceSegmentFeatureRSDQCComponents cmd{ ah_ };
  {
    cmd.pathname_ = filenames_.featureRSDQCComponents_csv_i;
  }
  auto loaded = cmd.process();
  EXPECT_TRUE(loaded);
}

/* LoadSequenceSegmentFeatureRSDQCComponentGroups */
TEST_F(ApplicationProcessorFixture, LoadSequenceSegmentFeatureRSDQCComponentGroups_GetName)
{
  LoadSequenceSegmentFeatureRSDQCComponentGroups cmd{ ah_ };
  EXPECT_STREQ(cmd.getName().c_str(), "LoadSequenceSegmentFeatureRSDQCComponentGroups");
}

TEST_F(ApplicationProcessorFixture, LoadSequenceSegmentFeatureRSDQCComponentGroups_ProcessFailsOnEmptySequence)
{
  // Default ApplicationHandler has a default SequenceHnadler with empty sequence
  ApplicationHandler ah;
  LoadSequenceSegmentFeatureRSDQCComponentGroups cmd{ ah };
  auto loaded = cmd.process();
  EXPECT_TRUE(!loaded);
}

TEST_F(ApplicationProcessorFixture, LoadSequenceSegmentFeatureRSDQCComponentGroups_ProcessSucceedsOnNonEmptySequenceAndCorrectPath)
{
  LoadSequenceSegmentFeatureRSDQCComponentGroups cmd{ ah_ };
  {
    cmd.pathname_ = filenames_.featureRSDQCComponentGroups_csv_i;
  }
  auto loaded = cmd.process();
  EXPECT_TRUE(loaded);
}

/* LoadSequenceSegmentFeatureBackgroundFilterComponents */
TEST_F(ApplicationProcessorFixture, LoadSequenceSegmentFeatureBackgroundFilterComponents_GetName)
{
  LoadSequenceSegmentFeatureBackgroundFilterComponents cmd{ ah_ };
  EXPECT_STREQ(cmd.getName().c_str(), "LoadSequenceSegmentFeatureBackgroundFilterComponents");
}

TEST_F(ApplicationProcessorFixture, LoadSequenceSegmentFeatureBackgroundFilterComponents_ProcessFailsOnEmptySequence)
{
  // Default ApplicationHandler has a default SequenceHnadler with empty sequence
  ApplicationHandler ah;
  LoadSequenceSegmentFeatureBackgroundFilterComponents cmd{ ah };
  auto loaded = cmd.process();
  EXPECT_TRUE(!loaded);
}

TEST_F(ApplicationProcessorFixture, LoadSequenceSegmentFeatureBackgroundFilterComponents_ProcessSucceedsOnNonEmptySequenceAndCorrectPath)
{
  LoadSequenceSegmentFeatureBackgroundFilterComponents cmd{ ah_ };
  {
    cmd.pathname_ = filenames_.featureBackgroundFilterComponents_csv_i;
  }
  auto loaded = cmd.process();
  EXPECT_TRUE(loaded);
}

/* LoadSequenceSegmentFeatureBackgroundFilterComponentGroups */
TEST_F(ApplicationProcessorFixture, LoadSequenceSegmentFeatureBackgroundFilterComponentGroups_GetName)
{
  LoadSequenceSegmentFeatureBackgroundFilterComponentGroups cmd{ ah_ };
  EXPECT_STREQ(cmd.getName().c_str(), "LoadSequenceSegmentFeatureBackgroundFilterComponentGroups");
}

TEST_F(ApplicationProcessorFixture, LoadSequenceSegmentFeatureBackgroundFilterComponentGroups_ProcessFailsOnEmptySequence)
{
  // Default ApplicationHandler has a default SequenceHnadler with empty sequence
  ApplicationHandler ah;
  LoadSequenceSegmentFeatureBackgroundFilterComponentGroups cmd{ ah };
  auto loaded = cmd.process();
  EXPECT_TRUE(!loaded);
}

TEST_F(ApplicationProcessorFixture, LoadSequenceSegmentFeatureBackgroundFilterComponentGroups_ProcessSucceedsOnNonEmptySequenceAndCorrectPath)
{
  LoadSequenceSegmentFeatureBackgroundFilterComponentGroups cmd{ ah_ };
  {
    cmd.pathname_ = filenames_.featureBackgroundFilterComponentGroups_csv_i;
  }
  auto loaded = cmd.process();
  EXPECT_TRUE(loaded);
}

/* LoadSequenceSegmentFeatureBackgroundQCComponents */
TEST_F(ApplicationProcessorFixture, LoadSequenceSegmentFeatureBackgroundQCComponents_GetName)
{
  LoadSequenceSegmentFeatureBackgroundQCComponents cmd{ ah_ };
  EXPECT_STREQ(cmd.getName().c_str(), "LoadSequenceSegmentFeatureBackgroundQCComponents");
}

TEST_F(ApplicationProcessorFixture, LoadSequenceSegmentFeatureBackgroundQCComponents_ProcessFailsOnEmptySequence)
{
  // Default ApplicationHandler has a default SequenceHnadler with empty sequence
  ApplicationHandler ah;
  LoadSequenceSegmentFeatureBackgroundQCComponents cmd{ ah };
  auto loaded = cmd.process();
  EXPECT_TRUE(!loaded);
}

TEST_F(ApplicationProcessorFixture, LoadSequenceSegmentFeatureBackgroundQCComponents_ProcessSucceedsOnNonEmptySequenceAndCorrectPath)
{
  LoadSequenceSegmentFeatureBackgroundQCComponents cmd{ ah_ };
  {
    cmd.pathname_ = filenames_.featureBackgroundQCComponents_csv_i;
  }
  auto loaded = cmd.process();
  EXPECT_TRUE(loaded);
}

/* LoadSequenceSegmentFeatureBackgroundQCComponentGroups */
TEST_F(ApplicationProcessorFixture, LoadSequenceSegmentFeatureBackgroundQCComponentGroups_GetName)
{
  LoadSequenceSegmentFeatureBackgroundQCComponentGroups cmd{ ah_ };
  EXPECT_STREQ(cmd.getName().c_str(), "LoadSequenceSegmentFeatureBackgroundQCComponentGroups");
}

TEST_F(ApplicationProcessorFixture, LoadSequenceSegmentFeatureBackgroundQCComponentGroups_ProcessFailsOnEmptySequence)
{
  // Default ApplicationHandler has a default SequenceHnadler with empty sequence
  ApplicationHandler ah;
  LoadSequenceSegmentFeatureBackgroundQCComponentGroups cmd{ ah };
  auto loaded = cmd.process();
  EXPECT_TRUE(!loaded);
}

TEST_F(ApplicationProcessorFixture, LoadSequenceSegmentFeatureBackgroundQCComponentGroups_ProcessSucceedsOnNonEmptySequenceAndCorrectPath)
{
  LoadSequenceSegmentFeatureBackgroundQCComponentGroups cmd{ ah_ };
  {
    cmd.pathname_ = filenames_.featureBackgroundQCComponentGroups_csv_i;
  }
  auto loaded = cmd.process();
  EXPECT_TRUE(loaded);
}

/* StoreSequenceFileAnalyst */
TEST_F(ApplicationProcessorFixture, StoreSequenceFileAnalyst_GetName)
{
  StoreSequenceFileAnalyst cmd{ ah_ };
  EXPECT_STREQ(cmd.getName().c_str(), "StoreSequenceFileAnalyst");
}

TEST_F(ApplicationProcessorFixture, StoreSequenceFileAnalyst_ProcessFailsOnEmptySequence)
{
  // Default ApplicationHandler has a default SequenceHnadler with empty sequence
  ApplicationHandler ah;
  StoreSequenceFileAnalyst cmd{ ah };
  auto store = cmd.process();
  EXPECT_TRUE(!store);
}

TEST_F(ApplicationProcessorFixture, StoreSequenceFileAnalyst_ProcessSucceedsOnNonEmptySequence)
{
  StoreSequenceFileAnalyst cmd{ ah_ };
  auto store = cmd.process();
  EXPECT_TRUE(store);
}

/* StoreSequenceFileMasshunter */
TEST_F(ApplicationProcessorFixture, StoreSequenceFileMasshunter_GetName)
{
  StoreSequenceFileMasshunter cmd{ ah_ };
  EXPECT_STREQ(cmd.getName().c_str(), "StoreSequenceFileMasshunter");
}

TEST_F(ApplicationProcessorFixture, StoreSequenceFileMasshunter_ProcessFailsOnEmptySequence)
{
  // Default ApplicationHandler has a default SequenceHnadler with empty sequence
  ApplicationHandler ah;
  StoreSequenceFileMasshunter cmd{ ah };
  auto store = cmd.process();
  EXPECT_TRUE(!store);
}

TEST_F(ApplicationProcessorFixture, StoreSequenceFileMasshunter_ProcessSucceedsOnNonEmptySequence)
{
  StoreSequenceFileMasshunter cmd{ ah_ };
  auto store = cmd.process();
  EXPECT_TRUE(store);
}

/* StoreSequenceFileXcalibur */
TEST_F(ApplicationProcessorFixture, StoreSequenceFileXcalibur_GetName)
{
  StoreSequenceFileXcalibur cmd{ ah_ };
  EXPECT_STREQ(cmd.getName().c_str(), "StoreSequenceFileXcalibur");
}

TEST_F(ApplicationProcessorFixture, StoreSequenceFileXcalibur_ProcessFailsOnEmptySequence)
{
  // Default ApplicationHandler has a default SequenceHnadler with empty sequence
  ApplicationHandler ah;
  StoreSequenceFileXcalibur cmd{ ah };
  auto store = cmd.process();
  EXPECT_TRUE(!store);
}

TEST_F(ApplicationProcessorFixture, StoreSequenceFileXcalibur_ProcessSucceedsOnNonEmptySequence)
{
  StoreSequenceFileXcalibur cmd{ ah_ };
  auto store = cmd.process();
  EXPECT_TRUE(store);
}

/* SetRawDataPathname */
TEST_F(ApplicationProcessorFixture, SetRawDataPathname_GetName)
{
  SetRawDataPathname cmd{ ah_ };
  EXPECT_STREQ(cmd.getName().c_str(), "SetRawDataPathname");
}

TEST_F(ApplicationProcessorFixture, SetRawDataPathname_ProcessSetsPath)
{
  SetRawDataPathname cmd{ ah_ };
  {
    cmd.pathname_ = "path/to/directory";
  }
  auto set = cmd.process();
  EXPECT_TRUE(set);
  EXPECT_EQ(ah_.mzML_dir_, cmd.pathname_);
}

/* SetInputFeaturesPathname */
TEST_F(ApplicationProcessorFixture, SetInputFeaturesPathname_GetName)
{
  SetInputFeaturesPathname cmd{ ah_ };
  EXPECT_STREQ(cmd.getName().c_str(), "SetInputFeaturesPathname");
}

TEST_F(ApplicationProcessorFixture, SetInputFeaturesPathname_ProcessSetsPath)
{
  SetInputFeaturesPathname cmd{ ah_ };
  {
    cmd.pathname_ = "path/to/directory";
  }
  auto set = cmd.process();
  EXPECT_TRUE(set);
  EXPECT_EQ(ah_.features_in_dir_, cmd.pathname_);
}

/* SetOutputFeaturesPathname */
TEST_F(ApplicationProcessorFixture, SetOutputFeaturesPathname_GetName)
{
  SetOutputFeaturesPathname cmd{ ah_ };
  EXPECT_STREQ(cmd.getName().c_str(), "SetOutputFeaturesPathname");
}

TEST_F(ApplicationProcessorFixture, SetOutputFeaturesPathname_ProcessSetsPath)
{
  SetOutputFeaturesPathname cmd{ ah_ };
  {
    cmd.pathname_ = "path/to/directory";
  }
  auto set = cmd.process();
  EXPECT_TRUE(set);
  EXPECT_EQ(ah_.features_out_dir_, cmd.pathname_);
}

TEST_F(ApplicationProcessorFixture, StoreSequenceWorkflow1)
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
  EXPECT_STREQ(application_processor.getName().c_str(), "StoreSequenceWorkflow");
  application_processor.pathname_ = (SMARTPEAK_GET_TEST_DATA_PATH("ApplicationProcessor_workflow.csv"));
  //application_processor.pathname_ = std::tmpnam(nullptr);
  EXPECT_TRUE(application_processor.process());
  // compare with reference file
  const string reference_filename = SMARTPEAK_GET_TEST_DATA_PATH("ApplicationProcessor_workflow.csv");
  EXPECT_STREQ(application_processor.pathname_.c_str(), reference_filename.c_str());
}

TEST_F(ApplicationProcessorFixture, LoadSequenceWorkflow1)
{
  ApplicationHandler application_handler;
  LoadSequenceWorkflow application_processor(application_handler);
  EXPECT_STREQ(application_processor.getName().c_str(), "LoadSequenceWorkflow");
  application_processor.pathname_ = SMARTPEAK_GET_TEST_DATA_PATH("ApplicationProcessor_workflow.csv");
  EXPECT_TRUE(application_processor.process());
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
  EXPECT_TRUE(commands.size() == expected_command_names.size());
  for (auto i=0; i< expected_command_names.size(); ++i)
  {
    EXPECT_EQ(expected_command_names[i], commands[i]);
  }
}
