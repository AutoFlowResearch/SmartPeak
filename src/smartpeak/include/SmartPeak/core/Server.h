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
#include <iostream>
#include <algorithm>
#include <vector>
#include <memory>
#include <filesystem>
#include <chrono>
#include <cassert>

#include <SmartPeak/io/SequenceParser.h>
#include <SmartPeak/io/InputDataValidation.h>

#include <SmartPeak/core/ConsoleHandler.h>
#include <SmartPeak/core/ApplicationHandler.h>
#include <SmartPeak/core/SessionHandler.h>
#include <SmartPeak/core/WorkflowManager.h>
#include <SmartPeak/core/EventDispatcher.h>
#include <SmartPeak/core/ProgressInfo.h>
#include <SmartPeak/core/ApplicationProcessor.h>
#include <SmartPeak/core/SequenceProcessor.h>
#include <SmartPeak/core/FeatureMetadata.h>
#include <SmartPeak/core/SharedProcessors.h>

#include <SmartPeak/iface/IApplicationProcessorObserver.h>
#include <SmartPeak/iface/ISequenceProcessorObserver.h>
#include <SmartPeak/iface/ISampleGroupProcessorObserver.h>
#include <SmartPeak/iface/ISequenceSegmentProcessorObserver.h>

#include <SmartPeak/ui/GuiAppender.h>
#include <plog/Log.h>


namespace SmartPeak {
  namespace serv {
  
    static bool contains_option(
      const std::vector<std::string>& list,
      const std::string& option,
      std::string log_msg="");

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

      inline void set_workflow_commands(const std::vector<ApplicationHandler::Command>& commands) { commands_ = commands; }
      inline const std::vector<ApplicationHandler::Command>& get_workflow_commands() { return commands_; }

      inline EventDispatcher& get_event_dispatcher() { return event_dispatcher_; }
      inline const EventDispatcher& get_event_dispatcher() const { return event_dispatcher_; }

      inline ProgressInfo& get_progress_info() { return *progress_info_ptr_; }
      inline const ProgressInfo& get_progress_info() const { return *progress_info_ptr_; }
      
      std::string               sequence_file;
      std::vector<std::string>  report { "FEATUREDB", "PIVOTTABLE", "ALL" };
      std::vector<std::string>  report_sample_types { "ALL" };
      std::vector<std::string>  report_metadata { "ALL" };
      std::vector<std::string>  workflow;
      std::vector<std::string>  integrity { "NONE" };
      std::string               log_dir;
      std::string               out_dir;
      
    private:
      ApplicationHandler application_handler_;
      SessionHandler session_handler_;
      WorkflowManager workflow_manager_;
      EventDispatcher event_dispatcher_;
      std::shared_ptr<ProgressInfo> progress_info_ptr_;
      std::vector<ApplicationHandler::Command> commands_;
      std::shared_ptr<GuiAppender> gui_appender_;
    };

    void _extract_report_sampletypes(
      const std::vector<std::string>& application_settings,
      std::set<SmartPeak::SampleType>& report_sample_types);

    void _extract_report_metadata(
      const std::vector<std::string>& application_settings,
      std::vector<SmartPeak::FeatureMetadata>& report_metadata);
  
    std::tuple<bool, std::vector<SmartPeak::GuiAppender::GuiAppenderRecord>>
      handleWorkflowRequest(ServerManager* application_manager, bool is_logger_init);
  
    bool handleLogger(SmartPeak::ConsoleHandler* logger);
  }
}
