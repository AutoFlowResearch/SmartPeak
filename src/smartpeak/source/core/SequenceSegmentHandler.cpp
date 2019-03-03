// TODO: Add copyright

#include <SmartPeak/core/SequenceSegmentHandler.h>

namespace SmartPeak
{
  SequenceSegmentHandler::SequenceSegmentHandler():
    quantitation_methods_(new std::vector<OpenMS::AbsoluteQuantitationMethod>())
  {
  }

  void SequenceSegmentHandler::clear()
  {
    sequence_segment_name_.clear();
    sample_indices_.clear();
    standards_concentrations_.clear();
    if (quantitation_methods_!=nullptr) quantitation_methods_->clear();
    components_to_concentrations_.clear();
  }

  void SequenceSegmentHandler::setSequenceSegmentName(const std::string& sequence_segment_name)
  {
    sequence_segment_name_ = sequence_segment_name;
  }

  std::string& SequenceSegmentHandler::getSequenceSegmentName()
  {
    return sequence_segment_name_;
  }

  const std::string& SequenceSegmentHandler::getSequenceSegmentName() const
  {
    return sequence_segment_name_;
  }

  void SequenceSegmentHandler::setSampleIndices(const std::vector<size_t>& sample_indices)
  {
    sample_indices_ = sample_indices;
  }

  std::vector<size_t>& SequenceSegmentHandler::getSampleIndices()
  {
    return sample_indices_;
  }

  const std::vector<size_t>& SequenceSegmentHandler::getSampleIndices() const
  {
    return sample_indices_;
  }

  void SequenceSegmentHandler::setStandardsConcentrations(const std::vector<OpenMS::AbsoluteQuantitationStandards::runConcentration>& standards_concentrations)
  {
    standards_concentrations_ = standards_concentrations;
  }

  std::vector<OpenMS::AbsoluteQuantitationStandards::runConcentration>& SequenceSegmentHandler::getStandardsConcentrations()
  {
    return standards_concentrations_;
  }

  const std::vector<OpenMS::AbsoluteQuantitationStandards::runConcentration>& SequenceSegmentHandler::getStandardsConcentrations() const
  {
    return standards_concentrations_;
  }

  void SequenceSegmentHandler::setQuantitationMethods(const std::vector<OpenMS::AbsoluteQuantitationMethod>& quantitation_methods)
  {
    quantitation_methods_.reset(new std::vector<OpenMS::AbsoluteQuantitationMethod>(quantitation_methods));
  }

  void SequenceSegmentHandler::setQuantitationMethods(std::shared_ptr<std::vector<OpenMS::AbsoluteQuantitationMethod>>& quantitation_methods)
  {
    quantitation_methods_ = quantitation_methods;
  }

  std::vector<OpenMS::AbsoluteQuantitationMethod>& SequenceSegmentHandler::getQuantitationMethods()
  {
    return *(quantitation_methods_.get());
  }

  const std::vector<OpenMS::AbsoluteQuantitationMethod>& SequenceSegmentHandler::getQuantitationMethods() const
  {
    return *(quantitation_methods_.get());
  }

  std::shared_ptr<std::vector<OpenMS::AbsoluteQuantitationMethod>>& SequenceSegmentHandler::getQuantitationMethodsShared()
  {
    return quantitation_methods_;
  }

  void SequenceSegmentHandler::setComponentsToConcentrations(
    const std::map<std::string, std::vector<OpenMS::AbsoluteQuantitationStandards::featureConcentration>> components_to_concentrations
  )
  {
    components_to_concentrations_ = components_to_concentrations;
  }

  std::map<std::string, std::vector<OpenMS::AbsoluteQuantitationStandards::featureConcentration>>&
  SequenceSegmentHandler::getComponentsToConcentrations()
  {
    return components_to_concentrations_;
  }

  const std::map<std::string, std::vector<OpenMS::AbsoluteQuantitationStandards::featureConcentration>>&
  SequenceSegmentHandler::getComponentsToConcentrations() const
  {
    return components_to_concentrations_;
  }
}
