// TODO: Add copyright

#include <SmartPeak/core/SequenceProcessor.h>
#include <SmartPeak/core/Filenames.h>
#include <SmartPeak/core/RawDataProcessor.h>
#include <SmartPeak/core/SequenceSegmentProcessor.h>
#include <SmartPeak/io/InputDataValidation.h>
#include <SmartPeak/io/OpenMSFile.h>
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

    RawDataHandler rawDataHandler;
    SequenceSegmentHandler sequenceSegmentHandler;

    SequenceParser::readSequenceFile(sequenceHandler_IO, filenames.sequence_csv_i, delimiter);

    // load rawDataHandler files (applies to the whole session)
    OpenMSFile::readRawDataProcessingParameters(rawDataHandler, filenames.parameters_csv_i);

    OpenMSFile::loadTraML(rawDataHandler, filenames.traML_csv_i, "csv", verbose_I);

    OpenMSFile::loadFeatureFilter(
      rawDataHandler,
      filenames.featureFilterComponents_csv_i,
      filenames.featureFilterComponentGroups_csv_i,
      verbose_I
    );

    OpenMSFile::loadFeatureQC(
      rawDataHandler,
      filenames.featureQCComponents_csv_i,
      filenames.featureQCComponentGroups_csv_i,
      verbose_I
    );
    // raw data files (i.e., mzML, trafo, etc., will be loaded dynamically)

    // load sequenceSegmentHandler files
    OpenMSFile::loadQuantitationMethods(sequenceSegmentHandler, filenames.quantitationMethods_csv_i, verbose_I);
    OpenMSFile::loadStandardsConcentrations(sequenceSegmentHandler, filenames.standardsConcentrations_csv_i, verbose_I);

    // copy over the quantitation methods to the rawDataHandler
    rawDataHandler.setQuantitationMethods(sequenceSegmentHandler.getQuantitationMethods());

    // initialize the sequence
    segmentSamplesInSequence(sequenceHandler_IO, sequenceSegmentHandler);
    addRawDataHandlerToSequence(sequenceHandler_IO, rawDataHandler);

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

  void SequenceProcessor::addRawDataHandlerToSequence(
    SequenceHandler& sequenceHandler_IO,
    const RawDataHandler& rawDataHandler
  )
  {
    for (InjectionHandler& injection : sequenceHandler_IO.getSequence()) {
      // copy the object to persist the data
      injection.setRawData(rawDataHandler);
      // update the metadata for the rawDataHandler
      injection.getRawData().setMetaData(injection.getMetaData());
    }
  }

  void SequenceProcessor::segmentSamplesInSequence(
    SequenceHandler& sequenceHandler_IO,
    const SequenceSegmentHandler& sequenceSegmentHandler_I
  )
  {
    const std::vector<InjectionHandler>& sequence = sequenceHandler_IO.getSequence();

    std::map<std::string, std::vector<size_t>> sequence_segments_dict;

    for (size_t i = 0; i < sequence.size(); ++i) {
      const std::string sequence_segment_name = sequence[i].getMetaData().getSequenceSegmentName();
      if (0 == sequence_segments_dict.count(sequence_segment_name))
        sequence_segments_dict.emplace(sequence_segment_name, std::vector<size_t>());
      sequence_segments_dict.at(sequence_segment_name).push_back(i);
    }

    std::vector<SequenceSegmentHandler> sequence_segments;

    for (const std::pair<std::string, std::vector<size_t>>& kv : sequence_segments_dict) {
      SequenceSegmentHandler sequenceSegmentHandler = sequenceSegmentHandler_I;
      sequenceSegmentHandler.setSequenceSegmentName(kv.first);
      sequenceSegmentHandler.setSampleIndices(kv.second);
      sequence_segments.push_back(sequenceSegmentHandler);
    }

    sequenceHandler_IO.setSequenceSegments(sequence_segments);
  }

  void SequenceProcessor::processSequence(
    SequenceHandler& sequenceHandler_IO,
    const std::map<std::string, Filenames>& filenames,
    const std::set<std::string>& injection_names,
    const std::vector<std::shared_ptr<RawDataProcessor>>& raw_data_processing_methods_I,
    const bool verbose_I
  )
  {
    std::vector<InjectionHandler> process_sequence;

    // handle user-desired samples
    if (injection_names.empty()) {
      process_sequence = sequenceHandler_IO.getSequence();
    } else {
      process_sequence = sequenceHandler_IO.getSamplesInSequence(injection_names);
    }

    if (filenames.size() != process_sequence.size()) {
      throw std::invalid_argument("The number of provided filenames locations is not correct.");
    }

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

    sequenceHandler_IO.setSequence(process_sequence);
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
