// TODO: Add copyright

#pragma once

#include <SmartPeak/core/SampleGroupHandler.h>
#include <SmartPeak/core/SampleHandler.h>
#include <SmartPeak/core/SequenceSegmentHandler.h>

namespace SmartPeak
{
  class SequenceHandler
  {
public:
    SequenceHandler() = default;
    ~SequenceHandler() = default;

private:
    std::vector<SampleHandler> sequence_;
    std::map<size_t, std::string> index_to_sample_;
    std::map<std::string, size_t> sample_to_index_;
    std::vector<SequenceSegmentHandler> sequence_segments_;
    std::vector<SampleGroupHandler> sample_groups_;

    std::string dir_static_;
    std::string dir_dynamic_;
    std::vector<std::string> filenames_;
  };
}
