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

#include <SmartPeak/core/SampleGroupProcessor.h>
#include <SmartPeak/core/SequenceHandler.h>
#include <SmartPeak/core/FeatureMetadata.h>
#include <SmartPeak/io/InputDataValidation.h>
#include <OpenMS/FORMAT/FeatureXMLFile.h>
#include <SmartPeak/io/SelectDilutionsParser.h>
#include <plog/Log.h>

namespace SmartPeak
{

  std::vector<std::string> MergeInjections::getRequirements() const
  {
    return { "sequence", "traML" };
  }

  ParameterSet MergeInjections::getParameterSchema() const
  {
    std::map<std::string, std::vector<std::map<std::string, std::string>>> param_struct({
    {"MergeInjections", {
      {
        {"name", "scan_polarity_merge_rule"},
        {"type", "string"},
        {"value", "WeightedMean"},
        {"description", "The method used to merge the different feature values. Options include Sum (the sum of all feature values), Min (the smallest feature value), Max (the largest feature value), Mean (the mean of all feature values), and WeightedMean (the weight average of all feature values)."},
        {"valid_strings", "['Sum','Min','Max','Mean','WeightedMean']"}
      },
      {
        {"name", "mass_range_merge_rule"},
        {"type", "string"},
        {"value", "Sum"},
        {"description", "The method used to merge the different feature values. Options include Sum (the sum of all feature values), Min (the smallest feature value), Max (the largest feature value), Mean (the mean of all feature values), and WeightedMean (the weight average of all feature values)."},
        {"valid_strings", "['Sum','Min','Max','Mean','WeightedMean']"}
      },
      {
        {"name", "dilution_series_merge_rule"},
        {"type", "string"},
        {"value", "Max"},
        {"description", "The method used to merge the different feature values. Options include Sum (the sum of all feature values), Min (the smallest feature value), Max (the largest feature value), Mean (the mean of all feature values), and WeightedMean (the weight average of all feature values)."},
        {"valid_strings", "['Sum','Min','Max','Mean','WeightedMean']"}
      },
      {
        {"name", "scan_polarity_merge_feature_name"},
        {"type", "string"},
        {"value", "peak_apex_int"},
        {"description", "The name of the FeatureMap attribute to use. Examples include peak_apex_int, peak_area, and intensity."}
      },
      {
        {"name", "mass_range_merge_feature_name"},
        {"type", "string"},
        {"value", "peak_apex_int"},
        {"description", "The name of the FeatureMap attribute to use. Examples include peak_apex_int, peak_area, and intensity."}
      },
      {
        {"name", "dilution_series_merge_feature_name"},
        {"type", "string"},
        {"value", "peak_apex_int"},
        {"description", "The name of the FeatureMap attribute to use. Examples include peak_apex_int, peak_area, and intensity."}
      },
      {
        {"name", "select_preferred_dilutions"},
        {"type", "bool"},
        {"value", "false"},
        {"description", "Select features from dilution preferences."}
      },
      {
        {"name", "select_preferred_dilutions_file"},
        {"type", "string"},
        {"value", "selectDilutions.csv"},
        {"description", "File listing preferred dilutions. csv file, with component_name and dilution_factor columns."}
      },
      {
        {"name", "merge_subordinates"},
        {"type", "bool"},
        {"value", "true"},
        {"description", "Merge subordinates"}
      },
    }} });
    return ParameterSet(param_struct);
  }

  void MergeInjections::process(
    SampleGroupHandler& sampleGroupHandler_IO,
    const SequenceHandler& sequenceHandler_I,
    const ParameterSet& params_I,
    Filenames& filenames_I
  ) const
  {
    LOGD << "START MergeInjections";
    getFilenames(filenames_I);

    // Complete user parameters with schema
    ParameterSet params(params_I);
    params.merge(getParameterSchema());

    // Extract out the parameters
    std::string scan_polarity_merge_rule; // Sum, Min, Max, Mean, WeightedMean
    std::string mass_range_merge_rule; // Sum, Min, Max, Mean, WeightedMean
    std::string dilution_series_merge_rule; // Sum, Min, Max, Mean, WeightedMean
    std::string scan_polarity_merge_feature_name; // The name of the feature to base the merge on in the case of max and min, and the feature to base the weighting on in the case of WeightedMean
    std::string mass_range_merge_feature_name;
    std::string dilution_series_merge_feature_name;
    bool merge_subordinates = true; // whether to merge at the feature level or at the subordinate level
    for (const auto& mi_params : params.at("MergeInjections")) {
      if (mi_params.getName() == "scan_polarity_merge_rule") {
        if (mi_params.getValueAsString() == "Sum" || mi_params.getValueAsString() == "Min" || mi_params.getValueAsString() == "Max" || mi_params.getValueAsString() == "Mean" || mi_params.getValueAsString() == "WeightedMean") {
          scan_polarity_merge_rule = mi_params.getValueAsString();
        }
        else 
        {
          LOGD << "Incorrect value for 'scan_polarity_merge_rule'.  Options are 'Sum', 'Min', 'Max', 'Mean', or 'WeightedMean'.";
        }
      }
      if (mi_params.getName() == "mass_range_merge_rule") {
        if (mi_params.getValueAsString() == "Sum" || mi_params.getValueAsString() == "Min" || mi_params.getValueAsString() == "Max" || mi_params.getValueAsString() == "Mean" || mi_params.getValueAsString() == "WeightedMean") {
          mass_range_merge_rule = mi_params.getValueAsString();
        }
        else
        {
          LOGD << "Incorrect value for 'mass_range_merge_rule'.  Options are 'Sum', 'Min', 'Max', 'Mean', or 'WeightedMean'.";
        }
      }
      if (mi_params.getName() == "dilution_series_merge_rule") {
        if (mi_params.getValueAsString() == "Sum" || mi_params.getValueAsString() == "Min" || mi_params.getValueAsString() == "Max" || mi_params.getValueAsString() == "Mean" || mi_params.getValueAsString() == "WeightedMean") {
          dilution_series_merge_rule = mi_params.getValueAsString();
        }
        else
        {
          LOGD << "Incorrect value for 'dilution_series_merge_rule'.  Options are 'Sum', 'Min', 'Max', 'Mean', or 'WeightedMean'.";
        }
      }
      if (mi_params.getName() == "scan_polarity_merge_feature_name") {
        scan_polarity_merge_feature_name = mi_params.getValueAsString();
      }
      if (mi_params.getName() == "mass_range_merge_feature_name") {
        mass_range_merge_feature_name = mi_params.getValueAsString();
      }
      if (mi_params.getName() == "dilution_series_merge_feature_name") {
        dilution_series_merge_feature_name = mi_params.getValueAsString();
      }
      if (mi_params.getName() == "merge_subordinates") {
        try {
          std::string value = mi_params.getValueAsString();
          std::transform(value.begin(), value.end(), value.begin(), ::tolower);
          merge_subordinates = (value == "true") ? true : false;
        }
        catch (const std::exception& e) {
          LOGE << e.what();
        }
      }
    }
    if (scan_polarity_merge_rule.empty() || mass_range_merge_rule.empty() || dilution_series_merge_rule.empty() ||
      scan_polarity_merge_feature_name.empty() || mass_range_merge_feature_name.empty() || dilution_series_merge_feature_name.empty()) {
      LOGE << "Missing parameters for MergeInjections. Not merging.";
      LOGD << "END MergeInjections";
      return;
    }

    // Get the injection names for all the different scan polarities, mass ranges and dilutions
    std::set<std::string> scan_polarities;
    std::set<std::pair<float, float>> scan_mass_ranges;
    std::set<float> dilution_factors;
    std::map<mergeKeyType, std::vector<std::set<std::string>>> merge_keys_to_injection_name;
    getMergeKeysToInjections(sampleGroupHandler_IO, sequenceHandler_I, scan_polarities, scan_mass_ranges, dilution_factors, merge_keys_to_injection_name);

    // Determine the ordering of merge
    orderMergeKeysToInjections(scan_polarities, scan_mass_ranges, dilution_factors, merge_keys_to_injection_name);

    // Organize the injection `FeatureMaps` into a map of std::pair<PeptideRef, native_id> with a value of
    // a map of `feature_name` with a value of
    // a map of a set of injection names with a value of the feature metadata value
    std::map<componentKeyType, std::map<std::string, std::map<std::set<std::string>, float>>> component_to_feature_to_injection_to_values;
    getComponentsToFeaturesToInjectionsToValues(sampleGroupHandler_IO, sequenceHandler_I, merge_subordinates, component_to_feature_to_injection_to_values);

    // Select preferred dilution
    if (!selectDilutions(params, filenames_I, sequenceHandler_I, component_to_feature_to_injection_to_values))
    {
      return;
    }

    // Merge the components and features in order and in place
    // pass 1: dilutions
    std::set<std::string> scan_polarities_keys_tup = scan_polarities;
    std::set<std::pair<float, float>> scan_mass_ranges_keys_tup = scan_mass_ranges;
    std::set<float> dilution_factors_keys_tup({-1});
    mergeComponentsToFeaturesToInjectionsToValues(dilution_series_merge_feature_name,
                                                  dilution_series_merge_rule, 
                                                  scan_polarities_keys_tup,
                                                  scan_mass_ranges_keys_tup,
                                                  dilution_factors_keys_tup, 
                                                  merge_keys_to_injection_name,
                                                  component_to_feature_to_injection_to_values);

    // pass 2: mass ranges
    scan_mass_ranges_keys_tup = std::set<std::pair<float, float>>({std::make_pair(-1,-1)});
    mergeComponentsToFeaturesToInjectionsToValues(scan_polarity_merge_feature_name,
                                                  mass_range_merge_rule,
                                                  scan_polarities_keys_tup,
                                                  scan_mass_ranges_keys_tup,
                                                  dilution_factors_keys_tup,
                                                  merge_keys_to_injection_name,
                                                  component_to_feature_to_injection_to_values);

    // pass 3: scan polarities
    scan_polarities_keys_tup = std::set<std::string>({ "" });
    mergeComponentsToFeaturesToInjectionsToValues(scan_polarity_merge_feature_name,
                                                  scan_polarity_merge_rule,
                                                  scan_polarities_keys_tup,
                                                  scan_mass_ranges_keys_tup,
                                                  dilution_factors_keys_tup,
                                                  merge_keys_to_injection_name,
                                                  component_to_feature_to_injection_to_values);

    // Make the final merged feature
    std::set<std::string> injection_names_set;
    for (const std::size_t& index : sampleGroupHandler_IO.getSampleIndices()) {
      injection_names_set.insert(sequenceHandler_I.getSequence().at(index).getMetaData().getInjectionName() );
    }
    OpenMS::FeatureMap fmap;
    makeFeatureMap(merge_subordinates, injection_names_set, component_to_feature_to_injection_to_values, fmap);

    sampleGroupHandler_IO.setFeatureMap(fmap);

    LOGI << "MergeInjections output size: " << fmap.size();
    LOGD << "END MergeInjections";
  }

  bool MergeInjections::selectDilutions(
    const ParameterSet& params,
    const Filenames& filenames_I,
    const SequenceHandler& sequenceHandler_I,
    std::map<componentKeyType, std::map<std::string, std::map<std::set<std::string>, float>>>& component_to_feature_to_injection_to_values)
  {
    if (params.at("MergeInjections").findParameter("select_preferred_dilutions")->getValueAsString() == "true")
    {
      std::map<std::string, int> select_dilution_map;
      std::filesystem::path dilution_file = params.at("MergeInjections").findParameter("select_preferred_dilutions_file")->getValueAsString();
      if (dilution_file.is_relative())
      {
        dilution_file = (std::filesystem::path(filenames_I.getTag(Filenames::Tag::MAIN_DIR)) / dilution_file).lexically_normal();
      }
      try
      {
        SelectDilutionsParser::read(dilution_file.generic_string(), select_dilution_map);
      }
      catch (const std::exception& e)
      {
        LOGE << "Failed to read select dilutions file [" << dilution_file.generic_string() << "] : " << e.what();
        return false;
      }
      std::map<componentKeyType, std::map<std::string, std::map<std::set<std::string>, float>>> new_component_to_feature_to_injection_to_values;
      for (const auto& component_to_feature_to_injection_to_value : component_to_feature_to_injection_to_values)
      {
        const auto& component_key = component_to_feature_to_injection_to_value.first;
        const auto& component_name = component_key.second;
        if (select_dilution_map.count(component_name))
        {
          float preferred_dilution = select_dilution_map.at(component_name);
          const auto& feature_map = component_to_feature_to_injection_to_value.second;
          // 1st pass: get selected injections
          std::set<std::set<std::string>> selected_injections;
          for (const auto& feature : feature_map)
          {
            const auto& feature_name = feature.first;
            const auto& injections_set_to_value = feature.second;
            for (const auto& injections_set : injections_set_to_value)
            {
              std::set<std::string> selected_injections_set;
              for (const auto& injection_name : injections_set.first)
              {
                // look for dilution of this injection
                for (const auto& sample : sequenceHandler_I.getSequence())
                {
                  if (sample.getMetaData().getInjectionName() == injection_name)
                  {
                    float dilution = sample.getMetaData().dilution_factor;
                    if (std::abs(dilution - preferred_dilution) < 1e-6)
                    {
                      selected_injections_set.insert(injection_name);
                    }
                  }
                }
              }
              if (!selected_injections_set.empty())
              {
                selected_injections.insert(selected_injections_set);
              }
            }
          }
          // 2nd pass: reconstruct the injection map
          std::map<std::string, std::map<std::set<std::string>, float>> new_feature_map;
          if (selected_injections.empty())
          {
            // if we haven't found preferred injection among this map, we just let it as it was.
            new_feature_map = feature_map;
          }
          else
          {
            for (const auto& feature : feature_map)
            {
              const auto& injections_set_to_value = feature.second;
              std::map<std::set<std::string>, float> new_injections_set_to_value;
              for (const auto& injections_set : injections_set_to_value)
              {
                if (selected_injections.count(injections_set.first))
                {
                  new_injections_set_to_value.insert(injections_set);
                }
              }
              new_feature_map.emplace(feature.first, new_injections_set_to_value);
            }
          }
          new_component_to_feature_to_injection_to_values.emplace(component_key, new_feature_map);
        }
        else
        {
          new_component_to_feature_to_injection_to_values.insert(component_to_feature_to_injection_to_value);
        }
      }
      component_to_feature_to_injection_to_values = std::move(new_component_to_feature_to_injection_to_values);
    }
    return true;
  }

  void MergeInjections::getMergeKeysToInjections(const SampleGroupHandler& sampleGroupHandler_IO,
                                                 const SequenceHandler& sequenceHandler_I,
                                                 std::set<std::string>& scan_polarities,
                                                 std::set<std::pair<float, float>>& scan_mass_ranges,
                                                 std::set<float>& dilution_factors,
                                                 std::map<mergeKeyType, std::vector<std::set<std::string>>>& merge_keys_to_injection_name)
  {
    for (const std::size_t& index : sampleGroupHandler_IO.getSampleIndices())
    {
      const auto& injection_metadata = sequenceHandler_I.getSequence().at(index).getMetaData();
      const auto key = std::make_tuple(injection_metadata.scan_polarity,
                                       std::make_pair(injection_metadata.scan_mass_low, injection_metadata.scan_mass_high),
                                       injection_metadata.dilution_factor);
      merge_keys_to_injection_name.emplace(key, std::vector<std::set<std::string>>());
      std::set<std::string> value({ injection_metadata.getInjectionName() });
      merge_keys_to_injection_name.at(key).push_back(value);
      scan_polarities.insert(injection_metadata.scan_polarity);
      scan_mass_ranges.insert(std::make_pair(injection_metadata.scan_mass_low, injection_metadata.scan_mass_high));
      dilution_factors.insert(injection_metadata.dilution_factor);
    }
  }
  void MergeInjections::orderMergeKeysToInjections(std::set<std::string>& scan_polarities,
                                                   const std::set<std::pair<float, float>>& scan_mass_ranges,
                                                   const std::set<float>& dilution_factors,
                                                   std::map<mergeKeyType, std::vector<std::set<std::string>>>& merge_keys_to_injection_name)
  {
    // pass 1: dilutions
    for (const auto& scan_polarity : scan_polarities) {
      for (const auto& scan_mass_range : scan_mass_ranges) {
        const auto key = std::make_tuple(scan_polarity, scan_mass_range, -1);
        merge_keys_to_injection_name.emplace(key, std::vector<std::set<std::string>>());
        for (const auto& dilution_factor : dilution_factors) {
          const auto key_tmp = std::make_tuple(scan_polarity, scan_mass_range, dilution_factor);
          if (merge_keys_to_injection_name.count(key_tmp) <= 0) continue;
          for (const auto& injection_name_set : merge_keys_to_injection_name.at(key_tmp)) {
            merge_keys_to_injection_name.at(key).push_back(injection_name_set);
          }
        }
      }
    }

    // pass 2: mass ranges
    for (const auto& scan_polarity : scan_polarities) {
      const auto key = std::make_tuple(scan_polarity, std::make_pair(-1, -1), -1);
      merge_keys_to_injection_name.emplace(key, std::vector<std::set<std::string>>());
      for (const auto& scan_mass_range : scan_mass_ranges) {
        const auto key_tmp = std::make_tuple(scan_polarity, scan_mass_range, -1);
        if (merge_keys_to_injection_name.count(key_tmp) <= 0) continue;
        // merge the injection name sets from the previous merge
        std::set<std::string> injection_names_prev_merge;
        for (const auto& injection_name_set : merge_keys_to_injection_name.at(key_tmp)) {
          for (const auto& injection_name : injection_name_set) {
            injection_names_prev_merge.insert(injection_name);
          }
        }
        // add the new injections set to the merge list
        merge_keys_to_injection_name.at(key).push_back(injection_names_prev_merge);
      }
    }

    // pass 3: scan polarities
    const auto key = std::make_tuple("", std::make_pair(-1, -1), -1);
    merge_keys_to_injection_name.emplace(key, std::vector<std::set<std::string>>());
    for (const auto& scan_polarity : scan_polarities) {
      const auto key_tmp = std::make_tuple(scan_polarity, std::make_pair(-1, -1), -1);
      if (merge_keys_to_injection_name.count(key_tmp) <= 0) continue;
      // merge the injection name sets from the previous merge
      std::set<std::string> injection_names_prev_merge;
      for (const auto& injection_name_set : merge_keys_to_injection_name.at(key_tmp)) {
        for (const auto& injection_name : injection_name_set) {
          injection_names_prev_merge.insert(injection_name);
        }
      }
      // add the new injections set to the merge list
      merge_keys_to_injection_name.at(key).push_back(injection_names_prev_merge);
    }
  }

  void MergeInjections::getComponentsToFeaturesToInjectionsToValues(const SampleGroupHandler& sampleGroupHandler_IO,
                                                                    const SequenceHandler& sequenceHandler_I,
                                                                    const bool& merge_subordinates,
                                                                    std::map<componentKeyType, std::map<std::string, std::map<std::set<std::string>, float>>>& component_to_feature_to_injection_to_values)
  {
    // initialize our map of feature names to values
    std::map<std::string, std::map<std::set<std::string>, float>> features_to_values;
    for (const auto& m : metadataFloatToString) {
      features_to_values.emplace(m.second, std::map<std::set<std::string>, float>());
    }

    // construct the map
    for (const std::size_t& index : sampleGroupHandler_IO.getSampleIndices()) {
      const OpenMS::FeatureMap& fmap = sequenceHandler_I.getSequence().at(index).getRawData().getFeatureMap();
      const std::string injection_name = sequenceHandler_I.getSequence().at(index).getMetaData().getInjectionName();

      // Get the features/subordinates
      OpenMS::FeatureMap fmap_tmp;
      for (const OpenMS::Feature& f : fmap) {
        // Feature level merge
        if (!merge_subordinates) {
          componentKeyType component(f.getMetaValue("PeptideRef").toString(), "");
          component_to_feature_to_injection_to_values.emplace(component, features_to_values);
          for (const auto& f_to_v : features_to_values) {
            const CastValue& datum = SequenceHandler::getMetaValue(f, f, f_to_v.first);
            if (datum.getTag() == CastValue::Type::UNINITIALIZED || datum.getTag() == CastValue::Type::STRING) {
              //LOGD << "Feature name: " << f_to_v.first << " was not found in the FeatureMap."; // This will polute the log
              continue;
            }
            component_to_feature_to_injection_to_values.at(component).at(f_to_v.first).emplace(std::set<std::string>({ injection_name }), datum.f_);
          }
        }
        // Subordinate level merge
        else if (merge_subordinates) 
        {
          for (const OpenMS::Feature& s : f.getSubordinates()) {
            componentKeyType component(f.getMetaValue("PeptideRef").toString(), s.getMetaValue("native_id").toString());
            component_to_feature_to_injection_to_values.emplace(component, features_to_values);
            for (const auto& f_to_v : features_to_values) {
              const CastValue& datum = SequenceHandler::getMetaValue(s, s, f_to_v.first);
              if (datum.getTag() == CastValue::Type::UNINITIALIZED || datum.getTag() == CastValue::Type::STRING) {
                //LOGD << "Feature name: " << f_to_v.first << " was not found in the FeatureMap."; // This will polute the log
                continue;
              }
              component_to_feature_to_injection_to_values.at(component).at(f_to_v.first).emplace(std::set<std::string>({ injection_name }), datum.f_);
            }
          }
        }
      }
    }
  }
  void MergeInjections::mergeComponentsToFeaturesToInjectionsToValues(const std::string& feature_name,
                                                                      const std::string& merge_rule,
                                                                      const std::set<std::string>& scan_polarities,
                                                                      const std::set<std::pair<float, float>>& scan_mass_ranges,
                                                                      const std::set<float>& dilution_factors,
                                                                      const std::map<mergeKeyType, std::vector<std::set<std::string>>>& merge_keys_to_injection_names,
                                                                      std::map<componentKeyType, std::map<std::string, std::map<std::set<std::string>, float>>>& component_to_feature_to_injection_to_values)
  {
    for (const auto& scan_polarity : scan_polarities) {
      for (const auto& scan_mass_range : scan_mass_ranges) {
        for (const auto& dilution_factor : dilution_factors) {
          const auto key = std::make_tuple(scan_polarity, scan_mass_range, dilution_factor);
          for (auto& component_to_feature_to_injection_to_value : component_to_feature_to_injection_to_values) {
            if (component_to_feature_to_injection_to_value.second.count(feature_name) <= 0) {
              //LOGD << "Feature name: " << feature_name << " was not found in the FeatureMap."; // This will polute the log
              continue;
            }
            if (merge_keys_to_injection_names.count(key) <= 0) continue;

            // Find the total value for weighting
            float total_value = 0;
            for (const std::set<std::string>& injection_names_set : merge_keys_to_injection_names.at(key)) {
              if (component_to_feature_to_injection_to_value.second.at(feature_name).count(injection_names_set) <= 0) continue;
              total_value += component_to_feature_to_injection_to_value.second.at(feature_name).at(injection_names_set);
            }

            // Calculated the weights and representative merged value
            float merged_value = 0;
            std::vector<float> weights;
            std::set<std::string> injections;
            std::set<std::string> max_or_min_injections;
            int cnt = 0;
            for (const std::set<std::string>& injection_names_set : merge_keys_to_injection_names.at(key)) {

              // record the injections
              // Note: this is done before checking if the feature exists to ensure that all injections are propogated to the next iteration
              for (const std::string& inj : injection_names_set) {
                injections.insert(inj);
              }

              // check if the feature is in the FeatureMap
              if (component_to_feature_to_injection_to_value.second.at(feature_name).count(injection_names_set) <= 0) continue;
              float value = component_to_feature_to_injection_to_value.second.at(feature_name).at(injection_names_set);
              weights.push_back(value / total_value); // add to the weights

              // initializations
              if (cnt == 0) 
              {
                max_or_min_injections = injection_names_set;
                if (merge_rule == "Min" || merge_rule == "Max") 
                {
                  merged_value = value;
                }
              }

              // calculations
              if (merge_rule == "Sum") 
              {
                merged_value += value;
              }
              else if (merge_rule == "Min") 
              {
                if (value < merged_value) {
                  merged_value = value;
                  max_or_min_injections = injection_names_set;
                }
              }
              else if (merge_rule == "Max") 
              {
                if (value > merged_value) {
                  merged_value = value;
                  max_or_min_injections = injection_names_set;
                }
              }
              else if (merge_rule == "Mean") 
              {
                merged_value += (value / merge_keys_to_injection_names.at(key).size());
              }
              else if (merge_rule == "WeightedMean") 
              {
                merged_value += (value * value / total_value);
              }
              ++cnt;
            }

            // Make the merged feature
            if (weights.size() <= 0) continue; // Note: we use the weights to check instead of the injections as the weights will be empty if no features exist for any of the injections
            component_to_feature_to_injection_to_value.second.at(feature_name).insert_or_assign(injections, merged_value);

            // Repeat for all other features
            for (auto& feature_to_injection_to_value : component_to_feature_to_injection_to_value.second) {
              if (feature_name == feature_to_injection_to_value.first) continue;
              if (feature_to_injection_to_value.second.count(max_or_min_injections) <= 0) continue;

              // Calculated the merged value
              float merged_value = 0;
              if (merge_rule == "Min" || merge_rule == "Max") {
                merged_value = feature_to_injection_to_value.second.at(max_or_min_injections);
              }
              else 
              {
                int cnt = 0;
                for (const std::set<std::string>& injection_names_set : merge_keys_to_injection_names.at(key)) {
                  if (feature_to_injection_to_value.second.count(injection_names_set) <= 0) continue;
                  float value = feature_to_injection_to_value.second.at(injection_names_set);
                  if (merge_rule == "Sum") merged_value += value;
                  else if (merge_rule == "Mean") merged_value += (value / weights.size());
                  else if (merge_rule == "WeightedMean") merged_value += (value * weights.at(cnt));
                  ++cnt;
                }
              }

              // Make the merged feature
              feature_to_injection_to_value.second.insert_or_assign(injections, merged_value);
            }
          }
        }
      }
    }
  }
  void MergeInjections::makeFeatureMap(const bool& merge_subordinates,
                                       std::set<std::string>& injection_names_set,
                                       const std::map<componentKeyType, std::map<std::string, std::map<std::set<std::string>, float>>>& component_to_feature_to_injection_to_values,
                                       OpenMS::FeatureMap& feature_map)
  {
    OpenMS::Feature f, s;
    std::vector<OpenMS::Feature> subs;
    std::string peptide_ref_cur = "";
    for (const auto& component_to_feature_to_injection_to_value : component_to_feature_to_injection_to_values) {

      // Decide whether to make a new feature or continue adding subordinates
      if (merge_subordinates) {
        subs.push_back(s);
        s = OpenMS::Feature();
        s.setUniqueId();
        s.setMetaValue("native_id", component_to_feature_to_injection_to_value.first.second);
      }
      if (component_to_feature_to_injection_to_value.first.first != peptide_ref_cur) {
        if (merge_subordinates) {
          f.setSubordinates(subs);
          subs.clear();
        }
        else 
        {
          s = f; // Ensure all features have a subordinate for downstream filtering and quantitation
          s.setMetaValue("native_id", f.getMetaValue("PeptideRef"));
          subs.push_back(s);
          f.setSubordinates(subs);
          subs.clear();
        }
        feature_map.push_back(f);
        f = OpenMS::Feature();
        f.setUniqueId();
        f.setMetaValue("PeptideRef", component_to_feature_to_injection_to_value.first.first);
        peptide_ref_cur = component_to_feature_to_injection_to_value.first.first;              
      }

      // Get the metadata
      for (const auto& feature_to_injection_to_value : component_to_feature_to_injection_to_value.second) {
        if (feature_to_injection_to_value.second.count(injection_names_set) <= 0) continue;
        float value = feature_to_injection_to_value.second.at(injection_names_set);
        if (!merge_subordinates) {
          if (feature_to_injection_to_value.first == "RT") 
          {
            f.setRT(value);
          }
          else if (feature_to_injection_to_value.first == "Intensity") 
          {
            f.setIntensity(value);
          }
          else if (feature_to_injection_to_value.first == "peak_area") 
          {
            f.setIntensity(value);
          }
          else if (feature_to_injection_to_value.first == "mz") 
          {
            f.setMZ(value);
          }
          else if (feature_to_injection_to_value.first == "charge") 
          {
            f.setCharge(static_cast<int>(value));
          }
          else 
          {
            f.setMetaValue(feature_to_injection_to_value.first, value);
          }
        }
        else if (merge_subordinates) 
        {
          if (feature_to_injection_to_value.first == "RT") 
          {
            s.setRT(value);
          }
          else if (feature_to_injection_to_value.first == "Intensity") 
          {
            s.setIntensity(value);
          }
          else if (feature_to_injection_to_value.first == "peak_area") 
          {
            s.setIntensity(value);
          }
          else if (feature_to_injection_to_value.first == "mz") 
          {
            s.setMZ(value);
          }
          else if (feature_to_injection_to_value.first == "charge") 
          {
            s.setCharge(static_cast<int>(value));
          }
          else 
          {
            s.setMetaValue(feature_to_injection_to_value.first, value);
          }
        }             
      }
    }

    // Add in the last feature
    if (merge_subordinates) {
      subs.push_back(s);
      f.setSubordinates(subs);
    }
    feature_map.push_back(f);

    // Remove the first feature
    feature_map.erase(feature_map.begin());
  }


  ParameterSet LoadFeaturesSampleGroup::getParameterSchema() const
  {
    return ParameterSet();
  }

  void LoadFeaturesSampleGroup::getFilenames(Filenames& filenames) const
  {
    filenames.addFileName("featureXMLSampleGroup_i", "${FEATURES_INPUT_PATH}/${INPUT_GROUP_NAME}.featureXML");
  };

  void LoadFeaturesSampleGroup::process(SampleGroupHandler& sampleGroupHandler_IO,
    const SequenceHandler& sequenceHandler_I,
    const ParameterSet& params_I,
    Filenames& filenames_I
  ) const
  {
    LOGD << "START LoadFeaturesSampleGroup";
    getFilenames(filenames_I);

    if (!InputDataValidation::prepareToLoad(filenames_I, "featureXMLSampleGroup_i"))
    {
      LOGD << "END " << getName();
      return;
    }

    try {
      OpenMS::FeatureXMLFile featurexml;
      featurexml.load(filenames_I.getFullPath("featureXMLSampleGroup_i").generic_string(), sampleGroupHandler_IO.getFeatureMap());
    }
    catch (const std::exception& e) {
      LOGE << e.what();
      sampleGroupHandler_IO.getFeatureMap().clear();
      LOGE << "feature map clear";
    }

    LOGD << "END LoadFeaturesSampleGroup";
  }

  ParameterSet StoreFeaturesSampleGroup::getParameterSchema() const
  {
    return ParameterSet();
  }

  void StoreFeaturesSampleGroup::getFilenames(Filenames& filenames) const
  {
    filenames.addFileName("featureXMLSampleGroup_o", "${FEATURES_OUTPUT_PATH}/${OUTPUT_GROUP_NAME}.featureXML");
  };

  void StoreFeaturesSampleGroup::process(SampleGroupHandler& sampleGroupHandler_IO,
    const SequenceHandler& sequenceHandler_I,
    const ParameterSet& params_I,
    Filenames& filenames_I
  ) const
  {
    LOGD << "START storeFeaturesSampleGroup";
    getFilenames(filenames_I);

    if (!InputDataValidation::prepareToStore(filenames_I, "featureXMLSampleGroup_o"))
    {
      LOGD << "END " << getName();
      return;
    }

    try {
      // Store outfile as featureXML
      OpenMS::FeatureXMLFile featurexml;
      featurexml.store(filenames_I.getFullPath("featureXMLSampleGroup_o").generic_string(), sampleGroupHandler_IO.getFeatureMap());
    }
    catch (const std::exception& e) {
      LOGE << e.what();
    }

    LOGD << "END storeFeaturesSampleGroup";
  }
}
