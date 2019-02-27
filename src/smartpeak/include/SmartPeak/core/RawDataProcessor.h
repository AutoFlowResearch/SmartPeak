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
    };

    // static RawDataProcMethod convertEventStringToEnum(const std::string& event);

    /** Run the openSWATH workflow for a single raw data file.

      @param[in,out] rawDataHandler_IO Raw data file class
      @param[in] MRMFeatureFinderScoring_params_I Dictionary of parameter names, values, descriptions, and tags
      @param[in] verbose_I Verbosity
    */
    static void pickFeatures(
      RawDataHandler& rawDataHandler_IO,
      const std::vector<std::map<std::string, std::string>>& MRMFeatureFinderScoring_params_I,
      const bool verbose_I = false
    );

    /** Filter features that do not pass the filter QCs.

      @param[in,out] rawDataHandler_IO Raw data file class
      @param[in] MRMFeatureFilter_filter_params_I Dictionary of parameter names, values, descriptions, and tags
      @param[in] verbose_I Verbosity
    */
    static void filterFeatures(
      RawDataHandler& rawDataHandler_IO,
      const std::vector<std::map<std::string, std::string>>& MRMFeatureFilter_filter_params_I,
      const bool verbose_I = false
    );

    /** Flag features that do not pass the filter QCs.

      @param[in,out] rawDataHandler_IO Raw data file class
      @param[in] MRMFeatureFilter_qc_params_I Dictionary of parameter names, values, descriptions, and tags
      @param[in] verbose_I Verbosity
    */
    static void checkFeatures(
      RawDataHandler& rawDataHandler_IO,
      const std::vector<std::map<std::string, std::string>>& MRMFeatureFilter_qc_params_I,
      const bool verbose_I = false
    );

    /** Run the openSWATH post processing filtering workflow for a single sample.

      @note TODO: Break into seperate methods or change the logic of how selection algorithms are chosen

      @param[in,out] rawDataHandler_IO Raw data file class
      @param[in] MRMFeatureSelector_select_params_I Dictionary of parameter names, values, descriptions, and tags
      @param[in] MRMFeatureSelector_schedule_params_I Dictionary of parameter names, values, descriptions, and tags
      @param[in] verbose_I Verbosity
    */
    static void selectFeatures(
      RawDataHandler& rawDataHandler_IO,
      const std::vector<std::map<std::string, std::string>>& MRMFeatureSelector_select_params_I,
      const std::vector<std::map<std::string, std::string>>& MRMFeatureSelector_schedule_params_I,
      const bool verbose_I = false
    );

    /** Extracts metadata from the chromatogram.

      @param[in,out] rawDataHandler_IO Raw data file class
      @param[in] verbose_I Verbosity
    */
    static void extractMetaData(
      RawDataHandler& rawDataHandler_IO,
      const bool verbose_I = false
    );

    /** Validate the selected peaks against reference data.

      @param[in,out] rawDataHandler_IO Raw data file class
      @param[in] MRMRFeatureValidator_params_I Dictionary of parameter names, values, descriptions, and tags
      @param[in] verbose_I Verbosity
    */
    static void validateFeatures(
      RawDataHandler& rawDataHandler_IO,
      const std::vector<std::map<std::string, std::string>>& MRMRFeatureValidator_params_I,
      const bool verbose_I = false
    );

    /** Export plots of peaks with features annotated.

      @param[in,out] rawDataHandler_IO Raw data file class
      @param[in] filename Output filename
      @param[in] FeaturePlotter_params_I Dictionary of parameter names, values, descriptions, and tags
      @param[in] verbose_I Verbosity
    */
    static void plotFeatures(
      RawDataHandler& rawDataHandler_IO,
      const std::vector<std::map<std::string, std::string>>& FeaturePlotter_params_I,
      const bool verbose_I = false
    );

    /** Quantify all unknown samples based on the quantitationMethod.

      @param[in,out] rawDataHandler_IO Raw data file class
      @param[in] verbose_I Verbosity
    */
    static void quantifyComponents(
      RawDataHandler& rawDataHandler_IO,
      const bool verbose_I = false
    );

    /** Apply processing event to a RawDataHandler.

      @param[in,out] rawDataHandler_IO Raw data file class
      @param[in] raw_data_processing_event Processing event to apply
      @param[in] MRMRFeatureValidator_params_I Dictionary of parameter names, values, descriptions, and tags
      @param[in] filenames Info about where data should be read from or written to
      @param[in] verbose_I Verbosity
    */
    static void processRawData(
      RawDataHandler& rawDataHandler_IO,
      const RawDataProcMethod raw_data_processing_event,
      const std::map<std::string, std::vector<std::map<std::string, std::string>>>& parameters,
      const Filenames& filenames,
      const bool verbose_I = false
    );
  };
}
