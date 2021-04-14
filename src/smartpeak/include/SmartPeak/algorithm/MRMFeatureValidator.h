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
#include <OpenMS/KERNEL/FeatureMap.h>
#include <SmartPeak/core/CastValue.h>

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
      Please note that in order to align the retention times properly the units
      should match in the featureMap, reference data and the Tr_window parameter.
      The default units for OpenMS are seconds.

      @note Potential code optimizations:
      - identify True Negatives and False Negatives (can be problematic due to
        selection of peaks not in the quantification target list...)
      - add in plots and other visualizations

      @param[in] reference_data_v Reference data
      @param[in] features Features
      @param[in] injection_name Injection name
      @param[out] output_validated Validated features
      @param[out] validation_metrics Validation metrics
      @param[in] Tr_window Retention time difference threshold (in seconds)
    */
    static void validate_MRMFeatures(
      const std::vector<std::map<std::string, CastValue>>& reference_data_v,
      const OpenMS::FeatureMap& features,
      const std::string& injection_name,
      OpenMS::FeatureMap& output_validated,
      std::map<std::string, float>& validation_metrics,
      const float Tr_window = 1.0
    );
  };
}
