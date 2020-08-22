// TODO: Add copyright

#pragma once

#include <OpenMS/KERNEL/FeatureMap.h>
#include <string>
#include <vector>

namespace SmartPeak
{
  class SampleGroupHandler
  {
public:
    void clear();

    void setSampleGroupName(const std::string& sample_group_name);
    std::string& getSampleGroupName();
    const std::string& getSampleGroupName() const;

    void setSampleIndices(const std::vector<size_t>& sample_indices);
    std::vector<size_t>& getSampleIndices();
    const std::vector<size_t>& getSampleIndices() const;

    void setFeatureMap(const OpenMS::FeatureMap& feature_map);
    OpenMS::FeatureMap& getFeatureMap();
    const OpenMS::FeatureMap& getFeatureMap() const;

private:
    std::string sample_group_name_;
    std::vector<size_t> sample_indices_;
    OpenMS::FeatureMap feature_map_; ///< merged featuremap for the group
  };
}
