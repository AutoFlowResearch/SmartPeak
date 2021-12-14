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

#include <SmartPeak/core/Utilities.h>
#include <SmartPeak/core/Server.h>

namespace SmartPeak {
  namespace serv {
  
    void ServerAppender::write(const plog::Record& record)
    {
      std::ostringstream ss;
      ss << PLOG_NSTR("[") << record.getFunc() << PLOG_NSTR("@") << record.getLine() << PLOG_NSTR("] ");
      ss << record.getMessage() << PLOG_NSTR("\n");
      
      #ifdef _WIN32
      const std::string sss = ss.str();
      plog::util::nstring str(sss.begin(), sss.end());
      #else
      plog::util::nstring str = ss.str();
      #endif
      std::lock_guard<std::mutex> g(messages_mutex);
      messages.emplace_back(record.getSeverity(), str);
    }

    std::vector<ServerAppender::ServerAppenderRecord> ServerAppender::getAppenderRecordList(plog::Severity severity)
    {
      std::vector<ServerAppender::ServerAppenderRecord> filtered;
      std::lock_guard<std::mutex> g(messages_mutex);
      for (const ServerAppender::ServerAppenderRecord& p : messages) {
        if (p.first <= severity) {
          filtered.push_back(p);
        }
      }
      messages.clear();
      return filtered;
    }
  
    bool contains_option(
      const std::vector<std::string>& list,
      const std::string& option,
      std::string log_msg)
    {
      auto flag = false;
      if (1 <= list.size()) {
        auto it = std::find(list.cbegin(), list.cend(), option);
        if (it != std::end(list)) {
          flag = true;
          LOG_DEBUG << log_msg << option;
        }
      }
      return flag;
    }

    void extract_report_sampletypes(const std::vector<std::string>& selected_report_sample_types,
                                    std::set<SmartPeak::SampleType>& report_sample_types)
    {
      auto& available_types = SmartPeak::sampleTypeToString;
      auto& types = selected_report_sample_types;
      if (contains_option(types, "ALL", "Detected report-sample-types option: "))
      {
        std::transform(available_types.cbegin(), available_types.cend(),
            std::inserter(report_sample_types, report_sample_types.begin()),
                [](const auto& m){ return m.first; });
      }
      else
      {
        std::for_each(types.cbegin(), types.cend(),
          [&available_types, &report_sample_types](const auto& type_prop)
          {
            auto it = std::find_if(available_types.cbegin(), available_types.cend(),
                [&type_prop](const auto& t) { return type_prop == SmartPeak::Utilities::str2upper(t.second); });
            if (it != std::cend(available_types))
            {
              LOG_DEBUG << "Detected report-sample-types option: " << it->second;
              report_sample_types.insert(it->first);
            }
          });
      }
    }

    void extract_report_metadata(
      const std::vector<std::string>& selected_report_metadata,
      std::vector<SmartPeak::FeatureMetadata>& report_metadata)
    {
      auto& available_metadata = SmartPeak::metadataToString;
      auto& meta = selected_report_metadata;
      if (contains_option(meta, "ALL", "Detected report-metadata option: "))
      {
        std::transform(available_metadata.cbegin(), available_metadata.cend(),
            std::back_inserter(report_metadata), [](const auto& m){ return m.first; });
      }
      else
      {
        std::for_each(meta.cbegin(), meta.cend(),
          [&available_metadata, &report_metadata](const auto& meta_prop)
          {
            auto it = std::find_if(available_metadata.cbegin(), available_metadata.cend(),
                [&meta_prop](const auto& t) { return meta_prop == SmartPeak::Utilities::str2upper(t.second); });
            if (it != std::cend(available_metadata))
            {
              LOG_DEBUG << "Detected report-metadata option: " << it->second;
              report_metadata.push_back(it->first);
            }
          });
      }
    }

    bool handleWorkflowRequest(ServerManager* application_manager, bool all_reports)
    {
      bool job_done = true;
      if (!std::filesystem::exists(std::filesystem::path(application_manager->dataset_path))) {
        LOGE << "No data set found at the given path : " << application_manager->dataset_path;
        LOGE << "Aborting!";
        return false;
      }
      LOGI << ">> SmartPeak Server is processing : " << application_manager->dataset_path;

      try
      {
        auto& application_handler = application_manager->get_application_handler();
        auto& workflow_manager = application_manager->get_workflow_manager();
        SmartPeak::LoadSession create_sequence(application_handler, workflow_manager);
        if (std::filesystem::is_regular_file(application_manager->dataset_path)) {
          create_sequence.onFilePicked(application_manager->dataset_path, &application_handler);
        } else {
          Filenames filenames_main = Utilities::buildFilenamesFromDirectory(application_handler, application_manager->dataset_path);
          application_handler.main_dir_ = filenames_main.getTagValue(Filenames::Tag::MAIN_DIR);
          create_sequence.filenames_ = filenames_main;
          create_sequence.process();
        }
        
        std::filesystem::path mzml_dir = application_manager->mzml_dir;
        if (mzml_dir.is_relative())
        {
          mzml_dir = (application_handler.main_dir_ / mzml_dir).lexically_normal();
        }
        application_handler.filenames_.setTagValue(Filenames::Tag::MZML_INPUT_PATH, mzml_dir.generic_string());

        std::filesystem::path features_out_dir = application_manager->features_out_dir;
        if (features_out_dir.is_relative())
        {
          features_out_dir = (application_handler.main_dir_ / features_out_dir).lexically_normal();
        }
        application_handler.filenames_.setTagValue(Filenames::Tag::FEATURES_OUTPUT_PATH, features_out_dir.generic_string());

        std::filesystem::path features_in_dir = application_manager->features_in_dir;
        if (features_in_dir.is_relative())
        {
          features_in_dir = (application_handler.main_dir_ / features_in_dir).lexically_normal();
        }
        application_handler.filenames_.setTagValue(Filenames::Tag::FEATURES_INPUT_PATH, features_in_dir.generic_string());


        auto paths = {
          application_handler.filenames_.getTagValue(Filenames::Tag::MZML_INPUT_PATH),
          application_handler.filenames_.getTagValue(Filenames::Tag::FEATURES_INPUT_PATH),
          application_handler.filenames_.getTagValue(Filenames::Tag::FEATURES_OUTPUT_PATH)
        };
        auto current_path = std::filesystem::path{};
        try
        {
          for (const auto& pathname : paths)
          {
            current_path = pathname;
            std::filesystem::create_directories(std::filesystem::path(pathname));
          }
        }
        catch (std::filesystem::filesystem_error& fe)
        {
          job_done &= false;
          if (fe.code() == std::errc::permission_denied) {
            LOG_ERROR
              << static_cast<std::ostringstream&&>(std::ostringstream()
              << "Unable to create output directory, permission denied: '" << current_path << "'").str();
          } else {
            LOG_ERROR
              << static_cast<std::ostringstream&&>(std::ostringstream()
              << "Unable to create output directory: '" << current_path << "'").str();
          }
        }
        SmartPeak::BuildCommandsFromNames buildCommandsFromNames{application_handler};
        {
          buildCommandsFromNames.names_ = application_handler.sequenceHandler_.getWorkflow();
          if (!application_manager->workflow.empty())
          {
            buildCommandsFromNames.names_ = application_manager->workflow;
            auto workflow_string = std::string{};
            for (const auto& cmd : buildCommandsFromNames.names_)
              workflow_string += "\t" + cmd + " \n";
            LOG_WARNING << "Override workflow with: \n" << workflow_string;
          }
        }
        if (!buildCommandsFromNames.process())
        {
          LOG_ERROR << "Failed to create workflow commands, abort.";
          job_done &= false;
        }
        const auto workflow_commands = buildCommandsFromNames.commands_;
        for (auto& cmd : buildCommandsFromNames.commands_)
        {
          for (auto& p : cmd.dynamic_filenames)
          {
            p.second.setTagValue(Filenames::Tag::MAIN_DIR, application_handler.main_dir_.generic_string());
                      p.second.setTagValue(Filenames::Tag::MZML_INPUT_PATH, application_handler.filenames_.getTagValue(Filenames::Tag::MZML_INPUT_PATH));
                      p.second.setTagValue(Filenames::Tag::FEATURES_INPUT_PATH, application_handler.filenames_.getTagValue(Filenames::Tag::FEATURES_INPUT_PATH));
                      p.second.setTagValue(Filenames::Tag::FEATURES_OUTPUT_PATH, application_handler.filenames_.getTagValue(Filenames::Tag::FEATURES_OUTPUT_PATH));
          }
        }
        
        auto& session_handler = application_manager->get_session_handler();
        auto& event_dispatcher = application_manager->get_event_dispatcher();
        try
        {
          const auto injection_names = session_handler.getSelectInjectionNamesWorkflow(application_handler.sequenceHandler_);
          const auto sequence_segment_names = session_handler.getSelectSequenceSegmentNamesWorkflow(application_handler.sequenceHandler_);
          const auto sample_group_names = session_handler.getSelectSampleGroupNamesWorkflow(application_handler.sequenceHandler_);

          workflow_manager.addWorkflow(
            application_handler, injection_names, sequence_segment_names,
            sample_group_names, workflow_commands,
            &event_dispatcher, &event_dispatcher, &event_dispatcher, &event_dispatcher, true);
        }
        catch(const std::exception& e)
        {
          LOG_ERROR << e.what();
          job_done &= false;
        }
        
        try
        {
          auto feature_db = all_reports;
          auto pivot_table = all_reports;
          auto report_sample_types = std::set<SmartPeak::SampleType>{};
          auto report_metadata = std::vector<SmartPeak::FeatureMetadata>{};
          extract_report_sampletypes(application_manager->report_sample_types, report_sample_types);
          extract_report_metadata(application_manager->report_metadata, report_metadata);

          std::filesystem::path reports_out_dir = application_manager->reports_out_dir;
          if (reports_out_dir.is_relative())
          {
            reports_out_dir = (application_handler.main_dir_ / reports_out_dir).lexically_normal();
          }
          if (!std::filesystem::exists(std::filesystem::path(reports_out_dir)) &&
              !std::filesystem::create_directories(std::filesystem::path(reports_out_dir)))
          {
            LOGE << "Failed to create output report directory: " << reports_out_dir.generic_string();
            job_done &= false;
          }

          if (feature_db)
          {
            auto& sequance_handler = application_handler.sequenceHandler_;
            const auto filepath = reports_out_dir / "FeatureDB.csv";
            SmartPeak::SequenceParser::writeDataTableFromMetaValue(
              sequance_handler, filepath,
              report_metadata, report_sample_types);
          }

          if (pivot_table)
          {
            auto& sequance_handler = application_handler.sequenceHandler_;
            const auto filepath = reports_out_dir / "PivotTable.csv";
            SmartPeak::SequenceParser::writeDataMatrixFromMetaValue(
              sequance_handler, filepath,
              report_metadata, report_sample_types);
          }

          job_done &= true;
        }
        catch(const std::exception& e)
        {
          LOG_ERROR << e.what();
          job_done &= false;
        }
      }
      catch(const std::exception& e)
      {
        LOG_ERROR << e.what();
        job_done &= false;
      }

      LOGI << " Server : Workflow finished : " << (job_done ? "YES" : "NO") << std::endl;
      LOGI << " Server : Waiting for jobs .. " << std::endl;
      return job_done;
    }

    void loadRawDataAndFeatures(ApplicationHandler& application_handler, SessionHandler& session_handler,
                                WorkflowManager& workflow_manager, EventDispatcher& event_dispatcher)
    {
      BuildCommandsFromNames buildCommandsFromNames(application_handler);
      buildCommandsFromNames.names_ = {"LOAD_RAW_DATA","LOAD_FEATURES","MAP_CHROMATOGRAMS"};
      if (!buildCommandsFromNames.process()) {
        LOGE << "Failed to create Commands, aborting.";
      } else {
        for (auto& cmd : buildCommandsFromNames.commands_) {
          for (auto& p : cmd.dynamic_filenames) {
            p.second.setTagValue(Filenames::Tag::MAIN_DIR, application_handler.main_dir_.generic_string());
            p.second.setTagValue(Filenames::Tag::MZML_INPUT_PATH, application_handler.filenames_.getTagValue(Filenames::Tag::MZML_INPUT_PATH));
            p.second.setTagValue(Filenames::Tag::FEATURES_INPUT_PATH, application_handler.filenames_.getTagValue(Filenames::Tag::FEATURES_INPUT_PATH));
            p.second.setTagValue(Filenames::Tag::FEATURES_OUTPUT_PATH, application_handler.filenames_.getTagValue(Filenames::Tag::FEATURES_OUTPUT_PATH));
          }
        }
        const std::set<std::string> injection_names = session_handler.getSelectInjectionNamesWorkflow(application_handler.sequenceHandler_);
        const std::set<std::string> sequence_segment_names = session_handler.getSelectSequenceSegmentNamesWorkflow(application_handler.sequenceHandler_);
        const std::set<std::string> sample_group_names = session_handler.getSelectSampleGroupNamesWorkflow(application_handler.sequenceHandler_);
        
        workflow_manager.addWorkflow(application_handler, injection_names, sequence_segment_names,
                                     sample_group_names, buildCommandsFromNames.commands_, &event_dispatcher,
                                     &event_dispatcher, &event_dispatcher, &event_dispatcher);
      }
    }
  }
}
