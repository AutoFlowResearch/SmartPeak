// TODO: Add copyright

#pragma once
#include <OpenMS/KERNEL/FeatureMap.h>
#include <SmartPeak/core/Utilities.h>

namespace SmartPeak
{
  /// MRMFeatureFilter performs validation on features (FeatureMap)
  class MRMFeatureValidator
  {
public:
    MRMFeatureValidator()                                      = delete;
    ~MRMFeatureValidator()                                     = delete;
    MRMFeatureValidator(const MRMFeatureValidator&)            = delete;
    MRMFeatureValidator& operator=(const MRMFeatureValidator&) = delete;
    MRMFeatureValidator(MRMFeatureValidator&&)                 = delete;
    MRMFeatureValidator& operator=(MRMFeatureValidator&&)      = delete;

    /**
      Map reference data to FeatureMap.

      @note Potential code optimizations:
      - identify True Negatives and False Negatives (can be problematic due to
        selection of peaks not in the quantification target list...)
      - add in plots and other visualizations

      @param[in] reference_data_v Reference data
      @param[in] features Features
      @param[in] injection_name Injection name
      @param[out] output_validated Validated features
      @param[out] validation_metrics Validation metrics
      @param[in] Tr_window Retention time difference threshold
    */
    static void validate_MRMFeatures(
      const std::vector<std::map<std::string, Utilities::CastValue>>& reference_data_v,
      const OpenMS::FeatureMap& features,
      const std::string& injection_name,
      OpenMS::FeatureMap& output_validated,
      std::map<std::string, float>& validation_metrics,
      const float Tr_window = 1.0
    );
  };
}
