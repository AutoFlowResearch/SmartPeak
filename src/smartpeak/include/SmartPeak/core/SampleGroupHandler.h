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

    std::string sample_group_name;
    std::vector<size_t> sample_indices;
  };
}
