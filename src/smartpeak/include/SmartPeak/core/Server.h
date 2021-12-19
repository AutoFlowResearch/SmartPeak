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
#include <SmartPeak/core/ServerAppender.h>
#include <SmartPeak/core/WorkflowManager.h>
#include <SmartPeak/core/EventDispatcher.h>
#include <SmartPeak/core/ProgressInfo.h>
#include <SmartPeak/core/ApplicationProcessor.h>
#include <SmartPeak/core/SequenceProcessor.h>
#include <SmartPeak/core/FeatureMetadata.h>
#include <SmartPeak/core/SharedProcessors.h>
#include <SmartPeak/core/WorkflowManager.h>
#include <SmartPeak/core/ApplicationProcessors/LoadSession.h>
#include <SmartPeak/core/ApplicationProcessors/BuildCommandsFromNames.h>

#include <SmartPeak/iface/IApplicationProcessorObserver.h>
#include <SmartPeak/iface/ISequenceProcessorObserver.h>
#include <SmartPeak/iface/ISampleGroupProcessorObserver.h>
#include <SmartPeak/iface/ISequenceSegmentProcessorObserver.h>

#include <plog/Log.h>
#include <sqlite3.h>

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
  
    struct SeverEventDispatcherObserver :
      IApplicationProcessorObserver,
      ISequenceProcessorObserver,
      ISequenceSegmentProcessorObserver,
      ISampleGroupProcessorObserver,
      ISequenceObserver,
      ITransitionsObserver
    {
      /**
        IApplicationProcessorObserver
      */
      virtual void onApplicationProcessorStart(const std::vector<std::string>& commands) override
      {
        events_.push_back(std::make_tuple("onApplicationProcessorStart", 0, "", commands));
      }
      virtual void onApplicationProcessorCommandStart(size_t command_index, const std::string& command_name) override
      {
        events_.push_back(std::make_tuple("onApplicationProcessorCommandStart", command_index, command_name, std::vector<std::string>()));
      }
      virtual void onApplicationProcessorCommandEnd(size_t command_index, const std::string& command_name) override
      {
        events_.push_back(std::make_tuple("onApplicationProcessorCommandEnd", command_index, command_name, std::vector<std::string>()));
      }
      virtual void onApplicationProcessorEnd() override
      {
        events_.push_back(std::make_tuple("onApplicationProcessorEnd", 0, "", std::vector<std::string>()));
      }
      virtual void onApplicationProcessorError(const std::string& error) override
      {
        events_.push_back(std::make_tuple("onApplicationProcessorError", 0, "", std::vector<std::string>({ error })));
      }
      /**
        ISequenceProcessorObserver
      */
      virtual void onSequenceProcessorStart(const size_t nb_injections) override
      {
        events_.push_back(std::make_tuple("onSequenceProcessorStart", nb_injections, "", std::vector<std::string>()));
      }
      virtual void onSequenceProcessorSampleStart(const std::string& sample_name) override
      {
        events_.push_back(std::make_tuple("onSequenceProcessorSampleStart", 0, sample_name, std::vector<std::string>()));
      }
      virtual void onSequenceProcessorSampleEnd(const std::string& sample_name) override
      {
        events_.push_back(std::make_tuple("onSequenceProcessorSampleEnd", 0, sample_name, std::vector<std::string>()));
      }
      virtual void onSequenceProcessorEnd() override
      {
        events_.push_back(std::make_tuple("onSequenceProcessorEnd", 0, "", std::vector<std::string>()));
      }
      virtual void onSequenceProcessorError(const std::string& sample_name, const std::string& processor_name, const std::string& error) override
      {
        events_.push_back(std::make_tuple("onSequenceProcessorError", 0, sample_name, std::vector<std::string>({ processor_name , error })));
      }
      /**
        ISequenceSegmentProcessorObserver
      */
      virtual void onSequenceSegmentProcessorStart(const size_t nb_segments) override
      {
        events_.push_back(std::make_tuple("onSequenceSegmentProcessorStart", nb_segments, "", std::vector<std::string>()));
      }
      virtual void onSequenceSegmentProcessorSampleStart(const std::string& segment_name) override
      {
        events_.push_back(std::make_tuple("onSequenceSegmentProcessorSampleStart", 0, segment_name, std::vector<std::string>()));
      }
      virtual void onSequenceSegmentProcessorSampleEnd(const std::string& segment_name) override
      {
        events_.push_back(std::make_tuple("onSequenceSegmentProcessorSampleEnd", 0, segment_name, std::vector<std::string>()));
      }
      virtual void onSequenceSegmentProcessorEnd() override
      {
        events_.push_back(std::make_tuple("onSequenceSegmentProcessorEnd", 0, "", std::vector<std::string>()));
      }
      virtual void onSequenceSegmentProcessorError(const std::string& segment_name, const std::string& processor_name, const std::string& error) override
      {
        events_.push_back(std::make_tuple("onSequenceSegmentProcessorError", 0, segment_name, std::vector<std::string>({ processor_name , error })));
      }
      /**
        ISampleGroupProcessorObserver
      */
      virtual void onSampleGroupProcessorStart(const size_t nb_groups) override
      {
        events_.push_back(std::make_tuple("onSampleGroupProcessorStart", nb_groups, "", std::vector<std::string>()));
      }
      virtual void onSampleGroupProcessorSampleStart(const std::string& group_name) override
      {
        events_.push_back(std::make_tuple("onSampleGroupProcessorSampleStart", 0, group_name, std::vector<std::string>()));
      }
      virtual void onSampleGroupProcessorSampleEnd(const std::string& group_name) override
      {
        events_.push_back(std::make_tuple("onSampleGroupProcessorSampleEnd", 0, group_name, std::vector<std::string>()));
      }
      virtual void onSampleGroupProcessorEnd() override
      {
        events_.push_back(std::make_tuple("onSampleGroupProcessorEnd", 0, "", std::vector<std::string>()));
      }
      virtual void onSampleGroupProcessorError(const std::string& group_name, const std::string& processor_name, const std::string& error)
      {
        events_.push_back(std::make_tuple("onSampleGroupProcessorError", 0, group_name, std::vector<std::string>({ processor_name , error})));
      }

      /**
        ISequenceObserver
      */
      virtual void onSequenceUpdated() override
      {
        events_.push_back(std::make_tuple("onSequenceUpdated", 0, "", std::vector<std::string>()));
      }

      /**
        ITransitionsObserver
      */
      virtual void onTransitionsUpdated() override
      {
        events_.push_back(std::make_tuple("onTransitionsUpdated", 0, "", std::vector<std::string>()));
      }

      std::vector<std::tuple<std::string, size_t, std::string, std::vector<std::string>>> events_;
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
      
      inline SeverEventDispatcherObserver& get_server_event_dispatcher_observer() { return server_event_dispatcher_observer_; }
      inline const SeverEventDispatcherObserver& get_server_event_dispatcher_observer() const { return server_event_dispatcher_observer_; }
      
      inline void reset() { dataset_path = ""; application_handler_.closeSession(); }
      
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
      SeverEventDispatcherObserver server_event_dispatcher_observer_;
      std::shared_ptr<ProgressInfo> progress_info_ptr_;
      std::vector<ApplicationHandler::Command> commands_;
      std::shared_ptr<ServerAppender> server_appender_;
    };

    void extractReportSampletypes(
      const std::vector<std::string>& application_settings,
      std::set<SmartPeak::SampleType>& report_sample_types);

    void extractReportMetadata(
      const std::vector<std::string>& application_settings,
      std::vector<SmartPeak::FeatureMetadata>& report_metadata);
  
    static bool containsOption(
      const std::vector<std::string>& list,
      const std::string& option, std::string log_msg="");
  
    bool handleWorkflowRequest(ServerManager* application_manager, bool export_all);
  
    void loadRawDataAndFeatures(
      ApplicationHandler& application_handler, SessionHandler& session_handler,
      WorkflowManager& workflow_manager, EventDispatcher& event_dispatcher);
  
    void processRemoteWorkflow(
      std::future<std::string>& runworkflow_future, std::string& username,
      ApplicationHandler& application_handler, SessionHandler& session_handler,
      WorkflowManager& workflow_manager, EventDispatcher& event_dispatcher, bool& RawDataAndFeatures_loaded);
  
  }
}
