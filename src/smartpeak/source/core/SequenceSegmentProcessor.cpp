// TODO: Add copyright

namespace SmartPeak
{
  void SequenceSegmentProcessor::getSampleIndicesBySampleType(
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

  void SequenceSegmentProcessor::optimizeCalibrationCurves(
    SequenceSegmentHandler& sequenceSegmentHandler_IO,
    const SequenceHandler& sequenceHandler_I,
    const std::vector<std::map<std::string, std::string>>& AbsoluteQuantitation_params_I,
    const bool verbose_I
  )
  {
    if (verbose_I) {
      std::cout << "Optimizing calibrators." << std::endl;
    }

    const std::vector<size_t> standards_indices;

    getSampleIndicesBySampleType(
      sequenceSegmentHandler_IO,
      sequenceHandler_I,
      MetaDataHandler::SampleType::Standard,
      standards_indices
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

  void SequenceSegmentProcessor::plotCalibrators(
    const SequenceSegmentHandler& sequenceSegmentHandler_I,
    const std::string& calibrators_pdf_o,
    const std::vector<std::map<std::string, std::string>>& SequenceSegmentPlotter_params_I,
    const bool verbose_I
  )
  {
    if (verbose_I)
      std::cout << "Plotting calibrators." << std::endl;

    if (SequenceSegmentPlotter_params_I.empty() || calibrators_pdf_o.empty())
      throw std::invalid_argument("Parameters or filename are empty.");

    SequenceSegmentPlotter sequenceSegmentPlotter;
    sequenceSegmentPlotter.setParameters(SequenceSegmentPlotter_params_I);
    sequenceSegmentPlotter.plotCalibrationPoints(calibrators_pdf_o, sequenceSegmentHandler_I);
  }

  void SequenceSegmentProcessor::processSequenceSegment(
    SequenceSegmentHandler& sequenceSegmentHandler_IO,
    const SequenceHandler& sequenceHandler_I,
    const std::string& sequence_segment_processing_event,
    const std::vector<std::map<std::string, std::string>>& parameters,
    const std::map<std::string, std::string>& filenames,
    const bool verbose_I
  )
  {
    try {
      if (sequence_segment_processing_event == "calculate_calibration") {
        optimizeCalibrationCurves(
          sequenceSegmentHandler_IO,
          sequenceHandler_I,
          parameters.at("AbsoluteQuantitation"),
          verbose_I
        );
        for (const size_t index : sequenceSegmentHandler_IO.getSampleIndices()) {
          sequenceHandler_I.getSequence().at(index).getRawData()
            .setQuantitationMethods(sequenceSegmentHandler_IO.getQuantitationMethods());
        }
      } else if (sequence_segment_processing_event == "store_quantitation_methods") {
        OpenMSFile::storeQuantitationMethods(sequenceSegmentHandler_IO, filenames.at("quantitationMethods_csv_o"), verbose_I);
      } else if (sequence_segment_processing_event == "load_quantitation_methods") {
        OpenMSFile::loadQuantitationMethods(sequenceSegmentHandler_IO, filenames.at("quantitationMethods_csv_i"), verbose_I);
      } else if (sequence_segment_processing_event == "plot_calibrators") {
        plotCalibrators(
          sequenceSegmentHandler_IO,
          filenames.at("calibrators_pdf_o"),
          parameters.at("SequenceSegmentPlotter"),
          verbose_I
        );
      } else {
        std::cout << "Sequence group processing event " << sequence_segment_processing_event << " was not recognized." << std::endl;
      }
    } catch (const std::exception& e) {
      std::cerr << e.what();
    }
  }
}
