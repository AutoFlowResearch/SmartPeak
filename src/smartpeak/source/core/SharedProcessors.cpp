#include <SmartPeak/core/SharedProcessors.h>

#include <SmartPeak/core/RawDataProcessor.h>
#include <SmartPeak/core/SequenceSegmentProcessor.h>
#include <map>
#include <memory>

namespace SmartPeak {
  const std::map<int, std::shared_ptr<RawDataProcessor>> n_to_raw_data_method_ {
    {1, std::make_shared<LoadRawData>()},
    {2, std::make_shared<LoadFeatures>()},
    {3, std::make_shared<PickFeatures>()},
    {4, std::make_shared<FilterFeatures>()},
    {5, std::make_shared<SelectFeatures>()},
    {6, std::make_shared<ValidateFeatures>()},
    {7, std::make_shared<QuantifyFeatures>()},
    {8, std::make_shared<CheckFeatures>()},
    {9, std::make_shared<StoreFeatures>()},
    {10, std::make_shared<PlotFeatures>()},
    {11, std::make_shared<MapChromatograms>()},
    {12, std::make_shared<ZeroChromatogramBaseline>()},
    {13, std::make_shared<ExtractChromatogramWindows>()},
    {14, std::make_shared<EMGProcessor>()},
    {4, std::make_shared<FilterFeaturesRSDs>()},
    {8, std::make_shared<CheckFeaturesRSDs>()},
    {4, std::make_shared<FilterFeaturesBackgroundInterferences>()},
    {8, std::make_shared<CheckFeaturesBackgroundInterferences>()}
  };
  const std::map<int, std::shared_ptr<SequenceSegmentProcessor>> n_to_seq_seg_method_ {
    {15, std::make_shared<CalculateCalibration>()},
    {16, std::make_shared<StoreQuantitationMethods>()},
    {17, std::make_shared<LoadQuantitationMethods>()},
    {15, std::make_shared<EstimateFeatureFilterValues>()},
    {15, std::make_shared<EstimateFeatureQCValues>()},
    {15, std::make_shared<TransferLOQToFeatureFilters>()},
    {15, std::make_shared<TransferLOQToFeatureQCs>()},
    {15, std::make_shared<EstimateFeatureRSDs>()},
    {15, std::make_shared<EstimateFeatureBackgroundInterferences>()}
  };
}
