#include <SmartPeak/core/Metadata.h>
#include <SmartPeak/core/SampleType.h>
#include <SmartPeak/core/SequenceHandler.h>
#include <SmartPeak/core/SequenceProcessor.h>
#include <SmartPeak/io/SequenceParser.h>

using namespace SmartPeak;

void example_HPLC_UV_Standards(
  const std::string& dir_I,
  const Filenames& static_filenames,
  const std::string& delimiter_I = ","
)
{
  SequenceHandler sequenceHandler;

  CreateSequence cs(sequenceHandler);
  cs.filenames        = static_filenames;
  cs.delimiter        = delimiter_I;
  cs.checkConsistency = true;
  cs.process();

  std::vector<std::shared_ptr<RawDataProcessor>> raw_data_processing_methods = {
    std::shared_ptr<RawDataProcessor>(new LoadRawData()),
    std::shared_ptr<RawDataProcessor>(new MapChromatograms()),
    std::shared_ptr<RawDataProcessor>(new ExtractChromatogramWindows()),
    std::shared_ptr<RawDataProcessor>(new ZeroChromatogramBaseline()),
    std::shared_ptr<RawDataProcessor>(new PickFeatures()),
    //std::shared_ptr<RawDataProcessor>(new FilterFeatures()),
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
    {Metadata::calculated_concentration},
    {SampleType::Standard}
  );

  SequenceParser::writeDataTableFromMetaValue(
    sequenceHandler,
    static_filenames.featureSummary_csv_o,
    {
      Metadata::peak_apex_intensity,
      Metadata::total_width,
      Metadata::width_at_50_peak_height,
      Metadata::tailing_factor,
      Metadata::asymmetry_factor,
      Metadata::baseline_delta_to_height,
      Metadata::points_across_baseline,
      Metadata::points_across_half_height,
      Metadata::log_signal_to_noise,
      Metadata::calculated_concentration,
      Metadata::qc_transition_message,
      Metadata::qc_transition_pass,
      Metadata::qc_transition_score,
      Metadata::qc_transition_group_message,
      Metadata::qc_transition_group_score
    },
    {SampleType::Standard}
  );
}
