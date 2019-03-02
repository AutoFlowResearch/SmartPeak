// TODO: Add copyright

#pragma once

#include <OpenMS/ANALYSIS/QUANTITATION/AbsoluteQuantitationMethod.h>
#include <OpenMS/METADATA/AbsoluteQuantitationStandards.h>

namespace SmartPeak
{
  class SequenceSegmentHandler
  {
public:
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
    std::map<std::string, std::vector<OpenMS::AbsoluteQuantitationStandards::featureConcentration>> components_to_concentrations_;
  };
}