#include <SmartPeak/core/SequenceHandler.h>
#include <SmartPeak/core/SequenceProcessor.h>
#include <SmartPeak/io/SequenceParser.h>

using namespace SmartPeak;

void example_LCMS_MRM_Standards(
  const std::string& dir_I,
  const SequenceHandler::Filenames& static_filenames,
  // const std::vector<SequenceHandler::Filenames>& dynamic_filenames,
  const std::string& delimiter_I = ",",
  const bool verbose_I = false
)
{
  SequenceHandler sequenceHandler;

  sequenceHandler.setDirDynamic(dir_I);

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

  SequenceProcessor::processSequence(
    sequenceHandler,
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

  SequenceProcessor::processSequenceSegments(
    sequenceHandler,
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

  SequenceProcessor::processSequence(
    sequenceHandler,
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
