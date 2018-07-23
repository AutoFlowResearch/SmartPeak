/**TODO:  Add copyright*/

#pragma once

#include <OpenMS/KERNEL/FeatureMap.h>
#include <OpenMS/ANALYSIS/TARGETED/TargetedExperiment.h>
#include <OpenMS/KERNEL/MSExperiment.h>
#include <OpenMS/ANALYSIS/OPENSWATH/MRMFeatureQC.h>
#include <OpenMS/ANALYSIS/QUANTITATION/AbsoluteQuantitationMethod.h>
#include <OpenMS/ANALYSIS/MAPMATCHING/TransformationDescription.h>

#include <SmartPeak/core/MetaDataHandler.h>

#include <map>
#include <vector>

namespace SmartPeak
{

  class RawDataHandler
  {
public:
    RawDataHandler() = default;
    ~RawDataHandler() = default;

    void setFeatureMap(const OpenMS::FeatureMap& featureMap_I);
    OpenMS::FeatureMap& getFeatureMap();
    OpenMS::FeatureMap getFeatureMap() const;

    void setMetaData(const MetaDataHandler& meta_data_I);
    MetaDataHandler& getMetaData();
    MetaDataHandler getMetaData() const;

    void setParameters(
      const std::map<std::string, std::vector<std::map<std::string, std::string>>>& parameters_I);
    std::map<std::string, std::vector<std::map<std::string, std::string>>>& getParameters();
    std::map<std::string, std::vector<std::map<std::string, std::string>>> getParameters() const;

    // [REFACTOR: change to set/getTargetedExperiment]
    void setTargeted(const OpenMS::TargetedExperiment& targeted_I);
    OpenMS::TargetedExperiment& getTargeted();
    OpenMS::TargetedExperiment getTargeted() const;

    // [TODO: need to update this at some point...]
    // setReferenceData(, reference_data_I);
    // getReferenceData();

    void setQuantitationMethods(const std::vector<OpenMS::AbsoluteQuantitationMethod>& quantitation_methods_I);
    std::vector<OpenMS::AbsoluteQuantitationMethod>& getQuantitationMethods();
    std::vector<OpenMS::AbsoluteQuantitationMethod> getQuantitationMethods() const;

    void setFeatureFilter(const OpenMS::MRMFeatureQC& feature_filter_I);
    OpenMS::MRMFeatureQC& getFeatureFilter();
    OpenMS::MRMFeatureQC getFeatureFilter() const;

    void setFeatureQC(const OpenMS::MRMFeatureQC& feature_qc_I);
    OpenMS::MRMFeatureQC& getFeatureQC();
    OpenMS::MRMFeatureQC getFeatureQC() const;

    void setFeatureMapHistory(const std::vector<OpenMS::FeatureMap>& feature_maps_I);
    std::vector<OpenMS::FeatureMap>& getFeatureMapHistory();
    std::vector<OpenMS::FeatureMap> getFeatureMapHistory() const;

private:
    // input
    OpenMS::MSExperiment msExperiment_;
    OpenMS::MSExperiment chromatogram_map_;
    OpenMS::TransformationDescription trafo_;
    OpenMS::MSExperiment swath_;

    // output
    OpenMS::FeatureMap featureMap_;
    MetaDataHandler meta_data_;
    std::map<std::string, float> validation_metrics_;
    std::vector<OpenMS::FeatureMap> featureMapHistory_;

    // input (reused between RawDataHandlers)
    std::map<std::string, std::vector<std::map<std::string, std::string>>> parameters_;
    OpenMS::TargetedExperiment targeted_;  // [REFACTOR: change to targeted_experiment]
    // self.reference_data = None
    std::vector<OpenMS::AbsoluteQuantitationMethod> quantitation_methods_;
    OpenMS::MRMFeatureQC feature_filter_;
    OpenMS::MRMFeatureQC feature_qc_;
  };
}