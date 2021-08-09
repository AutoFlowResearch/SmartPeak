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
// $Authors: Douglas McCloskey, Pasquale Domenico Colaianni $
// --------------------------------------------------------------------------

#include <SmartPeak/core/SequenceSegmentProcessor.h>
#include <SmartPeak/core/Filenames.h>
#include <SmartPeak/core/MetaDataHandler.h>
#include <SmartPeak/core/SampleType.h>
#include <SmartPeak/core/SequenceHandler.h>
#include <SmartPeak/core/Utilities.h>
#include <SmartPeak/core/ApplicationHandler.h>
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

  ParameterSet CalculateCalibration::getParameterSchema() const
  {
    OpenMS::AbsoluteQuantitation oms_params;
    return ParameterSet({ oms_params });
  }

  void CalculateCalibration::process(
    SequenceSegmentHandler& sequenceSegmentHandler_IO,
    const SequenceHandler& sequenceHandler_I,
    const ParameterSet& params_I,
    const Filenames& filenames_override
  ) const
  {
    LOGD << "START optimizeCalibrationCurves";
    Filenames filenames = prepareFileNames(filenames_override);

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

    // add in the method parameters
    OpenMS::AbsoluteQuantitation absoluteQuantitation;
    Utilities::setUserParameters(absoluteQuantitation, params_I);

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
      catch (...)
      {
        LOGW << "Warning: '" << row.getComponentName() << "' cannot be analysed.\n";
        continue;
      }
      // find the optimal calibration curve for each component

      components_to_concentrations.erase(row.getComponentName());
      components_to_concentrations.insert({row.getComponentName(), feature_concentrations_pruned});
    }
    // store results
    sequenceSegmentHandler_IO.setComponentsToConcentrations(components_to_concentrations);
    sequenceSegmentHandler_IO.getQuantitationMethods() = absoluteQuantitation.getQuantMethods();
    //sequenceSegmentHandler_IO.setQuantitationMethods(absoluteQuantitation.getQuantMethods());
    LOGD << "END optimizeCalibrationCurves";
  }

  ParameterSet LoadStandardsConcentrations::getParameterSchema() const
  {
    return ParameterSet();
  }

  bool LoadStandardsConcentrations::onFilePicked(const std::string& filename, ApplicationHandler* application_handler)
  {
    if (application_handler->sequenceHandler_.getSequence().size() == 0)
    {
      LOGE << "File cannot be loaded without first loading the sequence.";
      return false;
    }
    Filenames filenames;
    filenames.setFullPathName("quantitationMethods_csv_i", filename);
    for (SequenceSegmentHandler& sequenceSegmentHandler : application_handler->sequenceHandler_.getSequenceSegments()) {
      sequence_segment_observable_ = &(application_handler->sequenceHandler_);
      process(sequenceSegmentHandler, SequenceHandler(), {}, filenames);
    }
    return true;
  }

  void LoadStandardsConcentrations::getInputsOutputs(Filenames& filenames) const
  {
    filenames.addFileName("standardsConcentrations_csv_i", "standardsConcentrations.csv", Filenames::FileScope::EFileScopeMain);
  };

  void LoadStandardsConcentrations::process(
    SequenceSegmentHandler& sequenceSegmentHandler_IO,
    const SequenceHandler& sequenceHandler_I,
    const ParameterSet& params_I,
    const Filenames& filenames_override
  ) const
  {
    LOGD << "START loadStandardsConcentrations";
    Filenames filenames = prepareFileNames(filenames_override);

    LOGI << "Loading: " << filenames.getFullPathName("standardsConcentrations_csv_i");

    if (filenames.getFullPathName("standardsConcentrations_csv_i").empty()) {
      LOGE << "Filename is empty";
      LOGD << "END loadStandardsConcentrations";
      return;
    }

    if (!InputDataValidation::fileExists(filenames.getFullPathName("standardsConcentrations_csv_i"))) {
      LOGE << "File not found";
      LOGD << "END loadStandardsConcentrations";
      return;
    }

    try {
      OpenMS::AbsoluteQuantitationStandardsFile AQSf;
      AQSf.load(filenames.getFullPathName("standardsConcentrations_csv_i"), sequenceSegmentHandler_IO.getStandardsConcentrations());
    }
    catch (const std::exception& e) {
      LOGE << e.what();
      sequenceSegmentHandler_IO.getStandardsConcentrations().clear();
      LOGI << "Standards concentrations clear";
    }

    LOGD << "END loadStandardsConcentrations";
  }

  ParameterSet LoadQuantitationMethods::getParameterSchema() const
  {
    return ParameterSet();
  }

  void LoadQuantitationMethods::getInputsOutputs(Filenames& filenames) const
  {
    filenames.addFileName("quantitationMethods_csv_i", "quantitationMethods.csv", Filenames::FileScope::EFileScopeMain);
  };

  bool LoadQuantitationMethods::onFilePicked(const std::string& filename, ApplicationHandler* application_handler)
  {
    if (application_handler->sequenceHandler_.getSequence().size() == 0)
    {
      LOGE << "File cannot be loaded without first loading the sequence.";
      return false;
    }
    Filenames filenames;
    filenames.setFullPathName("quantitationMethods_csv_i", filename);
    for (SequenceSegmentHandler& sequenceSegmentHandler : application_handler->sequenceHandler_.getSequenceSegments()) {
      sequence_segment_observable_ = &(application_handler->sequenceHandler_);
      process(sequenceSegmentHandler, SequenceHandler(), {}, filenames);
    }
    return true;
  }

  void LoadQuantitationMethods::process(
    SequenceSegmentHandler& sequenceSegmentHandler_IO,
    const SequenceHandler& sequenceHandler_I,
    const ParameterSet& params_I,
    const Filenames& filenames_override
  ) const
  {
    LOGD << "START loadQuantitationMethods";
    Filenames filenames = prepareFileNames(filenames_override);

    LOGI << "Loading: " << filenames.getFullPathName("quantitationMethods_csv_i");

    if (filenames.getFullPathName("quantitationMethods_csv_i").empty()) {
      LOGE << "Filename is empty";
      LOGD << "END loadQuantitationMethods";
      return;
    }

    if (!InputDataValidation::fileExists(filenames.getFullPathName("quantitationMethods_csv_i"))) {
      LOGE << "File not found";
      LOGD << "END loadQuantitationMethods";
      return;
    }

    try {
      OpenMS::AbsoluteQuantitationMethodFile AQMf;
      AQMf.load(filenames.getFullPathName("quantitationMethods_csv_i"), sequenceSegmentHandler_IO.getQuantitationMethods());
      if (sequence_segment_observable_) sequence_segment_observable_->notifyQuantitationMethodsUpdated();
    }
    catch (const std::exception& e) {
      LOGE << e.what();
      sequenceSegmentHandler_IO.getQuantitationMethods().clear();
      LOGI << "quantitation methods clear";
    }
    LOGD << "END loadQuantitationMethods";
  }

  ParameterSet StoreQuantitationMethods::getParameterSchema() const
  {
    return ParameterSet();
  }

  void StoreQuantitationMethods::getInputsOutputs(Filenames& filenames) const
  {
    filenames.addFileName("quantitationMethods_csv_o", "_quantitationMethods.csv", Filenames::FileScope::EFileScopeInjectionOutput);
  };

  void StoreQuantitationMethods::process(
    SequenceSegmentHandler& sequenceSegmentHandler_IO,
    const SequenceHandler& sequenceHandler_I,
    const ParameterSet& params_I,
    const Filenames& filenames_override
  ) const
  {
    LOGD << "START storeQuantitationMethods";
    Filenames filenames = prepareFileNames(filenames_override);

    LOGI << "Storing: " << filenames.getFullPathName("quantitationMethods_csv_o");

    if (filenames.getFullPathName("quantitationMethods_csv_o").empty()) {
      LOGE << "Filename is empty";
      LOGD << "END storeQuantitationMethods";
      return;
    }

    try {
      OpenMS::AbsoluteQuantitationMethodFile aqmf;
      aqmf.store(
        filenames.getFullPathName("quantitationMethods_csv_o"),
        sequenceSegmentHandler_IO.getQuantitationMethods()
      );
    }
    catch (const std::exception& e) {
      LOGE << e.what();
    }

    LOGD << "END storeQuantitationMethods";
  }

  ParameterSet LoadFeatureFilters::getParameterSchema() const
  {
    return ParameterSet();
  }

  bool LoadFeatureFilters::onFilePicked(const std::string& filename, ApplicationHandler* application_handler)
  {
    if (application_handler->sequenceHandler_.getSequence().size() == 0)
    {
      LOGE << "File cannot be loaded without first loading the sequence.";
      return false;
    }
    Filenames filenames;
    if (component_group_)
    {
      filenames.setFullPathName("featureFilterComponents_csv_i", "");
      filenames.setFullPathName("featureFilterComponentGroups_csv_i", filename);
    }
    else
    {
      filenames.setFullPathName("featureFilterComponents_csv_i", filename);
      filenames.setFullPathName("featureFilterComponentGroups_csv_i", "");
    }
    for (SequenceSegmentHandler& sequenceSegmentHandler : application_handler->sequenceHandler_.getSequenceSegments()) {
      sequence_segment_observable_ = &(application_handler->sequenceHandler_);
      process(sequenceSegmentHandler, SequenceHandler(), {}, filenames);
    }
    return true;
  }

  void LoadFeatureFilters::process(
    SequenceSegmentHandler& sequenceSegmentHandler_IO,
    const SequenceHandler& sequenceHandler_I,
    const ParameterSet& params_I,
    const Filenames& filenames_override
  ) const
  {
    LOGD << "START loadFeatureFilter";
    Filenames filenames = prepareFileNames(filenames_override);

    LOGI << "Loading: " << filenames.getFullPathName("featureFilterComponents_csv_i") << " and " <<
      filenames.getFullPathName("featureFilterComponentGroups_csv_i");

    if (filenames.getFullPathName("featureFilterComponents_csv_i").empty() &&
      filenames.getFullPathName("featureFilterComponentGroups_csv_i").empty()) {
      LOGE << "Filenames are both empty";
      LOGD << "END loadFeatureFilter";
      return;
    }

    if (filenames.getFullPathName("featureFilterComponents_csv_i").size() &&
      !InputDataValidation::fileExists(filenames.getFullPathName("featureFilterComponents_csv_i"))) {
      LOGE << "File not found: " << filenames.getFullPathName("featureFilterComponents_csv_i");
      LOGD << "END loadFeatureFilter";
      return;
    }

    if (filenames.getFullPathName("featureFilterComponentGroups_csv_i").size() &&
      !InputDataValidation::fileExists(filenames.getFullPathName("featureFilterComponentGroups_csv_i"))) {
      LOGE << "File not found: " << filenames.getFullPathName("featureFilterComponentGroups_csv_i");
      LOGD << "END loadFeatureFilter";
      return;
    }

    try {
      OpenMS::MRMFeatureQCFile featureQCFile;
      if (filenames.getFullPathName("featureFilterComponents_csv_i").size()) { // because we don't know if either of the two names is empty
        featureQCFile.load(filenames.getFullPathName("featureFilterComponents_csv_i"), sequenceSegmentHandler_IO.getFeatureFilter(), false);
        if (sequence_segment_observable_) sequence_segment_observable_->notifyFeatureFiltersComponentsUpdated();
      }
      if (filenames.getFullPathName("featureFilterComponentGroups_csv_i").size()) {
        featureQCFile.load(filenames.getFullPathName("featureFilterComponentGroups_csv_i"), sequenceSegmentHandler_IO.getFeatureFilter(), true);
        if (sequence_segment_observable_) sequence_segment_observable_->notifyFeatureFiltersComponentGroupsUpdated();
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

  ParameterSet LoadFeatureQCs::getParameterSchema() const
  {
    return ParameterSet();
  }

  bool LoadFeatureQCs::onFilePicked(const std::string& filename, ApplicationHandler* application_handler)
  {
    if (application_handler->sequenceHandler_.getSequence().size() == 0)
    {
      LOGE << "File cannot be loaded without first loading the sequence.";
      return false;
    }
    Filenames filenames;
    if (component_group_)
    {
      filenames.setFullPathName("featureQCComponents_csv_i", "");
      filenames.setFullPathName("featureQCComponentGroups_csv_i", filename);
    }
    else
    {
      filenames.setFullPathName("featureQCComponents_csv_i", filename);
      filenames.setFullPathName("featureQCComponentGroups_csv_i", "");
    }
    for (SequenceSegmentHandler& sequenceSegmentHandler : application_handler->sequenceHandler_.getSequenceSegments()) {
      sequence_segment_observable_ = &(application_handler->sequenceHandler_);
      process(sequenceSegmentHandler, SequenceHandler(), {}, filenames);
    }
    return true;
  }

  void LoadFeatureQCs::process(
    SequenceSegmentHandler& sequenceSegmentHandler_IO,
    const SequenceHandler& sequenceHandler_I,
    const ParameterSet& params_I,
    const Filenames& filenames_override
  ) const
  {
    LOGD << "START loadFeatureQC";
    Filenames filenames = prepareFileNames(filenames_override);

    LOGI << "Loading: " << filenames.getFullPathName("featureQCComponents_csv_i") << " and " <<
      filenames.getFullPathName("featureQCComponentGroups_csv_i");

    if (filenames.getFullPathName("featureQCComponents_csv_i").empty() &&
      filenames.getFullPathName("featureQCComponentGroups_csv_i").empty()) {
      LOGE << "Filenames are both empty";
      LOGD << "END loadFeatureQC";
      return;
    }

    if (filenames.getFullPathName("featureQCComponents_csv_i").size() &&
      !InputDataValidation::fileExists(filenames.getFullPathName("featureQCComponents_csv_i"))) {
      LOGE << "File not found: " << filenames.getFullPathName("featureQCComponents_csv_i");
      LOGD << "END loadFeatureQC";
      return;
    }

    if (filenames.getFullPathName("featureQCComponentGroups_csv_i").size() &&
      !InputDataValidation::fileExists(filenames.getFullPathName("featureQCComponentGroups_csv_i"))) {
      LOGE << "File not found: " << filenames.getFullPathName("featureQCComponentGroups_csv_i");
      LOGD << "END loadFeatureQC";
      return;
    }

    try {
      OpenMS::MRMFeatureQCFile featureQCFile;
      if (filenames.getFullPathName("featureQCComponents_csv_i").size()) { // because we don't know if either of the two names is empty
        featureQCFile.load(filenames.getFullPathName("featureQCComponents_csv_i"), sequenceSegmentHandler_IO.getFeatureQC(), false);
        if (sequence_segment_observable_) sequence_segment_observable_->notifyFeatureQCComponentsUpdated();
      }
      if (filenames.getFullPathName("featureQCComponentGroups_csv_i").size()) {
        featureQCFile.load(filenames.getFullPathName("featureQCComponentGroups_csv_i"), sequenceSegmentHandler_IO.getFeatureQC(), true);
        if (sequence_segment_observable_) sequence_segment_observable_->notifyFeatureQCComponentGroupsUpdated();
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

  ParameterSet StoreFeatureFilters::getParameterSchema() const
  {
    return ParameterSet();
  }

  void StoreFeatureFilters::getInputsOutputs(Filenames& filenames) const
  {
    filenames.addFileName("featureFilterComponents_csv_o", "_featureFilterComponents.csv", Filenames::FileScope::EFileScopeInjectionOutput);
    filenames.addFileName("featureFilterComponentGroups_csv_o", "_featureFilterComponentGroups.csv", Filenames::FileScope::EFileScopeInjectionOutput);
  };

  void StoreFeatureFilters::process(
    SequenceSegmentHandler& sequenceSegmentHandler_IO,
    const SequenceHandler& sequenceHandler_I,
    const ParameterSet& params_I,
    const Filenames& filenames_override
  ) const
  {
    LOGD << "START storeFeatureFilter";
    Filenames filenames = prepareFileNames(filenames_override);

    LOGI << "Storing: " << filenames.getFullPathName("featureFilterComponents_csv_o") << " and " <<
      filenames.getFullPathName("featureFilterComponentGroups_csv_o");

    if (filenames.getFullPathName("featureFilterComponents_csv_o").empty() &&
      filenames.getFullPathName("featureFilterComponentGroups_csv_o").empty()) {
      LOGE << "Filenames are both empty";
      LOGD << "END storeFeatureFilter";
      return;
    }

    try {
      OpenMS::MRMFeatureQCFile featureQCFile;
      if (filenames.getFullPathName("featureFilterComponents_csv_o").size()) { // because we don't know if either of the two names is empty
        featureQCFile.store(filenames.getFullPathName("featureFilterComponents_csv_o"), sequenceSegmentHandler_IO.getFeatureFilter(), false);
      }
      if (filenames.getFullPathName("featureFilterComponentGroups_csv_o").size()) {
        featureQCFile.store(filenames.getFullPathName("featureFilterComponentGroups_csv_o"), sequenceSegmentHandler_IO.getFeatureFilter(), true);
      }
    }
    catch (const std::exception& e) {
      LOGE << e.what();
      LOGI << "feature filter store exception";
    }

    LOGD << "END storeFeatureFilter";
  }

  ParameterSet StoreFeatureQCs::getParameterSchema() const
  {
    return ParameterSet();
  }

  void StoreFeatureQCs::getInputsOutputs(Filenames& filenames) const
  {
    filenames.addFileName("featureQCComponents_csv_o", "_featureQCComponents.csv", Filenames::FileScope::EFileScopeInjectionOutput);
    filenames.addFileName("featureQCComponentGroups_csv_o", "_featureQCComponentGroups.csv", Filenames::FileScope::EFileScopeInjectionOutput);
  };

  void StoreFeatureQCs::process(
    SequenceSegmentHandler& sequenceSegmentHandler_IO,
    const SequenceHandler& sequenceHandler_I,
    const ParameterSet& params_I,
    const Filenames& filenames_override
  ) const
  {
    LOGD << "START storeFeatureQC";
    Filenames filenames = prepareFileNames(filenames_override);

    LOGI << "Loading: " << filenames.getFullPathName("featureQCComponents_csv_o") << " and " <<
      filenames.getFullPathName("featureQCComponentGroups_csv_o");

    if (filenames.getFullPathName("featureQCComponents_csv_o").empty() &&
      filenames.getFullPathName("featureQCComponentGroups_csv_o").empty()) {
      LOGE << "Filenames are both empty";
      LOGD << "END storeFeatureQC";
      return;
    }

    try {
      OpenMS::MRMFeatureQCFile featureQCFile;
      if (filenames.getFullPathName("featureQCComponents_csv_o").size()) { // because we don't know if either of the two names is empty
        featureQCFile.store(filenames.getFullPathName("featureQCComponents_csv_o"), sequenceSegmentHandler_IO.getFeatureQC(), false);
      }
      if (filenames.getFullPathName("featureQCComponentGroups_csv_o").size()) {
        featureQCFile.store(filenames.getFullPathName("featureQCComponentGroups_csv_o"), sequenceSegmentHandler_IO.getFeatureQC(), true);
      }
    }
    catch (const std::exception& e) {
      LOGE << e.what();
      LOGI << "Feature qc store exception";
    }

    LOGD << "END storeFeatureQC";
  }

  ParameterSet LoadFeatureRSDFilters::getParameterSchema() const
  {
    return ParameterSet();
  }

  void LoadFeatureRSDFilters::getInputsOutputs(Filenames& filenames) const
  {
    filenames.addFileName("featureRSDFilterComponents_csv_i", "featureRSDFilterComponents.csv", Filenames::FileScope::EFileScopeMain);
    filenames.addFileName("featureRSDFilterComponentGroups_csv_i", "featureRSDFilterComponentGroups.csv", Filenames::FileScope::EFileScopeMain);
  };

  bool LoadFeatureRSDFilters::onFilePicked(const std::string& filename, ApplicationHandler* application_handler)
  {
    if (application_handler->sequenceHandler_.getSequence().size() == 0)
    {
      LOGE << "File cannot be loaded without first loading the sequence.";
      return false;
    }
    Filenames filenames;
    if (component_group_)
    {
      filenames.setFullPathName("featureRSDFilterComponents_csv_i", "");
      filenames.setFullPathName("featureRSDFilterComponentGroups_csv_i", filename);
    }
    else
    {
      filenames.setFullPathName("featureRSDFilterComponents_csv_i", filename);
      filenames.setFullPathName("featureRSDFilterComponentGroups_csv_i", "");
    }
    for (SequenceSegmentHandler& sequenceSegmentHandler : application_handler->sequenceHandler_.getSequenceSegments()) {
      sequence_segment_observable_ = &(application_handler->sequenceHandler_);
      process(sequenceSegmentHandler, SequenceHandler(), {}, filenames);
    }
    return true;
  }

  void LoadFeatureRSDFilters::process(
    SequenceSegmentHandler& sequenceSegmentHandler_IO,
    const SequenceHandler& sequenceHandler_I,
    const ParameterSet& params_I,
    const Filenames& filenames_override
  ) const
  {
    LOGD << "START loadFeatureRSDFilter";
    Filenames filenames = prepareFileNames(filenames_override);

    LOGI << "Loading: " << filenames.getFullPathName("featureRSDFilterComponents_csv_i") << " and " <<
      filenames.getFullPathName("featureRSDFilterComponentGroups_csv_i");

    if (filenames.getFullPathName("featureRSDFilterComponents_csv_i").empty() &&
      filenames.getFullPathName("featureRSDFilterComponentGroups_csv_i").empty()) {
      LOGE << "Filenames are both empty";
      LOGD << "END loadFeatureRSDFilter";
      return;
    }

    if (filenames.getFullPathName("featureRSDFilterComponents_csv_i").size() &&
      !InputDataValidation::fileExists(filenames.getFullPathName("featureRSDFilterComponents_csv_i"))) {
      LOGE << "File not found: " << filenames.getFullPathName("featureRSDFilterComponents_csv_i");
      LOGD << "END loadFeatureRSDFilter";
      return;
    }

    if (filenames.getFullPathName("featureRSDFilterComponentGroups_csv_i").size() &&
      !InputDataValidation::fileExists(filenames.getFullPathName("featureRSDFilterComponentGroups_csv_i"))) {
      LOGE << "File not found: " << filenames.getFullPathName("featureRSDFilterComponentGroups_csv_i");
      LOGD << "END loadFeatureRSDFilter";
      return;
    }

    try {
      OpenMS::MRMFeatureQCFile featureQCFile;
      if (filenames.getFullPathName("featureRSDFilterComponents_csv_i").size()) { // because we don't know if either of the two names is empty
        featureQCFile.load(filenames.getFullPathName("featureRSDFilterComponents_csv_i"), sequenceSegmentHandler_IO.getFeatureRSDFilter(), false);
        if (sequence_segment_observable_) sequence_segment_observable_->notifyFeatureRSDFilterComponentsUpdated();
      }
      if (filenames.getFullPathName("featureRSDFilterComponentGroups_csv_i").size()) {
        featureQCFile.load(filenames.getFullPathName("featureRSDFilterComponentGroups_csv_i"), sequenceSegmentHandler_IO.getFeatureRSDFilter(), true);
        if (sequence_segment_observable_) sequence_segment_observable_->notifyFeatureRSDFilterComponentGroupsUpdated();
      }
    }
    catch (const std::exception& e) {
      LOGE << e.what();
      sequenceSegmentHandler_IO.getFeatureRSDFilter().component_qcs.clear();
      sequenceSegmentHandler_IO.getFeatureRSDFilter().component_group_qcs.clear();
      sequenceSegmentHandler_IO.getFeatureRSDFilter().component_group_pair_qcs.clear();
      LOGI << "feature filter clear";
    }
    LOGD << "END loadFeatureRSDFilter";
  }

  ParameterSet LoadFeatureRSDQCs::getParameterSchema() const
  {
    return ParameterSet();
  }

  void LoadFeatureRSDQCs::getInputsOutputs(Filenames& filenames) const
  {
    filenames.addFileName("featureRSDQCComponents_csv_i", "featureRSDQCComponents.csv", Filenames::FileScope::EFileScopeMain);
    filenames.addFileName("featureRSDQCComponentGroups_csv_i", "featureRSDQCComponentGroups.csv", Filenames::FileScope::EFileScopeMain);
  };

  bool LoadFeatureRSDQCs::onFilePicked(const std::string& filename, ApplicationHandler* application_handler)
  {
    if (application_handler->sequenceHandler_.getSequence().size() == 0)
    {
      LOGE << "File cannot be loaded without first loading the sequence.";
      return false;
    }
    Filenames filenames;
    if (component_group_)
    {
      filenames.setFullPathName("featureRSDQCComponents_csv_i", "");
      filenames.setFullPathName("featureRSDQCComponentGroups_csv_i", filename);
    }
    else
    {
      filenames.setFullPathName("featureRSDQCComponents_csv_i", filename);
      filenames.setFullPathName("featureRSDQCComponentGroups_csv_i", "");
    }
    for (SequenceSegmentHandler& sequenceSegmentHandler : application_handler->sequenceHandler_.getSequenceSegments()) {
      sequence_segment_observable_ = &(application_handler->sequenceHandler_);
      process(sequenceSegmentHandler, SequenceHandler(), {}, filenames);
    }
    return true;
  }

  void LoadFeatureRSDQCs::process(
    SequenceSegmentHandler& sequenceSegmentHandler_IO,
    const SequenceHandler& sequenceHandler_I,
    const ParameterSet& params_I,
    const Filenames& filenames_override
  ) const
  {
    LOGD << "START loadFeatureRSDQC";
    Filenames filenames = prepareFileNames(filenames_override);

    LOGI << "Loading: " << filenames.getFullPathName("featureRSDQCComponents_csv_i") << " and " <<
      filenames.getFullPathName("featureRSDQCComponentGroups_csv_i");

    if (filenames.getFullPathName("featureRSDQCComponents_csv_i").empty() &&
      filenames.getFullPathName("featureRSDQCComponentGroups_csv_i").empty()) {
      LOGE << "Filenames are both empty";
      LOGD << "END loadFeatureRSDQC";
      return;
    }

    if (filenames.getFullPathName("featureRSDQCComponents_csv_i").size() &&
      !InputDataValidation::fileExists(filenames.getFullPathName("featureRSDQCComponents_csv_i"))) {
      LOGE << "File not found: " << filenames.getFullPathName("featureRSDQCComponents_csv_i");
      LOGD << "END loadFeatureRSDQC";
      return;
    }

    if (filenames.getFullPathName("featureRSDQCComponentGroups_csv_i").size() &&
      !InputDataValidation::fileExists(filenames.getFullPathName("featureRSDQCComponentGroups_csv_i"))) {
      LOGE << "File not found: " << filenames.getFullPathName("featureRSDQCComponentGroups_csv_i");
      LOGD << "END loadFeatureRSDQC";
      return;
    }

    try {
      OpenMS::MRMFeatureQCFile featureQCFile;
      if (filenames.getFullPathName("featureRSDQCComponents_csv_i").size()) { // because we don't know if either of the two names is empty
        featureQCFile.load(filenames.getFullPathName("featureRSDQCComponents_csv_i"), sequenceSegmentHandler_IO.getFeatureRSDQC(), false);
        if (sequence_segment_observable_) sequence_segment_observable_->notifyFeatureRSDQCComponentsUpdated();
      }
      if (filenames.getFullPathName("featureRSDQCComponentGroups_csv_i").size()) {
        featureQCFile.load(filenames.getFullPathName("featureRSDQCComponentGroups_csv_i"), sequenceSegmentHandler_IO.getFeatureRSDQC(), true);
        if (sequence_segment_observable_) sequence_segment_observable_->notifyFeatureRSDQCComponentGroupsUpdated();
      }
    }
    catch (const std::exception& e) {
      LOGE << e.what();
      sequenceSegmentHandler_IO.getFeatureRSDQC().component_qcs.clear();
      sequenceSegmentHandler_IO.getFeatureRSDQC().component_group_qcs.clear();
      sequenceSegmentHandler_IO.getFeatureRSDQC().component_group_pair_qcs.clear();
      LOGI << "Feature qc clear";
    }
    LOGD << "END loadFeatureRSDQC";
  }

  ParameterSet StoreFeatureRSDFilters::getParameterSchema() const
  {
    return ParameterSet();
  }

  void StoreFeatureRSDFilters::getInputsOutputs(Filenames& filenames) const
  {
    filenames.addFileName("featureRSDFilterComponents_csv_o", "_featureRSDFilterComponents.csv", Filenames::FileScope::EFileScopeInjectionOutput);
    filenames.addFileName("featureRSDFilterComponentGroups_csv_o", "_featureRSDFilterComponentGroups.csv", Filenames::FileScope::EFileScopeInjectionOutput);
  };

  void StoreFeatureRSDFilters::process(
    SequenceSegmentHandler& sequenceSegmentHandler_IO,
    const SequenceHandler& sequenceHandler_I,
    const ParameterSet& params_I,
    const Filenames& filenames_override
  ) const
  {
    LOGD << "START storeFeatureRSDFilter";
    Filenames filenames = prepareFileNames(filenames_override);

    LOGI << "Storing: " << filenames.getFullPathName("featureRSDFilterComponents_csv_o") << " and " <<
      filenames.getFullPathName("featureRSDFilterComponentGroups_csv_o");

    if (filenames.getFullPathName("featureRSDFilterComponents_csv_o").empty() &&
      filenames.getFullPathName("featureRSDFilterComponentGroups_csv_o").empty()) {
      LOGE << "Filenames are both empty";
      LOGD << "END storeFeatureRSDFilter";
      return;
    }

    try {
      OpenMS::MRMFeatureQCFile featureQCFile;
      if (filenames.getFullPathName("featureRSDFilterComponents_csv_o").size()) { // because we don't know if either of the two names is empty
        featureQCFile.store(filenames.getFullPathName("featureRSDFilterComponents_csv_o"), sequenceSegmentHandler_IO.getFeatureRSDFilter(), false);
      }
      if (filenames.getFullPathName("featureRSDFilterComponentGroups_csv_o").size()) {
        featureQCFile.store(filenames.getFullPathName("featureRSDFilterComponentGroups_csv_o"), sequenceSegmentHandler_IO.getFeatureRSDFilter(), true);
      }
    }
    catch (const std::exception& e) {
      LOGE << e.what();
      LOGI << "feature filter store exception";
    }

    LOGD << "END storeFeatureRSDFilter";
  }

  ParameterSet StoreFeatureRSDQCs::getParameterSchema() const
  {
    return ParameterSet();
  }

  void StoreFeatureRSDQCs::getInputsOutputs(Filenames& filenames) const
  {
    filenames.addFileName("featureRSDQCComponents_csv_o", "_featureRSDQCComponents.csv", Filenames::FileScope::EFileScopeInjectionOutput);
    filenames.addFileName("featureRSDQCComponentGroups_csv_o", "_featureRSDQCComponentGroups.csv", Filenames::FileScope::EFileScopeInjectionOutput);
  };

  void StoreFeatureRSDQCs::process(
    SequenceSegmentHandler& sequenceSegmentHandler_IO,
    const SequenceHandler& sequenceHandler_I,
    const ParameterSet& params_I,
    const Filenames& filenames_override
  ) const
  {
    LOGD << "START storeFeatureRSDQC";
    Filenames filenames = prepareFileNames(filenames_override);

    LOGI << "Loading: " << filenames.getFullPathName("featureRSDQCComponents_csv_o") << " and " <<
      filenames.getFullPathName("featureRSDQCComponentGroups_csv_o");

    if (filenames.getFullPathName("featureRSDQCComponents_csv_o").empty() &&
      filenames.getFullPathName("featureRSDQCComponentGroups_csv_o").empty()) {
      LOGE << "Filenames are both empty";
      LOGD << "END storeFeatureRSDQC";
      return;
    }

    try {
      OpenMS::MRMFeatureQCFile featureQCFile;
      if (filenames.getFullPathName("featureRSDQCComponents_csv_o").size()) { // because we don't know if either of the two names is empty
        featureQCFile.store(filenames.getFullPathName("featureRSDQCComponents_csv_o"), sequenceSegmentHandler_IO.getFeatureRSDQC(), false);
      }
      if (filenames.getFullPathName("featureRSDQCComponentGroups_csv_o").size()) {
        featureQCFile.store(filenames.getFullPathName("featureRSDQCComponentGroups_csv_o"), sequenceSegmentHandler_IO.getFeatureRSDQC(), true);
      }
    }
    catch (const std::exception& e) {
      LOGE << e.what();
      LOGI << "Feature qc store exception";
    }

    LOGD << "END storeFeatureRSDQC";
  }

  ParameterSet LoadFeatureBackgroundFilters::getParameterSchema() const
  {
    return ParameterSet();
  }

  void LoadFeatureBackgroundFilters::getInputsOutputs(Filenames& filenames) const
  {
    filenames.addFileName("featureBackgroundFilterComponents_csv_i", "featureBackgroundFilterComponents.csv", Filenames::FileScope::EFileScopeMain);
    filenames.addFileName("featureBackgroundFilterComponentGroups_csv_i", "featureBackgroundFilterComponentGroups.csv", Filenames::FileScope::EFileScopeMain);
  };

  bool LoadFeatureBackgroundFilters::onFilePicked(const std::string& filename, ApplicationHandler* application_handler)
  {
    if (application_handler->sequenceHandler_.getSequence().size() == 0)
    {
      LOGE << "File cannot be loaded without first loading the sequence.";
      return false;
    }
    Filenames filenames;
    if (component_group_)
    {
      filenames.setFullPathName("featureBackgroundFilterComponents_csv_i", "");
      filenames.setFullPathName("featureBackgroundFilterComponentGroups_csv_i", filename);
    }
    else
    {
      filenames.setFullPathName("featureBackgroundFilterComponents_csv_i", filename);
      filenames.setFullPathName("featureBackgroundFilterComponentGroups_csv_i", "");
    }
    for (SequenceSegmentHandler& sequenceSegmentHandler : application_handler->sequenceHandler_.getSequenceSegments()) {
      sequence_segment_observable_ = &(application_handler->sequenceHandler_);
      process(sequenceSegmentHandler, SequenceHandler(), {}, filenames);
    }
    return true;
  }

  void LoadFeatureBackgroundFilters::process(
    SequenceSegmentHandler& sequenceSegmentHandler_IO,
    const SequenceHandler& sequenceHandler_I,
    const ParameterSet& params_I,
    const Filenames& filenames_override
  ) const
  {
    LOGD << "START loadFeatureBackgroundFilter";
    Filenames filenames = prepareFileNames(filenames_override);

    LOGI << "Loading: " << filenames.getFullPathName("featureBackgroundFilterComponents_csv_i") << " and " <<
      filenames.getFullPathName("featureBackgroundFilterComponentGroups_csv_i");

    if (filenames.getFullPathName("featureBackgroundFilterComponents_csv_i").empty() &&
      filenames.getFullPathName("featureBackgroundFilterComponentGroups_csv_i").empty()) {
      LOGE << "Filenames are both empty";
      LOGD << "END loadFeatureBackgroundFilter";
      return;
    }

    if (filenames.getFullPathName("featureBackgroundFilterComponents_csv_i").size() &&
      !InputDataValidation::fileExists(filenames.getFullPathName("featureBackgroundFilterComponents_csv_i"))) {
      LOGE << "File not found: " << filenames.getFullPathName("featureBackgroundFilterComponents_csv_i");
      LOGD << "END loadFeatureBackgroundFilter";
      return;
    }

    if (filenames.getFullPathName("featureBackgroundFilterComponentGroups_csv_i").size() &&
      !InputDataValidation::fileExists(filenames.getFullPathName("featureBackgroundFilterComponentGroups_csv_i"))) {
      LOGE << "File not found: " << filenames.getFullPathName("featureBackgroundFilterComponentGroups_csv_i");
      LOGD << "END loadFeatureBackgroundFilter";
      return;
    }

    try {
      OpenMS::MRMFeatureQCFile featureQCFile;
      if (filenames.getFullPathName("featureBackgroundFilterComponents_csv_i").size()) { // because we don't know if either of the two names is empty
        featureQCFile.load(filenames.getFullPathName("featureBackgroundFilterComponents_csv_i"), sequenceSegmentHandler_IO.getFeatureBackgroundFilter(), false);
        if (sequence_segment_observable_) sequence_segment_observable_->notifyFeatureBackgroundFilterComponentsUpdated();
      }
      if (filenames.getFullPathName("featureBackgroundFilterComponentGroups_csv_i").size()) {
        featureQCFile.load(filenames.getFullPathName("featureBackgroundFilterComponentGroups_csv_i"), sequenceSegmentHandler_IO.getFeatureBackgroundFilter(), true);
        if (sequence_segment_observable_) sequence_segment_observable_->notifyFeatureBackgroundFilterComponentGroupsUpdated();
      }
    }
    catch (const std::exception& e) {
      LOGE << e.what();
      sequenceSegmentHandler_IO.getFeatureBackgroundFilter().component_qcs.clear();
      sequenceSegmentHandler_IO.getFeatureBackgroundFilter().component_group_qcs.clear();
      sequenceSegmentHandler_IO.getFeatureBackgroundFilter().component_group_pair_qcs.clear();
      LOGI << "feature filter clear";
    }
    LOGD << "END loadFeatureBackgroundFilter";
  }

  ParameterSet LoadFeatureBackgroundQCs::getParameterSchema() const
  {
    return ParameterSet();
  }

  void LoadFeatureBackgroundQCs::getInputsOutputs(Filenames& filenames) const
  {
    filenames.addFileName("featureBackgroundQCComponents_csv_i", "featureBackgroundQCComponents.csv", Filenames::FileScope::EFileScopeMain);
    filenames.addFileName("featureBackgroundQCComponentGroups_csv_i", "featureBackgroundQCComponentGroups.csv", Filenames::FileScope::EFileScopeMain);
  };

  bool LoadFeatureBackgroundQCs::onFilePicked(const std::string& filename, ApplicationHandler* application_handler)
  {
    if (application_handler->sequenceHandler_.getSequence().size() == 0)
    {
      LOGE << "File cannot be loaded without first loading the sequence.";
      return false;
    }
    Filenames filenames;
    if (component_group_)
    {
      filenames.setFullPathName("featureBackgroundQCComponents_csv_i", "");
      filenames.setFullPathName("featureBackgroundQCComponentGroups_csv_i", filename);
    }
    else
    {
      filenames.setFullPathName("featureBackgroundQCComponents_csv_i", filename);
      filenames.setFullPathName("featureBackgroundQCComponentGroups_csv_i", "");
    }
    for (SequenceSegmentHandler& sequenceSegmentHandler : application_handler->sequenceHandler_.getSequenceSegments()) {
      sequence_segment_observable_ = &(application_handler->sequenceHandler_);
      process(sequenceSegmentHandler, SequenceHandler(), {}, filenames);
    }
    return true;
  }

  void LoadFeatureBackgroundQCs::process(
    SequenceSegmentHandler& sequenceSegmentHandler_IO,
    const SequenceHandler& sequenceHandler_I,
    const ParameterSet& params_I,
    const Filenames& filenames_override
  ) const
  {
    LOGD << "START loadFeatureBackgroundQC";
    Filenames filenames = prepareFileNames(filenames_override);

    LOGI << "Loading: " << filenames.getFullPathName("featureBackgroundQCComponents_csv_i") << " and " <<
      filenames.getFullPathName("featureBackgroundQCComponentGroups_csv_i");

    if (filenames.getFullPathName("featureBackgroundQCComponents_csv_i").empty() &&
      filenames.getFullPathName("featureBackgroundQCComponentGroups_csv_i").empty()) {
      LOGE << "Filenames are both empty";
      LOGD << "END loadFeatureBackgroundQC";
      return;
    }

    if (filenames.getFullPathName("featureBackgroundQCComponents_csv_i").size() &&
      !InputDataValidation::fileExists(filenames.getFullPathName("featureBackgroundQCComponents_csv_i"))) {
      LOGE << "File not found: " << filenames.getFullPathName("featureBackgroundQCComponents_csv_i");
      LOGD << "END loadFeatureBackgroundQC";
      return;
    }

    if (filenames.getFullPathName("featureBackgroundQCComponentGroups_csv_i").size() &&
      !InputDataValidation::fileExists(filenames.getFullPathName("featureBackgroundQCComponentGroups_csv_i"))) {
      LOGE << "File not found: " << filenames.getFullPathName("featureBackgroundQCComponentGroups_csv_i");
      LOGD << "END loadFeatureBackgroundQC";
      return;
    }

    try {
      OpenMS::MRMFeatureQCFile featureQCFile;
      if (filenames.getFullPathName("featureBackgroundQCComponents_csv_i").size()) { // because we don't know if either of the two names is empty
        featureQCFile.load(filenames.getFullPathName("featureBackgroundQCComponents_csv_i"), sequenceSegmentHandler_IO.getFeatureBackgroundQC(), false);
        if (sequence_segment_observable_) sequence_segment_observable_->notifyFeatureBackgroundQCComponentsUpdated();
      }
      if (filenames.getFullPathName("featureBackgroundQCComponentGroups_csv_i").size()) {
        featureQCFile.load(filenames.getFullPathName("featureBackgroundQCComponentGroups_csv_i"), sequenceSegmentHandler_IO.getFeatureBackgroundQC(), true);
        if (sequence_segment_observable_) sequence_segment_observable_->notifyFeatureBackgroundQCComponentGroupsUpdated();
      }
    }
    catch (const std::exception& e) {
      LOGE << e.what();
      sequenceSegmentHandler_IO.getFeatureBackgroundQC().component_qcs.clear();
      sequenceSegmentHandler_IO.getFeatureBackgroundQC().component_group_qcs.clear();
      sequenceSegmentHandler_IO.getFeatureBackgroundQC().component_group_pair_qcs.clear();
      LOGI << "Feature qc clear";
    }
    LOGD << "END loadFeatureBackgroundQC";
  }

  ParameterSet StoreFeatureBackgroundFilters::getParameterSchema() const
  {
    return ParameterSet();
  }

  void StoreFeatureBackgroundFilters::getInputsOutputs(Filenames& filenames) const
  {
    filenames.addFileName("featureBackgroundFilterComponents_csv_o", "_featureBackgroundFilterComponents.csv", Filenames::FileScope::EFileScopeInjectionOutput);
    filenames.addFileName("featureBackgroundFilterComponentGroups_csv_o", "_featureBackgroundFilterComponentGroups.csv", Filenames::FileScope::EFileScopeInjectionOutput);
  };

  void StoreFeatureBackgroundFilters::process(
    SequenceSegmentHandler& sequenceSegmentHandler_IO,
    const SequenceHandler& sequenceHandler_I,
    const ParameterSet& params_I,
    const Filenames& filenames_override
  ) const
  {
    LOGD << "START storeFeatureBackgroundFilter";
    Filenames filenames = prepareFileNames(filenames_override);

    LOGI << "Storing: " << filenames.getFullPathName("featureBackgroundFilterComponents_csv_o") << " and " <<
      filenames.getFullPathName("featureBackgroundFilterComponentGroups_csv_o");

    if (filenames.getFullPathName("featureBackgroundFilterComponents_csv_o").empty() &&
      filenames.getFullPathName("featureBackgroundFilterComponentGroups_csv_o").empty()) {
      LOGE << "Filenames are both empty";
      LOGD << "END storeFeatureBackgroundFilter";
      return;
    }

    try {
      OpenMS::MRMFeatureQCFile featureQCFile;
      if (filenames.getFullPathName("featureBackgroundFilterComponents_csv_o").size()) { // because we don't know if either of the two names is empty
        featureQCFile.store(filenames.getFullPathName("featureBackgroundFilterComponents_csv_o"), sequenceSegmentHandler_IO.getFeatureBackgroundFilter(), false);
      }
      if (filenames.getFullPathName("featureBackgroundFilterComponentGroups_csv_o").size()) {
        featureQCFile.store(filenames.getFullPathName("featureBackgroundFilterComponentGroups_csv_o"), sequenceSegmentHandler_IO.getFeatureBackgroundFilter(), true);
      }
    }
    catch (const std::exception& e) {
      LOGE << e.what();
      LOGI << "feature filter store exception";
    }

    LOGD << "END storeFeatureBackgroundFilter";
  }

  ParameterSet StoreFeatureBackgroundQCs::getParameterSchema() const
  {
    return ParameterSet();
  }

  void StoreFeatureBackgroundQCs::getInputsOutputs(Filenames& filenames) const
  {
    filenames.addFileName("featureBackgroundQCComponents_csv_o", "_featureBackgroundQCComponents.csv", Filenames::FileScope::EFileScopeInjectionOutput);
    filenames.addFileName("featureBackgroundQCComponentGroups_csv_o", "_featureBackgroundQCComponentGroups.csv", Filenames::FileScope::EFileScopeInjectionOutput);
  };

  void StoreFeatureBackgroundQCs::process(
    SequenceSegmentHandler& sequenceSegmentHandler_IO,
    const SequenceHandler& sequenceHandler_I,
    const ParameterSet& params_I,
    const Filenames& filenames_override
  ) const
  {
    LOGD << "START storeFeatureBackgroundQC";
    Filenames filenames = prepareFileNames(filenames_override);

    LOGI << "Loading: " << filenames.getFullPathName("featureBackgroundQCComponents_csv_o") << " and " <<
      filenames.getFullPathName("featureBackgroundQCComponentGroups_csv_o");

    if (filenames.getFullPathName("featureBackgroundQCComponents_csv_o").empty() &&
      filenames.getFullPathName("featureBackgroundQCComponentGroups_csv_o").empty()) {
      LOGE << "Filenames are both empty";
      LOGD << "END storeFeatureBackgroundQC";
      return;
    }

    try {
      OpenMS::MRMFeatureQCFile featureQCFile;
      if (filenames.getFullPathName("featureBackgroundQCComponents_csv_o").size()) { // because we don't know if either of the two names is empty
        featureQCFile.store(filenames.getFullPathName("featureBackgroundQCComponents_csv_o"), sequenceSegmentHandler_IO.getFeatureBackgroundQC(), false);
      }
      if (filenames.getFullPathName("featureBackgroundQCComponentGroups_csv_o").size()) {
        featureQCFile.store(filenames.getFullPathName("featureBackgroundQCComponentGroups_csv_o"), sequenceSegmentHandler_IO.getFeatureBackgroundQC(), true);
      }
    }
    catch (const std::exception& e) {
      LOGE << e.what();
      LOGI << "Feature qc store exception";
    }

    LOGD << "END storeFeatureBackgroundQC";
  }

  ParameterSet EstimateFeatureFilterValues::getParameterSchema() const
  {
    return ParameterSet();
  }

  void EstimateFeatureFilterValues::process(
    SequenceSegmentHandler& sequenceSegmentHandler_IO,
    const SequenceHandler& sequenceHandler_I,
    const ParameterSet& params_I,
    const Filenames& filenames_override
  ) const
  {
    LOGD << "START estimateFeatureFilterValues";
    Filenames filenames = prepareFileNames(filenames_override);


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

    OpenMS::MRMFeatureFilter featureFilter;
    featureFilter.EstimateDefaultMRMFeatureQCValues(
      standards_featureMaps,
      sequenceSegmentHandler_IO.getFeatureFilter(),
      sequenceHandler_I.getSequence().front().getRawData().getTargetedExperiment(), // Targeted experiment used by all injections in the sequence
      true
    );

    LOGD << "END estimateFeatureFilterValues";
  }

  ParameterSet EstimateFeatureQCValues::getParameterSchema() const
  {
    return ParameterSet();
  }

  void EstimateFeatureQCValues::process(
    SequenceSegmentHandler& sequenceSegmentHandler_IO,
    const SequenceHandler& sequenceHandler_I,
    const ParameterSet& params_I,
    const Filenames& filenames_override
  ) const
  {
    LOGD << "START estimateFeatureQCValues";
    Filenames filenames = prepareFileNames(filenames_override);


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
      LOGD << "END estimateFeatureQCValues";
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

    OpenMS::MRMFeatureFilter featureFilter;
    featureFilter.EstimateDefaultMRMFeatureQCValues(
      standards_featureMaps,
      sequenceSegmentHandler_IO.getFeatureQC(),
      sequenceHandler_I.getSequence().front().getRawData().getTargetedExperiment(), // Targeted experiment used by all injections in the sequence
      true
    );

    LOGD << "END estimateFeatureQCValues";
  }

  ParameterSet TransferLOQToFeatureFilters::getParameterSchema() const
  {
    return ParameterSet();
  }

  void TransferLOQToFeatureFilters::process(
    SequenceSegmentHandler& sequenceSegmentHandler_IO,
    const SequenceHandler& sequenceHandler_I,
    const ParameterSet& params_I,
    const Filenames& filenames_override
  ) const
  {
    LOGD << "START TransferLOQToFeatureFilters";
    Filenames filenames = prepareFileNames(filenames_override);


    // check if there are any quantitation methods
    if (sequenceSegmentHandler_IO.getQuantitationMethods().empty()) {
      LOGE << "quantitation methods is empty. Returning";
      LOGD << "END TransferLOQToFeatureFilters";
      return;
    }

    OpenMS::MRMFeatureFilter featureFilter;
    featureFilter.TransferLLOQAndULOQToCalculatedConcentrationBounds(
      sequenceSegmentHandler_IO.getQuantitationMethods(),
      sequenceSegmentHandler_IO.getFeatureFilter()
    );

    LOGD << "END TransferLOQToFeatureFilters";
  }

  ParameterSet TransferLOQToFeatureQCs::getParameterSchema() const
  {
    return ParameterSet();
  }

  void TransferLOQToFeatureQCs::process(
    SequenceSegmentHandler& sequenceSegmentHandler_IO,
    const SequenceHandler& sequenceHandler_I,
    const ParameterSet& params_I,
    const Filenames& filenames_override
  ) const
  {
    LOGD << "START TransferLOQToFeatureQCs";
    Filenames filenames = prepareFileNames(filenames_override);


    // check if there are any quantitation methods
    if (sequenceSegmentHandler_IO.getQuantitationMethods().empty()) {
      LOGE << "quantitation methods is empty. Returning";
      LOGD << "END TransferLOQToFeatureQCs";
      return;
    }

    OpenMS::MRMFeatureFilter featureFilter;
    featureFilter.TransferLLOQAndULOQToCalculatedConcentrationBounds(
      sequenceSegmentHandler_IO.getQuantitationMethods(),
      sequenceSegmentHandler_IO.getFeatureQC()
    );

    LOGD << "END TransferLOQToFeatureQCs";
  }

  ParameterSet EstimateFeatureRSDs::getParameterSchema() const
  {
    return ParameterSet();
  }

  void EstimateFeatureRSDs::process(
    SequenceSegmentHandler& sequenceSegmentHandler_IO,
    const SequenceHandler& sequenceHandler_I,
    const ParameterSet& params_I,
    const Filenames& filenames_override
  ) const
  {
    LOGD << "START EstimateFeatureRSDs";
    Filenames filenames = prepareFileNames(filenames_override);


    // get all QCs
    std::vector<size_t> qcs_indices;
    this->getSampleIndicesBySampleType(
      sequenceSegmentHandler_IO,
      sequenceHandler_I,
      SampleType::QC,
      qcs_indices
    );

    // check if there are any standards or QCs to estimate the feature filter parameters from
    if (qcs_indices.empty()) {
      LOGE << "qcs_indices argument is empty. Returning";
      LOGD << "END EstimateFeatureRSDs";
      return;
    }

    std::vector<OpenMS::FeatureMap> qcs_featureMaps;
    for (const size_t index : qcs_indices) {
      qcs_featureMaps.push_back(sequenceHandler_I.getSequence().at(index).getRawData().getFeatureMap());
    }

    OpenMS::MRMFeatureFilter featureFilter;
    OpenMS::MRMFeatureQC rsd_estimations = sequenceSegmentHandler_IO.getFeatureRSDFilter();
    featureFilter.EstimatePercRSD(
      qcs_featureMaps, 
      rsd_estimations,
      sequenceHandler_I.getSequence().front().getRawData().getTargetedExperiment() // Targeted experiment used by all injections in the sequence
    );
    sequenceSegmentHandler_IO.getFeatureRSDEstimations() = rsd_estimations; // Transfer over the estimations

    LOGD << "END EstimateFeatureRSDs";
  }

  ParameterSet EstimateFeatureBackgroundInterferences::getParameterSchema() const
  {
    return ParameterSet();
  }

  void EstimateFeatureBackgroundInterferences::process(
    SequenceSegmentHandler& sequenceSegmentHandler_IO,
    const SequenceHandler& sequenceHandler_I,
    const ParameterSet& params_I,
    const Filenames& filenames_override
  ) const
  {
    LOGD << "START EstimateFeatureBackgroundInterferences";
    Filenames filenames = prepareFileNames(filenames_override);


    // get all Blanks
    std::vector<size_t> blanks_indices;
    this->getSampleIndicesBySampleType(
      sequenceSegmentHandler_IO,
      sequenceHandler_I,
      SampleType::Blank,
      blanks_indices
    );

    // check if there are any Blanks to estimate the background interference from
    if (blanks_indices.empty()) {
      LOGE << "blanks_indices argument is empty. Returning";
      LOGD << "END EstimateFeatureBackgroundInterferences";
      return;
    }

    std::vector<OpenMS::FeatureMap> blanks_featureMaps;
    for (const size_t index : blanks_indices) {
      blanks_featureMaps.push_back(sequenceHandler_I.getSequence().at(index).getRawData().getFeatureMap());
    }

    // Initialize with a zero filter
    OpenMS::MRMFeatureFilter featureFilter;
    featureFilter.zeroFilterValues(sequenceSegmentHandler_IO.getFeatureBackgroundEstimations(), sequenceSegmentHandler_IO.getFeatureBackgroundFilter());

    // Then estimate the background interferences
    featureFilter.EstimateBackgroundInterferences(
      blanks_featureMaps,
      sequenceSegmentHandler_IO.getFeatureBackgroundEstimations(),
      sequenceHandler_I.getSequence().front().getRawData().getTargetedExperiment() // Targeted experiment used by all injections in the sequence
    );

    LOGD << "END EstimateFeatureBackgroundInterferences";
  }

  ParameterSet LoadFeatureRSDEstimations::getParameterSchema() const
  {
    return ParameterSet();
  }

  void LoadFeatureRSDEstimations::getInputsOutputs(Filenames& filenames) const
  {
    filenames.addFileName("featureRSDEstimationComponents_csv_i", "featureRSDEstimationComponents.csv", Filenames::FileScope::EFileScopeMain);
    filenames.addFileName("featureRSDEstimationComponentGroups_csv_i", "featureRSDEstimationComponentGroups.csv", Filenames::FileScope::EFileScopeMain);
  };

  void LoadFeatureRSDEstimations::process(
    SequenceSegmentHandler& sequenceSegmentHandler_IO,
    const SequenceHandler& sequenceHandler_I,
    const ParameterSet& params_I,
    const Filenames& filenames_override
  ) const
  {
    LOGD << "START loadFeatureRSDEstimation";
    Filenames filenames = prepareFileNames(filenames_override);

    LOGI << "Loading: " << filenames.getFullPathName("featureRSDEstimationComponents_csv_i") << " and " <<
      filenames.getFullPathName("featureRSDEstimationComponentGroups_csv_i");

    if (filenames.getFullPathName("featureRSDEstimationComponents_csv_i").empty() &&
      filenames.getFullPathName("featureRSDEstimationComponentGroups_csv_i").empty()) {
      LOGE << "Filenames are both empty";
      LOGD << "END loadFeatureRSDEstimation";
      return;
    }

    if (filenames.getFullPathName("featureRSDEstimationComponents_csv_i").size() &&
      !InputDataValidation::fileExists(filenames.getFullPathName("featureRSDEstimationComponents_csv_i"))) {
      LOGE << "File not found: " << filenames.getFullPathName("featureRSDEstimationComponents_csv_i");
      LOGD << "END loadFeatureRSDEstimation";
      return;
    }

    if (filenames.getFullPathName("featureRSDEstimationComponentGroups_csv_i").size() &&
      !InputDataValidation::fileExists(filenames.getFullPathName("featureRSDEstimationComponentGroups_csv_i"))) {
      LOGE << "File not found: " << filenames.getFullPathName("featureRSDEstimationComponentGroups_csv_i");
      LOGD << "END loadFeatureRSDEstimation";
      return;
    }

    try {
      OpenMS::MRMFeatureQCFile featureQCFile;
      if (filenames.getFullPathName("featureRSDEstimationComponents_csv_i").size()) { // because we don't know if either of the two names is empty
        featureQCFile.load(filenames.getFullPathName("featureRSDEstimationComponents_csv_i"), sequenceSegmentHandler_IO.getFeatureRSDEstimations(), false);
      }
      if (filenames.getFullPathName("featureRSDEstimationComponentGroups_csv_i").size()) {
        featureQCFile.load(filenames.getFullPathName("featureRSDEstimationComponentGroups_csv_i"), sequenceSegmentHandler_IO.getFeatureRSDEstimations(), true);
      }
    }
    catch (const std::exception& e) {
      LOGE << e.what();
      sequenceSegmentHandler_IO.getFeatureRSDEstimations().component_qcs.clear();
      sequenceSegmentHandler_IO.getFeatureRSDEstimations().component_group_qcs.clear();
      sequenceSegmentHandler_IO.getFeatureRSDEstimations().component_group_pair_qcs.clear();
      LOGI << "feature filter clear";
    }

    LOGD << "END loadFeatureRSDEstimation";
  }

  ParameterSet StoreFeatureRSDEstimations::getParameterSchema() const
  {
    return ParameterSet();
  }

  void StoreFeatureRSDEstimations::getInputsOutputs(Filenames& filenames) const
  {
    filenames.addFileName("featureRSDEstimationComponents_csv_o", "_featureRSDEstimationComponents.csv", Filenames::FileScope::EFileScopeInjectionOutput);
    filenames.addFileName("featureRSDEstimationComponentGroups_csv_o", "_featureRSDEstimationComponentGroups.csv", Filenames::FileScope::EFileScopeInjectionOutput);
  };

  void StoreFeatureRSDEstimations::process(
    SequenceSegmentHandler& sequenceSegmentHandler_IO,
    const SequenceHandler& sequenceHandler_I,
    const ParameterSet& params_I,
    const Filenames& filenames_override
  ) const
  {
    LOGD << "START storeFeatureRSDEstimation";
    Filenames filenames = prepareFileNames(filenames_override);

    LOGI << "Storing: " << filenames.getFullPathName("featureRSDEstimationComponents_csv_o") << " and " <<
      filenames.getFullPathName("featureRSDEstimationComponentGroups_csv_o");

    if (filenames.getFullPathName("featureRSDEstimationComponents_csv_o").empty() &&
      filenames.getFullPathName("featureRSDEstimationComponentGroups_csv_o").empty()) {
      LOGE << "Filenames are both empty";
      LOGD << "END storeFeatureRSDEstimation";
      return;
    }

    try {
      OpenMS::MRMFeatureQCFile featureQCFile;
      if (filenames.getFullPathName("featureRSDEstimationComponents_csv_o").size()) { // because we don't know if either of the two names is empty
        featureQCFile.store(filenames.getFullPathName("featureRSDEstimationComponents_csv_o"), sequenceSegmentHandler_IO.getFeatureRSDEstimations(), false);
      }
      if (filenames.getFullPathName("featureRSDEstimationComponentGroups_csv_o").size()) {
        featureQCFile.store(filenames.getFullPathName("featureRSDEstimationComponentGroups_csv_o"), sequenceSegmentHandler_IO.getFeatureRSDEstimations(), true);
      }
    }
    catch (const std::exception& e) {
      LOGE << e.what();
      LOGI << "feature filter store exception";
    }

    LOGD << "END storeFeatureRSDEstimation";
  }

  ParameterSet LoadFeatureBackgroundEstimations::getParameterSchema() const
  {
    return ParameterSet();
  }

  void LoadFeatureBackgroundEstimations::getInputsOutputs(Filenames& filenames) const
  {
    filenames.addFileName("featureBackgroundEstimationComponents_csv_i", "featureBackgroundEstimationComponents.csv", Filenames::FileScope::EFileScopeMain);
    filenames.addFileName("featureBackgroundEstimationComponentGroups_csv_i", "featureBackgroundEstimationComponentGroups.csv", Filenames::FileScope::EFileScopeMain);
  };

  void LoadFeatureBackgroundEstimations::process(
    SequenceSegmentHandler& sequenceSegmentHandler_IO,
    const SequenceHandler& sequenceHandler_I,
    const ParameterSet& params_I,
    const Filenames& filenames_override
  ) const
  {
    LOGD << "START loadFeatureBackgroundEstimation";
    Filenames filenames = prepareFileNames(filenames_override);

    LOGI << "Loading: " << filenames.getFullPathName("featureBackgroundEstimationComponents_csv_i") << " and " <<
      filenames.getFullPathName("featureBackgroundEstimationComponentGroups_csv_i");

    if (filenames.getFullPathName("featureBackgroundEstimationComponents_csv_i").empty() &&
      filenames.getFullPathName("featureBackgroundEstimationComponentGroups_csv_i").empty()) {
      LOGE << "Filenames are both empty";
      LOGD << "END loadFeatureBackgroundEstimation";
      return;
    }

    if (filenames.getFullPathName("featureBackgroundEstimationComponents_csv_i").size() &&
      !InputDataValidation::fileExists(filenames.getFullPathName("featureBackgroundEstimationComponents_csv_i"))) {
      LOGE << "File not found: " << filenames.getFullPathName("featureBackgroundEstimationComponents_csv_i");
      LOGD << "END loadFeatureBackgroundEstimation";
      return;
    }

    if (filenames.getFullPathName("featureBackgroundEstimationComponentGroups_csv_i").size() &&
      !InputDataValidation::fileExists(filenames.getFullPathName("featureBackgroundEstimationComponentGroups_csv_i"))) {
      LOGE << "File not found: " << filenames.getFullPathName("featureBackgroundEstimationComponentGroups_csv_i");
      LOGD << "END loadFeatureBackgroundEstimation";
      return;
    }

    try {
      OpenMS::MRMFeatureQCFile featureQCFile;
      if (filenames.getFullPathName("featureBackgroundEstimationComponents_csv_i").size()) { // because we don't know if either of the two names is empty
        featureQCFile.load(filenames.getFullPathName("featureBackgroundEstimationComponents_csv_i"), sequenceSegmentHandler_IO.getFeatureBackgroundEstimations(), false);
      }
      if (filenames.getFullPathName("featureBackgroundEstimationComponentGroups_csv_i").size()) {
        featureQCFile.load(filenames.getFullPathName("featureBackgroundEstimationComponentGroups_csv_i"), sequenceSegmentHandler_IO.getFeatureBackgroundEstimations(), true);
      }
    }
    catch (const std::exception& e) {
      LOGE << e.what();
      sequenceSegmentHandler_IO.getFeatureBackgroundEstimations().component_qcs.clear();
      sequenceSegmentHandler_IO.getFeatureBackgroundEstimations().component_group_qcs.clear();
      sequenceSegmentHandler_IO.getFeatureBackgroundEstimations().component_group_pair_qcs.clear();
      LOGI << "feature filter clear";
    }

    LOGD << "END loadFeatureBackgroundEstimation";
  }

  ParameterSet StoreFeatureBackgroundEstimations::getParameterSchema() const
  {
    return ParameterSet();
  }

  void StoreFeatureBackgroundEstimations::getInputsOutputs(Filenames& filenames) const
  {
    filenames.addFileName("featureBackgroundEstimationComponents_csv_o", "_featureBackgroundEstimationComponents.csv", Filenames::FileScope::EFileScopeInjectionOutput);
    filenames.addFileName("featureBackgroundEstimationComponentGroups_csv_o", "_featureBackgroundEstimationComponentGroups.csv", Filenames::FileScope::EFileScopeInjectionOutput);
  };

  void StoreFeatureBackgroundEstimations::process(
    SequenceSegmentHandler& sequenceSegmentHandler_IO,
    const SequenceHandler& sequenceHandler_I,
    const ParameterSet& params_I,
    const Filenames& filenames_override
  ) const
  {
    LOGD << "START storeFeatureBackgroundEstimation";
    Filenames filenames = prepareFileNames(filenames_override);

    LOGI << "Storing: " << filenames.getFullPathName("featureBackgroundEstimationComponents_csv_o") << " and " <<
      filenames.getFullPathName("featureBackgroundEstimationComponentGroups_csv_o");

    if (filenames.getFullPathName("featureBackgroundEstimationComponents_csv_o").empty() &&
      filenames.getFullPathName("featureBackgroundEstimationComponentGroups_csv_o").empty()) {
      LOGE << "Filenames are both empty";
      LOGD << "END storeFeatureBackgroundEstimation";
      return;
    }

    try {
      OpenMS::MRMFeatureQCFile featureQCFile;
      if (filenames.getFullPathName("featureBackgroundEstimationComponents_csv_o").size()) { // because we don't know if either of the two names is empty
        featureQCFile.store(filenames.getFullPathName("featureBackgroundEstimationComponents_csv_o"), sequenceSegmentHandler_IO.getFeatureBackgroundEstimations(), false);
      }
      if (filenames.getFullPathName("featureBackgroundEstimationComponentGroups_csv_o").size()) {
        featureQCFile.store(filenames.getFullPathName("featureBackgroundEstimationComponentGroups_csv_o"), sequenceSegmentHandler_IO.getFeatureBackgroundEstimations(), true);
      }
    }
    catch (const std::exception& e) {
      LOGE << e.what();
      LOGI << "feature filter store exception";
    }

    LOGD << "END storeFeatureBackgroundEstimation";
  }
}
