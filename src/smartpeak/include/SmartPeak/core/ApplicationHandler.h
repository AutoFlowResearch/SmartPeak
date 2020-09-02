#pragma once

#include <SmartPeak/core/Filenames.h>
#include <SmartPeak/core/RawDataProcessor.h>
#include <SmartPeak/core/SampleType.h>
#include <SmartPeak/core/SequenceSegmentProcessor.h>
#include <SmartPeak/core/SampleGroupProcessor.h>
#include <SmartPeak/io/InputDataValidation.h>
#include <map>
#include <string>
#include <vector>

namespace SmartPeak
{
  struct ApplicationHandler final {
    class Command {
    public:
      enum CommandType {
        RawDataMethod,
        SequenceSegmentMethod,
        SampleGroupMethod,
      } type;

      void setMethod(const std::shared_ptr<RawDataProcessor> method)
      {
        type = RawDataMethod;
        raw_data_method = method;
      }

      void setMethod(const std::shared_ptr<SequenceSegmentProcessor> method)
      {
        type = SequenceSegmentMethod;
        seq_seg_method = method;
      }

      void setMethod(const std::shared_ptr<SampleGroupProcessor> method)
      {
        type = SampleGroupMethod;
        sample_group_method = method;
      }

      int getID() const
      {
        if (type == RawDataMethod)
          return raw_data_method->getID();
        else if (type == SequenceSegmentMethod)
          return seq_seg_method->getID();
        else
          return sample_group_method->getID();
      }

      std::string getName() const
      {
        if (type == RawDataMethod)
          return raw_data_method->getName();
        else if (type == SequenceSegmentMethod)
          return seq_seg_method->getName();
        else
          return sample_group_method->getName();
      }

      std::shared_ptr<RawDataProcessor> raw_data_method;
      std::shared_ptr<SequenceSegmentProcessor> seq_seg_method;
      std::shared_ptr<SampleGroupProcessor> sample_group_method;

      std::map<std::string, Filenames> dynamic_filenames;
    };

    std::string                           sequence_pathname_;
    std::string                           main_dir_                = ".";
    std::string                           mzML_dir_;
    std::string                           features_in_dir_;
    std::string                           features_out_dir_;
    std::vector<Command>                  commands_;
    Filenames                             static_filenames_;
    SequenceHandler                       sequenceHandler_;
  };
}
