// TODO: Add copyright

#include <SmartPeak/core/SampleGroupProcessor.h>
#include <SmartPeak/core/SequenceHandler.h>
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
    std::string feature_name; // The name of the feature to use
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
      if (mi_params.at("name") == "feature_name") {
        feature_name = mi_params.at("value");
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
    if (scan_polarity_merge_rule.empty() || mass_range_merge_rule.empty() || dilution_series_merge_rule.empty() || feature_name.empty()) {
      LOGE << "Missing parameters for MergeInjections. Not merging.";
      LOGD << "END MergeInjections";
      return;
    }

    // Step 1: scan polarities
    // Check for multiple scan polarities
    std::set<std::string> scan_polarities;
    for (const std::size_t& index : sampleGroupHandler_IO.getSampleIndices()) {
      scan_polarities.insert(sequenceHandler_I.getSequence().at(index).getMetaData().scan_polarity);
    }

    if (scan_polarities.size() > 1) {
      // Get the different scan polarity fmaps
      std::vector<OpenMS::FeatureMap> mixed_scan_polarity_fmaps;
      for (const std::size_t& index : sampleGroupHandler_IO.getSampleIndices()) {
        const OpenMS::FeatureMap& fmap = sequenceHandler_I.getSequence().at(index).getRawData().getFeatureMap();

        // Get the features/subordinates
        OpenMS::FeatureMap fmap_scan_polarity;
        for (const OpenMS::Feature& f : fmap) {
          // Feature level merge
          if (!merge_subordinates && fmap.metaValueExists("scan_polarity")) {
            OpenMS::Feature f_scan_polarity = f;
            f_scan_polarity.setSubordinates(std::vector<OpenMS::Feature>());
            fmap_scan_polarity.push_back(f_scan_polarity);
          }
          // Subordinate level merge
          else if (merge_subordinates) {
            OpenMS::Feature f_scan_polarity = f;
            std::vector<OpenMS::Feature> s_scan_polarity;
            for (const OpenMS::Feature& s : f.getSubordinates()) {
              if (s.metaValueExists("scan_polarity")) {
                s_scan_polarity.push_back(s);
              }
            }
            f_scan_polarity.setSubordinates(s_scan_polarity);
          }
        }

        mixed_scan_polarity_fmaps.push_back(fmap_scan_polarity);
      }

      // Merge the scan polarities
      //TODO...
    }
    else {
      LOGD << "No multiple scan polarities to merge.";
    }

    // Step 2: mass ranges
    // Check for multiple mass ranges
    std::set<std::pair<float,float>> scan_mass_ranges;
    for (const std::size_t& index : sampleGroupHandler_IO.getSampleIndices()) {
      scan_mass_ranges.insert(std::make_pair(sequenceHandler_I.getSequence().at(index).getMetaData().scan_mass_low, sequenceHandler_I.getSequence().at(index).getMetaData().scan_mass_high));
    }

    // Step 3: dilutions
    // Check for multiple dilutions
    std::set<float> dilution_factors;
    for (const std::size_t& index : sampleGroupHandler_IO.getSampleIndices()) {
      dilution_factors.insert(sequenceHandler_I.getSequence().at(index).getMetaData().dilution_factor);
    }

    LOGD << "END optimizeCalibrationCurves";
  }
}
