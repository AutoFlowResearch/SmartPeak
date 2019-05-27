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
  struct RawDataProcessor
  {
    // C.67: A polymorphic class should suppress copying
    RawDataProcessor(const RawDataProcessor& other) = delete;
    RawDataProcessor& operator=(const RawDataProcessor& other) = delete;

    // C.35: A base class destructor should be either public and virtual, or protected and nonvirtual
    // C.127: A class with a virtual function should have a virtual or protected destructor
    virtual ~RawDataProcessor() = default;

    virtual int getID() const = 0; /// get the raw data processor struct ID
    virtual std::string getName() const = 0; /// get the raw data processor struct name
    virtual std::string getDescription() const = 0; /// get the raw data processor struct description

    /** Interface to all raw data processing methods.

      @param[in,out] rawDataHandler_IO Raw data file struct
      @param[in] params_I Dictionary of parameter names, values, descriptions, and tags
      @param[in] filenames Info about where data should be read from or written to
    */
    virtual void process(
      RawDataHandler& rawDataHandler_IO,
      const std::map<std::string, std::vector<std::map<std::string, std::string>>>& params_I,
      const Filenames& filenames
    ) const = 0;

  protected:
    // Forced to write this, because the other user-defined constructors inhibit
    // the implicit definition of a default constructor
    // Even though this class is abstract and hence can't be instantiated,
    // derived classes will call the base's constructor
    RawDataProcessor() = default;
  };

  struct LoadRawData : RawDataProcessor
  {
    int getID() const override { return 1; }
    std::string getName() const override { return "LOAD_RAW_DATA"; }
    std::string getDescription() const override { return "Read in raw data mzML file from disk."; }

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
    static void extractMetaData(RawDataHandler& rawDataHandler_IO);
  };

  struct StoreRawData : RawDataProcessor
  {
    int getID() const override { return -1; }
    std::string getName() const override { return "STORE_RAW_DATA"; }
    std::string getDescription() const override { return "Store the processed raw data mzML file to disk."; }

    /** Store the processed raw data mzML file to disk.
    */
    void process(
      RawDataHandler& rawDataHandler_IO,
      const std::map<std::string, std::vector<std::map<std::string, std::string>>>& params_I,
      const Filenames& filenames
    ) const override;
  };

  struct ZeroChromatogramBaseline : RawDataProcessor
  {
    int getID() const override { return 12; }
    std::string getName() const override { return "ZERO_CHROMATOGRAM_BASELINE"; }
    std::string getDescription() const override { return "Normalize the lowest chromatogram intensity to zero."; }

    /** Normalize the lowest chromatogram intensity to zero FOR MAPPED CHROMATOGRAMS.
    */
    void process(
      RawDataHandler& rawDataHandler_IO,
      const std::map<std::string, std::vector<std::map<std::string, std::string>>>& params_I,
      const Filenames& filenames
    ) const override;
  };

  struct MapChromatograms : RawDataProcessor
  {
    int getID() const override { return 11; }
    std::string getName() const override { return "MAP_CHROMATOGRAMS"; }
    std::string getDescription() const override { return "Map chromatograms to the loaded set of transitions."; }

    /** Map chromatograms to the loaded set of transitions.
    */
    void process(
      RawDataHandler& rawDataHandler_IO,
      const std::map<std::string, std::vector<std::map<std::string, std::string>>>& params_I,
      const Filenames& filenames
    ) const override;
  };

  struct ExtractChromatogramWindows : RawDataProcessor
  {
    int getID() const override { return 13; }
    std::string getName() const override { return "EXTRACT_CHROMATOGRAM_WINDOWS"; }
    std::string getDescription() const override { return "Extract out specified chromatogram windows using the componentFeatureFilters."; }

    /** Extract out specified chromatogram windows FROM A MAPPED CHROMATOGRAM using the componentFeatureFilters
    */
    void process(
      RawDataHandler& rawDataHandler_IO,
      const std::map<std::string, std::vector<std::map<std::string, std::string>>>& params_I,
      const Filenames& filenames
    ) const override;
  };

  struct LoadFeatures : RawDataProcessor
  {
    int getID() const override { return 2; }
    std::string getName() const override { return "LOAD_FEATURES"; }
    std::string getDescription() const override { return "Read in the features from disk."; }

    /** Read in the features from disk.
    */
    void process(
      RawDataHandler& rawDataHandler_IO,
      const std::map<std::string, std::vector<std::map<std::string, std::string>>>& params_I,
      const Filenames& filenames
    ) const override;
  };

  struct StoreFeatures : RawDataProcessor
  {
    int getID() const override { return 9; }
    std::string getName() const override { return "STORE_FEATURES"; }
    std::string getDescription() const override { return "Write the features to disk."; }

    /** Write the features to disk.
    */
    void process(
      RawDataHandler& rawDataHandler_IO,
      const std::map<std::string, std::vector<std::map<std::string, std::string>>>& params_I,
      const Filenames& filenames
    ) const override;
  };

  struct PickFeatures : RawDataProcessor
  {
    int getID() const override { return 3; }
    std::string getName() const override { return "PICK_FEATURES"; }
    std::string getDescription() const override { return "Run the peak picking algorithm."; }

    /** Run the openSWATH pick peaking and scoring workflow for a single raw data file.
    */
    void process(
      RawDataHandler& rawDataHandler_IO,
      const std::map<std::string, std::vector<std::map<std::string, std::string>>>& params_I,
      const Filenames& filenames
    ) const override;
  };

  struct FilterFeatures : RawDataProcessor
  {
    int getID() const override { return 4; }
    std::string getName() const override { return "FILTER_FEATURES"; }
    std::string getDescription() const override { return "Filter transitions and transitions groups based on a user defined criteria."; }

    /** Filter features that do not pass the filter QCs.
    */
    void process(
      RawDataHandler& rawDataHandler_IO,
      const std::map<std::string, std::vector<std::map<std::string, std::string>>>& params_I,
      const Filenames& filenames
    ) const override;
  };

  struct CheckFeatures : RawDataProcessor
  {
    int getID() const override { return 8; }
    std::string getName() const override { return "CHECK_FEATURES"; }
    std::string getDescription() const override { return "Flag and score transitions and transition groups based on a user defined criteria."; }

    /** Flag features that do not pass the filter QCs.
    */
    void process(
      RawDataHandler& rawDataHandler_IO,
      const std::map<std::string, std::vector<std::map<std::string, std::string>>>& params_I,
      const Filenames& filenames
    ) const override;
  };

  struct SelectFeatures : RawDataProcessor
  {
    int getID() const override { return 5; }
    std::string getName() const override { return "SELECT_FEATURES"; }
    std::string getDescription() const override { return "Run the peak selection/alignment algorithm."; }

    /** Select features using the MRMFeatureSelection algorithm.
    */
    void process(
      RawDataHandler& rawDataHandler_IO,
      const std::map<std::string, std::vector<std::map<std::string, std::string>>>& params_I,
      const Filenames& filenames
    ) const override;
  };

  struct ValidateFeatures : RawDataProcessor
  {
    int getID() const override { return 6; }
    std::string getName() const override { return "VALIDATE_FEATURES"; }
    std::string getDescription() const override { return "Compare selected features to a reference data set."; }

    /** Validate the selected peaks against reference data.
    */
    void process(
      RawDataHandler& rawDataHandler_IO,
      const std::map<std::string, std::vector<std::map<std::string, std::string>>>& params_I,
      const Filenames& filenames
    ) const override;
  };

  struct QuantifyFeatures : RawDataProcessor
  {
    int getID() const override { return 7; }
    std::string getName() const override { return "QUANTIFY_FEATURES"; }
    std::string getDescription() const override { return "Apply a calibration model defined in quantitationMethods to each transition."; }

    /** Quantify all unknown samples based on the quantitationMethod.
    */
    void process(
      RawDataHandler& rawDataHandler_IO,
      const std::map<std::string, std::vector<std::map<std::string, std::string>>>& params_I,
      const Filenames& filenames
    ) const override;
  };

  struct PlotFeatures : RawDataProcessor
  {
    int getID() const override { return 10; }
    std::string getName() const override { return "PLOT_FEATURES"; }
    std::string getDescription() const override { return "Plot the raw chromatogram with selected peaks overlaid."; }

    /** Validate the selected peaks against reference data.
    */
    void process(
      RawDataHandler& rawDataHandler_IO,
      const std::map<std::string, std::vector<std::map<std::string, std::string>>>& params_I,
      const Filenames& filenames
    ) const override;
  };

  struct LoadTransitions : RawDataProcessor
  {
    int getID() const override { return -1; }
    std::string getName() const override { return "LOAD_TRANSITIONS"; }
    std::string getDescription() const override { return "Load the transitions for the SRM experiments from the TraML file."; }

    /** Load the transitions from the TraML file.
    */
    void process(
      RawDataHandler& rawDataHandler_IO,
      const std::map<std::string, std::vector<std::map<std::string, std::string>>>& params_I,
      const Filenames& filenames
    ) const override;
  };

  struct LoadFeatureFilters : RawDataProcessor
  {
    int getID() const override { return -1; }
    std::string getName() const override { return "LOAD_FEATURE_FILTERS"; }
    std::string getDescription() const override { return "Load the component and component group transition filters from file."; }

    /** Load the component and component group transition filters from file.
    */
    void process(
      RawDataHandler& rawDataHandler_IO,
      const std::map<std::string, std::vector<std::map<std::string, std::string>>>& params_I,
      const Filenames& filenames
    ) const override;
  };

  struct LoadFeatureQCs : RawDataProcessor
  {
    int getID() const override { return -1; }
    std::string getName() const override { return "LOAD_FEATURE_QCS"; }
    std::string getDescription() const override { return "Load the component and component group transition QC specifications from file."; }

    /** Load the component and component group transition QCs from file.
    */
    void process(
      RawDataHandler& rawDataHandler_IO,
      const std::map<std::string, std::vector<std::map<std::string, std::string>>>& params_I,
      const Filenames& filenames
    ) const override;
  };

  struct LoadValidationData : RawDataProcessor
  {
    int getID() const override { return -1; }
    std::string getName() const override { return "LOAD_VALIDATION_DATA"; }
    std::string getDescription() const override { return "Load the validation data from file."; }

    /** Load the validation data from file.
    */
    void process(
      RawDataHandler& rawDataHandler_IO,
      const std::map<std::string, std::vector<std::map<std::string, std::string>>>& params_I,
      const Filenames& filenames
    ) const override;
  };

  struct LoadParameters : RawDataProcessor
  {
    int getID() const override { return -1; }
    std::string getName() const override { return "LOAD_PARAMETERS"; }
    std::string getDescription() const override { return "Load the data processing parameters from file."; }

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
  };
}
