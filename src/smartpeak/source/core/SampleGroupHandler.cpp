// TODO: Add copyright

#include <SmartPeak/core/SampleGroupHandler.h>

namespace SmartPeak
{
  void SampleGroupHandler::clear() {
    feature_map_.clear();
  }

  void SampleGroupHandler::setSampleGroupName(const std::string& sample_group_name)
  {
    sample_group_name_ = sample_group_name;
  }

  std::string& SampleGroupHandler::getSampleGroupName()
  {
    return sample_group_name_;
  }

  const std::string& SampleGroupHandler::getSampleGroupName() const
  {
    return sample_group_name_;
  }

  void SampleGroupHandler::setSampleIndices(const std::vector<size_t>& sample_indices)
  {
    sample_indices_ = sample_indices;
  }

  std::vector<size_t>& SampleGroupHandler::getSampleIndices()
  {
    return sample_indices_;
  }

  const std::vector<size_t>& SampleGroupHandler::getSampleIndices() const
  {
    return sample_indices_;
  }

  void SampleGroupHandler::setFeatureMap(const OpenMS::FeatureMap& feature_map)
  {
    feature_map_ = feature_map;
  }

  OpenMS::FeatureMap& SampleGroupHandler::getFeatureMap()
  {
    return feature_map_;
  }

  const OpenMS::FeatureMap& SampleGroupHandler::getFeatureMap() const
  {
    return feature_map_;
  }
}
