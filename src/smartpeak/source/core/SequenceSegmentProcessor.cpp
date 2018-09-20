// TODO: Add copyright

namespace SmartPeak
{
  SequenceSegmentProcessor::getSampleIndicesBySampleType(
    const SequenceSegmentHandler& sequenceSegmentHandler,
    const SequenceHandler& sequenceHandler,
    const MetaDataHandler::SampleType sampleType,
    std::vector<size_t>& sampleIndices
  )
  {
    sampleIndices.clear();
    for (const size_t index : sequenceSegmentHandler.getSampleIndices()) {
      if (sequenceHandler.getSequence().at(index).getMetaData().getSampleType() == sampleType) {
        sampleIndices.push_back(index);
      }
    }
  }

  SequenceSegmentProcessor::optimizeCalibrationCurves(
    SequenceSegmentHandler& sequenceSegmentHandler_IO,
    const SequenceHandler& sequenceHandler_I,
    const std::vector<std::map<std::string, std::string>>& AbsoluteQuantitation_params_I,
    const bool verbose_I
  )
  {
    if (verbose_I) {
      std::cout << "Optimizing calibrators." << std::endl;
    }

    const std::vector<size_t> standards_indices = getSampleIndicesBySampleType(
      sequenceSegmentHandler_IO, sequenceHandler_I, MetaDataHandler::SampleType::Standard
    );

    if (standards_indices.empty())
      return; // TODO[1]: or throw? print something?

    std::vector<OpenMS::FeatureMap> standards_featureMaps;
    for (const size_t index : standards_indices) {
      standards_featureMaps.push_back(sequenceHandler_I.getSequence().at(index).getRawData().getFeatureMap());
    }

    if (AbsoluteQuantitation_params_I.empty())
      return;  // TODO[2]: or throw? print something?

    OpenMS::AbsoluteQuantitation absoluteQuantitation;
    OpenMS::Param parameters = absoluteQuantitation.getParameters();
    Utilities::updateParameters(parameters, AbsoluteQuantitation_params_I);
    absoluteQuantitation.setParameters(parameters);

    absoluteQuantitation.setQuantMethods(sequenceSegmentHandler_IO.getQuantitationMethods());

    std::map<std::string, std::vector<OpenMS::AbsoluteQuantitationStandards::featureConcentration>> components_to_concentrations;

    for (const OpenMS::AbsoluteQuantitationMethod& row : sequenceSegmentHandler_IO.getQuantitationMethods()) {
      AbsoluteQuantitationStandards absoluteQuantitationStandards;
      std::vector<OpenMS::AbsoluteQuantitationStandards::featureConcentration> feature_concentrations;

      absoluteQuantitationStandards.getComponentFeatureConcentrations(
        sequenceSegmentHandler_IO.getStandardsConcentrations(),
        standards_featureMaps,
        row.getComponentName(),
        feature_concentrations
      );

      std::vector<OpenMS::AbsoluteQuantitationStandards::featureConcentration> feature_concentrations_pruned;
      for (const OpenMS::AbsoluteQuantitationStandards::featureConcentration& feature : feature_concentrations) {
        if (feature.actual_concentration > 0.0) {
          feature_concentrations_pruned.push_back(feature);
        }
      }

      if (feature_concentrations_pruned.empty())
        continue;

      absoluteQuantitation.optimizeSingleCalibrationCurve(
        row.getComponentName(),
        feature_concentrations_pruned
      );

      std::map<std::string, std::vector<OpenMS::AbsoluteQuantitationStandards::featureConcentration>>::iterator it =
        components_to_concentrations.find(row.getComponentName());
      if (it != components_to_concentrations.end()) {
        components_to_concentrations.erase(it);
      }
      components_to_concentrations.insert(row.getComponentName(), feature_concentrations_pruned);
    }

    sequenceSegmentHandler_IO.setComponentsToConcentrations(components_to_concentrations);
    sequenceSegmentHandler_IO.setQuantitationMethods(absoluteQuantitation.getQuantitationMethods);
  }
}
