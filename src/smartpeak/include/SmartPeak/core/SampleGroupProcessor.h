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
// $Authors: Douglas McCloskey $
// --------------------------------------------------------------------------

#pragma once

#include <SmartPeak/core/Filenames.h>
#include <SmartPeak/core/MetaDataHandler.h>
#include <SmartPeak/core/SampleType.h>
#include <SmartPeak/core/SequenceHandler.h>
#include <SmartPeak/core/SampleGroupHandler.h>
#include <SmartPeak/core/Parameters.h>
#include <SmartPeak/iface/IProcessorDescription.h>
#include <SmartPeak/iface/IInputsOutputsProvider.h>

namespace SmartPeak
{
  struct SampleGroupProcessor : IProcessorDescription, IInputsOutputsProvider
  {
    SampleGroupProcessor(const SampleGroupProcessor& other) = delete;
    SampleGroupProcessor& operator=(const SampleGroupProcessor& other) = delete;
    virtual ~SampleGroupProcessor() = default;

    /* IInputsOutputsProvider */
    virtual void getInputsOutputs(Filenames& filenames) const override { };

    /**
      Interface to all sample group processing methods.

      @param[in,out] sampleGroupHandler_IO Sequence segment handler
      @param[in] sequenceHandler_I Sequence handler
      @param[in] params_I Dictionary of parameter names, values, descriptions, and tags
      @param[in] filenames Info about where data should be read from or written to
    */
    virtual void process(
      SampleGroupHandler& sampleGroupHandler_IO,
      const SequenceHandler& sequenceHandler_I,
      const ParameterSet& params_I,
      const Filenames& filenames_override
    ) const = 0;

  protected:
    SampleGroupProcessor() = default;
  };

  struct SelectDilutions : SampleGroupProcessor
  {
    int getID() const override { return -1; }
    std::string getName() const override { return "SELECT_DILUTIONS"; }
    std::string getDescription() const override { return "Select features from dilution preferences"; }
    virtual ParameterSet getParameterSchema() const override;

    /**
      Merge multiple injections of the same sample.
    */
    void process(
      SampleGroupHandler& sampleGroupHandler_IO,
      const SequenceHandler& sequenceHandler_I,
      const ParameterSet& params_I,
      const Filenames& filenames_override
    ) const override;

    /* IInputsOutputsProvider */
    virtual void getInputsOutputs(Filenames& filenames) const override;
  };

  struct MergeInjections : SampleGroupProcessor
  {
    int getID() const override { return -1; }
    std::string getName() const override { return "MERGE_INJECTIONS"; }
    std::string getDescription() const override { return "Merge multiple injections of the same sample."; }
    virtual ParameterSet getParameterSchema() const override;

    /**
      Merge multiple injections of the same sample.
    */
    void process(
      SampleGroupHandler& sampleGroupHandler_IO,
      const SequenceHandler& sequenceHandler_I,
      const ParameterSet& params_I,
      const Filenames& filenames_override
    ) const override;

  private:

    typedef std::tuple<std::string, std::pair<float, float>, float> mergeKeyType;
    typedef std::pair<std::string, std::string> componentKeyType;

    static void getMergeKeysToInjections(const SampleGroupHandler& sampleGroupHandler_IO,
      const SequenceHandler& sequenceHandler_I, 
      std::set<std::string>& scan_polarities,
      std::set<std::pair<float, float>>& scan_mass_ranges,
      std::set<float>& dilution_factors,
      std::map<mergeKeyType, std::vector<std::set<std::string>>>& merge_keys_to_injection_name);

    static void orderMergeKeysToInjections(std::set<std::string>& scan_polarities,
      const std::set<std::pair<float, float>>& scan_mass_ranges,
      const std::set<float>& dilution_factors,
      std::map<mergeKeyType, std::vector<std::set<std::string>>>& merge_keys_to_injection_name);

    static void getComponentsToFeaturesToInjectionsToValues(const SampleGroupHandler& sampleGroupHandler_IO,
      const SequenceHandler& sequenceHandler_I,
      const bool& merge_subordinates,
      std::map<componentKeyType, std::map<std::string, std::map<std::set<std::string>, float>>>& component_to_feature_to_injection_to_values);

    static void mergeComponentsToFeaturesToInjectionsToValues(const std::string& feature_name, const std::string& merge_rule,
      const std::set<std::string>& scan_polarities,
      const std::set<std::pair<float, float>>& scan_mass_ranges,
      const std::set<float>& dilution_factors,
      const std::map<mergeKeyType, std::vector<std::set<std::string>>>& merge_keys_to_injection_name,
      std::map<componentKeyType, std::map<std::string, std::map<std::set<std::string>, float>>>& component_to_feature_to_injection_to_values);

    static void makeFeatureMap(const bool& merge_subordinates,
      std::set<std::string>& injection_names_set,
      const std::map<componentKeyType, std::map<std::string, std::map<std::set<std::string>, float>>>& component_to_feature_to_injection_to_values,
      OpenMS::FeatureMap& feature_map);
  };

  struct LoadFeaturesSampleGroup : SampleGroupProcessor
  {
    int getID() const override { return -1; }
    std::string getName() const override { return "LOAD_FEATURES_SAMPLE_GROUP"; }
    std::string getDescription() const override { return "Load the features for the sample group."; }
    virtual ParameterSet getParameterSchema() const override;

    /**
      Load the features for the sample group.
    */
    void process(
      SampleGroupHandler& sampleGroupHandler_IO,
      const SequenceHandler& sequenceHandler_I,
      const ParameterSet& params_I,
      const Filenames& filenames_override
    ) const override;

    /* IInputsOutputsProvider */
    virtual void getInputsOutputs(Filenames& filenames) const override;
  };

  struct StoreFeaturesSampleGroup : SampleGroupProcessor
  {
    int getID() const override { return -1; }
    std::string getName() const override { return "STORE_FEATURES_SAMPLE_GROUP"; }
    std::string getDescription() const override { return "Store the features for the sample group."; }
    virtual ParameterSet getParameterSchema() const override;

    /**
      Store the features for the sample group.
    */
    void process(
      SampleGroupHandler& sampleGroupHandler_IO,
      const SequenceHandler& sequenceHandler_I,
      const ParameterSet& params_I,
      const Filenames& filenames_override
    ) const override;

    /* IInputsOutputsProvider */
    virtual void getInputsOutputs(Filenames& filenames) const override;
  };
}
