// TODO: Add copyright

#include <SmartPeak/core/SequenceSegmentProcessor.h>
#include <SmartPeak/core/Filenames.h>
#include <SmartPeak/core/MetaDataHandler.h>
#include <SmartPeak/core/SampleType.h>
#include <SmartPeak/core/SequenceHandler.h>
#include <SmartPeak/core/Utilities.h>
#include <OpenMS/ANALYSIS/QUANTITATION/AbsoluteQuantitation.h>
#include <OpenMS/METADATA/AbsoluteQuantitationStandards.h>
#include <OpenMS/ANALYSIS/OPENSWATH/MRMFeatureFilter.h>
#include <SmartPeak/io/InputDataValidation.h>
#include <OpenMS/FORMAT/AbsoluteQuantitationStandardsFile.h>
#include <OpenMS/FORMAT/AbsoluteQuantitationMethodFile.h>
#include <OpenMS/FORMAT/MRMFeatureQCFile.h>  // load featureFilter and featureQC
#include <plog/Log.h>

namespace SmartPeak
{
  void SequenceSegmentProcessor::getSampleIndicesBySampleType(
    const SequenceSegmentHandler& sequenceSegmentHandler,
    const SequenceHandler& sequenceHandler,
    const SampleType sampleType,
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

  void CalculateCalibration::process(
    SequenceSegmentHandler& sequenceSegmentHandler_IO,
    const SequenceHandler& sequenceHandler_I,
    const std::map<std::string, std::vector<std::map<std::string, std::string>>>& params_I,
    const Filenames& filenames
  ) const
  {
    LOGD << "START optimizeCalibrationCurves";

    std::vector<size_t> standards_indices;

    // get all standards
    this->getSampleIndicesBySampleType(
      sequenceSegmentHandler_IO,
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

    absoluteQuantitation.setQuantMethods(sequenceSegmentHandler_IO.getQuantitationMethods());

    std::map<std::string, std::vector<OpenMS::AbsoluteQuantitationStandards::featureConcentration>> components_to_concentrations;

    for (const OpenMS::AbsoluteQuantitationMethod& row : sequenceSegmentHandler_IO.getQuantitationMethods()) {
      // map standards to features
      OpenMS::AbsoluteQuantitationStandards absoluteQuantitationStandards;
      std::vector<OpenMS::AbsoluteQuantitationStandards::featureConcentration> feature_concentrations;

      absoluteQuantitationStandards.getComponentFeatureConcentrations(
        sequenceSegmentHandler_IO.getStandardsConcentrations(),
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
      if (feature_concentrations_pruned.empty())
        continue;

      // find the optimal calibration curve for each component
      absoluteQuantitation.optimizeSingleCalibrationCurve(
        row.getComponentName(),
        feature_concentrations_pruned
      );

      components_to_concentrations.erase(row.getComponentName());
      components_to_concentrations.insert({row.getComponentName(), feature_concentrations_pruned});
    }

    // store results
    sequenceSegmentHandler_IO.setComponentsToConcentrations(components_to_concentrations);
    sequenceSegmentHandler_IO.getQuantitationMethods() = absoluteQuantitation.getQuantMethods();
    //sequenceSegmentHandler_IO.setQuantitationMethods(absoluteQuantitation.getQuantMethods());

    LOGD << "END optimizeCalibrationCurves";
  }

  void LoadStandardsConcentrations::process(
    SequenceSegmentHandler& sequenceSegmentHandler_IO,
    const SequenceHandler& sequenceHandler_I,
    const std::map<std::string, std::vector<std::map<std::string, std::string>>>& params_I,
    const Filenames& filenames
  ) const
  {
    LOGD << "START loadStandardsConcentrations";
    LOGI << "Loading: " << filenames.standardsConcentrations_csv_i;

    if (filenames.standardsConcentrations_csv_i.empty()) {
      LOGE << "Filename is empty";
      LOGD << "END loadStandardsConcentrations";
      return;
    }

    if (!InputDataValidation::fileExists(filenames.standardsConcentrations_csv_i)) {
      LOGE << "File not found";
      LOGD << "END loadStandardsConcentrations";
      return;
    }

    try {
      OpenMS::AbsoluteQuantitationStandardsFile AQSf;
      AQSf.load(filenames.standardsConcentrations_csv_i, sequenceSegmentHandler_IO.getStandardsConcentrations());
    }
    catch (const std::exception& e) {
      LOGE << e.what();
      sequenceSegmentHandler_IO.getStandardsConcentrations().clear();
      LOGI << "Standards concentrations clear";
    }

    LOGD << "END loadStandardsConcentrations";
  }

  void LoadQuantitationMethods::process(
    SequenceSegmentHandler& sequenceSegmentHandler_IO,
    const SequenceHandler& sequenceHandler_I,
    const std::map<std::string, std::vector<std::map<std::string, std::string>>>& params_I,
    const Filenames& filenames
  ) const
  {
    LOGD << "START loadQuantitationMethods";
    LOGI << "Loading: " << filenames.quantitationMethods_csv_i;

    if (filenames.quantitationMethods_csv_i.empty()) {
      LOGE << "Filename is empty";
      LOGD << "END loadQuantitationMethods";
      return;
    }

    if (!InputDataValidation::fileExists(filenames.quantitationMethods_csv_i)) {
      LOGE << "File not found";
      LOGD << "END loadQuantitationMethods";
      return;
    }

    try {
      OpenMS::AbsoluteQuantitationMethodFile AQMf;
      AQMf.load(filenames.quantitationMethods_csv_i, sequenceSegmentHandler_IO.getQuantitationMethods());
    }
    catch (const std::exception& e) {
      LOGE << e.what();
      sequenceSegmentHandler_IO.getQuantitationMethods().clear();
      LOGI << "quantitation methods clear";
    }

    LOGD << "END loadQuantitationMethods";
  }

  void StoreQuantitationMethods::process(
    SequenceSegmentHandler& sequenceSegmentHandler_IO,
    const SequenceHandler& sequenceHandler_I,
    const std::map<std::string, std::vector<std::map<std::string, std::string>>>& params_I,
    const Filenames& filenames
  ) const
  {
    LOGD << "START storeQuantitationMethods";
    LOGI << "Storing: " << filenames.quantitationMethods_csv_o;

    if (filenames.quantitationMethods_csv_o.empty()) {
      LOGE << "Filename is empty";
      LOGD << "END storeQuantitationMethods";
      return;
    }

    try {
      OpenMS::AbsoluteQuantitationMethodFile aqmf;
      aqmf.store(
        filenames.quantitationMethods_csv_o,
        sequenceSegmentHandler_IO.getQuantitationMethods()
      );
    }
    catch (const std::exception& e) {
      LOGE << e.what();
    }

    LOGD << "END storeQuantitationMethods";
  }

  void LoadFeatureFilters::process(
    SequenceSegmentHandler& sequenceSegmentHandler_IO,
    const SequenceHandler& sequenceHandler_I,
    const std::map<std::string, std::vector<std::map<std::string, std::string>>>& params_I,
    const Filenames& filenames
  ) const
  {
    LOGD << "START loadFeatureFilter";
    LOGI << "Loading: " << filenames.featureFilterComponents_csv_i << " and " <<
      filenames.featureFilterComponentGroups_csv_i;

    if (filenames.featureFilterComponents_csv_i.empty() &&
      filenames.featureFilterComponentGroups_csv_i.empty()) {
      LOGE << "Filenames are both empty";
      LOGD << "END loadFeatureFilter";
      return;
    }

    if (filenames.featureFilterComponents_csv_i.size() &&
      !InputDataValidation::fileExists(filenames.featureFilterComponents_csv_i)) {
      LOGE << "File not found: " << filenames.featureFilterComponents_csv_i;
      LOGD << "END loadFeatureFilter";
      return;
    }

    if (filenames.featureFilterComponentGroups_csv_i.size() &&
      !InputDataValidation::fileExists(filenames.featureFilterComponentGroups_csv_i)) {
      LOGE << "File not found: " << filenames.featureFilterComponentGroups_csv_i;
      LOGD << "END loadFeatureFilter";
      return;
    }

    try {
      OpenMS::MRMFeatureQCFile featureQCFile;
      if (filenames.featureFilterComponents_csv_i.size()) { // because we don't know if either of the two names is empty
        featureQCFile.load(filenames.featureFilterComponents_csv_i, sequenceSegmentHandler_IO.getFeatureFilter(), false);
      }
      if (filenames.featureFilterComponentGroups_csv_i.size()) {
        featureQCFile.load(filenames.featureFilterComponentGroups_csv_i, sequenceSegmentHandler_IO.getFeatureFilter(), true);
      }
    }
    catch (const std::exception& e) {
      LOGE << e.what();
      sequenceSegmentHandler_IO.getFeatureFilter().component_qcs.clear();
      sequenceSegmentHandler_IO.getFeatureFilter().component_group_qcs.clear();
      sequenceSegmentHandler_IO.getFeatureFilter().component_group_pair_qcs.clear();
      LOGI << "feature filter clear";
    }

    LOGD << "END loadFeatureFilter";
  }

  void LoadFeatureQCs::process(
    SequenceSegmentHandler& sequenceSegmentHandler_IO,
    const SequenceHandler& sequenceHandler_I,
    const std::map<std::string, std::vector<std::map<std::string, std::string>>>& params_I,
    const Filenames& filenames
  ) const
  {
    LOGD << "START loadFeatureQC";
    LOGI << "Loading: " << filenames.featureQCComponents_csv_i << " and " <<
      filenames.featureQCComponentGroups_csv_i;

    if (filenames.featureQCComponents_csv_i.empty() &&
      filenames.featureQCComponentGroups_csv_i.empty()) {
      LOGE << "Filenames are both empty";
      LOGD << "END loadFeatureQC";
      return;
    }

    if (filenames.featureQCComponents_csv_i.size() &&
      !InputDataValidation::fileExists(filenames.featureQCComponents_csv_i)) {
      LOGE << "File not found: " << filenames.featureQCComponents_csv_i;
      LOGD << "END loadFeatureQC";
      return;
    }

    if (filenames.featureQCComponentGroups_csv_i.size() &&
      !InputDataValidation::fileExists(filenames.featureQCComponentGroups_csv_i)) {
      LOGE << "File not found: " << filenames.featureQCComponentGroups_csv_i;
      LOGD << "END loadFeatureQC";
      return;
    }

    try {
      OpenMS::MRMFeatureQCFile featureQCFile;
      if (filenames.featureQCComponents_csv_i.size()) { // because we don't know if either of the two names is empty
        featureQCFile.load(filenames.featureQCComponents_csv_i, sequenceSegmentHandler_IO.getFeatureQC(), false);
      }
      if (filenames.featureQCComponentGroups_csv_i.size()) {
        featureQCFile.load(filenames.featureQCComponentGroups_csv_i, sequenceSegmentHandler_IO.getFeatureQC(), true);
      }
    }
    catch (const std::exception& e) {
      LOGE << e.what();
      sequenceSegmentHandler_IO.getFeatureQC().component_qcs.clear();
      sequenceSegmentHandler_IO.getFeatureQC().component_group_qcs.clear();
      sequenceSegmentHandler_IO.getFeatureQC().component_group_pair_qcs.clear();
      LOGI << "Feature qc clear";
    }

    LOGD << "END loadFeatureQC";
  }

  void StoreFeatureFilters::process(
    SequenceSegmentHandler& sequenceSegmentHandler_IO,
    const SequenceHandler& sequenceHandler_I,
    const std::map<std::string, std::vector<std::map<std::string, std::string>>>& params_I,
    const Filenames& filenames
  ) const
  {
    LOGD << "START storeFeatureFilter";
    LOGI << "Storing: " << filenames.featureFilterComponents_csv_i << " and " <<
      filenames.featureFilterComponentGroups_csv_i;

    if (filenames.featureFilterComponents_csv_i.empty() &&
      filenames.featureFilterComponentGroups_csv_i.empty()) {
      LOGE << "Filenames are both empty";
      LOGD << "END storeFeatureFilter";
      return;
    }

    try {
      OpenMS::MRMFeatureQCFile featureQCFile;
      if (filenames.featureFilterComponents_csv_i.size()) { // because we don't know if either of the two names is empty
        featureQCFile.store(filenames.featureFilterComponents_csv_i, sequenceSegmentHandler_IO.getFeatureFilter(), false);
      }
      if (filenames.featureFilterComponentGroups_csv_i.size()) {
        featureQCFile.store(filenames.featureFilterComponentGroups_csv_i, sequenceSegmentHandler_IO.getFeatureFilter(), true);
      }
    }
    catch (const std::exception& e) {
      LOGE << e.what();
      LOGI << "feature filter store exception";
    }

    LOGD << "END storeFeatureFilter";
  }

  void StoreFeatureQCs::process(
    SequenceSegmentHandler& sequenceSegmentHandler_IO,
    const SequenceHandler& sequenceHandler_I,
    const std::map<std::string, std::vector<std::map<std::string, std::string>>>& params_I,
    const Filenames& filenames
  ) const
  {
    LOGD << "START storeFeatureQC";
    LOGI << "Loading: " << filenames.featureQCComponents_csv_i << " and " <<
      filenames.featureQCComponentGroups_csv_i;

    if (filenames.featureQCComponents_csv_i.empty() &&
      filenames.featureQCComponentGroups_csv_i.empty()) {
      LOGE << "Filenames are both empty";
      LOGD << "END storeFeatureQC";
      return;
    }

    try {
      OpenMS::MRMFeatureQCFile featureQCFile;
      if (filenames.featureQCComponents_csv_i.size()) { // because we don't know if either of the two names is empty
        featureQCFile.store(filenames.featureQCComponents_csv_i, sequenceSegmentHandler_IO.getFeatureQC(), false);
      }
      if (filenames.featureQCComponentGroups_csv_i.size()) {
        featureQCFile.store(filenames.featureQCComponentGroups_csv_i, sequenceSegmentHandler_IO.getFeatureQC(), true);
      }
    }
    catch (const std::exception& e) {
      LOGE << e.what();
      LOGI << "Feature qc store exception";
    }

    LOGD << "END storeFeatureQC";
  }

  void PlotCalibrators::process(
    SequenceSegmentHandler& sequenceSegmentHandler_IO,
    const SequenceHandler& sequenceHandler_I,
    const std::map<std::string, std::vector<std::map<std::string, std::string>>>& params_I,
    const Filenames& filenames
  ) const
  {
    LOGD << "START PlotCalibrators";

    if (params_I.count("SequenceSegmentPlotter") &&
        params_I.at("SequenceSegmentPlotter").empty()) {
      LOGE << "No parameters for SequenceSegmentPlotter";
      LOGD << "END PlotCalibrators";
      return;
    }

    //// TODO: Uncomment when SequenceSegmentPlotter is implemented
    //SequenceSegmentPlotter sequenceSegmentPlotter;
    //sequenceSegmentPlotter.setParameters(SequenceSegmentPlotter_params_I);
    //sequenceSegmentPlotter.plotCalibrationPoints(calibrators_pdf_o, sequenceSegmentHandler_I);
    LOGD << "END PlotCalibrators";
  }

  void EstimateFeatureFilterValues::process(
    SequenceSegmentHandler& sequenceSegmentHandler_IO,
    const SequenceHandler& sequenceHandler_I,
    const std::map<std::string, std::vector<std::map<std::string, std::string>>>& params_I,
    const Filenames& filenames
  ) const
  {
    LOGD << "START estimateFeatureFilterValues";

    std::vector<size_t> standards_indices, qcs_indices;

    // get all standards
    this->getSampleIndicesBySampleType(
      sequenceSegmentHandler_IO,
      sequenceHandler_I,
      SampleType::Standard,
      standards_indices
    );

    // get all QCs
    this->getSampleIndicesBySampleType(
      sequenceSegmentHandler_IO,
      sequenceHandler_I,
      SampleType::QC,
      qcs_indices
    );

    // check if there are any standards or QCs to estimate the feature filter parameters from
    if (standards_indices.empty() && qcs_indices.empty()) {
      LOGE << "standards_indices and/or qcs_indices argument is empty. Returning";
      LOGD << "END estimateFeatureFilterValues";
      return;
    }

    // OPTIMIZATION: it would be prefered to only use those standards that are part of the optimized calibration curve for each component
    std::vector<OpenMS::FeatureMap> standards_featureMaps;
    for (const size_t index : standards_indices) {
      standards_featureMaps.push_back(sequenceHandler_I.getSequence().at(index).getRawData().getFeatureMap());
    }
    for (const size_t index : qcs_indices) {
      standards_featureMaps.push_back(sequenceHandler_I.getSequence().at(index).getRawData().getFeatureMap());
    }

    if (params_I.at("MRMFeatureFilter.filter_MRMFeatures").empty()) {
      LOGE << "No parameters passed to filterFeatures. Not filtering";
      LOGD << "END estimateFeatureFilterValues";
      return;
    }

    // add in the method parameters
    OpenMS::MRMFeatureFilter featureFilter;
    OpenMS::Param parameters = featureFilter.getParameters();
    Utilities::updateParameters(parameters, params_I.at("MRMFeatureFilter.filter_MRMFeatures"));
    featureFilter.setParameters(parameters);

    featureFilter.EstimateDefaultMRMFeatureQCValues(
      standards_featureMaps,
      sequenceSegmentHandler_IO.getFeatureFilter(),
      sequenceHandler_I.getSequence().front().getRawData().getTargetedExperiment(), // Targeted experiment used by all injections in the sequence
      true
    );

    // store results
    //sequenceSegmentHandler_IO.setQuantitationMethods(absoluteQuantitation.getQuantMethods());

    LOGD << "END estimateFeatureFilterValues";
  }
}
