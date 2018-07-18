// TODO: Add copyright

#include <SmartPeak/core/SequenceSegmentHandler.h>

namespace SmartPeak
{
  void SequenceSegmentHandler::clear_data() // TODO: rename to clear() ?
  {
    sequence_segment_name_.clear();
    sample_indices_.clear();
    standards_concentrations_.clear();
    quantitation_methods_.clear();
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

  void SequenceSegmentHandler::setStandardsConcentrations(std::vector<OpenMS::AbsoluteQuantitationStandards>& standards_concentrations)
  {
    standards_concentrations_ = standards_concentrations;
  }

  std::vector<OpenMS::AbsoluteQuantitationStandards>& SequenceSegmentHandler::getStandardsConcentrations()
  {
    return standards_concentrations_;
  }

  std::vector<OpenMS::AbsoluteQuantitationStandards> SequenceSegmentHandler::getStandardsConcentrations() const
  {
    return standards_concentrations_;
  }

  void SequenceSegmentHandler::setQuantitationMethods(std::vector<OpenMS::AbsoluteQuantitationMethod>& quantitation_methods)
  {
    quantitation_methods_ = quantitation_methods;
  }

  std::vector<OpenMS::AbsoluteQuantitationMethod>& SequenceSegmentHandler::getQuantitationMethods()
  {
    return quantitation_methods_;
  }

  std::vector<OpenMS::AbsoluteQuantitationMethod> SequenceSegmentHandler::getQuantitationMethods() const
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
