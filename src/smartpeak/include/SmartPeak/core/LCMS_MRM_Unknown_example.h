#include <SmartPeak/core/SequenceHandler.h>
#include <SmartPeak/core/SequenceProcessor.h>
#include <SmartPeak/io/SequenceParser.h>

using namespace SmartPeak;

void example_LCMS_MRM_Unknowns(
  const std::string& dir_I,
  const Filenames& static_filenames,
  const std::string& delimiter_I = ",",
  const bool verbose_I = false
)
{
  SequenceHandler sequenceHandler;

  SequenceProcessor::createSequence(sequenceHandler, static_filenames, delimiter_I, true);

  const std::vector<std::string> raw_data_processing_methods = {
    "load_raw_data",
    // "load_features",
    "pick_features",
    "filter_features",
    "filter_features",
    "select_features",
    // "validate_features",
    "quantify_features",
    "check_features",
    "store_features",
    // "plot_features"
  };

  std::map<std::string, Filenames> dynamic_filenames;
  for (const SampleHandler& sample : sequenceHandler.getSequence()) {
    const std::string& key = sample.getMetaData().getInjectionName();
    dynamic_filenames[key] = Filenames::getDefaultDynamicFilenames(
      dir_I + "/mzML/",
      dir_I + "/features/",
      dir_I + "/features/",
      sample.getMetaData().getSampleName(),
      key
    );
  }

  SequenceProcessor::processSequence(
    sequenceHandler,
    dynamic_filenames,
    std::vector<std::string>(),
    raw_data_processing_methods,
    verbose_I
  );

  SequenceParser::writeDataMatrixFromMetaValue(
    sequenceHandler,
    static_filenames.sequenceSummary_csv_o,
    {"calculated_concentration"},
    {MetaDataHandler::SampleType::Unknown}
  );

  SequenceParser::writeDataTableFromMetaValue(
    sequenceHandler,
    static_filenames.featureSummary_csv_o,
    {
      "peak_apex_int", "total_width", "width_at_50", "tailing_factor",
      "asymmetry_factor", "baseline_delta_2_height", "points_across_baseline",
      "points_across_half_height", "logSN", "calculated_concentration",
      "QC_transition_message", "QC_transition_pass", "QC_transition_score",
      "QC_transition_group_message", "QC_transition_group_score"
    },
    {MetaDataHandler::SampleType::Unknown}
  );
}
