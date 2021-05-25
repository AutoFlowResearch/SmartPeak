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
  bool LoadSessionFromSequence::buildStaticFilenames()
  {
    Filenames& f = application_handler_.static_filenames_;
    application_handler_.main_dir_ = application_handler_.sequence_pathname_.substr(0, application_handler_.sequence_pathname_.find_last_of('/'));
    f = Filenames::getDefaultStaticFilenames(application_handler_.main_dir_);
    clearNonExistantDefaultGeneratedFilenames(f);
    f.sequence_csv_i = application_handler_.sequence_pathname_;

    LOGN << "\n\n"
      "The following list of file was searched for:\n";
    std::vector<InputDataValidation::FilenameInfo> is_valid;
    is_valid.push_back(InputDataValidation::isValidFilename(f.sequence_csv_i, "sequence"));
    is_valid.push_back(InputDataValidation::isValidFilename(f.parameters_csv_i, "parameters"));
    is_valid.push_back(InputDataValidation::isValidFilename(f.traML_csv_i, "traml"));
    is_valid.push_back(InputDataValidation::isValidFilename(f.featureFilterComponents_csv_i, "featureFilter"));
    is_valid.push_back(InputDataValidation::isValidFilename(f.featureFilterComponentGroups_csv_i, "featureFilterGroups"));
    is_valid.push_back(InputDataValidation::isValidFilename(f.featureQCComponents_csv_i, "featureQC"));
    is_valid.push_back(InputDataValidation::isValidFilename(f.featureQCComponentGroups_csv_i, "featureQCGroups"));
    is_valid.push_back(InputDataValidation::isValidFilename(f.quantitationMethods_csv_i, "quantitationMethods"));
    is_valid.push_back(InputDataValidation::isValidFilename(f.standardsConcentrations_csv_i, "standardsConcentrations"));
    is_valid.push_back(InputDataValidation::isValidFilename(f.referenceData_csv_i, "referenceData"));

    std::cout << "\n\n";

    const bool requiredPathnamesAreValidBool = requiredPathnamesAreValid(is_valid);

    const bool otherPathnamesAreFine = std::all_of(
      is_valid.cbegin() + 3,
      is_valid.cend(),
      [](const InputDataValidation::FilenameInfo& arg)
        { return arg.validity != InputDataValidation::FilenameInfo::invalid; });

    if (!requiredPathnamesAreValidBool || !otherPathnamesAreFine) {
      LOGF << "\n\nERROR!!!\n"
        "One or more pathnames are not valid.\n";
      if (!requiredPathnamesAreValidBool) {
        LOGF << "\n\n"
        "Make sure that the following required pathnames are provided:\n"
        " - sequence\n"
        " - parameters\n"
        " - traml\n";
      }
      LOGN << "Apply the fixes and reload the sequence file.\n";
      return false;
    }

    return true;
  }

  bool LoadSessionFromSequence::requiredPathnamesAreValid(
    const std::vector<InputDataValidation::FilenameInfo>& validation
  )
  {
    const std::unordered_set<std::string> required {"sequence", "parameters", "traml"};
    bool is_valid {true};
    for (const InputDataValidation::FilenameInfo& v : validation) {
      if (required.count(v.member_name) &&
          v.validity != InputDataValidation::FilenameInfo::valid) {
        is_valid = false;
      }
    }
    return is_valid;
  }

  void LoadSessionFromSequence::clearNonExistantDefaultGeneratedFilenames(Filenames& f)
  {
    // clearNonExistantFilename(f.sequence_csv_i);   // The file must exist
    // clearNonExistantFilename(f.parameters_csv_i); // The file must exist
    // clearNonExistantFilename(f.traML_csv_i);      // The file must exist
    clearNonExistantFilename(f.featureFilterComponents_csv_i);
    clearNonExistantFilename(f.featureFilterComponentGroups_csv_i);
    clearNonExistantFilename(f.featureQCComponents_csv_i);
    clearNonExistantFilename(f.featureQCComponentGroups_csv_i);
    clearNonExistantFilename(f.quantitationMethods_csv_i);
    clearNonExistantFilename(f.standardsConcentrations_csv_i);
    clearNonExistantFilename(f.referenceData_csv_i);
  }

  void LoadSessionFromSequence::clearNonExistantFilename(std::string& filename)
  {
    if (InputDataValidation::fileExists(filename) == false) {
      filename.clear();
    }
  }

  std::string LoadSessionFromSequence::getValidPathnameOrPlaceholder(
    const std::string& pathname, const bool is_valid
  )
  {
    const std::string placeholder = "";
    return (is_valid ? pathname : placeholder) + "\n";
  }

  void LoadSessionFromSequence::updateFilenames(Filenames& f, const std::string& pathname)
  {
    std::ifstream stream(pathname);
    const std::regex re("([a-zA-Z_]+)=([^\\s]*)");
    std::smatch match;
    std::string line;
    if (!stream.is_open()) {
      LOGF << "Can't open file: " << pathname << "\n";
      return;
    }
    while (std::getline(stream, line)) {
      const bool matched = std::regex_match(line, match, re);
      if (matched == false) {
        LOGF << "\n\n"
          "Regex did not match with the line: " << line << "\n"
          "Please make sure that the format is correct.\n";
        std::exit(EXIT_FAILURE);
      }
      std::string label;
      std::string value;
      // The entire match is at index 0
      // Parenthesized sub-matches start from index 1
      if (match.size() == 3) {
        label = match[1].str();
        value = match[2].str();
        // LOGD << label << "=" << value << '\n';
      }
      if (label == "sequence") {
        f.sequence_csv_i = value;
        if (value.empty()) {
          LOGE << "\n\nError!!! The sequence csv file cannot be empty.\n";
        }
      } else if (label == "parameters") {
        f.parameters_csv_i = value;
        if (value.empty()) {
          LOGE << "\n\nError!!! The parameters csv file cannot be empty.\n";
        }
      } else if (label == "traml") {
        f.traML_csv_i = value;
        if (value.empty()) {
          LOGE << "\n\nError!!! The TraML file cannot be empty.\n";
        }
      } else if (label == "featureFilter") {
        f.featureFilterComponents_csv_i = value;
      } else if (label == "featureFilterGroups") {
        f.featureFilterComponentGroups_csv_i = value;
      } else if (label == "featureQC") {
        f.featureQCComponents_csv_i = value;
      } else if (label == "featureQCGroups") {
        f.featureQCComponentGroups_csv_i = value;
      } else if (label == "quantitationMethods") {
        f.quantitationMethods_csv_i = value;
      } else if (label == "standardsConcentrations") {
        f.standardsConcentrations_csv_i = value;
      } else if (label == "referenceData") {
        f.referenceData_csv_i = value;
      } else if (label == "workflow") {
        f.workflow_csv_i = value;
      } else {
        LOGE << "\n\nLabel is not valid: " << label << "\n";
        std::exit(EXIT_FAILURE);
      }
    }
  }

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

  bool LoadSessionFromSequence::processFilePicker()
  {
    application_handler_.sequence_pathname_ = pathname_;
    application_handler_.mzML_dir_.clear();
    application_handler_.features_in_dir_.clear();
    application_handler_.features_out_dir_.clear();
    LOGI << "Pathnames for 'mzML', 'INPUT features' and 'OUTPUT features' reset.";
    const bool pathnamesAreCorrect = buildStaticFilenames();
    if (pathnamesAreCorrect) {
      application_handler_.sequenceHandler_.clear();
      CreateSequence cs(application_handler_.sequenceHandler_);
      cs.filenames_        = application_handler_.static_filenames_;
      cs.delimiter        = ",";
      cs.checkConsistency = false; // NOTE: Requires a lot of time on large sequences with a large number of components
      cs.process();
      return true;
    } 
    else 
    {
      LOGE << "Provided and/or inferred pathnames are not correct."
        "The sequence has not been modified.";
      return false;
    }
  }

  bool RawDataFilePickerProcessor::processFilePicker()
  {
    if (!raw_data_processor_)
    {
      LOGE << "RawData Processor not set";
      return false;
    }
    else if (application_handler_.sequenceHandler_.getSequence().size() == 0)
    {
      LOGE << "File cannot be loaded without first loading the sequence.";
      return false;
    }
    else if (!filename_output_)
    {
      LOGE << "No filename output provided, cannot not load file";
      return false;
    }
    else
    {
      RawDataHandler& rawDataHandler = application_handler_.sequenceHandler_.getSequence().at(0).getRawData();
      *filename_output_ = pathname_;
      raw_data_processor_->process(rawDataHandler, {}, filenames_);
      return true;
    }
  }

  bool SequenceSegmentFilePickerProcessor::processFilePicker()
  {
    if (!sequence_segment_processor_)
    {
      LOGE << "SequenceSegment Processor not set";
      return false;
    }
    else if (application_handler_.sequenceHandler_.getSequence().size() == 0)
    {
      LOGE << "File cannot be loaded without first loading the sequence.";
      return false;
    }
    else if (!filename_output_)
    {
      LOGE << "No filename output provided, cannot not load file";
      return false;
    }
    else
    {
      for (SequenceSegmentHandler& sequenceSegmentHandler : application_handler_.sequenceHandler_.getSequenceSegments()) {
        // TODO original instanciates one processor for each iteration of the loop
        if (group_)
        {
          *filename_output_ = "";
          *filename_output_group_ = pathname_;
        }
        else
        {
          *filename_output_ = pathname_;
          *filename_output_group_ = "";
        }
        sequence_segment_processor_->sequence_segment_observable_ = &application_handler_.sequenceHandler_;
        sequence_segment_processor_->process(sequenceSegmentHandler, SequenceHandler(), {}, filenames_);
      }
      return true;
    }
  }

  bool StoreSequence::processFilePicker()
  {
    if (application_handler_.sequenceHandler_.getSequence().size()) {
      RawDataHandler& rawDataHandler = application_handler_.sequenceHandler_.getSequence().at(0).getRawData();
      StoreSequenceFileSmartPeak storeSequenceSmartPeak(application_handler_);
      storeSequenceSmartPeak.pathname_ = pathname_;
      storeSequenceSmartPeak.process();
      return true;
    }
    else
    {
      LOGE << "Parameters file cannot be stored without first loading the sequence.";
      return false;
    }
  }

  bool StoreSequenceFileSmartPeak::process() {
    if (application_handler_.sequenceHandler_.getSequence().size()) {
      SequenceParser::writeSequenceFileSmartPeak(application_handler_.sequenceHandler_, pathname_);
      return true;
    }
    else
    {
      LOGE << "Sequence file cannot be converted and stored without first loading the sequence.";
      return false;
    }
  }

  bool StoreSequenceFileAnalyst::process() {
    if (application_handler_.sequenceHandler_.getSequence().size()) {
      SequenceParser::writeSequenceFileAnalyst(application_handler_.sequenceHandler_, application_handler_.main_dir_ + "/SequenceFileAnalyst.txt");
      return true;
    }
    else 
    {
      LOGE << "Sequence file cannot be converted and stored without first loading the sequence.";
      return false;
    }
  }

  bool StoreSequenceFileMasshunter::process()
  {
    if (application_handler_.sequenceHandler_.getSequence().size()) {
      SequenceParser::writeSequenceFileMasshunter(application_handler_.sequenceHandler_, application_handler_.main_dir_ + "/SequenceFileMasshunter.tsv");
      return true;
    }
    else 
    {
      LOGE << "Sequence file cannot be converted and stored without first loading the sequence.";
      return false;
    }
  }

  bool StoreSequenceFileXcalibur::process()
  {
    if (application_handler_.sequenceHandler_.getSequence().size()) {
      SequenceParser::writeSequenceFileXcalibur(application_handler_.sequenceHandler_, application_handler_.main_dir_ + "/SequenceFileXcalibur.tsv");
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

  bool SetRawDataPathname::processFilePicker()
  {
    application_handler_.mzML_dir_ = pathname_;
    return true;
  }

  bool SetInputFeaturesPathname::processFilePicker()
  {
    application_handler_.features_in_dir_ = pathname_;
    return true;
  }

  bool SetOutputFeaturesPathname::processFilePicker()
  {
    application_handler_.features_out_dir_ = pathname_;
    return true;
  }

  bool LoadSequenceWorkflow::processFilePicker()
  {
    LoadWorkflow load_workflow(application_handler_.sequenceHandler_);
    load_workflow.filename_ = pathname_;
    load_workflow.process();
    return true;
  }

  bool StoreSequenceWorkflow::processFilePicker()
  {
    StoreWorkflow store_workflow(application_handler_.sequenceHandler_);
    store_workflow.filename_ = pathname_;
    store_workflow.process();
    return true;
  }
}
