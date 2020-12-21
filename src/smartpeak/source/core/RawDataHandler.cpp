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
// $Authors: Douglas McCloskey $
// --------------------------------------------------------------------------

#include <SmartPeak/core/RawDataHandler.h>
#include <ctime> // time format
#include <chrono> // current time

namespace SmartPeak
{
  RawDataHandler::RawDataHandler() :
    meta_data_(std::make_shared<MetaDataHandler>(MetaDataHandler())),
    parameters_(std::make_shared<std::map<std::string, std::vector<std::map<std::string, std::string>>>>(std::map<std::string, std::vector<std::map<std::string, std::string>>>())),
    targeted_exp_(std::make_shared<OpenMS::TargetedExperiment>(OpenMS::TargetedExperiment())),
    reference_data_(std::make_shared<std::vector<std::map<std::string, CastValue>>>(std::vector<std::map<std::string, CastValue>>())),
    quantitation_methods_(std::make_shared<std::vector<OpenMS::AbsoluteQuantitationMethod>>(std::vector<OpenMS::AbsoluteQuantitationMethod>())),
    feature_filter_(std::make_shared<OpenMS::MRMFeatureQC>(OpenMS::MRMFeatureQC())),
    feature_qc_(std::make_shared<OpenMS::MRMFeatureQC>(OpenMS::MRMFeatureQC())),
    feature_rsd_filter_(std::make_shared<OpenMS::MRMFeatureQC>(OpenMS::MRMFeatureQC())),
    feature_rsd_qc_(std::make_shared<OpenMS::MRMFeatureQC>(OpenMS::MRMFeatureQC())),
    feature_background_filter_(std::make_shared<OpenMS::MRMFeatureQC>(OpenMS::MRMFeatureQC())),
    feature_background_qc_(std::make_shared<OpenMS::MRMFeatureQC>(OpenMS::MRMFeatureQC())),
    feature_rsd_estimations_(std::make_shared<OpenMS::MRMFeatureQC>(OpenMS::MRMFeatureQC())),
    feature_background_estimations_(std::make_shared<OpenMS::MRMFeatureQC>(OpenMS::MRMFeatureQC()))
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

  void RawDataHandler::setReferenceData(const std::vector<std::map<std::string, CastValue>>& reference_data)
  {
    reference_data_.reset(new std::vector<std::map<std::string, CastValue>>(reference_data));
  }

  void RawDataHandler::setReferenceData(std::shared_ptr<std::vector<std::map<std::string, CastValue>>>& reference_data)
  {
    reference_data_ = reference_data;
  }

  std::vector<std::map<std::string, CastValue>>& RawDataHandler::getReferenceData()
  {
    return *(reference_data_.get());
  }

  const std::vector<std::map<std::string, CastValue>>& RawDataHandler::getReferenceData() const
  {
    return *(reference_data_.get());
  }

  std::shared_ptr<std::vector<std::map<std::string, CastValue>>>& RawDataHandler::getReferenceDataShared()
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

  void RawDataHandler::setFeatureRSDFilter(const OpenMS::MRMFeatureQC& feature_rsd_filter)
  {
    feature_rsd_filter_ = std::make_shared<OpenMS::MRMFeatureQC>(OpenMS::MRMFeatureQC(feature_rsd_filter));
  }

  void RawDataHandler::setFeatureRSDFilter(std::shared_ptr<OpenMS::MRMFeatureQC>& feature_rsd_filter)
  {
    feature_rsd_filter_ = feature_rsd_filter;
  }

  OpenMS::MRMFeatureQC& RawDataHandler::getFeatureRSDFilter()
  {
    return *(feature_rsd_filter_.get());
  }

  const OpenMS::MRMFeatureQC& RawDataHandler::getFeatureRSDFilter() const
  {
    return *(feature_rsd_filter_.get());
  }

  std::shared_ptr<OpenMS::MRMFeatureQC>& RawDataHandler::getFeatureRSDFilterShared()
  {
    return feature_rsd_filter_;
  }

  void RawDataHandler::setFeatureRSDQC(const OpenMS::MRMFeatureQC& feature_rsd_qc)
  {
    feature_rsd_qc_ = std::make_shared<OpenMS::MRMFeatureQC>(OpenMS::MRMFeatureQC(feature_rsd_qc));
  }

  void RawDataHandler::setFeatureRSDQC(std::shared_ptr<OpenMS::MRMFeatureQC>& feature_rsd_qc)
  {
    feature_rsd_qc_ = feature_rsd_qc;
  }

  OpenMS::MRMFeatureQC& RawDataHandler::getFeatureRSDQC()
  {
    return *(feature_rsd_qc_.get());
  }

  const OpenMS::MRMFeatureQC& RawDataHandler::getFeatureRSDQC() const
  {
    return *(feature_rsd_qc_.get());
  }

  std::shared_ptr<OpenMS::MRMFeatureQC>& RawDataHandler::getFeatureRSDQCShared()
  {
    return feature_rsd_qc_;
  }

  void RawDataHandler::setFeatureBackgroundFilter(const OpenMS::MRMFeatureQC& feature_background_filter)
  {
    feature_background_filter_ = std::make_shared<OpenMS::MRMFeatureQC>(OpenMS::MRMFeatureQC(feature_background_filter));
  }

  void RawDataHandler::setFeatureBackgroundFilter(std::shared_ptr<OpenMS::MRMFeatureQC>& feature_background_filter)
  {
    feature_background_filter_ = feature_background_filter;
  }

  OpenMS::MRMFeatureQC& RawDataHandler::getFeatureBackgroundFilter()
  {
    return *(feature_background_filter_.get());
  }

  const OpenMS::MRMFeatureQC& RawDataHandler::getFeatureBackgroundFilter() const
  {
    return *(feature_background_filter_.get());
  }

  std::shared_ptr<OpenMS::MRMFeatureQC>& RawDataHandler::getFeatureBackgroundFilterShared()
  {
    return feature_background_filter_;
  }

  void RawDataHandler::setFeatureBackgroundQC(const OpenMS::MRMFeatureQC& feature_background_qc)
  {
    feature_background_qc_ = std::make_shared<OpenMS::MRMFeatureQC>(OpenMS::MRMFeatureQC(feature_background_qc));
  }

  void RawDataHandler::setFeatureBackgroundQC(std::shared_ptr<OpenMS::MRMFeatureQC>& feature_background_qc)
  {
    feature_background_qc_ = feature_background_qc;
  }

  OpenMS::MRMFeatureQC& RawDataHandler::getFeatureBackgroundQC()
  {
    return *(feature_background_qc_.get());
  }

  const OpenMS::MRMFeatureQC& RawDataHandler::getFeatureBackgroundQC() const
  {
    return *(feature_background_qc_.get());
  }

  std::shared_ptr<OpenMS::MRMFeatureQC>& RawDataHandler::getFeatureBackgroundQCShared()
  {
    return feature_background_qc_;
  }

  void RawDataHandler::setFeatureRSDEstimations(const OpenMS::MRMFeatureQC& feature_rsd_estimations)
  {
    feature_rsd_estimations_ = std::make_shared<OpenMS::MRMFeatureQC>(OpenMS::MRMFeatureQC(feature_rsd_estimations));
  }

  void RawDataHandler::setFeatureRSDEstimations(std::shared_ptr<OpenMS::MRMFeatureQC>& feature_rsd_estimations)
  {
    feature_rsd_estimations_ = feature_rsd_estimations;
  }

  OpenMS::MRMFeatureQC& RawDataHandler::getFeatureRSDEstimations()
  {
    return *(feature_rsd_estimations_.get());
  }

  const OpenMS::MRMFeatureQC& RawDataHandler::getFeatureRSDEstimations() const
  {
    return *(feature_rsd_estimations_.get());
  }

  std::shared_ptr<OpenMS::MRMFeatureQC>& RawDataHandler::getFeatureRSDEstimationsShared()
  {
    return feature_rsd_estimations_;
  }

  void RawDataHandler::setFeatureBackgroundEstimations(const OpenMS::MRMFeatureQC& feature_background_estimations)
  {
    feature_background_estimations_ = std::make_shared<OpenMS::MRMFeatureQC>(OpenMS::MRMFeatureQC(feature_background_estimations));
  }

  void RawDataHandler::setFeatureBackgroundEstimations(std::shared_ptr<OpenMS::MRMFeatureQC>& feature_background_estimations)
  {
    feature_background_estimations_ = feature_background_estimations;
  }

  OpenMS::MRMFeatureQC& RawDataHandler::getFeatureBackgroundEstimations()
  {
    return *(feature_background_estimations_.get());
  }

  const OpenMS::MRMFeatureQC& RawDataHandler::getFeatureBackgroundEstimations() const
  {
    return *(feature_background_estimations_.get());
  }

  std::shared_ptr<OpenMS::MRMFeatureQC>& RawDataHandler::getFeatureBackgroundEstimationsShared()
  {
    return feature_background_estimations_;
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

  void RawDataHandler::setMzTab(const OpenMS::MzTab& mz_tab)
  {
    mz_tab_ = mz_tab;
  }

  OpenMS::MzTab& RawDataHandler::getMzTab()
  {
    return mz_tab_;
  }

  const OpenMS::MzTab& RawDataHandler::getMzTab() const
  {
    return mz_tab_;
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
    mz_tab_ = OpenMS::MzTab();
    if (parameters_!=nullptr) parameters_->clear();
    if (targeted_exp_!=nullptr) targeted_exp_->clear(true);
    if (quantitation_methods_ != nullptr) quantitation_methods_->clear();
    if (feature_filter_ != nullptr) feature_filter_ = std::make_shared<OpenMS::MRMFeatureQC>(OpenMS::MRMFeatureQC());
    if (feature_qc_ != nullptr) feature_qc_ = std::make_shared<OpenMS::MRMFeatureQC>(OpenMS::MRMFeatureQC());
    if (feature_rsd_filter_ != nullptr) feature_rsd_filter_ = std::make_shared<OpenMS::MRMFeatureQC>(OpenMS::MRMFeatureQC());
    if (feature_rsd_qc_ != nullptr) feature_rsd_qc_ = std::make_shared<OpenMS::MRMFeatureQC>(OpenMS::MRMFeatureQC());
    if (feature_background_filter_ != nullptr) feature_background_filter_ = std::make_shared<OpenMS::MRMFeatureQC>(OpenMS::MRMFeatureQC());
    if (feature_background_qc_ != nullptr) feature_background_qc_ = std::make_shared<OpenMS::MRMFeatureQC>(OpenMS::MRMFeatureQC());
    if (feature_rsd_estimations_ != nullptr) feature_rsd_estimations_ = std::make_shared<OpenMS::MRMFeatureQC>(OpenMS::MRMFeatureQC());
    if (feature_background_estimations_ != nullptr) feature_background_estimations_ = std::make_shared<OpenMS::MRMFeatureQC>(OpenMS::MRMFeatureQC());
  }

  void RawDataHandler::clearNonSharedData()
  {
    experiment_.clear(true);
    chromatogram_map_.clear(true);
    trafo_ = OpenMS::TransformationDescription();
    swath_.clear(true);
    feature_map_.clear(true);
    feature_map_history_.clear(true);
    validation_metrics_.clear();
    mz_tab_ = OpenMS::MzTab();
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
        if (!feature_new.metaValueExists("used_")) { // prevents overwriting feature_maps with existing "used_" attributes
          feature_new.setMetaValue("used_", "true");
        }
        if (!feature_new.metaValueExists("timestamp_")) { // prevents overwriting feature_maps with existing "timestamp_" attributes
          feature_new.setMetaValue("timestamp_", timestamp);
        }
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
      std::set<OpenMS::UInt64> unique_ids_feat_history, unique_ids_feat_select; // Get all feature IDs
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
        new_feature.setMetaValue("used_", "true");
        new_feature.setMetaValue("timestamp_", timestamp);
        for (OpenMS::Feature& subordinate_new : new_feature.getSubordinates()) {
          subordinate_new.setMetaValue("used_", "true");
          subordinate_new.setMetaValue("timestamp_", timestamp);
        }
        new_features.push_back(new_feature); // add the new feature to the list; we will add them to the history at the end
      }

      for (OpenMS::Feature& feature_copy : feature_map_history_) {
        if (unique_ids_feat_select.count(feature_copy.getUniqueId()) == 0) { // Removed feature
          feature_copy.setMetaValue("used_", "false");
          feature_copy.setMetaValue("timestamp_", timestamp);
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
          OpenMS::Feature feature_tmp = feature_select; // Make a temporary feature based on the selected feature that will be later swapped for the feature in the history
          bool update_feature = false;
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
            update_feature = true;
          }

          // Check the subordinates for changes
          for (OpenMS::Feature& subordinate_copy : feature_copy.getSubordinates()) {
            if (unique_ids_sub_select.count(subordinate_copy.getMetaValue("native_id")) == 0) { // Removed subordinate
              subordinate_copy.setMetaValue("used_", "false");
              subordinate_copy.setMetaValue("timestamp_", timestamp);
              update_feature = true;
              continue; // move on to the next subordinate from the history
            } 
            for (const OpenMS::Feature& subordinate_select : feature_select.getSubordinates()) {
              if (subordinate_select.getUniqueId() == subordinate_copy.getUniqueId() &&
                  subordinate_select.getMetaValue("native_id") == subordinate_copy.getMetaValue("native_id")) { // Matching subordinate
                subordinate_copy = subordinate_select; // copy over changed meta values from the current feature map subordinate
                if (!subordinate_copy.metaValueExists("used_"))  // overwrite "used_" only if it does not exist
                  subordinate_copy.setMetaValue("used_", "true");
                subordinate_copy.setMetaValue("timestamp_", timestamp);
                update_feature = true;
                break; // break the loop and move on to the next subordinate from the history
              }
            }
          }
          
          // Case of no subordinates
          if (feature_select.getSubordinates().size() <= 0) {
            update_feature = true;
          }

          if (update_feature) { // copy over the updated subordinates and change the feature to the updated version
            feature_tmp.setSubordinates(feature_copy.getSubordinates());
            feature_copy = feature_tmp;
            feature_copy.setMetaValue("used_", "true");
            feature_copy.setMetaValue("timestamp_", timestamp);
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
  void RawDataHandler::makeFeatureMapFromHistory()
  {
    // Current time stamp
    std::chrono::time_point<std::chrono::system_clock> time_now = std::chrono::system_clock::now();
    std::time_t time_now_t = std::chrono::system_clock::to_time_t(time_now);
    std::tm now_tm = *std::localtime(&time_now_t);
    char timestamp_char[64];
    std::strftime(timestamp_char, 64, "%Y-%m-%d-%H-%M-%S", &now_tm);
    std::string timestamp(timestamp_char);

    feature_map_.clear();
    for (OpenMS::Feature& feature_new : feature_map_history_) {
      std::vector<OpenMS::Feature> subs;
      bool copy_feature = false;

      // Case 1a: No subordinates
      if (feature_new.metaValueExists("used_") && feature_new.getMetaValue("used_").toString() == "true" && feature_new.getSubordinates().size() <= 0) {
        copy_feature = true;
      }
      // Case 1b: No subordinates and missing "used_"
      else if (!feature_new.metaValueExists("used_") && feature_new.getSubordinates().size() <= 0) {
        feature_new.setMetaValue("used_", "true");
        feature_new.setMetaValue("timestamp_", timestamp);
        copy_feature = true;
      }
      else {
        // Case 2a: Subordinates
        for (OpenMS::Feature& subordinate_new : feature_new.getSubordinates()) {
          if (subordinate_new.metaValueExists("used_") && subordinate_new.getMetaValue("used_").toString() == "true") {
            subs.push_back(subordinate_new);
            copy_feature = true;
          }
          // Case 2b: Subordinates and missing "used_
          else if (!subordinate_new.metaValueExists("used_")) {
            subordinate_new.setMetaValue("used_", "true");
            subordinate_new.setMetaValue("timestamp_", timestamp);
            subs.push_back(subordinate_new);
            copy_feature = true;
          }
        }
      }
      // Add the feature to the featureMap
      if (copy_feature) {
        OpenMS::Feature f = feature_new;
        f.setSubordinates(subs);
        feature_map_.push_back(f);
      }
    }
  }  
}
