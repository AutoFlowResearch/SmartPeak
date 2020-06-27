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
    std::vector<InputDataValidation::FilenameInfo> is_valid(10);
    is_valid[0] = InputDataValidation::isValidFilename(f.sequence_csv_i, "sequence");
    is_valid[1] = InputDataValidation::isValidFilename(f.parameters_csv_i, "parameters");
    is_valid[2] = InputDataValidation::isValidFilename(f.traML_csv_i, "traml");
    is_valid[3] = InputDataValidation::isValidFilename(f.featureFilterComponents_csv_i, "featureFilter");
    is_valid[4] = InputDataValidation::isValidFilename(f.featureFilterComponentGroups_csv_i, "featureFilterGroups");
    is_valid[5] = InputDataValidation::isValidFilename(f.featureQCComponents_csv_i, "featureQC");
    is_valid[6] = InputDataValidation::isValidFilename(f.featureQCComponentGroups_csv_i, "featureQCGroups");
    is_valid[7] = InputDataValidation::isValidFilename(f.quantitationMethods_csv_i, "quantitationMethods");
    is_valid[8] = InputDataValidation::isValidFilename(f.standardsConcentrations_csv_i, "standardsConcentrations");
    is_valid[9] = InputDataValidation::isValidFilename(f.referenceData_csv_i, "referenceData");

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
      } else {
        LOGE << "\n\nLabel is not valid: " << label << "\n";
        std::exit(EXIT_FAILURE);
      }
    }
  }

  namespace ApplicationProcessors {
  void processCommands(ApplicationHandler& state, std::vector<ApplicationHandler::Command> commands, const std::set<std::string>& injection_names, const std::set<std::string>& sequence_segment_names)
  {
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
        ProcessSequence ps(state.sequenceHandler_);
        ps.filenames = cmd.dynamic_filenames;
        ps.raw_data_processing_methods_I = raw_methods;
        ps.injection_names = injection_names;
        ps.process();
      } else if (cmd.type == ApplicationHandler::Command::SequenceSegmentMethod) {
        std::vector<std::shared_ptr<SequenceSegmentProcessor>> seq_seg_methods;
        std::transform(commands.begin() + i, commands.begin() + j, std::back_inserter(seq_seg_methods),
          [](const ApplicationHandler::Command& command){ return command.seq_seg_method; });
        ProcessSequenceSegments pss(state.sequenceHandler_);
        pss.filenames = cmd.dynamic_filenames;
        pss.sequence_segment_processing_methods_I = seq_seg_methods;
        pss.sequence_segment_names = sequence_segment_names;
        pss.process();
      } else {
        LOGW << "Skipping a command: " << cmd.type << "\n";
      }
      i = j;
    }
  }
  }

  bool CreateCommand::process(){
    // Enumerate the valid command keys
    std::vector<std::string> valid_commands_raw_data_processor;
    for (const auto& it: n_to_raw_data_method_) { valid_commands_raw_data_processor.push_back(it.first); }
    std::vector<std::string> valid_commands_sequence_segment_processor;
    for (const auto& it: n_to_seq_seg_method_) { valid_commands_sequence_segment_processor.push_back(it.first); }

    // Run the command depending on whether it is a raw data processor method or sequence segment processor method
    if (std::count(valid_commands_raw_data_processor.begin(), valid_commands_raw_data_processor.end(), name_)) {
      cmd_.setMethod(n_to_raw_data_method_.at(name_));
      for (const InjectionHandler& injection : application_handler_.sequenceHandler_.getSequence()) {
        const std::string& key = injection.getMetaData().getInjectionName();
        cmd_.dynamic_filenames[key] = Filenames::getDefaultDynamicFilenames(
          application_handler_.mzML_dir_,
          application_handler_.features_in_dir_,
          application_handler_.features_out_dir_,
          injection.getMetaData().getSampleName(),
          key
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
          key,
          key
        );
      }
    } else {
      LOGE << "\nNo command for selection name " << name_;
      return false;
    }
    return true;
  }

  bool LoadSessionFromSequence::process()
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
      cs.filenames        = application_handler_.static_filenames_;
      cs.delimiter        = ",";
      cs.checkConsistency = true;
      cs.process();
      return true;
    } else {
      LOGE << "Provided and/or inferred pathnames are not correct."
        "The sequence has not been modified.";
      return false;
    }
  }

  bool BuildCommandsFromNames::process()
  {
    commands_.clear();

    std::istringstream iss {names_};

    for (std::string n; iss >> n;) {
      CreateCommand createCommand(application_handler_);
      createCommand.name_ = n;
      const bool created = createCommand.process();
      if (created) {
        commands_.push_back(createCommand.cmd_);
      }
    }
    return true;
  }

  bool SetRawDataPathname::process()
  {
    application_handler_.mzML_dir_ = pathname_;
    return true;
  }

  bool SetInputFeaturesPathname::process()
  {
    application_handler_.features_in_dir_ = pathname_;
    return true;
  }

  bool SetOutputFeaturesPathname::process()
  {
    application_handler_.features_out_dir_ = pathname_;
    return true;
  }
}
