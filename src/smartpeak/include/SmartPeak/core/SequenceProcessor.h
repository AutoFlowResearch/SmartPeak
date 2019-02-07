// TODO: Add copyright

#pragma once
#include <SmartPeak/core/Filenames.h>
#include <SmartPeak/core/RawDataHandler.h>
#include <SmartPeak/core/RawDataProcessor.h>
#include <SmartPeak/core/SequenceHandler.h>
#include <SmartPeak/core/SequenceSegmentHandler.h>
#include <SmartPeak/core/SequenceSegmentProcessor.h>
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
      const std::set<std::string>& injection_names = std::set<std::string>(),
      const std::vector<RawDataProcessor::RawDataProcMethod>& raw_data_processing_methods_I =
        std::vector<RawDataProcessor::RawDataProcMethod>(),
      const bool verbose_I = true
    );

    static void processSequenceSegments(
      SequenceHandler& sequenceHandler_IO,
      const std::map<std::string, Filenames>& filenames,
      const std::set<std::string>& sequence_segment_names = std::set<std::string>(),
      const std::vector<SequenceSegmentProcessor::SeqSegProcMethod>& sequence_segment_processing_methods_I =
        std::vector<SequenceSegmentProcessor::SeqSegProcMethod>(),
      const bool verbose_I = true
    );
  };
}
