#pragma once

#include <SmartPeak/core/Filenames.h>
#include <SmartPeak/core/RawDataProcessor.h>
#include <SmartPeak/core/SequenceSegmentProcessor.h>
#include <SmartPeak/io/InputDataValidation.h>
#include <map>
#include <string>
#include <vector>

namespace SmartPeak
{
  struct AppState final {
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

      std::string getName() const
      {
        return type == RawDataMethod ? raw_data_method->getName() : seq_seg_method->getName();
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

    const std::map<int, std::shared_ptr<RawDataProcessor>> n_to_raw_data_method_ {
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
    const std::map<int, std::shared_ptr<SequenceSegmentProcessor>> n_to_seq_seg_method_ {
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
  };
}
