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
// $Authors: Douglas McCloskey, Pasquale Domenico Colaianni $
// --------------------------------------------------------------------------

#include <SmartPeak/core/ApplicationProcessor.h>
#include <SmartPeak/core/Filenames.h>
#include <SmartPeak/core/RawDataProcessor.h>
#include <SmartPeak/core/SequenceProcessor.h>
#include <SmartPeak/io/InputDataValidation.h>
#include <SmartPeak/io/SequenceParser.h>
#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iostream>
#include <regex>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <plog/Log.h>
#include <plog/Appenders/ConsoleAppender.h>
#include <SmartPeak/core/SharedProcessors.h>
#include <SmartPeak/io/CSVWriter.h>
#include <SmartPeak/io/csv.h>

namespace SmartPeak
{
  namespace ApplicationProcessors {

  ParameterSet getParameterSchema()
  {
    ParameterSet parameter_set;
    parameter_set.merge(ProcessSequence::getParameterSchemaStatic());
    return parameter_set;
  }

  template<typename COMMANDS_LIST_TYPE>
  void notifyStartCommands(ApplicationProcessorObservable& observable, size_t start_index, const COMMANDS_LIST_TYPE& commands)
  {
    for (const auto command : commands)
    {
      observable.notifyApplicationProcessorCommandStart(start_index, command->getName());
      start_index++;
    }
  }

  template<typename COMMANDS_LIST_TYPE>
  void notifyEndCommands(ApplicationProcessorObservable& observable, size_t start_index, const COMMANDS_LIST_TYPE& commands)
  {
    for (const auto command : commands)
    {
      observable.notifyApplicationProcessorCommandEnd(start_index, command->getName());
      start_index++;
    }
  }

  void processCommands(ApplicationHandler& application_handler,
    std::vector<ApplicationHandler::Command> commands,
    const std::set<std::string>& injection_names, 
    const std::set<std::string>& sequence_segment_names, 
    const std::set<std::string>& sample_group_names,
    IApplicationProcessorObserver* application_processor_observer,
    ISequenceProcessorObserver* sequence_processor_observer,
    ISequenceSegmentProcessorObserver* sequence_segment_processor_observer,
    ISampleGroupProcessorObserver* sample_group_processor_observer)
  {
    ApplicationProcessorObservable observable;
    observable.addApplicationProcessorObserver(application_processor_observer);

    std::vector<std::string> commands_names;
    for (const auto& command : commands)
    {
      commands_names.push_back(command.getName());
    }
    observable.notifyApplicationProcessorStart(commands_names);

    size_t i = 0;
    while (i < commands.size()) {
      const ApplicationHandler::Command::CommandType type = commands[i].type;
      size_t j = i + 1;
      for (; j < commands.size() && type == commands[j].type; ++j) {
        // empty body
      }
      const ApplicationHandler::Command& cmd = commands[i];
      if (cmd.type == ApplicationHandler::Command::RawDataMethod) {
        std::vector<std::shared_ptr<RawDataProcessor>> raw_methods;
        std::map<std::string, Filenames> filenames;
        std::for_each(commands.begin() + i, commands.begin() + j,
          [&](const ApplicationHandler::Command& command) 
        { 
          raw_methods.push_back(command.raw_data_method);
          for (const auto& filename_to_merge : command.dynamic_filenames)
          {
            if (filenames.find(filename_to_merge.first) == filenames.end())
            {
              filenames.emplace(filename_to_merge.first, filename_to_merge.second);
            }
            else
            {
              filenames.at(filename_to_merge.first).merge(filename_to_merge.second);
            }
          }
        });
        ProcessSequence ps(application_handler.sequenceHandler_, sequence_processor_observer);
        ps.filenames_ = filenames;
        ps.raw_data_processing_methods_ = raw_methods;
        ps.injection_names_ = injection_names;
        notifyStartCommands<decltype(raw_methods)>(observable, i, raw_methods);
        ps.process(application_handler.filenames_);
        notifyEndCommands<decltype(raw_methods)>(observable, i, raw_methods);
      } else if (cmd.type == ApplicationHandler::Command::SequenceSegmentMethod) {
        std::vector<std::shared_ptr<SequenceSegmentProcessor>> seq_seg_methods;
        std::map<std::string, Filenames> filenames;
        std::for_each(commands.begin() + i, commands.begin() + j,
          [&](const ApplicationHandler::Command& command)
        {
          seq_seg_methods.push_back(command.seq_seg_method);
          for (const auto& filename_to_merge : command.dynamic_filenames)
          {
            if (filenames.find(filename_to_merge.first) == filenames.end())
            {
              filenames.emplace(filename_to_merge.first, filename_to_merge.second);
            }
            else
            {
              filenames.at(filename_to_merge.first).merge(filename_to_merge.second);
            }
          }
        });
        ProcessSequenceSegments pss(application_handler.sequenceHandler_, sequence_segment_processor_observer);
        pss.filenames_ = filenames;
        pss.sequence_segment_processing_methods_ = seq_seg_methods;
        pss.sequence_segment_names_ = sequence_segment_names;
        notifyStartCommands<decltype(seq_seg_methods)>(observable, i, seq_seg_methods);
        pss.process(application_handler.filenames_);
        notifyEndCommands<decltype(seq_seg_methods)>(observable, i, seq_seg_methods);
      } else if (cmd.type == ApplicationHandler::Command::SampleGroupMethod) {
        std::vector<std::shared_ptr<SampleGroupProcessor>> sample_group_methods;
        std::map<std::string, Filenames> filenames;
        std::for_each(commands.begin() + i, commands.begin() + j,
          [&](const ApplicationHandler::Command& command)
        {
          sample_group_methods.push_back(command.sample_group_method);
          for (const auto& filename_to_merge : command.dynamic_filenames)
          {
            if (filenames.find(filename_to_merge.first) == filenames.end())
            {
              filenames.emplace(filename_to_merge.first, filename_to_merge.second);
            }
            else
            {
              filenames.at(filename_to_merge.first).merge(filename_to_merge.second);
            }
          }
        });
        ProcessSampleGroups psg(application_handler.sequenceHandler_, sample_group_processor_observer);
        psg.filenames_ = filenames;
        psg.sample_group_processing_methods_ = sample_group_methods;
        psg.sample_group_names_ = sample_group_names;
        notifyStartCommands<decltype(sample_group_methods)>(observable, i, sample_group_methods);
        psg.process(application_handler.filenames_);
        notifyEndCommands<decltype(sample_group_methods)>(observable, i, sample_group_methods);
      }
      else 
      {
        LOGW << "Skipping a command: " << cmd.type << "\n";
      }
      i = j;
    }
    observable.notifyApplicationProcessorEnd();
  }
  }

  bool CreateCommand::process() {
    // Enumerate the valid command keys
    std::vector<std::string> valid_commands_raw_data_processor;
    for (const auto& it: n_to_raw_data_method_) { valid_commands_raw_data_processor.push_back(it.first); }
    std::vector<std::string> valid_commands_sequence_segment_processor;
    for (const auto& it: n_to_seq_seg_method_) { valid_commands_sequence_segment_processor.push_back(it.first); }
    std::vector<std::string> valid_commands_sample_group_processor;
    for (const auto& it : n_to_sample_group_method_) { valid_commands_sample_group_processor.push_back(it.first); }

    // Run the command depending on whether it is a raw data processor method or sequence segment processor method
    if (std::count(valid_commands_raw_data_processor.begin(), valid_commands_raw_data_processor.end(), name_)) {
      const auto& method = n_to_raw_data_method_.at(name_);
      cmd_.setMethod(method);
      Filenames method_filenames = application_handler_.filenames_;
      for (const InjectionHandler& injection : application_handler_.sequenceHandler_.getSequence()) {
        const std::string& key = injection.getMetaData().getInjectionName();
        cmd_.dynamic_filenames[key] = method_filenames;
        cmd_.dynamic_filenames[key].setTag(Filenames::Tag::INPUT_MZML_FILENAME, injection.getMetaData().getFilename());
        cmd_.dynamic_filenames[key].setTag(Filenames::Tag::INPUT_INJECTION_NAME, key);
        cmd_.dynamic_filenames[key].setTag(Filenames::Tag::OUTPUT_INJECTION_NAME, key);
        cmd_.dynamic_filenames[key].setTag(Filenames::Tag::INPUT_GROUP_NAME, injection.getMetaData().getSampleGroupName());
        cmd_.dynamic_filenames[key].setTag(Filenames::Tag::OUTPUT_GROUP_NAME, injection.getMetaData().getSampleGroupName());
      }
    } else if (std::count(valid_commands_sequence_segment_processor.begin(), valid_commands_sequence_segment_processor.end(), name_)) {
      const auto& method = n_to_seq_seg_method_.at(name_);
      cmd_.setMethod(method);
      Filenames method_filenames = application_handler_.filenames_;
      for (const SequenceSegmentHandler& sequence_segment : application_handler_.sequenceHandler_.getSequenceSegments()) {
        const std::string& key = sequence_segment.getSequenceSegmentName();
        cmd_.dynamic_filenames[key] = method_filenames;
        cmd_.dynamic_filenames[key].setTag(Filenames::Tag::INPUT_MZML_FILENAME, "");
        cmd_.dynamic_filenames[key].setTag(Filenames::Tag::INPUT_INJECTION_NAME, key);
        cmd_.dynamic_filenames[key].setTag(Filenames::Tag::OUTPUT_INJECTION_NAME, key);
        cmd_.dynamic_filenames[key].setTag(Filenames::Tag::INPUT_GROUP_NAME, key);
        cmd_.dynamic_filenames[key].setTag(Filenames::Tag::OUTPUT_GROUP_NAME, key);
      }
    } else if (std::count(valid_commands_sample_group_processor.begin(), valid_commands_sample_group_processor.end(), name_)) {
      const auto& method = n_to_sample_group_method_.at(name_);
      cmd_.setMethod(method);
      Filenames method_filenames = application_handler_.filenames_;
      for (const SampleGroupHandler& sample_group : application_handler_.sequenceHandler_.getSampleGroups()) {
        const std::string& key = sample_group.getSampleGroupName();
        cmd_.dynamic_filenames[key] = method_filenames;
        cmd_.dynamic_filenames[key].setTag(Filenames::Tag::INPUT_MZML_FILENAME, "");
        cmd_.dynamic_filenames[key].setTag(Filenames::Tag::INPUT_INJECTION_NAME, key);
        cmd_.dynamic_filenames[key].setTag(Filenames::Tag::OUTPUT_INJECTION_NAME, key);
        cmd_.dynamic_filenames[key].setTag(Filenames::Tag::INPUT_GROUP_NAME, key);
        cmd_.dynamic_filenames[key].setTag(Filenames::Tag::OUTPUT_GROUP_NAME, key);
      }
    }
    else 
    {
      LOGE << "No command for selection name " << name_;
      return false;
    }
    return true;
  }

  bool BuildCommandsFromNames::process()
  {
    bool success = true;
    commands_.clear();
    CreateCommand createCommand(application_handler_);
    for (const auto& command_name : names_) {
      createCommand.name_ = command_name;
      bool commands_created = createCommand.process();
      success &= commands_created;
      if (commands_created) {
        commands_.push_back(createCommand.cmd_);
      } // if not, no need to log. createCommand already logs an error.
    }
    return success;
  }

  bool LoadSession::onFilePicked(const std::filesystem::path& filename, ApplicationHandler* application_handler)
  {
    application_handler->closeSession();
    application_handler->filenames_.getSessionDB().setDBFilePath(filename);
    application_handler->main_dir_ = std::filesystem::path(filename).remove_filename().generic_string();
    delimiter = ",";
    checkConsistency = false; // NOTE: Requires a lot of time on large sequences with a large number of components
    return process();
  }

  bool LoadSession::process()
  {
    LOGD << "START LoadSession";

    if (!filenames_) // if filenames are not provided, we will take it from the DB.
    {
      LoadFilenames load_filenames(application_handler_);
      if (!load_filenames.process())
      {
        notifyApplicationProcessorError("Failed to load session");
        return false;
      }
      filenames_ = application_handler_.filenames_;
    }

    if (!overrideFilenames())
    {
      return false;
    }

    filenames_->log();

    for (auto& loading_processor : application_handler_.loading_processors_)
    {
      // check if we need to use that loading processor
      Filenames loading_processor_filenames;
      loading_processor->getFilenames(loading_processor_filenames);
      bool load_it = false;
      for (const auto& file_id : loading_processor_filenames.getFileIds())
      {
        load_it |= (filenames_->isEmbedded(file_id)
                || (!filenames_->getFullPath(file_id).empty()));
      }

      if (load_it)
      {
        auto sequence_processor = std::dynamic_pointer_cast<SequenceProcessor>(loading_processor);
        if (sequence_processor)
        {
          try
          {
            sequence_processor->process(*filenames_);
          }
          catch (const std::exception& e)
          {
            LOGE << e.what();
            notifyApplicationProcessorError(e.what());
            return false;
          }
        }
        auto raw_data_processor = std::dynamic_pointer_cast<RawDataProcessor>(loading_processor);
        if (raw_data_processor)
        {
          if (!application_handler_.sequenceHandler_.getSequence().empty())
          {
            RawDataHandler& rawDataHandler = application_handler_.sequenceHandler_.getSequence()[0].getRawData();
            try
            {
              raw_data_processor->process(rawDataHandler, {}, *filenames_);
            }
            catch (const std::exception& e)
            {
              LOGE << e.what();
              notifyApplicationProcessorError(e.what());
              return false;
            }
          }
          else
          {
            LOGE << "No Sequence available, Loading process aborted.";
            notifyApplicationProcessorError("Failed to load session");
            return false;
          }
        }
        auto sequence_segment_processor = std::dynamic_pointer_cast<SequenceSegmentProcessor>(loading_processor);
        if (sequence_segment_processor)
        {
          if (!application_handler_.sequenceHandler_.getSequenceSegments().empty())
          {
            SequenceSegmentHandler& sequenceSegmentHandler = application_handler_.sequenceHandler_.getSequenceSegments().at(0);
            sequence_segment_processor->sequence_segment_observable_ = &application_handler_.sequenceHandler_;
            try
            {
              sequence_segment_processor->process(sequenceSegmentHandler, SequenceHandler(), {}, *filenames_);
            }
            catch (const std::exception& e)
            {
              LOGE << e.what();
              notifyApplicationProcessorError(e.what());
              return false;
            }
          }
          else
          {
            LOGE << "No Sequence Segment available, Loading process aborted.";
            notifyApplicationProcessorError("Failed to load session");
            return false;
          }
        }
      }
    }

    if (!overrideParameters())
    {
      return false;
    }

    if (checkConsistency)
    {
      if (!application_handler_.sequenceHandler_.getSequenceSegments().empty())
      {
        InputDataValidation::sampleNamesAreConsistent(application_handler_.sequenceHandler_);
        InputDataValidation::componentNamesAreConsistent(application_handler_.sequenceHandler_);
        InputDataValidation::componentNameGroupsAreConsistent(application_handler_.sequenceHandler_);
        InputDataValidation::heavyComponentsAreConsistent(application_handler_.sequenceHandler_);
      }
      else
      {
        LOGW << "No Sequence available, cannot check consistency";
      }
    }

    // Load Features
    std::vector<std::string> commands = { "LOAD_RAW_DATA","LOAD_FEATURES","MAP_CHROMATOGRAMS", "EXTRACT_CHROMATOGRAM_WINDOWS", "ZERO_CHROMATOGRAM_BASELINE"};
    BuildCommandsFromNames buildCommandsFromNames(application_handler_);
    buildCommandsFromNames.names_ = commands;
    if (!buildCommandsFromNames.process()) {
      LOGE << "Failed to create Commands, aborting.";
    }
    else
    {
      for (auto& cmd : buildCommandsFromNames.commands_)
      {
        for (auto& p : cmd.dynamic_filenames)
        {
          p.second.setTag(Filenames::Tag::MZML_INPUT_PATH, application_handler_.filenames_.getTag(Filenames::Tag::MZML_INPUT_PATH));
          p.second.setTag(Filenames::Tag::FEATURES_INPUT_PATH, application_handler_.filenames_.getTag(Filenames::Tag::FEATURES_INPUT_PATH));
          p.second.setTag(Filenames::Tag::FEATURES_OUTPUT_PATH, application_handler_.filenames_.getTag(Filenames::Tag::FEATURES_OUTPUT_PATH));
        }
      }
      std::set<std::string> injection_names;
      for (const auto& injection : application_handler_.sequenceHandler_.getSequence())
      {
        injection_names.insert(injection.getMetaData().getInjectionName());
      }
      const std::set<std::string> sequence_segment_names = {}; // session_handler_.getSelectSequenceSegmentNamesWorkflow(application_handler_.sequenceHandler_);
      const std::set<std::string> sample_group_names = {}; // session_handler_.getSelectSampleGroupNamesWorkflow(application_handler_.sequenceHandler_);
      workflow_manager_.addWorkflow(
        application_handler_,
        injection_names,
        sequence_segment_names,
        sample_group_names,
        buildCommandsFromNames.commands_,
        application_processor_observer_,
        sequence_processor_observer_,
        sequence_segment_processor_observer_,
        sample_group_processor_observer_,
        true // At the moment, we are blocking the process of the workflow. put this to true (and other works) to profit from the progress bar.
      );
    }

    application_handler_.sequenceHandler_.notifySequenceUpdated();
    LOGD << "END LoadSession";
    return true;
  }

  bool LoadLayout::process()
  {
    LOGD << "START LoadLayout";
    for (auto s : to_serialize)
    {
      LoadWidgets load_widget(application_handler_);
      load_widget.to_serialize = s;
      load_widget.process();
    }
    LOGD << "END LoadLayout";
    return true;
  }

  bool LoadSession::overrideFilenames()
  {
    if (filenames_override_)
    {
      filenames_override_->setTag(Filenames::Tag::MAIN_DIR, filenames_->getTag(Filenames::Tag::MAIN_DIR));
      const auto& src = filenames_->getFileIds();
      for (const auto& file_id : filenames_override_->getFileIds())
      {
        if (std::find(src.begin(), src.end(), file_id) == src.end())
        {
          LOGE << "Unknown overriding input file: " << file_id;
          return false;
        }
        else
        {
          const auto value = filenames_override_->getFullPath(file_id);
          LOGW << "Overriding input file: " << file_id << " -> " << "\"" << value.generic_string() << "\"";
          filenames_->setFullPath(file_id, value);
        }
      }
    }
    return true;
  }

  bool LoadSession::overrideParameters()
  {
    if (parameters_override_)
    {
      if (application_handler_.sequenceHandler_.getSequence().size())
      {
        auto schema_params = application_handler_.getWorkflowParameterSchema();
        parameters_override_->merge(schema_params);

        for (auto& function_parameter_override : *parameters_override_)
        {
          for (auto& parameter_override : function_parameter_override.second)
          {
            if (!parameter_override.isSchema())
            {
              if (!parameter_override.getSchema())
              {
                LOGE << "Overridden parameter \"" << function_parameter_override.first << ":" << parameter_override.getName()
                  << "\": " << "Unused or Unknown";
                return false;
              }
              if (!parameter_override.isValid())
              {
                LOGE << "Overridden parameter \"" << function_parameter_override.first << ":" << parameter_override.getName()
                  << "\", Invalid value: \"" << parameter_override.getValueAsString() << "\"";
                LOGE << "Expected Type: " << parameter_override.getType();
                auto constraints = parameter_override.getRestrictionsAsString();
                if (!constraints.empty())
                {
                  LOGE << "Constraints: " << constraints;
                }
                return false;
              }
              else
              {
                // looks ok, override parameter
                ParameterSet& user_parameters = application_handler_.sequenceHandler_.getSequence().at(0).getRawData().getParameters();
                Parameter* existing_parameter = user_parameters.findParameter(function_parameter_override.first, parameter_override.getName());
                LOGW << "Overridden parameter \"" << function_parameter_override.first << ":" << parameter_override.getName()
                  << "\", set value: \"" << parameter_override.getValueAsString() << "\"";
                if (existing_parameter)
                {
                  existing_parameter->setValueFromString(parameter_override.getValueAsString(), false);
                }
                else
                {
                  user_parameters.addParameter(function_parameter_override.first, parameter_override);
                }
              }
            }
          }
        }
      }
    }
    return true;
  }

  bool SaveSession::onFilePicked(const std::filesystem::path& filename, ApplicationHandler* application_handler)
  {
    application_handler->filenames_.getSessionDB().setDBFilePath(filename);
    process();
    return true;
  }

  bool SaveSession::process()
  {
    LOGD << "START SaveSession";

    StoreFilenames store_filenames(application_handler_);
    store_filenames.process();

    for (auto& storing_processor : application_handler_.storing_processors_)
    {
      bool to_be_saved = false;
      Filenames filenames;
      storing_processor->getFilenames(filenames);
      for (const auto& file_id : filenames.getFileIds())
      {
        to_be_saved |= (!application_handler_.isFileSaved(file_id));
      }
      if (to_be_saved)
      {
        auto sequence_processor = std::dynamic_pointer_cast<SequenceProcessor>(storing_processor);
        if (sequence_processor)
        {
          sequence_processor->process(application_handler_.filenames_);
        }
        auto raw_data_processor = std::dynamic_pointer_cast<RawDataProcessor>(storing_processor);
        if (raw_data_processor)
        {
          if (!application_handler_.sequenceHandler_.getSequence().empty())
          {
            RawDataHandler& rawDataHandler = application_handler_.sequenceHandler_.getSequence()[0].getRawData();
            raw_data_processor->process(rawDataHandler, {}, application_handler_.filenames_);
          }
          else
          {
            LOGE << "No Sequence available, Storing process aborted.";
          }
        }
        auto sequence_segment_processor = std::dynamic_pointer_cast<SequenceSegmentProcessor>(storing_processor);
        if (sequence_segment_processor)
        {
          if (!application_handler_.sequenceHandler_.getSequenceSegments().empty())
          {
            SequenceSegmentHandler& sequenceSegmentHandler = application_handler_.sequenceHandler_.getSequenceSegments().at(0);
            sequence_segment_processor->sequence_segment_observable_ = &application_handler_.sequenceHandler_;
            sequence_segment_processor->process(sequenceSegmentHandler, SequenceHandler(), {}, application_handler_.filenames_);
          }
          else
          {
            LOGE << "No Sequence Segment available, Storing process aborted.";
          }
        }
        // update saved state
        for (const auto& file_id : filenames.getFileIds())
        {
          application_handler_.setFileSavedState(file_id, true);
        }
      }
    }

    for (auto s : to_serialize)
    {
      StoreWidgets store_widget(application_handler_);
      store_widget.to_serialize = s;
      store_widget.process();
    }

    LOGD << "END SaveSession";
    return true;
  }

  bool LoadFilenames::process()
  {
    LOGD << "START LoadFilenames";
    auto filenames = LoadFilenames::loadFilenamesFromDB(application_handler_.filenames_.getSessionDB().getDBFilePath());
    if (!filenames)
    {
      return false;
    }
    // reset main dir
    (*filenames).setTag(Filenames::Tag::MAIN_DIR, application_handler_.main_dir_.generic_string());
    (*filenames).getSessionDB().setDBFilePath(application_handler_.filenames_.getSessionDB().getDBFilePath());
    application_handler_.filenames_ = *filenames;
    LOGD << "END LoadFilenames";
    return true;
  }

  std::optional<Filenames> LoadFilenames::loadFilenamesFromDB(const std::filesystem::path& path_db)
  {
    if (!std::filesystem::exists(path_db))
    {
      LOGE << "Session file does not exist: " << path_db.generic_string();
      return std::nullopt;
    }
    Filenames filenames;
    filenames.getSessionDB().setDBFilePath(path_db);
    auto db_context = filenames.getSessionDB().beginRead(
      "filenames",
      "file_id",
      "filename_pattern",
      "embedded"
    );
    if (!db_context)
    {
      return std::nullopt;
    }
    std::string file_id;
    std::string name_pattern;
    int embedded;
    while (filenames.getSessionDB().read(
      *db_context,
      file_id,
      name_pattern,
      embedded
    ))
    {
      filenames.addFileName(file_id, name_pattern);
      filenames.setEmbedded(file_id, embedded != 0);
    };
    filenames.getSessionDB().endRead(*db_context);

    db_context = filenames.getSessionDB().beginRead(
      "filenames_tags",
      "tag",
      "value"
    );
    if (!db_context)
    {
      return std::nullopt;
    }
    std::string tag;
    std::string value;
    while (filenames.getSessionDB().read(
      *db_context,
      tag,
      value
    ))
    {
      filenames.setTag(filenames.getTagNames().at(tag), value);
    };
    filenames.getSessionDB().endRead(*db_context);
    return filenames;
  }

  bool StoreFilenames::process()
  {
    LOGD << "START StoreFilenames";
    auto db_context = application_handler_.filenames_.getSessionDB().beginWrite(
      "filenames",
      "file_id", "TEXT",
      "filename_pattern", "TEXT",
      "embedded", "INT"
    );
    if (!db_context)
    {
      return false;
    }
    for (const auto& file_id : application_handler_.filenames_.getFileIds())
    {
      int embedded = (application_handler_.filenames_.isEmbedded(file_id) ? 1 : 0);
      application_handler_.filenames_.getSessionDB().write(
        *db_context,
        file_id,
        application_handler_.filenames_.getNamePattern(file_id).generic_string(),
        embedded
      );
    }
    application_handler_.filenames_.getSessionDB().endWrite(*db_context);

    db_context = application_handler_.filenames_.getSessionDB().beginWrite(
      "filenames_tags",
      "tag", "TEXT",
      "value", "TEXT"
    );
    if (!db_context)
    {
      return false;
    }
    for (const auto& tag_name : application_handler_.filenames_.getTagNames())
    {
      application_handler_.filenames_.getSessionDB().write(
        *db_context,
        tag_name.first,
        application_handler_.filenames_.getTag(tag_name.second)
      );
    }
    application_handler_.filenames_.getSessionDB().endWrite(*db_context);

    LOGD << "END StoreFilenames";
    return true;
  }

  bool StoreWidgets::process()
  {
    return application_handler_.filenames_.getSessionDB().writePropertiesHandler(*to_serialize);
  }

  bool LoadWidgets::process()
  {
    return application_handler_.filenames_.getSessionDB().readPropertiesHandler(*to_serialize);
  }

}
