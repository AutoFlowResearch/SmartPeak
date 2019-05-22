// TODO: Add copyright

#include <SmartPeak/core/SequenceProcessor.h>
#include <SmartPeak/core/Filenames.h>
#include <SmartPeak/core/RawDataProcessor.h>
#include <SmartPeak/core/SequenceSegmentProcessor.h>
#include <SmartPeak/io/InputDataValidation.h>
#include <SmartPeak/io/SequenceParser.h>
#include <plog/Log.h>

namespace SmartPeak
{
  void SequenceProcessor::createSequence(
    SequenceHandler& sequenceHandler_IO,
    const Filenames& filenames,
    const std::string& delimiter,
    const bool checkConsistency
  )
  {
    LOGD << "START createSequence";

    SequenceParser::readSequenceFile(sequenceHandler_IO, filenames.sequence_csv_i, delimiter);
    if (sequenceHandler_IO.getSequence().empty()) {
      LOGE << "Empty sequence. Returning";
      LOGD << "END createSequence";
      return;
    }
    // TODO: Given that the raw data is shared between all injections, it could
    // be beneficial to move it somewhere else (i.e. in SequenceHandler) and adapt
    // the algorithms to the change
    RawDataHandler& rawDataHandler = sequenceHandler_IO.getSequence()[0].getRawData();

    // load rawDataHandler files (applies to the whole session)
    LoadParameters loadParameters;
    loadParameters.process(rawDataHandler, {}, filenames);

    LoadTransitions loadTransitions;
    loadTransitions.process(rawDataHandler, {}, filenames);

    LoadFeatureFilters loadFeatureFilters;
    loadFeatureFilters.process(rawDataHandler, {}, filenames);

    LoadFeatureQCs loadFeatureQCs;
    loadFeatureQCs.process(rawDataHandler, {}, filenames);
    // raw data files (i.e., mzML, trafo, etc., will be loaded dynamically)

    LoadValidationData loadValidationData;
    loadValidationData.process(rawDataHandler, {}, filenames, verbose_I);

  const std::vector<std::map<std::string, Utilities::CastValue>>& ref_data = rawDataHandler.getReferenceData();

    // load sequenceSegmentHandler files
    for (SequenceSegmentHandler& sequenceSegmentHandler: sequenceHandler_IO.getSequenceSegments()) {
      LoadQuantitationMethods loadQuantitationMethods;
      loadQuantitationMethods.process(sequenceSegmentHandler, SequenceHandler(), {}, filenames);
      LoadStandardsConcentrations loadStandardsConcentrations;
      loadStandardsConcentrations.process(sequenceSegmentHandler, SequenceHandler(), {}, filenames);
    }

    if (checkConsistency) {
      InputDataValidation::sampleNamesAreConsistent(sequenceHandler_IO);
      InputDataValidation::componentNamesAreConsistent(sequenceHandler_IO);
      InputDataValidation::componentNameGroupsAreConsistent(sequenceHandler_IO);
      InputDataValidation::heavyComponentsAreConsistent(sequenceHandler_IO);
    }

    LOGD << "END createSequence";
  }

  void SequenceProcessor::processSequence(
    SequenceHandler& sequenceHandler_IO,
    const std::map<std::string, Filenames>& filenames,
    const std::set<std::string>& injection_names,
    const std::vector<std::shared_ptr<RawDataProcessor>>& raw_data_processing_methods_I
  )
  {
    std::vector<InjectionHandler> process_sequence = sequenceHandler_IO.getSamplesInSequence(injection_names);

    // handle user-desired samples
    if (injection_names.empty()) {
      process_sequence = sequenceHandler_IO.getSequence();
    }

    if (filenames.size() != process_sequence.size()) {
      throw std::invalid_argument("The number of provided filenames locations is not correct.");
    }

    // [OPTIMIZATION: add-in parallel execution here using
    //  `std::vector<std::future>>`, `std::packaged_task`, `std::thread`, and thread count/retrieval pattern]
    for (InjectionHandler& injection : process_sequence) {

      // handle user-desired raw_data_processing_methods
      if (raw_data_processing_methods_I.size() == 0) {
        throw "no raw data processing methods given.\n";
      }

      const size_t n = raw_data_processing_methods_I.size();

      // process the samples
      for (size_t i = 0; i < n; ++i) {
        LOGI << "[" << (i + 1) << "/" << n << "] steps in processing sequence";
        raw_data_processing_methods_I[i]->process(injection.getRawData(),
          injection.getRawData().getParameters(),
          filenames.at(injection.getMetaData().getInjectionName()));
      }
    }
  }

  void SequenceProcessor::processSequenceSegments(
    SequenceHandler& sequenceHandler_IO,
    const std::map<std::string, Filenames>& filenames,
    const std::set<std::string>& sequence_segment_names,
    const std::vector<std::shared_ptr<SequenceSegmentProcessor>>& sequence_segment_processing_methods_I
  )
  {
    std::vector<SequenceSegmentHandler> sequence_segments;

    if (sequence_segment_names.empty()) { // select all
      sequence_segments = sequenceHandler_IO.getSequenceSegments();
    } else { // select those with specific sequence segment names
      for (SequenceSegmentHandler& s : sequenceHandler_IO.getSequenceSegments()) {
        if (sequence_segment_names.count(s.getSequenceSegmentName())) {
          sequence_segments.push_back(s);
        }
      }
    }

    if (filenames.size() != sequence_segments.size()) {
      throw std::invalid_argument("The number of provided filenames locations is not correct.");
    }

    // process by sequence segment
    for (SequenceSegmentHandler& sequence_segment : sequence_segments) {

      // handle user-desired sequence_segment_processing_methods
      if (!sequence_segment_processing_methods_I.size()) {
        throw "no sequence segment processing methods given.\n";
      }

      const size_t n = sequence_segment_processing_methods_I.size();

      // process the sequence segment
      for (size_t i = 0; i < n; ++i) {
        LOGI << "[" << (i + 1) << "/" << n << "] steps in processing sequence segments";
        sequence_segment_processing_methods_I[i]->process(
          sequence_segment,
          sequenceHandler_IO,
          sequenceHandler_IO
            .getSequence()
            .at(sequence_segment.getSampleIndices().front())
            .getRawData()
            .getParameters(), // assumption: all parameters are the same for each sample in the sequence segment!
          filenames.at(sequence_segment.getSequenceSegmentName())
        );
      }
    }

    sequenceHandler_IO.setSequenceSegments(sequence_segments);
  }
}
