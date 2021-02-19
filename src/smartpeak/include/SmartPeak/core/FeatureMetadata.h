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
    scan_polarity,
    description,
    modifications, 
    chemical_formula,
    mz,
    charge,
    mz_error_ppm,
    mz_error_Da,
    average_accuracy,
    absolute_difference/*,
    accuracy,
    n_features,
    validation*/
  };

  // Returns the string representation that a FeatureMetadata would have in OpenMS
  extern const std::map<FeatureMetadata, std::string> metadataToString;
  extern const std::map<FeatureMetadata, std::string> metadatafloatToString;

  constexpr size_t FeatureMetadataSize { 28 }; // was 31 with accuracy, n_features, and validation
}
