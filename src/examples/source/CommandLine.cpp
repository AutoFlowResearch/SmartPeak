#include <SmartPeak/core/Filenames.h>
#include <SmartPeak/core/RawDataProcessor.h>
#include <SmartPeak/core/SequenceProcessor.h>
#include <SmartPeak/io/InputDataValidation.h>
#include <SmartPeak/io/SequenceParser.h>
#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <regex>
#include <unordered_map>

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
      WriteSequenceSummary,
      WriteFeatureSummary,
      GetSequenceInfo,
      GetParametersInfo,
      GetTraMLInfo,
      GetFeatureFilterInfo,
      GetFeatureQCInfo,
      GetQuantMethodsInfo,
      GetStandardsConcInfo,
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
  };
  const std::unordered_map<int, std::shared_ptr<SequenceSegmentProcessor>> n_to_seq_seg_method_ {
    {11, std::shared_ptr<SequenceSegmentProcessor>(new CalculateCalibration())},
    {12, std::shared_ptr<SequenceSegmentProcessor>(new StoreQuantitationMethods())},
    {13, std::shared_ptr<SequenceSegmentProcessor>(new LoadQuantitationMethods())},
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
    OPT_CALCULATE_CALIBRATION,
    OPT_STORE_QUANTITATION_METHODS,
    OPT_LOAD_QUANTITATION_METHODS,
    OPT_WRITE_SEQUENCE_SUMMARY,
    OPT_WRITE_FEATURE_SUMMARY,
    OPT_GET_SEQUENCE_INFO,
    OPT_GET_PARAMETERS_INFO,
    OPT_GET_TRAML_INFO,
    OPT_GET_COMP_N_GROUPS_FILTER_INFO,
    OPT_GET_COMP_N_GROUPS_QC_INFO,
    OPT_GET_QUANT_METHODS_INFO,
    OPT_GET_STANDARDS_CONC_INFO,
  };
  const std::string main_menu_ = "\n\n"
    "Please insert the sequence of methods to run.\n"
    "You can choose the same method multiple times.\n"
    "Separate chosen methods with a space.\n\n"
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
    "[" + std::to_string(OPT_CALCULATE_CALIBRATION) + "] Calculate calibration\n"
    "[" + std::to_string(OPT_STORE_QUANTITATION_METHODS) + "] Store quantitation methods\n"
    "[" + std::to_string(OPT_LOAD_QUANTITATION_METHODS) + "] Load quantitation methods\n"
    "[" + std::to_string(OPT_WRITE_SEQUENCE_SUMMARY) + "] Write SequenceSummary.csv\n"
    "[" + std::to_string(OPT_WRITE_FEATURE_SUMMARY) + "] Write FeatureSummary.csv\n"
    "[" + std::to_string(OPT_GET_SEQUENCE_INFO) + "] Print sequence info\n"
    "[" + std::to_string(OPT_GET_PARAMETERS_INFO) + "] Print parameters info\n"
    "[" + std::to_string(OPT_GET_TRAML_INFO) + "] Print TraML info\n"
    "[" + std::to_string(OPT_GET_COMP_N_GROUPS_FILTER_INFO) + "] Print components and components groups (FeatureFilter) info\n"
    "[" + std::to_string(OPT_GET_COMP_N_GROUPS_QC_INFO) + "] Print components and components groups (FeatureQC) info\n"
    "[" + std::to_string(OPT_GET_QUANT_METHODS_INFO) + "] Print quantitation methods info\n"
    "[" + std::to_string(OPT_GET_STANDARDS_CONC_INFO) + "] Print standards concentrations info\n"
    "[M]  Main menu\n\n"
    "Presets:\n"
    "Unknowns: 1 3 4 4 5 7 8 9 14 15\n"
    "Standards: 1 3 4 4 5 8 9 11 12 7 8 9 14 15\n"
    "Validation: 1 3 4 5 6 14 15\n\n";

  void menuMain()
  {
    std::cout <<
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
    std::cout <<
      "\n\n"
      "Main > File\n"
      "[1] New session\n"
      "[2] Load session\n"
      "[3] Save session\n"
      "[4] Import file\n"
      "[5] Export file\n"
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
    }
    else if ("4" == in) {
      menuImportFile();
    }
    else if ("5" == in) {
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
    std::cout <<
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
      buildStaticFilenames();
      sequenceHandler_.clear();
      std::cout << "Data has been cleared.\n";
      SequenceProcessor::createSequence(sequenceHandler_, static_filenames_, ",", true, verbose_);
    }
    else if ("2" == in) {
      const std::string pathname = getPathnameFromInput();
      static_filenames_.traML_csv_i = pathname;
      LoadTransitions loadTransitions;
      loadTransitions.process(sequenceHandler_.getSequence()[0].getRawData(), {}, static_filenames_, verbose_);
    }
    else if ("3" == in) {
      const std::string pathname = getPathnameFromInput();
      static_filenames_.quantitationMethods_csv_i = pathname;
      for (SequenceSegmentHandler& sequenceSegmentHandler : sequenceHandler_.getSequenceSegments()) {
        LoadQuantitationMethods loadQuantitationMethods;
        loadQuantitationMethods.process(sequenceSegmentHandler, SequenceHandler(), {}, static_filenames_, verbose_);
      }
    }
    else if ("4" == in) {
      const std::string pathname = getPathnameFromInput();
      static_filenames_.standardsConcentrations_csv_i = pathname;
      for (SequenceSegmentHandler& sequenceSegmentHandler : sequenceHandler_.getSequenceSegments()) {
        LoadStandardsConcentrations loadStandardsConcentrations;
        loadStandardsConcentrations.process(sequenceSegmentHandler, SequenceHandler(), {}, static_filenames_, verbose_);
      }
    }
    else if ("5" == in) {
      const std::string pathname = getPathnameFromInput();
      static_filenames_.featureFilterComponents_csv_i = pathname;
      LoadFeatureFilters loadFeatureFilters;
      const std::string backup = static_filenames_.featureFilterComponentGroups_csv_i;
      static_filenames_.featureFilterComponentGroups_csv_i.clear();
      loadFeatureFilters.process(sequenceHandler_.getSequence()[0].getRawData(), {}, static_filenames_, verbose_);
      static_filenames_.featureFilterComponentGroups_csv_i = backup;
    }
    else if ("6" == in) {
      const std::string pathname = getPathnameFromInput();
      static_filenames_.featureFilterComponentGroups_csv_i = pathname;
      LoadFeatureFilters loadFeatureFilters;
      const std::string backup = static_filenames_.featureFilterComponents_csv_i;
      static_filenames_.featureFilterComponents_csv_i.clear();
      loadFeatureFilters.process(sequenceHandler_.getSequence()[0].getRawData(), {}, static_filenames_, verbose_);
      static_filenames_.featureFilterComponents_csv_i = backup;
    }
    else if ("7" == in) {
      const std::string pathname = getPathnameFromInput();
      static_filenames_.featureQCComponents_csv_i = pathname;
      LoadFeatureQCs loadFeatureQCs;
      const std::string backup = static_filenames_.featureQCComponentGroups_csv_i;
      static_filenames_.featureQCComponentGroups_csv_i.clear();
      loadFeatureQCs.process(sequenceHandler_.getSequence()[0].getRawData(), {}, static_filenames_, verbose_);
      static_filenames_.featureQCComponentGroups_csv_i = backup;
    }
    else if ("8" == in) {
      const std::string pathname = getPathnameFromInput();
      static_filenames_.featureQCComponentGroups_csv_i = pathname;
      LoadFeatureQCs loadFeatureQCs;
      const std::string backup = static_filenames_.featureQCComponents_csv_i;
      static_filenames_.featureQCComponents_csv_i.clear();
      loadFeatureQCs.process(sequenceHandler_.getSequence()[0].getRawData(), {}, static_filenames_, verbose_);
      static_filenames_.featureQCComponents_csv_i = backup;
    }
    else if ("9" == in) {
      const std::string pathname = getPathnameFromInput();
      static_filenames_.parameters_csv_i = pathname;
      LoadParameters loadParameters;
      loadParameters.process(sequenceHandler_.getSequence()[0].getRawData(), {}, static_filenames_, verbose_);
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
    std::cout <<
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
        std::cout << "\nPipeline not modified.\n";
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
    std::cout <<
      "\n\n"
      "Main > View\n"
      "[1] Sequence status\n"
      "[2] Workflow wizard\n"
      "[3] Feature plot\n"
      "[4] Metric plot\n"
      "[5] Feature summary\n"
      "[6] Sequence summary\n"
      "[7] Log\n"
      "[M] Main menu\n\n";

    std::string in;
  menuView_label:
    in = getLineInput("> ", false);

    if      ("1" == in) {
    }
    else if ("2" == in) {
      std::cout << getPipelineString();
    }
    else if ("3" == in) {
    }
    else if ("4" == in) {
    }
    else if ("5" == in) {
      const std::string pathname = main_dir_ + "/FeatureSummary.csv";
      SequenceParser::writeDataTableFromMetaValue(
        sequenceHandler_,
        pathname,
        featureSummaryMetaData_,
        featureSummaryTypes_
      );
      std::cout << "FeatureSummary.csv file has been stored at: " << pathname << '\n';
    }
    else if ("6" == in) {
      const std::string pathname = main_dir_ + "/SequenceSummary.csv";
      SequenceParser::writeDataMatrixFromMetaValue(
        sequenceHandler_,
        pathname,
        sequenceSummaryMetaData_,
        sequenceSummaryTypes_
      );
      std::cout << "SequenceSummary.csv file has been stored at: " << pathname << '\n';
    }
    else if ("7" == in) {
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
    std::cout <<
      "\n\n"
      "Main > Actions\n"
      "[1] Run command\n"
      "[2] Run workflow\n"
      "[3] Quick info\n"
      "[4] Report\n"
      "[M] Main menu\n\n";

    std::string in;
  menuActions_label:
    in = getLineInput("> ", false);

    if      ("1" == in) {
    }
    else if ("2" == in) {
      size_t i = 0;
      while (i < commands_.size()) {
        const Command::CommandType type = commands_[i].type;
        size_t j = i + 1;
        for (; j < commands_.size() && type == commands_[j].type; ++j) {
          // empty body
        }
        const Command& cmd = commands_[i];
        if (cmd.type == Command::RawDataMethod) {
          std::vector<std::shared_ptr<RawDataProcessor>> raw_methods;
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
          std::vector<std::shared_ptr<SequenceSegmentProcessor>> seq_seg_methods;
          std::transform(commands_.begin() + i, commands_.begin() + j, std::back_inserter(seq_seg_methods),
            [](const Command& command){ return command.seq_seg_method; });
          SequenceProcessor::processSequenceSegments(
            sequenceHandler_,
            cmd.dynamic_filenames,
            std::set<std::string>(),
            seq_seg_methods,
            verbose_
          );
        } else {
          std::cout << "\nSkipping a command.\n";
        }
        i = j;
      }
      std::cout << "\nWorkflow completed.\n";
    }
    else if ("3" == in) {
      menuQuickInfo();
    }
    else if ("4" == in) {
    }
    else if ("m" == in || "M" == in) {
      // empty
    }
    else {
      goto menuActions_label;
    }
  }

  void menuQuickInfo()
  {
    std::cout <<
      "\n\n"
      "Main > Actions > Quick Info\n"
      "[1] Sequence\n"
      "[2] TraML\n"
      "[3] Quantitation methods\n"
      "[4] Standards concentrations\n"
      "[5] Feature filters\n"
      "[6] Feature QC\n"
      "[7] Parameters\n"
      "[M] Main menu\n\n";

    std::string in;
  menuQuickInfo_label:
    in = getLineInput("> ", false);

    if      ("1" == in) {
      std::cout << InputDataValidation::getSequenceInfo(sequenceHandler_, ",") << "\n";
    }
    else if ("2" == in) {
      std::cout << InputDataValidation::getTraMLInfo(
        sequenceHandler_.getSequence().front().getRawData()) << "\n";
    }
    else if ("3" == in) {
      std::cout << InputDataValidation::getQuantitationMethodsInfo(
        sequenceHandler_.getSequenceSegments().front()) << "\n";
    }
    else if ("4" == in) {
      std::cout << InputDataValidation::getStandardsConcentrationsInfo(
        sequenceHandler_.getSequenceSegments().front()) << "\n";
    }
    else if ("5" == in) {
      std::cout << InputDataValidation::getComponentsAndGroupsInfo(
        sequenceHandler_.getSequence().front().getRawData(), true) << "\n";
    }
    else if ("6" == in) {
      std::cout << InputDataValidation::getComponentsAndGroupsInfo(
        sequenceHandler_.getSequence().front().getRawData(), false) << "\n";
    }
    else if ("7" == in) {
      std::cout << InputDataValidation::getParametersInfo(
        sequenceHandler_.getSequence().front().getRawData().getParameters()) << "\n";
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
    std::cout <<
      "\n\n"
      "Main > Help\n"
      "[1] About\n"
      "[2] Documentation\n"
      "[3] Version\n"
      "[M] Main menu\n\n";

    std::string in;
  menuHelp_label:
    in = getLineInput("> ", false);

    if      ("1" == in) {
      std::cout << "SmartPeak2 about message\n";
    }
    else if ("2" == in) {
      std::cout << "SmartPeak2 documentation message\n";
    }
    else if ("3" == in) {
      // TODO: macro with commit revision? sha1sum of source file?
      std::cout << "SmartPeak2 version message\n";
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
    if (std::tolower(in.front()) == 'y') {
      std::exit(EXIT_SUCCESS);
    }
  }

  void buildStaticFilenames()
  {
    Filenames& f = static_filenames_;
    main_dir_ = sequence_pathname_.substr(0, sequence_pathname_.find_last_of('/'));
    f = Filenames::getDefaultStaticFilenames(main_dir_);
    clearNonExistantDefaultGeneratedFilenames(f);
    f.sequence_csv_i = sequence_pathname_;

    const std::string pathnamesFilePath = main_dir_ + "/" + std::string(pathnamesFilename_);

    if (InputDataValidation::fileExists(pathnamesFilePath)) {
      std::cout << "\n\n"
        "File " << pathnamesFilePath << " was found in the same directory. "
        "This file contains information about where the various experiment's files are found.\n\n"
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

    std::cout << "\n\n"
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
      std::cout << "\n\nOne or more files were not found.\n"
        "The file " << pathnamesFilePath <<
        " has been generated for you to fix pathnames.\n"
        "The incorrect information has been replaced with an empty value.\n"
        "If you want a pathname to be ignored, then remove its value and leave only the label.\n"
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
    if (InputDataValidation::fileExists(filename) == false) {
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
        std::cout << "\n\n"
          "Regex did not match with the line: " << line << "\n"
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

    std::cout << main_menu_;

    const std::string line = getLineInput("> ", false);

    if (line[0] == 'M' || line[0] == 'm') {
      return {};
    }

    std::istringstream iss {line};

    for (int n; iss >> n;) {
      if (n < 1 || n > 22 || n == 10) { // TODO: update this if plotting is implemented
        std::cout << "Skipping: " << n << '\n';
        continue;
      }
      Command cmd;
      if (n >= 1 && n <= 10) {
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
      } else if (n >= 11 && n <= 13) {
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
      } else if (n == 14) {
        cmd.type = Command::WriteSequenceSummary;
        sequenceSummaryMetaData_ = getMetaDataInput("\nSequenceSummary.csv\n");
        sequenceSummaryTypes_ = getSampleTypesInput();
      } else if (n == 15) {
        cmd.type = Command::WriteFeatureSummary;
        featureSummaryMetaData_ = getMetaDataInput("\nFeatureSummary.csv\n");
        featureSummaryTypes_ = getSampleTypesInput();
      } else if (n == 16) {
        cmd.type = Command::GetSequenceInfo;
      } else if (n == 17) {
        cmd.type = Command::GetParametersInfo;
      } else if (n == 18) {
        cmd.type = Command::GetTraMLInfo;
      } else if (n == 19) {
        cmd.type = Command::GetFeatureFilterInfo;
      } else if (n == 20) {
        cmd.type = Command::GetFeatureQCInfo;
      } else if (n == 21) {
        cmd.type = Command::GetQuantMethodsInfo;
      } else if (n == 22) {
        cmd.type = Command::GetStandardsConcInfo;
      } else {
        std::cout << "\ninvalid option\n";
        continue;
      }
      methods.push_back(cmd);
    }

    return methods;
  }

  void setSequencePathnameFromInput()
  {
    std::cout << "\n\nSet the sequence file pathname.\n";
    if (sequence_pathname_.size()) {
      std::cout << "Current: " << sequence_pathname_ << "\n\n";
    } else {
      std::cout <<
        "Example:\n"
        // "> /home/user/data/some_sequence_file.csv\n\n"
        "> /home/pasdom/SmartPeak2/src/examples/data/HPLC_UV_Standards/sequence.csv\n\n";
    }
    std::cout << "Enter the absolute pathname:\n";
    while (true) {
      sequence_pathname_ = getLineInput("> ", false);
      if (InputDataValidation::fileExists(sequence_pathname_)) {
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

  std::string getPathnameFromInput()
  {
    std::string pathname;
    std::cout << "Pathname: ";
    while (true) {
      pathname = getLineInput("> ", false);
      if (InputDataValidation::fileExists(pathname)) {
        break;
      }
      std::cout << "The file does not exist. Try again.\n";
    }
    return pathname;
  }

  std::set<MetaDataHandler::SampleType> getSampleTypesInput()
  {
    std::cout << "\nPlease select the sample types. Insert the indexes separated by a space:\n"
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
      "[12] QC transition group message\n"
      "[13] QC transition group score\n"
      "[14] Tailing factor\n"
      "[15] Total width\n"
      "[16] Width at 50% peak's height\n"
      "[17] Retention time\n"
      "[18] Preset (all): 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17\n\n";

    std::string line;

    do {
      line = getLineInput("> ");
    } while (line.empty());

    std::istringstream iss;
    iss.str(line);
    std::vector<std::string> metadata;

    for (int n; iss >> n;) {
      if (n < 1 || n > 18) {
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
        metadata.push_back("QC_transition_group_message");
        break;
      case 13:
        metadata.push_back("QC_transition_group_score");
        break;
      case 14:
        metadata.push_back("tailing_factor");
        break;
      case 15:
        metadata.push_back("total_width");
        break;
      case 16:
        metadata.push_back("width_at_50");
        break;
      case 17:
        metadata.push_back("RT");
        break;
      case 18:
        metadata = {
          "peak_apex_int", "peak_area", "total_width", "width_at_50", "tailing_factor",
          "asymmetry_factor", "baseline_delta_2_height", "points_across_baseline",
          "points_across_half_height", "logSN", "calculated_concentration",
          "QC_transition_message", "QC_transition_pass", "QC_transition_score",
          "QC_transition_group_message", "QC_transition_group_score", "RT"
        };
        break;
      }
    }
    return metadata;
  }

  CommandLine(int argc, char **argv)
  {
    // Three ways of setting `sequence_pathname_`
    if (argc == 2 && InputDataValidation::fileExists(argv[1])) { // sequence.csv abs. path passed as argument
      sequence_pathname_ = argv[1];
    } else if (InputDataValidation::fileExists("sequence.csv")) { // or found in the same folder of the executable
      char cwd_buf[33000];
      if (!mygetcwd(cwd_buf, 33000)) {
        throw "\ngetcwd failed.\n";
      }
      sequence_pathname_ = std::string(cwd_buf) + std::string("/sequence.csv");
    } else {
      setSequencePathnameFromInput();
    }

    buildStaticFilenames();
    SequenceProcessor::createSequence(sequenceHandler_, static_filenames_, ",", true, verbose_);

    while (true) {
      // printMenu();
      // const std::string line = getLineInput("> ", false);
      menuMain();
      // parseCommand(line);
    }
  }

  // void printMenu()
  // {
  //   std::cout << "\n\n"
  //     "SmartPeak Main menu\n"
  //     "[1] Set sequence.csv pathname\t[\"" << sequence_pathname_ << "\"]\n"
  //     "[2] Set processing steps [" << getPipelineString() << "]\n"
  //     "[3] Run the pipeline\n"
  //     "[E] Exit SmartPeak\n\n"
  //     "Please select your action.\n";
  // }

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
      } else if(cmd.type == Command::WriteSequenceSummary) {
        s.append(std::to_string(OPT_WRITE_SEQUENCE_SUMMARY));
      } else if(cmd.type == Command::WriteFeatureSummary) {
        s.append(std::to_string(OPT_WRITE_FEATURE_SUMMARY));
      } else if(cmd.type == Command::GetSequenceInfo) {
        s.append(std::to_string(OPT_GET_SEQUENCE_INFO));
      } else if(cmd.type == Command::GetParametersInfo) {
        s.append(std::to_string(OPT_GET_PARAMETERS_INFO));
      } else if(cmd.type == Command::GetTraMLInfo) {
        s.append(std::to_string(OPT_GET_TRAML_INFO));
      } else if(cmd.type == Command::GetFeatureFilterInfo) {
        s.append(std::to_string(OPT_GET_COMP_N_GROUPS_FILTER_INFO));
      } else if(cmd.type == Command::GetFeatureQCInfo) {
        s.append(std::to_string(OPT_GET_COMP_N_GROUPS_QC_INFO));
      } else if(cmd.type == Command::GetQuantMethodsInfo) {
        s.append(std::to_string(OPT_GET_QUANT_METHODS_INFO));
      } else if(cmd.type == Command::GetStandardsConcInfo) {
        s.append(std::to_string(OPT_GET_STANDARDS_CONC_INFO));
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

  void parseCommand(const std::string& line)
  {
    int opt;

    try {
      opt = std::stoi(line);
    } catch (const std::invalid_argument&) {
      if (line[0] == 'E' || line[0] == 'e') {
        std::exit(EXIT_SUCCESS);
      }
      std::cout << "Invalid input. Try again.\n";
      return;
    }

    switch (opt) {
    case 1:
      setSequencePathnameFromInput();
      buildStaticFilenames();
      commands_.clear();
      sequenceHandler_.clear();
      SequenceProcessor::createSequence(sequenceHandler_, static_filenames_, ",", true, verbose_);
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
        std::cout << "\nPipeline not modified.\n";
        break;
      }
      commands_ = methods;
      break;
    }
    case 3:
    {
      size_t i = 0;
      while (i < commands_.size()) {
        const Command::CommandType type = commands_[i].type;
        size_t j = i + 1;
        for (; j < commands_.size() && type == commands_[j].type; ++j) {
          // empty body
        }
        const Command& cmd = commands_[i];
        if (cmd.type == Command::RawDataMethod) {
          std::vector<std::shared_ptr<RawDataProcessor>> raw_methods;
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
          std::vector<std::shared_ptr<SequenceSegmentProcessor>> seq_seg_methods;
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
        } else if (cmd.type == Command::GetSequenceInfo) {
          std::cout << InputDataValidation::getSequenceInfo(sequenceHandler_, ",") << "\n";
        } else if (cmd.type == Command::GetParametersInfo) {
          std::cout << InputDataValidation::getParametersInfo(
            sequenceHandler_.getSequence().front().getRawData().getParameters()) << "\n";
        } else if (cmd.type == Command::GetTraMLInfo) {
          std::cout << InputDataValidation::getTraMLInfo(
            sequenceHandler_.getSequence().front().getRawData()) << "\n";
        } else if (cmd.type == Command::GetFeatureFilterInfo) {
          std::cout << InputDataValidation::getComponentsAndGroupsInfo(
            sequenceHandler_.getSequence().front().getRawData(), true) << "\n";
        } else if (cmd.type == Command::GetFeatureQCInfo) {
          std::cout << InputDataValidation::getComponentsAndGroupsInfo(
            sequenceHandler_.getSequence().front().getRawData(), false) << "\n";
        } else if (cmd.type == Command::GetQuantMethodsInfo) {
          std::cout << InputDataValidation::getQuantitationMethodsInfo(
            sequenceHandler_.getSequenceSegments().front()) << "\n";
        } else if (cmd.type == Command::GetStandardsConcInfo) {
          std::cout << InputDataValidation::getStandardsConcentrationsInfo(
            sequenceHandler_.getSequenceSegments().front()) << "\n";
        } else {
          std::cout << "\ninvalid command\n";
        }
        i = j;
      }
      break;
    }
    default:
      std::cout << "\nInvalid input. Try again.\n";
    }
  }
};

int main(int argc, char **argv)
{
  CommandLine cli(argc, argv);
  return 0;
}

