// TODO: Add copyright

#pragma once

#include <SmartPeak/core/Filenames.h>
#include <SmartPeak/core/MetaDataHandler.h>
#include <SmartPeak/core/SampleType.h>
#include <SmartPeak/core/SequenceHandler.h>
#include <SmartPeak/core/SampleGroupHandler.h>

namespace SmartPeak
{
  struct SampleGroupProcessor
  {
    SampleGroupProcessor(const SampleGroupProcessor& other) = delete;
    SampleGroupProcessor& operator=(const SampleGroupProcessor& other) = delete;
    virtual ~SampleGroupProcessor() = default;

    virtual int getID() const = 0; /// get the sample group processor struct ID
    virtual std::string getName() const = 0; /// get the sample group processor struct name
    virtual std::string getDescription() const = 0; /// get the sample group processor struct description

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
      const std::map<std::string, std::vector<std::map<std::string, std::string>>>& params_I,
      const Filenames& filenames
    ) const = 0;

  protected:
    SampleGroupProcessor() = default;
  };

  struct MergeInjections : SampleGroupProcessor
  {
    int getID() const override { return -1; }
    std::string getName() const override { return "MERGE_INJECTIONS"; }
    std::string getDescription() const override { return "Merge multiple injections of the same sample."; }

    /**
      Merge multiple injections of the same sample.
    */
    void process(
      SampleGroupHandler& sampleGroupHandler_IO,
      const SequenceHandler& sequenceHandler_I,
      const std::map<std::string, std::vector<std::map<std::string, std::string>>>& params_I,
      const Filenames& filenames
    ) const override;

    static void getMergeKeysToInjections(const SampleGroupHandler& sampleGroupHandler_IO,
      const SequenceHandler& sequenceHandler_I, 
      std::set<std::string>& scan_polarities,
      std::set<std::pair<float, float>>& scan_mass_ranges,
      std::set<float>& dilution_factors,
      std::map<std::tuple<std::string, std::pair<float, float>, float>, std::vector<std::set<std::string>>>& merge_keys_to_injection_name);
    static void orderMergeKeysToInjections(std::set<std::string>& scan_polarities,
      const std::set<std::pair<float, float>>& scan_mass_ranges,
      const std::set<float>& dilution_factors,
      std::map<std::tuple<std::string, std::pair<float, float>, float>, std::vector<std::set<std::string>>>& merge_keys_to_injection_name);
    static void getComponentsToFeaturesToInjectionsToValues(const SampleGroupHandler& sampleGroupHandler_IO,
      const SequenceHandler& sequenceHandler_I,
      const bool& merge_subordinates,
      std::map<std::pair<std::string, std::string>, std::map<std::string, std::map<std::set<std::string>, float>>>& component_to_feature_to_injection_to_values);
    static void mergeComponentsToFeaturesToInjectionsToValues(const std::string& feature_name, const std::string& merge_rule,
      std::set<std::string>& scan_polarities,
      const std::set<std::pair<float, float>>& scan_mass_ranges,
      const std::set<float>& dilution_factors,
      const std::map<std::tuple<std::string, std::pair<float, float>, float>, std::vector<std::set<std::string>>>& merge_keys_to_injection_name,
      std::map<std::pair<std::string, std::string>, std::map<std::string, std::map<std::set<std::string>, float>>>& component_to_feature_to_injection_to_values);
    static void makeFeatureMap(const bool& merge_subordinates,
      std::set<std::string>& scan_polarities,
      const std::set<std::pair<float, float>>& scan_mass_ranges,
      const std::set<float>& dilution_factors,
      const std::map<std::tuple<std::string, std::pair<float, float>, float>, std::vector<std::set<std::string>>>& merge_keys_to_injection_name,
      const std::map<std::pair<std::string, std::string>, std::map<std::string, std::map<std::set<std::string>, float>>>& component_to_feature_to_injection_to_values,
      OpenMS::FeatureMap& feature_map);
  };
}
