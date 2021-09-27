// --------------------------------------------------------------------------
//   SmartPeak -- Fast and Accurate CE-, GC- and LC-MS(/MS) Data Processing
// --------------------------------------------------------------------------
// Copyright The SmartPeak Team -- Novo Nordisk Foundation 
// Center for Biosustainability, Technical University of Denmark 2018-2021.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL ANY OF THE AUTHORS OR THE CONTRIBUTING
// INSTITUTIONS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
// OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
// WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
// OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
// ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// --------------------------------------------------------------------------
// $Maintainer: Douglas McCloskey $
// $Authors: Douglas McCloskey, Pasquale Domenico Colaianni, Svetlana Kutuzova, Ahmed Khalil $
// --------------------------------------------------------------------------

#pragma once

#include <SmartPeak/core/FeatureFiltersUtils.h>
#include <SmartPeak/core/Filenames.h>
#include <SmartPeak/core/MetaDataHandler.h>
#include <SmartPeak/core/RawDataHandler.h>
#include <SmartPeak/core/Parameters.h>
#include <SmartPeak/iface/IProcessorDescription.h>
#include <SmartPeak/core/WorkflowObservable.h>
#include <SmartPeak/core/ParametersObservable.h>
#include <SmartPeak/core/TransitionsObservable.h>
#include <SmartPeak/iface/IFilePickerHandler.h>
#include <SmartPeak/iface/IFilenamesHandler.h>

#include <map>
#include <vector>
#include <regex>
#include <sstream>

namespace SmartPeak
{
  struct RawDataProcessor : IProcessorDescription, IFilenamesHandler
  {
    RawDataProcessor(const RawDataProcessor& other) = delete;
    RawDataProcessor& operator=(const RawDataProcessor& other) = delete;
    virtual ~RawDataProcessor() = default;

    /* IProcessorDescription */
    virtual ParameterSet getParameterSchema() const override { return ParameterSet(); };
    virtual std::vector<std::string> getRequirements() const override { return {}; };

    /** Interface to all raw data processing methods.

      @param[in,out] rawDataHandler_IO Raw data file struct
      @param[in] params_I Dictionary of parameter names, values, descriptions, and tags
      @param[in] filenames Info about where data should be read from or written to
    */
    virtual void process(
      RawDataHandler& rawDataHandler_IO,
      const ParameterSet& params_I,
      Filenames& filenames_I
    ) const = 0;

    /* IFilenamesHandler */
    virtual void getFilenames(Filenames& filenames) const override { };

  protected:
    // Forced to write this, because the other user-defined constructors inhibit
    // the implicit definition of a default constructor
    // Even though this class is abstract and hence can't be instantiated,
    // derived classes will call the base's constructor
    RawDataProcessor() = default;
  };

  struct LoadRawData : RawDataProcessor
  {
    /* IProcessorDescription */
    virtual std::string getName() const override { return "LOAD_RAW_DATA"; }
    virtual std::string getDescription() const override { return "Read in raw data mzML file from disk."; }
    virtual ParameterSet getParameterSchema() const override;

    /** Read in raw data mzML file from disk.

      Depending upon user specifications, the mzML file will be mapped to the TraML file
      and/or various pre-processing methods will be run to extract out the data.
    */
    void process(
      RawDataHandler& rawDataHandler_IO,
      const ParameterSet& params_I,
      Filenames& filenames_I
    ) const override;

    /** Extracts metadata from the chromatogram.
    
      NOTE:
      - Not used in the current version of SmartPeak
      - Would need to be refactored or carefully implemented so as not to conflict
        with current injection, sample, and file naming patterns
    */
    static void extractMetaData(RawDataHandler& rawDataHandler_IO);

    /* IFilenamesHandler */
    virtual void getFilenames(Filenames& filenames) const override;
  };

  struct StoreRawData : RawDataProcessor
  {
    
    /* IProcessorDescription */
    virtual std::string getName() const override { return "STORE_RAW_DATA"; }
    virtual std::string getDescription() const override { return "Store the processed raw data mzML file to disk."; }

    /** Store the processed raw data mzML file to disk.
    */
    void process(
      RawDataHandler& rawDataHandler_IO,
      const ParameterSet& params_I,
      Filenames& filenames_I
    ) const override;

    /* IFilenamesHandler */
    virtual void getFilenames(Filenames& filenames) const override;
  };

  struct ZeroChromatogramBaseline : RawDataProcessor
  {
    /* IProcessorDescription */
    virtual std::string getName() const override { return "ZERO_CHROMATOGRAM_BASELINE"; }
    virtual std::string getDescription() const override { return "Normalize the lowest chromatogram intensity to zero."; }
    virtual std::vector<std::string> getRequirements() const override;

    /** Normalize the lowest chromatogram intensity to zero FOR MAPPED CHROMATOGRAMS.
    */
    void process(
      RawDataHandler& rawDataHandler_IO,
      const ParameterSet& params_I,
      Filenames& filenames_I
    ) const override;
  };

  struct MapChromatograms : RawDataProcessor
  {
    
    /* IProcessorDescription */
    virtual std::string getName() const override { return "MAP_CHROMATOGRAMS"; }
    virtual std::string getDescription() const override { return "Map chromatograms to the loaded set of transitions."; }
    virtual ParameterSet getParameterSchema() const override;
    virtual std::vector<std::string> getRequirements() const override;

    /** Map chromatograms to the loaded set of transitions.
    */
    void process(
      RawDataHandler& rawDataHandler_IO,
      const ParameterSet& params_I,
      Filenames& filenames_I
    ) const override;
  };

  struct ExtractChromatogramWindows : RawDataProcessor
  {
    /* IProcessorDescription */
    virtual std::string getName() const override { return "EXTRACT_CHROMATOGRAM_WINDOWS"; }
    virtual std::string getDescription() const override { return "Extract out specified chromatogram windows using the componentFeatureFilters."; }
    virtual std::vector<std::string> getRequirements() const override;

    /** Extract out specified chromatogram windows FROM A MAPPED CHROMATOGRAM using the componentFeatureFilters
    */
    void process(
      RawDataHandler& rawDataHandler_IO,
      const ParameterSet& params_I,
      Filenames& filenames_I
    ) const override;
  };

  struct ExtractSpectraWindows : RawDataProcessor
  {
    /* IProcessorDescription */
    virtual std::string getName() const override { return "EXTRACT_SPECTRA_WINDOWS"; }
    virtual std::string getDescription() const override { return "Extract out specified spectra windows based on the user parameters."; }
    virtual ParameterSet getParameterSchema() const override;
    virtual std::vector<std::string> getRequirements() const override;

    /** Extract out specified spectra windows from an MSExperiment using the range specified in the parameters
    */
    void process(
      RawDataHandler& rawDataHandler_IO,
      const ParameterSet& params_I,
      Filenames& filenames_I
    ) const override;
  };

  struct MergeSpectra : RawDataProcessor
  {
    /* IProcessorDescription */
    virtual std::string getName() const override { return "MERGE_SPECTRA"; }
    virtual std::string getDescription() const override { return "Merge all spectra along the time axis."; }
    virtual ParameterSet getParameterSchema() const override;
    virtual std::vector<std::string> getRequirements() const override;

    /** Merge all spectra along the time axis using a binning strategy that is resolution dependent
    */
    void process(
      RawDataHandler& rawDataHandler_IO,
      const ParameterSet& params_I,
      Filenames& filenames_I
    ) const override;
  };

  struct LoadFeatures : RawDataProcessor
  {
    /* IProcessorDescription */
    virtual std::string getName() const override { return "LOAD_FEATURES"; }
    virtual std::string getDescription() const override { return "Read in the features from disk."; }

    /** Read in the features from disk.
    */
    void process(
      RawDataHandler& rawDataHandler_IO,
      const ParameterSet& params_I,
      Filenames& filenames_I
    ) const override;

    /* IFilenamesHandler */
    virtual void getFilenames(Filenames& filenames) const override;
  };

  struct StoreFeatures : RawDataProcessor
  {
    /* IProcessorDescription */
    virtual std::string getName() const override { return "STORE_FEATURES"; }
    virtual std::string getDescription() const override { return "Write the features to disk."; }
    virtual std::vector<std::string> getRequirements() const override;

    /** Write the features to disk.
    */
    void process(
      RawDataHandler& rawDataHandler_IO,
      const ParameterSet& params_I,
      Filenames& filenames_I
    ) const override;

    /* IFilenamesHandler */
    virtual void getFilenames(Filenames& filenames) const override;
  };

  struct LoadAnnotations : RawDataProcessor
  {
    /* IProcessorDescription */
    virtual std::string getName() const override { return "LOAD_ANNOTATIONS"; }
    virtual std::string getDescription() const override { return "Read in the annotations from disk."; }

    /** Read in the annotations from disk.
    */
    void process(
      RawDataHandler& rawDataHandler_IO,
      const ParameterSet& params_I,
      Filenames& filenames_I
    ) const override;

    /* IFilenamesHandler */
    virtual void getFilenames(Filenames& filenames) const override;
  };

  struct StoreAnnotations : RawDataProcessor
  {
    /* IProcessorDescription */
    virtual std::string getName() const override { return "STORE_ANNOTATIONS"; }
    virtual std::string getDescription() const override { return "Write the annotations to disk."; }
    virtual std::vector<std::string> getRequirements() const override;

    /** Write the annotations to disk.
    */
    void process(
      RawDataHandler& rawDataHandler_IO,
      const ParameterSet& params_I,
      Filenames& filenames_I
    ) const override;

    /* IFilenamesHandler */
    virtual void getFilenames(Filenames& filenames) const override;
  };

  struct PickMRMFeatures : RawDataProcessor
  {
    /* IProcessorDescription */
    virtual std::string getName() const override { return "PICK_MRM_FEATURES"; }
    virtual std::string getDescription() const override { return "Run the peak picking algorithm for SRMs/MRMs."; }
    virtual ParameterSet getParameterSchema() const override;
    virtual std::vector<std::string> getRequirements() const override;

    /** Run the openSWATH pick peaking and scoring workflow for a single raw data file.
    */
    void process(
      RawDataHandler& rawDataHandler_IO,
      const ParameterSet& params_I,
      Filenames& filenames_I
    ) const override;
  };

  struct PickMS1Features : RawDataProcessor
  {
    /* IProcessorDescription */
    virtual std::string getName() const override { return "PICK_MS1_FEATURES"; }
    virtual std::string getDescription() const override { return "Run the peak picking algorithm for MS1 spectra."; }
    virtual ParameterSet getParameterSchema() const override;
    virtual std::vector<std::string> getRequirements() const override;

    /** Run the MS1 peak picking and scoring algorithm.
    */
    void process(
      RawDataHandler& rawDataHandler_IO,
      const ParameterSet& params_I,
      Filenames& filenames_I
    ) const override;
  };

  struct PickMS2Features : RawDataProcessor
  {
    /* IProcessorDescription */
    virtual std::string getName() const override { return "PICK_MS2_FEATURES"; }
    virtual std::string getDescription() const override { return "Pick MS2 Features"; }
    virtual ParameterSet getParameterSchema() const override;
    virtual std::vector<std::string> getRequirements() const override;

    /** PickMS2Features
     */
    void process(
      RawDataHandler& rawDataHandler_IO,
      const ParameterSet& params_I,
      Filenames& filenames_I
    ) const override;
  };

  struct SearchAccurateMass : RawDataProcessor
  {
    /* IProcessorDescription */
    virtual std::string getName() const override { return "SEARCH_ACCURATE_MASS"; }
    virtual std::string getDescription() const override { return "Run the accurate mass search algorithm."; }
    virtual ParameterSet getParameterSchema() const override;
    virtual std::vector<std::string> getRequirements() const override;

    /** Run the accurate mass search algorithm.
    */
    void process(
      RawDataHandler& rawDataHandler_IO,
      const ParameterSet& params_I,
      Filenames& filenames_I
    ) const override;
  };

  struct MergeFeatures : RawDataProcessor
  {
    /* IProcessorDescription */
    virtual std::string getName() const override { return "MERGE_FEATURES"; }
    virtual std::string getDescription() const override { return "Create merged features from accurate mass search results."; }
    virtual std::vector<std::string> getRequirements() const override;

    /** Create merged features from accurate mass search results.
    */
    void process(
      RawDataHandler& rawDataHandler_IO,
      const ParameterSet& params_I,
      Filenames& filenames_I
    ) const override;
  };

  struct SearchSpectrum : RawDataProcessor
  {
    /* IProcessorDescription */
    virtual std::string getName() const override { return "SEARCH_SPECTRUM"; }
    virtual std::string getDescription() const override { return "Search accurate masses and add identification (peptide hits) as features/sub-features"; }
    virtual ParameterSet getParameterSchema() const override;
    virtual std::vector<std::string> getRequirements() const override;

    /** Create merged features from accurate mass search results.
    */
    void process(
      RawDataHandler& rawDataHandler_IO,
      const ParameterSet& params_I,
      Filenames& filenames_I
    ) const override;
  };

  struct DDA : RawDataProcessor
  {
    /* IProcessorDescription */
    virtual std::string getName() const override { return "DDA"; }
    virtual std::string getDescription() const override { return "Data-Dependent Acquisition workflow step"; }
    virtual ParameterSet getParameterSchema() const override;
    virtual std::vector<std::string> getRequirements() const override;

    /** Create merged features from accurate mass search results.
    */
    void process(
      RawDataHandler& rawDataHandler_IO,
      const ParameterSet& params_I,
      Filenames& filenames_I
    ) const override;

    /* IFilenamesHandler */
    virtual void getFilenames(Filenames& filenames) const override;
  };

  struct FilterFeatures : RawDataProcessor
  {
    /* IProcessorDescription */
    virtual std::string getName() const override { return "FILTER_FEATURES"; }
    virtual std::string getDescription() const override { return "Filter transitions and transitions groups based on a user defined criteria."; }
    virtual ParameterSet getParameterSchema() const override;
    virtual std::vector<std::string> getRequirements() const override;

    /** Filter features that do not pass the filter QCs.
    */
    void process(
      RawDataHandler& rawDataHandler_IO,
      const ParameterSet& params_I,
      Filenames& filenames_I
    ) const override;
  };

  struct CheckFeatures : RawDataProcessor
  {
    /* IProcessorDescription */
    virtual std::string getName() const override { return "CHECK_FEATURES"; }
    virtual std::string getDescription() const override { return "Flag and score transitions and transition groups based on a user defined criteria."; }
    virtual ParameterSet getParameterSchema() const override;
    virtual std::vector<std::string> getRequirements() const override;

    /** Flag features that do not pass the filter QCs.
    */
    void process(
      RawDataHandler& rawDataHandler_IO,
      const ParameterSet& params_I,
      Filenames& filenames_I
    ) const override;
  };

  struct SelectFeatures : RawDataProcessor
  {
    /* IProcessorDescription */
    virtual std::string getName() const override { return "SELECT_FEATURES"; }
    virtual std::string getDescription() const override { return "Run the peak selection/alignment algorithm."; }
    virtual std::vector<std::string> getRequirements() const override;

    /** Select features using the MRMFeatureSelection algorithm.
    */
    void process(
      RawDataHandler& rawDataHandler_IO,
      const ParameterSet& params_I,
      Filenames& filenames_I
    ) const override;
  };

  struct ValidateFeatures : RawDataProcessor
  {
    /* IProcessorDescription */
    virtual std::string getName() const override { return "VALIDATE_FEATURES"; }
    virtual std::string getDescription() const override { return "Compare selected features to a reference data set."; }
    virtual ParameterSet getParameterSchema() const override;
    virtual std::vector<std::string> getRequirements() const override;

    /** Validate the selected peaks against reference data.
    */
    void process(
      RawDataHandler& rawDataHandler_IO,
      const ParameterSet& params_I,
      Filenames& filenames_I
    ) const override;
  };

  struct QuantifyFeatures : RawDataProcessor
  {
    /* IProcessorDescription */
    virtual std::string getName() const override { return "QUANTIFY_FEATURES"; }
    virtual std::string getDescription() const override { return "Apply a calibration model defined in quantitationMethods to each transition."; }
    virtual ParameterSet getParameterSchema() const override;
    virtual std::vector<std::string> getRequirements() const override;

    /** Quantify all unknown samples based on the quantitationMethod.
    */
    void process(
      RawDataHandler& rawDataHandler_IO,
      const ParameterSet& params_I,
      Filenames& filenames_I
    ) const override;
  };

  struct PlotFeatures : RawDataProcessor
  {
    /* IProcessorDescription */
    virtual std::string getName() const override { return "PLOT_FEATURES"; }
    virtual std::string getDescription() const override { return "Plot the raw chromatogram with selected peaks overlaid."; }
    virtual std::vector<std::string> getRequirements() const override;

    /** Validate the selected peaks against reference data.
    */
    void process(
      RawDataHandler& rawDataHandler_IO,
      const ParameterSet& params_I,
      Filenames& filenames_I
    ) const override;
  };

  struct LoadTransitions : RawDataProcessor, IFilePickerHandler
  {
    /**
    IFilePickerHandler
    */
    bool onFilePicked(const std::filesystem::path& filename, ApplicationHandler* application_handler) override;

    /* IProcessorDescription */
    virtual std::string getName() const override { return "LOAD_TRANSITIONS"; }
    virtual std::string getDescription() const override { return "Load the transitions for the SRM experiments from the TraML file."; }
    virtual ParameterSet getParameterSchema() const override;

    /** Load the transitions from the TraML file.
    */
    void process(
      RawDataHandler& rawDataHandler_IO,
      const ParameterSet& params_I,
      Filenames& filenames_I
    ) const override;
    
    /* IFilenamesHandler */
    virtual void getFilenames(Filenames& filenames) const override;

    TransitionsObservable* transitions_observable_ = nullptr;
  };

  struct LoadFeatureFiltersRDP : RawDataProcessor
  {
    /* IProcessorDescription */
    virtual std::string getName() const override { return "LOAD_FEATURE_FILTERS"; }
    virtual std::string getDescription() const override { return "Load the component and component group transition filters from file."; }

    /** Load the component and component group transition filters from file.
    */
    void process(
      RawDataHandler& rawDataHandler_IO,
      const ParameterSet& params_I,
      Filenames& filenames_I
    ) const override;

    /* IFilenamesHandler */
    virtual void getFilenames(Filenames& filenames) const override;
  };

  struct LoadFeatureQCsRDP : RawDataProcessor
  {
    /* IProcessorDescription */
    virtual std::string getName() const override { return "LOAD_FEATURE_QCS"; }
    virtual std::string getDescription() const override { return "Load the component and component group transition QC specifications from file."; }

    /** Load the component and component group transition QCs from file.
    */
    void process(
      RawDataHandler& rawDataHandler_IO,
      const ParameterSet& params_I,
      Filenames& filenames_I
    ) const override;

    /* IFilenamesHandler */
    virtual void getFilenames(Filenames& filenames) const override;
  };

  struct StoreFeatureFiltersRDP : RawDataProcessor, IFilePickerHandler
  {
    StoreFeatureFiltersRDP(int feature_filter_mode = FeatureFiltersUtils::EHandleAll, bool static_filenames = false)
      : feature_filter_mode_(feature_filter_mode), static_filenames_(static_filenames) {}
    int feature_filter_mode_;
    bool static_filenames_;

    /* IProcessorDescription */
    virtual std::string getName() const override { return "STORE_FEATURE_FILTERS"; }
    virtual std::string getDescription() const override { return "Store the component and component group transition filters from file."; }
    virtual std::vector<std::string> getRequirements() const override;

    /** Store the component and component group transition filters from file.
    */
    void process(
      RawDataHandler& rawDataHandler_IO,
      const ParameterSet& params_I,
      Filenames& filenames_I
    ) const override;

    /**
    IFilePickerHandler
    */
    bool onFilePicked(const std::filesystem::path& filename, ApplicationHandler* application_handler) override;

    /* IFilenamesHandler */
    virtual void getFilenames(Filenames& filenames) const override;
  };

  struct StoreFeatureQCsRDP : RawDataProcessor, IFilePickerHandler
  {
    StoreFeatureQCsRDP(int feature_filter_mode = FeatureFiltersUtils::EHandleAll, bool static_filenames = false)
      : feature_filter_mode_(feature_filter_mode), static_filenames_(static_filenames) {}
    int feature_filter_mode_;
    bool static_filenames_;

    /* IProcessorDescription */
    virtual std::string getName() const override { return "STORE_FEATURE_QCS"; }
    virtual std::string getDescription() const override { return "Store the component and component group transition QC specifications from file."; }
    virtual std::vector<std::string> getRequirements() const override;

    /** Store the component and component group transition QCs from file.
    */
    void process(
      RawDataHandler& rawDataHandler_IO,
      const ParameterSet& params_I,
      Filenames& filenames_I
    ) const override;

    /**
    IFilePickerHandler
    */
    bool onFilePicked(const std::filesystem::path& filename, ApplicationHandler* application_handler) override;

    /* IFilenamesHandler */
    virtual void getFilenames(Filenames& filenames) const override;
  };

  struct LoadValidationData : RawDataProcessor, IFilePickerHandler
  {
    /**
    IFilePickerHandler
    */
    bool onFilePicked(const std::filesystem::path& filename, ApplicationHandler* application_handler) override;

    /* IProcessorDescription */
    virtual std::string getName() const override { return "LOAD_VALIDATION_DATA"; }
    virtual std::string getDescription() const override { return "Load the validation data from file."; }

    /** Load the validation data from file.
    */
    void process(
      RawDataHandler& rawDataHandler_IO,
      const ParameterSet& params_I,
      Filenames& filenames_I
    ) const override;

    /* IFilenamesHandler */
    virtual void getFilenames(Filenames& filenames) const override;
  };

  struct StoreValidationData : RawDataProcessor, IFilePickerHandler
  {
    /**
    IFilePickerHandler
    */
    bool onFilePicked(const std::filesystem::path& filename, ApplicationHandler* application_handler) override;

    StoreValidationData() = default;
    void process(
      RawDataHandler& rawDataHandler_IO,
      const ParameterSet& params_I,
      Filenames& filenames_I
    ) const override;
    std::string filename_;

    /* IProcessorDescription */
    virtual std::string getName() const override { return "STORE_VALIDATION_DATA"; }
    virtual std::string getDescription() const override { return "Store the validation data."; }
    virtual std::vector<std::string> getRequirements() const override;

    /* IFilenamesHandler */
    virtual void getFilenames(Filenames& filenames) const override;
  };

  struct LoadParameters : RawDataProcessor, IFilePickerHandler
  {
    /**
    IFilePickerHandler
    */
    bool onFilePicked(const std::filesystem::path& filename, ApplicationHandler* application_handler) override;

    /* IProcessorDescription */
    virtual std::string getName() const override { return "LOAD_PARAMETERS"; }
    virtual std::string getDescription() const override { return "Load the data processing parameters from file."; }

    /** Load the data processing parameters from file.
    */
    void process(
      RawDataHandler& rawDataHandler_IO,
      const ParameterSet& params_I,
      Filenames& filenames_I
    ) const override;
    static void sanitizeParameters(
      ParameterSet& params_I
    );

    /* IFilenamesHandler */
    virtual void getFilenames(Filenames& filenames) const override;

    ParametersObservable* parameters_observable_ = nullptr;
  };

  struct StoreParameters : RawDataProcessor, IFilePickerHandler
  {
    /**
    IFilePickerHandler
    */
    bool onFilePicked(const std::filesystem::path& filename, ApplicationHandler* application_handler) override;

    StoreParameters() = default;
    void process(
      RawDataHandler& rawDataHandler_IO,
      const ParameterSet& params_I,
      Filenames& filenames_I
    ) const override;
    std::string filename_;

    /* IProcessorDescription */
    virtual std::string getName() const override { return "STORE_PARAMETERS"; }
    virtual std::string getDescription() const override { return "Store a parameters to file"; }
    virtual std::vector<std::string> getRequirements() const override;

    /* IFilenamesHandler */
    virtual void getFilenames(Filenames& filenames) const override;
  };

  struct FitFeaturesEMG : RawDataProcessor
  {
    /* IProcessorDescription */
    virtual std::string getName() const override { return "FIT_FEATURES_EMG"; }
    virtual std::string getDescription() const override { return "Reconstruct a peak from available data points."; }
    virtual ParameterSet getParameterSchema() const override;
    virtual std::vector<std::string> getRequirements() const override;

    /** Apply the EMG peak reconstruction technique to the data points.
    */
    void process(
      RawDataHandler& rawDataHandler_IO,
      const ParameterSet& params_I,
      Filenames& filenames_I
    ) const override;

  private:
    void extractPointsIntoVectors(
      const OpenMS::MSChromatogram& chromatogram,
      const double left,
      const double right,
      std::vector<double>& x,
      std::vector<double>& y
    ) const;
  };

  struct FilterFeaturesRSDs : RawDataProcessor
  {
    /* IProcessorDescription */
    virtual std::string getName() const override { return "FILTER_FEATURES_RSDS"; }
    virtual std::string getDescription() const override { return "Filter transitions and transitions groups based on a user defined criteria."; }
    virtual ParameterSet getParameterSchema() const override;
    virtual std::vector<std::string> getRequirements() const override;

    /** Filter features that do not pass the filter QCs.
    */
    void process(
      RawDataHandler& rawDataHandler_IO,
      const ParameterSet& params_I,
      Filenames& filenames_I
    ) const override;
  };

  struct CheckFeaturesRSDs : RawDataProcessor
  {
    /* IProcessorDescription */
    virtual std::string getName() const override { return "CHECK_FEATURES_RSDS"; }
    virtual std::string getDescription() const override { return "Flag and score transitions and transition groups based on a user defined criteria."; }
    virtual ParameterSet getParameterSchema() const override;
    virtual std::vector<std::string> getRequirements() const override;

    /** Flag features that do not pass the filter QCs.
    */
    void process(
      RawDataHandler& rawDataHandler_IO,
      const ParameterSet& params_I,
      Filenames& filenames_I
    ) const override;
  };

  struct FilterFeaturesBackgroundInterferences : RawDataProcessor
  {
    /* IProcessorDescription */
    virtual std::string getName() const override { return "FILTER_FEATURES_BACKGROUND_INTERFERENCES"; }
    virtual std::string getDescription() const override { return "Filter transitions and transitions groups based on a user defined criteria."; }
    virtual ParameterSet getParameterSchema() const override;
    virtual std::vector<std::string> getRequirements() const override;

    /** Filter features that do not pass the filter QCs.
    */
    void process(
      RawDataHandler& rawDataHandler_IO,
      const ParameterSet& params_I,
      Filenames& filenames_I
    ) const override;
  };

  struct CheckFeaturesBackgroundInterferences : RawDataProcessor
  {
    /* IProcessorDescription */
    virtual std::string getName() const override { return "CHECK_FEATURES_BACKGROUND_INTERFERENCES"; }
    virtual std::string getDescription() const override { return "Flag and score transitions and transition groups based on a user defined criteria."; }
    virtual ParameterSet getParameterSchema() const override;
    virtual std::vector<std::string> getRequirements() const override;

    /** Flag features that do not pass the filter QCs.
    */
    void process(
      RawDataHandler& rawDataHandler_IO,
      const ParameterSet& params_I,
      Filenames& filenames_I
    ) const override;
  };

  struct ClearData : RawDataProcessor
  {
    /* IProcessorDescription */
    virtual std::string getName() const override { return "CLEAR_DATA"; }
    virtual std::string getDescription() const override { return "Clear raw and processed data."; }

    /** Clear all raw and process data
    */
    void process(
      RawDataHandler& rawDataHandler_IO,
      const ParameterSet& params_I,
      Filenames& filenames_I
    ) const override;
  };
  
  struct CalculateMDVs : RawDataProcessor
  {
    /* IProcessorDescription */
    virtual std::string getName() const override { return "CALCULATE_MDVS"; }
    virtual std::string getDescription() const override { return "Calculate MDVs."; }
    virtual ParameterSet getParameterSchema() const override;
    virtual std::vector<std::string> getRequirements() const override;

    /** Calculate MDVs
     */
    void process(
                 RawDataHandler& rawDataHandler_IO,
                 const ParameterSet& params_I,
                 Filenames& filenames_I
                 ) const override;
  };
  
  struct IsotopicCorrections : RawDataProcessor
  {
    /* IProcessorDescription */
    virtual std::string getName() const override { return "ISOTOPIC_CORRECTIONS"; }
    virtual std::string getDescription() const override { return "Perform Isotopic Corrections."; }
    virtual ParameterSet getParameterSchema() const override;
    virtual std::vector<std::string> getRequirements() const override;

    /** Correct MDVs
     */
    void process(
                 RawDataHandler& rawDataHandler_IO,
                 const ParameterSet& params_I,
                 Filenames& filenames_I
                 ) const override;
  };
  
  struct CalculateIsotopicPurities : RawDataProcessor
  {
    /* IProcessorDescription */
    virtual std::string getName() const override { return "CALCULATE_MDV_ISOTOPIC_PURITIES"; }
    virtual std::string getDescription() const override { return "Calculate MDV Isotopic Purities."; }
    virtual ParameterSet getParameterSchema() const override;
    virtual std::vector<std::string> getRequirements() const override;

    /** Calculate MDV Isotopic Purities
     */
    void process(
                 RawDataHandler& rawDataHandler_IO,
                 const ParameterSet& params_I,
                 Filenames& filenames_I
                 ) const override;
  };

  struct CalculateMDVAccuracies : RawDataProcessor
  {
    /* IProcessorDescription */
    virtual std::string getName() const override { return "CALCULATE_MDV_ACCURACIES"; }
    virtual std::string getDescription() const override { return "Compare MDVs to Theoretical"; }
    virtual ParameterSet getParameterSchema() const override;
    virtual std::vector<std::string> getRequirements() const override;

    /** Compare MDVs to Theoretical
     */
    void process(
                 RawDataHandler& rawDataHandler_IO,
                 const ParameterSet& params_I,
                 Filenames& filenames_I
                 ) const override;
  };

}
