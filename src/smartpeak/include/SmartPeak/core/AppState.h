#pragma once

#include <SmartPeak/core/Filenames.h>
#include <SmartPeak/core/RawDataProcessor.h>
#include <SmartPeak/core/SampleType.h>
#include <SmartPeak/core/SequenceSegmentProcessor.h>
#include <SmartPeak/io/InputDataValidation.h>
#include <map>
#include <string>
#include <vector>

namespace SmartPeak
{
  struct AppState final {
    class Command {
    public:
      enum CommandType {
        RawDataMethod,
        SequenceSegmentMethod,
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

      int getID() const
      {
        return type == RawDataMethod ? raw_data_method->getID() : seq_seg_method->getID();
      }

      std::string getName() const
      {
        return type == RawDataMethod ? raw_data_method->getName() : seq_seg_method->getName();
      }

      std::shared_ptr<RawDataProcessor> raw_data_method;
      std::shared_ptr<SequenceSegmentProcessor> seq_seg_method;

      std::map<std::string, Filenames> dynamic_filenames;
    };

    std::string                     pathnamesFilename_       = "pathnames.txt"; // TODO: remove this
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
