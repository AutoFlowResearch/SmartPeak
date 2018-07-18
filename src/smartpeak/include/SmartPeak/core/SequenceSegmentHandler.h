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

    void clear_data();

    void setSequenceSegmentName(std::string& sequence_segment_name);
    std::string& getSequenceSegmentName();
    std::string getSequenceSegmentName() const;

    void setSampleIndices(std::vector<size_t>& sample_indices);
    std::vector<size_t>& getSampleIndices();
    std::vector<size_t> getSampleIndices() const;

    void setStandardsConcentrations(std::vector<OpenMS::AbsoluteQuantitationStandards>& standards_concentrations);
    std::vector<OpenMS::AbsoluteQuantitationStandards>& getStandardsConcentrations();
    std::vector<OpenMS::AbsoluteQuantitationStandards> getStandardsConcentrations() const;

    void setQuantitationMethods(std::vector<OpenMS::AbsoluteQuantitationMethod>& quantitation_methods);
    std::vector<OpenMS::AbsoluteQuantitationMethod>& getQuantitationMethods();
    std::vector<OpenMS::AbsoluteQuantitationMethod> getQuantitationMethods() const;

    void setComponentsToConcentrations(
      std::map<std::string, std::vector<OpenMS::AbsoluteQuantitationStandards::featureConcentration>> components_to_concentrations
    );

    std::map<std::string, std::vector<OpenMS::AbsoluteQuantitationStandards::featureConcentration>>&
    getComponentsToConcentrations();

    std::map<std::string, std::vector<OpenMS::AbsoluteQuantitationStandards::featureConcentration>>
    getComponentsToConcentrations() const;

private:
    std::string sequence_segment_name_;
    std::vector<size_t> sample_indices_;
    std::vector<OpenMS::AbsoluteQuantitationStandards> standards_concentrations_;
    std::vector<OpenMS::AbsoluteQuantitationMethod> quantitation_methods_;
    std::map<std::string, std::vector<OpenMS::AbsoluteQuantitationStandards::featureConcentration>> components_to_concentrations_;
  };
}