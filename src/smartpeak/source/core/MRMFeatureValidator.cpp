// TODO: Add copyright

#include <SmartPeak/core/MRMFeatureValidator.h>

namespace SmartPeak
{
  void MRMFeatureValidator::validate_MRMFeatures(
    const std::map<std::string, float>& reference_data,
    const OpenMS::FeatureMap& features,
    OpenMS::FeatureMap& output_filtered,
    std::map<std::string, float>& validation_metrics,
    const float Tr_window
  )
  {
    std::vector<float> y_true;
    std::vector<float> y_pred;
    output_filtered.clear(true);

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
        const float reference_rt = reference_data.at(reference_data_key);
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
          y_pred.push_back(1.0);
          y_true.push_back(1.0);
        } else {
          subordinate_copy.setMetaValue("validation", "FP");
          subordinates_tmp.push_back(subordinate_copy);
          y_pred.push_back(0.0);
          y_true.push_back(1.0);
        }
      }
      if (subordinates_tmp.empty())
        continue;
      OpenMS::Feature feature_tmp = feature;
      feature_tmp.setSubordinates(subordinates_tmp);
      output_filtered.push_back(feature_tmp);
    }

    validation_metrics = calculate_validationMetrics(y_true, y_pred, true);
  }

  std::map<std::string, float> MRMFeatureValidator::calculate_validationMetrics(
    const std::vector<float>& y_true,
    const std::vector<float>& y_pred,
    const bool verbose_I
  )
  {
    const std::array<size_t, 4> conf = computeConfusionMatrix(y_true, y_pred, true); // [0,1,2,3] = tp, fp, fn, tn
    const float accuracy = (conf[0] + conf[3]) / static_cast<float>(conf[0] + conf[1] + conf[2] + conf[3]);
    const float recall = conf[0] / static_cast<float>(conf[0] + conf[2]);
    const float precision = conf[0] / static_cast<float>(conf[0] + conf[1]);

    return {
      {"accuracy", accuracy},
      {"recall", recall},
      {"precision", precision},
    };
  }

  std::array<size_t, 4> MRMFeatureValidator::computeConfusionMatrix(
    const std::vector<float>& y_true,
    const std::vector<float>& y_pred,
    const bool verbose_I
  )
  {
    if (y_true.size() != y_pred.size())
      throw std::invalid_argument("Sizes don't match.");

    std::array<size_t, 4> conf = {0, 0, 0, 0}; // tp, fp, fn, tn
    std::vector<float>::const_iterator p = y_pred.cbegin(); // a = actual
    std::vector<float>::const_iterator a = y_true.cbegin(); // p = predicted

    for (; p != y_pred.cend(); ++p, ++a) {
      if (*p == 1.0) {      // positives
        if (*a == 1.0)
          ++conf[0];
        else
          ++conf[1];
      }
      else {               // negatives
        if (*a == 1.0)
          ++conf[2];
        else
          ++conf[3];
      }
    }

    if (verbose_I)
      std::cout << "Confusion matrix: [TP, FP, FN, TN] = [" << conf[0] << ", " << conf[1] << ", " << conf[2] << ", " << conf[3] << "]" << std::endl;

    return conf;
  }
// TODO: the code should maybe account for machine floating error. use integer data, instead?
}
