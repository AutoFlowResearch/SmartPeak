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
    integration_right_boundary
  };

  // Returns the string representation that a FeatureMetadata would have in OpenMS
  static const std::map<FeatureMetadata, std::string> metadataToString {
    { FeatureMetadata::asymmetry_factor,            "asymmetry_factor"            },
    { FeatureMetadata::baseline_delta_to_height,    "baseline_delta_2_height"     },
    { FeatureMetadata::calculated_concentration,    "calculated_concentration"    },
    { FeatureMetadata::log_signal_to_noise,         "logSN"                       },
    { FeatureMetadata::peak_apex_intensity,         "peak_apex_int"               },
    { FeatureMetadata::peak_area,                   "peak_area"                   },
    { FeatureMetadata::points_across_baseline,      "points_across_baseline"      },
    { FeatureMetadata::points_across_half_height,   "points_across_half_height"   },
    { FeatureMetadata::qc_transition_pass,          "QC_transition_pass"          },
    { FeatureMetadata::qc_transition_message,       "QC_transition_message"       },
    { FeatureMetadata::qc_transition_score,         "QC_transition_score"         },
    { FeatureMetadata::qc_transition_group_pass,    "QC_transition_group_pass"    },
    { FeatureMetadata::qc_transition_group_message, "QC_transition_group_message" },
    { FeatureMetadata::qc_transition_group_score,   "QC_transition_group_score"   },
    { FeatureMetadata::tailing_factor,              "tailing_factor"              },
    { FeatureMetadata::total_width,                 "total_width"                 },
    { FeatureMetadata::width_at_50_peak_height,     "width_at_50"                 },
    { FeatureMetadata::retention_time,              "RT"                          },
    { FeatureMetadata::integration_left_boundary,   "leftWidth"                   },
    { FeatureMetadata::integration_right_boundary,  "rightWidth"                  }
  };
}
