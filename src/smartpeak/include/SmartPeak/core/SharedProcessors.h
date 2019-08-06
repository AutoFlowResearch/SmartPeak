#pragma once

#include <map>
#include <memory>

namespace SmartPeak {
  struct RawDataProcessor;
  struct SequenceSegmentProcessor;
  extern const std::map<int, std::shared_ptr<RawDataProcessor>> n_to_raw_data_method_;
  extern const std::map<int, std::shared_ptr<SequenceSegmentProcessor>> n_to_seq_seg_method_;
}
