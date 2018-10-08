// TODO: Add copyright

#include <SmartPeak/core/MRMFeatureValidator.h>
#include <SmartPeak/core/Utilities.h>

namespace SmartPeak
{
  void MRMFeatureValidator::validate_MRMFeatures(
    const std::vector<std::map<std::string, Utilities::CastValue>>& reference_data_v,
    const OpenMS::FeatureMap& features,
    const std::string& sample_name,
    OpenMS::FeatureMap& output_validated,
    std::map<std::string, float>& validation_metrics,
    const float Tr_window,
    const bool verbose_I
  )
  {
    std::vector<int> y_true;
    std::vector<int> y_pred;
    output_validated.clear(true);

    // TODO: need to add in the experiment_id and acquisition_method_id
    // to feature or as a parameter
    // reference_data_dict = {
    // (d['experiment_id'],d['acquisition_method_id'],
    // d['quantitation_method_id'],d['sample_name'],d['component_name']):d
    // for d in reference_data}

    std::map<std::string, std::map<std::string, Utilities::CastValue>> reference_data;
    for (const std::map<std::string, Utilities::CastValue>& m : reference_data_v) {
      if (m.at("sample_name").s_ != sample_name)
        continue;
      const std::string& name = m.at("component_name").s_;
      reference_data[name] = m;
    }

    for (const OpenMS::Feature& feature : features) {
      std::vector<OpenMS::Feature> subordinates_tmp;
      for (const OpenMS::Feature& subordinate : feature.getSubordinates()) {
        bool fc_pass = false;
        if (!subordinate.metaValueExists("native_id"))
          throw "native_id info is missing.";
        const std::string reference_data_key = subordinate.getMetaValue("native_id").toString();
        OpenMS::Feature subordinate_copy = subordinate;

        if (0 == reference_data.count(reference_data_key)) {
          subordinate_copy.setMetaValue("validation", "ND");
          subordinates_tmp.push_back(subordinate_copy);
          continue;
        }

        const float reference_rt = reference_data[reference_data_key]["retention_time"].f_;

        if (0.0 == reference_rt) {
          continue;
        }
        const float feature_rt = feature.getRT();
        const float feature_leftWidth = static_cast<float>(feature.getMetaValue("leftWidth"));
        const float feature_rightWidth = static_cast<float>(feature.getMetaValue("rightWidth"));
        if (std::fabs(reference_rt - feature_rt) < Tr_window)
          fc_pass = true;
        if (reference_rt > feature_leftWidth && reference_rt < feature_rightWidth)
          fc_pass = true;
        if (fc_pass) {
          subordinate_copy.setMetaValue("validation", "TP");
          subordinates_tmp.push_back(subordinate_copy);
          y_pred.push_back(1);
          y_true.push_back(1);
        } else {
          subordinate_copy.setMetaValue("validation", "FP");
          subordinates_tmp.push_back(subordinate_copy);
          y_pred.push_back(1);
          y_true.push_back(0);
        }
      }
      if (subordinates_tmp.empty())
        continue;
      OpenMS::Feature feature_tmp = feature;
      feature_tmp.setSubordinates(subordinates_tmp);
      output_validated.push_back(feature_tmp);
    }

    validation_metrics = Utilities::calculateValidationMetrics(y_true, y_pred, verbose_I);
  }
}
