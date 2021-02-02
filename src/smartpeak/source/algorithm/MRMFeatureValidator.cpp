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

#include <SmartPeak/algorithm/MRMFeatureValidator.h>
#include <SmartPeak/core/Utilities.h>

namespace SmartPeak
{
  void MRMFeatureValidator::validate_MRMFeatures(
    const std::vector<std::map<std::string, CastValue>>& reference_data_v,
    const OpenMS::FeatureMap& features,
    const std::string& injection_name,
    OpenMS::FeatureMap& output_validated,
    std::map<std::string, float>& validation_metrics,
    const float Tr_window
  )
  {
    std::vector<int> y_true;
    std::vector<int> y_pred;
    output_validated.clear(true);

    std::map<std::string, std::map<std::string, CastValue>> reference_data;
    for (const std::map<std::string, CastValue>& m : reference_data_v) {
      if (m.at("injection_name").s_ != injection_name) {
        continue;
      }
      const std::string& name = m.at("component_name").s_;
      reference_data[name] = m;
    }

    for (const OpenMS::Feature& feature : features) {
      std::vector<OpenMS::Feature> subordinates_tmp;
      for (const OpenMS::Feature& subordinate : feature.getSubordinates()) {
	      if (subordinate.metaValueExists("used_")) {
          const std::string used = subordinate.getMetaValue("used_").toString();
            if (used.empty() || used[0] == 'f' || used[0] == 'F')
              continue;
        }
        bool fc_pass = false;
        if (!subordinate.metaValueExists("native_id")) {
          throw "native_id info is missing.";
        }
        const std::string reference_data_key = subordinate.getMetaValue("native_id").toString();
        OpenMS::Feature subordinate_copy = subordinate;

        if (0 == reference_data.count(reference_data_key)) {
          subordinate_copy.setMetaValue("validation", "ND");
          subordinates_tmp.push_back(subordinate_copy);
          continue;
        }

        // extract and format rt information
        const float reference_rt = reference_data[reference_data_key]["retention_time"].f_;

        if (0.0 == reference_rt) {
          continue;
        }
        const float feature_rt = feature.getRT();
        const float feature_leftWidth = static_cast<float>(feature.getMetaValue("leftWidth"));
        const float feature_rightWidth = static_cast<float>(feature.getMetaValue("rightWidth"));
        // validate the retention time
        if (std::fabs(reference_rt - feature_rt) < Tr_window ||
            (reference_rt > feature_leftWidth &&
             reference_rt < feature_rightWidth)) {
          fc_pass = true;
        }
        // other validation parameters
        // NOTE: if there are no other validation parameters that are
        // transition-specific, there is no need to loop
        // through each transition
        if (fc_pass) { // True Positive
          subordinate_copy.setMetaValue("validation", "TP");
          subordinates_tmp.push_back(subordinate_copy);
          y_pred.push_back(1);
          y_true.push_back(1);
        } else {       // False Positive
          subordinate_copy.setMetaValue("validation", "FP");
          subordinates_tmp.push_back(subordinate_copy);
          y_pred.push_back(1);
          y_true.push_back(0);
        }
      }
      // check that subordinates were found
      if (subordinates_tmp.empty()) {
        continue;
      }
      // copy out all feature values
      OpenMS::Feature feature_tmp = feature;
      feature_tmp.setSubordinates(subordinates_tmp);
      output_validated.push_back(feature_tmp);
    }

    validation_metrics = Utilities::calculateValidationMetrics(y_true, y_pred);
  }
}
