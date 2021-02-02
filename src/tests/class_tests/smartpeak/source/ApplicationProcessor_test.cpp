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
#include <boost/filesystem.hpp>

using namespace SmartPeak;
using namespace std;

BOOST_AUTO_TEST_SUITE(applicationprocessor)

BOOST_AUTO_TEST_CASE(buildcommandsfromids)
{
  ApplicationHandler application_handler;
  BuildCommandsFromNames buildCommandsFromNames(application_handler);
  std::vector<ApplicationHandler::Command> methods;

  buildCommandsFromNames.names_ = std::string("");
  buildCommandsFromNames.process();
  BOOST_CHECK_EQUAL(buildCommandsFromNames.commands_.size(), 0);

  buildCommandsFromNames.names_ = std::string("     ");
  buildCommandsFromNames.process();
  BOOST_CHECK_EQUAL(buildCommandsFromNames.commands_.size(), 0);

  buildCommandsFromNames.names_ = std::string("LOAD_RAW_DATA LOAD_FEATURES PICK_MRM_FEATURES");
  buildCommandsFromNames.process();
  BOOST_CHECK_EQUAL(buildCommandsFromNames.commands_.size(), 3);
  BOOST_CHECK_EQUAL(buildCommandsFromNames.commands_.at(0).getName(), "LOAD_RAW_DATA");
  BOOST_CHECK_EQUAL(buildCommandsFromNames.commands_.at(1).getName(), "LOAD_FEATURES");
  BOOST_CHECK_EQUAL(buildCommandsFromNames.commands_.at(2).getName(), "PICK_MRM_FEATURES");

  buildCommandsFromNames.names_ = std::string("LOAD_RAW_DATA PLOT_FEATURES LOAD_FEATURES"); // no plotting processor yet
  buildCommandsFromNames.process();
  BOOST_CHECK_EQUAL(buildCommandsFromNames.commands_.size(), 2);
  BOOST_CHECK_EQUAL(buildCommandsFromNames.commands_.at(0).getName(), "LOAD_RAW_DATA");
  BOOST_CHECK_EQUAL(buildCommandsFromNames.commands_.at(1).getName(), "LOAD_FEATURES");

  buildCommandsFromNames.names_ = std::string("55 87");
  buildCommandsFromNames.process();
  BOOST_CHECK_EQUAL(buildCommandsFromNames.commands_.size(), 0);
}

BOOST_AUTO_TEST_CASE(CreateCommandGetName)
{
  ApplicationHandler application_handler;
  CreateCommand application_processor(application_handler);
  BOOST_CHECK_EQUAL(application_processor.getName(), "CreateCommand");
}

BOOST_AUTO_TEST_CASE(BuildCommandsFromNamesGetName)
{
  ApplicationHandler application_handler;
  BuildCommandsFromNames application_processor(application_handler);
  BOOST_CHECK_EQUAL(application_processor.getName(), "BuildCommandsFromNames");
}

BOOST_AUTO_TEST_CASE(LoadSessionFromSequenceGetName)
{
  ApplicationHandler application_handler;
  LoadSessionFromSequence application_processor(application_handler);
  BOOST_CHECK_EQUAL(application_processor.getName(), "LoadSessionFromSequence");
}

BOOST_AUTO_TEST_CASE(LoadSequenceParametersGetName)
{
  ApplicationHandler application_handler;
  LoadSequenceParameters application_processor(application_handler);
  BOOST_CHECK_EQUAL(application_processor.getName(), "LoadSequenceParameters");
}

BOOST_AUTO_TEST_CASE(LoadSequenceTransitionsGetName)
{
  ApplicationHandler application_handler;
  LoadSequenceTransitions application_processor(application_handler);
  BOOST_CHECK_EQUAL(application_processor.getName(), "LoadSequenceTransitions");
}

BOOST_AUTO_TEST_CASE(LoadSequenceValidationDataGetName)
{
  ApplicationHandler application_handler;
  LoadSequenceValidationData application_processor(application_handler);
  BOOST_CHECK_EQUAL(application_processor.getName(), "LoadSequenceValidationData");
}

BOOST_AUTO_TEST_CASE(LoadSequenceSegmentQuantitationMethodsGetName)
{
  ApplicationHandler application_handler;
  LoadSequenceSegmentQuantitationMethods application_processor(application_handler);
  BOOST_CHECK_EQUAL(application_processor.getName(), "LoadSequenceSegmentQuantitationMethods");
}

BOOST_AUTO_TEST_CASE(LoadSequenceSegmentStandardsConcentrationsGetName)
{
  ApplicationHandler application_handler;
  LoadSequenceSegmentStandardsConcentrations application_processor(application_handler);
  BOOST_CHECK_EQUAL(application_processor.getName(), "LoadSequenceSegmentStandardsConcentrations");
}

BOOST_AUTO_TEST_CASE(LoadSequenceSegmentFeatureFilterComponentsGetName)
{
  ApplicationHandler application_handler;
  LoadSequenceSegmentFeatureFilterComponents application_processor(application_handler);
  BOOST_CHECK_EQUAL(application_processor.getName(), "LoadSequenceSegmentFeatureFilterComponents");
}

BOOST_AUTO_TEST_CASE(LoadSequenceSegmentFeatureFilterComponentGroupsGetName)
{
  ApplicationHandler application_handler;
  LoadSequenceSegmentFeatureFilterComponentGroups application_processor(application_handler);
  BOOST_CHECK_EQUAL(application_processor.getName(), "LoadSequenceSegmentFeatureFilterComponentGroups");
}

BOOST_AUTO_TEST_CASE(LoadSequenceSegmentFeatureQCComponentsGetName)
{
  ApplicationHandler application_handler;
  LoadSequenceSegmentFeatureQCComponents application_processor(application_handler);
  BOOST_CHECK_EQUAL(application_processor.getName(), "LoadSequenceSegmentFeatureQCComponents");
}

BOOST_AUTO_TEST_CASE(LoadSequenceSegmentFeatureQCComponentGroupsGetName)
{
  ApplicationHandler application_handler;
  LoadSequenceSegmentFeatureQCComponentGroups application_processor(application_handler);
  BOOST_CHECK_EQUAL(application_processor.getName(), "LoadSequenceSegmentFeatureQCComponentGroups");
}

BOOST_AUTO_TEST_CASE(LoadSequenceSegmentFeatureRSDFilterComponentsGetName)
{
  ApplicationHandler application_handler;
  LoadSequenceSegmentFeatureRSDFilterComponents application_processor(application_handler);
  BOOST_CHECK_EQUAL(application_processor.getName(), "LoadSequenceSegmentFeatureRSDFilterComponents");
}

BOOST_AUTO_TEST_CASE(LoadSequenceSegmentFeatureRSDFilterComponentGroupsGetName)
{
  ApplicationHandler application_handler;
  LoadSequenceSegmentFeatureRSDFilterComponentGroups application_processor(application_handler);
  BOOST_CHECK_EQUAL(application_processor.getName(), "LoadSequenceSegmentFeatureRSDFilterComponentGroups");
}

BOOST_AUTO_TEST_CASE(LoadSequenceSegmentFeatureRSDQCComponentsGetName)
{
  ApplicationHandler application_handler;
  LoadSequenceSegmentFeatureRSDQCComponents application_processor(application_handler);
  BOOST_CHECK_EQUAL(application_processor.getName(), "LoadSequenceSegmentFeatureRSDQCComponents");
}

BOOST_AUTO_TEST_CASE(LoadSequenceSegmentFeatureRSDQCComponentGroupsGetName)
{
  ApplicationHandler application_handler;
  LoadSequenceSegmentFeatureRSDQCComponentGroups application_processor(application_handler);
  BOOST_CHECK_EQUAL(application_processor.getName(), "LoadSequenceSegmentFeatureRSDQCComponentGroups");
}

BOOST_AUTO_TEST_CASE(LoadSequenceSegmentFeatureBackgroundFilterComponentsGetName)
{
  ApplicationHandler application_handler;
  LoadSequenceSegmentFeatureBackgroundFilterComponents application_processor(application_handler);
  BOOST_CHECK_EQUAL(application_processor.getName(), "LoadSequenceSegmentFeatureBackgroundFilterComponents");
}

BOOST_AUTO_TEST_CASE(LoadSequenceSegmentFeatureBackgroundFilterComponentGroupsGetName)
{
  ApplicationHandler application_handler;
  LoadSequenceSegmentFeatureBackgroundFilterComponentGroups application_processor(application_handler);
  BOOST_CHECK_EQUAL(application_processor.getName(), "LoadSequenceSegmentFeatureBackgroundFilterComponentGroups");
}

BOOST_AUTO_TEST_CASE(LoadSequenceSegmentFeatureBackgroundQCComponentsGetName)
{
  ApplicationHandler application_handler;
  LoadSequenceSegmentFeatureBackgroundQCComponents application_processor(application_handler);
  BOOST_CHECK_EQUAL(application_processor.getName(), "LoadSequenceSegmentFeatureBackgroundQCComponents");
}

BOOST_AUTO_TEST_CASE(LoadSequenceSegmentFeatureBackgroundQCComponentGroupsGetName)
{
  ApplicationHandler application_handler;
  LoadSequenceSegmentFeatureBackgroundQCComponentGroups application_processor(application_handler);
  BOOST_CHECK_EQUAL(application_processor.getName(), "LoadSequenceSegmentFeatureBackgroundQCComponentGroups");
}

BOOST_AUTO_TEST_CASE(StoreSequenceFileAnalyst1)
{
  ApplicationHandler application_handler;
  StoreSequenceFileAnalyst application_processor(application_handler);
  BOOST_CHECK_EQUAL(application_processor.getName(), "StoreSequenceFileAnalyst");
}

BOOST_AUTO_TEST_CASE(StoreSequenceFileMasshunter1)
{
  ApplicationHandler application_handler;
  StoreSequenceFileMasshunter application_processor(application_handler);
  BOOST_CHECK_EQUAL(application_processor.getName(), "StoreSequenceFileMasshunter");
}

BOOST_AUTO_TEST_CASE(StoreSequenceFileXcalibur1)
{
  ApplicationHandler application_handler;
  StoreSequenceFileXcalibur application_processor(application_handler);
  BOOST_CHECK_EQUAL(application_processor.getName(), "StoreSequenceFileXcalibur");
}

BOOST_AUTO_TEST_CASE(SetRawDataPathnameGetName)
{
  ApplicationHandler application_handler;
  SetRawDataPathname application_processor(application_handler);
  BOOST_CHECK_EQUAL(application_processor.getName(), "SetRawDataPathname");
}

BOOST_AUTO_TEST_CASE(SetInputFeaturesPathnameGetName)
{
  ApplicationHandler application_handler;
  SetInputFeaturesPathname application_processor(application_handler);
  BOOST_CHECK_EQUAL(application_processor.getName(), "SetInputFeaturesPathname");
}

BOOST_AUTO_TEST_CASE(SetOutputFeaturesPathnameGetName)
{
  ApplicationHandler application_handler;
  SetOutputFeaturesPathname application_processor(application_handler);
  BOOST_CHECK_EQUAL(application_processor.getName(), "SetOutputFeaturesPathname");
}

BOOST_AUTO_TEST_CASE(StoreSequenceWorkflow1)
{
  namespace fs = boost::filesystem;
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
  application_processor.pathname_ = (fs::temp_directory_path().append(fs::unique_path().string())).string();
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
