#include <SmartPeak/core/SequenceHandler.h>
#include <SmartPeak/core/SequenceProcessor.h>
#include <SmartPeak/io/SequenceParser.h>

using namespace SmartPeak;

void example_LCMS_MRM_Standards(
  const std::string& dir_I,
  const Filenames& static_filenames,
  const std::string& delimiter_I = ",",
  const bool verbose_I = false
)
{
  SequenceHandler sequenceHandler;

  SequenceProcessor::createSequence(sequenceHandler, static_filenames, delimiter_I);

  std::vector<std::string> raw_data_processing_methods = {
    "load_raw_data",
    // # "load_features",
    "pick_features",
    "filter_features",
    "filter_features",
    "select_features",
    "check_features",
    "store_features",
    // # "plot_features"
  };

  std::map<std::string, Filenames> dynamic_filenames1;
  for (const SampleHandler& sample : sequenceHandler.getSequence()) {
    const std::string& key = sample.getMetaData().getSampleName();
    dynamic_filenames1[key] = Filenames::getDefaultDynamicFilenames(dir_I, key);
  }

  SequenceProcessor::processSequence(
    sequenceHandler,
    dynamic_filenames1,
    std::vector<std::string>(),
    raw_data_processing_methods,
    true
  );

  const std::vector<std::string> sequence_segment_processing_methods = {
    "calculate_calibration",
  // "plot_calibrators",
    "store_quantitation_methods",
  // # "load_quantitation_methods",
  // # "store_components_to_concentrations"
  };

  std::map<std::string, Filenames> dynamic_filenames2;
  for (const SequenceSegmentHandler& sequence_segment : sequenceHandler.getSequenceSegments()) {
    const std::string& key = sequence_segment.getSequenceSegmentName();
    dynamic_filenames2[key] = Filenames::getDefaultDynamicFilenames(dir_I, key);
  }

  SequenceProcessor::processSequenceSegments(
    sequenceHandler,
    dynamic_filenames2,
    std::set<std::string>(),
    sequence_segment_processing_methods,
    true
  );

  raw_data_processing_methods = {
    "quantify_features",
    "check_features",
    "store_features",
    // # "plot_features"
  };

  std::map<std::string, Filenames> dynamic_filenames3;
  for (const SampleHandler& sample : sequenceHandler.getSequence()) {
    const std::string& key = sample.getMetaData().getSampleName();
    dynamic_filenames3[key] = Filenames::getDefaultDynamicFilenames(dir_I, key);
  }

  SequenceProcessor::processSequence(
    sequenceHandler,
    dynamic_filenames3,
    std::vector<std::string>(),
    raw_data_processing_methods,
    true
  );

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
