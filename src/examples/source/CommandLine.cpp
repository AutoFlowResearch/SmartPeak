#include <SmartPeak/core/Filenames.h>
#include <SmartPeak/core/RawDataProcessor.h>
#include <SmartPeak/core/SequenceProcessor.h>
#include <SmartPeak/io/InputDataValidation.h>
#include <SmartPeak/io/SequenceParser.h>
#include <algorithm>
#include <chrono>
#include <cctype>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iostream>
#include <regex>
#include <unordered_map>
#include <plog/Log.h>
#include <plog/Appenders/ConsoleAppender.h>

#ifdef _WIN32
  // https://docs.microsoft.com/en-us/cpp/c-runtime-library/reference/getcwd-wgetcwd
  #include <direct.h>
  auto mygetcwd = &_getcwd;
#else
  // http://pubs.opengroup.org/onlinepubs/9699919799/functions/getcwd.html
  #include <unistd.h>
  auto mygetcwd = &getcwd;
#endif

using namespace SmartPeak;

class CommandLine final {
public:
  class Command {
  public:
    enum CommandType {
      RawDataMethod,
      SequenceSegmentMethod,
    } type;

    void setMethod(const std::shared_ptr<RawDataProcessor> method)
    {
      type = RawDataMethod;
      raw_data_method = method;
    }

    void setMethod(const std::shared_ptr<SequenceSegmentProcessor> method)
    {
      type = SequenceSegmentMethod;
      seq_seg_method = method;
    }

    std::shared_ptr<RawDataProcessor> raw_data_method;
    std::shared_ptr<SequenceSegmentProcessor> seq_seg_method;

    std::map<std::string, Filenames> dynamic_filenames;
  };

  const std::string                     pathnamesFilename_       = "pathnames.txt";
  std::string                           sequence_pathname_;
  std::string                           main_dir_                = ".";
  std::string                           mzML_dir_;
  std::string                           features_in_dir_;
  std::string                           features_out_dir_;
  std::set<MetaDataHandler::SampleType> sequenceSummaryTypes_;
  std::set<MetaDataHandler::SampleType> featureSummaryTypes_;
  std::vector<std::string>              sequenceSummaryMetaData_;
  std::vector<std::string>              featureSummaryMetaData_;
  std::vector<Command>                  commands_;
  Filenames                             static_filenames_;
  SequenceHandler                       sequenceHandler_;

  const std::unordered_map<int, std::shared_ptr<RawDataProcessor>> n_to_raw_data_method_ {
    {1, std::shared_ptr<RawDataProcessor>(new LoadRawData())},
    {2, std::shared_ptr<RawDataProcessor>(new LoadFeatures())},
    {3, std::shared_ptr<RawDataProcessor>(new PickFeatures())},
    {4, std::shared_ptr<RawDataProcessor>(new FilterFeatures())},
    {5, std::shared_ptr<RawDataProcessor>(new SelectFeatures())},
    {6, std::shared_ptr<RawDataProcessor>(new ValidateFeatures())},
    {7, std::shared_ptr<RawDataProcessor>(new QuantifyFeatures())},
    {8, std::shared_ptr<RawDataProcessor>(new CheckFeatures())},
    {9, std::shared_ptr<RawDataProcessor>(new StoreFeatures())},
    {10, std::shared_ptr<RawDataProcessor>(new PlotFeatures())},
    {11, std::shared_ptr<RawDataProcessor>(new MapChromatograms())},
    {12, std::shared_ptr<RawDataProcessor>(new ZeroChromatogramBaseline())},
    {13, std::shared_ptr<RawDataProcessor>(new ExtractChromatogramWindows())},
  };
  const std::unordered_map<int, std::shared_ptr<SequenceSegmentProcessor>> n_to_seq_seg_method_ {
    {14, std::shared_ptr<SequenceSegmentProcessor>(new CalculateCalibration())},
    {15, std::shared_ptr<SequenceSegmentProcessor>(new StoreQuantitationMethods())},
    {16, std::shared_ptr<SequenceSegmentProcessor>(new LoadQuantitationMethods())},
  };
  enum ProcOpt {
    OPT_LOAD_RAW_DATA = 1,
    OPT_LOAD_FEATURES,
    OPT_PICK_FEATURES,
    OPT_FILTER_FEATURES,
    OPT_SELECT_FEATURES,
    OPT_VALIDATE_FEATURES,
    OPT_QUANTIFY_FEATURES,
    OPT_CHECK_FEATURES,
    OPT_STORE_FEATURES,
    OPT_PLOT_FEATURES,
    OPT_MAP_CHROMATROGRAMS,
    OPT_ZERO_CHROMATOGRAM_BASELINE,
    OPT_EXTRACT_CHROMATOGRAM_WIDOWS,
    OPT_CALCULATE_CALIBRATION,
    OPT_STORE_QUANTITATION_METHODS,
    OPT_LOAD_QUANTITATION_METHODS,
  };
  const std::string main_menu_ = "\n\n"
    "Please insert the sequence of methods to run.\n"
    "You can choose the same method multiple times.\n"
    "Separate chosen methods with a space.\n\n"
    + commandsString() +
    "[M]  Main menu\n\n"
    "Presets:\n"
    "LCMS MRM Unknowns: 1 11 3 7 8 5 9\n"
    "LCMS MRM Standards: 1 11 3 4 4 5 14 15 7 8 9\n"
    "HPLC UV Unknowns: 1 11 13 12 3 7 8 5 9\n"
    "HPLC UV Standards: 1 11 13 12 3 8 5 14 15 7 9\n"
    "GCMS SIM Unknowns: 1 11 13 3 7 8 5 9\n"
    "LCMS MRM Validation: 1 11 3 4 5 6\n\n";

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
      "[1] New session\n"
      "[2] Load session\n"
      "[3] Load session from sequence\n"
      "[4] Save session\n"
      "[5] Import file\n"
      "[6] Export file\n"
      "[M] Main menu\n"
      "[E] Exit\n\n";

    std::string in;
  menuFile_label:
    in = getLineInput("> ", false);

    if      ("1" == in) {
    }
    else if ("2" == in) {
    }
    else if ("3" == in) {
      setSequencePathnameFromInput();
      mzML_dir_.clear();
      features_in_dir_.clear();
      features_out_dir_.clear();
      LOGI << "Pathnames for 'mzML', 'INPUT features' and 'OUTPUT features' reset.";
      const bool pathnamesAreCorrect = buildStaticFilenames();
      if (pathnamesAreCorrect) {
        sequenceHandler_.clear();
        SequenceProcessor::createSequence(sequenceHandler_, static_filenames_, ",", true);
      } else {
        LOGE << "Provided and/or inferred pathnames are not correct."
          "The sequence has not been modified. Check file: " << pathnamesFilename_;
      }
    }
    else if ("4" == in) {
    }
    else if ("5" == in) {
      menuImportFile();
    }
    else if ("6" == in) {
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
      "[4] Standards concentrations\n"
      "[5] Component filters\n"
      "[6] Component group filters\n"
      "[7] Component QCs\n"
      "[8] Component group QCs\n"
      "[9] Parameters\n"
      "[M] Main menu\n\n";

    std::string in;
  menuImportFile_label:
    in = getLineInput("> ", false);

    if      ("1" == in) {
      setSequencePathnameFromInput();
      static_filenames_.sequence_csv_i = sequence_pathname_;
      sequenceHandler_.clear();
      LOGI << "Sequence handler has been cleared";
      SequenceParser::readSequenceFile(sequenceHandler_, static_filenames_.sequence_csv_i, ",");
    }
    else if ("2" == in) {
      const std::string pathname = getPathnameFromInput();
      static_filenames_.traML_csv_i = pathname;
      LoadTransitions loadTransitions;
      loadTransitions.process(sequenceHandler_.getSequence()[0].getRawData(), {}, static_filenames_);
    }
    else if ("3" == in) {
      const std::string pathname = getPathnameFromInput();
      static_filenames_.quantitationMethods_csv_i = pathname;
      for (SequenceSegmentHandler& sequenceSegmentHandler : sequenceHandler_.getSequenceSegments()) {
        LoadQuantitationMethods loadQuantitationMethods;
        loadQuantitationMethods.process(sequenceSegmentHandler, SequenceHandler(), {}, static_filenames_);
      }
    }
    else if ("4" == in) {
      const std::string pathname = getPathnameFromInput();
      static_filenames_.standardsConcentrations_csv_i = pathname;
      for (SequenceSegmentHandler& sequenceSegmentHandler : sequenceHandler_.getSequenceSegments()) {
        LoadStandardsConcentrations loadStandardsConcentrations;
        loadStandardsConcentrations.process(sequenceSegmentHandler, SequenceHandler(), {}, static_filenames_);
      }
    }
    else if ("5" == in) {
      const std::string pathname = getPathnameFromInput();
      static_filenames_.featureFilterComponents_csv_i = pathname;
      LoadFeatureFilters loadFeatureFilters;
      const std::string backup = static_filenames_.featureFilterComponentGroups_csv_i;
      static_filenames_.featureFilterComponentGroups_csv_i.clear();
      loadFeatureFilters.process(sequenceHandler_.getSequence()[0].getRawData(), {}, static_filenames_);
      static_filenames_.featureFilterComponentGroups_csv_i = backup;
    }
    else if ("6" == in) {
      const std::string pathname = getPathnameFromInput();
      static_filenames_.featureFilterComponentGroups_csv_i = pathname;
      LoadFeatureFilters loadFeatureFilters;
      const std::string backup = static_filenames_.featureFilterComponents_csv_i;
      static_filenames_.featureFilterComponents_csv_i.clear();
      loadFeatureFilters.process(sequenceHandler_.getSequence()[0].getRawData(), {}, static_filenames_);
      static_filenames_.featureFilterComponents_csv_i = backup;
    }
    else if ("7" == in) {
      const std::string pathname = getPathnameFromInput();
      static_filenames_.featureQCComponents_csv_i = pathname;
      LoadFeatureQCs loadFeatureQCs;
      const std::string backup = static_filenames_.featureQCComponentGroups_csv_i;
      static_filenames_.featureQCComponentGroups_csv_i.clear();
      loadFeatureQCs.process(sequenceHandler_.getSequence()[0].getRawData(), {}, static_filenames_);
      static_filenames_.featureQCComponentGroups_csv_i = backup;
    }
    else if ("8" == in) {
      const std::string pathname = getPathnameFromInput();
      static_filenames_.featureQCComponentGroups_csv_i = pathname;
      LoadFeatureQCs loadFeatureQCs;
      const std::string backup = static_filenames_.featureQCComponents_csv_i;
      static_filenames_.featureQCComponents_csv_i.clear();
      loadFeatureQCs.process(sequenceHandler_.getSequence()[0].getRawData(), {}, static_filenames_);
      static_filenames_.featureQCComponents_csv_i = backup;
    }
    else if ("9" == in) {
      const std::string pathname = getPathnameFromInput();
      static_filenames_.parameters_csv_i = pathname;
      LoadParameters loadParameters;
      loadParameters.process(sequenceHandler_.getSequence()[0].getRawData(), {}, static_filenames_);
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
      "[1]  Undo\n"
      "[2]  Redo\n"
      "[3]  Cut\n"
      "[4]  Copy\n"
      "[5]  Paste\n"
      "    ------------------------\n"
      "[6]  Sequence\n"
      "[7]  TraML\n"
      "[8]  Quantitation methods\n"
      "[9]  Standards concentrations\n"
      "[10] Component filters\n"
      "[11] Component group filters\n"
      "[12] Component QCs\n"
      "[13] Component group QCs\n"
      "[14] Parameters\n"
      "    ------------------------\n"
      "[15] Workflow\n"
      "[M] Main menu\n\n";

    std::string in;
  menuEdit_label:
    in = getLineInput("> ", false);

    if      ("1" == in) {
    }
    else if ("2" == in) {
    }
    else if ("3" == in) {
    }
    else if ("4" == in) {
    }
    else if ("5" == in) {
    }
    else if ("6" == in) {
    }
    else if ("7" == in) {
    }
    else if ("8" == in) {
    }
    else if ("9" == in) {
    }
    else if ("10" == in) {
    }
    else if ("11" == in) {
    }
    else if ("12" == in) {
    }
    else if ("13" == in) {
    }
    else if ("14" == in) {
    }
    else if ("15" == in) {
      initializeDataDirs();
      const std::vector<Command> methods = getMethodsInput();
      if (methods.empty()) {
        LOGW << "\n\nPipeline not modified";
      } else {
        commands_ = methods;
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
      "[1] Sequence status\n"
      "[2] Workflow wizard\n"
      "[3] Feature plot\n"
      "[4] Metric plot\n"
      "[5] Log\n"
      "[M] Main menu\n\n";

    std::string in;
  menuView_label:
    in = getLineInput("> ", false);

    if      ("1" == in) {
    }
    else if ("2" == in) {
      LOGN << "\n\n" << getPipelineString();
    }
    else if ("3" == in) {
    }
    else if ("4" == in) {
    }
    else if ("5" == in) {
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

      LOGN << "\n\n" << commandsString();

      const std::string input = getLineInput("> ");
      try {
        const int n = std::stoi(input);
        Command cmd;
        if (createCommand(n, cmd)) {
          processCommands({cmd});
        }
      } catch (...) {
        LOGE << "\n\nInvalid input: cannot convert to integer.\n";
      }
    }
    else if ("2" == in) {
      initializeDataDirs();
      processCommands(commands_);
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
      InputDataValidation::sampleNamesAreConsistent(sequenceHandler_);
    }
    else if ("2" == in) {
      InputDataValidation::componentNamesAreConsistent(sequenceHandler_);
    }
    else if ("3" == in) {
      InputDataValidation::componentNameGroupsAreConsistent(sequenceHandler_);
    }
    else if ("4" == in) {
      InputDataValidation::heavyComponentsAreConsistent(sequenceHandler_);
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
      featureSummaryMetaData_ = getMetaDataInput("\nMain > Actions > Report > Feature summary\n");
      featureSummaryTypes_    = getSampleTypesInput();
      const std::string pathname = main_dir_ + "/FeatureSummary.csv";
      const bool data_was_written = SequenceParser::writeDataTableFromMetaValue(
        sequenceHandler_,
        pathname,
        featureSummaryMetaData_,
        featureSummaryTypes_
      );
      if (data_was_written) {
        LOGN << "\n\nFeatureSummary.csv file has been stored at: " << pathname << '\n';
      } else {
        LOGE << "\n\nError during write. FeatureSummary.csv content is invalid.\n";
      }
    }
    else if ("2" == in) {
      sequenceSummaryMetaData_ = getMetaDataInput("\nMain > Actions > Report > Sequence summary\n");
      sequenceSummaryTypes_    = getSampleTypesInput();
      const std::string pathname = main_dir_ + "/SequenceSummary.csv";
      const bool data_was_written = SequenceParser::writeDataMatrixFromMetaValue(
        sequenceHandler_,
        pathname,
        sequenceSummaryMetaData_,
        sequenceSummaryTypes_
      );
      if (data_was_written) {
        LOGN << "\n\nSequenceSummary.csv file has been stored at: " << pathname << '\n';
      } else {
        LOGE << "\n\nError during write. FeatureSummary.csv content is invalid.\n";
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
      LOGN << "\n\n" << InputDataValidation::getSequenceInfo(sequenceHandler_) << "\n";
    }
    else if ("2" == in) {
      LOGN << "\n\n" << InputDataValidation::getTraMLInfo(
        sequenceHandler_.getSequence().front().getRawData()) << "\n";
    }
    else if ("3" == in) {
      LOGN << "\n\n" << InputDataValidation::getQuantitationMethodsInfo(
        sequenceHandler_.getSequenceSegments().front()) << "\n";
    }
    else if ("4" == in) {
      LOGN << "\n\n" << InputDataValidation::getStandardsConcentrationsInfo(
        sequenceHandler_.getSequenceSegments().front()) << "\n";
    }
    else if ("5" == in) {
      LOGN << "\n\n" << InputDataValidation::getComponentsAndGroupsInfo(
        sequenceHandler_.getSequence().front().getRawData(), true) << "\n";
    }
    else if ("6" == in) {
      LOGN << "\n\n" << InputDataValidation::getComponentsAndGroupsInfo(
        sequenceHandler_.getSequence().front().getRawData(), false) << "\n";
    }
    else if ("7" == in) {
      LOGN << "\n\n" << InputDataValidation::getParametersInfo(
        sequenceHandler_.getSequence().front().getRawData().getParameters()) << "\n";
    }
    else if ("8" == in) {
      LOGN << "\n\n" << sequenceHandler_.getRawDataFilesInfo() << "\n";
    }
    else if ("9" == in) {
      LOGN << "\n\n" << sequenceHandler_.getAnalyzedFeaturesInfo() << "\n";
    }
    else if ("10" == in) {
      LOGN << "\n\n" << sequenceHandler_.getSelectedFeaturesInfo() << "\n";
    }
    else if ("11" == in) {
      LOGN << "\n\n" << sequenceHandler_.getPickedPeaksInfo() << "\n";
    }
    else if ("12" == in) {
      LOGN << "\n\n" << sequenceHandler_.getFilteredSelectedPeaksInfo() << "\n";
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
      "[1] About\n"
      "[2] Documentation\n"
      "[3] Getting started\n"
      "[4] Version\n"
      "[M] Main menu\n\n";

    std::string in;
  menuHelp_label:
    in = getLineInput("> ", false);

    if      ("1" == in) {
    }
    else if ("2" == in) {
    }
    else if ("3" == in) {
      LOGN << "\n\n" << gettingStartedString();
    }
    else if ("4" == in) {
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

  bool buildStaticFilenames()
  {
    Filenames& f = static_filenames_;
    main_dir_ = sequence_pathname_.substr(0, sequence_pathname_.find_last_of('/'));
    f = Filenames::getDefaultStaticFilenames(main_dir_);
    clearNonExistantDefaultGeneratedFilenames(f);
    f.sequence_csv_i = sequence_pathname_;

    const std::string pathnamesFilePath = main_dir_ + "/" + pathnamesFilename_;

    if (InputDataValidation::fileExists(pathnamesFilePath)) {
      LOGN << "\n\n"
        "Pathnames file was found:\n" <<
        " - " << pathnamesFilePath << "\n"
        "Should its values be used to search for the input files? [Y/n]\n";
      const std::string in = getLineInput("> ");
      if (in.empty() || in.front() == 'y') {
        LOGN << "\n\nValues in " << pathnamesFilePath << ": USED\n";
        updateFilenames(f, pathnamesFilePath);
      } else {
        LOGN << "\n\nValues in " << pathnamesFilePath << ": IGNORED\n";
      }
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
      getLineInput("Press Enter to go back to the Main menu.\n");
      return false;
    }

    return true;
  }

  bool requiredPathnamesAreValid(const std::vector<InputDataValidation::FilenameInfo>& validation)
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

  void clearNonExistantDefaultGeneratedFilenames(Filenames& f)
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

  void clearNonExistantFilename(std::string& filename)
  {
    if (InputDataValidation::fileExists(filename) == false) {
      filename.clear();
    }
  }

  void generatePathnamesTxt(
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

  std::string getValidPathnameOrPlaceholder(const std::string& pathname, const bool is_valid)
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

  std::vector<Command> getMethodsInput()
  {
    std::vector<Command> methods;

    LOGN << main_menu_;

    const std::string line = getLineInput("> ", false);

    if (line[0] == 'M' || line[0] == 'm') {
      return {};
    }

    std::istringstream iss {line};

    for (int n; iss >> n;) {
      Command cmd;
      const bool created = createCommand(n, cmd);
      if (created) {
        methods.push_back(cmd);
      }
    }

    return methods;
  }

  void setSequencePathnameFromInput()
  {
    LOGN << "\n\nSet the sequence file pathname.\n";
    if (sequence_pathname_.size()) {
      LOGN << "\n\nCurrent: " << sequence_pathname_ << "\n\n";
    } else {
      LOGN << "\n\n"
        "Example:\n"
        // "> /home/user/data/some_sequence_file.csv\n\n"
        "> /home/pasdom/SmartPeak2/src/examples/data/HPLC_UV_Standards/sequence.csv\n\n";
    }
    LOGN << "\n\nEnter the absolute pathname:\n";
    while (true) {
      sequence_pathname_ = getLineInput("> ", false);
      if (InputDataValidation::fileExists(sequence_pathname_)) {
        break;
      }
      LOGE << "\n\nThe file does not exist. Try again.\n";
    }
    LOGI << "\n\nSequence pathname set to: " << sequence_pathname_;
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
    LOGD << "Input line: " << line;
    return line;
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

  std::set<MetaDataHandler::SampleType> getSampleTypesInput()
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
    std::set<MetaDataHandler::SampleType> sample_types;

    for (int n; iss >> n;) {
      if (n < 1 || n > 6) {
        LOGW << "Skipping: " << n << '\n';
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
    std::vector<std::string> metadata;

    for (int n; iss >> n;) {
      if (n < 1 || n > 19) {
        LOGW << "Skipping: " << n << '\n';
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
        metadata.push_back("peak_area"); // "intensity" attribute of a subordinate feature
        // NOTE: there is also an "intensity" attribute at the feature level
        // which is the sum of all subordinate feature "intensities"
        break;
      case 7:
        metadata.push_back("points_across_baseline");
        break;
      case 8:
        metadata.push_back("points_across_half_height");
        break;
      case 9:
        metadata.push_back("QC_transition_pass");
        break;
      case 10:
        metadata.push_back("QC_transition_message");
        break;
      case 11:
        metadata.push_back("QC_transition_score");
        break;
      case 12:
        metadata.push_back("QC_transition_group_pass");
        break;
      case 13:
        metadata.push_back("QC_transition_group_message");
        break;
      case 14:
        metadata.push_back("QC_transition_group_score");
        break;
      case 15:
        metadata.push_back("tailing_factor");
        break;
      case 16:
        metadata.push_back("total_width");
        break;
      case 17:
        metadata.push_back("width_at_50");
        break;
      case 18:
        metadata.push_back("RT");
        break;
      case 19:
        metadata = {
          "peak_apex_int", "peak_area", "total_width", "width_at_50", "tailing_factor",
          "asymmetry_factor", "baseline_delta_2_height", "points_across_baseline",
          "points_across_half_height", "logSN", "calculated_concentration",
          "QC_transition_message", "QC_transition_pass", "QC_transition_score",
          "QC_transition_group_message", "QC_transition_group_pass",
          "QC_transition_group_score", "RT"
        };
        break;
      }
    }
    return metadata;
  }

  std::string gettingStartedString()
  {
    return
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
  }

  std::string commandsString()
  {
    return
      "[" + std::to_string(OPT_LOAD_RAW_DATA) + "]  Load raw data\n"
      "[" + std::to_string(OPT_LOAD_FEATURES) + "]  Load features\n"
      "[" + std::to_string(OPT_PICK_FEATURES) + "]  Pick features\n"
      "[" + std::to_string(OPT_FILTER_FEATURES) + "]  Filter features\n"
      "[" + std::to_string(OPT_SELECT_FEATURES) + "]  Select features\n"
      "[" + std::to_string(OPT_VALIDATE_FEATURES) + "]  Validate features\n"
      "[" + std::to_string(OPT_QUANTIFY_FEATURES) + "]  Quantify features\n"
      "[" + std::to_string(OPT_CHECK_FEATURES) + "]  Check features\n"
      "[" + std::to_string(OPT_STORE_FEATURES) + "]  Store features\n"
      "[" + std::to_string(OPT_PLOT_FEATURES) + "] Plot features (not implemented)\n"
      "[" + std::to_string(OPT_MAP_CHROMATROGRAMS) + "] Map transitions to the raw data\n"
      "[" + std::to_string(OPT_ZERO_CHROMATOGRAM_BASELINE) + "] Zero the chromatogram baseline\n"
      "[" + std::to_string(OPT_EXTRACT_CHROMATOGRAM_WIDOWS) + "] Extract chromatogram windows\n"
      "[" + std::to_string(OPT_CALCULATE_CALIBRATION) + "] Calculate calibration\n"
      "[" + std::to_string(OPT_STORE_QUANTITATION_METHODS) + "] Store quantitation methods\n"
      "[" + std::to_string(OPT_LOAD_QUANTITATION_METHODS) + "] Load quantitation methods\n";
  }

  void processCommands(const std::vector<Command>& commands)
  {
    size_t i = 0;
    while (i < commands.size()) {
      const Command::CommandType type = commands[i].type;
      size_t j = i + 1;
      for (; j < commands.size() && type == commands[j].type; ++j) {
        // empty body
      }
      const Command& cmd = commands[i];
      if (cmd.type == Command::RawDataMethod) {
        std::vector<std::shared_ptr<RawDataProcessor>> raw_methods;
        std::transform(commands.begin() + i, commands.begin() + j, std::back_inserter(raw_methods),
          [](const Command& command){ return command.raw_data_method; });
        SequenceProcessor::processSequence(
          sequenceHandler_,
          cmd.dynamic_filenames,
          std::set<std::string>(),
          raw_methods
        );
      } else if (cmd.type == Command::SequenceSegmentMethod) {
        std::vector<std::shared_ptr<SequenceSegmentProcessor>> seq_seg_methods;
        std::transform(commands.begin() + i, commands.begin() + j, std::back_inserter(seq_seg_methods),
          [](const Command& command){ return command.seq_seg_method; });
        SequenceProcessor::processSequenceSegments(
          sequenceHandler_,
          cmd.dynamic_filenames,
          std::set<std::string>(),
          seq_seg_methods
        );
      } else {
        LOGW << "Skipping a command: " << cmd.type << "\n";
      }
      i = j;
    }
  }

  bool createCommand(const int n, Command& cmd)
  {
    if (n < 1 || n > 16 || n == 10) { // TODO: update this if plotting is implemented
      LOGW << "\n\nSkipping: " << n;
      return false;
    }
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
    } else {
      LOGE << "\nNo command for selection number " << n;
      return false;
    }
    return true;
  }

  void initializeDataDirs()
  {
    if (mzML_dir_.empty()) {
      mzML_dir_ = main_dir_ + "/mzML";
      LOGN << "\n\nPath for 'mzML':\t" << mzML_dir_;
      LOGN << "\n\nEnter an absolute pathname to change it, or press Enter to confirm.\n";
      const std::string path_input = getLineInput("> ");
      if (path_input.size()) {
        mzML_dir_ = path_input;
      }
    }

    if (features_in_dir_.empty()) {
      features_in_dir_ = main_dir_ + "/features";
      LOGN << "\n\nPath for 'INPUT features':\t" << features_in_dir_;
      LOGN << "\n\nEnter an absolute pathname to change it, or press Enter to confirm.\n";
      const std::string path_input = getLineInput("> ");
      if (path_input.size()) {
        features_in_dir_ = path_input;
      }
    }

    if (features_out_dir_.empty()) {
      features_out_dir_ = main_dir_ + "/features";
      LOGN << "\n\nPath for 'OUTPUT features':\t" << features_out_dir_;
      LOGN << "\n\nEnter an absolute pathname to change it, or press Enter to confirm.\n";
      const std::string path_input = getLineInput("> ");
      if (path_input.size()) {
        features_out_dir_ = path_input;
      }
    }
  }

  // // Initializes the sequence structure
  // CommandLine(int argc, char **argv)
  // {
  //   // Three ways of setting `sequence_pathname_`
  //   if (argc == 2 && InputDataValidation::fileExists(argv[1])) { // sequence.csv abs. path passed as argument
  //     sequence_pathname_ = argv[1];
  //   } else if (InputDataValidation::fileExists("sequence.csv")) { // or found in the same folder of the executable
  //     char cwd_buf[33000];
  //     if (!mygetcwd(cwd_buf, 33000)) {
  //       throw "\ngetcwd failed.\n";
  //     }
  //     sequence_pathname_ = std::string(cwd_buf) + std::string("/sequence.csv");
  //   } else {
  //     setSequencePathnameFromInput();
  //   }
  //   const bool pathnamesAreCorrect = buildStaticFilenames();
  //   if (pathnamesAreCorrect) {
  //     SequenceProcessor::createSequence(sequenceHandler_, static_filenames_, ",", true);
  //   }
  // }

  CommandLine()                                    = default;
  ~CommandLine()                                   = default;
  CommandLine(const CommandLine& other)            = delete;
  CommandLine& operator=(const CommandLine& other) = delete;
  CommandLine(CommandLine&& other)                 = delete;
  CommandLine& operator=(CommandLine&& other)      = delete;

  void runApp() {
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
        std::tm t;
        (plog::util::localtime_s)(&t, &record.getTime().time);

        plog::util::nostringstream ss;
        ss << t.tm_year + 1900 << "-" << std::setfill(PLOG_NSTR('0')) << std::setw(2) << t.tm_mon + 1 << PLOG_NSTR("-") << std::setfill(PLOG_NSTR('0')) << std::setw(2) << t.tm_mday << PLOG_NSTR(" ");
        ss << std::setfill(PLOG_NSTR('0')) << std::setw(2) << t.tm_hour << PLOG_NSTR(":") << std::setfill(PLOG_NSTR('0')) << std::setw(2) << t.tm_min << PLOG_NSTR(":") << std::setfill(PLOG_NSTR('0')) << std::setw(2) << t.tm_sec << PLOG_NSTR(" ");
        ss << std::setfill(PLOG_NSTR(' ')) << std::setw(5) << std::left << severityToString(record.getSeverity()) << PLOG_NSTR(" ");
        // ss << PLOG_NSTR("[") << record.getTid() << PLOG_NSTR("] ");
        ss << PLOG_NSTR("\n") << record.getMessage() << PLOG_NSTR("\n");

        return ss.str();
      }
    };

    // Add console appender, instead of only the file one
    static plog::ConsoleAppender<ConsoleFormatter> consoleAppender;

    // Init logger with two appenders
    plog::init(plog::debug, &fileAppender).addAppender(&consoleAppender);

    LOGN << "\n\n" << gettingStartedString();
    while (true) {
      menuMain();
    }
  }

  // Returns a string representation of the workflow steps
  // i.e. 1 2 3 4 5 5 18
  std::string getPipelineString()
  {
    std::string s;
    for (const Command& cmd : commands_) {
      if (cmd.type == Command::RawDataMethod) {
        const std::unordered_map<int, std::shared_ptr<RawDataProcessor>>::const_iterator
        it = std::find_if(
          n_to_raw_data_method_.cbegin(),
          n_to_raw_data_method_.cend(),
          [&cmd](const std::pair<int, std::shared_ptr<RawDataProcessor>>& p)
            { return p.second == cmd.raw_data_method; }
        );
        s.append(std::to_string(it->first));
      } else if (cmd.type == Command::SequenceSegmentMethod) {
        const std::unordered_map<int, std::shared_ptr<SequenceSegmentProcessor>>::const_iterator
        it = std::find_if(
          n_to_seq_seg_method_.cbegin(),
          n_to_seq_seg_method_.cend(),
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
};

// int main(int argc, char **argv)
int main()
{
  // CommandLine cli(argc, argv);
  CommandLine cli;
  cli.runApp();
  return 0;
}

