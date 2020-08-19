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
    LOGD << "START optimizeCalibrationCurves";

    std::vector<size_t> standards_indices;
    // get all standards
    this->getSampleIndicesBySampleType(
      sampleGroupHandler_IO,
      sequenceHandler_I,
      SampleType::Standard,
      standards_indices
    );

    // check if there are any standards to calculate the calibrators from
    if (standards_indices.empty()) {
      LOGE << "standards_indices argument is empty. Returning";
      LOGD << "END optimizeCalibrationCurves";
      return;
    }

    std::vector<OpenMS::FeatureMap> standards_featureMaps;
    for (const size_t index : standards_indices) {
      standards_featureMaps.push_back(sequenceHandler_I.getSequence().at(index).getRawData().getFeatureMap());
    }

    if (params_I.at("AbsoluteQuantitation").empty()) {
      LOGE << "Parameters not found for AbsoluteQuantitation. Returning";
      LOGD << "END optimizeCalibrationCurves";
      return;
    }

    // add in the method parameters
    OpenMS::AbsoluteQuantitation absoluteQuantitation;
    OpenMS::Param parameters = absoluteQuantitation.getParameters();
    Utilities::updateParameters(parameters, params_I.at("AbsoluteQuantitation"));
    absoluteQuantitation.setParameters(parameters);

    absoluteQuantitation.setQuantMethods(sampleGroupHandler_IO.getQuantitationMethods());
    std::map<std::string, std::vector<OpenMS::AbsoluteQuantitationStandards::featureConcentration>> components_to_concentrations;
    for (const OpenMS::AbsoluteQuantitationMethod& row : sampleGroupHandler_IO.getQuantitationMethods()) {
      // map standards to features
      OpenMS::AbsoluteQuantitationStandards absoluteQuantitationStandards;
      std::vector<OpenMS::AbsoluteQuantitationStandards::featureConcentration> feature_concentrations;

      absoluteQuantitationStandards.getComponentFeatureConcentrations(
        sampleGroupHandler_IO.getStandardsConcentrations(),
        standards_featureMaps,
        row.getComponentName(),
        feature_concentrations
      );
      // remove features with an actual concentration of 0.0 or less
      std::vector<OpenMS::AbsoluteQuantitationStandards::featureConcentration> feature_concentrations_pruned;
      for (const OpenMS::AbsoluteQuantitationStandards::featureConcentration& feature : feature_concentrations) {
        if (feature.actual_concentration > 0.0) {
          feature_concentrations_pruned.push_back(feature);
        }
      }

      // remove components without any points
      if (feature_concentrations_pruned.empty()) {
        continue;
      }

      try
      {
        absoluteQuantitation.optimizeSingleCalibrationCurve(
          row.getComponentName(),
          feature_concentrations_pruned
        );
      }
      catch (OpenMS::Exception::DivisionByZero& )
      {
        LOGW << "Warning: '" << row.getComponentName() << "' cannot be analysed - division by zero\n";
        continue;
      }
      // find the optimal calibration curve for each component

      components_to_concentrations.erase(row.getComponentName());
      components_to_concentrations.insert({row.getComponentName(), feature_concentrations_pruned});
    }
    // store results
    sampleGroupHandler_IO.setComponentsToConcentrations(components_to_concentrations);
    sampleGroupHandler_IO.getQuantitationMethods() = absoluteQuantitation.getQuantMethods();
    //sampleGroupHandler_IO.setQuantitationMethods(absoluteQuantitation.getQuantMethods());
    LOGD << "END optimizeCalibrationCurves";
  }
}
