// TODO: Add copyright

#pragma once

#include <SmartPeak/core/SequenceSegmentHandler.h>
#include <SmartPeak/core/MetaDataHandler.h>
#include <SmartPeak/core/SequenceHandler.h>

namespace SmartPeak
{
  class SequenceSegmentProcessor
  {
public:
    SequenceSegmentProcessor() = delete;
    ~SequenceSegmentProcessor() = delete;

    static void getSampleIndicesBySampleType(
      const SequenceSegmentHandler& sequenceSegmentHandler,
      const SequenceHandler& sequenceHandler,
      const MetaDataHandler::SampleType sampleType,
      std::vector<size_t>& sampleIndices
    );

    static void optimizeCalibrationCurves(
      SequenceSegmentHandler& sequenceSegmentHandler_IO,
      const SequenceHandler& sequenceHandler_I,
      const std::vector<std::map<std::string, std::string>>& AbsoluteQuantitation_params_I,
      const bool verbose_I
    );

    static void plotCalibrators(
      const SequenceSegmentHandler& sequenceSegmentHandler_I,
      const std::string& calibrators_pdf_o,
      const std::vector<std::map<std::string, std::string>>& SequenceSegmentPlotter_params_I,
      const bool verbose_I
    );

    static void processSequenceSegment(
      SequenceSegmentHandler& sequenceSegmentHandler_IO,
      const SequenceHandler& sequenceHandler_I,
      const std::string& sequence_segment_processing_event,
      const std::map<std::string, std::vector<std::map<std::string, std::string>>>& parameters,
      const std::map<std::string, std::string>& filenames,
      const bool verbose_I
    );

    static void getDefaultSequenceSegmentProcessingWorkflow(
      const MetaDataHandler::SampleType sample_type,
      std::vector<std::string>& default_workflow
    );

    static bool checkRawDataProcessingWorkflow(
      const std::vector<std::string>& sequence_segment_processing
    );
  };
}
