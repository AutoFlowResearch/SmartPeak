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

  void processCommands(ApplicationHandler& state,
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
        std::transform(commands.begin() + i, commands.begin() + j, std::back_inserter(raw_methods),
          [](const ApplicationHandler::Command& command){ return command.raw_data_method; });
        ProcessSequence ps(state.sequenceHandler_, sequence_processor_observer);
        ps.filenames_ = cmd.dynamic_filenames;
        ps.raw_data_processing_methods_ = raw_methods;
        ps.injection_names_ = injection_names;
        notifyStartCommands<decltype(raw_methods)>(observable, i, raw_methods);
        ps.process();
        notifyEndCommands<decltype(raw_methods)>(observable, i, raw_methods);
      } else if (cmd.type == ApplicationHandler::Command::SequenceSegmentMethod) {
        std::vector<std::shared_ptr<SequenceSegmentProcessor>> seq_seg_methods;
        std::transform(commands.begin() + i, commands.begin() + j, std::back_inserter(seq_seg_methods),
          [](const ApplicationHandler::Command& command){ return command.seq_seg_method; });
        ProcessSequenceSegments pss(state.sequenceHandler_, sequence_segment_processor_observer);
        pss.filenames_ = cmd.dynamic_filenames;
        pss.sequence_segment_processing_methods_ = seq_seg_methods;
        pss.sequence_segment_names_ = sequence_segment_names;
        notifyStartCommands<decltype(seq_seg_methods)>(observable, i, seq_seg_methods);
        pss.process();
        notifyEndCommands<decltype(seq_seg_methods)>(observable, i, seq_seg_methods);
      } else if (cmd.type == ApplicationHandler::Command::SampleGroupMethod) {
        std::vector<std::shared_ptr<SampleGroupProcessor>> sample_group_methods;
        std::transform(commands.begin() + i, commands.begin() + j, std::back_inserter(sample_group_methods),
          [](const ApplicationHandler::Command& command) { return command.sample_group_method; });
        ProcessSampleGroups psg(state.sequenceHandler_, sample_group_processor_observer);
        psg.filenames_ = cmd.dynamic_filenames;
        psg.sample_group_processing_methods_ = sample_group_methods;
        psg.sample_group_names_ = sample_group_names;
        notifyStartCommands<decltype(sample_group_methods)>(observable, i, sample_group_methods);
        psg.process();
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
      cmd_.setMethod(n_to_raw_data_method_.at(name_));
      for (const InjectionHandler& injection : application_handler_.sequenceHandler_.getSequence()) {
        const std::string& key = injection.getMetaData().getInjectionName();
        cmd_.dynamic_filenames[key] = Filenames::getDefaultDynamicFilenames(
          application_handler_.mzML_dir_,
          application_handler_.features_in_dir_,
          application_handler_.features_out_dir_,
          injection.getMetaData().getFilename(),
          key,
          key,
          injection.getMetaData().getSampleGroupName(),
          injection.getMetaData().getSampleGroupName()
        );
      }
    } else if (std::count(valid_commands_sequence_segment_processor.begin(), valid_commands_sequence_segment_processor.end(), name_)) {
      cmd_.setMethod(n_to_seq_seg_method_.at(name_));
      for (const SequenceSegmentHandler& sequence_segment : application_handler_.sequenceHandler_.getSequenceSegments()) {
        const std::string& key = sequence_segment.getSequenceSegmentName();
        cmd_.dynamic_filenames[key] = Filenames::getDefaultDynamicFilenames(
          application_handler_.mzML_dir_,
          application_handler_.features_in_dir_,
          application_handler_.features_out_dir_,
          "",
          key,
          key,
          key,
          key
        );
      }
    } else if (std::count(valid_commands_sample_group_processor.begin(), valid_commands_sample_group_processor.end(), name_)) {
      cmd_.setMethod(n_to_sample_group_method_.at(name_));
      for (const SampleGroupHandler& sample_group : application_handler_.sequenceHandler_.getSampleGroups()) {
        const std::string& key = sample_group.getSampleGroupName();
        cmd_.dynamic_filenames[key] = Filenames::getDefaultDynamicFilenames(
          application_handler_.mzML_dir_,
          application_handler_.features_in_dir_,
          application_handler_.features_out_dir_,
          "",
          key,
          key,
          key,
          key
        );
      }
    }
    else 
    {
      LOGE << "\nNo command for selection name " << name_;
      return false;
    }
    return true;
  }

  bool StoreSequenceFileSmartPeak::onFilePicked(const std::string& filename, ApplicationHandler* application_handler)
  {
    if (application_handler->sequenceHandler_.getSequence().size() == 0)
    {
      LOGE << "File cannot be loaded without first loading the sequence.";
      return false;
    }
    filename_ = filename;
    process();
    return true;
  }

  bool StoreSequenceFileSmartPeak::process() {
    if (application_handler_.sequenceHandler_.getSequence().size()) {
      SequenceParser::writeSequenceFileSmartPeak(application_handler_.sequenceHandler_, filename_);
      return true;
    }
    else
    {
      LOGE << "Sequence file cannot be converted and stored without first loading the sequence.";
      return false;
    }
  }

  bool StoreSequenceFileAnalyst::onFilePicked(const std::string& filename, ApplicationHandler* application_handler)
  {
    if (application_handler->sequenceHandler_.getSequence().size() == 0)
    {
      LOGE << "File cannot be loaded without first loading the sequence.";
      return false;
    }
    filename_ = filename;
    process();
    return true;
  }

  bool StoreSequenceFileAnalyst::process() {
    if (application_handler_.sequenceHandler_.getSequence().size()) {
      SequenceParser::writeSequenceFileAnalyst(application_handler_.sequenceHandler_, filename_);
      return true;
    }
    else 
    {
      LOGE << "Sequence file cannot be converted and stored without first loading the sequence.";
      return false;
    }
  }

  bool StoreSequenceFileMasshunter::onFilePicked(const std::string& filename, ApplicationHandler* application_handler)
  {
    if (application_handler->sequenceHandler_.getSequence().size() == 0)
    {
      LOGE << "File cannot be loaded without first loading the sequence.";
      return false;
    }
    filename_ = filename;
    process();
    return true;
  }

  bool StoreSequenceFileMasshunter::process()
  {
    if (application_handler_.sequenceHandler_.getSequence().size()) {
      SequenceParser::writeSequenceFileMasshunter(application_handler_.sequenceHandler_, filename_);
      return true;
    }
    else 
    {
      LOGE << "Sequence file cannot be converted and stored without first loading the sequence.";
      return false;
    }
  }

  bool StoreSequenceFileXcalibur::onFilePicked(const std::string& filename, ApplicationHandler* application_handler)
  {
    if (application_handler->sequenceHandler_.getSequence().size() == 0)
    {
      LOGE << "File cannot be loaded without first loading the sequence.";
      return false;
    }
    filename_ = filename;
    process();
    return true;
  }

  bool StoreSequenceFileXcalibur::process()
  {
    if (application_handler_.sequenceHandler_.getSequence().size()) {
      SequenceParser::writeSequenceFileXcalibur(application_handler_.sequenceHandler_, filename_);
      return true;
    }
    else 
    {
      LOGE << "Sequence file cannot be converted and stored without first loading the sequence.";
      return false;
    }
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

}
