// TODO: Add copyright

#include <SmartPeak/core/SampleGroupHandler.h>

namespace SmartPeak
{
    void SampleGroupHandler::setSampleGroupName(std::string& sample_group_name)
    {
      sample_group_name_ = sample_group_name;
    }

    const std::string& SampleGroupHandler::getSampleGroupName() const
    {
      return sample_group_name_;
    }

    void SampleGroupHandler::setSampleIndices(std::vector<size_t>& sample_indices)
    {
      sample_indices_ = sample_indices;
    }

    const std::vector<size_t>& SampleGroupHandler::getSampleIndices() const
    {
      return sample_indices_;
    }
}
