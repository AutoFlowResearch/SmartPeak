// TODO: Add copyright

#include <SmartPeak/core/SequenceProcessor.h>

namespace SmartPeak
{
  void SequenceProcessor::createSequence(
    SequenceHandler& sequenceHandler_IO,
    const char delimiter = ',',
    const bool verbose_I = false
  )
  {
    const std::map<std::string, std::string>& filenames = sequenceHandler_IO.getFilenames();

    if (filenames.empty()) {
      throw std::invalid_argument("No filenames in provided SequenceHandler.");
    }

    SequenceParser::readSequenceFile(sequenceHandler_IO, filenames.at("sequence_csv_i"), delimiter);

    OpenMSFile::readRawDataProcessingParameters(rawDataHandler, filenames.at("parameters_csv_i"), delimiter);

    OpenMSFile::loadTraML(rawDataHandler, filenames.at("traML_csv_i"), "csv", verbose_I); // TODO: make sure the format is "csv"

    OpenMSFile::loadFeatureFilter(
      rawDataHandler,
      filenames.at("featureFilterComponents_csv_i"),
      filenames.at("featureFilterComponents_csv_i"),
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

    segmentSamplesInSequence(sequenceHandler_IO, sequenceSegmentHandler);
    addRawDataHandlerToSequence(sequenceHandler_IO, rawDataHandler);
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
    const SequenceSegmentHandler& sequenceSegmentHandler_I = SequenceSegmentHandler()
  )
  {
    const std::vector<SampleHandler>& sequence = sequenceHandler_IO.getSequence();
    std::map<std::string, std::vector<size_t>> sequence_segments_dict;
    for (size_t i = 0; i < sequence.size(); ++i) {
      const std::string& sequence_segment_name = sequence[i].getMetaData().getSequenceSegmentName();
      if (0 == sequence_segments_dict.count(sequence_segment_name))
        sequence_segments_dict.emplace(sequence_segment_name, std::vector<size_t>());
      sequence_segments_dict.at(sequence_segment_name).push_back(i);
    }
    sequence_segments;
    std::vector<SequenceSegmentHandler>& sequence_segments = sequenceHandler_IO.getSequenceSegments();
    sequence_segments.clear();
    for (const std::pair<std::string, std::vector<size_t>>& kv : sequence_segments_dict) {
      SequenceSegmentHandler sequenceSegmentHandler = sequenceSegmentHandler_I;
      sequenceSegmentHandler.setSequenceSegmentName(kv.first);
      sequenceSegmentHandler.setSampleIndices(kv.second);
      sequence_segments.push_back(sequenceSegmentHandler);
    }
  }
}
