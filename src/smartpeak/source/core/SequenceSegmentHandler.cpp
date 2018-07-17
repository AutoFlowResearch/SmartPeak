// TODO: Add copyright

#include <SmartPeak/core/SequenceSegmentHandler.h>

namespace SmartPeak
{
  void SequenceSegmentHandler::clear_data()
  {
    sequence_segment_name_.clear();
    sample_indices_.clear();
    // TODO: implement the following two clears
    // STD_CONC_TYPE standards_concentrations_.clear();
    // OpenMS::AbsoluteQuantitationMethod quantitation_methods_;
    components_to_concentrations_.clear();
  }

  void SequenceSegmentHandler::setSequenceSegmentName(std::string& sequence_segment_name)
  {
    sequence_segment_name_ = sequence_segment_name;
  }

  std::string& SequenceSegmentHandler::getSequenceSegmentName()
  {
    return sequence_segment_name_;
  }

  std::string SequenceSegmentHandler::getSequenceSegmentName() const
  {
    return sequence_segment_name_;
  }

  void SequenceSegmentHandler::setSampleIndices(std::vector<size_t>& sample_indices)
  {
    sample_indices_ = sample_indices;
  }

  std::vector<size_t>& SequenceSegmentHandler::getSampleIndices()
  {
    return sample_indices_;
  }

  std::vector<size_t> SequenceSegmentHandler::getSampleIndices() const
  {
    return sample_indices_;
  }

  // void SequenceSegmentHandler::setStandardsConcentrations(STD_CONC_TYPE& standards_concentrations)
  // {
  //   standards_concentrations_ = standards_concentrations;
  // }

  // STD_CONC_TYPE& SequenceSegmentHandler::getStandardsConcentrations()
  // {
  //   return standards_concentrations_;
  // }

  // STD_CONC_TYPE SequenceSegmentHandler::getStandardsConcentrations() const
  // {
  //   return standards_concentrations_;
  // }

  void SequenceSegmentHandler::setQuantitationMethods(OpenMS::AbsoluteQuantitationMethod& quantitation_methods)
  {
    quantitation_methods_ = quantitation_methods;
  }

  OpenMS::AbsoluteQuantitationMethod& SequenceSegmentHandler::getQuantitationMethods()
  {
    return quantitation_methods_;
  }

  OpenMS::AbsoluteQuantitationMethod SequenceSegmentHandler::getQuantitationMethods() const
  {
    return quantitation_methods_;
  }

  void SequenceSegmentHandler::setComponentsToConcentrations(
    std::map<std::string, std::vector<OpenMS::AbsoluteQuantitationStandards::featureConcentration>> components_to_concentrations
  )
  {
    components_to_concentrations_ = components_to_concentrations;
  }

  std::map<std::string, std::vector<OpenMS::AbsoluteQuantitationStandards::featureConcentration>>&
  SequenceSegmentHandler::getComponentsToConcentrations()
  {
    return components_to_concentrations_;
  }

  std::map<std::string, std::vector<OpenMS::AbsoluteQuantitationStandards::featureConcentration>>
  SequenceSegmentHandler::getComponentsToConcentrations() const
  {
    return components_to_concentrations_;
  }
}
