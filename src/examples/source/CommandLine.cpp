#include <SmartPeak/core/Filenames.h>
#include <SmartPeak/core/RawDataProcessor.h>
#include <SmartPeak/core/SequenceProcessor.h>
#include <SmartPeak/io/InputDataValidation.h>
#include <SmartPeak/io/SequenceParser.h>
#include <algorithm>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <regex>
#include <unordered_map>
#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;
// #include <filesystem> // requires gcc 8, the default is 7.3.0 in ubuntu 18.04
// namespace fs = std::filesystem;

using namespace SmartPeak;

class CommandLine final {
public:
  class Command {
  public:
    enum CommandType {
      RawDataMethod,
      SequenceSegmentMethod,
      WriteSequenceSummary,
      WriteFeatureSummary
    } type;

    void setMethod(const RawDataProcessor::RawDataProcMethod method)
    {
      type = RawDataMethod;
      raw_data_method = method;
    }

    void setMethod(const SequenceSegmentProcessor::SeqSegProcMethod method)
    {
      type = SequenceSegmentMethod;
      seq_seg_method = method;
    }

    union {
      RawDataProcessor::RawDataProcMethod raw_data_method;
      SequenceSegmentProcessor::SeqSegProcMethod seq_seg_method;
    };

    std::map<std::string, Filenames> dynamic_filenames;
  };

  std::string                           pathnamesFilename_       = "pathnames.txt";
  std::string                           sequence_pathname_;
  std::string                           main_dir_;
  std::string                           mzML_dir_;
  std::string                           features_in_dir_;
  std::string                           features_out_dir_;
  bool                                  verbose_                 = true;
  std::set<MetaDataHandler::SampleType> sequenceSummaryTypes_;
  std::set<MetaDataHandler::SampleType> featureSummaryTypes_;
  std::vector<std::string>              sequenceSummaryMetaData_;
  std::vector<std::string>              featureSummaryMetaData_;
  std::vector<Command>                  commands_;
  Filenames                             static_filenames_;
  SequenceHandler                       sequenceHandler_;

  const std::unordered_map<int, RawDataProcessor::RawDataProcMethod> n_to_raw_data_method_ {
    {1, RawDataProcessor::LOAD_RAW_DATA},
    {2, RawDataProcessor::LOAD_FEATURES},
    {3, RawDataProcessor::PICK_FEATURES},
    {4, RawDataProcessor::FILTER_FEATURES},
    {5, RawDataProcessor::SELECT_FEATURES},
    {6, RawDataProcessor::VALIDATE_FEATURES},
    {7, RawDataProcessor::QUANTIFY_FEATURES},
    {8, RawDataProcessor::CHECK_FEATURES},
    {9, RawDataProcessor::STORE_FEATURES},
    {10, RawDataProcessor::PLOT_FEATURES},
    {11, RawDataProcessor::SAVE_FEATURES},
    {12, RawDataProcessor::ANNOTATE_USED_FEATURES},
    {13, RawDataProcessor::CLEAR_FEATURE_HISTORY}
  };
  const std::unordered_map<int, SequenceSegmentProcessor::SeqSegProcMethod> n_to_seq_seg_method_ {
    {14, SequenceSegmentProcessor::CALCULATE_CALIBRATION},
    {15, SequenceSegmentProcessor::STORE_QUANTITATION_METHODS},
    {16, SequenceSegmentProcessor::LOAD_QUANTITATION_METHODS},
  };
  const std::string main_menu_ = "\n\n"
    "Please insert the sequence of methods to run.\n"
    "You can choose the same method multiple times.\n"
    "Separate chosen methods with a space.\n\n"
    "[1]  Load raw data\n"
    "[2]  Load features\n"
    "[3]  Pick features\n"
    "[4]  Filter features\n"
    "[5]  Select features\n"
    "[6]  Validate features\n"
    "[7]  Quantify features\n"
    "[8]  Check features\n"
    "[9]  Store features\n"
    "[10] Plot features (not implemented)\n"
    "[11] Save features\n"
    "[12] Annotate used features\n"
    "[13] Clear feature history\n"
    "[14] Calculate calibration\n"
    "[15] Store quantitation methods\n"
    "[16] Load quantitation methods\n"
    "[17] Write SequenceSummary.csv\n"
    "[18] Write FeatureSummary.csv\n"
    "[M]  Main menu\n\n"
    "Presets:\n"
    "Unknowns: 1 3 4 4 5 7 8 9 17 18\n"
    "Standards: 1 3 4 4 5 8 9 14 15 7 8 9 17 18\n"
    "Validation: 1 3 4 5 6 17 18\n\n";

  void buildStaticFilenames()
  {
    Filenames& f = static_filenames_;
    main_dir_ = fs::path(sequence_pathname_).parent_path().string();
    f = Filenames::getDefaultStaticFilenames(main_dir_);
    clearNonExistantDefaultGeneratedFilenames(f);
    f.sequence_csv_i = sequence_pathname_;

    const std::string pathnamesFilePath = main_dir_ + "/" + std::string(pathnamesFilename_);

    if (InputDataValidation::fileExists(pathnamesFilePath)) {
      std::cout << "\n\n" <<
        "File " << pathnamesFilePath << " was found in the same directory. " <<
        "This file contains information about where the various experiment's files are found.\n\n" <<
        "Should its values be used to search for pathnames? [y]/n\n";
      const std::string in = getLineInput("> ");
      std::cout << '\n';
      if (in.empty() || in.front() == 'y') {
        updateFilenames(f, pathnamesFilePath);
        std::cout << "Values in " << pathnamesFilePath << ": USED\n";
      } else {
        std::cout << "Values in " << pathnamesFilePath << ": IGNORED\n";
      }
    }

    std::cout << "\n\n" <<
      "The following list of file was searched for:\n";
    std::vector<bool> is_valid(10);
    is_valid[0] = InputDataValidation::isValidFilename(f.sequence_csv_i, "sequence");
    is_valid[1] = InputDataValidation::isValidFilename(f.parameters_csv_i, "parameters");
    is_valid[2] = InputDataValidation::isValidFilename(f.traML_csv_i, "traml");
    is_valid[3] = InputDataValidation::isValidFilename(f.featureFilterComponents_csv_i, "feature_filter_c");
    is_valid[4] = InputDataValidation::isValidFilename(f.featureFilterComponentGroups_csv_i, "feature_filter_c_groups");
    is_valid[5] = InputDataValidation::isValidFilename(f.featureQCComponents_csv_i, "feature_qc_c");
    is_valid[6] = InputDataValidation::isValidFilename(f.featureQCComponentGroups_csv_i, "feature_qc_c_groups");
    is_valid[7] = InputDataValidation::isValidFilename(f.quantitationMethods_csv_i, "quantitation_methods");
    is_valid[8] = InputDataValidation::isValidFilename(f.standardsConcentrations_csv_i, "standards_concentrations");
    is_valid[9] = InputDataValidation::isValidFilename(f.referenceData_csv_i, "reference_data");

    const bool something_has_failed = std::any_of(is_valid.cbegin(), is_valid.cend(),
      [](const bool arg){ return false == arg; });

    if (something_has_failed) {
      generatePathnamesTxt(pathnamesFilePath, f, is_valid);
      std::cout << "\n\nOne or more files were not found.\n" <<
        "The file " << pathnamesFilePath <<
        " has been generated for you to fix pathnames.\n" <<
        "The incorrect information has been replaced with an empty value.\n" <<
        "If you want a pathname to be ignored, then remove its value and leave only the label.\n" <<
        "Make sure that the pathnames are correct and run the application again.\n";
      std::exit(EXIT_FAILURE);
    }
  }

  void clearNonExistantDefaultGeneratedFilenames(Filenames& f)
  {
    clearNonExistantFilename(f.parameters_csv_i);
    clearNonExistantFilename(f.traML_csv_i);
    clearNonExistantFilename(f.featureFilterComponents_csv_i);
    clearNonExistantFilename(f.featureFilterComponentGroups_csv_i);
    clearNonExistantFilename(f.featureQCComponents_csv_i);
    clearNonExistantFilename(f.featureQCComponentGroups_csv_i);
    clearNonExistantFilename(f.quantitationMethods_csv_i);
    clearNonExistantFilename(f.standardsConcentrations_csv_i);
    clearNonExistantFilename(f.referenceData_csv_i);
  }

  void clearNonExistantFilename(std::string& filename)
  {
    if (fs::exists(filename) == false) {
      filename.clear();
    }
  }

  void generatePathnamesTxt(const std::string& pathname, const Filenames& f, const std::vector<bool>& is_valid)
  {
    std::ofstream ofs(pathname);
    if (!ofs.is_open()) {
      std::cout << "\n\nCannot open " << pathname << "\n";
      std::exit(EXIT_FAILURE);
    }
    std::vector<bool>::const_iterator it = is_valid.cbegin();
    ofs <<
      "sequence="                 << getPathnameOrPlaceholder(f.sequence_csv_i, *it++) <<
      "parameters="               << getPathnameOrPlaceholder(f.parameters_csv_i, *it++) <<
      "traml="                    << getPathnameOrPlaceholder(f.traML_csv_i, *it++) <<
      "feature_filter_c="         << getPathnameOrPlaceholder(f.featureFilterComponents_csv_i, *it++) <<
      "feature_filter_c_groups="  << getPathnameOrPlaceholder(f.featureFilterComponentGroups_csv_i, *it++) <<
      "feature_qc_c="             << getPathnameOrPlaceholder(f.featureQCComponents_csv_i, *it++) <<
      "feature_qc_c_groups="      << getPathnameOrPlaceholder(f.featureQCComponentGroups_csv_i, *it++) <<
      "quantitation_methods="     << getPathnameOrPlaceholder(f.quantitationMethods_csv_i, *it++) <<
      "standards_concentrations=" << getPathnameOrPlaceholder(f.standardsConcentrations_csv_i, *it++) <<
      "reference_data="           << getPathnameOrPlaceholder(f.referenceData_csv_i, *it++);
  }

  std::string getPathnameOrPlaceholder(const std::string& pathname, const bool is_valid)
  {
    const std::string placeholder = "";
    return (is_valid ? pathname : placeholder) + "\n";
  }

  void updateFilenames(Filenames& f, const std::string& pathname)
  {
    std::ifstream stream(pathname);
    const std::regex re("([a-zA-Z_]+)=([^\\s]*)");
    std::smatch match;
    std::string line;
    while (std::getline(stream, line)) {
      const bool matched = std::regex_match(line, match, re);
      if (matched == false) {
        std::cout << "\n\n" <<
          "Regex did not match with the line: " << line << "\n" <<
          "Please make sure that the format is correct.\n";
        std::exit(EXIT_FAILURE);
      }
      std::string label;
      std::string value;
      if (match.size() == 3) { // the entire match is at index 0, and parenthesized sub-matches start from index 1
        label = match[1].str();
        value = match[2].str();
        // std::cout << label << "=" << value << '\n';
      }
      if (label == "sequence") {
        f.sequence_csv_i = value;
      } else if (label == "parameters") {
        f.parameters_csv_i = value;
      } else if (label == "traml") {
        f.traML_csv_i = value;
      } else if (label == "feature_filter_c") {
        f.featureFilterComponents_csv_i = value;
      } else if (label == "feature_filter_c_groups") {
        f.featureFilterComponentGroups_csv_i = value;
      } else if (label == "feature_qc_c") {
        f.featureQCComponents_csv_i = value;
      } else if (label == "feature_qc_c_groups") {
        f.featureQCComponentGroups_csv_i = value;
      } else if (label == "quantitation_methods") {
        f.quantitationMethods_csv_i = value;
      } else if (label == "standards_concentrations") {
        f.standardsConcentrations_csv_i = value;
      } else if (label == "reference_data") {
        f.referenceData_csv_i = value;
      } else {
        std::cout << "\n\nLabel is not valid: " << label << "\n";
        std::exit(EXIT_FAILURE);
      }
    }
  }

  std::vector<Command> getMethodsInput()
  {
    std::vector<Command> methods;
    std::string line;
    std::istringstream iss;

    std::cout << main_menu_;

    do {
      line = getLineInput("> ");
    } while (line.empty());

    if (line[0] == 'M' || line[0] == 'm') {
      return {};
    }

    iss.str(line);

    for (int n; iss >> n;) {
      if (n < 1 || n > 18 || n == 10) { // TODO: update this if plotting is implemented
        std::cout << "Skipping: " << n << '\n';
        continue;
      }
      Command cmd;
      if (n >= 1 && n <= 13) {
        cmd.setMethod(n_to_raw_data_method_.at(n));
        for (const InjectionHandler& injection : sequenceHandler_.getSequence()) {
          const std::string& key = injection.getMetaData().getInjectionName();
          cmd.dynamic_filenames[key] = Filenames::getDefaultDynamicFilenames(
            mzML_dir_,
            features_in_dir_,
            features_out_dir_,
            injection.getMetaData().getSampleName(),
            key
          );
        }
      } else if (n >= 14 && n <= 16) {
        cmd.setMethod(n_to_seq_seg_method_.at(n));
        for (const SequenceSegmentHandler& sequence_segment : sequenceHandler_.getSequenceSegments()) {
          const std::string& key = sequence_segment.getSequenceSegmentName();
          cmd.dynamic_filenames[key] = Filenames::getDefaultDynamicFilenames(
            mzML_dir_,
            features_in_dir_,
            features_out_dir_,
            key,
            key
          );
        }
      } else if (n == 17) {
        cmd.type = Command::WriteSequenceSummary;
        sequenceSummaryMetaData_ = getMetaDataInput("\nSequenceSummary.csv\n");
        sequenceSummaryTypes_ = getSampleTypesInput();
      } else {
        cmd.type = Command::WriteFeatureSummary;
        featureSummaryMetaData_ = getMetaDataInput("\nFeatureSummary.csv\n");
        featureSummaryTypes_ = getSampleTypesInput();
      }
      methods.push_back(cmd);
    }

    return methods;
  }

  void setSequencePathnameFromInput()
  {
    std::cout << "\n\n" <<
      "Please provide the pathname for the sequence file.\n" <<
      "Example:\n" <<
      // "> /home/user/data/some_sequence_file.csv\n\n" <<
      "> /home/pasdom/SmartPeak2/src/examples/data/HPLC_UV_Standards/sequence.csv\n\n" <<
      "Enter the absolute pathname:\n";
    while (true) {
      sequence_pathname_ = getLineInput("> ", false);
      if (fs::exists(sequence_pathname_)) {
        break;
      }
      std::cout << "The file does not exist. Try again.\n";
    }
  }

  std::string getLineInput(const std::string& message = "", const bool canBeEmpty = true)
  {
    std::string line;
    do {
      if (message.size()) {
        std::cout << message;
      }
      std::getline(std::cin, line);
      line = Utilities::trimString(line);
    } while (!canBeEmpty && line.empty());
    return line;
  }

  std::set<MetaDataHandler::SampleType> getSampleTypesInput()
  {
    std::cout << "\nPlease select the sample types. Insert the indexes separated by a space:\n" <<
      "[1] Unknown\n" <<
      "[2] Standard\n" <<
      "[3] QC\n" <<
      "[4] Blank\n" <<
      "[5] DoubleBlank\n" <<
      "[6] Solvent\n" <<
      "[M] Main menu\n\n";

    std::string line;

    do {
      line = getLineInput("> ");
    } while (line.empty());

    if (line[0] == 'M' || line[0] == 'm') {
      return {};
    }

    std::istringstream iss;
    iss.str(line);
    std::set<MetaDataHandler::SampleType> sample_types;

    for (int n; iss >> n;) {
      if (n < 1 || n > 6) {
        std::cout << "Skipping: " << n << '\n';
        continue;
      }
      switch (n) {
        case 1:
          sample_types.insert(MetaDataHandler::Unknown);
          break;
        case 2:
          sample_types.insert(MetaDataHandler::Standard);
          break;
        case 3:
          sample_types.insert(MetaDataHandler::QC);
          break;
        case 4:
          sample_types.insert(MetaDataHandler::Blank);
          break;
        case 5:
          sample_types.insert(MetaDataHandler::DoubleBlank);
          break;
        case 6:
          sample_types.insert(MetaDataHandler::Solvent);
          break;
      }
    }

    return sample_types;
  }

  std::vector<std::string> getMetaDataInput(
    const std::string& title
  )
  {
    std::cout << title <<
      "Please select the metadata. Insert the indexes separated by a space:\n" <<
      "[1]  Asymmetry factor\n" <<
      "[2]  Baseline delta/height\n" <<
      "[3]  Calculated concentration\n" <<
      "[4]  Log(Signal/Noise)\n" <<
      "[5]  Peak apex intensity\n" <<
      "[6]  Points across baseline\n" <<
      "[7]  Points across half height\n" <<
      "[8]  QC transition pass\n" <<
      "[9]  QC transition message\n" <<
      "[10] QC transition score\n" <<
      "[11] QC transition group message\n" <<
      "[12] QC transition group score\n" <<
      "[13] Tailing factor\n" <<
      "[14] Total width\n" <<
      "[15] Width at 50% peak's height\n" <<
      "[16] Preset (all): 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15\n\n";

    std::string line;

    do {
      line = getLineInput("> ");
    } while (line.empty());

    std::istringstream iss;
    iss.str(line);
    std::vector<std::string> metadata;

    for (int n; iss >> n;) {
      if (n < 1 || n > 16) {
        std::cout << "Skipping: " << n << '\n';
        continue;
      }
      switch (n) {
      case 1:
        metadata.push_back("asymmetry_factor");
        break;
      case 2:
        metadata.push_back("baseline_delta_2_height");
        break;
      case 3:
        metadata.push_back("calculated_concentration");
        break;
      case 4:
        metadata.push_back("logSN");
        break;
      case 5:
        metadata.push_back("peak_apex_int");
        break;
      case 6:
        metadata.push_back("points_across_baseline");
        break;
      case 7:
        metadata.push_back("points_across_half_height");
        break;
      case 8:
        metadata.push_back("QC_transition_pass");
        break;
      case 9:
        metadata.push_back("QC_transition_message");
        break;
      case 10:
        metadata.push_back("QC_transition_score");
        break;
      case 11:
        metadata.push_back("QC_transition_group_message");
        break;
      case 12:
        metadata.push_back("QC_transition_group_score");
        break;
      case 13:
        metadata.push_back("tailing_factor");
        break;
      case 14:
        metadata.push_back("total_width");
        break;
      case 15:
        metadata.push_back("width_at_50");
        break;
      case 16:
        metadata = {
          "peak_apex_int", "total_width", "width_at_50", "tailing_factor",
          "asymmetry_factor", "baseline_delta_2_height", "points_across_baseline",
          "points_across_half_height", "logSN", "calculated_concentration",
          "QC_transition_message", "QC_transition_pass", "QC_transition_score",
          "QC_transition_group_message", "QC_transition_group_score"
        };
        break;
      }
    }
    return metadata;
  }

  CommandLine(int argc, char **argv)
  {
    if (argc == 2 && fs::exists(argv[1])) {
      sequence_pathname_ = argv[1];
    } else if (fs::exists("sequence.csv")) {
      sequence_pathname_ = fs::current_path().string() + std::string("/sequence.csv");
    } else {
      setSequencePathnameFromInput();
    }

    buildStaticFilenames();
    SequenceProcessor::createSequence(sequenceHandler_, static_filenames_, ",", verbose_);

    while (true) {
      printMenu();
      const std::string line = getLineInput("> ", false);
      parseCommand(line);
    }
  }

  void printMenu()
  {
    std::cout << "\n\n" <<
      "SmartPeak Main menu\n" <<
      "[1] Set sequence.csv pathname\t[\"" << sequence_pathname_ << "\"]\n" <<
      "[2] Add processing step to pipeline [" << getPipelineString() << "]\n"
      "[3] Run the pipeline\n" <<
      "[E] Exit SmartPeak\n\n" <<
      "Please select your action.\n";
  }

  std::string getPipelineString()
  {
    std::string s;
    for (const Command& cmd : commands_) {
      if (cmd.type == Command::RawDataMethod) {
        const std::unordered_map<int, RawDataProcessor::RawDataProcMethod>::const_iterator
        it = std::find_if(
          n_to_raw_data_method_.cbegin(),
          n_to_raw_data_method_.cend(),
          [&cmd](const std::pair<int, RawDataProcessor::RawDataProcMethod>& p)
            { return p.second == cmd.raw_data_method; }
        );
        s.append(std::to_string(it->first));
      } else if (cmd.type == Command::SequenceSegmentMethod) {
        const std::unordered_map<int, SequenceSegmentProcessor::SeqSegProcMethod>::const_iterator
        it = std::find_if(
          n_to_seq_seg_method_.cbegin(),
          n_to_seq_seg_method_.cend(),
          [&cmd](const std::pair<int, SequenceSegmentProcessor::SeqSegProcMethod>& p)
            { return p.second == cmd.seq_seg_method; }
        );
        s.append(std::to_string(it->first));
      } else if(cmd.type == Command::WriteSequenceSummary) {
        s.append("17");
      } else if(cmd.type == Command::WriteFeatureSummary) {
        s.append("18");
      }
      s.append(" ");
    }
    if (s.size()) {
      s.pop_back();
    }
    return s;
  }

  void parseCommand(const std::string& line)
  {
    int opt;

    try {
      opt = std::stoi(line);
    } catch (const std::invalid_argument&) {
      if (line[0] == 'E' || line[0] == 'e') {
        std::exit(EXIT_SUCCESS);
      }
      std::cout << "Bad input. Try again.\n";
      return;
    }

    switch (opt) {
    case 1:
      setSequencePathnameFromInput();
      buildStaticFilenames();
      commands_.clear();
      sequenceHandler_.clear();
      SequenceProcessor::createSequence(sequenceHandler_, static_filenames_, ",", verbose_);
      break;
    case 2:
    {
      if (mzML_dir_.empty()) {
        mzML_dir_ = main_dir_ + "/mzML";
        std::cout << "\nPath for 'mzML':\t" << mzML_dir_ << '\n';
        std::cout << "Enter an absolute pathname to change it, or press Enter to confirm.\n";
        const std::string path_input = getLineInput("> ");
        if (path_input.size()) {
          mzML_dir_ = path_input;
        }
      }

      if (features_in_dir_.empty()) {
        features_in_dir_ = main_dir_ + "/features";
        std::cout << "\nPath for 'INPUT features':\t" << features_in_dir_ << '\n';
        std::cout << "Enter an absolute pathname to change it, or press Enter to confirm.\n";
        const std::string path_input = getLineInput("> ");
        if (path_input.size()) {
          features_in_dir_ = path_input;
        }
      }

      if (features_out_dir_.empty()) {
        features_out_dir_ = main_dir_ + "/features";
        std::cout << "\nPath for 'OUTPUT features':\t" << features_out_dir_ << '\n';
        std::cout << "Enter an absolute pathname to change it, or press Enter to confirm.\n";
        const std::string path_input = getLineInput("> ");
        if (path_input.size()) {
          features_out_dir_ = path_input;
        }
      }

      const std::vector<Command> methods = getMethodsInput();
      if (methods.empty()) {
        break;
      }
      commands_.insert(commands_.end(), methods.begin(), methods.end());
      break;
    }
    case 3:
    {
      size_t i = 0;
      while (i < commands_.size()) {
        const Command::CommandType type = commands_[i].type;
        size_t j = i + 1;
        for (; type == commands_[j].type; ++j)
        {
          // empty body
        }
        const Command& cmd = commands_[i];
        if (cmd.type == Command::RawDataMethod) {
          std::vector<RawDataProcessor::RawDataProcMethod> raw_methods;
          std::transform(commands_.begin() + i, commands_.begin() + j, std::back_inserter(raw_methods),
            [](const Command& command){ return command.raw_data_method; });
          SequenceProcessor::processSequence(
            sequenceHandler_,
            cmd.dynamic_filenames,
            std::set<std::string>(),
            raw_methods,
            verbose_
          );
        } else if (cmd.type == Command::SequenceSegmentMethod) {
          std::vector<SequenceSegmentProcessor::SeqSegProcMethod> seq_seg_methods;
          std::transform(commands_.begin() + i, commands_.begin() + j, std::back_inserter(seq_seg_methods),
            [](const Command& command){ return command.seq_seg_method; });
          SequenceProcessor::processSequenceSegments(
            sequenceHandler_,
            cmd.dynamic_filenames,
            std::set<std::string>(),
            seq_seg_methods,
            verbose_
          );
        } else if (cmd.type == Command::WriteSequenceSummary) {
          const std::string pathname = main_dir_ + "/SequenceSummary.csv";
          SequenceParser::writeDataMatrixFromMetaValue(
            sequenceHandler_,
            pathname,
            sequenceSummaryMetaData_,
            sequenceSummaryTypes_
          );
          std::cout << "SequenceSummary.csv file has been stored at: " << pathname << '\n';
        } else if (cmd.type == Command::WriteFeatureSummary) {
          const std::string pathname = main_dir_ + "/FeatureSummary.csv";
          SequenceParser::writeDataTableFromMetaValue(
            sequenceHandler_,
            pathname,
            featureSummaryMetaData_,
            featureSummaryTypes_
          );
          std::cout << "FeatureSummary.csv file has been stored at: " << pathname << '\n';
        }
        i = j;
      }
      break;
    }
    default:
      std::cout << "\nBad input. Try again.\n";
    }
  }
};

int main(int argc, char **argv)
{
  CommandLine cli(argc, argv);
  return 0;
}
