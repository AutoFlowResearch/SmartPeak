#pragma once

#include <map>
#include <string>

namespace SmartPeak {
  enum class Metadata {
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

  // Returns the string representation that a Metadata would have in OpenMS
  static const std::map<Metadata, std::string> metadataToString {
    { Metadata::asymmetry_factor,            "asymmetry_factor"            },
    { Metadata::baseline_delta_to_height,    "baseline_delta_2_height"     },
    { Metadata::calculated_concentration,    "calculated_concentration"    },
    { Metadata::log_signal_to_noise,         "logSN"                       },
    { Metadata::peak_apex_intensity,         "peak_apex_int"               },
    { Metadata::peak_area,                   "peak_area"                   },
    { Metadata::points_across_baseline,      "points_across_baseline"      },
    { Metadata::points_across_half_height,   "points_across_half_height"   },
    { Metadata::qc_transition_pass,          "QC_transition_pass"          },
    { Metadata::qc_transition_message,       "QC_transition_message"       },
    { Metadata::qc_transition_score,         "QC_transition_score"         },
    { Metadata::qc_transition_group_pass,    "QC_transition_group_pass"    },
    { Metadata::qc_transition_group_message, "QC_transition_group_message" },
    { Metadata::qc_transition_group_score,   "QC_transition_group_score"   },
    { Metadata::tailing_factor,              "tailing_factor"              },
    { Metadata::total_width,                 "total_width"                 },
    { Metadata::width_at_50_peak_height,     "width_at_50"                 },
    { Metadata::retention_time,              "RT"                          },
    { Metadata::integration_left_boundary,   "leftWidth"                   },
    { Metadata::integration_right_boundary,  "rightWidth"                  }
  };
}
