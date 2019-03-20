#include <SmartPeak/core/SequenceHandler.h>
#include <SmartPeak/core/SequenceProcessor.h>
#include <SmartPeak/io/SequenceParser.h>

using namespace SmartPeak;

void example_GCMS_SIM_Unknowns(
  const std::string& dir_I,
  const Filenames& static_filenames,
  const std::string& delimiter_I = ",",
  const bool verbose_I = false
)
{
  SequenceHandler sequenceHandler;

  SequenceProcessor::createSequence(sequenceHandler, static_filenames, delimiter_I, true, true);

  const std::vector<std::shared_ptr<RawDataProcessor>> raw_data_processing_methods = {
    std::shared_ptr<RawDataProcessor>(new LoadRawData()),
    std::shared_ptr<RawDataProcessor>(new MapChromatograms()),
    std::shared_ptr<RawDataProcessor>(new ExtractChromatogramWindows()),
    std::shared_ptr<RawDataProcessor>(new PickFeatures()),
    std::shared_ptr<RawDataProcessor>(new FilterFeatures()),
    std::shared_ptr<RawDataProcessor>(new FilterFeatures()),
    std::shared_ptr<RawDataProcessor>(new SelectFeatures()),
    std::shared_ptr<RawDataProcessor>(new QuantifyFeatures()),
    std::shared_ptr<RawDataProcessor>(new CheckFeatures()),
    std::shared_ptr<RawDataProcessor>(new StoreFeatures())
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
