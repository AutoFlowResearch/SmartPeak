#pragma once

#include <map>
#include <memory>
#include <string>

namespace SmartPeak {
  struct RawDataProcessor;
  struct SequenceSegmentProcessor;
  struct SampleGroupProcessor;
  extern const std::map<std::string, std::shared_ptr<RawDataProcessor>> n_to_raw_data_method_;
  extern const std::map<std::string, std::shared_ptr<SequenceSegmentProcessor>> n_to_seq_seg_method_;
  extern const std::map<std::string, std::shared_ptr<SampleGroupProcessor>> n_to_sample_group_method_;
}
