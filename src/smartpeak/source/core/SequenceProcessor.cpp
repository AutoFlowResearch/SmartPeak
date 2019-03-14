// TODO: Add copyright

#include <SmartPeak/core/SequenceProcessor.h>
#include <SmartPeak/core/Filenames.h>
#include <SmartPeak/core/RawDataProcessor.h>
#include <SmartPeak/core/SequenceSegmentProcessor.h>
#include <SmartPeak/io/InputDataValidation.h>
#include <SmartPeak/io/SequenceParser.h>

namespace SmartPeak
{
  void SequenceProcessor::createSequence(
    SequenceHandler& sequenceHandler_IO,
    const Filenames& filenames,
    const std::string& delimiter,
    const bool checkConsistency,
    const bool verbose_I
  )
  {
    if (verbose_I) {
      std::cout << "==== START createSequence()" << std::endl;
    }

    SequenceParser::readSequenceFile(sequenceHandler_IO, filenames.sequence_csv_i, delimiter);
    if (sequenceHandler_IO.getSequence().empty()) {
      std::cout << "Empty sequence. Returning.\n"
        "==== END   createSequence()" << std::endl;
      return;
    }
    // TODO: Given that the raw data is shared between all injections, it could
    // be beneficial to move it somewhere else (i.e. in SequenceHandler) and adapt
    // the algorithms to the change
    RawDataHandler& rawDataHandler = sequenceHandler_IO.getSequence()[0].getRawData();

    // load rawDataHandler files (applies to the whole session)
    LoadParameters loadParameters;
    loadParameters.process(rawDataHandler, {}, filenames, verbose_I);

    LoadTransitions loadTransitions;
    loadTransitions.process(rawDataHandler, {}, filenames, verbose_I);

    LoadFeatureFilters loadFeatureFilters;
    loadFeatureFilters.process(rawDataHandler, {}, filenames, verbose_I);

    LoadFeatureQCs loadFeatureQCs;
    loadFeatureQCs.process(rawDataHandler, {}, filenames, verbose_I);
    // raw data files (i.e., mzML, trafo, etc., will be loaded dynamically)

    // load sequenceSegmentHandler files
    for (SequenceSegmentHandler& sequenceSegmentHandler: sequenceHandler_IO.getSequenceSegments()) {
      LoadQuantitationMethods loadQuantitationMethods;
      loadQuantitationMethods.process(sequenceSegmentHandler, SequenceHandler(), {}, filenames, verbose_I);
      LoadStandardsConcentrations loadStandardsConcentrations;
      loadStandardsConcentrations.process(sequenceSegmentHandler, SequenceHandler(), {}, filenames, verbose_I);
    }

    if (checkConsistency) {
      InputDataValidation::sampleNamesAreConsistent(sequenceHandler_IO);
      InputDataValidation::componentNamesAreConsistent(sequenceHandler_IO);
      InputDataValidation::componentNameGroupsAreConsistent(sequenceHandler_IO);
      InputDataValidation::heavyComponentsAreConsistent(sequenceHandler_IO);
    }

    if (verbose_I) {
      std::cout << "==== END   createSequence()" << std::endl;
    }
  }

  void SequenceProcessor::processSequence(
    SequenceHandler& sequenceHandler_IO,
    const std::map<std::string, Filenames>& filenames,
    const std::set<std::string>& injection_names,
    const std::vector<std::shared_ptr<RawDataProcessor>>& raw_data_processing_methods_I,
    const bool verbose_I
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
        if (verbose_I) {
          std::cout << "\n[" << (i + 1) << "/" << n << "]" << std::endl;
        }
        raw_data_processing_methods_I[i]->process(injection.getRawData(), 
          injection.getRawData().getParameters(), 
          filenames.at(injection.getMetaData().getInjectionName()), 
          verbose_I);
      }
    }
  }

  void SequenceProcessor::processSequenceSegments(
    SequenceHandler& sequenceHandler_IO,
    const std::map<std::string, Filenames>& filenames,
    const std::set<std::string>& sequence_segment_names,
    const std::vector<std::shared_ptr<SequenceSegmentProcessor>>& sequence_segment_processing_methods_I,
    const bool verbose_I
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

      // process the sequence segment
      for (const std::shared_ptr<SequenceSegmentProcessor> sequence_segment_processing_method : sequence_segment_processing_methods_I) {
        sequence_segment_processing_method->process(
          sequence_segment,
          sequenceHandler_IO,
          sequenceHandler_IO
            .getSequence()
            .at(sequence_segment.getSampleIndices().front())
            .getRawData()
            .getParameters(), // assumption: all parameters are the same for each sample in the sequence segment!
          filenames.at(sequence_segment.getSequenceSegmentName()),
          verbose_I
        );
      }
    }

    sequenceHandler_IO.setSequenceSegments(sequence_segments);
  }
}
