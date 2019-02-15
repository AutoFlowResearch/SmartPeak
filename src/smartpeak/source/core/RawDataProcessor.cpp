// TODO: Add copyright

#include <SmartPeak/core/RawDataProcessor.h>
#include <SmartPeak/core/Filenames.h>
#include <SmartPeak/core/MRMFeatureValidator.h>
#include <SmartPeak/core/Utilities.h>
#include <SmartPeak/io/OpenMSFile.h>
#include <OpenMS/ANALYSIS/OPENSWATH/MRMBatchFeatureSelector.h>
#include <OpenMS/ANALYSIS/OPENSWATH/MRMFeatureFilter.h>
#include <OpenMS/ANALYSIS/OPENSWATH/MRMFeatureFinderScoring.h>
#include <OpenMS/ANALYSIS/OPENSWATH/MRMFeatureSelector.h>
#include <OpenMS/ANALYSIS/QUANTITATION/AbsoluteQuantitation.h>

namespace SmartPeak
{
  void RawDataProcessor::pickFeatures(
    RawDataHandler& rawDataHandler_IO,
    const std::vector<std::map<std::string, std::string>>& MRMFeatureFinderScoring_params_I,
    const bool verbose_I
  )
  {
    if (verbose_I) {
      std::cout << "==== START pickFeatures" << std::endl;
      std::cout << "Experiment size: " << rawDataHandler_IO.getChromatogramMap().size() << std::endl;
    }

    OpenMS::MRMFeatureFinderScoring featureFinder;
    OpenMS::Param parameters = featureFinder.getParameters();
    Utilities::updateParameters(parameters, MRMFeatureFinderScoring_params_I);
    featureFinder.setParameters(parameters);

    OpenMS::FeatureMap featureMap;

    featureFinder.pickExperiment(
      rawDataHandler_IO.getChromatogramMap(),
      featureMap,
      rawDataHandler_IO.getTargetedExperiment(),
      rawDataHandler_IO.getTransformationDescription(),
      rawDataHandler_IO.getSWATH()
    );

    featureMap.setPrimaryMSRunPath({rawDataHandler_IO.getMetaData().getFilename()});

    rawDataHandler_IO.setFeatureMap(featureMap);

    if (verbose_I) {
      std::cout << "pickFeatures: output size: " << featureMap.size() << std::endl;
      std::cout << "==== END   pickFeatures" << std::endl;
    }
  }

  void RawDataProcessor::filterFeatures(
    RawDataHandler& rawDataHandler_IO,
    const std::vector<std::map<std::string, std::string>>& MRMFeatureFilter_filter_params_I,
    const bool verbose_I
  )
  {
    if (verbose_I) {
      std::cout << "==== START filterFeatures" << std::endl;
      std::cout << "filterFeatures: input size: " << rawDataHandler_IO.getFeatureMap().size() << std::endl;
    }

    if (MRMFeatureFilter_filter_params_I.empty()) {
      std::cout << "No parameters passed to filterFeatures(). Not filtering." << std::endl;
      return;
    }

    OpenMS::MRMFeatureFilter featureFilter;
    OpenMS::Param parameters = featureFilter.getParameters();
    Utilities::updateParameters(parameters, MRMFeatureFilter_filter_params_I);
    featureFilter.setParameters(parameters);

    OpenMS::FeatureMap& featureMap = rawDataHandler_IO.getFeatureMap();

    featureFilter.FilterFeatureMap(
      featureMap,
      rawDataHandler_IO.getFeatureFilter(),
      rawDataHandler_IO.getTargetedExperiment()
    );

    if (verbose_I) {
      std::cout << "filterFeatures: output size: " << featureMap.size() << std::endl;
      std::cout << "==== END   filterFeatures" << std::endl;
    }
  }

  void RawDataProcessor::checkFeatures(
    RawDataHandler& rawDataHandler_IO,
    const std::vector<std::map<std::string, std::string>>& MRMFeatureFilter_qc_params_I,
    const bool verbose_I
  )
  {
    if (verbose_I) {
      std::cout << "==== START checkFeatures" << std::endl;
      std::cout << "checkFeatures: input size: " << rawDataHandler_IO.getFeatureMap().size() << std::endl;
    }

    if (MRMFeatureFilter_qc_params_I.empty()) {
      std::cout << "No parameters passed to checkFeatures(). Not checking." << std::endl;
      return;
    }

    OpenMS::MRMFeatureFilter featureFilter;
    OpenMS::Param parameters = featureFilter.getParameters();
    Utilities::updateParameters(parameters, MRMFeatureFilter_qc_params_I);
    featureFilter.setParameters(parameters);

    OpenMS::FeatureMap& featureMap = rawDataHandler_IO.getFeatureMap();

    featureFilter.FilterFeatureMap(
      featureMap,
      rawDataHandler_IO.getFeatureQC(),
      rawDataHandler_IO.getTargetedExperiment()
    );

    rawDataHandler_IO.setFeatureMap(featureMap);

    if (verbose_I) {
      std::cout << "checkFeatures: output size: " << featureMap.size() << std::endl;
      std::cout << "==== END   checkFeatures" << std::endl;
    }
  }

  void RawDataProcessor::selectFeatures(
    RawDataHandler& rawDataHandler_IO,
    const std::vector<std::map<std::string, std::string>>& MRMFeatureSelector_select_params_I,
    const std::vector<std::map<std::string, std::string>>& MRMFeatureSelector_schedule_params_I,
    const bool verbose_I
  )
  {
    if (verbose_I) {
      std::cout << "==== START selectFeatures" << std::endl;
      std::cout << "selectFeatures: input size: " << rawDataHandler_IO.getFeatureMap().size() << std::endl;
    }

    OpenMS::FeatureMap output;

    std::vector<OpenMS::MRMFeatureSelector::SelectorParameters> p =
      Utilities::extractSelectorParameters(MRMFeatureSelector_schedule_params_I, MRMFeatureSelector_select_params_I);

    if (MRMFeatureSelector_schedule_params_I.size()) {
      OpenMS::MRMBatchFeatureSelector::batchMRMFeaturesQMIP(rawDataHandler_IO.getFeatureMap(), output, p);
    } else if (MRMFeatureSelector_schedule_params_I.size()) {
      OpenMS::MRMBatchFeatureSelector::batchMRMFeaturesScore(rawDataHandler_IO.getFeatureMap(), output, p);
    } else {
      throw std::invalid_argument("Both arguments 'select params' and 'schedule params' are empty.");
    }

    output.setPrimaryMSRunPath({rawDataHandler_IO.getMetaData().getFilename()});

    rawDataHandler_IO.setFeatureMap(output);

    if (verbose_I) {
      std::cout << "selectFeatures: output size: " << output.size() << std::endl;
      std::cout << "==== END   selectFeatures" << std::endl;
    }
  }

  void RawDataProcessor::extractMetaData(
    RawDataHandler& rawDataHandler_IO,
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

  void RawDataProcessor::validateFeatures(
    RawDataHandler& rawDataHandler_IO,
    const std::vector<std::map<std::string, std::string>>& MRMRFeatureValidator_params_I,
    const bool verbose_I
  )
  {
    if (verbose_I) {
      std::cout << "==== START validateFeatures" << std::endl;
    }

    if (MRMRFeatureValidator_params_I.empty()) {
      std::cout << "No parameters passed to validateFeatures(). Not validating." << std::endl;
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
      std::stof(MRMRFeatureValidator_params_I.front().at("value")), // TODO: While this probably works, it might be nice to add some check that the parameter passed is the desired one
      verbose_I
    );

    rawDataHandler_IO.setFeatureMap(mapped_features);
    rawDataHandler_IO.setValidationMetrics(validation_metrics);

    if (verbose_I) {
      std::cout << "==== END   validateFeatures" << std::endl;
    }
  }

  void RawDataProcessor::plotFeatures(
    RawDataHandler& rawDataHandler_IO,
    const std::string& filename,
    const std::vector<std::map<std::string, std::string>>& FeaturePlotter_params_I,
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

  void RawDataProcessor::quantifyComponents(
    RawDataHandler& rawDataHandler_IO,
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
    } catch (const std::exception& e) {
      std::cerr << "quantifyComponents(): " << e.what() << std::endl;
    }

    if (verbose_I) {
      std::cout << "==== END   quantifyComponents" << std::endl;
    }
  }

  void RawDataProcessor::processRawData(
    RawDataHandler& rawDataHandler_IO,
    const RawDataProcMethod raw_data_processing_event,
    const std::map<std::string, std::vector<std::map<std::string, std::string>>>& parameters,
    const Filenames& filenames,
    const bool verbose_I
  )
  {
    const RawDataProcMethod event = raw_data_processing_event;
    if (event == LOAD_RAW_DATA) {
      // fileReaderOpenMS.load_SWATHorDIA(rawDataHandler_IO, {})
      OpenMSFile::loadMSExperiment(
        rawDataHandler_IO,
        filenames.mzML_i,
        parameters.at("MRMMapping"),
        parameters.at("ChromatogramExtractor"),
        parameters.at("mzML"),
        verbose_I
      );
      // fileReaderOpenMS.load_Trafo(); # skip, no transformation of RT
    } else if (event == LOAD_FEATURES) {
      OpenMSFile::loadFeatureMap(
        rawDataHandler_IO,
        filenames.featureXML_i,
        verbose_I
      );
    } else if (event == PICK_FEATURES) {
      pickFeatures(
        rawDataHandler_IO,
        parameters.at("MRMFeatureFinderScoring"),
        verbose_I
      );
    } else if (event == FILTER_FEATURES) {
      filterFeatures(
        rawDataHandler_IO,
        parameters.at("MRMFeatureFilter.filter_MRMFeatures"),
        verbose_I
      );
    } else if (event == SELECT_FEATURES) {
      selectFeatures(
        rawDataHandler_IO,
        parameters.at("MRMFeatureSelector.select_MRMFeatures_qmip"),
        parameters.at("MRMFeatureSelector.schedule_MRMFeatures_qmip"),
        verbose_I
      );
    } else if (event == VALIDATE_FEATURES) {
      OpenMSFile::loadValidationData(rawDataHandler_IO, filenames.referenceData_csv_i);
      validateFeatures(
        rawDataHandler_IO,
        parameters.at("MRMFeatureValidator.validate_MRMFeatures"),
        verbose_I
      );
    } else if (event == QUANTIFY_FEATURES) {
      quantifyComponents(rawDataHandler_IO, verbose_I);
    } else if (event == CHECK_FEATURES) {
      checkFeatures(
        rawDataHandler_IO,
        parameters.at("MRMFeatureFilter.filter_MRMFeatures.qc"),
        verbose_I
      );
    } else if (event == STORE_FEATURES) {
      OpenMSFile::storeFeatureMap(
        rawDataHandler_IO,
        filenames.featureXML_o,
        verbose_I
      );
    } else if (event == PLOT_FEATURES) {
      plotFeatures(
        rawDataHandler_IO,
        filenames.features_pdf_o,
        parameters.at("FeaturePlotter"),
        verbose_I
      );
    } else if (event == SAVE_FEATURES) {
      saveCurrentFeatureMapToHistory(
        rawDataHandler_IO,
        verbose_I
      );
    } else if (event == ANNOTATE_USED_FEATURES) {
      annotateUsedFeatures(
        rawDataHandler_IO,
        verbose_I
      );
    } else if (event == CLEAR_FEATURE_HISTORY) {
      rawDataHandler_IO.getFeatureMapHistory().clear();
    }
  }

  std::vector<RawDataProcessor::RawDataProcMethod> RawDataProcessor::getDefaultRawDataProcessingWorkflow(
    const MetaDataHandler::SampleType sample_type
  )
  {
    const std::vector<RawDataProcMethod> opt {
      LOAD_RAW_DATA,     // 0
      PICK_FEATURES,     // 1
      FILTER_FEATURES,   // 2
      SELECT_FEATURES,   // 3
      QUANTIFY_FEATURES, // 4
      CHECK_FEATURES     // 5
    };
    switch (sample_type) {
      case MetaDataHandler::SampleType::Unknown:
      case MetaDataHandler::SampleType::Standard:
      case MetaDataHandler::SampleType::QC:
      case MetaDataHandler::SampleType::Blank:
        return opt;
      case MetaDataHandler::SampleType::DoubleBlank:
      case MetaDataHandler::SampleType::Solvent:
        return {opt[0], opt[1], opt[2], opt[3], opt[5]};
      default:
        throw "case not handled.";
    }
  }

  void RawDataProcessor::annotateUsedFeatures(
    RawDataHandler& rawDataHandler_IO,
    const bool verbose_I
  )
  {
    if (verbose_I)
      std::cout << "Annotating used features" << std::endl;

    OpenMS::FeatureMap features_annotated;
    for (OpenMS::Feature feature_copy : rawDataHandler_IO.getFeatureMapHistory().back()) {
      std::vector<OpenMS::Feature> subordinates_annotated;
      for (OpenMS::Feature subordinate_copy : feature_copy.getSubordinates()) {
        subordinate_copy.setMetaValue("used_", "false");
        subordinates_annotated.push_back(subordinate_copy);
      }
      for (const OpenMS::Feature& feature_select : rawDataHandler_IO.getFeatureMap()) {
        if (feature_select.getUniqueId() != feature_copy.getUniqueId())
          continue;
        for (OpenMS::Feature& subordinate_copy : subordinates_annotated) {
          subordinate_copy.setMetaValue("used_", "true");
        }
        break;
      }
      feature_copy.setSubordinates(subordinates_annotated);
      features_annotated.push_back(feature_copy);
    }

    rawDataHandler_IO.setFeatureMap(features_annotated);
  }

  void RawDataProcessor::saveCurrentFeatureMapToHistory(
    RawDataHandler& rawDataHandler_IO,
    const bool verbose_I
  )
  {
    if (verbose_I)
      std::cout << "Saving features" << std::endl;

    OpenMS::FeatureMap features_annotated;

    for (OpenMS::Feature feature_copy : rawDataHandler_IO.getFeatureMap()) {
      std::vector<OpenMS::Feature> subordinates_annotated;
      for (OpenMS::Feature subordinate_copy : feature_copy.getSubordinates()) {
        subordinates_annotated.push_back(subordinate_copy);
      }
      feature_copy.setSubordinates(subordinates_annotated);
      features_annotated.push_back(feature_copy);
    }

    rawDataHandler_IO.getFeatureMapHistory().push_back(features_annotated);
  }

  // TODO: implemented in error (thought it'd be necessary). Eventually remove it.
  // RawDataProcessor::RawDataProcMethod RawDataProcessor::convertEventStringToEnum(const std::string& event)
  // {
  //   if (event == "load_raw_data") {
  //     return LOAD_RAW_DATA;
  //   } else if (event == "load_features") {
  //     return LOAD_FEATURES;
  //   } else if (event == "pick_features") {
  //     return PICK_FEATURES;
  //   } else if (event == "filter_features") {
  //     return FILTER_FEATURES;
  //   } else if (event == "select_features") {
  //     return SELECT_FEATURES;
  //   } else if (event == "validate_features") {
  //     return VALIDATE_FEATURES;
  //   } else if (event == "quantify_features") {
  //     return QUANTIFY_FEATURES;
  //   } else if (event == "check_features") {
  //     return CHECK_FEATURES;
  //   } else if (event == "store_features") {
  //     return STORE_FEATURES;
  //   } else if (event == "plot_features") {
  //     return PLOT_FEATURES;
  //   } else if (event == "save_features") {
  //     return SAVE_FEATURES;
  //   } else if (event == "annotate_used_features") {
  //     return ANNOTATE_USED_FEATURES;
  //   } else if (event == "clear_feature_history") {
  //     return CLEAR_FEATURE_HISTORY;
  //   } else {
  //     throw "Raw data processing event " + event + " is not recognized.";
  //   }
  // }
}
