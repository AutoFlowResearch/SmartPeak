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

    bool contains_option(
        const std::vector<std::string>& list,
        const std::string& option,
        std::string log_msg)
    {
      auto flag = false;
      if (1 <= list.size())
      {
        auto it = std::find(list.cbegin(), list.cend(), option);
        if (it != std::end(list))
        {
          flag = true;
          LOG_DEBUG << log_msg << option;
        }
      }
      return flag;
    }

    void _extract_report_sampletypes(
        const std::vector<std::string>& selected_report_sample_types,
        std::set<SmartPeak::SampleType>& report_sample_types)
    {
      auto& available_types = SmartPeak::sampleTypeToString;
      auto& types = selected_report_sample_types;
      if (contains_option(
              types, "ALL", "Detected report-sample-types option: "))
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

    void _extract_report_metadata(
        const std::vector<std::string>& selected_report_metadata,
        std::vector<SmartPeak::FeatureMetadata>& report_metadata)
    {
      auto& available_metadata = SmartPeak::metadataToString;
      auto& meta = selected_report_metadata;
      if (contains_option(
              meta, "ALL", "Detected report-metadata option: "))
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

    std::tuple<bool, std::vector<SmartPeak::GuiAppender::GuiAppenderRecord>>
      handleWorkflowRequest(ServerManager* application_manager, bool is_logger_init)
    {
      bool job_done = false;
      std::cout << ">>> SERVER received : " << application_manager->sequence_file << std::endl;
        LOGI << ">>> SERVER received : " << application_manager->sequence_file;
        
      try
      {
        auto& application_handler = application_manager->get_application_handler();
        auto create_sequence = SmartPeak::CreateSequence{application_handler.sequenceHandler_};
        create_sequence.onFilePicked(application_manager->sequence_file, &application_handler);
        
        // init-dirs
        auto main_dir = application_handler.main_dir_;
        if (!application_manager->out_dir.empty() && application_manager->out_dir != ".")
        {
          main_dir = application_manager->out_dir;
          LOG_DEBUG << "Output feature directory: " << main_dir.generic_string();
        }
        application_handler.mzML_dir_           = application_handler.main_dir_ / "mzML";
        application_handler.features_in_dir_    = application_handler.main_dir_ / "features";
        application_handler.features_out_dir_   = main_dir / "features";

        auto paths = {
          application_handler.mzML_dir_,
          application_handler.features_in_dir_,
          application_handler.features_out_dir_
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
          if (fe.code() == std::errc::permission_denied)
            LOG_ERROR << static_cast<std::ostringstream&&>(
              std::ostringstream()
                << "Unable to create output directory, permission denied: '" << current_path << "'").str();
          else
            LOG_ERROR << static_cast<std::ostringstream&&>(
              std::ostringstream()
                << "Unable to create output directory: '" << current_path << "'").str();
        }

        SmartPeak::BuildCommandsFromNames buildCommandsFromNames{application_handler};
        {
          buildCommandsFromNames.names_ = application_handler.sequenceHandler_.getWorkflow();
          if (!application_manager->workflow.empty())
          {
            buildCommandsFromNames.names_ = application_manager->workflow;
            auto workflow_string = std::string{};
            for (const auto& cmd : buildCommandsFromNames.names_) workflow_string += "\t" + cmd + " \n";
            LOG_WARNING << "Override workflow with: \n" << workflow_string;
          }
        }
        if (!buildCommandsFromNames.process())
        {
          LOG_ERROR << "Failed to create workflow commands, abort.";
        }
        application_manager->set_workflow_commands(buildCommandsFromNames.commands_);
        for (auto& cmd : buildCommandsFromNames.commands_)
        {
          for (auto& p : cmd.dynamic_filenames)
          {
            p.second.setTag(SmartPeak::Filenames::Tag::MAIN_DIR, application_handler.main_dir_.generic_string());
            p.second.setTag(SmartPeak::Filenames::Tag::MZML_INPUT_PATH, application_handler.mzML_dir_.generic_string());
            p.second.setTag(SmartPeak::Filenames::Tag::FEATURES_INPUT_PATH, application_handler.features_in_dir_.generic_string());
            p.second.setTag(SmartPeak::Filenames::Tag::FEATURES_OUTPUT_PATH, application_handler.features_out_dir_.generic_string());
          }
        }
        
        // RunInC-impl.
        auto& session_handler = application_manager->get_session_handler();
        auto& workflow_manager = application_manager->get_workflow_manager();
        auto& event_dispatcher = application_manager->get_event_dispatcher();
        auto& progress_info = application_manager->get_progress_info();
        {
          //event_dispatcher.addApplicationProcessorObserver(this);
          //event_dispatcher.addSequenceProcessorObserver(this);
          //event_dispatcher.addSequenceSegmentProcessorObserver(this);
          //event_dispatcher.addSampleGroupProcessorObserver(this);
        }
        try
        {
          const auto injection_names = session_handler.getSelectInjectionNamesWorkflow(application_handler.sequenceHandler_);
          const auto sequence_segment_names = session_handler.getSelectSequenceSegmentNamesWorkflow(application_handler.sequenceHandler_);
          const auto sample_group_names = session_handler.getSelectSampleGroupNamesWorkflow(application_handler.sequenceHandler_);

          workflow_manager.addWorkflow(
            application_handler, injection_names, sequence_segment_names,
            sample_group_names, application_manager->get_workflow_commands(),
            &event_dispatcher, &event_dispatcher, &event_dispatcher, &event_dispatcher, true);

          if (false)
          {
            auto freq = static_cast<double>(20.); // Hz
            auto rate = std::chrono::milliseconds(static_cast<uint64_t>(1000. / freq));
            while (!workflow_manager.isWorkflowDone())
            {
              event_dispatcher.dispatchEvents();
              //std::this_thread::sleep_for(rate);
              //show_progress(progress_info, 40);
            }
          }
        }
        catch(const std::exception& e)
        {
          LOG_ERROR << e.what();
        }

        try
        {
          bool all_reports = true; //todo
          auto feature_db = all_reports;
          auto pivot_table = all_reports;

          auto report_sample_types = std::set<SmartPeak::SampleType>{};
          auto report_metadata = std::vector<SmartPeak::FeatureMetadata>{};
          _extract_report_sampletypes(application_manager->report_sample_types, report_sample_types);
          _extract_report_metadata(application_manager->report_metadata, report_metadata);

          auto main_dir = application_handler.main_dir_;
          if (!application_manager->out_dir.empty() && application_manager->out_dir != ".")
          {
            main_dir = application_manager->out_dir;
          }

          if (feature_db)
          {
            auto& sequance_handler = application_handler.sequenceHandler_;
            const auto filepath = main_dir / "FeatureDB.csv";
            SmartPeak::SequenceParser::writeDataTableFromMetaValue(
              sequance_handler, filepath,
              report_metadata, report_sample_types);
          }
          if (pivot_table)
          {
            auto& sequance_handler = application_handler.sequenceHandler_;
            const auto filepath = main_dir / "PivotTable.csv";
            SmartPeak::SequenceParser::writeDataMatrixFromMetaValue(
              sequance_handler, filepath,
              report_metadata, report_sample_types);
          }
          
          job_done = true; //chainer-req
        }
        catch(const std::exception& e)
        {
          LOG_ERROR << e.what();
        }
      }
      catch(const std::exception& e)
      {
        LOG_ERROR << e.what();
      }
      
      std::cout << ">>> Server : job_done : " << (job_done ? "YES" : "NO") << std::endl;
      std::cout << ">>> Server : Server waiting .. " << std::endl;
      
      std::vector<SmartPeak::GuiAppender::GuiAppenderRecord> logs;
      return std::tie(job_done, logs);
    }

    bool handleLogger(SmartPeak::ConsoleHandler* logger)
    {
      const std::time_t t = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
      char filename[128];
      strftime(filename, 128, "smartpeak_log_%Y-%m-%d_%H-%M-%S.log", std::localtime(&t));

      auto logfilepath = std::filesystem::path{};
      auto logdirpath = std::string{};
      auto logdir_created = false;
      auto error_msg = std::string{};
      try
      {
        std::tie(logfilepath, logdir_created) = SmartPeak::Utilities::getLogFilepath(filename);
        logdirpath = std::filesystem::path(logfilepath).parent_path().string();
      }
      catch (const std::runtime_error& re)
      {
        error_msg = re.what();
      }
      logger = &SmartPeak::ConsoleHandler::get_instance();
      logger->set_log_directory(logdirpath);
      logger->use_colors(false);
      logger->set_severity(plog::debug);
      logger->initialize("Start SmartPeak version " + SmartPeak::Utilities::getSmartPeakVersion());
      return true;
    }
  }
}
