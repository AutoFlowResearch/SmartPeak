// TODO: Add copyright

#pragma once

#include <OpenMS/ANALYSIS/QUANTITATION/AbsoluteQuantitationMethod.h>
#include <OpenMS/METADATA/AbsoluteQuantitationStandards.h>

namespace SmartPeak
{
  class SequenceSegmentHandler
  {
public:
    SequenceSegmentHandler() = default;
    ~SequenceSegmentHandler() = default;

    void clear();

    void setSequenceSegmentName(const std::string& sequence_segment_name);
    std::string& getSequenceSegmentName();
    std::string getSequenceSegmentName() const;

    void setSampleIndices(const std::vector<size_t>& sample_indices);
    std::vector<size_t>& getSampleIndices();
    std::vector<size_t> getSampleIndices() const;

    void setStandardsConcentrations(const std::vector<OpenMS::AbsoluteQuantitationStandards::runConcentration>& standards_concentrations);
    std::vector<OpenMS::AbsoluteQuantitationStandards::runConcentration>& getStandardsConcentrations();
    std::vector<OpenMS::AbsoluteQuantitationStandards::runConcentration> getStandardsConcentrations() const;

    void setQuantitationMethods(const std::vector<OpenMS::AbsoluteQuantitationMethod>& quantitation_methods);
    std::vector<OpenMS::AbsoluteQuantitationMethod>& getQuantitationMethods();
    std::vector<OpenMS::AbsoluteQuantitationMethod> getQuantitationMethods() const;

    void setComponentsToConcentrations(
      const std::map<std::string, std::vector<OpenMS::AbsoluteQuantitationStandards::featureConcentration>> components_to_concentrations
    );

    std::map<std::string, std::vector<OpenMS::AbsoluteQuantitationStandards::featureConcentration>>&
    getComponentsToConcentrations();

    std::map<std::string, std::vector<OpenMS::AbsoluteQuantitationStandards::featureConcentration>>
    getComponentsToConcentrations() const;

private:
    std::string sequence_segment_name_;
    std::vector<size_t> sample_indices_;
    std::vector<OpenMS::AbsoluteQuantitationStandards::runConcentration> standards_concentrations_;
    std::vector<OpenMS::AbsoluteQuantitationMethod> quantitation_methods_;
    std::map<std::string, std::vector<OpenMS::AbsoluteQuantitationStandards::featureConcentration>> components_to_concentrations_;
  };
}