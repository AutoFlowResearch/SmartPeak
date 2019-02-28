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
    RawDataProcessor() = default;
    ~RawDataProcessor() = default;

    int getID() const { return id_; };  ///< get the raw data processor class ID
    std::string getName() const { return name_; };  ///< get the raw data processor class name
    std::string getDescription() const { return description_; };  ///< get the raw data processor class description

    /** Interface to all raw data processing methods.

      @param[in,out] rawDataHandler_IO Raw data file class
      @param[in] params_I Dictionary of parameter names, values, descriptions, and tags
      @param[in] filenames Info about where data should be read from or written to
    */
    virtual void process(
      RawDataHandler& rawDataHandler_IO,
      const std::map<std::string, std::vector<std::map<std::string, std::string>>>& params_I,
      const Filenames& filenames,
      const bool verbose_I = false) = 0;

  protected:
    int id_ = 0;  ///< the raw data processor class ID
    std::string name_ = "";  ///< the raw data processor class name
    std::string description_ = "";  ///< the raw data processor class description
  };

  class LoadRawData : public RawDataProcessor
  {
  public:
    using RawDataProcessor::RawDataProcessor;
    using RawDataProcessor::~RawDataProcessor;

    /** Read in raw data mzML file from disk.

      Depending upon user specifications, the mzML file will be mapped to the TraML file
      and/or various pre-processing methods will be run to extract out the data.
    */
    static void process(
      RawDataHandler& rawDataHandler_IO,
      const std::map<std::string, std::vector<std::map<std::string, std::string>>>& params_I,
      const Filenames& filenames,
      const bool verbose_I = false
    );

  protected:
    int id_ = 1;
    std::string name_ = "LOAD_RAW_DATA";
    std::string description_ = "Read in raw data mzML file from disk.";
  };

  class StoreRawData : public RawDataProcessor
  {
  public:
    using RawDataProcessor::RawDataProcessor;
    using RawDataProcessor::~RawDataProcessor;

    /** Run the openSWATH workflow for a single raw data file.
    */
    static void process(
      RawDataHandler& rawDataHandler_IO,
      const std::map<std::string, std::vector<std::map<std::string, std::string>>>& params_I,
      const Filenames& filenames,
      const bool verbose_I = false
    );

  protected:
    int id_ = 2;
    std::string name_ = "STORE_RAW_DATA";
    std::string description_ = "Store the processed raw data mzML file to disk.";
  };

  class LoadFeatures : public RawDataProcessor
  {
  public:
    using RawDataProcessor::RawDataProcessor;
    using RawDataProcessor::~RawDataProcessor;

    /** Read in the features from disk.
    */
    static void process(
      RawDataHandler& rawDataHandler_IO,
      const std::map<std::string, std::vector<std::map<std::string, std::string>>>& params_I,
      const Filenames& filenames,
      const bool verbose_I = false
    );

  protected:
    int id_ = 3;
    std::string name_ = "LOAD_FEATURES";
    std::string description_ = "Read in the features from disk.";
  };

  class StoreFeatures : public RawDataProcessor
  {
  public:
    using RawDataProcessor::RawDataProcessor;
    using RawDataProcessor::~RawDataProcessor;

    /** Write the features to disk.
    */
    static void process(
      RawDataHandler& rawDataHandler_IO,
      const std::map<std::string, std::vector<std::map<std::string, std::string>>>& params_I,
      const Filenames& filenames,
      const bool verbose_I = false
    );

  protected:
    int id_ = 4;
    std::string name_ = "STORE_FEATURES";
    std::string description_ = "Write the features to disk.";
  };

  class PickFeatures : public RawDataProcessor
  {
  public:
    using RawDataProcessor::RawDataProcessor;
    using RawDataProcessor::~RawDataProcessor;

    /** Run the openSWATH pick peaking and scoring workflow for a single raw data file.
    */
    static void process(
      RawDataHandler& rawDataHandler_IO,
      const std::map<std::string, std::vector<std::map<std::string, std::string>>>& params_I,
      const Filenames& filenames,
      const bool verbose_I = false
    );

  protected:
    int id_ = 5;
    std::string name_ = "PICK_FEATURES";
    std::string description_ = "Run the peak picking algorithm.";
  };

  class FilterFeatures : public RawDataProcessor
  {
  public:
    using RawDataProcessor::RawDataProcessor;
    using RawDataProcessor::~RawDataProcessor;

    /** Filter features that do not pass the filter QCs.
    */
    static void process(
      RawDataHandler& rawDataHandler_IO,
      const std::map<std::string, std::vector<std::map<std::string, std::string>>>& params_I,
      const Filenames& filenames,
      const bool verbose_I = false
    );

  protected:
    int id_ = 6;
    std::string name_ = "FILTER_FEATURES";
    std::string description_ = "Filter transitions and transitions groups based on a user defined criteria.";
  };

  class CheckFeatures : public RawDataProcessor
  {
  public:
    using RawDataProcessor::RawDataProcessor;
    using RawDataProcessor::~RawDataProcessor;

    /** Flag features that do not pass the filter QCs.
    */
    static void process(
      RawDataHandler& rawDataHandler_IO,
      const std::map<std::string, std::vector<std::map<std::string, std::string>>>& params_I,
      const Filenames& filenames,
      const bool verbose_I = false
    );

  protected:
    int id_ = 7;
    std::string name_ = "CHECK_FEATURES";
    std::string description_ = "Flag and score transitions and transition groups based on a user defined criteria.";
  };

  class SelectFeatures : public RawDataProcessor
  {
  public:
    using RawDataProcessor::RawDataProcessor;
    using RawDataProcessor::~RawDataProcessor;

    /** Select features using the MRMFeatureSelection algorithm.
    */
    static void process(
      RawDataHandler& rawDataHandler_IO,
      const std::map<std::string, std::vector<std::map<std::string, std::string>>>& params_I,
      const Filenames& filenames,
      const bool verbose_I = false
    );

  protected:
    int id_ = 8;
    std::string name_ = "SELECT_FEATURES";
    std::string description_ = "Run the peak selection/alignment algorithm.";
  };

  class ExtractMetaData : public RawDataProcessor
  {
  public:
    using RawDataProcessor::RawDataProcessor;
    using RawDataProcessor::~RawDataProcessor;

    /** Extracts metadata from the chromatogram.
    */
    static void process(
      RawDataHandler& rawDataHandler_IO,
      const std::map<std::string, std::vector<std::map<std::string, std::string>>>& params_I,
      const Filenames& filenames,
      const bool verbose_I = false
    );

  protected:
    int id_ = -1;  // Not currently available to the user
    std::string name_ = "EXTRACT_METADATA";
    std::string description_ = "Extracts metadata from the chromatogram.";
  };

  class ValidateFeatures : public RawDataProcessor
  {
  public:
    using RawDataProcessor::RawDataProcessor;
    using RawDataProcessor::~RawDataProcessor;

    /** Validate the selected peaks against reference data.
    */
    static void process(
      RawDataHandler& rawDataHandler_IO,
      const std::map<std::string, std::vector<std::map<std::string, std::string>>>& params_I,
      const Filenames& filenames,
      const bool verbose_I = false
    );

  protected:
    int id_ = 9;
    std::string name_ = "VALIDATE_FEATURES";
    std::string description_ = "Compare selected features to a reference data set.";
  };

  class QuantifyFeatures : public RawDataProcessor
  {
  public:
    using RawDataProcessor::RawDataProcessor;
    using RawDataProcessor::~RawDataProcessor;

    /** Quantify all unknown samples based on the quantitationMethod.
    */
    static void process(
      RawDataHandler& rawDataHandler_IO,
      const std::map<std::string, std::vector<std::map<std::string, std::string>>>& params_I,
      const Filenames& filenames,
      const bool verbose_I = false
    );

  protected:
    int id_ = 10;
    std::string name_ = "QUANTIFY_FEATURES";
    std::string description_ = "Apply a calibration model defined in quantitationMethods to each transition.";
  };

  class PlotFeatures : public RawDataProcessor
  {
  public:
    using RawDataProcessor::RawDataProcessor;
    using RawDataProcessor::~RawDataProcessor;

    /** Validate the selected peaks against reference data.
    */
    static void process(
      RawDataHandler& rawDataHandler_IO,
      const std::map<std::string, std::vector<std::map<std::string, std::string>>>& params_I,
      const Filenames& filenames,
      const bool verbose_I = false
    );

  protected:
    int id_ = 11;
    std::string name_ = "PLOT_FEATURES";
    std::string description_ = "Plot the raw chromatogram with selected peaks overlaid.";
  };

  class LoadTransitions : public RawDataProcessor
  {
  public:
    using RawDataProcessor::RawDataProcessor;
    using RawDataProcessor::~RawDataProcessor;

    /** Load the transitions from the TraML file.
    */
    static void process(
      RawDataHandler& rawDataHandler_IO,
      const std::map<std::string, std::vector<std::map<std::string, std::string>>>& params_I,
      const Filenames& filenames,
      const bool verbose_I = false
    );

  protected:
    int id_ = 12;
    std::string name_ = "LOAD_TRANSITIONS";
    std::string description_ = "Load the transitions for the SRM experiments from the TraML file.";
  };

  class LoadFeatureFilters : public RawDataProcessor
  {
  public:
    using RawDataProcessor::RawDataProcessor;
    using RawDataProcessor::~RawDataProcessor;

    /** Load the component and component group transition filters from file.
    */
    static void process(
      RawDataHandler& rawDataHandler_IO,
      const std::map<std::string, std::vector<std::map<std::string, std::string>>>& params_I,
      const Filenames& filenames,
      const bool verbose_I = false
    );

  protected:
    int id_ = 13;
    std::string name_ = "LOAD_FEATURE_FILTERS";
    std::string description_ = "Load the component and component group transition filters from file.";
  };

  class LoadFeatureQCs : public RawDataProcessor
  {
  public:
    using RawDataProcessor::RawDataProcessor;
    using RawDataProcessor::~RawDataProcessor;

    /** Load the component and component group transition QCs from file.
    */
    static void process(
      RawDataHandler& rawDataHandler_IO,
      const std::map<std::string, std::vector<std::map<std::string, std::string>>>& params_I,
      const Filenames& filenames,
      const bool verbose_I = false
    );

  protected:
    int id_ = 14;
    std::string name_ = "LOAD_FEATURE_QCS";
    std::string description_ = "Load the component and component group transition QC specifications from file.";
  };

  class LoadValidationData : public RawDataProcessor
  {
  public:
    using RawDataProcessor::RawDataProcessor;
    using RawDataProcessor::~RawDataProcessor;

    /** Load the validation data from file.
    */
    static void process(
      RawDataHandler& rawDataHandler_IO,
      const std::map<std::string, std::vector<std::map<std::string, std::string>>>& params_I,
      const Filenames& filenames,
      const bool verbose_I = false
    );

  protected:
    int id_ = 15;
    std::string name_ = "LOAD_VALIDATION_DATA";
    std::string description_ = "Load the validation data from file.";
  };

  class LoadParameters : public RawDataProcessor
  {
  public:
    using RawDataProcessor::RawDataProcessor;
    using RawDataProcessor::~RawDataProcessor;

    /** Load the data processing parameters from file.
    */
    static void process(
      RawDataHandler& rawDataHandler_IO,
      const std::map<std::string, std::vector<std::map<std::string, std::string>>>& params_I,
      const Filenames& filenames,
      const bool verbose_I = false
    );

  protected:
    int id_ = 16;
    std::string name_ = "LOAD_PARAMETERS";
    std::string description_ = "Load the data processing parameters from file.";
  };
}
