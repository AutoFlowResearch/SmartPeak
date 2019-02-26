#include <SmartPeak/core/SequenceHandler.h>
#include <SmartPeak/core/SequenceProcessor.h>
#include <SmartPeak/io/SequenceParser.h>

using namespace SmartPeak;

void example_LCMS_MRM_Validation(
  const std::string& dir_I,
  const Filenames& static_filenames,
  const std::string& delimiter_I = ",",
  const bool verbose_I = false
)
{
  SequenceHandler sequenceHandler;

  SequenceProcessor::createSequence(sequenceHandler, static_filenames, delimiter_I, true, true);

  const std::vector<RawDataProcessor::RawDataProcMethod> raw_data_processing_methods = {
    RawDataProcessor::LOAD_RAW_DATA,
    RawDataProcessor::PICK_FEATURES,
    RawDataProcessor::FILTER_FEATURES,
    RawDataProcessor::SELECT_FEATURES,
    RawDataProcessor::VALIDATE_FEATURES
  };

  std::map<std::string, Filenames> dynamic_filenames;
  for (const InjectionHandler& injection : sequenceHandler.getSequence()) {
    const std::string& key = injection.getMetaData().getInjectionName();
    dynamic_filenames[key] = Filenames::getDefaultDynamicFilenames(
      dir_I + "/mzML/",
      dir_I + "/features/",
      dir_I + "/features/",
      injection.getMetaData().getSampleName(),
      key
    );
  }

  SequenceProcessor::processSequence(
    sequenceHandler,
    dynamic_filenames,
    std::set<std::string>(),
    raw_data_processing_methods,
    true
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
