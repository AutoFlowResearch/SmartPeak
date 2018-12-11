// TODO: Add copyright

#include <SmartPeak/core/RawDataProcessor.h>
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

    featureMap.setPrimaryMSRunPath({rawDataHandler_IO.getMetaData().getSampleName()});

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

    output.setPrimaryMSRunPath({rawDataHandler_IO.getMetaData().getSampleName()});

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
    // std::string instrument;
    // std::string software;

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
      throw "no sample name?\n";
    }

    // const OpenMS::String& instrument_name = rawDataHandler_IO.getChromatogramMap().getInstrument().getName();
    // if (instrument_name.size()) {
    //   instrument = instrument_name;
    //   const OpenMS::String& software_name = rawDataHandler_IO.getChromatogramMap().getInstrument().getSoftware().getName();
    //   if (software_name.size())
    //     software = software_name;
    // }

    rawDataHandler_IO.getMetaData().setSampleName(samplename);
    rawDataHandler_IO.getMetaData().setFilename(filename);
  }

  void RawDataProcessor::validateFeatures(
    RawDataHandler& rawDataHandler_IO,
    const std::vector<std::map<std::string, std::string>>& MRMRFeatureValidator_params_I,
    const bool verbose_I
  )
  {
    if (verbose_I)
      std::cout << "Validating features" << std::endl;

    if (MRMRFeatureValidator_params_I.empty()) {
      std::cout << "No parameters passed to validateFeatures(). Not validating." << std::endl;
      return;
    }

    OpenMS::FeatureMap mapped_features;
    std::map<std::string, float> validation_metrics; // keys: accuracy, recall, precision

    MRMFeatureValidator::validate_MRMFeatures(
      rawDataHandler_IO.getReferenceData(),
      rawDataHandler_IO.getFeatureMap(),
      rawDataHandler_IO.getMetaData().getSampleName(),
      mapped_features,
      validation_metrics,
      std::stof(MRMRFeatureValidator_params_I.front().at("value")), // TODO: While this probably works, it might be nice to add some check that the parameter passed is the desired one
      verbose_I
    );

    rawDataHandler_IO.setFeatureMap(mapped_features);
    rawDataHandler_IO.setValidationMetrics(validation_metrics);
  }

  void RawDataProcessor::plotFeatures(
    RawDataHandler& rawDataHandler_IO,
    const std::string& filename,
    const std::vector<std::map<std::string, std::string>>& FeaturePlotter_params_I,
    const bool verbose_I
  )
  {
    if (verbose_I)
      std::cout << "Plotting peaks with features" << std::endl;

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
    const std::string& raw_data_processing_event,
    const std::map<std::string, std::vector<std::map<std::string, std::string>>>& parameters,
    const std::map<std::string, std::string>& filenames,
    const bool verbose_I
  )
  {
    const std::string& event = raw_data_processing_event;
    if (event == "load_raw_data") {
      // fileReaderOpenMS.load_SWATHorDIA(rawDataHandler_IO, {})
      OpenMSFile::loadMSExperiment(
        rawDataHandler_IO,
        filenames.at("mzML_i"),
        parameters.at("MRMMapping"),
        parameters.at("ChromatogramExtractor"),
        parameters.at("mzML"),
        verbose_I
      );
      // fileReaderOpenMS.load_Trafo(); # skip, no transformation of RT
    } else if (event == "load_features") {
      OpenMSFile::loadFeatureMap(
        rawDataHandler_IO,
        filenames.at("featureXML_i"),
        verbose_I
      );
    } else if (event == "pick_features") {
      pickFeatures(
        rawDataHandler_IO,
        parameters.at("MRMFeatureFinderScoring"),
        verbose_I
      );
    } else if (event == "filter_features") {
      filterFeatures(
        rawDataHandler_IO,
        parameters.at("MRMFeatureFilter.filter_MRMFeatures"),
        verbose_I
      );
    } else if (event == "select_features") {
      selectFeatures(
        rawDataHandler_IO,
        parameters.at("MRMFeatureSelector.select_MRMFeatures_qmip"),
        parameters.at("MRMFeatureSelector.schedule_MRMFeatures_qmip"),
        verbose_I
      );
    } else if (event == "validate_features") {
      OpenMSFile::loadValidationData(rawDataHandler_IO, filenames.at("referenceData_csv_i"));
      validateFeatures(
        rawDataHandler_IO,
        parameters.at("MRMFeatureValidator.validate_MRMFeatures"),
        verbose_I
      );
    } else if (event == "quantify_features") {
      quantifyComponents(rawDataHandler_IO, verbose_I);
    } else if (event == "check_features") {
      checkFeatures(
        rawDataHandler_IO,
        parameters.at("MRMFeatureFilter.filter_MRMFeatures.qc"),
        verbose_I
      );
    } else if (event == "store_features") {
      OpenMSFile::storeFeatureMap(
        rawDataHandler_IO,
        filenames.at("featureXML_o"),
        verbose_I
      );
    } else if (event == "plot_features") {
      plotFeatures(
        rawDataHandler_IO,
        filenames.at("features_pdf_o"),
        parameters.at("FeaturePlotter"),
        verbose_I
      );
    } else if (event == "save_features") {
      saveCurrentFeatureMapToHistory(
        rawDataHandler_IO,
        verbose_I
      );
    } else if (event == "annotate_used_features") {
      annotateUsedFeatures(
        rawDataHandler_IO,
        verbose_I
      );
    } else if (event == "clear_feature_history") {
      rawDataHandler_IO.getFeatureMapHistory().clear();
    } else {
      throw "Raw data processing event " + raw_data_processing_event + " was not recognized.";
    }
  }

  bool RawDataProcessor::checkRawDataProcessingWorkflow(
    const std::vector<std::string>& raw_data_processing_I
  )
  {
    const std::set<std::string> valid_events = {
      "load_raw_data",
      "load_features",
      "pick_features",
      "filter_features",
      "select_features",
      "validate_features",
      "quantify_features",
      "check_features",
      "plot_features",
      "store_features",
      "save_features",
      "annotate_used_features",
      "clear_feature_history"
    };
    bool is_valid = true;
    for (const std::string& event : raw_data_processing_I) {
      if (0 == valid_events.count(event)) {
        std::cout << "Raw data processing event '" << event << "' is not valid." << std::endl;
        is_valid = false;
      }
    }
    return is_valid;
  }

  void RawDataProcessor::getDefaultRawDataProcessingWorkflow(
    const MetaDataHandler::SampleType sample_type,
    std::vector<std::string>& default_workflow
  )
  {
    const std::vector<std::string> opt {
      "load_raw_data",     // 0
      "pick_features",     // 1
      "filter_features",   // 2
      "select_features",   // 3
      "quantify_features", // 4
      "check_features"     // 5
    };
    switch (sample_type) {
      case MetaDataHandler::SampleType::Unknown:
      case MetaDataHandler::SampleType::Standard:
      case MetaDataHandler::SampleType::QC:
      case MetaDataHandler::SampleType::Blank:
        default_workflow = opt;
        break;
      case MetaDataHandler::SampleType::DoubleBlank:
      case MetaDataHandler::SampleType::Solvent:
        default_workflow = {opt[0], opt[1], opt[2], opt[3], opt[5]};
        break;
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
}
