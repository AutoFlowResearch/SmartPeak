// TODO: Add copyright

#pragma once
#include <OpenMS/KERNEL/FeatureMap.h>
#include <SmartPeak/core/Utilities.h>

namespace SmartPeak
{
  class MRMFeatureValidator
  {
public:
    MRMFeatureValidator() = delete;
    ~MRMFeatureValidator() = delete;

    static void validate_MRMFeatures(
      const std::vector<std::map<std::string, Utilities::CastValue>>& reference_data_v,
      const OpenMS::FeatureMap& features,
      const std::string& sample_name,
      OpenMS::FeatureMap& output_validated,
      std::map<std::string, float>& validation_metrics,
      const float Tr_window = 1.0,
      const bool verbose_I = false
    );
  };
}
