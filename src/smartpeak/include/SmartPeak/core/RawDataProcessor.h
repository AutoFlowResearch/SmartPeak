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
    virtual ~RawDataProcessor() = default;

    virtual int getID() const = 0;  ///< get the raw data processor class ID
    virtual std::string getName() const = 0;  ///< get the raw data processor class name
    virtual std::string getDescription() const = 0;  ///< get the raw data processor class description

    /** Interface to all raw data processing methods.

      @param[in,out] rawDataHandler_IO Raw data file class
      @param[in] params_I Dictionary of parameter names, values, descriptions, and tags
      @param[in] filenames Info about where data should be read from or written to
    */
    virtual void process(
      RawDataHandler& rawDataHandler_IO,
      const std::map<std::string, std::vector<std::map<std::string, std::string>>>& params_I,
      const Filenames& filenames
    ) const = 0;
  };

  class LoadRawData : public RawDataProcessor
  {
  public:
    // using RawDataProcessor::RawDataProcessor;

    int getID() const { return id_; };
    std::string getName() const { return name_; };
    std::string getDescription() const { return description_; };

    /** Read in raw data mzML file from disk.

      Depending upon user specifications, the mzML file will be mapped to the TraML file
      and/or various pre-processing methods will be run to extract out the data.
    */
    void process(
      RawDataHandler& rawDataHandler_IO,
      const std::map<std::string, std::vector<std::map<std::string, std::string>>>& params_I,
      const Filenames& filenames
    ) const override;

    /** Extracts metadata from the chromatogram.
    */
    static void extractMetaData(
      RawDataHandler& rawDataHandler_IO
    );

  protected:
    int id_ = 1;
    std::string name_ = "LOAD_RAW_DATA";
    std::string description_ = "Read in raw data mzML file from disk.";
  };

  class StoreRawData : public RawDataProcessor
  {
  public:
    // using RawDataProcessor::RawDataProcessor;

    int getID() const { return id_; };
    std::string getName() const { return name_; };
    std::string getDescription() const { return description_; };

    /** Store the processed raw data mzML file to disk.
    */
    void process(
      RawDataHandler& rawDataHandler_IO,
      const std::map<std::string, std::vector<std::map<std::string, std::string>>>& params_I,
      const Filenames& filenames
    ) const override;

  protected:
    int id_ = 2;
    std::string name_ = "STORE_RAW_DATA";
    std::string description_ = "Store the processed raw data mzML file to disk.";
  };

  class ZeroChromatogramBaseline : public RawDataProcessor
  {
  public:
    // using RawDataProcessor::RawDataProcessor;

    int getID() const { return id_; };
    std::string getName() const { return name_; };
    std::string getDescription() const { return description_; };

    /** Normalize the lowest chromatogram intensity to zero FOR MAPPED CHROMATOGRAMS.
    */
    void process(
      RawDataHandler& rawDataHandler_IO,
      const std::map<std::string, std::vector<std::map<std::string, std::string>>>& params_I,
      const Filenames& filenames
    ) const override;

  protected:
    int id_ = 17;
    std::string name_ = "ZERO_CHROMATOGRAM_BASELINE";
    std::string description_ = "Normalize the lowest chromatogram intensity to zero.";
  };

  class MapChromatograms : public RawDataProcessor
  {
  public:
    // using RawDataProcessor::RawDataProcessor;

    int getID() const { return id_; };
    std::string getName() const { return name_; };
    std::string getDescription() const { return description_; };

    /** Map chromatograms to the loaded set of transitions.
    */
    void process(
      RawDataHandler& rawDataHandler_IO,
      const std::map<std::string, std::vector<std::map<std::string, std::string>>>& params_I,
      const Filenames& filenames
    ) const override;

  protected:
    int id_ = 18;
    std::string name_ = "MAP_CHROMATOGRAMS";
    std::string description_ = "Map chromatograms to the loaded set of transitions.";
  };

  class ExtractChromatogramWindows : public RawDataProcessor
  {
  public:
    // using RawDataProcessor::RawDataProcessor;

    int getID() const { return id_; };
    std::string getName() const { return name_; };
    std::string getDescription() const { return description_; };

    /** Extract out specified chromatogram windows FROM A MAPPED CHROMATOGRAM using the componentFeatureFilters
    */
    void process(
      RawDataHandler& rawDataHandler_IO,
      const std::map<std::string, std::vector<std::map<std::string, std::string>>>& params_I,
      const Filenames& filenames
    ) const override;

  protected:
    int id_ = 19;
    std::string name_ = "EXTRACT_CHROMATOGRAM_WINDOWS";
    std::string description_ = "Extract out specified chromatogram windows using the componentFeatureFilters.";
  };

  class LoadFeatures : public RawDataProcessor
  {
  public:
    // using RawDataProcessor::RawDataProcessor;

    int getID() const { return id_; };
    std::string getName() const { return name_; };
    std::string getDescription() const { return description_; };

    /** Read in the features from disk.
    */
    void process(
      RawDataHandler& rawDataHandler_IO,
      const std::map<std::string, std::vector<std::map<std::string, std::string>>>& params_I,
      const Filenames& filenames
    ) const override;

  protected:
    int id_ = 3;
    std::string name_ = "LOAD_FEATURES";
    std::string description_ = "Read in the features from disk.";
  };

  class StoreFeatures : public RawDataProcessor
  {
  public:
    // using RawDataProcessor::RawDataProcessor;

    int getID() const { return id_; };
    std::string getName() const { return name_; };
    std::string getDescription() const { return description_; };

    /** Write the features to disk.
    */
    void process(
      RawDataHandler& rawDataHandler_IO,
      const std::map<std::string, std::vector<std::map<std::string, std::string>>>& params_I,
      const Filenames& filenames
    ) const override;

  protected:
    int id_ = 4;
    std::string name_ = "STORE_FEATURES";
    std::string description_ = "Write the features to disk.";
  };

  class PickFeatures : public RawDataProcessor
  {
  public:
    // using RawDataProcessor::RawDataProcessor;

    int getID() const { return id_; };
    std::string getName() const { return name_; };
    std::string getDescription() const { return description_; };

    /** Run the openSWATH pick peaking and scoring workflow for a single raw data file.
    */
    void process(
      RawDataHandler& rawDataHandler_IO,
      const std::map<std::string, std::vector<std::map<std::string, std::string>>>& params_I,
      const Filenames& filenames
    ) const override;

  protected:
    int id_ = 5;
    std::string name_ = "PICK_FEATURES";
    std::string description_ = "Run the peak picking algorithm.";
  };

  class FilterFeatures : public RawDataProcessor
  {
  public:
    // using RawDataProcessor::RawDataProcessor;

    int getID() const { return id_; };
    std::string getName() const { return name_; };
    std::string getDescription() const { return description_; };

    /** Filter features that do not pass the filter QCs.
    */
    void process(
      RawDataHandler& rawDataHandler_IO,
      const std::map<std::string, std::vector<std::map<std::string, std::string>>>& params_I,
      const Filenames& filenames
    ) const override;

  protected:
    int id_ = 6;
    std::string name_ = "FILTER_FEATURES";
    std::string description_ = "Filter transitions and transitions groups based on a user defined criteria.";
  };

  class CheckFeatures : public RawDataProcessor
  {
  public:
    // using RawDataProcessor::RawDataProcessor;

    int getID() const { return id_; };
    std::string getName() const { return name_; };
    std::string getDescription() const { return description_; };

    /** Flag features that do not pass the filter QCs.
    */
    void process(
      RawDataHandler& rawDataHandler_IO,
      const std::map<std::string, std::vector<std::map<std::string, std::string>>>& params_I,
      const Filenames& filenames
    ) const override;

  protected:
    int id_ = 7;
    std::string name_ = "CHECK_FEATURES";
    std::string description_ = "Flag and score transitions and transition groups based on a user defined criteria.";
  };

  class SelectFeatures : public RawDataProcessor
  {
  public:
    // using RawDataProcessor::RawDataProcessor;

    int getID() const { return id_; };
    std::string getName() const { return name_; };
    std::string getDescription() const { return description_; };

    /** Select features using the MRMFeatureSelection algorithm.
    */
    void process(
      RawDataHandler& rawDataHandler_IO,
      const std::map<std::string, std::vector<std::map<std::string, std::string>>>& params_I,
      const Filenames& filenames
    ) const override;

  protected:
    int id_ = 8;
    std::string name_ = "SELECT_FEATURES";
    std::string description_ = "Run the peak selection/alignment algorithm.";
  };

  class ValidateFeatures : public RawDataProcessor
  {
  public:
    // using RawDataProcessor::RawDataProcessor;

    int getID() const { return id_; };
    std::string getName() const { return name_; };
    std::string getDescription() const { return description_; };

    /** Validate the selected peaks against reference data.
    */
    void process(
      RawDataHandler& rawDataHandler_IO,
      const std::map<std::string, std::vector<std::map<std::string, std::string>>>& params_I,
      const Filenames& filenames
    ) const override;

  protected:
    int id_ = 9;
    std::string name_ = "VALIDATE_FEATURES";
    std::string description_ = "Compare selected features to a reference data set.";
  };

  class QuantifyFeatures : public RawDataProcessor
  {
  public:
    // using RawDataProcessor::RawDataProcessor;

    int getID() const { return id_; };
    std::string getName() const { return name_; };
    std::string getDescription() const { return description_; };

    /** Quantify all unknown samples based on the quantitationMethod.
    */
    void process(
      RawDataHandler& rawDataHandler_IO,
      const std::map<std::string, std::vector<std::map<std::string, std::string>>>& params_I,
      const Filenames& filenames
    ) const override;

  protected:
    int id_ = 10;
    std::string name_ = "QUANTIFY_FEATURES";
    std::string description_ = "Apply a calibration model defined in quantitationMethods to each transition.";
  };

  class PlotFeatures : public RawDataProcessor
  {
  public:
    // using RawDataProcessor::RawDataProcessor;

    int getID() const { return id_; };
    std::string getName() const { return name_; };
    std::string getDescription() const { return description_; };

    /** Validate the selected peaks against reference data.
    */
    void process(
      RawDataHandler& rawDataHandler_IO,
      const std::map<std::string, std::vector<std::map<std::string, std::string>>>& params_I,
      const Filenames& filenames
    ) const override;

  protected:
    int id_ = 11;
    std::string name_ = "PLOT_FEATURES";
    std::string description_ = "Plot the raw chromatogram with selected peaks overlaid.";
  };

  class LoadTransitions : public RawDataProcessor
  {
  public:
    // using RawDataProcessor::RawDataProcessor;

    int getID() const { return id_; };
    std::string getName() const { return name_; };
    std::string getDescription() const { return description_; };

    /** Load the transitions from the TraML file.
    */
    void process(
      RawDataHandler& rawDataHandler_IO,
      const std::map<std::string, std::vector<std::map<std::string, std::string>>>& params_I,
      const Filenames& filenames
    ) const override;

  protected:
    int id_ = 12;
    std::string name_ = "LOAD_TRANSITIONS";
    std::string description_ = "Load the transitions for the SRM experiments from the TraML file.";
  };

  class LoadFeatureFilters : public RawDataProcessor
  {
  public:
    // using RawDataProcessor::RawDataProcessor;

    int getID() const { return id_; };
    std::string getName() const { return name_; };
    std::string getDescription() const { return description_; };

    /** Load the component and component group transition filters from file.
    */
    void process(
      RawDataHandler& rawDataHandler_IO,
      const std::map<std::string, std::vector<std::map<std::string, std::string>>>& params_I,
      const Filenames& filenames
    ) const override;

  protected:
    int id_ = 13;
    std::string name_ = "LOAD_FEATURE_FILTERS";
    std::string description_ = "Load the component and component group transition filters from file.";
  };

  class LoadFeatureQCs : public RawDataProcessor
  {
  public:
    // using RawDataProcessor::RawDataProcessor;

    int getID() const { return id_; };
    std::string getName() const { return name_; };
    std::string getDescription() const { return description_; };

    /** Load the component and component group transition QCs from file.
    */
    void process(
      RawDataHandler& rawDataHandler_IO,
      const std::map<std::string, std::vector<std::map<std::string, std::string>>>& params_I,
      const Filenames& filenames
    ) const override;

  protected:
    int id_ = 14;
    std::string name_ = "LOAD_FEATURE_QCS";
    std::string description_ = "Load the component and component group transition QC specifications from file.";
  };

  class LoadValidationData : public RawDataProcessor
  {
  public:
    // using RawDataProcessor::RawDataProcessor;

    int getID() const { return id_; };
    std::string getName() const { return name_; };
    std::string getDescription() const { return description_; };

    /** Load the validation data from file.
    */
    void process(
      RawDataHandler& rawDataHandler_IO,
      const std::map<std::string, std::vector<std::map<std::string, std::string>>>& params_I,
      const Filenames& filenames
    ) const override;

  protected:
    int id_ = 15;
    std::string name_ = "LOAD_VALIDATION_DATA";
    std::string description_ = "Load the validation data from file.";
  };

  class LoadParameters : public RawDataProcessor
  {
  public:
    // using RawDataProcessor::RawDataProcessor;

    int getID() const { return id_; };
    std::string getName() const { return name_; };
    std::string getDescription() const { return description_; };

    /** Load the data processing parameters from file.
    */
    void process(
      RawDataHandler& rawDataHandler_IO,
      const std::map<std::string, std::vector<std::map<std::string, std::string>>>& params_I,
      const Filenames& filenames
    ) const override;
    static void sanitizeParameters(
      std::map<std::string, std::vector<std::map<std::string, std::string>>>& params_I
    );

  protected:
    int id_ = 16;
    std::string name_ = "LOAD_PARAMETERS";
    std::string description_ = "Load the data processing parameters from file.";
  };

  class MetaLoad : public RawDataProcessor
  {
  public:
    using RawDataProcessor::RawDataProcessor;

    int getID() const { return id_; };
    std::string getName() const { return name_; };
    std::string getDescription() const { return description_; };

    /** Load the data processing parameters from file.
    */
    void process(
      RawDataHandler& rawDataHandler_IO,
      const std::map<std::string, std::vector<std::map<std::string, std::string>>>& params_I,
      const Filenames& filenames
    ) const;
    static void sanitizeParameters(
      std::map<std::string, std::vector<std::map<std::string, std::string>>>& params_I
    );

  protected:
    int id_ = 16;
    std::string name_ = "LOAD_PARAMETERS";
    std::string description_ = "Load the data processing parameters from file.";
  };
}
