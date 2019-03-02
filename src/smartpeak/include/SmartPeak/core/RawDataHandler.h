/**TODO:  Add copyright*/

#pragma once

#include <OpenMS/ANALYSIS/MAPMATCHING/TransformationDescription.h>
#include <OpenMS/ANALYSIS/OPENSWATH/MRMFeatureQC.h>
#include <OpenMS/ANALYSIS/QUANTITATION/AbsoluteQuantitationMethod.h>
#include <OpenMS/ANALYSIS/TARGETED/TargetedExperiment.h>
#include <OpenMS/KERNEL/FeatureMap.h>
#include <OpenMS/KERNEL/MSExperiment.h>

#include <SmartPeak/core/MetaDataHandler.h>
#include <SmartPeak/core/Utilities.h>

#include <map>
#include <vector>

namespace SmartPeak
{

  class RawDataHandler
  {
public:
    void setFeatureMap(const OpenMS::FeatureMap& feature_map);
    OpenMS::FeatureMap& getFeatureMap();
    const OpenMS::FeatureMap& getFeatureMap() const;

    void setMetaData(const MetaDataHandler& meta_data);
    void setMetaData(std::shared_ptr<MetaDataHandler>& meta_data);
    MetaDataHandler& getMetaData();
    const MetaDataHandler& getMetaData() const;

    void setParameters(
      const std::map<std::string, std::vector<std::map<std::string, std::string>>>& parameters);
    void setParameters(
      std::shared_ptr<std::map<std::string, std::vector<std::map<std::string, std::string>>>>& parameters);
    std::map<std::string, std::vector<std::map<std::string, std::string>>>& getParameters();
    const std::map<std::string, std::vector<std::map<std::string, std::string>>>& getParameters() const;

    void setTargetedExperiment(const OpenMS::TargetedExperiment& targeted_exp);
    void setTargetedExperiment(std::shared_ptr<OpenMS::TargetedExperiment>& targeted_exp);
    OpenMS::TargetedExperiment& getTargetedExperiment();
    const OpenMS::TargetedExperiment& getTargetedExperiment() const;

    void setReferenceData(const std::vector<std::map<std::string, Utilities::CastValue>>& reference_data);
    void setReferenceData(std::shared_ptr<std::vector<std::map<std::string, Utilities::CastValue>>>& reference_data);
    std::vector<std::map<std::string, Utilities::CastValue>>& getReferenceData();
    const std::vector<std::map<std::string, Utilities::CastValue>>& getReferenceData() const;

    void setQuantitationMethods(const std::vector<OpenMS::AbsoluteQuantitationMethod>& quantitation_methods);
    void setQuantitationMethods(std::shared_ptr<std::vector<OpenMS::AbsoluteQuantitationMethod>>& quantitation_methods);
    std::vector<OpenMS::AbsoluteQuantitationMethod>& getQuantitationMethods();
    const std::vector<OpenMS::AbsoluteQuantitationMethod>& getQuantitationMethods() const;

    void setFeatureFilter(const OpenMS::MRMFeatureQC& feature_filter);
    void setFeatureFilter(std::shared_ptr<OpenMS::MRMFeatureQC>& feature_filter);
    OpenMS::MRMFeatureQC& getFeatureFilter();
    const OpenMS::MRMFeatureQC& getFeatureFilter() const;

    void setFeatureQC(const OpenMS::MRMFeatureQC& feature_qc);
    void setFeatureQC(std::shared_ptr<OpenMS::MRMFeatureQC>& feature_qc);
    OpenMS::MRMFeatureQC& getFeatureQC();
    const OpenMS::MRMFeatureQC& getFeatureQC() const;

    void setFeatureMapHistory(const OpenMS::FeatureMap& feature_map_history);
    OpenMS::FeatureMap& getFeatureMapHistory();
    const OpenMS::FeatureMap& getFeatureMapHistory() const;

    void setExperiment(const OpenMS::MSExperiment& experiment);
    OpenMS::MSExperiment& getExperiment();
    const OpenMS::MSExperiment& getExperiment() const;

    void setChromatogramMap(const OpenMS::MSExperiment& chromatogram_map);
    OpenMS::MSExperiment& getChromatogramMap();
    const OpenMS::MSExperiment& getChromatogramMap() const;

    void setTransformationDescription(const OpenMS::TransformationDescription& trafo);
    OpenMS::TransformationDescription& getTransformationDescription();
    const OpenMS::TransformationDescription& getTransformationDescription() const;

    void setSWATH(const OpenMS::MSExperiment& swath);
    OpenMS::MSExperiment& getSWATH();
    const OpenMS::MSExperiment& getSWATH() const;

    void setValidationMetrics(const std::map<std::string, float>& validation_metrics);
    std::map<std::string, float>& getValidationMetrics();
    const std::map<std::string, float>& getValidationMetrics() const;

    void clear();

    /** Update the Feature map history based on the
      filtered, selected, or new features in the current featureMap.

      All new features are initialized as "used_" = True with the "modified_" attribute set to the current time-stamp.
      All removed features are changed to "used_" = False with the "modified_" attribute set to the current time-stamp.

      ASSUMPTIONS: the unique ID attribute is set within OpenMS
    */
    void updateFeatureMapHistory();

private:
    // input
    OpenMS::MSExperiment experiment_;  ///< Raw MS data derived from the mzML file
    OpenMS::MSExperiment chromatogram_map_;  ///< MS data annotated with transition information derived from the TraML file
    OpenMS::TransformationDescription trafo_;  ///< Mapping of retention time values; currently not used (maybe shared between all raw data handlers)
    OpenMS::MSExperiment swath_;

    // output
    OpenMS::FeatureMap feature_map_; ///< The most recently generated set of features for the experiment
    OpenMS::FeatureMap feature_map_history_; ///< A record of all changes that have occured to the features in the experiment
    std::shared_ptr<MetaDataHandler> meta_data_;  ///< sample meta data; shared between the injection handler and the raw data handler
    std::map<std::string, float> validation_metrics_;

    // input (reused between RawDataHandlers)
    std::shared_ptr<std::map<std::string, std::vector<std::map<std::string, std::string>>>> parameters_ = nullptr;  ///< algorithm parameters; shared between all raw data handlers in the sequence
    std::shared_ptr<OpenMS::TargetedExperiment> targeted_exp_ = nullptr;  ///< transitions for the SRM experiments; shared between all raw data handlers in the sequence
    std::shared_ptr<std::vector<std::map<std::string, Utilities::CastValue>>> reference_data_ = nullptr;  ///< Reference data to compare algorithm accuracy; shared between all raw data handlers in the sequence
    std::shared_ptr<std::vector<OpenMS::AbsoluteQuantitationMethod>> quantitation_methods_ = nullptr;  ///< Transition quantitation methods; shared between all raw data handlers in the sequence segment
    std::shared_ptr<OpenMS::MRMFeatureQC> feature_filter_ = nullptr;  ///< Feature Filters; shared between all raw data handlers in the sequence
    std::shared_ptr<OpenMS::MRMFeatureQC> feature_qc_ = nullptr;  ///< Feature QCs; shared between all raw data handlers in the sequence
  };
}
