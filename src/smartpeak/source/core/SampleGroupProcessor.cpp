// TODO: Add copyright

#include <SmartPeak/core/SampleGroupProcessor.h>
#include <SmartPeak/core/SequenceHandler.h>
#include <SmartPeak/core/FeatureMetadata.h>
#include <plog/Log.h>

namespace SmartPeak
{
  void MergeInjections::process(
    SampleGroupHandler& sampleGroupHandler_IO,
    const SequenceHandler& sequenceHandler_I,
    const std::map<std::string, std::vector<std::map<std::string, std::string>>>& params_I,
    const Filenames& filenames
  ) const
  {
    LOGD << "START MergeInjections";

    // Check the parameters
    if (params_I.at("MergeInjections").empty() && params_I.at("MergeInjections").empty()) {
      LOGE << "Parameters not found for MergeInjections. Returning";
      LOGD << "END MergeInjections";
      return;
    }

    // Extract out the parameters
    std::string scan_polarity_merge_rule; // Sum, Min, Max, Mean, WeightedMean
    std::string mass_range_merge_rule; // Sum, Min, Max, Mean, WeightedMean
    std::string dilution_series_merge_rule; // Sum, Min, Max, Mean, WeightedMean
    std::string scan_polarity_merge_feature_name; // The name of the feature to base the merge on in the case of max and min, and the feature to base the weighting on in the case of WeightedMean
    std::string mass_range_merge_feature_name;
    std::string dilution_series_merge_feature_name;
    bool merge_subordinates = true; // whether to merge at the feature level or at the subordinate level
    for (const auto& mi_params : params_I.at("MergeInjections")) {
      if (mi_params.at("name") == "scan_polarity_merge_rule") {
        if (mi_params.at("value") == "Sum" || mi_params.at("value") == "Min" || mi_params.at("value") == "Max" || mi_params.at("value") == "Mean" || mi_params.at("value") == "WeightedMean") {
          scan_polarity_merge_rule = mi_params.at("value");
        }
        else {
          LOGD << "Incorrect value for 'scan_polarity_merge_rule'.  Options are 'Sum', 'Min', 'Max', 'Mean', or 'WeightedMean'.";
        }
      }
      if (mi_params.at("name") == "mass_range_merge_rule") {
        if (mi_params.at("value") == "Sum" || mi_params.at("value") == "Min" || mi_params.at("value") == "Max" || mi_params.at("value") == "Mean" || mi_params.at("value") == "WeightedMean") {
          mass_range_merge_rule = mi_params.at("value");
        }
        else {
          LOGD << "Incorrect value for 'mass_range_merge_rule'.  Options are 'Sum', 'Min', 'Max', 'Mean', or 'WeightedMean'.";
        }
      }
      if (mi_params.at("name") == "dilution_series_merge_rule") {
        if (mi_params.at("value") == "Sum" || mi_params.at("value") == "Min" || mi_params.at("value") == "Max" || mi_params.at("value") == "Mean" || mi_params.at("value") == "WeightedMean") {
          dilution_series_merge_rule = mi_params.at("value");
        }
        else {
          LOGD << "Incorrect value for 'dilution_series_merge_rule'.  Options are 'Sum', 'Min', 'Max', 'Mean', or 'WeightedMean'.";
        }
      }
      if (mi_params.at("name") == "scan_polarity_merge_feature_name") {
        scan_polarity_merge_feature_name = mi_params.at("value");
      }
      if (mi_params.at("name") == "mass_range_merge_feature_name") {
        mass_range_merge_feature_name = mi_params.at("value");
      }
      if (mi_params.at("name") == "dilution_series_merge_feature_name") {
        dilution_series_merge_feature_name = mi_params.at("value");
      }
      if (mi_params.at("name") == "merge_subordinates") {
        try {
          std::string value = mi_params.at("value");
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
    std::map<std::tuple<std::string, std::pair<float, float>, float>, std::vector<std::set<std::string>>> merge_keys_to_injection_name;
    getMergeKeysToInjections(sampleGroupHandler_IO, sequenceHandler_I, scan_polarities, scan_mass_ranges, dilution_factors, merge_keys_to_injection_name);

    // Determine the ordering of merge
    orderMergeKeysToInjections(scan_polarities, scan_mass_ranges, dilution_factors, merge_keys_to_injection_name);

    // Organize the injection `FeatureMaps` into a map of std::pair<PeptideRef, native_id> with a value of
    // a map of `feature_name` with a value of
    // a map of a set of injection names with a value of the feature metadata value
    std::map<std::pair<std::string, std::string>, std::map<std::string, std::map<std::set<std::string>, float>>> component_to_feature_to_injection_to_values;
    getComponentsToFeaturesToInjectionsToValues(sampleGroupHandler_IO, sequenceHandler_I, merge_subordinates, component_to_feature_to_injection_to_values);
    
    // Merge the components and features in order and in place
    // pass 1: dilutions
    std::set<std::string> scan_polarities_keys_tup = scan_polarities;
    std::set<std::pair<float, float>> scan_mass_ranges_keys_tup = scan_mass_ranges;
    std::set<float> dilution_factors_keys_tup({-1});
    mergeComponentsToFeaturesToInjectionsToValues(dilution_series_merge_feature_name, dilution_series_merge_rule, 
      scan_polarities_keys_tup, scan_mass_ranges_keys_tup, dilution_factors_keys_tup, 
      merge_keys_to_injection_name, component_to_feature_to_injection_to_values);

    // pass 2: mass ranges
    scan_mass_ranges_keys_tup = std::set<std::pair<float, float>>({std::make_pair(-1,-1)});
    mergeComponentsToFeaturesToInjectionsToValues(dilution_series_merge_feature_name, dilution_series_merge_rule,
      scan_polarities_keys_tup, scan_mass_ranges_keys_tup, dilution_factors_keys_tup,
      merge_keys_to_injection_name, component_to_feature_to_injection_to_values);

    // pass 3: scan polarities
    scan_polarities_keys_tup = std::set<std::string>({ "" });
    mergeComponentsToFeaturesToInjectionsToValues(dilution_series_merge_feature_name, dilution_series_merge_rule,
      scan_polarities_keys_tup, scan_mass_ranges_keys_tup, dilution_factors_keys_tup,
      merge_keys_to_injection_name, component_to_feature_to_injection_to_values);

    // Make the final merged feature

    LOGD << "END optimizeCalibrationCurves";
  }
  void MergeInjections::getMergeKeysToInjections(const SampleGroupHandler& sampleGroupHandler_IO, const SequenceHandler& sequenceHandler_I, std::set<std::string>& scan_polarities, std::set<std::pair<float, float>>& scan_mass_ranges, std::set<float>& dilution_factors, std::map<std::tuple<std::string, std::pair<float, float>, float>, std::vector<std::set<std::string>>>& merge_keys_to_injection_name)
  {
    for (const std::size_t& index : sampleGroupHandler_IO.getSampleIndices()) {
      const auto key = std::make_tuple(sequenceHandler_I.getSequence().at(index).getMetaData().scan_polarity,
        std::make_pair(sequenceHandler_I.getSequence().at(index).getMetaData().scan_mass_low, sequenceHandler_I.getSequence().at(index).getMetaData().scan_mass_high),
        sequenceHandler_I.getSequence().at(index).getMetaData().dilution_factor);
      merge_keys_to_injection_name.emplace(key, std::vector<std::set<std::string>>());
      std::set<std::string> value({ sequenceHandler_I.getSequence().at(index).getMetaData().getInjectionName() });
      merge_keys_to_injection_name.at(key).push_back(value);
      scan_polarities.insert(sequenceHandler_I.getSequence().at(index).getMetaData().scan_polarity);
      scan_mass_ranges.insert(std::make_pair(sequenceHandler_I.getSequence().at(index).getMetaData().scan_mass_low, sequenceHandler_I.getSequence().at(index).getMetaData().scan_mass_high));
      dilution_factors.insert(sequenceHandler_I.getSequence().at(index).getMetaData().dilution_factor);
    }
  }
  void MergeInjections::orderMergeKeysToInjections(std::set<std::string>& scan_polarities, const std::set<std::pair<float, float>>& scan_mass_ranges, const std::set<float>& dilution_factors, std::map<std::tuple<std::string, std::pair<float, float>, float>, std::vector<std::set<std::string>>>& merge_keys_to_injection_name)
  {
    // pass 1: dilutions
    for (const auto& scan_polarity : scan_polarities) {
      for (const auto& scan_mass_range : scan_mass_ranges) {
        const auto key = std::make_tuple(scan_polarity, scan_mass_range, -1);
        merge_keys_to_injection_name.emplace(key, std::vector<std::set<std::string>>());
        for (const auto& dilution_factor : dilution_factors) {
          const auto key_tmp = std::make_tuple(scan_polarity, scan_mass_range, dilution_factor);
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
  void MergeInjections::getComponentsToFeaturesToInjectionsToValues(const SampleGroupHandler& sampleGroupHandler_IO, const SequenceHandler& sequenceHandler_I, const bool& merge_subordinates, std::map<std::pair<std::string, std::string>, std::map<std::string, std::map<std::set<std::string>, float>>>& component_to_feature_to_injection_to_values)
  {
    // initialize our map of feature names to values
    std::map<std::string, std::map<std::set<std::string>, float>> features_to_values;
    for (const auto& m : metadatafloatToString) {
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
          std::pair<std::string, std::string> component(f.getMetaValue("PeptideRef").toString(), "");
          auto found = component_to_feature_to_injection_to_values.emplace(component, features_to_values);
          for (const auto& f_to_v : features_to_values) {
            CastValue datum = SequenceHandler::getMetaValue(f, f, f_to_v.first);
            component_to_feature_to_injection_to_values.at(component).at(f_to_v.first).emplace(std::set<std::string>({ injection_name }), datum.f_);
          }
        }
        // Subordinate level merge
        else if (merge_subordinates) {
          for (const OpenMS::Feature& s : f.getSubordinates()) {
            std::pair<std::string, std::string> component(f.getMetaValue("PeptideRef").toString(), s.getMetaValue("native_id").toString());
            auto found = component_to_feature_to_injection_to_values.emplace(component, features_to_values);
            for (const auto& f_to_v : features_to_values) {
              CastValue datum = SequenceHandler::getMetaValue(s, s, f_to_v.first);
              component_to_feature_to_injection_to_values.at(component).at(f_to_v.first).emplace(std::set<std::string>({ injection_name }), datum.f_);
            }
          }
        }
      }
    }
  }
  void MergeInjections::mergeComponentsToFeaturesToInjectionsToValues(const std::string& feature_name, const std::string& merge_rule, std::set<std::string>& scan_polarities, const std::set<std::pair<float, float>>& scan_mass_ranges, const std::set<float>& dilution_factors, const std::map<std::tuple<std::string, std::pair<float, float>, float>, std::vector<std::set<std::string>>>& merge_keys_to_injection_name, std::map<std::pair<std::string, std::string>, std::map<std::string, std::map<std::set<std::string>, float>>>& component_to_feature_to_injection_to_values)
  {
    for (const auto& scan_polarity : scan_polarities) {
      for (const auto& scan_mass_range : scan_mass_ranges) {
        for (const auto& dilution_factor : dilution_factors) {
          const auto key = std::make_tuple(scan_polarity, scan_mass_range, dilution_factor);

          for (auto& component_to_feature_to_injection_to_value : component_to_feature_to_injection_to_values) {

            // Find the total value for weighting
            float total_value = 0;
            for (const std::set<std::string>& injection_names_set : merge_keys_to_injection_name.at(key)) {
              total_value += component_to_feature_to_injection_to_value.second.at(feature_name).at(injection_names_set);
            }

            // Calculated the weights and representative merged value
            float merged_value = 0;
            std::vector<float> weights;
            std::set<std::string> injections;
            std::set<std::string> max_or_min_injections;
            int cnt = 0;
            for (const std::set<std::string>& injection_names_set : merge_keys_to_injection_name.at(key)) {
              float value = component_to_feature_to_injection_to_value.second.at(feature_name).at(injection_names_set);
              weights.push_back(value / total_value);
              for (const std::string& inj : injection_names_set) {
                injections.insert(inj);
              }
              if (cnt == 0) {
                merged_value = value;
                max_or_min_injections = injection_names_set;
              }
              else {
                if (merge_rule == "Sum") {
                  merged_value += value;
                }
                else if (merge_rule == "Min") {
                  if (value < merged_value) {
                    merged_value = value;
                    max_or_min_injections = injection_names_set;
                  }
                }
                else if (merge_rule == "Max") {
                  if (value > merged_value) {
                    merged_value = value;
                    max_or_min_injections = injection_names_set;
                  }
                }
                else if (merge_rule == "Mean") {
                  merged_value += value / merge_keys_to_injection_name.at(key).size();
                }
                else if (merge_rule == "WeightedMean") {
                  merged_value += value / total_value;
                }
              }
              ++cnt;
            }

            // Make the merged feature
            component_to_feature_to_injection_to_value.second.at(feature_name).emplace(injections, 0);
            component_to_feature_to_injection_to_value.second.at(feature_name).at(injections) = merged_value;

            // Repeat for all other features
            for (auto& feature_to_injection_to_value : component_to_feature_to_injection_to_value.second) {
              if (feature_name == feature_to_injection_to_value.first) continue;

              // Calculated the merged value
              float merged_value = 0;
              if (merge_rule == "Min" || merge_rule == "Max") {
                merged_value = feature_to_injection_to_value.second.at(max_or_min_injections);
              }
              else {
                int cnt = 0;
                for (const std::set<std::string>& injection_names_set : merge_keys_to_injection_name.at(key)) {
                  float value = feature_to_injection_to_value.second.at(injection_names_set);
                  if (cnt == 0) {
                    merged_value = value;
                  }
                  else {
                    if (merge_rule == "Sum") merged_value += value;
                    else if (merge_rule == "Mean") merged_value += value / weights.size();
                    else if (merge_rule == "WeightedMean") merged_value += value * weights.at(cnt);
                  }
                  ++cnt;
                }
              }

              // Make the merged feature
              feature_to_injection_to_value.second.emplace(injections, 0);
              feature_to_injection_to_value.second.at(injections) = merged_value;
            }
          }
        }
      }
    }
  }
}
