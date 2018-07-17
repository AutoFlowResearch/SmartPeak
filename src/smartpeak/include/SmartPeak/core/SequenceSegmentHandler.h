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

    // void setStandardsConcentrations(STD_CONC_TYPE& standards_concentrations);
    // STD_CONC_TYPE& getStandardsConcentrations();
    // STD_CONC_TYPE getStandardsConcentrations() const;

    void setQuantitationMethods(OpenMS::AbsoluteQuantitationMethod& quantitation_methods);
    OpenMS::AbsoluteQuantitationMethod& getQuantitationMethods();
    OpenMS::AbsoluteQuantitationMethod getQuantitationMethods() const;

    void setComponentsToConcentrations(
      std::map<std::string, std::vector<OpenMS::AbsoluteQuantitationStandards::featureConcentration>> components_to_concentrations
    );

    std::map<std::string, std::vector<OpenMS::AbsoluteQuantitationStandards::featureConcentration>>&
    getComponentsToConcentrations();

    std::map<std::string, std::vector<OpenMS::AbsoluteQuantitationStandards::featureConcentration>>
    getComponentsToConcentrations() const;

    // void getDefaultSampleOrder(sample_type):
    //     """Return the default order for each sample in a group

    //     Args:
    //         sample_type (str): type of sample

    //     Returns:
    //         int: order

    //     """

    //     order = -1
    //     if sample_type == "Standard":
    //         order = 0
    //     elif sample_type == "Unknown":
    //         order = 1
    //     elif sample_type == "QC":
    //         order = 2
    //     elif sample_type == "Blank":
    //         order = 3
    //     elif sample_type == "Double Blank":
    //         order = 3
    //     elif sample_type == "Solvent":
    //         pass

    //     return order

private:
    std::string sequence_segment_name_;
    std::vector<size_t> sample_indices_;
    // STD_CONC_TYPE standards_concentrations_;
    OpenMS::AbsoluteQuantitationMethod quantitation_methods_;
    std::map<std::string, std::vector<OpenMS::AbsoluteQuantitationStandards::featureConcentration>> components_to_concentrations_;
  };
}