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
    const std::string& delimiter,
    const bool verbose_I
  )
  {
    const std::map<std::string, std::string> filenames = sequenceHandler_IO.getFilenames();
    SequenceSegmentHandler sequenceSegmentHandler;
    RawDataHandler rawDataHandler;

    if (filenames.empty()) {
      std::cout << "No filenames in provided SequenceHandler." << std::endl;
    } else {
      SequenceParser::readSequenceFile(sequenceHandler_IO, filenames.at("sequence_csv_i")); // TODO: should this support delimiter as in python?

      OpenMSFile::readRawDataProcessingParameters(rawDataHandler, filenames.at("parameters_csv_i"), delimiter);

      OpenMSFile::loadTraML(rawDataHandler, filenames.at("traML_csv_i"), "csv", verbose_I); // TODO: make sure the format is "csv"

      OpenMSFile::loadFeatureFilter(
        rawDataHandler,
        filenames.at("featureFilterComponents_csv_i"),
        filenames.at("featureFilterComponentGroups_csv_i"),
        verbose_I
      );

      OpenMSFile::loadFeatureQC(
        rawDataHandler,
        filenames.at("featureQCComponents_csv_i"),
        filenames.at("featureQCComponentGroups_csv_i"),
        verbose_I
      );

      OpenMSFile::loadQuantitationMethods(sequenceSegmentHandler, filenames.at("quantitationMethods_csv_i"), verbose_I);
      OpenMSFile::loadStandardsConcentrations(sequenceSegmentHandler, filenames.at("standardsConcentrations_csv_i"), verbose_I);

      rawDataHandler.setQuantitationMethods(sequenceSegmentHandler.getQuantitationMethods());
    }

    segmentSamplesInSequence(sequenceHandler_IO, sequenceSegmentHandler);
    addRawDataHandlerToSequence(sequenceHandler_IO, rawDataHandler);
    // TODO: so if filenames is empty, basically the code works on empty data. Is this the desired behavior? This is what would happen in python
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
    const std::vector<std::string>& sample_names_I,
    const std::vector<std::string>& raw_data_processing_methods_I,
    const bool verbose_I
  )
  {
    std::vector<SampleHandler> process_sequence;

    if (sample_names_I.empty()) {
      process_sequence = sequenceHandler_IO.getSequence();
    } else {
      sequenceHandler_IO.getSamplesInSequence(sample_names_I, process_sequence);
    }

    for (SampleHandler& sample : process_sequence) {
      std::vector<std::string> raw_data_processing_methods;
      if (raw_data_processing_methods_I.size()) {
        raw_data_processing_methods = raw_data_processing_methods_I;
      } else {
        RawDataProcessor::getDefaultRawDataProcessingWorkflow(sample.getMetaData().getSampleType(), raw_data_processing_methods);
      }
      for (const std::string& event : raw_data_processing_methods) {
        RawDataProcessor::processRawData(
          sample.getRawData(),
          event,
          sample.getRawData().getParameters(),
          sequenceHandler_IO.getDefaultDynamicFilenames(
            sequenceHandler_IO.getDirDynamic(),
            sample.getMetaData().getSampleName()
          ),
          verbose_I
        );
      }
    }
  }

  void SequenceProcessor::processSequenceSegments(
    SequenceHandler& sequenceHandler_IO,
    const std::set<std::string>& sequence_segment_names,
    const std::vector<std::string>& sequence_segment_processing_methods_I,
    const bool verbose_I
  )
  {
    std::vector<SequenceSegmentHandler> sequence_segments;

    if (sequence_segment_names.empty()) {
      sequence_segments = sequenceHandler_IO.getSequenceSegments();
    } else {
      for (SequenceSegmentHandler& s : sequenceHandler_IO.getSequenceSegments()) {
        if (sequence_segment_names.count(s.getSequenceSegmentName())) {
          sequence_segments.push_back(s);
        }
      }
    }

    for (SequenceSegmentHandler& sequence_segment : sequence_segments) {
      std::vector<std::string> sequence_segment_processing_methods;
      if (sequence_segment_processing_methods_I.size()) {
        sequence_segment_processing_methods = sequence_segment_processing_methods_I;
      } else {
        std::set<std::string> sequence_segment_processing_methods_set;
        for (const size_t sample_index : sequence_segment.getSampleIndices()) {
          const MetaDataHandler::SampleType sample_type =
            sequenceHandler_IO.getSequence().at(sample_index).getMetaData().getSampleType();
          std::vector<std::string> workflow;
          SequenceSegmentProcessor::getDefaultSequenceSegmentProcessingWorkflow(sample_type, workflow);
          sequence_segment_processing_methods_set.insert(workflow.cbegin(), workflow.cend());
        }
      }
      for (const std::string& event : sequence_segment_processing_methods) {
        SequenceSegmentProcessor::processSequenceSegment(
          sequence_segment,
          sequenceHandler_IO,
          event,
          sequenceHandler_IO
            .getSequence()
            .at(sequence_segment.getSampleIndices().front())
            .getRawData()
            .getParameters(), // assumption that all parameters are the same for each sample in the sequence segment!
          sequenceHandler_IO.getDefaultDynamicFilenames(sequenceHandler_IO.getDirDynamic(), sequence_segment.getSequenceSegmentName()),
          verbose_I
        );
      }
    }
  }
}
