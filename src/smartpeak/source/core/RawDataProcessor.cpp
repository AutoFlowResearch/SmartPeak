// TODO: Add copyright

#include <SmartPeak/core/RawDataProcessor.h>
#include <SmartPeak/core/Filenames.h>
#include <SmartPeak/algorithm/MRMFeatureValidator.h>
#include <SmartPeak/core/Utilities.h>
#include <SmartPeak/io/OpenMSFile.h>
#include <OpenMS/ANALYSIS/OPENSWATH/MRMBatchFeatureSelector.h>
#include <OpenMS/ANALYSIS/OPENSWATH/MRMFeatureFilter.h>
#include <OpenMS/ANALYSIS/OPENSWATH/MRMFeatureFinderScoring.h>
#include <OpenMS/ANALYSIS/OPENSWATH/MRMFeatureSelector.h>
#include <OpenMS/ANALYSIS/QUANTITATION/AbsoluteQuantitation.h>

namespace SmartPeak
{
  void PickFeatures::processRawData(
    RawDataHandler& rawDataHandler_IO,
    const std::map<std::string, std::vector<std::map<std::string, std::string>>>& params_I,
    const Filenames& filenames,
    const bool verbose_I
  )
  {
    if (verbose_I) {
      std::cout << "==== START pickFeatures" << std::endl;
      std::cout << "Experiment size: " << rawDataHandler_IO.getChromatogramMap().size() << std::endl;
    }

    if (params_I.find("MRMFeatureFinderScoring") != params_I.end() && params_I.at("MRMFeatureFinderScoring").empty()) {
      std::cout << "No parameters passed to PickFeatures. Not picking." << std::endl;
      return;
    }

    OpenMS::MRMFeatureFinderScoring featureFinder;
    OpenMS::Param parameters = featureFinder.getParameters();
    Utilities::updateParameters(parameters, params_I.at("MRMFeatureFinderScoring"));
    featureFinder.setParameters(parameters);

    OpenMS::FeatureMap featureMap;

    featureFinder.pickExperiment(
      rawDataHandler_IO.getChromatogramMap(),
      featureMap,
      rawDataHandler_IO.getTargetedExperiment(),
      rawDataHandler_IO.getTransformationDescription(),
      rawDataHandler_IO.getSWATH()
    );

    // NOTE: setPrimaryMSRunPath() is needed for calculate_calibration
    featureMap.setPrimaryMSRunPath({rawDataHandler_IO.getMetaData().getFilename()});

    rawDataHandler_IO.setFeatureMap(featureMap);
    rawDataHandler_IO.updateFeatureMapHistory();

    if (verbose_I) {
      std::cout << "pickFeatures: output size: " << featureMap.size() << std::endl;
      std::cout << "==== END   pickFeatures" << std::endl;
    }
  }

  void FilterFeatures::processRawData(
    RawDataHandler& rawDataHandler_IO,
    const std::map<std::string, std::vector<std::map<std::string, std::string>>>& params_I,
    const Filenames& filenames,
    const bool verbose_I
  )
  {
    if (verbose_I) {
      std::cout << "==== START filterFeatures" << std::endl;
      std::cout << "filterFeatures: input size: " << rawDataHandler_IO.getFeatureMap().size() << std::endl;
    }

    if (params_I.find("MRMFeatureFilter.filter_MRMFeatures") != params_I.end() && params_I.at("MRMFeatureFilter.filter_MRMFeatures").empty()) {
      std::cout << "No parameters passed to filterFeatures. Not filtering." << std::endl;
      return;
    }

    OpenMS::MRMFeatureFilter featureFilter;
    OpenMS::Param parameters = featureFilter.getParameters();
    Utilities::updateParameters(parameters, params_I.at("MRMFeatureFilter.filter_MRMFeatures"));
    featureFilter.setParameters(parameters);

    OpenMS::FeatureMap& featureMap = rawDataHandler_IO.getFeatureMap();

    featureFilter.FilterFeatureMap(
      featureMap,
      rawDataHandler_IO.getFeatureFilter(),
      rawDataHandler_IO.getTargetedExperiment()
    );

    rawDataHandler_IO.updateFeatureMapHistory();

    if (verbose_I) {
      std::cout << "filterFeatures: output size: " << featureMap.size() << std::endl;
      std::cout << "==== END   filterFeatures" << std::endl;
    }
  }

  void CheckFeatures::processRawData(
    RawDataHandler& rawDataHandler_IO,
    const std::map<std::string, std::vector<std::map<std::string, std::string>>>& params_I,
    const Filenames& filenames,
    const bool verbose_I
  )
  {
    if (verbose_I) {
      std::cout << "==== START checkFeatures" << std::endl;
      std::cout << "checkFeatures: input size: " << rawDataHandler_IO.getFeatureMap().size() << std::endl;
    }

    if (params_I.find("MRMFeatureFilter.filter_MRMFeatures.qc") != params_I.end() && params_I.at("MRMFeatureFilter.filter_MRMFeatures.qc").empty()) {
      std::cout << "No parameters passed to checkFeatures. Not checking." << std::endl;
      return;
    }

    OpenMS::MRMFeatureFilter featureFilter;
    OpenMS::Param parameters = featureFilter.getParameters();
    Utilities::updateParameters(parameters, params_I.at("MRMFeatureFilter.filter_MRMFeatures.qc"));
    featureFilter.setParameters(parameters);

    OpenMS::FeatureMap& featureMap = rawDataHandler_IO.getFeatureMap();

    featureFilter.FilterFeatureMap(
      featureMap,
      rawDataHandler_IO.getFeatureQC(),
      rawDataHandler_IO.getTargetedExperiment()
    );

    rawDataHandler_IO.updateFeatureMapHistory();

    if (verbose_I) {
      std::cout << "checkFeatures: output size: " << featureMap.size() << std::endl;
      std::cout << "==== END   checkFeatures" << std::endl;
    }
  }

  void SelectFeatures::processRawData(
    RawDataHandler& rawDataHandler_IO,
    const std::map<std::string, std::vector<std::map<std::string, std::string>>>& params_I,
    const Filenames& filenames,
    const bool verbose_I
  )
  {
    if (verbose_I) {
      std::cout << "==== START selectFeatures" << std::endl;
      std::cout << "selectFeatures: input size: " << rawDataHandler_IO.getFeatureMap().size() << std::endl;
    }

    if (params_I.find("MRMFeatureSelector.schedule_MRMFeatures_qmip") != params_I.end() &&
      params_I.find("MRMFeatureSelector.schedule_MRMFeatures_score") != params_I.end()) {
      std::cout << "No parameters passed to selectFeatures. Not selecting." << std::endl;
      return;
    }

    OpenMS::FeatureMap output;
    
    if (params_I.at("MRMFeatureSelector.schedule_MRMFeatures_qmip").size()) {
      std::vector<OpenMS::MRMFeatureSelector::SelectorParameters> p =
        Utilities::extractSelectorParameters(params_I.at("MRMFeatureSelector.schedule_MRMFeatures_qmip"), params_I.at("MRMFeatureSelector.select_MRMFeatures_qmip"));
      OpenMS::MRMBatchFeatureSelector::batchMRMFeaturesQMIP(rawDataHandler_IO.getFeatureMap(), output, p);
    } else if (params_I.at("MRMFeatureSelector.schedule_MRMFeatures_score").size()) {
      std::vector<OpenMS::MRMFeatureSelector::SelectorParameters> p =
        Utilities::extractSelectorParameters(params_I.at("MRMFeatureSelector.schedule_MRMFeatures_score"), params_I.at("MRMFeatureSelector.select_MRMFeatures_score"));
      OpenMS::MRMBatchFeatureSelector::batchMRMFeaturesScore(rawDataHandler_IO.getFeatureMap(), output, p);
    } else {
      throw std::invalid_argument("Both arguments 'select params' and 'schedule params' are empty.");
    }

    output.setPrimaryMSRunPath({rawDataHandler_IO.getMetaData().getFilename()});

    rawDataHandler_IO.setFeatureMap(output);
    rawDataHandler_IO.updateFeatureMapHistory();

    if (verbose_I) {
      std::cout << "selectFeatures: output size: " << output.size() << std::endl;
      std::cout << "==== END   selectFeatures" << std::endl;
    }
  }

  void ExtractMetaData::processRawData(
    RawDataHandler& rawDataHandler_IO,
    const std::map<std::string, std::vector<std::map<std::string, std::string>>>& params_I,
    const Filenames& filenames,
    const bool verbose_I
  )
  {
    if (verbose_I)
      std::cout << "Extracting metadata" << std::endl;

    std::string filename;
    std::string samplename;

    const std::string loaded_file_path = rawDataHandler_IO.getChromatogramMap().getLoadedFilePath();

    if (loaded_file_path.size()) {
      const std::string prefix {"file://"};
      filename = !loaded_file_path.find(prefix) ? loaded_file_path.substr(prefix.size()) : loaded_file_path;
    }

    OpenMS::DataValue dv_mzml_id = rawDataHandler_IO.getChromatogramMap().getMetaValue("mzml_id");

    if (!dv_mzml_id.isEmpty() && dv_mzml_id.toString().size()) {
      samplename = dv_mzml_id.toString();
      const size_t pos = samplename.find('-');
      if (pos != std::string::npos)
        samplename = samplename.substr(pos + 1);
    } else {
      throw "no mzml_id found\n";
    }

    const OpenMS::MSExperiment& chromatogram_map = rawDataHandler_IO.getChromatogramMap();

    MetaDataHandler& metaDataHandler = rawDataHandler_IO.getMetaData();

    if (metaDataHandler.getSampleName().empty())
      metaDataHandler.setSampleName(samplename);

    if (metaDataHandler.getFilename().empty())
      metaDataHandler.setFilename(filename);

    if (metaDataHandler.proc_method_name.empty())
      metaDataHandler.proc_method_name = chromatogram_map.getInstrument().getSoftware().getName();

    if (metaDataHandler.instrument.empty())
      metaDataHandler.instrument = chromatogram_map.getInstrument().getName();

    if (metaDataHandler.operator_name.empty() && chromatogram_map.getContacts().size())
      metaDataHandler.operator_name = chromatogram_map.getContacts()[0].getLastName() + " " + chromatogram_map.getContacts()[0].getFirstName();

    if (metaDataHandler.acquisition_date_and_time.tm_year == 0) {
      // some noise because OpenMS uses uint and the standard library uses int (for time structure's members)
      struct { OpenMS::UInt tm_mon, tm_mday, tm_year, tm_hour, tm_min, tm_sec; } dt_uint;
      rawDataHandler_IO.getChromatogramMap().getDateTime().get(
        dt_uint.tm_mon,
        dt_uint.tm_mday,
        dt_uint.tm_year,
        dt_uint.tm_hour,
        dt_uint.tm_min,
        dt_uint.tm_sec
      );
      std::tm& dt_int = metaDataHandler.acquisition_date_and_time;
      dt_int.tm_year = dt_uint.tm_year;
      dt_int.tm_mon = dt_uint.tm_mon;
      dt_int.tm_mday = dt_uint.tm_mday;
      dt_int.tm_hour = dt_uint.tm_hour;
      dt_int.tm_min = dt_uint.tm_min;
      dt_int.tm_sec = dt_uint.tm_sec;
    }
  }

  void ValidateFeatures::processRawData(
    RawDataHandler& rawDataHandler_IO,
    const std::map<std::string, std::vector<std::map<std::string, std::string>>>& params_I,
    const Filenames& filenames,
    const bool verbose_I
  )
  {
    if (verbose_I) {
      std::cout << "==== START validateFeatures" << std::endl;
    }

    if (params_I.find("MRMFeatureValidator.validate_MRMFeatures") != params_I.end() && params_I.at("MRMFeatureValidator.validate_MRMFeatures").empty()) {
      std::cout << "No parameters passed to validateFeatures. Not validating." << std::endl;
      return;
    }

    OpenMS::FeatureMap mapped_features;
    std::map<std::string, float> validation_metrics; // keys: accuracy, recall, precision

    MRMFeatureValidator::validate_MRMFeatures(
      rawDataHandler_IO.getReferenceData(),
      rawDataHandler_IO.getFeatureMap(),
      rawDataHandler_IO.getMetaData().getInjectionName(),
      mapped_features,
      validation_metrics,
      std::stof(params_I.at("MRMFeatureValidator.validate_MRMFeatures").front().at("value")), // TODO: While this probably works, it might be nice to add some check that the parameter passed is the desired one
      verbose_I
    );

    rawDataHandler_IO.setFeatureMap(mapped_features);
    rawDataHandler_IO.setValidationMetrics(validation_metrics);

    if (verbose_I) {
      std::cout << "==== END   validateFeatures" << std::endl;
    }
  }

  void PlotFeatures::processRawData(
    RawDataHandler& rawDataHandler_IO,
    const std::map<std::string, std::vector<std::map<std::string, std::string>>>& params_I,
    const Filenames& filenames,
    const bool verbose_I
  )
  {
    if (verbose_I)
      std::cout << "Plotting peaks with features (NOT IMPLEMENTED)" << std::endl;

    // TODO: Uncomment once FeaturePlotter is ready

    // if (FeaturePlotter_params_I.empty() || filename.empty())
    //   return;

    // FeaturePlotter featurePlotter;
    // featurePlotter.setParameters(FeaturePlotter_params_I);
    // featurePlotter.plot_peaks(
    //   filename,
    //   rawDataHandler_IO.getTargetedExperiment(),
    //   rawDataHandler_IO.getChromatogramMap(),
    //   rawDataHandler_IO.getFeatureMap()
    // );
  }

  void QuantifyFeatures::processRawData(
    RawDataHandler& rawDataHandler_IO,
    const std::map<std::string, std::vector<std::map<std::string, std::string>>>& params_I,
    const Filenames& filenames,
    const bool verbose_I
  )
  {
    if (verbose_I) {
      std::cout << "==== START quantifyComponents" << std::endl;
      std::cout << "Processing # quantitation methods: " << rawDataHandler_IO.getQuantitationMethods().size() << std::endl;
    }

    try {
      OpenMS::AbsoluteQuantitation aq;
      aq.setQuantMethods(rawDataHandler_IO.getQuantitationMethods());
      aq.quantifyComponents(rawDataHandler_IO.getFeatureMap());
      rawDataHandler_IO.updateFeatureMapHistory();
    } catch (const std::exception& e) {
      std::cerr << "quantifyComponents(): " << e.what() << std::endl;
    }

    if (verbose_I) {
      std::cout << "==== END   quantifyComponents" << std::endl;
    }
  }
}
