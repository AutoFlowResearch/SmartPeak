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
  struct Command {
    // enum Function {
    //   selectStaticInputDirectory,
    //   addRawDataMethod, // also selects dynamic filenames
    //   addSequenceSegmentMethod, // also selects dynamic filenames
    //   enableSequenceSummary,
    //   enableFeatureSummary,
    // };
    enum CommandType { RawDataMethod, SequenceSegmentMethod } type;
    std::vector<int> methods;
    std::map<std::string, Filenames> dynamic_filenames;
  };

  // This will be updated during runtime: an absolute path will be prefixed to it.
  std::string pathnamesTxtPath_     = "pathnames.txt";
  std::string sequence_pathname_    = "";
  std::string static_input_dir_     = "";
  bool        storeSequenceSummary_ = false;
  bool        storeFeatureSummary_  = false;
  MetaDataHandler::SampleType sequenceSummaryType = MetaDataHandler::Unknown;
  MetaDataHandler::SampleType featureSummaryType = MetaDataHandler::Unknown;
  std::vector<Command> commands_;
  Filenames static_filenames_;

  void buildStaticFilenames()
  {
    Filenames& f = static_filenames_;
    const std::string directory = fs::path(sequence_pathname_).parent_path().string();
    f = Filenames::getDefaultStaticFilenames(directory);
    f.sequence_csv_i = sequence_pathname_;

    pathnamesTxtPath_ = directory + "/" + std::string(pathnamesTxtPath_);

    if (InputDataValidation::fileExists(pathnamesTxtPath_)) {
      std::cout << "\n\n" <<
        "File " << pathnamesTxtPath_ << " was found in the directory. This file contains information about where the various experiment's files are found.\n\n" <<
        "Should its values be used to search for pathnames? [y]/n\n";
      const std::string in = getLineInput("> ");
      if (in.empty() || in.front() == 'y') {
        updateFilenames(f, pathnamesTxtPath_);
        std::cout << "Values in " << pathnamesTxtPath_ << ": USED\n";
      } else {
        std::cout << "Values in " << pathnamesTxtPath_ << ": IGNORED\n";
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
      generatePathnamesTxt(pathnamesTxtPath_, f, is_valid);
      std::cout << "\n\nOne or more files were not found.\n" <<
        "The file " << pathnamesTxtPath_ <<
        " has been generated for you to fix pathnames.\n" <<
        "The incorrect information has been replaced with an empty value.\n" <<
        "If you want a pathname to be ignored, then remove its value and leave only the label.\n" <<
        "Make sure that the pathnames are correct and run the application again.\n";
      std::exit(EXIT_FAILURE);
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
      "sequence:"                 << getPathnameOrPlaceholder(f.sequence_csv_i, *it++) <<
      "parameters:"               << getPathnameOrPlaceholder(f.parameters_csv_i, *it++) <<
      "traml:"                    << getPathnameOrPlaceholder(f.traML_csv_i, *it++) <<
      "feature_filter_c:"         << getPathnameOrPlaceholder(f.featureFilterComponents_csv_i, *it++) <<
      "feature_filter_c_groups:"  << getPathnameOrPlaceholder(f.featureFilterComponentGroups_csv_i, *it++) <<
      "feature_qc_c:"             << getPathnameOrPlaceholder(f.featureQCComponents_csv_i, *it++) <<
      "feature_qc_c_groups:"      << getPathnameOrPlaceholder(f.featureQCComponentGroups_csv_i, *it++) <<
      "quantitation_methods:"     << getPathnameOrPlaceholder(f.quantitationMethods_csv_i, *it++) <<
      "standards_concentrations:" << getPathnameOrPlaceholder(f.standardsConcentrations_csv_i, *it++) <<
      "reference_data:"           << getPathnameOrPlaceholder(f.referenceData_csv_i, *it++);
  }

  std::string getPathnameOrPlaceholder(const std::string& pathname, const bool is_valid)
  {
    const std::string placeholder = "";
    return (is_valid ? pathname : placeholder) + "\n";
  }

  void updateFilenames(Filenames& f, const std::string& pathname)
  {
    std::ifstream stream(pathname);
    const std::regex re("([a-zA-Z_]+):([^\\s]*)");
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
        // std::cout << label << ":" << value << '\n';
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

  std::vector<RawDataProcessor::RawDataProcMethod> getRawDataProcMethodsInput()
  {
    const std::unordered_map<int, RawDataProcessor::RawDataProcMethod> n_to_method {
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
    std::vector<RawDataProcessor::RawDataProcMethod> methods;
    std::string line;
    std::istringstream iss;

  getRawDataProcMethodsInput_menu:
    methods.clear();

    std::cout << "\n\n\n" <<
      "Please select the sequence of methods you want to run.\n" <<
      "You can select the same method multiple times." <<
      "Separate selected methods with a space.\n" <<
      "Available methods are:\n" <<
      "[1] LOAD_RAW_DATA\n" <<
      "[2] LOAD_FEATURES\n" <<
      "[3] PICK_FEATURES\n" <<
      "[4] FILTER_FEATURES\n" <<
      "[5] SELECT_FEATURES\n" <<
      "[6] VALIDATE_FEATURES\n" <<
      "[7] QUANTIFY_FEATURES\n" <<
      "[8] CHECK_FEATURES\n" <<
      "[9] STORE_FEATURES\n" <<
      "[10] PLOT_FEATURES (not implemented)\n" <<
      "[11] SAVE_FEATURES\n" <<
      "[12] ANNOTATE_USED_FEATURES\n" <<
      "[13] CLEAR_FEATURE_HISTORY\n\n" <<
      "Select the methods (example: > 1 3 4 4 5 7 8 9) and press Enter:\n";

    do {
      line = getLineInput("> ");
    } while (line.empty());

    iss.str(line);

    for (int n; iss >> n;) {
      if (n < 1 || n > 13 || n == 10) {
        std::cout << "One or more options are not available, try again.\n";
        goto getRawDataProcMethodsInput_menu;
      }
      methods.push_back(n_to_method.at(n));
    }

    return methods;
  }

  void setSequencePathnameFromInput()
  {
    std::cout << "\n\nPlease provide the pathname for the sequence file.\n" <<
      "Example:\n" <<
      // "> /home/user/data/some_sequence_file.csv\n\n" <<
      "> /home/pasdom/SmartPeak2/src/examples/data/HPLC_UV_Standards/sequence.csv\n\n" <<
      "Enter the absolute pathname:\n";
    sequence_pathname_ = getLineInput("> ", false);
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

  MetaDataHandler::SampleType getSampleTypeInput()
  {
    std::cout << "Please select the sample type. Insert its index:\n" <<
      "[1] Unknown\n" <<
      "[2] Standard\n" <<
      "[3] QC\n" <<
      "[4] Blank\n" <<
      "[5] DoubleBlank\n" <<
      "[6] Solvent\n";
    while (true) {
      const std::string line = getLineInput("> ");
      switch (std::stoi(line)) {
        case 1:
          return MetaDataHandler::Unknown;
        case 2:
          return MetaDataHandler::Standard;
        case 3:
          return MetaDataHandler::QC;
        case 4:
          return MetaDataHandler::Blank;
        case 5:
          return MetaDataHandler::DoubleBlank;
        case 6:
          return MetaDataHandler::Solvent;
        default:
          std::cout << "Sample type is incorrect. Try again.\n";
      }
    }
  }

  CommandLine(int argc, char **argv)
  {
    if (argc == 2) {
      sequence_pathname_ = argv[1];
    } else if (fs::exists("sequence.csv")) {
      sequence_pathname_ = fs::current_path().string() + std::string("/sequence.csv");
    } else {
      setSequencePathnameFromInput();
    }

    buildStaticFilenames();

    const bool verbose_I = false;

    SequenceHandler sequenceHandler;
    SequenceProcessor::createSequence(sequenceHandler, static_filenames_, ",", verbose_I);

    while (true) {
      printMenu();
      const std::string line = getLineInput("> ", false);
      parseCommand(line);
    }

    executeCommands(commands_);


    std::cout << "\n\n" <<
      "Choose if summary files should be stored when computation is done.\n";
    const std::string answer1 = getLineInput("\nSequenceSummary.csv? [y]/n\n");
    MetaDataHandler::SampleType sampleType1;
    if (answer1.empty() || answer1.front() == 'y') {
      sampleType1 = getSampleTypeInput();
    }

    const std::string answer2 = getLineInput("\nFeatureSummary.csv? [y]/n\n");
    MetaDataHandler::SampleType sampleType2;
    if (answer2.empty() || answer2.front() == 'y') {
      sampleType2 = getSampleTypeInput();
    }

    SequenceProcessor::processSequence(
      sequenceHandler,
      dynamic_filenames,
      std::set<std::string>(),
      raw_data_processing_methods,
      verbose_I
    );

    if (answer1.empty() || answer1.front() == 'y') {
      const std::string pathname = directory + "/SequenceSummary.csv";
      SequenceParser::writeDataMatrixFromMetaValue(
        sequenceHandler,
        pathname,
        {"calculated_concentration"},
        {sampleType1}
      );
      std::cout << "SequenceSummary.csv file has been stored at: " << pathname << '\n';
    }

    if (answer2.empty() || answer2.front() == 'y') {
      const std::string pathname = directory + "/FeatureSummary.csv";
      SequenceParser::writeDataTableFromMetaValue(
        sequenceHandler,
        pathname,
        {
          "peak_apex_int", "total_width", "width_at_50", "tailing_factor",
          "asymmetry_factor", "baseline_delta_2_height", "points_across_baseline",
          "points_across_half_height", "logSN", "calculated_concentration",
          "QC_transition_message", "QC_transition_pass", "QC_transition_score",
          "QC_transition_group_message", "QC_transition_group_score"
        },
        {sampleType2}
      );
      std::cout << "FeatureSummary.csv file has been stored at: " << pathname << '\n';
    }
  }

  void printMenu()
  {
    std::cout << "\n\n\n" <<
      "SmartPeak main menu\n" <<
      "[1] Set sequence.csv pathname\t[\"" << sequence_pathname_ << "\"]\n" <<
      "[2] Add Raw Data Processing\t[" << countCommands(Command::RawDataMethod) << "]\n" <<
      "[3] Add Sequence Segment Processing\t[" << countCommands(Command::SequenceSegmentMethod) << "]\n" <<
      "[4] SequenceSummary.csv\t[" << (storeSequenceSummary_ ? "EN" : "DIS") << "ABLED]\n" <<
      "[5] FeatureSummary.csv\t[" << (storeFeatureSummary_ ? "EN" : "DIS") << "ABLED]\n";
      "[6] Run the pipeline and exit\n";
  }

  int countCommands(Command::CommandType type)
  {
    return std::count_if(commands_.cbegin(), commands_.cend(),
      [type](struct Command& cmd){ return cmd.type == type; });
  }

  void parseCommand(const std::string& line)
  {
    int opt;

    try {
      opt = std::stoi(line);
    } catch (const std::invalid_argument&) {
      std::cout << "Bad input. Try again.\n";
      return;
    }

    switch (opt) {
    case 1:
      setSequencePathnameFromInput();
      buildStaticFilenames();
      break;
    case 2:
    {
      const std::vector<RawDataProcessor::RawDataProcMethod> methods = getRawDataProcMethodsInput();
      Command cmd;
      cmd.type = Command::RawDataMethod;
      cmd.methods = methods;

      const std::string directory = fs::path(sequence_pathname_).parent_path().string();
      std::string mzML_dir = directory + "/mzML";
      std::cout << "Path for 'mzML' files is currently:\t" << mzML_dir << '\n';
      std::cout << "Enter an absolute pathname if you want to change it, or just press Enter if you accept it.\n";
      std::string path_input = getLineInput("> ");
      if (path_input.size()) {
        mzML_dir = path_input;
      }
      std::string features_in_dir = directory + "/features";
      std::cout << "Path for 'INPUT features' files is currently:\t" << features_in_dir << '\n';
      std::cout << "Enter an absolute pathname if you want to change it, or just press Enter if you accept it.\n";
      path_input = getLineInput("> ");
      if (path_input.size()) {
        features_in_dir = path_input;
      }
      const std::string features_out_dir = directory + "/features";
      std::cout << "Path for 'OUTPUT features' files is currently:\t" << features_out_dir << '\n';
      std::cout << "Enter an absolute pathname if you want to change it, or just press Enter if you accept it.\n";
      path_input = getLineInput("> ");
      if (path_input.size()) {
        features_out_dir = path_input;
      }

      std::cout << "\n\n" <<
        "Input .mzML files are searched in:\t" << mzML_dir << '\n' <<
        "Input .featureXML files are searched in:\t" << features_in_dir << '\n' <<
        "Output .featureXML files are stored in:\t" << features_out_dir << '\n';

      std::map<std::string, Filenames>& dynamic_filenames = cmd.dynamic_filenames;
      for (const SampleHandler& sample : sequenceHandler.getSequence()) {
        const std::string& key = sample.getMetaData().getInjectionName();
        dynamic_filenames[key] = Filenames::getDefaultDynamicFilenames(
          mzML_dir,
          features_in_dir,
          features_out_dir,
          sample.getMetaData().getSampleName(),
          key
        );
      }

      commands_.push_back(cmd);
      break;
    }
    case 3:
      break;
    case 4:
      storeSequenceSummary_ = !storeSequenceSummary_;
      break;
    case 5:
      storeFeatureSummary_ = !storeFeatureSummary_;
      break;
    case 6:
      // run pipeline
      std::exit(EXIT_SUCCESS);
    }
  }
};

int main(int argc, char **argv)
{
  CommandLine cli(argc, argv);
  return 0;
}
