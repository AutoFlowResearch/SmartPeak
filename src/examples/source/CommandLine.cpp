#include <SmartPeak/core/Filenames.h>
#include <SmartPeak/io/InputDataValidation.h>
#include <algorithm>
#include <iostream>
#include <cstdlib>
#include <regex>
#include <fstream>

// This will be updated during runtime: an absolute path will be prefixed to it.
std::string PATHNAMES = "pathnames.txt";

using namespace SmartPeak;

Filenames getStaticFilenamesInput(const std::string& sequence_pathname);
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
  const size_t buf_size = 1048576;
  char line[buf_size];
  std::smatch match;
  while (stream.getline(line, buf_size)) {
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
