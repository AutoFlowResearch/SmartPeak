// TODO: Add copyright

#pragma once

#include <OpenMS/ANALYSIS/QUANTITATION/AbsoluteQuantitationMethod.h>
#include <OpenMS/METADATA/AbsoluteQuantitationStandards.h>
#include <OpenMS/ANALYSIS/OPENSWATH/MRMFeatureQC.h>

namespace SmartPeak
{
  class SequenceSegmentHandler
  {
public:
    SequenceSegmentHandler(); ///< constructor to initialize shared resources

    void clear();

    void setSequenceSegmentName(const std::string& sequence_segment_name);
    std::string& getSequenceSegmentName();
    const std::string& getSequenceSegmentName() const;

    void setSampleIndices(const std::vector<size_t>& sample_indices);
    std::vector<size_t>& getSampleIndices();
    const std::vector<size_t>& getSampleIndices() const;

    void setStandardsConcentrations(const std::vector<OpenMS::AbsoluteQuantitationStandards::runConcentration>& standards_concentrations);
    std::vector<OpenMS::AbsoluteQuantitationStandards::runConcentration>& getStandardsConcentrations();
    const std::vector<OpenMS::AbsoluteQuantitationStandards::runConcentration>& getStandardsConcentrations() const;

    void setQuantitationMethods(const std::vector<OpenMS::AbsoluteQuantitationMethod>& quantitation_methods);
    void setQuantitationMethods(std::shared_ptr<std::vector<OpenMS::AbsoluteQuantitationMethod>>& quantitation_methods);
    std::vector<OpenMS::AbsoluteQuantitationMethod>& getQuantitationMethods();
    const std::vector<OpenMS::AbsoluteQuantitationMethod>& getQuantitationMethods() const;
    std::shared_ptr<std::vector<OpenMS::AbsoluteQuantitationMethod>>& getQuantitationMethodsShared();

    void setFeatureFilter(const OpenMS::MRMFeatureQC& feature_filter);
    void setFeatureFilter(std::shared_ptr<OpenMS::MRMFeatureQC>& feature_filter);
    OpenMS::MRMFeatureQC& getFeatureFilter();
    const OpenMS::MRMFeatureQC& getFeatureFilter() const;
    std::shared_ptr<OpenMS::MRMFeatureQC>& getFeatureFilterShared();

    void setFeatureQC(const OpenMS::MRMFeatureQC& feature_qc);
    void setFeatureQC(std::shared_ptr<OpenMS::MRMFeatureQC>& feature_qc);
    OpenMS::MRMFeatureQC& getFeatureQC();
    const OpenMS::MRMFeatureQC& getFeatureQC() const;
    std::shared_ptr<OpenMS::MRMFeatureQC>& getFeatureQCShared();

    void setFeatureRSDFilter(const OpenMS::MRMFeatureQC& feature_rsd_filter);
    void setFeatureRSDFilter(std::shared_ptr<OpenMS::MRMFeatureQC>& feature_rsd_filter);
    OpenMS::MRMFeatureQC& getFeatureRSDFilter();
    const OpenMS::MRMFeatureQC& getFeatureRSDFilter() const;
    std::shared_ptr<OpenMS::MRMFeatureQC>& getFeatureRSDFilterShared();

    void setFeatureRSDQC(const OpenMS::MRMFeatureQC& feature_rsd_qc);
    void setFeatureRSDQC(std::shared_ptr<OpenMS::MRMFeatureQC>& feature_rsd_qc);
    OpenMS::MRMFeatureQC& getFeatureRSDQC();
    const OpenMS::MRMFeatureQC& getFeatureRSDQC() const;
    std::shared_ptr<OpenMS::MRMFeatureQC>& getFeatureRSDQCShared();

    void setFeatureBackgroundFilter(const OpenMS::MRMFeatureQC& feature_background_filter);
    void setFeatureBackgroundFilter(std::shared_ptr<OpenMS::MRMFeatureQC>& feature_background_filter);
    OpenMS::MRMFeatureQC& getFeatureBackgroundFilter();
    const OpenMS::MRMFeatureQC& getFeatureBackgroundFilter() const;
    std::shared_ptr<OpenMS::MRMFeatureQC>& getFeatureBackgroundFilterShared();

    void setFeatureBackgroundQC(const OpenMS::MRMFeatureQC& feature_background_qc);
    void setFeatureBackgroundQC(std::shared_ptr<OpenMS::MRMFeatureQC>& feature_background_qc);
    OpenMS::MRMFeatureQC& getFeatureBackgroundQC();
    const OpenMS::MRMFeatureQC& getFeatureBackgroundQC() const;
    std::shared_ptr<OpenMS::MRMFeatureQC>& getFeatureBackgroundQCShared();

    void setFeatureRSDEstimations(const OpenMS::MRMFeatureQC& feature_rsd_estimations);
    void setFeatureRSDEstimations(std::shared_ptr<OpenMS::MRMFeatureQC>& feature_rsd_estimations);
    OpenMS::MRMFeatureQC& getFeatureRSDEstimations();
    const OpenMS::MRMFeatureQC& getFeatureRSDEstimations() const;
    std::shared_ptr<OpenMS::MRMFeatureQC>& getFeatureRSDEstimationsShared();

    void setFeatureBackgroundEstimations(const OpenMS::MRMFeatureQC& feature_background_estimations);
    void setFeatureBackgroundEstimations(std::shared_ptr<OpenMS::MRMFeatureQC>& feature_background_estimations);
    OpenMS::MRMFeatureQC& getFeatureBackgroundEstimations();
    const OpenMS::MRMFeatureQC& getFeatureBackgroundEstimations() const;
    std::shared_ptr<OpenMS::MRMFeatureQC>& getFeatureBackgroundEstimationsShared();

    void setComponentsToConcentrations(
      const std::map<std::string, std::vector<OpenMS::AbsoluteQuantitationStandards::featureConcentration>> components_to_concentrations
    );

    std::map<std::string, std::vector<OpenMS::AbsoluteQuantitationStandards::featureConcentration>>&
    getComponentsToConcentrations();

    const std::map<std::string, std::vector<OpenMS::AbsoluteQuantitationStandards::featureConcentration>>&
    getComponentsToConcentrations() const;

private:
    std::string sequence_segment_name_;
    std::vector<size_t> sample_indices_;  ///< The indices of each injection; this could be replaced with `std::shared_ptr<InjectionHandler>` to save the map lookup
    std::vector<OpenMS::AbsoluteQuantitationStandards::runConcentration> standards_concentrations_;
    std::shared_ptr<std::vector<OpenMS::AbsoluteQuantitationMethod>> quantitation_methods_ = nullptr;  ///< Transition quantitation methods; shared between all raw data handlers in the sequence segment    
    std::shared_ptr<OpenMS::MRMFeatureQC> feature_filter_ = nullptr;  ///< Feature Filters; shared between all raw data handlers in the sequence segment
    std::shared_ptr<OpenMS::MRMFeatureQC> feature_qc_ = nullptr;  ///< Feature QCs; shared between all raw data handlers in the sequence segment
    std::shared_ptr<OpenMS::MRMFeatureQC> feature_rsd_filter_ = nullptr;  ///< Feature Filters; shared between all raw data handlers in the sequence segment
    std::shared_ptr<OpenMS::MRMFeatureQC> feature_rsd_qc_ = nullptr;  ///< Feature QCs; shared between all raw data handlers in the sequence segment
    std::shared_ptr<OpenMS::MRMFeatureQC> feature_background_filter_ = nullptr;  ///< Feature Filters; shared between all raw data handlers in the sequence segment
    std::shared_ptr<OpenMS::MRMFeatureQC> feature_background_qc_ = nullptr;  ///< Feature QCs; shared between all raw data handlers in the sequence segment
    std::shared_ptr<OpenMS::MRMFeatureQC> feature_rsd_estimations_ = nullptr;  ///< Percent RSD estimations; shared between all raw data handlers in the sequence segment
    std::shared_ptr<OpenMS::MRMFeatureQC> feature_background_estimations_ = nullptr;  ///< Background interference estimations; shared between all raw data handlers in the sequence segment
    std::map<std::string, std::vector<OpenMS::AbsoluteQuantitationStandards::featureConcentration>> components_to_concentrations_;
  };
}