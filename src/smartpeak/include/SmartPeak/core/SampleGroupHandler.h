// TODO: Add copyright

#pragma once

#include <string>
#include <vector>

namespace SmartPeak
{
  class SampleGroupHandler
  {
public:
    SampleGroupHandler() = default;
    ~SampleGroupHandler() = default;

    void setSampleGroupName(const std::string& sample_group_name);
    std::string& getSampleGroupName();
    const std::string& getSampleGroupName() const;

    void setSampleIndices(const std::vector<size_t>& sample_indices);
    std::vector<size_t>& getSampleIndices();
    const std::vector<size_t>& getSampleIndices() const;

private:
    std::string sample_group_name_;
    std::vector<size_t> sample_indices_;
  };
}
