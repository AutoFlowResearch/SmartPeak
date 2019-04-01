// TODO: Add copyright

#include <SmartPeak/core/SequenceSegmentProcessor.h>
#include <SmartPeak/core/Filenames.h>
#include <SmartPeak/core/MetaDataHandler.h>
#include <SmartPeak/core/SequenceHandler.h>
#include <OpenMS/ANALYSIS/QUANTITATION/AbsoluteQuantitation.h>
#include <OpenMS/METADATA/AbsoluteQuantitationStandards.h>
#include <SmartPeak/io/InputDataValidation.h>
#include <OpenMS/FORMAT/AbsoluteQuantitationStandardsFile.h>
#include <OpenMS/FORMAT/AbsoluteQuantitationMethodFile.h>
#include <plog/Log.h>

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

  void CalculateCalibration::process(
    SequenceSegmentHandler& sequenceSegmentHandler_IO,
    const SequenceHandler& sequenceHandler_I,
    const std::map<std::string, std::vector<std::map<std::string, std::string>>>& params_I,
    const Filenames& filenames,
    const bool verbose_I
  ) const {
    if (verbose_I) {
      std::cout << "==== START optimizeCalibrationCurves" << std::endl;
    }
    LOGD << "START optimizeCalibrationCurves";

    std::vector<size_t> standards_indices;

    // get all standards
    this->getSampleIndicesBySampleType(
      sequenceSegmentHandler_IO,
      sequenceHandler_I,
      MetaDataHandler::SampleType::Standard,
      standards_indices
    );

    // check if there are any standards to calculate the calibrators from
    if (standards_indices.empty()) {
      std::cout << "standards_indices argument is empty. Returning." << std::endl;
      LOGE << "standards_indices argument is empty. Returning";
      LOGD << "END optimizeCalibrationCurves";
      return;
    }

    std::vector<OpenMS::FeatureMap> standards_featureMaps;
    for (const size_t index : standards_indices) {
      standards_featureMaps.push_back(sequenceHandler_I.getSequence().at(index).getRawData().getFeatureMap());
    }

    if (params_I.at("AbsoluteQuantitation").empty()) {
      std::cout << "AbsoluteQuantitation_params_I argument is empty. Returning." << std::endl;
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
    sequenceSegmentHandler_IO.setQuantitationMethods(absoluteQuantitation.getQuantMethods());

    if (verbose_I) {
      std::cout << "==== END   optimizeCalibrationCurves" << std::endl;
    }
    LOGD << "END optimizeCalibrationCurves";
  }

  void LoadStandardsConcentrations::process(
    SequenceSegmentHandler& sequenceSegmentHandler_IO,
    const SequenceHandler& sequenceHandler_I,
    const std::map<std::string, std::vector<std::map<std::string, std::string>>>& params_I,
    const Filenames& filenames,
    const bool verbose_I
  ) const {
    if (verbose_I) {
      std::cout << "==== START loadStandardsConcentrations"
        << "\nloadStandardsConcentrations(): loading " << filenames.standardsConcentrations_csv_i << std::endl;
    }
    LOGD << "START loadStandardsConcentrations";

    LOGI << "Loading: " << filenames.standardsConcentrations_csv_i;

    if (filenames.standardsConcentrations_csv_i.empty()) {
      std::cout << "loadStandardsConcentrations(): filename is empty\n";
      LOGE << "Filename is empty";
      LOGD << "END loadStandardsConcentrations";
      return;
    }

    if (!InputDataValidation::fileExists(filenames.standardsConcentrations_csv_i)) {
      std::cout << "loadStandardsConcentrations(): file not found\n";
      LOGE << "File not found";
      LOGD << "END loadStandardsConcentrations";
      return;
    }

    try {
      OpenMS::AbsoluteQuantitationStandardsFile AQSf;
      AQSf.load(filenames.standardsConcentrations_csv_i, sequenceSegmentHandler_IO.getStandardsConcentrations());
    }
    catch (const std::exception& e) {
      std::cerr << "loadStandardsConcentrations(): " << e.what() << std::endl;
      LOGE << e.what();
      sequenceSegmentHandler_IO.getStandardsConcentrations().clear();
      std::cerr << "loadStandardsConcentrations(): standards concentrations clear" << std::endl;
      LOGI << "Standards concentrations clear";
    }

    if (verbose_I) {
      std::cout << "==== END   loadStandardsConcentrations" << std::endl;
    }
    LOGD << "END loadStandardsConcentrations";
  }

  void LoadQuantitationMethods::process(
    SequenceSegmentHandler& sequenceSegmentHandler_IO,
    const SequenceHandler& sequenceHandler_I,
    const std::map<std::string, std::vector<std::map<std::string, std::string>>>& params_I,
    const Filenames& filenames,
    const bool verbose_I
  ) const
  {
    if (verbose_I) {
      std::cout << "==== START loadQuantitationMethods"
        << "\nloadQuantitationMethods(): loading " << filenames.quantitationMethods_csv_i << std::endl;
    }
    LOGD << "START loadQuantitationMethods";

    LOGI << "Loading: " << filenames.quantitationMethods_csv_i;

    if (filenames.quantitationMethods_csv_i.empty()) {
      std::cout << "loadQuantitationMethods(): filename is empty\n";
      LOGE << "Filename is empty";
      LOGD << "END loadQuantitationMethods";
      return;
    }

    if (!InputDataValidation::fileExists(filenames.quantitationMethods_csv_i)) {
      std::cout << "loadQuantitationMethods(): file not found\n";
      LOGE << "File not found";
      LOGD << "END loadQuantitationMethods";
      return;
    }

    try {
      OpenMS::AbsoluteQuantitationMethodFile AQMf;
      AQMf.load(filenames.quantitationMethods_csv_i, sequenceSegmentHandler_IO.getQuantitationMethods());
    }
    catch (const std::exception& e) {
      std::cerr << "loadQuantitationMethods(): " << e.what() << std::endl;
      LOGE << e.what();
      sequenceSegmentHandler_IO.getQuantitationMethods().clear();
      std::cerr << "loadQuantitationMethods(): quantitation methods clear" << std::endl;
      LOGI << "quantitation methods clear";
    }

    // std::cout << InputDataValidation::getQuantitationMethodsInfo(sequenceSegmentHandler_IO);

    if (verbose_I) {
      std::cout << "==== END   loadQuantitationMethods" << std::endl;
    }
    LOGD << "END loadQuantitationMethods";
  }

  void StoreQuantitationMethods::process(
    SequenceSegmentHandler& sequenceSegmentHandler_IO,
    const SequenceHandler& sequenceHandler_I,
    const std::map<std::string, std::vector<std::map<std::string, std::string>>>& params_I,
    const Filenames& filenames,
    const bool verbose_I
  ) const
  {
    if (verbose_I) {
      std::cout << "==== START storeQuantitationMethods"
        << "\nstoreQuantitationMethods(): storing " << filenames.quantitationMethods_csv_o << std::endl;
    }
    LOGD << "START storeQuantitationMethods";

    LOGI << "Storing: " << filenames.quantitationMethods_csv_o;

    if (filenames.quantitationMethods_csv_o.empty()) {
      std::cout << "storeQuantitationMethods(): filename is empty\n";
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
      std::cerr << "storeQuantitationMethods(): " << e.what() << std::endl;
      LOGE << e.what();
    }

    if (verbose_I) {
      std::cout << "==== END   storeQuantitationMethods" << std::endl;
    }
    LOGD << "END storeQuantitationMethods";
  }

  void PlotCalibrators::process(
    SequenceSegmentHandler& sequenceSegmentHandler_IO,
    const SequenceHandler& sequenceHandler_I,
    const std::map<std::string, std::vector<std::map<std::string, std::string>>>& params_I,
    const Filenames& filenames,
    const bool verbose_I
  ) const
  {
    LOGD << "START PlotCalibrators";

    if (params_I.at("SequenceSegmentPlotter").empty()) {
      std::cout << "PlotCalibrators: params_I is empty\n";
      LOGE << "params_I is empty";
      LOGD << "END PlotCalibrators";
      return;
    }

    //// TODO: Uncomment when SequenceSegmentPlotter is implemented
    //SequenceSegmentPlotter sequenceSegmentPlotter;
    //sequenceSegmentPlotter.setParameters(SequenceSegmentPlotter_params_I);
    //sequenceSegmentPlotter.plotCalibrationPoints(calibrators_pdf_o, sequenceSegmentHandler_I);
    LOGD << "END PlotCalibrators";
  }
}
