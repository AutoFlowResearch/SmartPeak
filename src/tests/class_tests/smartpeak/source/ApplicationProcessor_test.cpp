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
  /*
  LoadSessionFromSequence cmd{ ah_ };
//  BOOST_CHECK_EQUAL(cmd.getName(), "LoadSessionFromSequence");
*/
}

BOOST_AUTO_TEST_CASE(LoadSessionFromSequence_ProcessSucceedsOnCorrectPath)
{
  /*
  LoadSessionFromSequence cmd{ ah_ };
  {
    cmd.pathname_ = filenames_.sequence_csv_i;
  }
  auto loaded = cmd.processFilePicker();
  BOOST_CHECK(loaded);
  */
}

/*
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
//  BOOST_CHECK_EQUAL(application_processor.getName(), "StoreSequenceWorkflow");
  application_processor.pathname_ = (SMARTPEAK_GET_TEST_DATA_PATH("ApplicationProcessor_workflow.csv"));
  //application_processor.pathname_ = std::tmpnam(nullptr);
  BOOST_REQUIRE(application_processor.processFilePicker());
  // compare with reference file
  const string reference_filename = SMARTPEAK_GET_TEST_DATA_PATH("ApplicationProcessor_workflow.csv");
  std::ifstream created_if(application_processor.pathname_);
  std::ifstream reference_if(reference_filename);
  std::istream_iterator<char> created_is(created_if), created_end;
  std::istream_iterator<char> reference_is(reference_if), reference_end;
  BOOST_CHECK_EQUAL_COLLECTIONS(created_is, created_end, reference_is, reference_end);
}
*/
/*
BOOST_AUTO_TEST_CASE(LoadSequenceWorkflow1)
{
  ApplicationHandler application_handler;
  LoadSequenceWorkflow application_processor(application_handler);
//  BOOST_CHECK_EQUAL(application_processor.getName(), "LoadSequenceWorkflow");
  application_processor.pathname_ = SMARTPEAK_GET_TEST_DATA_PATH("ApplicationProcessor_workflow.csv");
  BOOST_REQUIRE(application_processor.processFilePicker());
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
*/
BOOST_AUTO_TEST_SUITE_END()
