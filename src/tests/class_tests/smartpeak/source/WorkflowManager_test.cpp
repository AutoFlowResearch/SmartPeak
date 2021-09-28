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
// $Maintainer: Douglas McCloskey, Bertrand Boudaud $
// $Authors: Douglas McCloskey $
// --------------------------------------------------------------------------

#include <gtest/gtest.h>
#include <SmartPeak/test_config.h>
#include <SmartPeak/core/WorkflowManager.h>
#include <SmartPeak/core/ApplicationProcessor.h>

using namespace SmartPeak;
using namespace std;

TEST(WorkflowManager, isWorkflowDone)
{
  WorkflowManager workflow_manager;
  EXPECT_TRUE(workflow_manager.isWorkflowDone());
}

TEST(WorkflowManager, getRequirements)
{
  WorkflowManager workflow_manager;
  ApplicationHandler application_handler;
  BuildCommandsFromNames buildCommandsFromNames(application_handler);
  buildCommandsFromNames.names_ = { "LOAD_RAW_DATA", "LOAD_FEATURES", "PICK_MRM_FEATURES" };
  ASSERT_TRUE(buildCommandsFromNames.process());

  auto requirements = workflow_manager.getRequirements(buildCommandsFromNames.commands_);
  ASSERT_EQ(requirements, std::set<std::string>({"sequence", "traML"}));
  EXPECT_TRUE(workflow_manager.isWorkflowDone());
}

TEST(WorkflowManager, isMissingRequirements)
{
  WorkflowManager workflow_manager;
  std::set<std::string> requirements = { "sequence" };
  Filenames filenames;
  EXPECT_TRUE(workflow_manager.isMissingRequirements(filenames, requirements));
  filenames.setFullPath("sequence", "non_existing_file.csv");
  EXPECT_TRUE(workflow_manager.isMissingRequirements(filenames, requirements));
  filenames.setFullPath("sequence", SMARTPEAK_GET_TEST_DATA_PATH("SequenceParser_sequence_1.csv"));
  EXPECT_FALSE(workflow_manager.isMissingRequirements(filenames, requirements));
}