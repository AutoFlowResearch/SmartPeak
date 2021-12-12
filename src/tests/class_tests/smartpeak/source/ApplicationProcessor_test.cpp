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
#include <SmartPeak/core/SequenceProcessor.h>
#include <SmartPeak/core/Filenames.h>
#include <SmartPeak/core/ApplicationProcessors/BuildCommandsFromNames.h>
#include <SmartPeak/core/ApplicationProcessors/CreateCommand.h>
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

    filenames_.setFullPath("referenceData", (std::filesystem::path{datapath_} / std::filesystem::path{"MRMFeatureValidator_referenceData_1.csv"}).string());

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
