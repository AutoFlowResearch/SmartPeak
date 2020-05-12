#include <SmartPeak/core/SharedProcessors.h>

#include <SmartPeak/core/RawDataProcessor.h>
#include <SmartPeak/core/SequenceSegmentProcessor.h>
#include <map>
#include <memory>

namespace SmartPeak {
  // TODO: refactor RawDataProcessor and SequenceSegmentProcess to statically declare `getName()`
  //       to avoid duplicaiton of text while still not needing to instantiate each object
  const std::map<std::string, std::shared_ptr<RawDataProcessor>> n_to_raw_data_method_ {
    {"LOAD_RAW_DATA", std::make_shared<LoadRawData>()},
    {"LOAD_FEATURES", std::make_shared<LoadFeatures>()},
    {"PICK_FEATURES", std::make_shared<PickFeatures>()},
    {"FILTER_FEATURES", std::make_shared<FilterFeatures>()},
    {"SELECT_FEATURES", std::make_shared<SelectFeatures>()},
    {"VALIDATE_FEATURES", std::make_shared<ValidateFeatures>()},
    {"QUANTIFY_FEATURES", std::make_shared<QuantifyFeatures>()},
    {"CHECK_FEATURES", std::make_shared<CheckFeatures>()},
    {"STORE_FEATURES", std::make_shared<StoreFeatures>()},
    //{"PLOT_FEATURES", std::make_shared<PlotFeatures>()},
    {"MAP_CHROMATOGRAMS", std::make_shared<MapChromatograms>()},
    {"ZERO_CHROMATOGRAM_BASELINE", std::make_shared<ZeroChromatogramBaseline>()},
    {"EXTRACT_CHROMATOGRAM_WINDOWS", std::make_shared<ExtractChromatogramWindows>()},
    {"FIT_FEATURES_EMG", std::make_shared<FitFeaturesEMG>()},
    {"FILTER_FEATURES_RSDS", std::make_shared<FilterFeaturesRSDs>()},
    {"CHECK_FEATURES_RSDS", std::make_shared<CheckFeaturesRSDs>()},
    {"FILTER_FEATURES_BACKGROUND_INTERFERENCES", std::make_shared<FilterFeaturesBackgroundInterferences>()},
    {"FILTER_FEATURES_BACKGROUND_INTERFERENCES", std::make_shared<CheckFeaturesBackgroundInterferences>()}
  };
  const std::map<std::string, std::shared_ptr<SequenceSegmentProcessor>> n_to_seq_seg_method_ {
    {"CALCULATE_CALIBRATION", std::make_shared<CalculateCalibration>()},
    {"STORE_QUANTITATION_METHODS", std::make_shared<StoreQuantitationMethods>()},
    {"LOAD_QUANTITATION_METHODS", std::make_shared<LoadQuantitationMethods>()},
    {"ESTIMATE_FEATURE_FILTER_VALUES", std::make_shared<EstimateFeatureFilterValues>()},
    {"ESTIMATE_FEATURE_QC_VALUES", std::make_shared<EstimateFeatureQCValues>()},
    {"TRANSFER_LOQ_TO_FEATURE_FILTERS", std::make_shared<TransferLOQToFeatureFilters>()},
    {"TRANSFER_LOQ_TO_FEATURE_QCS", std::make_shared<TransferLOQToFeatureQCs>()},
    {"ESTIMATE_FEATURE_RSDS", std::make_shared<EstimateFeatureRSDs>()},
    {"ESTIMATE_FEATURE_BACKGROUND_INTERFERENCES", std::make_shared<EstimateFeatureBackgroundInterferences>()},
    {"STORE_FEATURE_FILTERS", std::make_shared<StoreFeatureFilters>()},
    {"LOAD_FEATURE_FILTERS", std::make_shared<LoadFeatureFilters>()},
    {"STORE_FEATURE_QCS", std::make_shared<StoreFeatureQCs>()},
    {"LOAD_FEATURE_QCS", std::make_shared<LoadFeatureQCs>()},
  };
}
