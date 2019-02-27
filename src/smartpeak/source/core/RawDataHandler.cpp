/**TODO:  Add copyright*/

#include <SmartPeak/core/RawDataHandler.h>
#include <ctime> // time format
#include <chrono> // current time

namespace SmartPeak
{
  void RawDataHandler::setFeatureMap(const OpenMS::FeatureMap& feature_map)
  {
    feature_map_ = feature_map;
    if (feature_map_history_ != feature_map_) {
      updateFeatureMapHistory();
    }
    else {
      setFeatureMapHistory(feature_map);
    }
  }

  OpenMS::FeatureMap& RawDataHandler::getFeatureMap()
  {
    return feature_map_;
  }

  const OpenMS::FeatureMap& RawDataHandler::getFeatureMap() const
  {
    return feature_map_;
  }

  void RawDataHandler::setMetaData(const MetaDataHandler& meta_data)
  {
    meta_data_ = meta_data;
  }

  MetaDataHandler& RawDataHandler::getMetaData()
  {
    return meta_data_;
  }

  const MetaDataHandler& RawDataHandler::getMetaData() const
  {
    return meta_data_;
  }

  void RawDataHandler::setParameters(
    const std::map<std::string, std::vector<std::map<std::string, std::string>>>& parameters)
  {
    parameters_ = parameters;
  }

  std::map<std::string, std::vector<std::map<std::string, std::string>>>& RawDataHandler::getParameters()
  {
    return parameters_;
  }

  const std::map<std::string, std::vector<std::map<std::string, std::string>>>& RawDataHandler::getParameters() const
  {
    return parameters_;
  }

  void RawDataHandler::setTargetedExperiment(const OpenMS::TargetedExperiment& targeted_exp)
  {
    targeted_exp_ = targeted_exp;
  }

  OpenMS::TargetedExperiment& RawDataHandler::getTargetedExperiment()
  {
    return targeted_exp_;
  }

  const OpenMS::TargetedExperiment& RawDataHandler::getTargetedExperiment() const
  {
    return targeted_exp_;
  }

  void RawDataHandler::setReferenceData(const std::vector<std::map<std::string, Utilities::CastValue>>& reference_data)
  {
    reference_data_ = reference_data;
  }

  std::vector<std::map<std::string, Utilities::CastValue>>& RawDataHandler::getReferenceData()
  {
    return reference_data_;
  }

  const std::vector<std::map<std::string, Utilities::CastValue>>& RawDataHandler::getReferenceData() const
  {
    return reference_data_;
  }

  void RawDataHandler::setQuantitationMethods(const std::vector<OpenMS::AbsoluteQuantitationMethod>& quantitation_methods)
  {
    quantitation_methods_ = quantitation_methods;
  }

  std::vector<OpenMS::AbsoluteQuantitationMethod>& RawDataHandler::getQuantitationMethods()
  {
    return quantitation_methods_;
  }

  const std::vector<OpenMS::AbsoluteQuantitationMethod>& RawDataHandler::getQuantitationMethods() const
  {
    return quantitation_methods_;
  }

  void RawDataHandler::setFeatureFilter(const OpenMS::MRMFeatureQC& feature_filter)
  {
    feature_filter_ = feature_filter;
  }

  OpenMS::MRMFeatureQC& RawDataHandler::getFeatureFilter()
  {
    return feature_filter_;
  }

  const OpenMS::MRMFeatureQC& RawDataHandler::getFeatureFilter() const
  {
    return feature_filter_;
  }

  void RawDataHandler::setFeatureQC(const OpenMS::MRMFeatureQC& feature_qc)
  {
    feature_qc_ = feature_qc;
  }

  OpenMS::MRMFeatureQC& RawDataHandler::getFeatureQC()
  {
    return feature_qc_;
  }

  const OpenMS::MRMFeatureQC& RawDataHandler::getFeatureQC() const
  {
    return feature_qc_;
  }

  void RawDataHandler::setFeatureMapHistory(const OpenMS::FeatureMap& feature_map_history)
  {
    feature_map_history_ = feature_map_history;
  }

  OpenMS::FeatureMap& RawDataHandler::getFeatureMapHistory()
  {
    return feature_map_history_;
  }

  const OpenMS::FeatureMap& RawDataHandler::getFeatureMapHistory() const
  {
    return feature_map_history_;
  }

  void RawDataHandler::setExperiment(const OpenMS::MSExperiment& experiment)
  {
    experiment_ = experiment;
  }

  OpenMS::MSExperiment& RawDataHandler::getExperiment()
  {
    return experiment_;
  }

  const OpenMS::MSExperiment& RawDataHandler::getExperiment() const
  {
    return experiment_;
  }

  void RawDataHandler::setChromatogramMap(const OpenMS::MSExperiment& chromatogram_map)
  {
    chromatogram_map_ = chromatogram_map;
  }

  OpenMS::MSExperiment& RawDataHandler::getChromatogramMap()
  {
    return chromatogram_map_;
  }

  const OpenMS::MSExperiment& RawDataHandler::getChromatogramMap() const
  {
    return chromatogram_map_;
  }

  void RawDataHandler::setTransformationDescription(const OpenMS::TransformationDescription& trafo)
  {
    trafo_ = trafo;
  }

  OpenMS::TransformationDescription& RawDataHandler::getTransformationDescription()
  {
    return trafo_;
  }

  const OpenMS::TransformationDescription& RawDataHandler::getTransformationDescription() const
  {
    return trafo_;
  }

  void RawDataHandler::setSWATH(const OpenMS::MSExperiment& swath)
  {
    swath_ = swath;
  }

  OpenMS::MSExperiment& RawDataHandler::getSWATH()
  {
    return swath_;
  }

  const OpenMS::MSExperiment& RawDataHandler::getSWATH() const
  {
    return swath_;
  }

  void RawDataHandler::setValidationMetrics(const std::map<std::string, float>& validation_metrics)
  {
    validation_metrics_ = validation_metrics;
  }

  std::map<std::string, float>& RawDataHandler::getValidationMetrics()
  {
    return validation_metrics_;
  }

  const std::map<std::string, float>& RawDataHandler::getValidationMetrics() const
  {
    return validation_metrics_;
  }

  void RawDataHandler::clear()
  {
    experiment_.clear(true);
    chromatogram_map_.clear(true);
    trafo_ = OpenMS::TransformationDescription();
    swath_.clear(true);
    feature_map_.clear(true);
    feature_map_history_.clear(true);
    meta_data_.clear();
    validation_metrics_.clear();
    parameters_.clear();
    targeted_exp_.clear(true);
    quantitation_methods_.clear();
    feature_filter_ = OpenMS::MRMFeatureQC();
    feature_qc_ = OpenMS::MRMFeatureQC();
  }

  void RawDataHandler::updateFeatureMapHistory()
  {
    // Current time stamp
    std::chrono::time_point<std::chrono::system_clock> time_now = std::chrono::system_clock::now();
    std::time_t time_now_t = std::chrono::system_clock::to_time_t(time_now);
    std::tm now_tm = *std::localtime(&time_now_t);
    char timestamp_char[64];
    std::strftime(timestamp_char, 64, "%Y-%m-%d-%H-%M-%S", &now_tm);
    std::string timestamp(timestamp_char);

    // Get all feature IDs
    std::set<OpenMS::UInt> unique_ids;
    for (const OpenMS::Feature& feature_copy : feature_map_history_) { unique_ids.insert(feature_copy.getUniqueId()); }

    // "Remove" filtered/non-selected features and "Add" new features via "used_" and "modified" feature metadata attributes
    std::vector<OpenMS::Feature> new_features;
    for (OpenMS::Feature& feature_copy : feature_map_history_) {
      for (const OpenMS::Feature& feature_select : feature_map_) {
        if (feature_select.getUniqueId() == feature_copy.getUniqueId() && feature_select.getMetaValue("PeptideRef") == feature_copy.getMetaValue("PeptideRef")) { // Matching feature
          std::vector<OpenMS::Feature> new_subordinates;
          for (OpenMS::Feature& subordinate_copy : feature_copy.getSubordinates()) {
            for (const OpenMS::Feature& subordinate_select : feature_select.getSubordinates()) {
              if (subordinate_select.getUniqueId() == subordinate_copy.getUniqueId() && subordinate_select.getMetaValue("native_id") == subordinate_copy.getMetaValue("native_id")) { // Matching subordinate
                subordinate_copy.setMetaValue("used_", "true");
                subordinate_copy.setMetaValue("modified_", timestamp);
              }
              else if (subordinate_select.getMetaValue("native_id") == subordinate_copy.getMetaValue("native_id")) { // Removed subordinate
                subordinate_copy.setMetaValue("used_", "false");
                subordinate_copy.setMetaValue("modified_", timestamp);
              }
              else { // New subordinate
                OpenMS::Feature new_subordinate = subordinate_select;
                new_subordinate.setMetaValue("used_", "true");
                new_subordinate.setMetaValue("modified_", timestamp);
                new_subordinates.push_back(new_subordinate);
              }
            }
          }
          if (new_subordinates.size() > 0) {
            for (OpenMS::Feature& subordinate_copy : feature_copy.getSubordinates()) {
              new_subordinates.push_back(subordinate_copy);
            }
            feature_copy.setSubordinates(new_subordinates);
          }
          break;
        }
        else if ( unique_ids.count(feature_select.getUniqueId()) == 0) { // New Feature
          OpenMS::Feature new_feature = feature_select;
          for (OpenMS::Feature& subordinate_new : new_feature.getSubordinates()) {
            subordinate_new.setMetaValue("used_", "true");
            subordinate_new.setMetaValue("modified_", timestamp);
          }
          new_features.push_back(new_feature);
          break;
        }
      }
    }

    // Add in the new features to the feature history
    if (new_features.size() > 0) {
      for (OpenMS::Feature& feature_new : new_features) {
        feature_map_history_.push_back(feature_new);
      }
    }
  }
}
