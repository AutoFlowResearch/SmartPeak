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
    MetaDataHandler& getMetaData();
    const MetaDataHandler& getMetaData() const;

    void setParameters(
      const std::map<std::string, std::vector<std::map<std::string, std::string>>>& parameters);
    std::map<std::string, std::vector<std::map<std::string, std::string>>>& getParameters();
    const std::map<std::string, std::vector<std::map<std::string, std::string>>>& getParameters() const;

    void setTargetedExperiment(const OpenMS::TargetedExperiment& targeted_exp);
    OpenMS::TargetedExperiment& getTargetedExperiment();
    const OpenMS::TargetedExperiment& getTargetedExperiment() const;

    void setReferenceData(const std::vector<std::map<std::string, Utilities::CastValue>>& reference_data);
    std::vector<std::map<std::string, Utilities::CastValue>>& getReferenceData();
    const std::vector<std::map<std::string, Utilities::CastValue>>& getReferenceData() const;

    void setQuantitationMethods(const std::vector<OpenMS::AbsoluteQuantitationMethod>& quantitation_methods);
    std::vector<OpenMS::AbsoluteQuantitationMethod>& getQuantitationMethods();
    const std::vector<OpenMS::AbsoluteQuantitationMethod>& getQuantitationMethods() const;

    void setFeatureFilter(const OpenMS::MRMFeatureQC& feature_filter);
    OpenMS::MRMFeatureQC& getFeatureFilter();
    const OpenMS::MRMFeatureQC& getFeatureFilter() const;

    void setFeatureQC(const OpenMS::MRMFeatureQC& feature_qc);
    OpenMS::MRMFeatureQC& getFeatureQC();
    const OpenMS::MRMFeatureQC& getFeatureQC() const;

    void setFeatureMapHistory(const std::vector<OpenMS::FeatureMap>& feature_maps);
    std::vector<OpenMS::FeatureMap>& getFeatureMapHistory();
    const std::vector<OpenMS::FeatureMap>& getFeatureMapHistory() const;

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

private:
    // input
    OpenMS::MSExperiment experiment_;
    OpenMS::MSExperiment chromatogram_map_;
    OpenMS::TransformationDescription trafo_;
    OpenMS::MSExperiment swath_;

    // output
    OpenMS::FeatureMap feature_map_;
    MetaDataHandler meta_data_;
    std::map<std::string, float> validation_metrics_;
    std::vector<OpenMS::FeatureMap> feature_map_history_;

    // input (reused between RawDataHandlers)
    std::map<std::string, std::vector<std::map<std::string, std::string>>> parameters_;
    OpenMS::TargetedExperiment targeted_exp_;
    std::vector<std::map<std::string, Utilities::CastValue>> reference_data_;
    std::vector<OpenMS::AbsoluteQuantitationMethod> quantitation_methods_;
    OpenMS::MRMFeatureQC feature_filter_;
    OpenMS::MRMFeatureQC feature_qc_;
  };
}
