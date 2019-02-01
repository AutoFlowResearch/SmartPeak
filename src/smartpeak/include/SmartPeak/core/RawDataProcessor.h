/**TODO:  Add copyright*/

#pragma once

#include <OpenMS/KERNEL/FeatureMap.h>
#include <OpenMS/ANALYSIS/TARGETED/TargetedExperiment.h>
#include <OpenMS/KERNEL/MSExperiment.h>
#include <OpenMS/ANALYSIS/OPENSWATH/MRMFeatureQC.h>
#include <OpenMS/ANALYSIS/QUANTITATION/AbsoluteQuantitationMethod.h>
#include <OpenMS/ANALYSIS/MAPMATCHING/TransformationDescription.h>

#include <SmartPeak/core/Filenames.h>
#include <SmartPeak/core/MetaDataHandler.h>
#include <SmartPeak/core/RawDataHandler.h>

#include <map>
#include <vector>

namespace SmartPeak
{
  class RawDataProcessor
  {
public:
    RawDataProcessor() = delete;
    ~RawDataProcessor() = delete;

    enum RawDataProcMethod {
      LOAD_RAW_DATA = 1,
      LOAD_FEATURES,
      PICK_FEATURES,
      FILTER_FEATURES,
      SELECT_FEATURES,
      VALIDATE_FEATURES,
      QUANTIFY_FEATURES,
      CHECK_FEATURES,
      STORE_FEATURES,
      PLOT_FEATURES,
      SAVE_FEATURES,
      ANNOTATE_USED_FEATURES,
      CLEAR_FEATURE_HISTORY,
    };

    // static RawDataProcMethod convertEventStringToEnum(const std::string& event);

    static void pickFeatures(
      RawDataHandler& rawDataHandler_IO,
      const std::vector<std::map<std::string, std::string>>& MRMFeatureFinderScoring_params_I,
      const bool verbose_I = false
    );

    static void filterFeatures(
      RawDataHandler& rawDataHandler_IO,
      const std::vector<std::map<std::string, std::string>>& MRMFeatureFilter_filter_params_I,
      const bool verbose_I = false
    );

    static void checkFeatures(
      RawDataHandler& rawDataHandler_IO,
      const std::vector<std::map<std::string, std::string>>& MRMFeatureFilter_qc_params_I,
      const bool verbose_I = false
    );

    static void selectFeatures(
      RawDataHandler& rawDataHandler_IO,
      const std::vector<std::map<std::string, std::string>>& MRMFeatureSelector_select_params_I,
      const std::vector<std::map<std::string, std::string>>& MRMFeatureSelector_schedule_params_I,
      const bool verbose_I = false
    );

    static void extractMetaData(
      RawDataHandler& rawDataHandler_IO,
      const bool verbose_I = false
    );

    static void validateFeatures(
      RawDataHandler& rawDataHandler_IO,
      const std::vector<std::map<std::string, std::string>>& MRMRFeatureValidator_params_I,
      const bool verbose_I = false
    );

    static void plotFeatures(
      RawDataHandler& rawDataHandler_IO,
      const std::string& filename,
      const std::vector<std::map<std::string, std::string>>& FeaturePlotter_params_I,
      const bool verbose_I = false
    );

    static void quantifyComponents(
      RawDataHandler& rawDataHandler_IO,
      const bool verbose_I = false
    );

    static void processRawData(
      RawDataHandler& rawDataHandler_IO,
      const RawDataProcMethod raw_data_processing_event,
      const std::map<std::string, std::vector<std::map<std::string, std::string>>>& parameters,
      const Filenames& filenames,
      const bool verbose_I = false
    );

    static std::vector<RawDataProcMethod> getDefaultRawDataProcessingWorkflow(
      const MetaDataHandler::SampleType sample_type
    );

    static void annotateUsedFeatures(
      RawDataHandler& rawDataHandler_IO,
      const bool verbose_I = false
    );

    static void saveCurrentFeatureMapToHistory(
      RawDataHandler& rawDataHandler_IO,
      const bool verbose_I = false
    );
  };
}
