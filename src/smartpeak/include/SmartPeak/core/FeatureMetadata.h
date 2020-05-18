#pragma once

#include <map>
#include <string>

namespace SmartPeak {
  enum class FeatureMetadata {
    asymmetry_factor = 1,
    baseline_delta_to_height,
    calculated_concentration,
    log_signal_to_noise,
    peak_apex_intensity,
    peak_area,
    points_across_baseline,
    points_across_half_height,
    qc_transition_pass,
    qc_transition_message,
    qc_transition_score,
    qc_transition_group_pass,
    qc_transition_group_message,
    qc_transition_group_score,
    tailing_factor,
    total_width,
    width_at_50_peak_height,
    retention_time,
    integration_left_boundary,
    integration_right_boundary,
    accuracy,
    n_features,
    validation
  };

  // Returns the string representation that a FeatureMetadata would have in OpenMS
  extern const std::map<FeatureMetadata, std::string> metadataToString;

  constexpr size_t FeatureMetadataSize { 23 };
}
