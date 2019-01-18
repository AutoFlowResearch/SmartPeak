// TODO: Add copyright

#pragma once
#include <SmartPeak/core/Filenames.h>
#include <SmartPeak/core/RawDataHandler.h>
#include <SmartPeak/core/SequenceHandler.h>
#include <SmartPeak/core/SequenceSegmentHandler.h>
#include <vector>

namespace SmartPeak
{
  class SequenceProcessor
  {
public:
    SequenceProcessor() = delete;
    ~SequenceProcessor() = delete;

    static void createSequence(
      SequenceHandler& sequenceHandler_IO,
      const Filenames& filenames,
      const std::string& delimiter = ",",
      const bool verbose_I = true
    );

    static void addRawDataHandlerToSequence(
      SequenceHandler& sequenceHandler_IO,
      const RawDataHandler& rawDataHandler
    );

    static void segmentSamplesInSequence(
      SequenceHandler& sequenceHandler_IO,
      const SequenceSegmentHandler& sequenceSegmentHandler_I = SequenceSegmentHandler()
    );

    static void processSequence(
      SequenceHandler& sequenceHandler_IO,
      const std::map<std::string, Filenames>& filenames,
      const std::vector<std::string>& injection_names = std::vector<std::string>(),
      const std::vector<std::string>& raw_data_processing_methods_I = std::vector<std::string>(),
      const bool verbose_I = true
    );

    static void processSequenceSegments(
      SequenceHandler& sequenceHandler_IO,
      const std::map<std::string, Filenames>& filenames,
      const std::set<std::string>& sequence_segment_names = std::set<std::string>(),
      const std::vector<std::string>& sequence_segment_processing_methods_I = std::vector<std::string>(),
      const bool verbose_I = true
    );
  };
}
