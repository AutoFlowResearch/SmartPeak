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

// This will be updated during runtime: an absolute path will be prefixed to it.
std::string PATHNAMES = "pathnames.txt";
const size_t BUF_SIZE = 1048576;

using namespace SmartPeak;

Filenames getStaticFilenamesInput(const std::string& sequence_pathname);
std::vector<RawDataProcessor::RawDataProcMethod> getRawDataProcMethodsInput();
void generatePathnamesTxt(const std::string& pathname, const Filenames& f, const std::vector<bool>& is_valid);
std::string getPathnameOrPlaceholder(const std::string& pathname, const bool is_valid);
void updateFilenames(Filenames& f, const std::string& pathname);
std::string getDirectoryFromAbsolutePathname(const std::string& pathname);

int main(int argc, char **argv)
{
  std::string sequence_pathname;
  if (argc == 2) {
    sequence_pathname = argv[1];
  } else {
    std::cout << "\n\nPlease provide the pathname for the sequence file.\n"
      << "Example:\n"
      // << "> /home/user/data/some_sequence_file.csv\n\n"
      << "> /home/pasdom/SmartPeak2/src/examples/data/HPLC_UV_Standards/sequence.csv\n\n"
      << "Enter the absolute pathname:\n"
      << "> ";
    std::cin >> sequence_pathname;
  }
  const Filenames static_filenames = getStaticFilenamesInput(sequence_pathname);

  const bool verbose_I = true;

  SequenceHandler sequenceHandler;
  SequenceProcessor::createSequence(sequenceHandler, static_filenames, ",", verbose_I);

  const std::vector<RawDataProcessor::RawDataProcMethod> raw_data_processing_methods =
    getRawDataProcMethodsInput();

  const std::string directory = getDirectoryFromAbsolutePathname(sequence_pathname);
  const std::string mzML_dir = directory + "/mzML";
  const std::string features_in_dir = directory + "/features";
  const std::string features_out_dir = directory + "/features";

  std::cout << "Input .mzML files are searched in: " << mzML_dir;
  std::cout << "Input .featureXML files are searched in: " << features_in_dir;
  std::cout << "Output .featureXML files are stored in: " << features_out_dir;

  std::map<std::string, Filenames> dynamic_filenames;
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

  SequenceProcessor::processSequence(
    sequenceHandler,
    dynamic_filenames,
    std::vector<std::string>(),
    raw_data_processing_methods,
    verbose_I
  );

  SequenceParser::writeDataMatrixFromMetaValue(
    sequenceHandler,
    static_filenames.sequenceSummary_csv_o,
    {"calculated_concentration"},
    {MetaDataHandler::SampleType::Unknown}
  );

  SequenceParser::writeDataTableFromMetaValue(
    sequenceHandler,
    static_filenames.featureSummary_csv_o,
    {
      "peak_apex_int", "total_width", "width_at_50", "tailing_factor",
      "asymmetry_factor", "baseline_delta_2_height", "points_across_baseline",
      "points_across_half_height", "logSN", "calculated_concentration",
      "QC_transition_message", "QC_transition_pass", "QC_transition_score",
      "QC_transition_group_message", "QC_transition_group_score"
    },
    {MetaDataHandler::SampleType::Unknown}
  );
}

Filenames getStaticFilenamesInput(const std::string& sequence_pathname)
{
  const std::string directory = getDirectoryFromAbsolutePathname(sequence_pathname);
  Filenames f = Filenames::getDefaultStaticFilenames(directory);
  f.sequence_csv_i = sequence_pathname;

  PATHNAMES = directory + "/" + std::string(PATHNAMES);

  if (InputDataValidation::fileExists(PATHNAMES)) {
    std::cout << "File " << PATHNAMES << " was found.\n" <<
      "Its values will be used to search for pathnames.\n\n";
    updateFilenames(f, PATHNAMES);
  }

  std::cout << "The following list of file was searched for:\n";
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
    generatePathnamesTxt(PATHNAMES, f, is_valid);
    std::cout << "\n\nOne or more files were not found.\n" <<
      "The file " << PATHNAMES <<
      " has been generated for you to fix pathnames.\n" <<
      "The incorrect information has been replaced with an empty value.\n" <<
      "If you want a pathname to be ignored, then remove its value and leave only the label.\n" <<
      "Make sure that the pathnames are correct and run the application again.\n";
    std::exit(EXIT_FAILURE);
  }

  return f;
}

void generatePathnamesTxt(const std::string& pathname, const Filenames& f, const std::vector<bool>& is_valid)
{
  std::ofstream ofs(pathname);
  if (!ofs.is_open()) {
    std::cout << "\n\nCannot open " << pathname << "\n";
    std::exit(EXIT_FAILURE);
  }
  std::vector<bool>::const_iterator it = is_valid.cbegin();
  ofs << "sequence:"                  << getPathnameOrPlaceholder(f.sequence_csv_i, *it++) <<
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
  char line[BUF_SIZE];
  std::smatch match;
  while (stream.getline(line, BUF_SIZE)) {
    // std::regex_match cannot work with temporary objects
    // https://stackoverflow.com/questions/32164501/error-use-of-deleted-function-bool-regex-match-with-gcc-5-2-0
    // Hence "string_line" is used
    const std::string string_line = line;
    const bool matched = std::regex_match(string_line, match, re);
    if (matched == false) {
      std::cout << "\n\nregex did not match with the extracted line: " << line << "\n" <<
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
      std::cout << "\n\nlabel is not valid: " << label << "\n";
      std::exit(EXIT_FAILURE);
    }
  }
}

// It doesn't return the last slash
// It is assumed that the file is not found in the root directory /
std::string getDirectoryFromAbsolutePathname(const std::string& pathname)
{
  return pathname.substr(0, pathname.find_last_of("/"));
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
  char line_array[BUF_SIZE];
  std::istringstream iss;

print_methods_menu:
  methods.clear();
  std::cout << "\n\n\n" <<
    "Please select the sequence of methods you want to run.\n" <<
    "You can select the same method multiple times." <<
    "Separate selected methods with a space.\n" <<
    "You can also select a preset, instead of constructing a custom workflow.\n\n" <<
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
    "[10] PLOT_FEATURES\n" <<
    "[11] SAVE_FEATURES\n" <<
    "[12] ANNOTATE_USED_FEATURES\n" <<
    "[13] CLEAR_FEATURE_HISTORY\n\n" <<
    "[14] Preset: Unknowns\n" <<
    "[15] Preset: Validation\n" <<
    "[16] Preset: Standards (not implemented, yet)\n\n" <<
    "Select the methods (example: > 1 3 4 4 5 7 8 9) and press Enter:\n" <<
    "> ";
  std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
  std::cin.getline(line_array, BUF_SIZE);
  iss.str(line_array);
  for (int n; iss >> n;) {
    if (iss.fail()) {
      std::cout << "Unexpected data in input.\n";
      std::exit(EXIT_FAILURE);
    }
    if (n < 1 || n > 16) {
      std::cout << "One or more options are not available, try again.\n";
      goto print_methods_menu;
    }
    if (n >= 1 && n <= 13) {
      methods.push_back(n_to_method.at(n));
    } else {
      if (n == 14) {
        methods = {
          RawDataProcessor::LOAD_RAW_DATA,
          RawDataProcessor::PICK_FEATURES,
          RawDataProcessor::FILTER_FEATURES,
          RawDataProcessor::FILTER_FEATURES,
          RawDataProcessor::SELECT_FEATURES,
          RawDataProcessor::QUANTIFY_FEATURES,
          RawDataProcessor::CHECK_FEATURES,
          RawDataProcessor::STORE_FEATURES
        };
      } else if (n == 15) {
        methods = {
          RawDataProcessor::LOAD_RAW_DATA,
          RawDataProcessor::PICK_FEATURES,
          RawDataProcessor::FILTER_FEATURES,
          RawDataProcessor::SELECT_FEATURES,
          RawDataProcessor::VALIDATE_FEATURES
        };
      } else {
        methods = {
          RawDataProcessor::LOAD_RAW_DATA,
          RawDataProcessor::PICK_FEATURES,
          RawDataProcessor::FILTER_FEATURES,
          RawDataProcessor::FILTER_FEATURES,
          RawDataProcessor::SELECT_FEATURES,
          RawDataProcessor::CHECK_FEATURES,
          RawDataProcessor::STORE_FEATURES
        };
        throw "Standards workflow is not implemented, yet.\n";
      }
      break;
    }
  }
  return methods;
}
