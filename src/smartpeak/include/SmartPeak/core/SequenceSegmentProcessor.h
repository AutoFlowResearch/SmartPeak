// TODO: Add copyright

#pragma once

#include <SmartPeak/core/Filenames.h>
#include <SmartPeak/core/MetaDataHandler.h>
#include <SmartPeak/core/SequenceHandler.h>
#include <SmartPeak/core/SequenceSegmentHandler.h>

namespace SmartPeak
{
  class SequenceSegmentProcessor
  {
public:
    SequenceSegmentProcessor() = delete;
    ~SequenceSegmentProcessor() = delete;

    enum SeqSegProcMethod {
      CALCULATE_CALIBRATION = 1,
      STORE_QUANTITATION_METHODS,
      LOAD_QUANTITATION_METHODS,
    };

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
      const bool verbose_I = false
    );

    // TODO: Class SequenceSegmentPlotter is missing
    // static void plotCalibrators(
    //   const SequenceSegmentHandler& sequenceSegmentHandler_I,
    //   const std::string& calibrators_pdf_o,
    //   const std::vector<std::map<std::string, std::string>>& SequenceSegmentPlotter_params_I,
    //   const bool verbose_I = false
    // );

    static void processSequenceSegment(
      SequenceSegmentHandler& sequenceSegmentHandler_IO,
      SequenceHandler& sequenceHandler_IO,
      const SeqSegProcMethod sequence_segment_processing_event,
      const std::map<std::string, std::vector<std::map<std::string, std::string>>>& parameters,
      const Filenames& filenames,
      const bool verbose_I = false
    );

    static std::vector<SeqSegProcMethod> getDefaultSequenceSegmentProcessingWorkflow(
      const MetaDataHandler::SampleType sample_type
    );

    static bool checkSequenceSegmentProcessing(
      const std::vector<std::string>& sequence_segment_processing
    );
  };
}
