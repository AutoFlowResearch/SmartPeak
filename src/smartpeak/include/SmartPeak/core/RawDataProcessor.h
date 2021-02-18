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

#include <OpenMS/KERNEL/FeatureMap.h>
#include <OpenMS/ANALYSIS/TARGETED/TargetedExperiment.h>
#include <OpenMS/KERNEL/MSExperiment.h>
#include <OpenMS/ANALYSIS/OPENSWATH/MRMFeatureQC.h>
#include <OpenMS/ANALYSIS/QUANTITATION/AbsoluteQuantitationMethod.h>
#include <OpenMS/ANALYSIS/QUANTITATION/IsotopeLabelingMDVs.h>
#include <OpenMS/ANALYSIS/MAPMATCHING/TransformationDescription.h>
#include <OpenMS/DATASTRUCTURES/DefaultParamHandler.h>

#include <SmartPeak/core/Filenames.h>
#include <SmartPeak/core/MetaDataHandler.h>
#include <SmartPeak/core/RawDataHandler.h>
#include <SmartPeak/core/Parameters.h>
#include <SmartPeak/iface/IProcessorDescription.h>

#include <map>
#include <vector>
#include <regex>
#include <sstream>

namespace SmartPeak
{
  struct RawDataProcessor : IProcessorDescription
  {
    RawDataProcessor(const RawDataProcessor& other) = delete;
    RawDataProcessor& operator=(const RawDataProcessor& other) = delete;
    virtual ~RawDataProcessor() = default;

    /** Interface to all raw data processing methods.

      @param[in,out] rawDataHandler_IO Raw data file struct
      @param[in] params_I Dictionary of parameter names, values, descriptions, and tags
      @param[in] filenames Info about where data should be read from or written to
    */
    virtual void process(
      RawDataHandler& rawDataHandler_IO,
      const ParameterSet& params_I,
      const Filenames& filenames
    ) const = 0;

    virtual ParameterSet getParameterSchema() const;

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
      const ParameterSet& params_I,
      const Filenames& filenames
    ) const override;

    /** Extracts metadata from the chromatogram.
    
      NOTE:
      - Not used in the current version of SmartPeak
      - Would need to be refactored or carefully implemented so as not to conflict
        with current injection, sample, and file naming patterns
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
      const ParameterSet& params_I,
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
      const ParameterSet& params_I,
      const Filenames& filenames
    ) const override;
  };

  struct MapChromatograms : RawDataProcessor
  {
    int getID() const override { return 11; }
    std::string getName() const override { return "MAP_CHROMATOGRAMS"; }
    std::string getDescription() const override { return "Map chromatograms to the loaded set of transitions."; }

    virtual ParameterSet getParameterSchema() const override;

    /** Map chromatograms to the loaded set of transitions.
    */
    void process(
      RawDataHandler& rawDataHandler_IO,
      const ParameterSet& params_I,
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
      const ParameterSet& params_I,
      const Filenames& filenames
    ) const override;
  };

  struct ExtractSpectraWindows : RawDataProcessor
  {
    int getID() const override { return -1; }
    std::string getName() const override { return "EXTRACT_SPECTRA_WINDOWS"; }
    std::string getDescription() const override { return "Extract out specified spectra windows based on the user parameters."; }

    virtual ParameterSet getParameterSchema() const override;

    /** Extract out specified spectra windows from an MSExperiment using the range specified in the parameters
    */
    void process(
      RawDataHandler& rawDataHandler_IO,
      const ParameterSet& params_I,
      const Filenames& filenames
    ) const override;
  };

  struct MergeSpectra : RawDataProcessor
  {
    int getID() const override { return -1; }
    std::string getName() const override { return "MERGE_SPECTRA"; }
    std::string getDescription() const override { return "Merge all spectra along the time axis."; }

    virtual ParameterSet getParameterSchema() const override;

    /** Merge all spectra along the time axis using a binning strategy that is resolution dependent
    */
    void process(
      RawDataHandler& rawDataHandler_IO,
      const ParameterSet& params_I,
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
      const ParameterSet& params_I,
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
      const ParameterSet& params_I,
      const Filenames& filenames
    ) const override;
  };

  struct LoadAnnotations : RawDataProcessor
  {
    int getID() const override { return -1; }
    std::string getName() const override { return "LOAD_ANNOTATIONS"; }
    std::string getDescription() const override { return "Read in the annotations from disk."; }

    /** Read in the annotations from disk.
    */
    void process(
      RawDataHandler& rawDataHandler_IO,
      const ParameterSet& params_I,
      const Filenames& filenames
    ) const override;
  };

  struct StoreAnnotations : RawDataProcessor
  {
    int getID() const override { return -1; }
    std::string getName() const override { return "STORE_ANNOTATIONS"; }
    std::string getDescription() const override { return "Write the annotations to disk."; }

    /** Write the annotations to disk.
    */
    void process(
      RawDataHandler& rawDataHandler_IO,
      const ParameterSet& params_I,
      const Filenames& filenames
    ) const override;
  };

  struct PickMRMFeatures : RawDataProcessor
  {
    int getID() const override { return 3; }
    std::string getName() const override { return "PICK_MRM_FEATURES"; }
    std::string getDescription() const override { return "Run the peak picking algorithm for SRMs/MRMs."; }

    virtual ParameterSet getParameterSchema() const override;

    /** Run the openSWATH pick peaking and scoring workflow for a single raw data file.
    */
    void process(
      RawDataHandler& rawDataHandler_IO,
      const ParameterSet& params_I,
      const Filenames& filenames
    ) const override;
  };

  struct PickMS1Features : RawDataProcessor
  {
    int getID() const override { return -1; }
    std::string getName() const override { return "PICK_MS1_FEATURES"; }
    std::string getDescription() const override { return "Run the peak picking algorithm for MS1 spectra."; }

    virtual ParameterSet getParameterSchema() const override;

    /** Run the MS1 peak picking and scoring algorithm.
    */
    void process(
      RawDataHandler& rawDataHandler_IO,
      const ParameterSet& params_I,
      const Filenames& filenames
    ) const override;
  };

  struct SearchAccurateMass : RawDataProcessor
  {
    int getID() const override { return -1; }
    std::string getName() const override { return "SEARCH_ACCURATE_MASS"; }
    std::string getDescription() const override { return "Run the accurate mass search algorithm."; }

    virtual ParameterSet getParameterSchema() const override;

    /** Run the accurate mass search algorithm.
    */
    void process(
      RawDataHandler& rawDataHandler_IO,
      const ParameterSet& params_I,
      const Filenames& filenames
    ) const override;
  };

  struct MergeFeatures : RawDataProcessor
  {
      int getID() const override { return -1; }
      std::string getName() const override { return "MERGE_FEATURES"; }
      std::string getDescription() const override { return "Create merged features from accurate mass search results."; }

      /** Create merged features from accurate mass search results.
      */
      void process(
          RawDataHandler& rawDataHandler_IO,
          const ParameterSet& params_I,
          const Filenames& filenames
      ) const override;
  };

  struct FilterFeatures : RawDataProcessor
  {
    int getID() const override { return 4; }
    std::string getName() const override { return "FILTER_FEATURES"; }
    std::string getDescription() const override { return "Filter transitions and transitions groups based on a user defined criteria."; }

    virtual ParameterSet getParameterSchema() const override;

    /** Filter features that do not pass the filter QCs.
    */
    void process(
      RawDataHandler& rawDataHandler_IO,
      const ParameterSet& params_I,
      const Filenames& filenames
    ) const override;
  };

  struct CheckFeatures : RawDataProcessor
  {
    int getID() const override { return 8; }
    std::string getName() const override { return "CHECK_FEATURES"; }
    std::string getDescription() const override { return "Flag and score transitions and transition groups based on a user defined criteria."; }

    virtual ParameterSet getParameterSchema() const override;

    /** Flag features that do not pass the filter QCs.
    */
    void process(
      RawDataHandler& rawDataHandler_IO,
      const ParameterSet& params_I,
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
      const ParameterSet& params_I,
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
      const ParameterSet& params_I,
      const Filenames& filenames
    ) const override;
  };

  struct QuantifyFeatures : RawDataProcessor
  {
    int getID() const override { return 7; }
    std::string getName() const override { return "QUANTIFY_FEATURES"; }
    std::string getDescription() const override { return "Apply a calibration model defined in quantitationMethods to each transition."; }

    virtual ParameterSet getParameterSchema() const override;

    /** Quantify all unknown samples based on the quantitationMethod.
    */
    void process(
      RawDataHandler& rawDataHandler_IO,
      const ParameterSet& params_I,
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
      const ParameterSet& params_I,
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
      const ParameterSet& params_I,
      const Filenames& filenames
    ) const override;
  };

  struct LoadFeatureFiltersRDP : RawDataProcessor
  {
    int getID() const override { return -1; }
    std::string getName() const override { return "LOAD_FEATURE_FILTERS"; }
    std::string getDescription() const override { return "Load the component and component group transition filters from file."; }

    /** Load the component and component group transition filters from file.
    */
    void process(
      RawDataHandler& rawDataHandler_IO,
      const ParameterSet& params_I,
      const Filenames& filenames
    ) const override;
  };

  struct LoadFeatureQCsRDP : RawDataProcessor
  {
    int getID() const override { return -1; }
    std::string getName() const override { return "LOAD_FEATURE_QCS"; }
    std::string getDescription() const override { return "Load the component and component group transition QC specifications from file."; }

    /** Load the component and component group transition QCs from file.
    */
    void process(
      RawDataHandler& rawDataHandler_IO,
      const ParameterSet& params_I,
      const Filenames& filenames
    ) const override;
  };

  struct StoreFeatureFiltersRDP : RawDataProcessor
  {
    int getID() const override { return -1; }
    std::string getName() const override { return "STORE_FEATURE_FILTERS"; }
    std::string getDescription() const override { return "Store the component and component group transition filters from file."; }

    /** Store the component and component group transition filters from file.
    */
    void process(
      RawDataHandler& rawDataHandler_IO,
      const ParameterSet& params_I,
      const Filenames& filenames
    ) const override;
  };

  struct StoreFeatureQCsRDP : RawDataProcessor
  {
    int getID() const override { return -1; }
    std::string getName() const override { return "STORE_FEATURE_QCS"; }
    std::string getDescription() const override { return "Store the component and component group transition QC specifications from file."; }

    /** Store the component and component group transition QCs from file.
    */
    void process(
      RawDataHandler& rawDataHandler_IO,
      const ParameterSet& params_I,
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
      const ParameterSet& params_I,
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
      const ParameterSet& params_I,
      const Filenames& filenames
    ) const override;
    static void sanitizeParameters(
      ParameterSet& params_I
    );
  };

  struct FitFeaturesEMG : RawDataProcessor
  {
    int getID() const override { return 14; }
    std::string getName() const override { return "FIT_FEATURES_EMG"; }
    std::string getDescription() const override { return "Reconstruct a peak from available data points."; }

    virtual ParameterSet getParameterSchema() const override;

    /** Apply the EMG peak reconstruction technique to the data points.
    */
    void process(
      RawDataHandler& rawDataHandler_IO,
      const ParameterSet& params_I,
      const Filenames& filenames
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
    int getID() const override { return 4; }
    std::string getName() const override { return "FILTER_FEATURES_RSDS"; }
    std::string getDescription() const override { return "Filter transitions and transitions groups based on a user defined criteria."; }

    virtual ParameterSet getParameterSchema() const override;

    /** Filter features that do not pass the filter QCs.
    */
    void process(
      RawDataHandler& rawDataHandler_IO,
      const ParameterSet& params_I,
      const Filenames& filenames
    ) const override;
  };

  struct CheckFeaturesRSDs : RawDataProcessor
  {
    int getID() const override { return 8; }
    std::string getName() const override { return "CHECK_FEATURES_RSDS"; }
    std::string getDescription() const override { return "Flag and score transitions and transition groups based on a user defined criteria."; }

    virtual ParameterSet getParameterSchema() const override;

    /** Flag features that do not pass the filter QCs.
    */
    void process(
      RawDataHandler& rawDataHandler_IO,
      const ParameterSet& params_I,
      const Filenames& filenames
    ) const override;
  };

  struct FilterFeaturesBackgroundInterferences : RawDataProcessor
  {
    int getID() const override { return 4; }
    std::string getName() const override { return "FILTER_FEATURES_BACKGROUND_INTERFERENCES"; }
    std::string getDescription() const override { return "Filter transitions and transitions groups based on a user defined criteria."; }

    virtual ParameterSet getParameterSchema() const override;

    /** Filter features that do not pass the filter QCs.
    */
    void process(
      RawDataHandler& rawDataHandler_IO,
      const ParameterSet& params_I,
      const Filenames& filenames
    ) const override;
  };

  struct CheckFeaturesBackgroundInterferences : RawDataProcessor
  {
    int getID() const override { return 8; }
    std::string getName() const override { return "CHECK_FEATURES_BACKGROUND_INTERFERENCES"; }
    std::string getDescription() const override { return "Flag and score transitions and transition groups based on a user defined criteria."; }

    virtual ParameterSet getParameterSchema() const override;

    /** Flag features that do not pass the filter QCs.
    */
    void process(
      RawDataHandler& rawDataHandler_IO,
      const ParameterSet& params_I,
      const Filenames& filenames
    ) const override;
  };

  struct ClearData : RawDataProcessor
  {
    int getID() const override { return -1; }
    std::string getName() const override { return "CLEAR_DATA"; }
    std::string getDescription() const override { return "Clear raw and processed data."; }

    /** Clear all raw and process data
    */
    void process(
      RawDataHandler& rawDataHandler_IO,
      const ParameterSet& params_I,
      const Filenames& filenames
    ) const override;
  };
  
  struct CalculateMDVs : RawDataProcessor
  {
    int getID() const override { return 0; }
    std::string getName() const override { return "CALCULATE_MDVS"; }
    std::string getDescription() const override { return "Calculate MDVs."; }
    
    virtual ParameterSet getParameterSchema() const override;

    /** Calculate MDVs
     */
    void process(
                 RawDataHandler& rawDataHandler_IO,
                 const ParameterSet& params_I,
                 const Filenames& filenames
                 ) const override;
  };
  
  struct IsotopicCorrections : RawDataProcessor
  {
    int getID() const override { return 0; }
    std::string getName() const override { return "ISOTOPIC_CORRECTIONS"; }
    std::string getDescription() const override { return "Perform Isotopic Corrections."; }
    
    virtual ParameterSet getParameterSchema() const override;

    /** Correct MDVs
     */
    void process(
                 RawDataHandler& rawDataHandler_IO,
                 const ParameterSet& params_I,
                 const Filenames& filenames
                 ) const override;
  };
  
  struct CalculateIsotopicPurities : RawDataProcessor
  {
    int getID() const override { return 0; }
    std::string getName() const override { return "CALCULATE_MDV_ISOTOPIC_PURITIES"; }
    std::string getDescription() const override { return "Calculate MDV Isotopic Purities."; }
    
    virtual ParameterSet getParameterSchema() const override;

    /** Calculate MDV Isotopic Purities
     */
    void process(
                 RawDataHandler& rawDataHandler_IO,
                 const ParameterSet& params_I,
                 const Filenames& filenames
                 ) const override;
  };

  struct CalculateMDVAccuracies : RawDataProcessor
  {
    int getID() const override { return 0; }
    std::string getName() const override { return "COMPARE_MDV_TO_THEORETICAL"; }
    std::string getDescription() const override { return "Compare MDVs to Theoretical"; }
    
    virtual ParameterSet getParameterSchema() const override;

    /** Compare MDVs to Theoretical
     */
    void process(
                 RawDataHandler& rawDataHandler_IO,
                 const ParameterSet& params_I,
                 const Filenames& filenames
                 ) const override;
  };
}
