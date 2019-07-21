#include <SmartPeak/core/AppState.h>
#include <SmartPeak/core/AppStateProcessor.h>
#include <SmartPeak/core/FeatureMetadata.h>
#include <SmartPeak/core/SampleType.h>
#include <SmartPeak/core/SequenceProcessor.h>
#include <SmartPeak/core/Utilities.h>
#include <SmartPeak/io/SequenceParser.h>
#include <plog/Log.h>
#include <plog/Appenders/ConsoleAppender.h>
#include <chrono>

using namespace SmartPeak;

AppState state;

std::string getLineInput(const std::string& message, const bool canBeEmpty = true);

std::vector<AppState::Command> getMethodsInput();

void setSequencePathnameFromInput();

std::string getPathnameFromInput();

std::set<SampleType> getSampleTypesInput();

std::vector<FeatureMetadata> getMetaDataInput(
  const std::string& title
);

void menuMain();

void menuFile();

void menuImportFile();

void menuEdit();

void menuView();

void menuActions();

void menuDataIntegrity();

void menuReport();

void menuQuickInfo();

void menuHelp();

void exitSmartPeak();

// Returns a string representation of the workflow steps
// i.e. 1 2 3 4 5 5 18
std::string getPipelineString();

void initializeDataDirs();

void initializeDataDir(
  const std::string& label,
  std::string& data_dir_member,
  const std::string& default_dir
);

std::string commandsString =
  "[" + std::to_string(AppState::OPT_LOAD_RAW_DATA) + "]  Load raw data\n"
  "[" + std::to_string(AppState::OPT_LOAD_FEATURES) + "]  Load features\n"
  "[" + std::to_string(AppState::OPT_PICK_FEATURES) + "]  Pick features\n"
  "[" + std::to_string(AppState::OPT_FILTER_FEATURES) + "]  Filter features\n"
  "[" + std::to_string(AppState::OPT_SELECT_FEATURES) + "]  Select features\n"
  "[" + std::to_string(AppState::OPT_VALIDATE_FEATURES) + "]  Validate features\n"
  "[" + std::to_string(AppState::OPT_QUANTIFY_FEATURES) + "]  Quantify features\n"
  "[" + std::to_string(AppState::OPT_CHECK_FEATURES) + "]  Check features\n"
  "[" + std::to_string(AppState::OPT_STORE_FEATURES) + "]  Store features\n"
  "[" + std::to_string(AppState::OPT_PLOT_FEATURES) + "] Plot features (not implemented)\n"
  "[" + std::to_string(AppState::OPT_MAP_CHROMATROGRAMS) + "] Map transitions to the raw data\n"
  "[" + std::to_string(AppState::OPT_ZERO_CHROMATOGRAM_BASELINE) + "] Zero the chromatogram baseline\n"
  "[" + std::to_string(AppState::OPT_EXTRACT_CHROMATOGRAM_WIDOWS) + "] Extract chromatogram windows\n"
  "[" + std::to_string(AppState::OPT_CALCULATE_CALIBRATION) + "] Calculate calibration\n"
  "[" + std::to_string(AppState::OPT_STORE_QUANTITATION_METHODS) + "] Store quantitation methods\n"
  "[" + std::to_string(AppState::OPT_LOAD_QUANTITATION_METHODS) + "] Load quantitation methods\n";

std::string mainMenuString =
  "\n"
  "Please insert the sequence of methods to run.\n"
  "You can choose the same method multiple times.\n"
  "Separate chosen methods with a space.\n\n"
  + commandsString +
  "[M]  Main menu\n\n"
  "Presets:\n"
  "LCMS MRM Unknowns: 1 11 3 7 8 5 9\n"
  "LCMS MRM Standards: 1 11 3 8 5 14 15 7 9\n"
  "HPLC UV Unknowns: 1 11 13 12 3 7 8 5 9\n"
  "HPLC UV Standards: 1 11 13 12 3 8 5 14 15 7 9\n"
  "GCMS SIM Unknowns: 1 11 13 12 3 7 8 5 9\n"
  "GCMS Full Scan Unknowns: 1 11 13 12 3 8 5 14 15 7 9\n"
  "LCMS MRM Validation - LP: 1 11 13 3 4 4 5 6 9\n"
  "LCMS MRM Validation - QMIP: 1 11 3 5 6 9\n";

std::string gettingStartedString =
  "Welcome to SmartPeak\n\n"

  "Load an existing session from a sequence file\n"
  "`File -> Load session from sequence`\n"
  "`Edit -> Workflow ->\"1 2\"` then `Actions->Run workflow` to load the raw data and associated features\n\n"

  "Get quick information about the current application state\n"
  "`Actions -> Quick Info`\n\n"

  "Check the integrity of loaded data\n"
  "`Actions -> Check data integrity`\n\n"

  "Run a workflow\n"
  "`Edit -> Workflow -> your_commands_here`\n"
  "`Actions -> Run workflow`\n\n"

  "Save workflow results\n"
  "`Edit -> Workflow`, input `9` to select `Store features`\n"
  "`Actions -> Run workflow`\n\n"

  "Report workflow results\n"
  "`Actions -> Report`\n\n"

  "See this introductory tutorial again\n"
  "`Help -> Getting started`\n";

std::string getLineInput(const std::string& message, const bool canBeEmpty)
{
  std::string line;
  do {
    if (message.size()) {
      std::cout << message;
    }
    std::getline(std::cin, line);
    line = Utilities::trimString(line);
  } while (!canBeEmpty && line.empty());
  LOGD << "Input line: " << line;
  return line;
}

std::vector<AppState::Command> getMethodsInput()
{
  std::vector<AppState::Command> methods;

  LOGN << mainMenuString;

  const std::string line = getLineInput("> ", false);

  if (line[0] == 'M' || line[0] == 'm') {
    return {};
  }

  BuildCommandsFromIds buildCommandsFromIds(state);
  methods = buildCommandsFromIds(line);

  return methods;
}

void setSequencePathnameFromInput()
{
  LOGN << "\n\nSet the sequence file pathname.\n";
  if (state.sequence_pathname_.size()) {
    LOGN << "\n\nCurrent: " << state.sequence_pathname_ << "\n\n";
  } else {
    LOGN << "\n\n"
      "Example:\n"
      // "> /home/user/data/some_sequence_file.csv\n\n"
      "> /home/pasdom/SmartPeak2/src/examples/data/HPLC_UV_Standards/sequence.csv\n\n";
  }
  LOGN << "\n\nEnter the absolute pathname:\n";
  while (true) {
    state.sequence_pathname_ = getLineInput("> ", false);
    if (InputDataValidation::fileExists(state.sequence_pathname_)) {
      break;
    }
    LOGE << "\n\nThe file does not exist. Try again.\n";
  }
  LOGI << "\n\nSequence pathname set to: " << state.sequence_pathname_;
}

std::string getPathnameFromInput()
{
  std::string pathname;
  LOGN << "Pathname: ";
  while (true) {
    pathname = getLineInput("> ", false);
    if (InputDataValidation::fileExists(pathname)) {
      break;
    }
    LOGE << "\n\nThe pathname is not correct. Try again.\n";
  }
  LOGD << "Pathname being returned: " << pathname;
  return pathname;
}

std::set<SampleType> getSampleTypesInput()
{
  LOGN << "\n\n"
    "Please select the sample types. Insert the indexes separated by a space:\n"
    "[1] Unknown\n"
    "[2] Standard\n"
    "[3] QC\n"
    "[4] Blank\n"
    "[5] DoubleBlank\n"
    "[6] Solvent\n"
    "[M] Main menu\n\n";

  std::string line;

  do {
    line = getLineInput("> ");
  } while (line.empty());

  if (line[0] == 'M' || line[0] == 'm') {
    return {};
  }

  std::istringstream iss {line};
  std::set<SampleType> sample_types;

  for (int n; iss >> n;) {
    if (n < 1 || n > 6) {
      LOGW << "Skipping: " << n << '\n';
      continue;
    }
    switch (n) {
      case 1:
        sample_types.insert(SampleType::Unknown);
        break;
      case 2:
        sample_types.insert(SampleType::Standard);
        break;
      case 3:
        sample_types.insert(SampleType::QC);
        break;
      case 4:
        sample_types.insert(SampleType::Blank);
        break;
      case 5:
        sample_types.insert(SampleType::DoubleBlank);
        break;
      case 6:
        sample_types.insert(SampleType::Solvent);
        break;
    }
  }

  return sample_types;
}

std::vector<FeatureMetadata> getMetaDataInput(
  const std::string& title
)
{
  LOGN << "\n\n" << title <<
    "Please select the metadata. Insert the indexes separated by a space:\n"
    "[1]  Asymmetry factor\n"
    "[2]  Baseline delta/height\n"
    "[3]  Calculated concentration\n"
    "[4]  Log(Signal/Noise)\n"
    "[5]  Peak apex intensity\n"
    "[6]  Peak area\n"
    "[7]  Points across baseline\n"
    "[8]  Points across half height\n"
    "[9]  QC transition pass\n"
    "[10] QC transition message\n"
    "[11] QC transition score\n"
    "[12] QC transition group pass\n"
    "[13] QC transition group message\n"
    "[14] QC transition group score\n"
    "[15] Tailing factor\n"
    "[16] Total width\n"
    "[17] Width at 50% peak's height\n"
    "[18] Retention time\n"
    "[19] Preset (all): 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18\n\n";

  std::string line;

  do {
    line = getLineInput("> ");
  } while (line.empty());

  std::istringstream iss;
  iss.str(line);
  std::vector<FeatureMetadata> metadata;

  for (int n; iss >> n;) {
    if (n < 1 || n > 19) {
      LOGW << "Skipping: " << n << '\n';
      continue;
    }
    switch (n) {
    case 1:
      metadata.push_back(FeatureMetadata::asymmetry_factor);
      break;
    case 2:
      metadata.push_back(FeatureMetadata::baseline_delta_to_height);
      break;
    case 3:
      metadata.push_back(FeatureMetadata::calculated_concentration);
      break;
    case 4:
      metadata.push_back(FeatureMetadata::log_signal_to_noise);
      break;
    case 5:
      metadata.push_back(FeatureMetadata::peak_apex_intensity);
      break;
    case 6:
      metadata.push_back(FeatureMetadata::peak_area); // "intensity" attribute of a subordinate feature
      // NOTE: there is also an "intensity" attribute at the feature level
      // which is the sum of all subordinate feature "intensities"
      break;
    case 7:
      metadata.push_back(FeatureMetadata::points_across_baseline);
      break;
    case 8:
      metadata.push_back(FeatureMetadata::points_across_half_height);
      break;
    case 9:
      metadata.push_back(FeatureMetadata::qc_transition_pass);
      break;
    case 10:
      metadata.push_back(FeatureMetadata::qc_transition_message);
      break;
    case 11:
      metadata.push_back(FeatureMetadata::qc_transition_score);
      break;
    case 12:
      metadata.push_back(FeatureMetadata::qc_transition_group_pass);
      break;
    case 13:
      metadata.push_back(FeatureMetadata::qc_transition_group_message);
      break;
    case 14:
      metadata.push_back(FeatureMetadata::qc_transition_group_score);
      break;
    case 15:
      metadata.push_back(FeatureMetadata::tailing_factor);
      break;
    case 16:
      metadata.push_back(FeatureMetadata::total_width);
      break;
    case 17:
      metadata.push_back(FeatureMetadata::width_at_50_peak_height);
      break;
    case 18:
      metadata.push_back(FeatureMetadata::retention_time);
      break;
    case 19:
      metadata = {
        FeatureMetadata::asymmetry_factor,
        FeatureMetadata::baseline_delta_to_height,
        FeatureMetadata::calculated_concentration,
        FeatureMetadata::log_signal_to_noise,
        FeatureMetadata::peak_apex_intensity,
        FeatureMetadata::peak_area,
        FeatureMetadata::points_across_baseline,
        FeatureMetadata::points_across_half_height,
        FeatureMetadata::qc_transition_pass,
        FeatureMetadata::qc_transition_message,
        FeatureMetadata::qc_transition_score,
        FeatureMetadata::qc_transition_group_pass,
        FeatureMetadata::qc_transition_group_message,
        FeatureMetadata::qc_transition_group_score,
        FeatureMetadata::tailing_factor,
        FeatureMetadata::total_width,
        FeatureMetadata::width_at_50_peak_height,
        FeatureMetadata::retention_time
      };
      break;
    }
  }
  return metadata;
}

void menuMain()
{
  LOGN <<
    "\n\n"
    "Main\n"
    "[1] File\n"
    "[2] Edit\n"
    "[3] View\n"
    "[4] Actions\n"
    "[5] Help\n\n";

  std::string in;
menuMain_label:
  in = getLineInput("> ", false);

  if      ("1" == in) {
    menuFile();
  }
  else if ("2" == in) {
    menuEdit();
  }
  else if ("3" == in) {
    menuView();
  }
  else if ("4" == in) {
    menuActions();
  }
  else if ("5" == in) {
    menuHelp();
  }
  else {
    goto menuMain_label;
  }
}

void menuFile()
{
  LOGN <<
    "\n\n"
    "Main > File\n"
    "[1] Load session from sequence\n"
    "[2] Import file\n"
    "[M] Main menu\n"
    "[E] Exit\n\n";

  std::string in;
menuFile_label:
  in = getLineInput("> ", false);

  if ("1" == in) {
    setSequencePathnameFromInput();
    LoadSessionFromSequence processor(state);
    processor(state.sequence_pathname_.c_str());
  }
  else if ("2" == in) {
    menuImportFile();
  }
  else if ("m" == in || "M" == in) {
    // empty
  }
  else if ("e" == in || "E" == in) {
    exitSmartPeak();
  }
  else {
    goto menuFile_label;
  }
}

void menuImportFile()
{
  LOGN <<
    "\n\n"
    "Main > File > Import file\n"
    "[1] Sequence\n"
    "[2] TraML\n"
    "[3] Quantitation methods\n"
    "[4] Reference data\n"
    "[5] Standards concentrations\n"
    "[6] Component filters\n"
    "[7] Component group filters\n"
    "[8] Component QCs\n"
    "[9] Component group QCs\n"
    "[10] Parameters\n"
    "[M] Main menu\n\n";

  std::string in;
menuImportFile_label:
  in = getLineInput("> ", false);

  if      ("1" == in) {
    setSequencePathnameFromInput();
    state.static_filenames_.sequence_csv_i = state.sequence_pathname_;
    state.sequenceHandler_.clear();
    LOGI << "Sequence handler has been cleared";
    SequenceParser::readSequenceFile(state.sequenceHandler_, state.static_filenames_.sequence_csv_i, ",");
  }
  else if ("2" == in) {
    const std::string pathname = getPathnameFromInput();
    state.static_filenames_.traML_csv_i = pathname;
    LoadTransitions loadTransitions;
    loadTransitions.process(state.sequenceHandler_.getSequence()[0].getRawData(), {}, state.static_filenames_);
  }
  else if ("3" == in) {
    const std::string pathname = getPathnameFromInput();
    state.static_filenames_.quantitationMethods_csv_i = pathname;
    for (SequenceSegmentHandler& sequenceSegmentHandler : state.sequenceHandler_.getSequenceSegments()) {
      LoadQuantitationMethods loadQuantitationMethods;
      loadQuantitationMethods.process(sequenceSegmentHandler, SequenceHandler(), {}, state.static_filenames_);
    }
  }
  else if ("4" == in) {
    const std::string pathname = getPathnameFromInput();
    state.static_filenames_.referenceData_csv_i = pathname;
    LoadValidationData loadValidationData;
    loadValidationData.process(state.sequenceHandler_.getSequence()[0].getRawData(), {}, state.static_filenames_);
  }
  else if ("5" == in) {
    const std::string pathname = getPathnameFromInput();
    state.static_filenames_.standardsConcentrations_csv_i = pathname;
    for (SequenceSegmentHandler& sequenceSegmentHandler : state.sequenceHandler_.getSequenceSegments()) {
      LoadStandardsConcentrations loadStandardsConcentrations;
      loadStandardsConcentrations.process(sequenceSegmentHandler, SequenceHandler(), {}, state.static_filenames_);
    }
  }
  else if ("6" == in) {
    const std::string pathname = getPathnameFromInput();
    state.static_filenames_.featureFilterComponents_csv_i = pathname;
    LoadFeatureFilters loadFeatureFilters;
    const std::string backup = state.static_filenames_.featureFilterComponentGroups_csv_i;
    state.static_filenames_.featureFilterComponentGroups_csv_i.clear();
    loadFeatureFilters.process(state.sequenceHandler_.getSequence()[0].getRawData(), {}, state.static_filenames_);
    state.static_filenames_.featureFilterComponentGroups_csv_i = backup;
  }
  else if ("7" == in) {
    const std::string pathname = getPathnameFromInput();
    state.static_filenames_.featureFilterComponentGroups_csv_i = pathname;
    LoadFeatureFilters loadFeatureFilters;
    const std::string backup = state.static_filenames_.featureFilterComponents_csv_i;
    state.static_filenames_.featureFilterComponents_csv_i.clear();
    loadFeatureFilters.process(state.sequenceHandler_.getSequence()[0].getRawData(), {}, state.static_filenames_);
    state.static_filenames_.featureFilterComponents_csv_i = backup;
  }
  else if ("8" == in) {
    const std::string pathname = getPathnameFromInput();
    state.static_filenames_.featureQCComponents_csv_i = pathname;
    LoadFeatureQCs loadFeatureQCs;
    const std::string backup = state.static_filenames_.featureQCComponentGroups_csv_i;
    state.static_filenames_.featureQCComponentGroups_csv_i.clear();
    loadFeatureQCs.process(state.sequenceHandler_.getSequence()[0].getRawData(), {}, state.static_filenames_);
    state.static_filenames_.featureQCComponentGroups_csv_i = backup;
  }
  else if ("9" == in) {
    const std::string pathname = getPathnameFromInput();
    state.static_filenames_.featureQCComponentGroups_csv_i = pathname;
    LoadFeatureQCs loadFeatureQCs;
    const std::string backup = state.static_filenames_.featureQCComponents_csv_i;
    state.static_filenames_.featureQCComponents_csv_i.clear();
    loadFeatureQCs.process(state.sequenceHandler_.getSequence()[0].getRawData(), {}, state.static_filenames_);
    state.static_filenames_.featureQCComponents_csv_i = backup;
  }
  else if ("10" == in) {
    const std::string pathname = getPathnameFromInput();
    state.static_filenames_.parameters_csv_i = pathname;
    LoadParameters loadParameters;
    loadParameters.process(state.sequenceHandler_.getSequence()[0].getRawData(), {}, state.static_filenames_);
  }
  else if ("m" == in || "M" == in) {
    // empty
  }
  else {
    goto menuImportFile_label;
  }
}

void menuEdit()
{
  LOGN <<
    "\n\n"
    "Main > Edit\n"
    "[1] Workflow\n"
    "[M] Main menu\n\n";

  std::string in;
menuEdit_label:
  in = getLineInput("> ", false);

  if ("1" == in) {
    initializeDataDirs();
    const std::vector<AppState::Command> methods = getMethodsInput();
    if (methods.empty()) {
      LOGW << "\n\nPipeline not modified";
    } else {
      state.commands_ = methods;
    }
  }
  else if ("m" == in || "M" == in) {
    // empty
  }
  else {
    goto menuEdit_label;
  }
}

void menuView()
{
  LOGN <<
    "\n\n"
    "Main > View\n"
    "[1] Workflow wizard\n"
    "[M] Main menu\n\n";

  std::string in;
menuView_label:
  in = getLineInput("> ", false);

  if ("1" == in) {
    LOGN << "\n\n" << getPipelineString();
  }
  else if ("m" == in || "M" == in) {
    // empty
  }
  else {
    goto menuView_label;
  }
}

void menuActions()
{
  LOGN <<
    "\n\n"
    "Main > Actions\n"
    "[1] Run command\n"
    "[2] Run workflow\n"
    "[3] Quick info\n"
    "[4] Check data integrity\n"
    "[5] Report\n"
    "[M] Main menu\n\n";

  std::string in;
menuActions_label:
  in = getLineInput("> ", false);

  if      ("1" == in) {
    initializeDataDirs();

    LOGN << "\n\n" << commandsString;

    const std::string input = getLineInput("> ");
    try {
      const int n = std::stoi(input);
      AppState::Command cmd;
      CreateCommand createCommand(state);
      if (createCommand(n, cmd)) {
        ProcessCommands processCommands(state);
        processCommands({cmd});
      }
    } catch (...) {
      LOGE << "\n\nInvalid input: cannot convert to integer.\n";
    }
  }
  else if ("2" == in) {
    initializeDataDirs();
    ProcessCommands processCommands(state);
    processCommands(state.commands_);
    LOGN << "\n\nWorkflow completed.\n";
  }
  else if ("3" == in) {
    menuQuickInfo();
  }
  else if ("4" == in) {
    menuDataIntegrity();
  }
  else if ("5" == in) {
    menuReport();
  }
  else if ("m" == in || "M" == in) {
    // empty
  }
  else {
    goto menuActions_label;
  }
}

void menuDataIntegrity()
{
  LOGN <<
    "\n\n"
    "Main > Actions > Check data integrity\n"
    "[1] Sample names\n"
    "[2] Component names\n"
    "[3] Component groups names\n"
    "[4] Heavy components\n"
    "[M] Main menu\n\n";

  std::string in;
menuDataIntegrity_label:
  in = getLineInput("> ", false);

  if      ("1" == in) {
    InputDataValidation::sampleNamesAreConsistent(state.sequenceHandler_);
  }
  else if ("2" == in) {
    InputDataValidation::componentNamesAreConsistent(state.sequenceHandler_);
  }
  else if ("3" == in) {
    InputDataValidation::componentNameGroupsAreConsistent(state.sequenceHandler_);
  }
  else if ("4" == in) {
    InputDataValidation::heavyComponentsAreConsistent(state.sequenceHandler_);
  }
  else if ("m" == in || "M" == in) {
    // empty
  }
  else {
    goto menuDataIntegrity_label;
  }
}

void menuReport()
{
  LOGN <<
    "\n\n"
    "Main > Actions > Report\n"
    "[1] Feature summary\n"
    "[2] Sequence summary\n"
    "[M] Main menu\n\n";

  std::string in;
menuReport_label:
  in = getLineInput("> ", false);

  if ("1" == in) {
    const std::vector<FeatureMetadata> summaryMetaData = getMetaDataInput("\nMain > Actions > Report > Feature summary\n");
    const std::set<SampleType> summarySampleTypes = getSampleTypesInput();
    const std::string pathname = state.main_dir_ + "/FeatureDB.csv";
    const bool data_was_written = SequenceParser::writeDataTableFromMetaValue(
      state.sequenceHandler_,
      pathname,
      summaryMetaData,
      summarySampleTypes
    );
    if (data_was_written) {
      LOGN << "\n\nFeatureDB.csv file has been stored at: " << pathname << '\n';
    } else {
      LOGE << "\n\nError during write. FeatureDB.csv content is invalid.\n";
    }
  }
  else if ("2" == in) {
    const std::vector<FeatureMetadata> summaryMetaData = getMetaDataInput("\nMain > Actions > Report > Sequence summary\n");
    const std::set<SampleType> summarySampleTypes = getSampleTypesInput();
    const std::string pathname = state.main_dir_ + "/PivotTable.csv";
    const bool data_was_written = SequenceParser::writeDataMatrixFromMetaValue(
      state.sequenceHandler_,
      pathname,
      summaryMetaData,
      summarySampleTypes
    );
    if (data_was_written) {
      LOGN << "\n\nPivotTable.csv file has been stored at: " << pathname << '\n';
    } else {
      LOGE << "\n\nError during write. FeatureDB.csv content is invalid.\n";
    }
  }
  else if ("m" == in || "M" == in) {
    // empty
  }
  else {
    goto menuReport_label;
  }
}

void menuQuickInfo()
{
  LOGN <<
    "\n\n"
    "Main > Actions > Quick Info\n"
    "[1] Sequence\n"
    "[2] TraML\n"
    "[3] Quantitation methods\n"
    "[4] Standards concentrations\n"
    "[5] Feature filters\n"
    "[6] Feature QC\n"
    "[7] Parameters\n"
    "[8] Raw data files\n"
    "[9] Analyzed features\n"
    "[10] Selected features\n"
    "[11] Picked peaks\n"
    "[12] Filtered/selected peaks\n"
    "[M] Main menu\n\n";

  std::string in;
menuQuickInfo_label:
  in = getLineInput("> ", false);

  if      ("1" == in) {
    LOGN << "\n\n" << InputDataValidation::getSequenceInfo(state.sequenceHandler_) << "\n";
  }
  else if ("2" == in) {
    LOGN << "\n\n" << InputDataValidation::getTraMLInfo(
      state.sequenceHandler_.getSequence().front().getRawData()) << "\n";
  }
  else if ("3" == in) {
    LOGN << "\n\n" << InputDataValidation::getQuantitationMethodsInfo(
      state.sequenceHandler_.getSequenceSegments().front()) << "\n";
  }
  else if ("4" == in) {
    LOGN << "\n\n" << InputDataValidation::getStandardsConcentrationsInfo(
      state.sequenceHandler_.getSequenceSegments().front()) << "\n";
  }
  else if ("5" == in) {
    LOGN << "\n\n" << InputDataValidation::getComponentsAndGroupsInfo(
      state.sequenceHandler_.getSequence().front().getRawData(), true) << "\n";
  }
  else if ("6" == in) {
    LOGN << "\n\n" << InputDataValidation::getComponentsAndGroupsInfo(
      state.sequenceHandler_.getSequence().front().getRawData(), false) << "\n";
  }
  else if ("7" == in) {
    LOGN << "\n\n" << InputDataValidation::getParametersInfo(
      state.sequenceHandler_.getSequence().front().getRawData().getParameters()) << "\n";
  }
  else if ("8" == in) {
    LOGN << "\n\n" << state.sequenceHandler_.getRawDataFilesInfo() << "\n";
  }
  else if ("9" == in) {
    LOGN << "\n\n" << state.sequenceHandler_.getAnalyzedFeaturesInfo() << "\n";
  }
  else if ("10" == in) {
    LOGN << "\n\n" << state.sequenceHandler_.getSelectedFeaturesInfo() << "\n";
  }
  else if ("11" == in) {
    LOGN << "\n\n" << state.sequenceHandler_.getPickedPeaksInfo() << "\n";
  }
  else if ("12" == in) {
    LOGN << "\n\n" << state.sequenceHandler_.getFilteredSelectedPeaksInfo() << "\n";
  }
  else if ("m" == in || "M" == in) {
    // empty
  }
  else {
    goto menuQuickInfo_label;
  }
}

void menuHelp()
{
  LOGN <<
    "\n\n"
    "Main > Help\n"
    "[1] Getting started\n"
    "[M] Main menu\n\n";

  std::string in;
menuHelp_label:
  in = getLineInput("> ", false);

  if ("1" == in) {
    LOGN << "\n\n" << gettingStartedString;
  }
  else if ("m" == in || "M" == in) {
    // empty
  }
  else {
    goto menuHelp_label;
  }
}

void exitSmartPeak()
{
  const std::string in = getLineInput("\nExit SmartPeak? [y/N]\n> ");
  if (in.size() && std::tolower(in.front()) == 'y') {
    std::exit(EXIT_SUCCESS);
  }
}

// Returns a string representation of the workflow steps
// i.e. 1 2 3 4 5 5 18
std::string getPipelineString()
{
  std::string s;
  for (const AppState::Command& cmd : state.commands_) {
    if (cmd.type == AppState::Command::RawDataMethod) {
      const std::map<int, std::shared_ptr<RawDataProcessor>>::const_iterator
      it = std::find_if(
        state.n_to_raw_data_method_.cbegin(),
        state.n_to_raw_data_method_.cend(),
        [&cmd](const std::pair<int, std::shared_ptr<RawDataProcessor>>& p)
          { return p.second == cmd.raw_data_method; }
      );
      s.append(std::to_string(it->first));
    } else if (cmd.type == AppState::Command::SequenceSegmentMethod) {
      const std::map<int, std::shared_ptr<SequenceSegmentProcessor>>::const_iterator
      it = std::find_if(
        state.n_to_seq_seg_method_.cbegin(),
        state.n_to_seq_seg_method_.cend(),
        [&cmd](const std::pair<int, std::shared_ptr<SequenceSegmentProcessor>>& p)
          { return p.second == cmd.seq_seg_method; }
      );
      s.append(std::to_string(it->first));
    } else {
      throw "\nunsupported step\n";
    }
    s.append(" ");
  }
  if (s.size()) {
    s.pop_back();
  }
  return s;
}

void initializeDataDirs()
{
  initializeDataDir("mzML", state.mzML_dir_, "mzML");
  initializeDataDir("INPUT features", state.features_in_dir_, "features");
  initializeDataDir("OUTPUT features", state.features_out_dir_, "features");
}

void initializeDataDir(
  const std::string& label,
  std::string& data_dir_member,
  const std::string& default_dir
)
{
  if (data_dir_member.size()) {
    return;
  }
  data_dir_member = state.main_dir_ + "/" + default_dir;
  LOGN << "\n\nGenerated path for '" << label << "':\t" << data_dir_member;
  LOGN << "\n\nEnter an absolute pathname to change it, or press Enter to confirm.\n";
  const std::string path_input = getLineInput("> ");
  if (path_input.size()) {
    data_dir_member = path_input;
  }
}

int main()
{
  const std::time_t t = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
  char filename[128];
  strftime(filename, 128, "smartpeak_log_%Y-%m-%d_%H-%M-%S.csv", std::localtime(&t));

  // Add .csv appender: 32 MiB per file, max. 100 log files
  static plog::RollingFileAppender<plog::CsvFormatter>
    fileAppender(filename, 1024 * 1024 * 32, 100);

  // Inspired by plog::TxtFormatter
  class ConsoleFormatter
  {
  public:
    static plog::util::nstring header()
    {
      return plog::util::nstring();
    }

    static plog::util::nstring format(const plog::Record& record)
    {
      if (record.getSeverity() > plog::info) {
        return plog::util::nstring();
      }

      std::tm t;
      (plog::util::localtime_s)(&t, &record.getTime().time);

      plog::util::nostringstream ss;
      ss << t.tm_year + 1900 << "-" << std::setfill(PLOG_NSTR('0')) << std::setw(2) << t.tm_mon + 1 << PLOG_NSTR("-") << std::setfill(PLOG_NSTR('0')) << std::setw(2) << t.tm_mday << PLOG_NSTR(" ");
      ss << std::setfill(PLOG_NSTR('0')) << std::setw(2) << t.tm_hour << PLOG_NSTR(":") << std::setfill(PLOG_NSTR('0')) << std::setw(2) << t.tm_min << PLOG_NSTR(":") << std::setfill(PLOG_NSTR('0')) << std::setw(2) << t.tm_sec << PLOG_NSTR(" ");
      // ss << std::setfill(PLOG_NSTR(' ')) << std::setw(5) << std::left << severityToString(record.getSeverity()) << PLOG_NSTR(" ");
      // ss << PLOG_NSTR("[") << record.getTid() << PLOG_NSTR("] ");
      ss << record.getMessage() << PLOG_NSTR("\n");

      return ss.str();
    }
  };

  // Add console appender, instead of only the file one
  static plog::ConsoleAppender<ConsoleFormatter> consoleAppender;

  // Init logger with two appenders
  plog::init(plog::debug, &fileAppender).addAppender(&consoleAppender);

  LOGN << "Log file at: " << filename;

  LOGN << "\n\n" << gettingStartedString;
  while (true) {
    menuMain();
  }

  return 0;
}
