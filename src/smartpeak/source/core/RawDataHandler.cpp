// TODO: Add copyright

#include <SmartPeak/core/RawDataHandler.h>
#include <ctime> // time format
#include <chrono> // current time

namespace SmartPeak
{
  RawDataHandler::RawDataHandler() :
    meta_data_(new MetaDataHandler()),
    parameters_(new std::map<std::string, std::vector<std::map<std::string, std::string>>>()),
    targeted_exp_(new OpenMS::TargetedExperiment()),
    reference_data_(new std::vector<std::map<std::string, Utilities::CastValue>>()),
    quantitation_methods_(new std::vector<OpenMS::AbsoluteQuantitationMethod>()),
    feature_filter_(new OpenMS::MRMFeatureQC()),
    feature_qc_(new OpenMS::MRMFeatureQC())
  {
  }

  void RawDataHandler::setFeatureMap(const OpenMS::FeatureMap& feature_map)
  {
    feature_map_ = feature_map;
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
    meta_data_.reset(new MetaDataHandler(meta_data));
  }

  void RawDataHandler::setMetaData(std::shared_ptr<MetaDataHandler>& meta_data)
  {
    meta_data_ = meta_data;
  }

  MetaDataHandler& RawDataHandler::getMetaData()
  {
    return *(meta_data_.get());
  }

  const MetaDataHandler& RawDataHandler::getMetaData() const
  {
    return *(meta_data_.get());
  }

  std::shared_ptr<MetaDataHandler>& RawDataHandler::getMetaDataShared()
  {
    return meta_data_;
  }

  void RawDataHandler::setParameters(
    const std::map<std::string, std::vector<std::map<std::string, std::string>>>& parameters)
  {
    parameters_.reset(new std::map<std::string, std::vector<std::map<std::string, std::string>>>(parameters));
  }

  void RawDataHandler::setParameters(std::shared_ptr<std::map<std::string, std::vector<std::map<std::string, std::string>>>>& parameters)
  {
    parameters_ = parameters;
  }

  std::map<std::string, std::vector<std::map<std::string, std::string>>>& RawDataHandler::getParameters()
  {
    return *(parameters_.get());
  }

  const std::map<std::string, std::vector<std::map<std::string, std::string>>>& RawDataHandler::getParameters() const
  {
    return *(parameters_.get());
  }

  std::shared_ptr<std::map<std::string, std::vector<std::map<std::string, std::string>>>>& RawDataHandler::getParametersShared()
  {
    return parameters_;
  }

  void RawDataHandler::setTargetedExperiment(const OpenMS::TargetedExperiment& targeted_exp)
  {
    targeted_exp_.reset(new OpenMS::TargetedExperiment(targeted_exp));
  }

  void RawDataHandler::setTargetedExperiment(std::shared_ptr<OpenMS::TargetedExperiment>& targeted_exp)
  {
    targeted_exp_ = targeted_exp;
  }

  OpenMS::TargetedExperiment& RawDataHandler::getTargetedExperiment()
  {
    return *(targeted_exp_.get());
  }

  const OpenMS::TargetedExperiment& RawDataHandler::getTargetedExperiment() const
  {
    return *(targeted_exp_.get());
  }

  std::shared_ptr<OpenMS::TargetedExperiment>& RawDataHandler::getTargetedExperimentShared()
  {
    return targeted_exp_;
  }

  void RawDataHandler::setReferenceData(const std::vector<std::map<std::string, Utilities::CastValue>>& reference_data)
  {
    reference_data_.reset(new std::vector<std::map<std::string, Utilities::CastValue>>(reference_data));
  }

  void RawDataHandler::setReferenceData(std::shared_ptr<std::vector<std::map<std::string, Utilities::CastValue>>>& reference_data)
  {
    reference_data_ = reference_data;
  }

  std::vector<std::map<std::string, Utilities::CastValue>>& RawDataHandler::getReferenceData()
  {
    return *(reference_data_.get());
  }

  const std::vector<std::map<std::string, Utilities::CastValue>>& RawDataHandler::getReferenceData() const
  {
    return *(reference_data_.get());
  }

  std::shared_ptr<std::vector<std::map<std::string, Utilities::CastValue>>>& RawDataHandler::getReferenceDataShared()
  {
    return reference_data_;
  }

  void RawDataHandler::setQuantitationMethods(const std::vector<OpenMS::AbsoluteQuantitationMethod>& quantitation_methods)
  {
    quantitation_methods_.reset(new std::vector<OpenMS::AbsoluteQuantitationMethod>(quantitation_methods));
  }

  void RawDataHandler::setQuantitationMethods(std::shared_ptr<std::vector<OpenMS::AbsoluteQuantitationMethod>>& quantitation_methods)
  {
    quantitation_methods_ = quantitation_methods;
  }

  std::vector<OpenMS::AbsoluteQuantitationMethod>& RawDataHandler::getQuantitationMethods()
  {
    return *(quantitation_methods_.get());
  }

  const std::vector<OpenMS::AbsoluteQuantitationMethod>& RawDataHandler::getQuantitationMethods() const
  {
    return *(quantitation_methods_.get());
  }

  std::shared_ptr<std::vector<OpenMS::AbsoluteQuantitationMethod>>& RawDataHandler::getQuantitationMethodsShared()
  {
    return quantitation_methods_;
  }

  void RawDataHandler::setFeatureFilter(const OpenMS::MRMFeatureQC& feature_filter)
  {
    feature_filter_.reset(new OpenMS::MRMFeatureQC(feature_filter));
  }

  void RawDataHandler::setFeatureFilter(std::shared_ptr<OpenMS::MRMFeatureQC>& feature_filter)
  {
    feature_filter_ = feature_filter;
  }

  OpenMS::MRMFeatureQC& RawDataHandler::getFeatureFilter()
  {
    return *(feature_filter_.get());
  }

  const OpenMS::MRMFeatureQC& RawDataHandler::getFeatureFilter() const
  {
    return *(feature_filter_.get());
  }

  std::shared_ptr<OpenMS::MRMFeatureQC>& RawDataHandler::getFeatureFilterShared()
  {
    return feature_filter_;
  }

  void RawDataHandler::setFeatureQC(const OpenMS::MRMFeatureQC& feature_qc)
  {
    feature_qc_.reset(new OpenMS::MRMFeatureQC(feature_qc));
  }

  void RawDataHandler::setFeatureQC(std::shared_ptr<OpenMS::MRMFeatureQC>& feature_qc)
  {
    feature_qc_ = feature_qc;
  }

  OpenMS::MRMFeatureQC& RawDataHandler::getFeatureQC()
  {
    return *(feature_qc_.get());
  }

  const OpenMS::MRMFeatureQC& RawDataHandler::getFeatureQC() const
  {
    return *(feature_qc_.get());
  }

  std::shared_ptr<OpenMS::MRMFeatureQC>& RawDataHandler::getFeatureQCShared()
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
    if (meta_data_!=nullptr) meta_data_->clear();
    validation_metrics_.clear();
    if (parameters_!=nullptr) parameters_->clear();
    if (targeted_exp_!=nullptr) targeted_exp_->clear(true);
    if (quantitation_methods_ != nullptr) quantitation_methods_->clear();
    if (feature_filter_ != nullptr) feature_filter_ = std::shared_ptr<OpenMS::MRMFeatureQC>(new OpenMS::MRMFeatureQC());
    if (feature_qc_ != nullptr) feature_qc_ = std::shared_ptr<OpenMS::MRMFeatureQC>(new OpenMS::MRMFeatureQC());
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

    // Case 1: Copy the current featuremap and timestamp
    if (feature_map_history_.empty()) {
      feature_map_history_ = feature_map_; // ensures PrimaryMSRunPath is copied
      for (OpenMS::Feature& feature_new : feature_map_history_) {
        for (OpenMS::Feature& subordinate_new : feature_new.getSubordinates()) {
          if (!subordinate_new.metaValueExists("used_")) { // prevents overwriting feature_maps with existing "used_" attributes
            subordinate_new.setMetaValue("used_", "true");
          }
          if (!subordinate_new.metaValueExists("timestamp_")) { // prevents overwriting feature_maps with existing "timestamp_" attributes
            subordinate_new.setMetaValue("timestamp_", timestamp);
          }
        }
      }
    }

    // Case 2: "Remove" filtered/non-selected features and "Add" new features via "used_" and "timestamp_" feature metadata attributes
    else {
      std::vector<OpenMS::Feature> new_features;
      std::set<OpenMS::UInt> unique_ids_feat_history, unique_ids_feat_select; // Get all feature IDs
      for (const OpenMS::Feature& feature_copy : feature_map_history_) {
        unique_ids_feat_history.insert(feature_copy.getUniqueId());
      }
      for (const OpenMS::Feature& feature_select : feature_map_) {
        unique_ids_feat_select.insert(feature_select.getUniqueId());
        if (unique_ids_feat_history.count(feature_select.getUniqueId())) {
          continue;
        }
        // New Feature
        OpenMS::Feature new_feature = feature_select;
        for (OpenMS::Feature& subordinate_new : new_feature.getSubordinates()) {
          subordinate_new.setMetaValue("used_", "true");
          subordinate_new.setMetaValue("timestamp_", timestamp);
        }
        new_features.push_back(new_feature); // add the new feature to the list; we will add them to the history at the end
      }

      for (OpenMS::Feature& feature_copy : feature_map_history_) {
        if (unique_ids_feat_select.count(feature_copy.getUniqueId()) == 0) { // Removed feature
          for (OpenMS::Feature& subordinate_copy : feature_copy.getSubordinates()) {
            subordinate_copy.setMetaValue("used_", "false");
            subordinate_copy.setMetaValue("timestamp_", timestamp);
          }
          continue; // move on to the next feature in the history
        }
        for (const OpenMS::Feature& feature_select : feature_map_) {
          // skip feature if any of the following is true:
          // - unique ids differ
          // - peptide refs differ
          if (feature_select.getUniqueId() != feature_copy.getUniqueId() ||
              feature_select.getMetaValue("PeptideRef") !=
               feature_copy.getMetaValue("PeptideRef")) {
            continue;
          }

          // Matching feature
          std::vector<OpenMS::Feature> new_subordinates;
          // get the unique subordinate names (and not unique ids!)
          std::set<std::string> unique_ids_sub_history, unique_ids_sub_select;
          for (const OpenMS::Feature& subordinate_copy : feature_copy.getSubordinates()) {
            unique_ids_sub_history.insert(subordinate_copy.getMetaValue("native_id"));
          }
          for (const OpenMS::Feature& subordinate_select : feature_select.getSubordinates()) {
            unique_ids_sub_select.insert(subordinate_select.getMetaValue("native_id"));
            if (unique_ids_sub_history.count(subordinate_select.getMetaValue("native_id"))) {
              continue;
            }
            // New subordinate
            OpenMS::Feature new_subordinate = subordinate_select;
            new_subordinate.setMetaValue("used_", "true");
            new_subordinate.setMetaValue("timestamp_", timestamp);
            // add the new subordinate to the list; we will add them to the feature subordinate list at the end
            new_subordinates.push_back(new_subordinate);
          }

          for (OpenMS::Feature& subordinate_copy : feature_copy.getSubordinates()) {
            if (0 == unique_ids_sub_select.count(subordinate_copy.getMetaValue("native_id"))) { // Removed subordinate
              subordinate_copy.setMetaValue("used_", "false");
              subordinate_copy.setMetaValue("timestamp_", timestamp);
              continue; // move on to the next subordinate from the history
            }
            for (const OpenMS::Feature& subordinate_select : feature_select.getSubordinates()) {
              if (subordinate_select.getUniqueId() == subordinate_copy.getUniqueId() &&
                  subordinate_select.getMetaValue("native_id") ==
                    subordinate_copy.getMetaValue("native_id")) { // Matching subordinate
                subordinate_copy = subordinate_select; // copy over changed meta values from the current feature map subordinate
                subordinate_copy.setMetaValue("used_", "true");
                subordinate_copy.setMetaValue("timestamp_", timestamp);
                break; // break the loop and move on to the next subordinate from the history
              }
            }
          }
          if (new_subordinates.size()) { // append new subordinates to the existing subordinates list
            for (OpenMS::Feature& subordinate_copy : feature_copy.getSubordinates()) {
              new_subordinates.insert(new_subordinates.begin(), subordinate_copy);
            }
            feature_copy.setSubordinates(new_subordinates);
          }
          break; // break the loop and move on to the next feature in the history
        }
      }

      // Add in the new features to the feature history
      if (new_features.size()) {
        for (OpenMS::Feature& feature_new : new_features) {
          feature_map_history_.push_back(feature_new);
        }
      }
    }
  }
}
