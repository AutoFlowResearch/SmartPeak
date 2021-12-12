// --------------------------------------------------------------------------
//   SmartPeak -- Fast and Accurate CE-, GC- and LC-MS(/MS) Data Processing
// --------------------------------------------------------------------------
// Copyright The SmartPeak Team -- Novo Nordisk Foundation 
// Center for Biosustainability, Technical University of Denmark 2018-2021.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL ANY OF THE AUTHORS OR THE CONTRIBUTING
// INSTITUTIONS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
// OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
// WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
// OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
// ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// --------------------------------------------------------------------------
// $Maintainer: Douglas McCloskey $
// $Authors: Douglas McCloskey, Pasquale Domenico Colaianni $
// --------------------------------------------------------------------------

#include <SmartPeak/core/FeatureMetadata.h>
#include <map>
#include <string>

namespace SmartPeak {
  const std::map<FeatureMetadata, std::string> metadataToString {
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
    { FeatureMetadata::integration_right_boundary,  "rightWidth"                  },
    { FeatureMetadata::scan_polarity,               "scan_polarity"               },
    { FeatureMetadata::description,                 "description"                 },
    { FeatureMetadata::modifications,               "modifications"               },
    { FeatureMetadata::chemical_formula,            "chemical_formula"            },
    { FeatureMetadata::mz,                          "mz"                          },
    { FeatureMetadata::charge,                      "charge"                      },
    { FeatureMetadata::mz_error_ppm,                "mz_error_ppm"                },
    { FeatureMetadata::mz_error_Da,                 "mz_error_Da"                 },
    { FeatureMetadata::average_accuracy,            "average_accuracy"            },
    { FeatureMetadata::absolute_difference,         "absolute_difference"         }/*,
    { FeatureMetadata::accuracy,                    "accuracy"                    },
    { FeatureMetadata::n_features,                  "n_features"                  },
    { FeatureMetadata::validation,                  "validation"                  }*/
  };
  const std::map<FeatureMetadata, std::string> metadataFloatToString{
    { FeatureMetadata::asymmetry_factor,            "asymmetry_factor"            },
    { FeatureMetadata::baseline_delta_to_height,    "baseline_delta_2_height"     },
    { FeatureMetadata::calculated_concentration,    "calculated_concentration"    },
    { FeatureMetadata::log_signal_to_noise,         "logSN"                       },
    { FeatureMetadata::peak_apex_intensity,         "peak_apex_int"               },
    { FeatureMetadata::peak_area,                   "peak_area"                   },
    { FeatureMetadata::points_across_baseline,      "points_across_baseline"      },
    { FeatureMetadata::points_across_half_height,   "points_across_half_height"   },
    { FeatureMetadata::qc_transition_score,         "QC_transition_score"         },
    { FeatureMetadata::qc_transition_group_score,   "QC_transition_group_score"   },
    { FeatureMetadata::tailing_factor,              "tailing_factor"              },
    { FeatureMetadata::total_width,                 "total_width"                 },
    { FeatureMetadata::width_at_50_peak_height,     "width_at_50"                 },
    { FeatureMetadata::retention_time,              "RT"                          },
    { FeatureMetadata::integration_left_boundary,   "leftWidth"                   },
    { FeatureMetadata::integration_right_boundary,  "rightWidth"                  },
    { FeatureMetadata::mz,                          "mz"                          },
    { FeatureMetadata::charge,                      "charge"                      },
    { FeatureMetadata::mz_error_ppm,                "mz_error_ppm"                },
    { FeatureMetadata::mz_error_Da,                 "mz_error_Da"                 },
    { FeatureMetadata::average_accuracy,            "average_accuracy"            },
    { FeatureMetadata::absolute_difference,         "absolute_difference"         }/*,
    { FeatureMetadata::accuracy,                    "accuracy"                    },
    { FeatureMetadata::n_features,                  "n_features"                  },
    { FeatureMetadata::validation,                  "validation"                  }*/
  };
}
