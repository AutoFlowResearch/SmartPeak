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
// $Maintainer: Ahmed Khalil $
// $Authors: Ahmed Khalil $
// --------------------------------------------------------------------------
#pragma once
#include <SmartPeak/io/SequenceParser.h>
#include <SmartPeak/io/InputDataValidation.h>

#include <SmartPeak/core/ApplicationHandler.h>
#include <SmartPeak/core/SessionHandler.h>
#include <SmartPeak/core/WorkflowManager.h>
#include <SmartPeak/core/EventDispatcher.h>
#include <SmartPeak/core/ProgressInfo.h>
#include <SmartPeak/core/ApplicationProcessor.h>
#include <SmartPeak/core/SequenceProcessor.h>
#include <SmartPeak/core/FeatureMetadata.h>
#include <SmartPeak/core/SharedProcessors.h>
#include <SmartPeak/core/ApplicationProcessors/LoadSession.h>
#include <SmartPeak/core/ApplicationProcessors/BuildCommandsFromNames.h>

#include <SmartPeak/iface/IApplicationProcessorObserver.h>
#include <SmartPeak/iface/ISequenceProcessorObserver.h>
#include <SmartPeak/iface/ISampleGroupProcessorObserver.h>
#include <SmartPeak/iface/ISequenceSegmentProcessorObserver.h>

#include <plog/Log.h>

#include <iostream>
#include <mutex>
#include <algorithm>
#include <vector>
#include <memory>
#include <filesystem>
#include <chrono>
#include <cassert>


namespace SmartPeak {
  namespace serv {
  
    class ServerAppender : public plog::IAppender
    {
    public:
      typedef std::pair<plog::Severity, plog::util::nstring> ServerAppenderRecord;
      
      void write(const plog::Record& record) override;
      
      std::vector<ServerAppenderRecord> getAppenderRecordList(plog::Severity severity);

    private:
      std::vector<ServerAppenderRecord> messages;
      mutable std::mutex messages_mutex;
    };
  
    class ServerManager {
    public:
      /**
       * Constructs the ServerManager object.
       */
      ServerManager()
      {
        application_handler_.sequenceHandler_.addTransitionsObserver(&event_dispatcher_);
        application_handler_.sequenceHandler_.addSequenceObserver(&event_dispatcher_);
        event_dispatcher_.addTransitionsObserver(&session_handler_);
        event_dispatcher_.addSequenceObserver(&session_handler_);
        progress_info_ptr_ = std::make_shared<ProgressInfo>(
            event_dispatcher_, event_dispatcher_, event_dispatcher_, event_dispatcher_);
      }
      virtual ~ServerManager() = default;

    public:
      inline ApplicationHandler& get_application_handler() { return application_handler_; }
      inline const ApplicationHandler& get_application_handler() const { return application_handler_; }

      inline SessionHandler& get_session_handler() { return session_handler_; }
      inline const SessionHandler& get_session_handler() const { return session_handler_; }

      inline WorkflowManager& get_workflow_manager() { return workflow_manager_; }
      inline const WorkflowManager& get_workflow_manager() const { return workflow_manager_; }

      inline EventDispatcher& get_event_dispatcher() { return event_dispatcher_; }
      inline const EventDispatcher& get_event_dispatcher() const { return event_dispatcher_; }
      
      std::string               dataset_path;
      std::vector<std::string>  report {"FEATUREDB","PIVOTTABLE","ALL"};
      std::vector<std::string>  report_sample_types {"ALL"};
      std::vector<std::string>  report_metadata {"ALL"};
      std::vector<std::string>  workflow;
      std::vector<std::string>  integrity {"NONE"};
      std::string               log_dir;
      std::string               out_dir;
      std::string               features_out_dir {"./features"};
      std::string               features_in_dir {"./features"};
      std::vector<std::string>  input_files;
      std::string               mzml_dir {"./mzML"};
      std::string               reports_out_dir {"exports"};
      
    private:
      ApplicationHandler application_handler_;
      SessionHandler session_handler_;
      WorkflowManager workflow_manager_;
      EventDispatcher event_dispatcher_;
      std::shared_ptr<ProgressInfo> progress_info_ptr_;
      std::vector<ApplicationHandler::Command> commands_;
      std::shared_ptr<serv::ServerAppender> server_appender_;
    };

    void extract_report_sampletypes(
      const std::vector<std::string>& application_settings,
      std::set<SmartPeak::SampleType>& report_sample_types);

    void extract_report_metadata(
      const std::vector<std::string>& application_settings,
      std::vector<SmartPeak::FeatureMetadata>& report_metadata);
  
    static bool contains_option(
      const std::vector<std::string>& list,
      const std::string& option, std::string log_msg="");
  
    bool handleWorkflowRequest(ServerManager* application_manager, bool export_all);
  
    void loadRawDataAndFeatures(
      ApplicationHandler& application_handler, SessionHandler& session_handler,
      WorkflowManager& workflow_manager, EventDispatcher& event_dispatcher);
  }
}
