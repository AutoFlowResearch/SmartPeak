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
// $Authors: Douglas McCloskey, Pasquale Domenico Colaianni $
// --------------------------------------------------------------------------

#pragma once

#include <SmartPeak/core/Filenames.h>
#include <SmartPeak/core/MetaDataHandler.h>
#include <SmartPeak/core/SampleType.h>
#include <SmartPeak/core/SequenceHandler.h>
#include <SmartPeak/core/SequenceSegmentHandler.h>
#include <SmartPeak/iface/IProcessorDescription.h>
#include <SmartPeak/core/Parameters.h>

namespace SmartPeak
{
  struct SequenceSegmentProcessor : IProcessorDescription
  {
    SequenceSegmentProcessor(const SequenceSegmentProcessor& other) = delete;
    SequenceSegmentProcessor& operator=(const SequenceSegmentProcessor& other) = delete;
    virtual ~SequenceSegmentProcessor() = default;

    /**
      Interface to all  sequence segment processing methods.

      @param[in,out] sequenceSegmentHandler_IO Sequence segment handler
      @param[in] sequenceHandler_I Sequence handler
      @param[in] params_I Dictionary of parameter names, values, descriptions, and tags
      @param[in] filenames Info about where data should be read from or written to
    */
    virtual void process(
      SequenceSegmentHandler& sequenceSegmentHandler_IO,
      const SequenceHandler& sequenceHandler_I,
      const ParameterSet& params_I,
      const Filenames& filenames
    ) const = 0;

    /**
      Return all injection indices that belong to a given sample type.

      @param[in] sequenceSegmentHandler Sequence segment handler
      @param[in] sequenceHandler Sequence handler
      @param[in] sampleType Sample type filter
      @param[out] sampleIndices Output sample indices
    */
    static void getSampleIndicesBySampleType(
      const SequenceSegmentHandler& sequenceSegmentHandler,
      const SequenceHandler& sequenceHandler,
      const SampleType sampleType,
      std::vector<size_t>& sampleIndices
    );

  protected:
    // Forced to write this, because the other user-defined constructors inhibit
    // the implicit definition of a default constructor
    // Even though this class is abstract and hence can't be instantiated,
    // derived classes will call the base's constructor
    SequenceSegmentProcessor() = default;
  };

  struct CalculateCalibration : SequenceSegmentProcessor
  {
    int getID() const override { return 15; }
    std::string getName() const override { return "CALCULATE_CALIBRATION"; }
    std::string getDescription() const override { return "Determine the optimal relationship between known sample concentration and measured intensity."; }

    virtual ParameterSet getParameterSchema() const override;

    /**
      Optimize the calibration curve for all components.
    */
    void process(
      SequenceSegmentHandler& sequenceSegmentHandler_IO,
      const SequenceHandler& sequenceHandler_I,
      const ParameterSet& params_I,
      const Filenames& filenames
    ) const override;
  };

  struct LoadStandardsConcentrations : SequenceSegmentProcessor
  {
    int getID() const override { return -1; }
    std::string getName() const override { return "LOAD_STANDARDS_CONCENTRATIONS"; }
    std::string getDescription() const override { return "Load the standards concentrations file that gives the relationship between injection, component, and known concentration from disk."; }

    virtual ParameterSet getParameterSchema() const override;

    /**
      Load the standards concentration file.
    */
    void process(
      SequenceSegmentHandler& sequenceSegmentHandler_IO,
      const SequenceHandler& sequenceHandler_I,
      const ParameterSet& params_I,
      const Filenames& filenames
    ) const override;
  };

  struct LoadQuantitationMethods : SequenceSegmentProcessor
  {
    int getID() const override { return 17; }
    std::string getName() const override { return "LOAD_QUANTITATION_METHODS"; }
    std::string getDescription() const override { return "Load each transitions calibration model defined in quantitationMethods from disk."; }

    virtual ParameterSet getParameterSchema() const override;

    /**
      Load the quantitation methods file.
    */
    void process(
      SequenceSegmentHandler& sequenceSegmentHandler_IO,
      const SequenceHandler& sequenceHandler_I,
      const ParameterSet& params_I,
      const Filenames& filenames
    ) const override;
  };

  struct StoreQuantitationMethods : SequenceSegmentProcessor
  {
    int getID() const override { return 16; }
    std::string getName() const override { return "STORE_QUANTITATION_METHODS"; }
    std::string getDescription() const override { return "Write each transitions calibration model to disk for later use."; }

    virtual ParameterSet getParameterSchema() const override;

    /**
      Write the quantitation methods to disk.
    */
    void process(
      SequenceSegmentHandler& sequenceSegmentHandler_IO,
      const SequenceHandler& sequenceHandler_I,
      const ParameterSet& params_I,
      const Filenames& filenames
    ) const override;
  };

  struct LoadFeatureFilters : SequenceSegmentProcessor
  {
    int getID() const override { return -1; }
    std::string getName() const override { return "LOAD_FEATURE_FILTERS"; }
    std::string getDescription() const override { return "Load the component and component group filters from file."; }

    virtual ParameterSet getParameterSchema() const override;

    /** Load the component and component group filters from file.
    */
    void process(
      SequenceSegmentHandler& sequenceSegmentHandler_IO,
      const SequenceHandler& sequenceHandler_I,
      const ParameterSet& params_I,
      const Filenames& filenames
    ) const override;
  };

  struct LoadFeatureQCs : SequenceSegmentProcessor
  {
    int getID() const override { return -1; }
    std::string getName() const override { return "LOAD_FEATURE_QCS"; }
    std::string getDescription() const override { return "Load the component and component group QCs from file."; }

    virtual ParameterSet getParameterSchema() const override;

    /** Load the component and component group QCs from file.
    */
    void process(
      SequenceSegmentHandler& sequenceSegmentHandler_IO,
      const SequenceHandler& sequenceHandler_I,
      const ParameterSet& params_I,
      const Filenames& filenames
    ) const override;
  };

  struct StoreFeatureFilters : SequenceSegmentProcessor
  {
    int getID() const override { return -1; }
    std::string getName() const override { return "STORE_FEATURE_FILTERS"; }
    std::string getDescription() const override { return "Store the component and component group filters to disk."; }

    virtual ParameterSet getParameterSchema() const override;

    /** Store the component and component group filters to disk.
    */
    void process(
      SequenceSegmentHandler& sequenceSegmentHandler_IO,
      const SequenceHandler& sequenceHandler_I,
      const ParameterSet& params_I,
      const Filenames& filenames
    ) const override;
  };

  struct StoreFeatureQCs : SequenceSegmentProcessor
  {
    int getID() const override { return -1; }
    std::string getName() const override { return "STORE_FEATURE_QCS"; }
    std::string getDescription() const override { return "Store the component and component group QCs to disk."; }

    virtual ParameterSet getParameterSchema() const override;

    /** Store the component and component group QCs to disk.
    */
    void process(
      SequenceSegmentHandler& sequenceSegmentHandler_IO,
      const SequenceHandler& sequenceHandler_I,
      const ParameterSet& params_I,
      const Filenames& filenames
    ) const override;
  };

  struct LoadFeatureRSDFilters : SequenceSegmentProcessor
  {
    int getID() const override { return -1; }
    std::string getName() const override { return "LOAD_FEATURE_RSD_FILTERS"; }
    std::string getDescription() const override { return "Load the component and component group percent RSD filters from file."; }

    virtual ParameterSet getParameterSchema() const override;

    /** Load the component and component group percent RSD filters from file.
    */
    void process(
      SequenceSegmentHandler& sequenceSegmentHandler_IO,
      const SequenceHandler& sequenceHandler_I,
      const ParameterSet& params_I,
      const Filenames& filenames
    ) const override;
  };

  struct LoadFeatureRSDQCs : SequenceSegmentProcessor
  {
    int getID() const override { return -1; }
    std::string getName() const override { return "LOAD_FEATURE_RSD_QCS"; }
    std::string getDescription() const override { return "Load the component and component group percent RSD QCs from file."; }

    virtual ParameterSet getParameterSchema() const override;

    /** Load the component and component group percent RSD QCs from file.
    */
    void process(
      SequenceSegmentHandler& sequenceSegmentHandler_IO,
      const SequenceHandler& sequenceHandler_I,
      const ParameterSet& params_I,
      const Filenames& filenames
    ) const override;
  };

  struct StoreFeatureRSDFilters : SequenceSegmentProcessor
  {
    int getID() const override { return -1; }
    std::string getName() const override { return "STORE_FEATURE_RSD_FILTERS"; }
    std::string getDescription() const override { return "Store the component and component group percent RSD filters to disk."; }

    virtual ParameterSet getParameterSchema() const override;

    /** Store the component and component group percent RSD filters to disk.
    */
    void process(
      SequenceSegmentHandler& sequenceSegmentHandler_IO,
      const SequenceHandler& sequenceHandler_I,
      const ParameterSet& params_I,
      const Filenames& filenames
    ) const override;
  };

  struct StoreFeatureRSDQCs : SequenceSegmentProcessor
  {
    int getID() const override { return -1; }
    std::string getName() const override { return "STORE_FEATURE_RSD_QCS"; }
    std::string getDescription() const override { return "Store the component and component group percent RSD QCs to disk."; }

    virtual ParameterSet getParameterSchema() const override;

    /** Store the component and component group percent RSD QCs to disk.
    */
    void process(
      SequenceSegmentHandler& sequenceSegmentHandler_IO,
      const SequenceHandler& sequenceHandler_I,
      const ParameterSet& params_I,
      const Filenames& filenames
    ) const override;
  };

  struct LoadFeatureBackgroundFilters : SequenceSegmentProcessor
  {
    int getID() const override { return -1; }
    std::string getName() const override { return "LOAD_FEATURE_BACKGROUND_FILTERS"; }
    std::string getDescription() const override { return "Load the component and component group percent Background Interference filters from file."; }

    virtual ParameterSet getParameterSchema() const override;

    /** Load the component and component group percent Background Interference filters from file.
    */
    void process(
      SequenceSegmentHandler& sequenceSegmentHandler_IO,
      const SequenceHandler& sequenceHandler_I,
      const ParameterSet& params_I,
      const Filenames& filenames
    ) const override;
  };

  struct LoadFeatureBackgroundQCs : SequenceSegmentProcessor
  {
    int getID() const override { return -1; }
    std::string getName() const override { return "LOAD_FEATURE_BACKGROUND_QCS"; }
    std::string getDescription() const override { return "Load the component and component group percent Background Interference QCs from file."; }

    virtual ParameterSet getParameterSchema() const override;

    /** Load the component and component group percent Background Interference QCs from file.
    */
    void process(
      SequenceSegmentHandler& sequenceSegmentHandler_IO,
      const SequenceHandler& sequenceHandler_I,
      const ParameterSet& params_I,
      const Filenames& filenames
    ) const override;
  };

  struct StoreFeatureBackgroundFilters : SequenceSegmentProcessor
  {
    int getID() const override { return -1; }
    std::string getName() const override { return "STORE_FEATURE_BACKGROUND_FILTERS"; }
    std::string getDescription() const override { return "Store the component and component group percent Background Interference filters to disk."; }

    virtual ParameterSet getParameterSchema() const override;

    /** Store the component and component group percent Background Interference filters to disk.
    */
    void process(
      SequenceSegmentHandler& sequenceSegmentHandler_IO,
      const SequenceHandler& sequenceHandler_I,
      const ParameterSet& params_I,
      const Filenames& filenames
    ) const override;
  };

  struct StoreFeatureBackgroundQCs : SequenceSegmentProcessor
  {
    int getID() const override { return -1; }
    std::string getName() const override { return "STORE_FEATURE_BACKGROUND_QCS"; }
    std::string getDescription() const override { return "Store the component and component group percent Background Interference QCs to disk."; }

    virtual ParameterSet getParameterSchema() const override;

    /** Store the component and component group percent Background Interference QCs to disk.
    */
    void process(
      SequenceSegmentHandler& sequenceSegmentHandler_IO,
      const SequenceHandler& sequenceHandler_I,
      const ParameterSet& params_I,
      const Filenames& filenames
    ) const override;
  };

  struct EstimateFeatureFilterValues : SequenceSegmentProcessor
  {
    int getID() const override { return -1; }
    std::string getName() const override { return "ESTIMATE_FEATURE_FILTER_VALUES"; }
    std::string getDescription() const override { return "Estimate default FeatureQC parameter values for the feature filters from Standard and QC samples."; }

    virtual ParameterSet getParameterSchema() const override;

    /**
      @brief Estimate default FeatureQC parameter values from Standard and QC samples.
        The Standard samples should span the LLOQ and ULOQ. The `setComponentsToConcentrations`
        will be used to guide which Standard samples.
    */
    void process(
      SequenceSegmentHandler& sequenceSegmentHandler_IO,
      const SequenceHandler& sequenceHandler_I,
      const ParameterSet& params_I,
      const Filenames& filenames
    ) const override;
  };

  struct EstimateFeatureQCValues : SequenceSegmentProcessor
  {
    int getID() const override { return -1; }
    std::string getName() const override { return "ESTIMATE_FEATURE_QC_VALUES"; }
    std::string getDescription() const override { return "Estimate default FeatureQC parameter values for the feature QCs from Standard and QC samples."; }

    virtual ParameterSet getParameterSchema() const override;

    /**
      @brief Estimate default FeatureQC parameter values from Standard and QC samples.
        The Standard samples should span the LLOQ and ULOQ. The `setComponentsToConcentrations`
        will be used to guide which Standard samples.
    */
    void process(
      SequenceSegmentHandler& sequenceSegmentHandler_IO,
      const SequenceHandler& sequenceHandler_I,
      const ParameterSet& params_I,
      const Filenames& filenames
    ) const override;
  };

  struct TransferLOQToFeatureFilters : SequenceSegmentProcessor
  {
    int getID() const override { return -1; }
    std::string getName() const override { return "TRANSFER_LOQ_TO_FEATURE_FILTERS"; }
    std::string getDescription() const override { return "Transfer the upper (u)/lower (l) limits of quantitation (LOQ) values from the quantitation methods to the calculated concentration bounds of the feature filters."; }

    virtual ParameterSet getParameterSchema() const override;

    /**
      Transfer the upper (u)/lower (l) limits of quantitation (LOQ) values from the quantitation methods to the calculated concentration bounds of the feature filters
    */
    void process(
      SequenceSegmentHandler& sequenceSegmentHandler_IO,
      const SequenceHandler& sequenceHandler_I,
      const ParameterSet& params_I,
      const Filenames& filenames
    ) const override;
  };

  struct TransferLOQToFeatureQCs : SequenceSegmentProcessor
  {
    int getID() const override { return -1; }
    std::string getName() const override { return "TRANSFER_LOQ_TO_FEATURE_QCS"; }
    std::string getDescription() const override { return "Transfer the upper (u)/lower (l) limits of quantitation (LOQ) values from the quantitation methods to the calculated concentration bounds of the feature filters."; }

    virtual ParameterSet getParameterSchema() const override;

    /**
      Transfer the upper (u)/lower (l) limits of quantitation (LOQ) values from the quantitation methods to the calculated concentration bounds of the feature QCs
    */
    void process(
      SequenceSegmentHandler& sequenceSegmentHandler_IO,
      const SequenceHandler& sequenceHandler_I,
      const ParameterSet& params_I,
      const Filenames& filenames
    ) const override;
  };

  struct EstimateFeatureRSDs : SequenceSegmentProcessor
  {
    int getID() const override { return -1; }
    std::string getName() const override { return "ESTIMATE_FEATURE_RSDS"; }
    std::string getDescription() const override { return "Estimate the %RSD for component and component group feature filter attributes from pooled QC samples."; }

    virtual ParameterSet getParameterSchema() const override;

    /**
      Estimate the %RSD for component and component group feature filter attributes from pooled QC samples

      NOTE: estimation from replicate Unknown samples is not yet implemented
    */
    void process(
      SequenceSegmentHandler& sequenceSegmentHandler_IO,
      const SequenceHandler& sequenceHandler_I,
      const ParameterSet& params_I,
      const Filenames& filenames
    ) const override;
  };

  struct EstimateFeatureBackgroundInterferences : SequenceSegmentProcessor
  {
    int getID() const override { return -1; }
    std::string getName() const override { return "ESTIMATE_FEATURE_BACKGROUND_INTERFERENCES"; }
    std::string getDescription() const override { return "Estimate the %BackgroundInterferences for component and component group feature filter ion intensity attributes from Blank samples."; }

    virtual ParameterSet getParameterSchema() const override;

    /**
      Estimate the %BackgroundInterferences for component and component group feature filter ion intensity attributes from Blank samples
    */
    void process(
      SequenceSegmentHandler& sequenceSegmentHandler_IO,
      const SequenceHandler& sequenceHandler_I,
      const ParameterSet& params_I,
      const Filenames& filenames
    ) const override;
  };


  struct LoadFeatureRSDEstimations : SequenceSegmentProcessor
  {
    int getID() const override { return -1; }
    std::string getName() const override { return "LOAD_FEATURE_RSD_ESTIMATIONS"; }
    std::string getDescription() const override { return "Load the component and component group percent RSD estimations from file."; }

    virtual ParameterSet getParameterSchema() const override;

    /** Load the component and component group percent RSD estimations from file.
    */
    void process(
      SequenceSegmentHandler& sequenceSegmentHandler_IO,
      const SequenceHandler& sequenceHandler_I,
      const ParameterSet& params_I,
      const Filenames& filenames
    ) const override;
  };

  struct StoreFeatureRSDEstimations : SequenceSegmentProcessor
  {
    int getID() const override { return -1; }
    std::string getName() const override { return "STORE_FEATURE_RSD_ESTIMATIONS"; }
    std::string getDescription() const override { return "Store the component and component group percent RSD estimations to disk."; }

    virtual ParameterSet getParameterSchema() const override;

    /** Store the component and component group percent RSD estimations to disk.
    */
    void process(
      SequenceSegmentHandler& sequenceSegmentHandler_IO,
      const SequenceHandler& sequenceHandler_I,
      const ParameterSet& params_I,
      const Filenames& filenames
    ) const override;
  };

  struct LoadFeatureBackgroundEstimations : SequenceSegmentProcessor
  {
    int getID() const override { return -1; }
    std::string getName() const override { return "LOAD_FEATURE_BACKGROUND_ESTIMATIONS"; }
    std::string getDescription() const override { return "Load the component and component group percent Background Interference estimations from file."; }

    virtual ParameterSet getParameterSchema() const override;

    /** Load the component and component group percent Background Interference estimations from file.
    */
    void process(
      SequenceSegmentHandler& sequenceSegmentHandler_IO,
      const SequenceHandler& sequenceHandler_I,
      const ParameterSet& params_I,
      const Filenames& filenames
    ) const override;
  };

  struct StoreFeatureBackgroundEstimations : SequenceSegmentProcessor
  {
    int getID() const override { return -1; }
    std::string getName() const override { return "STORE_FEATURE_BACKGROUND_ESTIMATIONS"; }
    std::string getDescription() const override { return "Store the component and component group percent Background Interference estimations to disk."; }

    virtual ParameterSet getParameterSchema() const override;

    /** Store the component and component group percent Background Interference estimations to disk.
    */
    void process(
      SequenceSegmentHandler& sequenceSegmentHandler_IO,
      const SequenceHandler& sequenceHandler_I,
      const ParameterSet& params_I,
      const Filenames& filenames
    ) const override;
  };
}
