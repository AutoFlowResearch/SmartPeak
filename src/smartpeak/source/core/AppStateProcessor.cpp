#include <SmartPeak/core/AppStateProcessor.h>
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
    Filenames& f = state_.static_filenames_;
    state_.main_dir_ = state_.sequence_pathname_.substr(0, state_.sequence_pathname_.find_last_of('/'));
    f = Filenames::getDefaultStaticFilenames(state_.main_dir_);
    clearNonExistantDefaultGeneratedFilenames(f);
    f.sequence_csv_i = state_.sequence_pathname_;

    const std::string pathnamesFilePath = state_.main_dir_ + "/" + state_.pathnamesFilename_;

    // if (InputDataValidation::fileExists(pathnamesFilePath)) {
    //   LOGN << "\n\n"
    //     "Pathnames file was found:\n" <<
    //     " - " << pathnamesFilePath << "\n"
    //     "Should its values be used to search for the input files? [Y/n]\n";
    //   const std::string in = getLineInput("> ");
    //   if (in.empty() || in.front() == 'y') {
    //     LOGN << "\n\nValues in " << pathnamesFilePath << ": USED\n";
    //     updateFilenames(f, pathnamesFilePath);
    //   } else {
    //     LOGN << "\n\nValues in " << pathnamesFilePath << ": IGNORED\n";
    //   }
    // }

    if (InputDataValidation::fileExists(pathnamesFilePath)) {
      LOGN << "\n\n"
        "Pathnames file was found:\n" <<
        " - " << pathnamesFilePath << "\n"
        "Its values will be used.\n";
      LOGN << "\n\nValues in " << pathnamesFilePath << ": USED\n";
      updateFilenames(f, pathnamesFilePath);
    }

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
      generatePathnamesTxt(pathnamesFilePath, f, is_valid);
      LOGF << "\n\nERROR!!!\n"
        "One or more pathnames are not valid.\n"
        "A file has been generated for you to fix the pathnames:\n"
        " - " << pathnamesFilePath << "\n"
        "The incorrect information has been replaced with an empty value.\n"
        "If a pathname is to be ignored, leave it blank.\n";
      if (!requiredPathnamesAreValidBool) {
        LOGF << "\n\n"
        "Make sure that the following required pathnames are provided:\n"
        " - sequence\n"
        " - parameters\n"
        " - traml\n";
      }
      LOGN << "Apply the fixes and reload the sequence file.\n";
      // getLineInput("Press Enter to go back to the Main menu.\n");
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

  void LoadSessionFromSequence::generatePathnamesTxt(
    const std::string& pathname,
    const Filenames& f,
    const std::vector<InputDataValidation::FilenameInfo>& is_valid
  )
  {
    std::ofstream ofs(pathname);
    if (!ofs.is_open()) {
      LOGF << "\n\nCan't open file: " << pathname << "\n";
      return;
    }
    std::vector<InputDataValidation::FilenameInfo>::const_iterator it = is_valid.cbegin();
    ofs <<
      "sequence="            << getValidPathnameOrPlaceholder(f.sequence_csv_i, (it++)->validity) <<
      "parameters="          << getValidPathnameOrPlaceholder(f.parameters_csv_i, (it++)->validity) <<
      "traml="               << getValidPathnameOrPlaceholder(f.traML_csv_i, (it++)->validity) <<
      "featureFilter="       << getValidPathnameOrPlaceholder(f.featureFilterComponents_csv_i, (it++)->validity) <<
      "featureFilterGroups=" << getValidPathnameOrPlaceholder(f.featureFilterComponentGroups_csv_i, (it++)->validity) <<
      "featureQC="           << getValidPathnameOrPlaceholder(f.featureQCComponents_csv_i, (it++)->validity) <<
      "featureQCGroups="     << getValidPathnameOrPlaceholder(f.featureQCComponentGroups_csv_i, (it++)->validity) <<
      "quantitationMethods=" << getValidPathnameOrPlaceholder(f.quantitationMethods_csv_i, (it++)->validity) <<
      "standardsConcentrations=" << getValidPathnameOrPlaceholder(f.standardsConcentrations_csv_i, (it++)->validity) <<
      "referenceData="       << getValidPathnameOrPlaceholder(f.referenceData_csv_i, (it++)->validity);
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

  namespace AppStateProcessors {
  void processCommands(AppState& state, std::vector<AppState::Command> commands)
  {
    size_t i = 0;
    while (i < commands.size()) {
      const AppState::Command::CommandType type = commands[i].type;
      size_t j = i + 1;
      for (; j < commands.size() && type == commands[j].type; ++j) {
        // empty body
      }
      const AppState::Command& cmd = commands[i];
      if (cmd.type == AppState::Command::RawDataMethod) {
        std::vector<std::shared_ptr<RawDataProcessor>> raw_methods;
        std::transform(commands.begin() + i, commands.begin() + j, std::back_inserter(raw_methods),
          [](const AppState::Command& command){ return command.raw_data_method; });
        ProcessSequence ps(state.sequenceHandler_);
        ps.filenames                     = cmd.dynamic_filenames;
        ps.raw_data_processing_methods_I = raw_methods;
        ps.process();
      } else if (cmd.type == AppState::Command::SequenceSegmentMethod) {
        std::vector<std::shared_ptr<SequenceSegmentProcessor>> seq_seg_methods;
        std::transform(commands.begin() + i, commands.begin() + j, std::back_inserter(seq_seg_methods),
          [](const AppState::Command& command){ return command.seq_seg_method; });
        ProcessSequenceSegments pss(state.sequenceHandler_);
        pss.filenames                             = cmd.dynamic_filenames;
        pss.sequence_segment_processing_methods_I = seq_seg_methods;
        pss.process();
      } else {
        LOGW << "Skipping a command: " << cmd.type << "\n";
      }
      i = j;
    }
  }
  }

  bool CreateCommand::operator()(const int n, AppState::Command& cmd)
  {
    if (n < 1 || n > 16 || n == 10) { // TODO: update this if plotting is implemented
      LOGW << "\n\nSkipping: " << n;
      return false;
    }
    if (n >= 1 && n <= 13) {
      cmd.setMethod(n_to_raw_data_method_.at(n));
      for (const InjectionHandler& injection : state_.sequenceHandler_.getSequence()) {
        const std::string& key = injection.getMetaData().getInjectionName();
        cmd.dynamic_filenames[key] = Filenames::getDefaultDynamicFilenames(
          state_.mzML_dir_,
          state_.features_in_dir_,
          state_.features_out_dir_,
          injection.getMetaData().getSampleName(),
          key
        );
      }
    } else if (n >= 14 && n <= 16) {
      cmd.setMethod(n_to_seq_seg_method_.at(n));
      for (const SequenceSegmentHandler& sequence_segment : state_.sequenceHandler_.getSequenceSegments()) {
        const std::string& key = sequence_segment.getSequenceSegmentName();
        cmd.dynamic_filenames[key] = Filenames::getDefaultDynamicFilenames(
          state_.mzML_dir_,
          state_.features_in_dir_,
          state_.features_out_dir_,
          key,
          key
        );
      }
    } else {
      LOGE << "\nNo command for selection number " << n;
      return false;
    }
    return true;
  }

  void LoadSessionFromSequence::operator()(const char* const pathname)
  {
    state_.sequence_pathname_ = std::string(pathname);
    state_.mzML_dir_.clear();
    state_.features_in_dir_.clear();
    state_.features_out_dir_.clear();
    LOGI << "Pathnames for 'mzML', 'INPUT features' and 'OUTPUT features' reset.";
    const bool pathnamesAreCorrect = buildStaticFilenames();
    if (pathnamesAreCorrect) {
      state_.sequenceHandler_.clear();
      CreateSequence cs(state_.sequenceHandler_);
      cs.filenames        = state_.static_filenames_;
      cs.delimiter        = ",";
      cs.checkConsistency = true;
      cs.process();
    } else {
      LOGE << "Provided and/or inferred pathnames are not correct."
        "The sequence has not been modified. Check file: " << state_.pathnamesFilename_;
    }
  }

  std::vector<AppState::Command> BuildCommandsFromIds::operator()(const std::string& ids)
  {
    std::vector<AppState::Command> commands;

    std::istringstream iss {ids};

    for (int n; iss >> n;) {
      AppState::Command cmd;
      CreateCommand createCommand(state_);
      const bool created = createCommand(n, cmd);
      if (created) {
        commands.push_back(cmd);
      }
    }

    return commands;
  }

  void SetRawDataPathname::operator()(const char* const pathname)
  {
    state_.mzML_dir_ = pathname;
  }

  void SetInputFeaturesPathname::operator()(const char* const pathname)
  {
    state_.features_in_dir_ = pathname;
  }

  void SetOutputFeaturesPathname::operator()(const char* const pathname)
  {
    state_.features_out_dir_ = pathname;
  }
}
