#include <SmartPeak/core/SequenceHandler.h>
#include <SmartPeak/core/SequenceProcessor.h>

using namespace SmartPeak;

OpenMS::MzTab example_FIAMS_FullScan_Unknowns(
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

  const std::vector<std::shared_ptr<RawDataProcessor>> raw_data_processing_methods = {
    std::make_shared<LoadRawData>(),
    std::make_shared<ExtractSpectraWindows>(),
    std::make_shared<MergeSpectra>(),
    std::make_shared<PickMS1Features>(),
    std::make_shared<SearchAccurateMass>(),
    //std::make_shared<StoreAnnotations>(), // Files are quite large
    std::make_shared<StoreFeatures>()
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

  ProcessSequence ps(sequenceHandler);
  ps.filenames                     = dynamic_filenames;
  ps.raw_data_processing_methods_I = raw_data_processing_methods;
  ps.process();

  return sequenceHandler.getSequence().front().getRawData().getMzTab();
}
