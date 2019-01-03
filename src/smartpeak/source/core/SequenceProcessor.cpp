// TODO: Add copyright

#include <SmartPeak/core/SequenceProcessor.h>
#include <SmartPeak/core/RawDataProcessor.h>
#include <SmartPeak/core/SequenceSegmentProcessor.h>
#include <SmartPeak/io/OpenMSFile.h>
#include <SmartPeak/io/SequenceParser.h>

namespace SmartPeak
{
  void SequenceProcessor::createSequence(
    SequenceHandler& sequenceHandler_IO,
    const SequenceHandler::Filenames& filenames,
    const std::string& delimiter,
    const bool verbose_I
  )
  {
    if (verbose_I) {
      std::cout << "==== START createSequence()" << std::endl;
    }

    SequenceSegmentHandler sequenceSegmentHandler;
    RawDataHandler rawDataHandler;

    SequenceParser::readSequenceFile(sequenceHandler_IO, filenames.sequence_csv_i, delimiter);

    OpenMSFile::readRawDataProcessingParameters(rawDataHandler, filenames.parameters_csv_i, delimiter);

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

    OpenMSFile::loadQuantitationMethods(sequenceSegmentHandler, filenames.quantitationMethods_csv_i, verbose_I);
    OpenMSFile::loadStandardsConcentrations(sequenceSegmentHandler, filenames.standardsConcentrations_csv_i, verbose_I);

    rawDataHandler.setQuantitationMethods(sequenceSegmentHandler.getQuantitationMethods());

    segmentSamplesInSequence(sequenceHandler_IO, sequenceSegmentHandler);
    addRawDataHandlerToSequence(sequenceHandler_IO, rawDataHandler);

    if (verbose_I) {
      std::cout << "==== END   createSequence()" << std::endl;
    }
  }

  void SequenceProcessor::addRawDataHandlerToSequence(
    SequenceHandler& sequenceHandler_IO,
    const RawDataHandler& rawDataHandler
  )
  {
    for (SampleHandler& sample : sequenceHandler_IO.getSequence()) {
      sample.setRawData(rawDataHandler);
      sample.getRawData().setMetaData(sample.getMetaData());
    }
  }

  void SequenceProcessor::segmentSamplesInSequence(
    SequenceHandler& sequenceHandler_IO,
    const SequenceSegmentHandler& sequenceSegmentHandler_I
  )
  {
    const std::vector<SampleHandler>& sequence = sequenceHandler_IO.getSequence();

    std::map<std::string, std::vector<size_t>> sequence_segments_dict;

    for (size_t i = 0; i < sequence.size(); ++i) {
      const std::string sequence_segment_name = sequence[i].getMetaData().getSequenceSegmentName();
      if (0 == sequence_segments_dict.count(sequence_segment_name))
        sequence_segments_dict.emplace(sequence_segment_name, std::vector<size_t>());
      sequence_segments_dict.at(sequence_segment_name).push_back(i);
    }

    std::vector<SequenceSegmentHandler>& sequence_segments = sequenceHandler_IO.getSequenceSegments();
    sequence_segments.clear();

    for (const std::pair<std::string, std::vector<size_t>>& kv : sequence_segments_dict) {
      SequenceSegmentHandler sequenceSegmentHandler = sequenceSegmentHandler_I;
      sequenceSegmentHandler.setSequenceSegmentName(kv.first);
      sequenceSegmentHandler.setSampleIndices(kv.second);
      sequence_segments.push_back(sequenceSegmentHandler);
    }
  }

  void SequenceProcessor::processSequence(
    SequenceHandler& sequenceHandler_IO,
    const std::vector<SequenceHandler::Filenames>& filenames,
    const std::vector<std::string>& sample_names_I,
    const std::vector<std::string>& raw_data_processing_methods_I,
    const bool verbose_I
  )
  {
    std::vector<SampleHandler> process_sequence;

    if (sample_names_I.empty()) {
      process_sequence = sequenceHandler_IO.getSequence();
    } else {
      process_sequence = sequenceHandler_IO.getSamplesInSequence(sample_names_I);
    }

    if (filenames.size() != process_sequence.size()) {
      throw std::invalid_argument("The number of provided filenames locations is not correct.");
    }

    for (size_t j = 0; j < process_sequence.size(); ++j) {
      SampleHandler& sample = process_sequence[j];
      std::vector<std::string> raw_data_processing_methods;

      if (raw_data_processing_methods_I.size()) {
        raw_data_processing_methods = raw_data_processing_methods_I;
      } else {
        RawDataProcessor::getDefaultRawDataProcessingWorkflow(
          sample.getMetaData().getSampleType(),
          raw_data_processing_methods
        );
      }

      const size_t n = raw_data_processing_methods.size();

      for (size_t i = 0; i < n; ++i) {
        if (verbose_I) {
          std::cout << "\n[" << (i + 1) << "/" << n << "]" << std::endl;
        }
        RawDataProcessor::processRawData(
          sample.getRawData(),
          raw_data_processing_methods[i], // event
          sample.getRawData().getParameters(),
          filenames[j],
          verbose_I
        );
      }
    }

    sequenceHandler_IO.setSequence(process_sequence);
  }

  void SequenceProcessor::processSequenceSegments(
    SequenceHandler& sequenceHandler_IO,
    const std::vector<SequenceHandler::Filenames>& filenames,
    const std::set<std::string>& sequence_segment_names,
    const std::vector<std::string>& sequence_segment_processing_methods_I,
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

    for (size_t j = 0; j < sequence_segments.size(); ++j) { // for each sequence segment
      SequenceSegmentHandler& sequence_segment = sequence_segments[j];
      std::vector<std::string> sequence_segment_processing_methods;

      // collect its methods
      if (sequence_segment_processing_methods_I.size()) {
        sequence_segment_processing_methods = sequence_segment_processing_methods_I;
      } else {
        for (const size_t sample_index : sequence_segment.getSampleIndices()) {
          const MetaDataHandler::SampleType sample_type =
            sequenceHandler_IO.getSequence().at(sample_index).getMetaData().getSampleType();
          std::vector<std::string> workflow;
          SequenceSegmentProcessor::getDefaultSequenceSegmentProcessingWorkflow(sample_type, workflow);
          sequence_segment_processing_methods = workflow;
        }
      }

      // and process them
      for (const std::string& event : sequence_segment_processing_methods) {
        SequenceSegmentProcessor::processSequenceSegment(
          sequence_segment,
          sequenceHandler_IO,
          event,
          sequenceHandler_IO
            .getSequence()
            .at(sequence_segment.getSampleIndices().front())
            .getRawData()
            .getParameters(), // assuming that all parameters are the same for each sample in the sequence segment!
          filenames[j],
          verbose_I
        );
      }
    }

    sequenceHandler_IO.setSequenceSegments(sequence_segments);
  }
}
