// TODO: Add copyright

#pragma once
#include <OpenMS/KERNEL/FeatureMap.h>
#include <map>

namespace SmartPeak
{
  class MRMFeatureValidator
  {
public:
    MRMFeatureValidator() = delete;
    ~MRMFeatureValidator() = delete;

    static void validate_MRMFeatures(
      const std::map<std::string, float>& reference_data,
      const OpenMS::FeatureMap& features,
      OpenMS::FeatureMap& output_filtered,
      std::map<std::string, float>& validation_metrics,
      const float Tr_window = 1.0
    );

    static std::map<std::string, float> calculate_validationMetrics(
      const std::vector<float>& y_true,
      const std::vector<float>& y_pred,
      const bool verbose_I = false
    );

    static std::array<size_t, 4> computeConfusionMatrix(
      const std::vector<float>& y_true,
      const std::vector<float>& y_pred,
      const bool verbose_I = false
    );
  };
}
