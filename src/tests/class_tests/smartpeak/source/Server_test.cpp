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
// $Maintainer: Ahmed Khalil, Douglas McCloskey $
// $Authors: Ahmed Khalil $
// --------------------------------------------------------------------------

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <SmartPeak/test_config.h>
#include <SmartPeak/core/Server.h>


TEST(Server, handleWorkflowRequest)
{
  SmartPeak::serv::ServerManager server_manager;
  server_manager.dataset_path = std::string{SMARTPEAK_GET_TEST_DATA_PATH("workflow_csv_files")};
  server_manager.get_application_handler().sequenceHandler_.setWorkflow({"LOAD_FEATURES", "STORE_FEATURES"});
  bool export_all;
  EXPECT_TRUE(handleWorkflowRequest(&server_manager, true));
}

TEST(Server, extractReportSampletypes)
{
  const std::vector<std::string> server_settings{"ALL","Solvent","QC"};
  std::set<SmartPeak::SampleType> report_sample_types;
  
  SmartPeak::serv::extractReportSampletypes(server_settings, report_sample_types);
  EXPECT_TRUE(report_sample_types.size() == 7);
}

TEST(Server, extractReportMetadata)
{
  const std::vector<std::string> server_settings{"points_across_baseline","peak_area", "ALL"};
  std::vector<SmartPeak::FeatureMetadata> report_metadata;
  
  SmartPeak::serv::extractReportMetadata(server_settings, report_metadata);
  EXPECT_TRUE(report_metadata.size() == 30);
}

TEST(Server, containsOption)
{
  const std::vector<std::string> list{"ALL","Solvent","QC"};
  const std::string option{"QC"};
  
  EXPECT_TRUE(SmartPeak::serv::containsOption(list, option, "log_msg"));
}

TEST(Server, processRemoteWorkflow)
{
  SmartPeak::serv::ServerManager server_manager;
  server_manager.dataset_path = std::string{SMARTPEAK_GET_TEST_DATA_PATH("workflow_csv_files")};
  server_manager.get_application_handler().sequenceHandler_.setWorkflow({"LOAD_FEATURES", "STORE_FEATURES"});
  server_manager.get_application_handler().sequenceHandler_.notifyWorkflowUpdated();
  bool success = handleWorkflowRequest(&server_manager, true);
  
  std::promise<std::string> runworkflow_promise;
  if (success) { runworkflow_promise.set_value("YES"); }
  std::future<std::string> runworkflow_future = runworkflow_promise.get_future();
  std::string username = "user1";
  SmartPeak::ApplicationHandler& application_handler = server_manager.get_application_handler();
  SmartPeak::SessionHandler& session_handler = server_manager.get_session_handler();
  
  SmartPeak::WorkflowManager& workflow_manager = server_manager.get_workflow_manager();
  SmartPeak::EventDispatcher& event_dispatcher = server_manager.get_event_dispatcher();
  bool RawDataAndFeatures_loaded = false;
  
//  SmartPeak::serv::processRemoteWorkflow(
//    runworkflow_future, username, application_handler, session_handler,
//    workflow_manager, event_dispatcher, RawDataAndFeatures_loaded);
//  EXPECT_TRUE(RawDataAndFeatures_loaded);
  EXPECT_TRUE(success);
}
