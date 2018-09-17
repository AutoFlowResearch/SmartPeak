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
  class RawDataHandlerProcessor
  {
public:
    RawDataHandlerProcessor() = default;
    ~RawDataHandlerProcessor() = default;

    void pickFeatures(
      RawDataHandler& rawDataHandler_IO,
      const std::vector<std::map<std::string, std::string>>& MRMFeatureFinderScoring_params_I,
      const bool verbose_I = false
    );

    void filterFeatures(
      RawDataHandler& rawDataHandler_IO,
      const std::vector<std::map<std::string, std::string>>& MRMFeatureFilter_filter_params_I,
      const bool verbose_I = false
    );

    void checkFeatures(
      RawDataHandler& rawDataHandler_IO,
      const std::vector<std::map<std::string, std::string>>& MRMFeatureFilter_qc_params_I,
      const bool verbose_I = false
    );

    void selectFeatures(
      RawDataHandler& rawDataHandler_IO,
      const std::vector<std::map<std::string, std::string>>& MRMFeatureSelector_select_params_I,
      const std::vector<std::map<std::string, std::string>>& MRMFeatureSelector_schedule_params_I,
      const bool verbose_I = false
    );

    void extractMetaData(
      RawDataHandler& rawDataHandler_IO,
      const bool verbose_I = false
    );

    void validateFeatures(
      RawDataHandler& rawDataHandler_IO,
      const std::vector<std::map<std::string, std::string>>& MRMRFeatureValidator_params_I,
      const bool verbose_I = false
    );

    void plotFeatures(
      RawDataHandler& rawDataHandler_IO,
      const std::string& filename,
      const std::vector<std::map<std::string, std::string>>& FeaturePlotter_params_I,
      const bool verbose_I = false
    );

    void quantifyComponents(
      RawDataHandler& rawDataHandler_IO,
      const bool verbose_I = false
    );

    void processRawData(
      RawDataHandler& rawDataHandler_IO,
      const std::string& raw_data_processing_event,
      const std::map<std::string, std::vector<std::map<std::string, std::string>>>& parameters,
      const std::map<std::string, std::string>& filenames,
      const bool verbose_I = false
    );

    bool checkRawDataProcessingWorkflow(
      const std::vector<std::string>& raw_data_processing_I
    );

    void getDefaultRawDataProcessingWorkflow(
      const MetaDataHandler::SampleType sample_type,
      std::vector<std::string>& default_workflow
    );

    void annotateUsedFeatures(
      RawDataHandler& rawDataHandler_IO,
      const bool verbose_I = false
    );

    void saveCurrentFeatureMapToHistory(
      RawDataHandler& rawDataHandler_IO,
      const bool verbose_I = false
    );
  };
}
