/**TODO:  Add copyright*/

#pragma once

#include <OpenMS/KERNEL/FeatureMap.h>
#include <OpenMS/ANALYSIS/TARGETED/TargetedExperiment.h>
#include <OpenMS/KERNEL/MSExperiment.h>
#include <OpenMS/ANALYSIS/OPENSWATH/MRMFeatureQC.h>
#include <OpenMS/ANALYSIS/QUANTITATION/AbsoluteQuantitationMethod.h>
#include <OpenMS/ANALYSIS/MAPMATCHING/TransformationDescription.h>
#include <SmartPeak/core/RawDataHandler.h>

#include <SmartPeak/core/MetaDataHandler.h>

#include <map>
#include <vector>

namespace SmartPeak
{
  class RawDataProcessor
  {
public:
    RawDataProcessor() = delete;
    ~RawDataProcessor() = delete;

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
      const std::string& raw_data_processing_event,
      const std::map<std::string, std::vector<std::map<std::string, std::string>>>& parameters,
      const std::map<std::string, std::string>& filenames,
      const bool verbose_I = false
    );

    static bool checkRawDataProcessingWorkflow(
      const std::vector<std::string>& raw_data_processing_I
    );

    static void getDefaultRawDataProcessingWorkflow(
      const MetaDataHandler::SampleType sample_type,
      std::vector<std::string>& default_workflow
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
