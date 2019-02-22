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

    /**
      Return all injection indices that belong to a given sample type.

      @param[in] sequenceSegmentHandler Sequence segment handler
      @param[in] sequenceHandler Sequence handler
      @param[in] sampleType Sample type filter
      @param[out] sampleIndices Output sample indices
    */
    static void getSampleIndicesBySampleType(
      const SequenceSegmentHandler& sequenceSegmentHandler,
      const SequenceHandler& sequenceHandler,
      const MetaDataHandler::SampleType sampleType,
      std::vector<size_t>& sampleIndices
    );

    /**
      Optimize the calibration curve for all components.

      @param[in,out] sequenceSegmentHandler_IO Sequence segment handler
      @param[in] sequenceHandler_I Sequence handler
      @param[in] AbsoluteQuantitation_params_I Parameters
      @param[in] verbose_I Verbosity
    */
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

    /**
      Apply processing methods to a raw data handler.

      @param[in,out] sequenceSegmentHandler_IO Sequence segment handler
      @param[in] sequenceHandler_IO Sequence handler
      @param[in] sequence_segment_processing_event Event to process
      @param[in] parameters Parameters
      @param[in] filenames Pathnames info
      @param[in] verbose_I Verbosity
    */
    static void processSequenceSegment(
      SequenceSegmentHandler& sequenceSegmentHandler_IO,
      SequenceHandler& sequenceHandler_IO,
      const SeqSegProcMethod sequence_segment_processing_event,
      const std::map<std::string, std::vector<std::map<std::string, std::string>>>& parameters,
      const Filenames& filenames,
      const bool verbose_I = false
    );

    /**
      Return the default workflow events for a given sequence.

      @param[in] sample_type Sample type
    */
    static std::vector<SeqSegProcMethod> getDefaultSequenceSegmentProcessingWorkflow(
      const MetaDataHandler::SampleType sample_type
    );

    /**
      Check the sequence processing steps.

      @param[in] sequence_segment_processing List of sequence group processing events
    */
    static bool checkSequenceSegmentProcessing(
      const std::vector<std::string>& sequence_segment_processing
    );
  };
}
