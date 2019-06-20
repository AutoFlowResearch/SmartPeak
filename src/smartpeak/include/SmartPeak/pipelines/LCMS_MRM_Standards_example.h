#include <SmartPeak/core/SequenceHandler.h>
#include <SmartPeak/core/SequenceProcessor.h>
#include <SmartPeak/io/SequenceParser.h>

using namespace SmartPeak;

void example_LCMS_MRM_Standards(
  const std::string& dir_I,
  const Filenames& static_filenames,
  const std::string& delimiter_I = ","
)
{
  SequenceHandler sequenceHandler;

  CreateSequence cs(sequenceHandler);
  cs.filenames          = static_filenames;
  cs.delimiter          = delimiter_I;
  cs.checkConsistency   = true;
  cs.process();

  std::vector<std::shared_ptr<RawDataProcessor>> raw_data_processing_methods = {
    std::shared_ptr<RawDataProcessor>(new LoadRawData()),
    std::shared_ptr<RawDataProcessor>(new MapChromatograms()),
    std::shared_ptr<RawDataProcessor>(new PickFeatures()),
    std::shared_ptr<RawDataProcessor>(new FilterFeatures()),
    std::shared_ptr<RawDataProcessor>(new FilterFeatures()),
    std::shared_ptr<RawDataProcessor>(new SelectFeatures()),
    std::shared_ptr<RawDataProcessor>(new CheckFeatures()),
    std::shared_ptr<RawDataProcessor>(new StoreFeatures())
  };

  std::map<std::string, Filenames> dynamic_filenames1;
  for (const InjectionHandler& injection : sequenceHandler.getSequence()) {
    const std::string& key = injection.getMetaData().getInjectionName();
    dynamic_filenames1[key] = Filenames::getDefaultDynamicFilenames(
      dir_I + "/mzML/",
      dir_I + "/features/",
      dir_I + "/features/",
      injection.getMetaData().getSampleName(),
      key
    );
  }

  ProcessSequence ps(sequenceHandler);
  ps.filenames                     = dynamic_filenames1;
  ps.raw_data_processing_methods_I = raw_data_processing_methods;
  ps.process();

  const std::vector<std::shared_ptr<SequenceSegmentProcessor>> sequence_segment_processing_methods = {
    std::shared_ptr<SequenceSegmentProcessor>(new CalculateCalibration()),
    std::shared_ptr<SequenceSegmentProcessor>(new StoreQuantitationMethods()),
  };

  std::map<std::string, Filenames> dynamic_filenames2;
  for (const SequenceSegmentHandler& sequence_segment : sequenceHandler.getSequenceSegments()) {
    const std::string& key = sequence_segment.getSequenceSegmentName();
    dynamic_filenames2[key] = Filenames::getDefaultDynamicFilenames(
      dir_I + "/mzML/",
      dir_I + "/features/",
      dir_I + "/features/",
      key,
      key
    );
  }

  ProcessSequenceSegments pss(sequenceHandler);
  pss.filenames                             = dynamic_filenames2;
  pss.sequence_segment_processing_methods_I = sequence_segment_processing_methods;
  pss.process();

  raw_data_processing_methods = {
    std::shared_ptr<RawDataProcessor>(new QuantifyFeatures()),
    std::shared_ptr<RawDataProcessor>(new CheckFeatures()),
    std::shared_ptr<RawDataProcessor>(new StoreFeatures())
  };

  std::map<std::string, Filenames> dynamic_filenames3;
  for (const InjectionHandler& injection : sequenceHandler.getSequence()) {
    const std::string& key = injection.getMetaData().getInjectionName();
    dynamic_filenames3[key] = Filenames::getDefaultDynamicFilenames(
      dir_I + "/mzML/",
      dir_I + "/features/",
      dir_I + "/features/",
      injection.getMetaData().getSampleName(),
      key
    );
  }

  ps.filenames                     = dynamic_filenames3;
  ps.raw_data_processing_methods_I = raw_data_processing_methods;
  ps.process();

  SequenceParser::writeDataMatrixFromMetaValue(
    sequenceHandler,
    static_filenames.sequenceSummary_csv_o,
    {"calculated_concentration"},
    {MetaDataHandler::SampleType::Standard}
  );

  SequenceParser::writeDataTableFromMetaValue(
    sequenceHandler,
    static_filenames.featureSummary_csv_o,
    {
      "peak_apex_int", "total_width", "width_at_50", "tailing_factor", "asymmetry_factor",
      "baseline_delta_2_height", "points_across_baseline", "points_across_half_height",
      "logSN", "calculated_concentration", "QC_transition_message", "QC_transition_pass",
      "QC_transition_score", "QC_transition_group_message", "QC_transition_group_score"
    },
    {MetaDataHandler::SampleType::Standard}
  );
}
