#include <SmartPeak/core/Filenames.h>
#include <SmartPeak/core/RawDataProcessor.h>
#include <SmartPeak/core/SequenceSegmentProcessor.h>
#include <SmartPeak/io/InputDataValidation.h>
#include <string>
#include <unordered_map>
#include <vector>

namespace SmartPeak
{
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

    bool buildStaticFilenames();

    bool requiredPathnamesAreValid(const std::vector<InputDataValidation::FilenameInfo>& validation);

    void clearNonExistantDefaultGeneratedFilenames(Filenames& f);

    void clearNonExistantFilename(std::string& filename);

    void generatePathnamesTxt(
      const std::string& pathname,
      const Filenames& f,
      const std::vector<InputDataValidation::FilenameInfo>& is_valid
    );

    std::string getValidPathnameOrPlaceholder(const std::string& pathname, const bool is_valid);

    void updateFilenames(Filenames& f, const std::string& pathname);

    std::vector<Command> getMethodsInput();

    void setSequencePathnameFromInput();

    std::string getLineInput(const std::string& message = "", const bool canBeEmpty = true);

    std::string getPathnameFromInput();

    std::set<MetaDataHandler::SampleType> getSampleTypesInput();

    std::vector<std::string> getMetaDataInput(const std::string& title);

    std::string gettingStartedString();

    std::string commandsString();

    void processCommands(const std::vector<Command>& commands);

    bool createCommand(const int n, Command& cmd);

    void initializeDataDirs();

    CommandLine()                                    = default;
    ~CommandLine()                                   = default;
    CommandLine(const CommandLine& other)            = delete;
    CommandLine& operator=(const CommandLine& other) = delete;
    CommandLine(CommandLine&& other)                 = delete;
    CommandLine& operator=(CommandLine&& other)      = delete;

    void runApp();

    std::string getPipelineString();
  };
}
