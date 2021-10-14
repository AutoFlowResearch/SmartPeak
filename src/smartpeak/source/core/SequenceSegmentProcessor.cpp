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
#include <SmartPeak/core/FeatureFiltersUtils.h>
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

  void SequenceSegmentProcessor::processForAllSegments(
    std::vector<SmartPeak::SequenceSegmentHandler>& sequence_segment_handlers,
    SequenceSegmentObservable* sequence_segment_observable,
    Filenames& filenames)
  {
    for (SequenceSegmentHandler& sequence_segment_handler : sequence_segment_handlers) {
      sequence_segment_observable_ = sequence_segment_observable;
      process(sequence_segment_handler, SequenceHandler(), {}, filenames);
    }
  }

  std::string SequenceSegmentProcessor::constructFilename(const std::string& filename, bool static_filename) const
  {
    if (static_filename)
    {
      return "${MAIN_DIR}/" + filename;
    }
    else
    {
      return "${FEATURES_OUTPUT_PATH}/${OUTPUT_INJECTION_NAME}_" + filename;
    }
  }

  std::vector<std::string> CalculateCalibration::getRequirements() const
  {
    return { "sequence", "traML" };
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
    Filenames& filenames_I
  ) const
  {
    LOGD << "START optimizeCalibrationCurves";
    getFilenames(filenames_I);

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
      throw std::invalid_argument("standards_indices argument is empty.");
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

  std::vector<std::string> LoadStandardsConcentrations::getRequirements() const
  {
    return { "sequence", "traML" };
  }

  ParameterSet LoadStandardsConcentrations::getParameterSchema() const
  {
    return ParameterSet();
  }

  bool LoadStandardsConcentrations::onFilePicked(const std::filesystem::path& filename, ApplicationHandler* application_handler)
  {
    if (application_handler->sequenceHandler_.getSequence().size() == 0)
    {
      LOGE << "File cannot be loaded without first loading the sequence.";
      return false;
    }
    Filenames filenames;
    filenames.setFullPath("quantitationMethods", filename);
    processForAllSegments(application_handler->sequenceHandler_.getSequenceSegments(),
                          &(application_handler->sequenceHandler_),
                          filenames);
    return true;
  }

  void LoadStandardsConcentrations::getFilenames(Filenames& filenames) const
  {
    filenames.addFileName("standardsConcentrations", constructFilename("standardsConcentrations.csv", static_filenames_), "Standards Concentrations", true, true);
  };

  void LoadStandardsConcentrations::process(
    SequenceSegmentHandler& sequenceSegmentHandler_IO,
    const SequenceHandler& sequenceHandler_I,
    const ParameterSet& params_I,
    Filenames& filenames_I
  ) const
  {
    LOGD << "START loadStandardsConcentrations";
    getFilenames(filenames_I);

    if (!InputDataValidation::prepareToLoad(filenames_I, "standardsConcentrations"))
    {
      throw std::invalid_argument("Failed to load input file");
    }

    try
    {
      if (filenames_I.isEmbedded("standardsConcentrations"))
      {
        auto db_context = filenames_I.getSessionDB().beginRead(
          "standardsConcentrations",
          "sample_name",
          "component_name",
          "IS_component_name",
          "actual_concentration",
          "IS_actual_concentration",
          "concentration_units",
          "dilution_factor"
        );
        if (!db_context)
        {
          throw std::runtime_error("Failed to load from session database");
        }
        OpenMS::AbsoluteQuantitationStandards::runConcentration run_concentration;
        while (filenames_I.getSessionDB().read(
          *db_context,
          run_concentration.sample_name,
          run_concentration.component_name,
          run_concentration.IS_component_name,
          run_concentration.actual_concentration,
          run_concentration.IS_actual_concentration,
          run_concentration.concentration_units,
          run_concentration.dilution_factor
        ))
        {
          sequenceSegmentHandler_IO.getStandardsConcentrations().push_back(run_concentration);
        }
        filenames_I.getSessionDB().endRead(*db_context);
      }
      else
      {
        OpenMS::AbsoluteQuantitationStandardsFile AQSf;
        AQSf.load(filenames_I.getFullPath("standardsConcentrations").generic_string(), sequenceSegmentHandler_IO.getStandardsConcentrations());
      }
    }
    catch (const std::exception& e) {
      sequenceSegmentHandler_IO.getStandardsConcentrations().clear();
      LOGI << "Standards concentrations clear";
      throw e;
    }

    LOGD << "END loadStandardsConcentrations";
  }

  std::vector<std::string> StoreStandardsConcentrations::getRequirements() const
  {
    return { "sequence", "traML" };
  }

  ParameterSet StoreStandardsConcentrations::getParameterSchema() const
  {
    return ParameterSet();
  }

  bool StoreStandardsConcentrations::onFilePicked(const std::filesystem::path& filename, ApplicationHandler* application_handler)
  {
    if (application_handler->sequenceHandler_.getSequence().size() == 0)
    {
      LOGE << "File cannot be loaded without first loading the sequence.";
      return false;
    }
    Filenames filenames;
    filenames.setFullPath("quantitationMethods", filename);
    sequence_segment_observable_ = &application_handler->sequenceHandler_;
    process(application_handler->sequenceHandler_.getSequenceSegments()[0], SequenceHandler(), {}, filenames);
    return true;
  }

  void StoreStandardsConcentrations::getFilenames(Filenames& filenames) const
  {
    filenames.addFileName("standardsConcentrations", constructFilename("standardsConcentrations.csv", static_filenames_), "Standards Concentrations", true, true);
  };

  void StoreStandardsConcentrations::process(
    SequenceSegmentHandler& sequenceSegmentHandler_IO,
    const SequenceHandler& sequenceHandler_I,
    const ParameterSet& params_I,
    Filenames& filenames_I
  ) const
  {
    LOGD << "START StoreStandardsConcentrations";

    if (!InputDataValidation::prepareToStore(filenames_I, "standardsConcentrations"))
    {
      throw std::invalid_argument("Failed to store output file");
    }
    if (filenames_I.isEmbedded("standardsConcentrations"))
    {
      auto db_context = filenames_I.getSessionDB().beginWrite(
        "standardsConcentrations",
        "sample_name", "TEXT",
        "component_name", "TEXT",
        "IS_component_name", "TEXT",
        "actual_concentration", "REAL",
        "IS_actual_concentration", "REAL",
        "concentration_units", "TEXT",
        "dilution_factor", "REAL"
        );
      if (!db_context)
      {
        throw std::runtime_error("Failed to save in session database");
      }
      for (const auto& concentration : sequenceSegmentHandler_IO.getStandardsConcentrations())
      {
        filenames_I.getSessionDB().write(
          *db_context,
          concentration.sample_name,
          concentration.component_name,
          concentration.IS_component_name,
          concentration.actual_concentration,
          concentration.IS_actual_concentration,
          concentration.concentration_units,
          concentration.dilution_factor
          );
      }
      filenames_I.getSessionDB().endWrite(*db_context);
    }
    else
    {
      LOGE << "Not implemented";
    }
  }

  std::vector<std::string> LoadQuantitationMethods::getRequirements() const
  {
    return { "sequence", "traML" };
  }

  ParameterSet LoadQuantitationMethods::getParameterSchema() const
  {
    return ParameterSet();
  }

  void LoadQuantitationMethods::getFilenames(Filenames& filenames) const
  {
    filenames.addFileName("quantitationMethods", constructFilename("quantitationMethods.csv", static_filenames_), "Quantitation Methods");
  };

  bool LoadQuantitationMethods::onFilePicked(const std::filesystem::path& filename, ApplicationHandler* application_handler)
  {
    if (application_handler->sequenceHandler_.getSequence().size() == 0)
    {
      LOGE << "File cannot be loaded without first loading the sequence.";
      return false;
    }
    Filenames filenames;
    filenames.setFullPath("quantitationMethods", filename);
    processForAllSegments(application_handler->sequenceHandler_.getSequenceSegments(),
      &(application_handler->sequenceHandler_),
      filenames);
    return true;
  }

  void LoadQuantitationMethods::process(
    SequenceSegmentHandler& sequenceSegmentHandler_IO,
    const SequenceHandler& sequenceHandler_I,
    const ParameterSet& params_I,
    Filenames& filenames_I
  ) const
  {
    LOGD << "START loadQuantitationMethods";
    getFilenames(filenames_I);

    if (!InputDataValidation::prepareToLoad(filenames_I, "quantitationMethods"))
    {
      throw std::invalid_argument("Failed to load input file");
    }

    try {
      OpenMS::AbsoluteQuantitationMethodFile AQMf;
      AQMf.load(filenames_I.getFullPath("quantitationMethods").generic_string(), sequenceSegmentHandler_IO.getQuantitationMethods());
      if (sequence_segment_observable_) sequence_segment_observable_->notifyQuantitationMethodsUpdated();
    }
    catch (const std::exception& e) {
      LOGE << e.what();
      sequenceSegmentHandler_IO.getQuantitationMethods().clear();
      LOGI << "quantitation methods clear";
    }
    LOGD << "END loadQuantitationMethods";
  }

  std::vector<std::string> StoreQuantitationMethods::getRequirements() const
  {
    return { "sequence", "traML" };
  }

  ParameterSet StoreQuantitationMethods::getParameterSchema() const
  {
    return ParameterSet();
  }

  void StoreQuantitationMethods::getFilenames(Filenames& filenames) const
  {
    filenames.addFileName("quantitationMethods", constructFilename("quantitationMethods.csv", static_filenames_), "Quantitation Methods");
  };

  void StoreQuantitationMethods::process(
    SequenceSegmentHandler& sequenceSegmentHandler_IO,
    const SequenceHandler& sequenceHandler_I,
    const ParameterSet& params_I,
    Filenames& filenames_I
  ) const
  {
    LOGD << "START storeQuantitationMethods";
    getFilenames(filenames_I);

    if (!InputDataValidation::prepareToStore(filenames_I, "quantitationMethods"))
    {
      throw std::invalid_argument("Failed to store output file");
    }

    try {
      OpenMS::AbsoluteQuantitationMethodFile aqmf;
      aqmf.store(
        filenames_I.getFullPath("quantitationMethods").generic_string(),
        sequenceSegmentHandler_IO.getQuantitationMethods()
      );
    }
    catch (const std::exception& e) {
      LOGE << e.what();
    }

    LOGD << "END storeQuantitationMethods";
  }

  std::vector<std::string> LoadFeatureFilters::getRequirements() const
  {
    return { "sequence", "traML" };
  }

  ParameterSet LoadFeatureFilters::getParameterSchema() const
  {
    return ParameterSet();
  }

  bool LoadFeatureFilters::onFilePicked(const std::filesystem::path& filename, ApplicationHandler* application_handler)
  {
    Filenames filenames;
    if (!FeatureFiltersUtils::onFilePicked(
      filename,
      application_handler,
      filenames,
      "featureFilterComponents",
      "featureFilterComponentGroups",
      feature_filter_mode_))
    {
      return false;
    }
    processForAllSegments(application_handler->sequenceHandler_.getSequenceSegments(),
      &(application_handler->sequenceHandler_),
      filenames);
    return true;
  }

  void LoadFeatureFilters::getFilenames(Filenames& filenames) const
  {
    if (feature_filter_mode_ & FeatureFiltersUtilsMode::EFeatureFiltersModeGroup)
    {
      filenames.addFileName("featureFilterComponentGroups", constructFilename("featureFilterComponentGroups.csv", static_filenames_), "Components Group Filters", true, true);
    }
    else if (feature_filter_mode_ & FeatureFiltersUtilsMode::EFeatureFiltersModeComponent)
    {
      filenames.addFileName("featureFilterComponents", constructFilename("featureFilterComponents.csv", static_filenames_), "Components Filters", true, true);
    }
  };

  void LoadFeatureFilters::process(
    SequenceSegmentHandler& sequenceSegmentHandler_IO,
    const SequenceHandler& sequenceHandler_I,
    const ParameterSet& params_I,
    Filenames& filenames_I
  ) const
  {
    LOGD << "START loadFeatureFilter";
    getFilenames(filenames_I);
    FeatureFiltersUtils::loadFeatureFilters(
      "featureFilterComponents",
      "featureFilterComponentGroups",
      filenames_I,
      sequenceSegmentHandler_IO.getFeatureFilter(),
      [&]() { if (sequence_segment_observable_) sequence_segment_observable_->notifyFeatureFiltersComponentsUpdated(); },
      [&]() { if (sequence_segment_observable_) sequence_segment_observable_->notifyFeatureFiltersComponentGroupsUpdated(); },
      feature_filter_mode_
    );
    LOGD << "END loadFeatureFilter";
  }

  std::vector<std::string> LoadFeatureQCs::getRequirements() const
  {
    return { "sequence", "traML" };
  }

  ParameterSet LoadFeatureQCs::getParameterSchema() const
  {
    return ParameterSet();
  }

  bool LoadFeatureQCs::onFilePicked(const std::filesystem::path& filename, ApplicationHandler* application_handler)
  {
    Filenames filenames;
    if (!FeatureFiltersUtils::onFilePicked(
      filename,
      application_handler,
      filenames,
      "featureQCComponents",
      "featureQCComponentGroups",
      feature_filter_mode_))
    {
      return false;
    }
    processForAllSegments(application_handler->sequenceHandler_.getSequenceSegments(),
      &(application_handler->sequenceHandler_),
      filenames);
    return true;
  }

  void LoadFeatureQCs::getFilenames(Filenames& filenames) const
  {
    if (feature_filter_mode_ & FeatureFiltersUtilsMode::EFeatureFiltersModeGroup)
    {
      filenames.addFileName("featureQCComponentGroups", constructFilename("featureQCComponentGroups.csv", static_filenames_), "Components Group QCs", true, true);
    }
    else if (feature_filter_mode_ & FeatureFiltersUtilsMode::EFeatureFiltersModeComponent)
    {
      filenames.addFileName("featureQCComponents", constructFilename("featureQCComponents.csv", static_filenames_), "Components QCs", true, true);
    }
  };

  void LoadFeatureQCs::process(
    SequenceSegmentHandler& sequenceSegmentHandler_IO,
    const SequenceHandler& sequenceHandler_I,
    const ParameterSet& params_I,
    Filenames& filenames_I
  ) const
  {
    LOGD << "START loadFeatureQC";
    getFilenames(filenames_I);
    FeatureFiltersUtils::loadFeatureFilters(
      "featureQCComponents",
      "featureQCComponentGroups",
      filenames_I,
      sequenceSegmentHandler_IO.getFeatureQC(),
      [&]() { if (sequence_segment_observable_) sequence_segment_observable_->notifyFeatureQCComponentsUpdated(); },
      [&]() { if (sequence_segment_observable_) sequence_segment_observable_->notifyFeatureQCComponentGroupsUpdated(); },
      feature_filter_mode_
    );
    LOGD << "END loadFeatureQC";
  }

  std::vector<std::string> StoreFeatureFilters::getRequirements() const
  {
    return { "sequence", "traML" };
  }

  ParameterSet StoreFeatureFilters::getParameterSchema() const
  {
    return ParameterSet();
  }

  bool StoreFeatureFilters::onFilePicked(const std::filesystem::path& filename, ApplicationHandler* application_handler)
  {
    Filenames filenames;
    if (!FeatureFiltersUtils::onFilePicked(
      filename,
      application_handler,
      filenames,
      "featureFilterComponents",
      "featureFilterComponentGroups",
      feature_filter_mode_))
    {
      return false;
    }
    sequence_segment_observable_ = &application_handler->sequenceHandler_;
    process(application_handler->sequenceHandler_.getSequenceSegments()[0], SequenceHandler(), {}, filenames);
    return true;
  }

  void StoreFeatureFilters::getFilenames(Filenames& filenames) const
  {
    if (feature_filter_mode_ & FeatureFiltersUtilsMode::EFeatureFiltersModeGroup)
    {
      filenames.addFileName("featureFilterComponentGroups", constructFilename("featureFilterComponentGroups.csv", static_filenames_), "Components Group Filters", true, true);
    }
    else if (feature_filter_mode_ & FeatureFiltersUtilsMode::EFeatureFiltersModeComponent)
    {
      filenames.addFileName("featureFilterComponents", constructFilename("featureFilterComponents.csv", static_filenames_), "Components Filters", true, true);
    }
  };

  void StoreFeatureFilters::process(
    SequenceSegmentHandler& sequenceSegmentHandler_IO,
    const SequenceHandler& sequenceHandler_I,
    const ParameterSet& params_I,
    Filenames& filenames_I
  ) const
  {
    LOGD << "START storeFeatureFilter";
    getFilenames(filenames_I);
    FeatureFiltersUtils::storeFeatureFilters(
      "featureFilterComponents",
      "featureFilterComponentGroups",
      filenames_I,
      sequenceSegmentHandler_IO.getFeatureFilter(),
      feature_filter_mode_);
    LOGD << "END storeFeatureFilter";
  }

  std::vector<std::string> StoreFeatureQCs::getRequirements() const
  {
    return { "sequence", "traML" };
  }

  ParameterSet StoreFeatureQCs::getParameterSchema() const
  {
    return ParameterSet();
  }

  bool StoreFeatureQCs::onFilePicked(const std::filesystem::path& filename, ApplicationHandler* application_handler)
  {
    Filenames filenames;
    if (!FeatureFiltersUtils::onFilePicked(
      filename,
      application_handler,
      filenames,
      "featureQCComponents",
      "featureQCComponentGroups",
      feature_filter_mode_))
    {
      return false;
    }
    sequence_segment_observable_ = &application_handler->sequenceHandler_;
    process(application_handler->sequenceHandler_.getSequenceSegments()[0], SequenceHandler(), {}, filenames);
    return true;
  }

  void StoreFeatureQCs::getFilenames(Filenames& filenames) const
  {
    if (feature_filter_mode_ & FeatureFiltersUtilsMode::EFeatureFiltersModeGroup)
    {
      filenames.addFileName("featureQCComponentGroups", constructFilename("featureQCComponentGroups.csv", static_filenames_), "Components Group QC", true, true);
    }
    else if (feature_filter_mode_ & FeatureFiltersUtilsMode::EFeatureFiltersModeComponent)
    {
      filenames.addFileName("featureQCComponents", constructFilename("featureQCComponents.csv", static_filenames_), "Components QC", true, true);
    }
  };

  void StoreFeatureQCs::process(
    SequenceSegmentHandler& sequenceSegmentHandler_IO,
    const SequenceHandler& sequenceHandler_I,
    const ParameterSet& params_I,
    Filenames& filenames_I
  ) const
  {
    LOGD << "START storeFeatureQC";
    getFilenames(filenames_I);
    FeatureFiltersUtils::storeFeatureFilters(
      "featureQCComponents",
      "featureQCComponentGroups",
      filenames_I,
      sequenceSegmentHandler_IO.getFeatureQC(),
      feature_filter_mode_);
    LOGD << "END storeFeatureQC";
  }

  std::vector<std::string> LoadFeatureRSDFilters::getRequirements() const
  {
    return { "sequence", "traML" };
  }

  ParameterSet LoadFeatureRSDFilters::getParameterSchema() const
  {
    return ParameterSet();
  }

  void LoadFeatureRSDFilters::getFilenames(Filenames& filenames) const
  {
    if (feature_filter_mode_ & FeatureFiltersUtilsMode::EFeatureFiltersModeGroup)
    {
      filenames.addFileName("featureRSDFilterComponentGroups", 
                            constructFilename("featureRSDFilterComponentGroups.csv", static_filenames_),
                            "Components group %RSD Filters",
                            true,
                            true);
    }
    else if (feature_filter_mode_ & FeatureFiltersUtilsMode::EFeatureFiltersModeComponent)
    {
      filenames.addFileName("featureRSDFilterComponents",
                            constructFilename("featureRSDFilterComponents.csv", static_filenames_),
                            "Components %RSD Filters",
                            true,
                            true);
    }
  };

  bool LoadFeatureRSDFilters::onFilePicked(const std::filesystem::path& filename, ApplicationHandler* application_handler)
  {
    Filenames filenames;
    if (!FeatureFiltersUtils::onFilePicked(
      filename,
      application_handler,
      filenames,
      "featureRSDFilterComponents",
      "featureRSDFilterComponentGroups",
      feature_filter_mode_))
    {
      return false;
    }
    processForAllSegments(application_handler->sequenceHandler_.getSequenceSegments(),
      &(application_handler->sequenceHandler_),
      filenames);
    return true;
  }

  void LoadFeatureRSDFilters::process(
    SequenceSegmentHandler& sequenceSegmentHandler_IO,
    const SequenceHandler& sequenceHandler_I,
    const ParameterSet& params_I,
    Filenames& filenames_I
  ) const
  {
    LOGD << "START loadFeatureRSDFilter";
    getFilenames(filenames_I);
    FeatureFiltersUtils::loadFeatureFilters(
      "featureRSDFilterComponents",
      "featureRSDFilterComponentGroups",
      filenames_I,
      sequenceSegmentHandler_IO.getFeatureRSDFilter(),
      [&]() { if (sequence_segment_observable_) sequence_segment_observable_->notifyFeatureRSDFilterComponentsUpdated(); },
      [&]() { if (sequence_segment_observable_) sequence_segment_observable_->notifyFeatureRSDFilterComponentGroupsUpdated(); },
      feature_filter_mode_
    );
    LOGD << "END loadFeatureRSDFilter";
  }

  std::vector<std::string> LoadFeatureRSDQCs::getRequirements() const
  {
    return { "sequence", "traML" };
  }

  ParameterSet LoadFeatureRSDQCs::getParameterSchema() const
  {
    return ParameterSet();
  }

  void LoadFeatureRSDQCs::getFilenames(Filenames& filenames) const
  {
    if (feature_filter_mode_ & FeatureFiltersUtilsMode::EFeatureFiltersModeGroup)
    {
      filenames.addFileName("featureRSDQCComponentGroups",
                            constructFilename("featureRSDQCComponentGroups.csv", static_filenames_),
                            "Components Group %RSD QCs",
                            true,
                            true);
    }
    else if (feature_filter_mode_ & FeatureFiltersUtilsMode::EFeatureFiltersModeComponent)
    {
      filenames.addFileName("featureRSDQCComponents",
                            constructFilename("featureRSDQCComponents.csv", static_filenames_),
                            "Components %RSD QCs",
                            true,
                            true);
    }
  };

  bool LoadFeatureRSDQCs::onFilePicked(const std::filesystem::path& filename, ApplicationHandler* application_handler)
  {
    Filenames filenames;
    if (!FeatureFiltersUtils::onFilePicked(
      filename,
      application_handler,
      filenames,
      "featureRSDQCComponents",
      "featureRSDQCComponentGroups",
      feature_filter_mode_))
    {
      return false;
    }
    processForAllSegments(application_handler->sequenceHandler_.getSequenceSegments(),
      &(application_handler->sequenceHandler_),
      filenames);
    return true;
  }

  void LoadFeatureRSDQCs::process(
    SequenceSegmentHandler& sequenceSegmentHandler_IO,
    const SequenceHandler& sequenceHandler_I,
    const ParameterSet& params_I,
    Filenames& filenames_I
  ) const
  {
    LOGD << "START loadFeatureRSDQC";
    getFilenames(filenames_I);
    FeatureFiltersUtils::loadFeatureFilters(
      "featureRSDQCComponents",
      "featureRSDQCComponentGroups",
      filenames_I,
      sequenceSegmentHandler_IO.getFeatureRSDQC(),
      [&]() { if (sequence_segment_observable_) sequence_segment_observable_->notifyFeatureRSDQCComponentsUpdated(); },
      [&]() { if (sequence_segment_observable_) sequence_segment_observable_->notifyFeatureRSDQCComponentGroupsUpdated(); },
      feature_filter_mode_
    );
    LOGD << "END loadFeatureRSDQC";
  }

  std::vector<std::string> StoreFeatureRSDFilters::getRequirements() const
  {
    return { "sequence", "traML" };
  }

  ParameterSet StoreFeatureRSDFilters::getParameterSchema() const
  {
    return ParameterSet();
  }

  bool StoreFeatureRSDFilters::onFilePicked(const std::filesystem::path& filename, ApplicationHandler* application_handler)
  {
    Filenames filenames;
    if (!FeatureFiltersUtils::onFilePicked(
      filename,
      application_handler,
      filenames,
      "featureRSDFilterComponents",
      "featureRSDFilterComponentGroups",
      feature_filter_mode_))
    {
      return false;
    }
    sequence_segment_observable_ = &application_handler->sequenceHandler_;
    process(application_handler->sequenceHandler_.getSequenceSegments()[0], SequenceHandler(), {}, filenames);
    return true;
  }

  void StoreFeatureRSDFilters::getFilenames(Filenames& filenames) const
  {
    if (feature_filter_mode_ & FeatureFiltersUtilsMode::EFeatureFiltersModeGroup)
    {
      filenames.addFileName("featureRSDFilterComponentGroups",
                            constructFilename("featureRSDFilterComponentGroups.csv", static_filenames_),
                            "Components group %RSD Filters");
    }
    else if (feature_filter_mode_ & FeatureFiltersUtilsMode::EFeatureFiltersModeComponent)
    {
      filenames.addFileName("featureRSDFilterComponents",
                            constructFilename("featureRSDFilterComponents.csv", static_filenames_),
                            "Components %RSD Filters");
    }
  };

  void StoreFeatureRSDFilters::process(
    SequenceSegmentHandler& sequenceSegmentHandler_IO,
    const SequenceHandler& sequenceHandler_I,
    const ParameterSet& params_I,
    Filenames& filenames_I
  ) const
  {
    LOGD << "START storeFeatureRSDFilter";
    getFilenames(filenames_I);
    FeatureFiltersUtils::storeFeatureFilters(
      "featureRSDFilterComponents",
      "featureRSDFilterComponentGroups",
      filenames_I,
      sequenceSegmentHandler_IO.getFeatureRSDFilter(),
      feature_filter_mode_);
    LOGD << "END storeFeatureRSDFilter";
  }

  std::vector<std::string> StoreFeatureRSDQCs::getRequirements() const
  {
    return { "sequence", "traML" };
  }

  ParameterSet StoreFeatureRSDQCs::getParameterSchema() const
  {
    return ParameterSet();
  }

  bool StoreFeatureRSDQCs::onFilePicked(const std::filesystem::path& filename, ApplicationHandler* application_handler)
  {
    Filenames filenames;
    if (!FeatureFiltersUtils::onFilePicked(
      filename,
      application_handler,
      filenames,
      "featureRSDQCComponents",
      "featureRSDQCComponentGroups",
      feature_filter_mode_))
    {
      return false;
    }
    sequence_segment_observable_ = &application_handler->sequenceHandler_;
    process(application_handler->sequenceHandler_.getSequenceSegments()[0], SequenceHandler(), {}, filenames);
    return true;
  }

  void StoreFeatureRSDQCs::getFilenames(Filenames& filenames) const
  {
    if (feature_filter_mode_ & FeatureFiltersUtilsMode::EFeatureFiltersModeGroup)
    {
      filenames.addFileName("featureRSDQCComponentGroups",
                            constructFilename("featureRSDQCComponentGroups.csv", static_filenames_),
                            "Components Group %RSD QCs");
    }
    else if (feature_filter_mode_ & FeatureFiltersUtilsMode::EFeatureFiltersModeComponent)
    {
      filenames.addFileName("featureRSDQCComponents",
                            constructFilename("featureRSDQCComponents.csv", static_filenames_),
                            "Components %RSD QCs");
    }
  };

  void StoreFeatureRSDQCs::process(
    SequenceSegmentHandler& sequenceSegmentHandler_IO,
    const SequenceHandler& sequenceHandler_I,
    const ParameterSet& params_I,
    Filenames& filenames_I
  ) const
  {
    LOGD << "START storeFeatureRSDQC";
    getFilenames(filenames_I);
    FeatureFiltersUtils::storeFeatureFilters(
      "featureRSDQCComponents",
      "featureRSDQCComponentGroups",
      filenames_I,
      sequenceSegmentHandler_IO.getFeatureRSDQC(),
      feature_filter_mode_);
    LOGD << "END storeFeatureRSDQC";
  }

  std::vector<std::string> LoadFeatureBackgroundFilters::getRequirements() const
  {
    return { "sequence", "traML" };
  }

  ParameterSet LoadFeatureBackgroundFilters::getParameterSchema() const
  {
    return ParameterSet();
  }

  void LoadFeatureBackgroundFilters::getFilenames(Filenames& filenames) const
  {
    if (feature_filter_mode_ & FeatureFiltersUtilsMode::EFeatureFiltersModeGroup)
    {
      filenames.addFileName("featureBackgroundFilterComponentGroups", 
        constructFilename("featureBackgroundFilterComponentGroups.csv", static_filenames_),
        "Components Group % Background Filters",
        true,
        true);
    }
    else if (feature_filter_mode_ & FeatureFiltersUtilsMode::EFeatureFiltersModeComponent)
    {
      filenames.addFileName("featureBackgroundFilterComponents", 
        constructFilename("featureBackgroundFilterComponents.csv", static_filenames_),
        "Components % Background Filters",
        true,
        true);
    }
  };

  bool LoadFeatureBackgroundFilters::onFilePicked(const std::filesystem::path& filename, ApplicationHandler* application_handler)
  {
    Filenames filenames;
    if (!FeatureFiltersUtils::onFilePicked(
      filename,
      application_handler,
      filenames,
      "featureBackgroundFilterComponents",
      "featureBackgroundFilterComponentGroups",
      feature_filter_mode_))
    {
      return false;
    }
    processForAllSegments(application_handler->sequenceHandler_.getSequenceSegments(),
      &(application_handler->sequenceHandler_),
      filenames);
    return true;
  }

  void LoadFeatureBackgroundFilters::process(
    SequenceSegmentHandler& sequenceSegmentHandler_IO,
    const SequenceHandler& sequenceHandler_I,
    const ParameterSet& params_I,
    Filenames& filenames_I
  ) const
  {
    LOGD << "START loadFeatureBackgroundFilter";
    getFilenames(filenames_I);
    FeatureFiltersUtils::loadFeatureFilters(
      "featureBackgroundFilterComponents",
      "featureBackgroundFilterComponentGroups",
      filenames_I,
      sequenceSegmentHandler_IO.getFeatureBackgroundFilter(),
      [&]() { if (sequence_segment_observable_) sequence_segment_observable_->notifyFeatureBackgroundFilterComponentsUpdated(); },
      [&]() { if (sequence_segment_observable_) sequence_segment_observable_->notifyFeatureBackgroundFilterComponentGroupsUpdated(); },
      feature_filter_mode_
    );
    LOGD << "END loadFeatureBackgroundFilter";
  }

  std::vector<std::string> LoadFeatureBackgroundQCs::getRequirements() const
  {
    return { "sequence", "traML" };
  }

  ParameterSet LoadFeatureBackgroundQCs::getParameterSchema() const
  {
    return ParameterSet();
  }

  void LoadFeatureBackgroundQCs::getFilenames(Filenames& filenames) const
  {
    if (feature_filter_mode_ & FeatureFiltersUtilsMode::EFeatureFiltersModeGroup)
    {
      filenames.addFileName("featureBackgroundQCComponentGroups",
                            constructFilename("featureBackgroundQCComponentGroups.csv", static_filenames_),
                            "Components Group %Background QCs",
                            true,
                            true);
    }
    else if (feature_filter_mode_ & FeatureFiltersUtilsMode::EFeatureFiltersModeComponent)
    {
      filenames.addFileName("featureBackgroundQCComponents",
                            constructFilename("featureBackgroundQCComponents.csv", static_filenames_),
                            "Components % Background QCs",
                            true,
                            true);
    }
  };

  bool LoadFeatureBackgroundQCs::onFilePicked(const std::filesystem::path& filename, ApplicationHandler* application_handler)
  {
    Filenames filenames;
    if (!FeatureFiltersUtils::onFilePicked(
      filename,
      application_handler,
      filenames,
      "featureBackgroundQCComponents",
      "featureBackgroundQCComponentGroups",
      feature_filter_mode_))
    {
      return false;
    }
    processForAllSegments(application_handler->sequenceHandler_.getSequenceSegments(),
      &(application_handler->sequenceHandler_),
      filenames);
    return true;
  }

  void LoadFeatureBackgroundQCs::process(
    SequenceSegmentHandler& sequenceSegmentHandler_IO,
    const SequenceHandler& sequenceHandler_I,
    const ParameterSet& params_I,
    Filenames& filenames_I
  ) const
  {
    LOGD << "START loadFeatureBackgroundQC";
    getFilenames(filenames_I);
    FeatureFiltersUtils::loadFeatureFilters(
      "featureBackgroundQCComponents",
      "featureBackgroundQCComponentGroups",
      filenames_I,
      sequenceSegmentHandler_IO.getFeatureBackgroundQC(),
      [&]() { if (sequence_segment_observable_) sequence_segment_observable_->notifyFeatureBackgroundQCComponentsUpdated(); },
      [&]() { if (sequence_segment_observable_) sequence_segment_observable_->notifyFeatureBackgroundQCComponentGroupsUpdated(); },
      feature_filter_mode_
    );
    LOGD << "END loadFeatureBackgroundQC";
  }

  std::vector<std::string> StoreFeatureBackgroundFilters::getRequirements() const
  {
    return { "sequence", "traML" };
  }

  ParameterSet StoreFeatureBackgroundFilters::getParameterSchema() const
  {
    return ParameterSet();
  }

  bool StoreFeatureBackgroundFilters::onFilePicked(const std::filesystem::path& filename, ApplicationHandler* application_handler)
  {
    Filenames filenames;
    if (!FeatureFiltersUtils::onFilePicked(
      filename,
      application_handler,
      filenames,
      "featureBackgroundFilterComponents",
      "featureBackgroundFilterComponentGroups",
      feature_filter_mode_))
    {
      return false;
    }
    sequence_segment_observable_ = &application_handler->sequenceHandler_;
    process(application_handler->sequenceHandler_.getSequenceSegments()[0], SequenceHandler(), {}, filenames);
    return true;
  }

  void StoreFeatureBackgroundFilters::getFilenames(Filenames& filenames) const
  {
    if (feature_filter_mode_ & FeatureFiltersUtilsMode::EFeatureFiltersModeGroup)
    {
      filenames.addFileName("featureBackgroundFilterComponentGroups",
                            constructFilename("featureBackgroundFilterComponentGroups.csv", static_filenames_),
                            "Components Group % Background Filters");
    }
    else if (feature_filter_mode_ & FeatureFiltersUtilsMode::EFeatureFiltersModeComponent)
    {
      filenames.addFileName("featureBackgroundFilterComponents",
                            constructFilename("featureBackgroundFilterComponents.csv", static_filenames_),
                            "Components % Background Filters");
    }
  };

  void StoreFeatureBackgroundFilters::process(
    SequenceSegmentHandler& sequenceSegmentHandler_IO,
    const SequenceHandler& sequenceHandler_I,
    const ParameterSet& params_I,
    Filenames& filenames_I
  ) const
  {
    LOGD << "START storeFeatureBackgroundFilter";
    getFilenames(filenames_I);
    FeatureFiltersUtils::storeFeatureFilters(
      "featureBackgroundFilterComponents",
      "featureBackgroundFilterComponentGroups",
      filenames_I,
      sequenceSegmentHandler_IO.getFeatureBackgroundFilter(),
      feature_filter_mode_);
    LOGD << "END storeFeatureBackgroundFilter";
  }

  std::vector<std::string> StoreFeatureBackgroundQCs::getRequirements() const
  {
    return { "sequence", "traML" };
  }

  ParameterSet StoreFeatureBackgroundQCs::getParameterSchema() const
  {
    return ParameterSet();
  }

  bool StoreFeatureBackgroundQCs::onFilePicked(const std::filesystem::path& filename, ApplicationHandler* application_handler)
  {
    Filenames filenames;
    if (!FeatureFiltersUtils::onFilePicked(
      filename,
      application_handler,
      filenames,
      "featureBackgroundQCComponents",
      "featureBackgroundQCComponentGroups",
      feature_filter_mode_))
    {
      return false;
    }
    sequence_segment_observable_ = &application_handler->sequenceHandler_;
    process(application_handler->sequenceHandler_.getSequenceSegments()[0], SequenceHandler(), {}, filenames);
    return true;
  }

  void StoreFeatureBackgroundQCs::getFilenames(Filenames& filenames) const
  {
    if (feature_filter_mode_ & FeatureFiltersUtilsMode::EFeatureFiltersModeGroup)
    {
      filenames.addFileName("featureBackgroundQCComponentGroups",
                            constructFilename("featureBackgroundQCComponentGroups.csv", static_filenames_),
                            "Components Group %Background QCs");
    }
    else if (feature_filter_mode_ & FeatureFiltersUtilsMode::EFeatureFiltersModeComponent)
    {
      filenames.addFileName("featureBackgroundQCComponents",
                            constructFilename("featureBackgroundQCComponents.csv", static_filenames_),
                            "Components %Background QCs");
    }
  };

  void StoreFeatureBackgroundQCs::process(
    SequenceSegmentHandler& sequenceSegmentHandler_IO,
    const SequenceHandler& sequenceHandler_I,
    const ParameterSet& params_I,
    Filenames& filenames_I
  ) const
  {
    LOGD << "START storeFeatureBackgroundQC";
    getFilenames(filenames_I);
    FeatureFiltersUtils::storeFeatureFilters(
      "featureBackgroundQCComponents",
      "featureBackgroundQCComponentGroups",
      filenames_I,
      sequenceSegmentHandler_IO.getFeatureBackgroundQC(),
      feature_filter_mode_);
    LOGD << "END storeFeatureBackgroundQC";
  }

  std::vector<std::string> EstimateFeatureFilterValues::getRequirements() const
  {
    return { "sequence", "traML" };
  }

  ParameterSet EstimateFeatureFilterValues::getParameterSchema() const
  {
    return ParameterSet();
  }

  void EstimateFeatureFilterValues::process(
    SequenceSegmentHandler& sequenceSegmentHandler_IO,
    const SequenceHandler& sequenceHandler_I,
    const ParameterSet& params_I,
    Filenames& filenames_I
  ) const
  {
    LOGD << "START estimateFeatureFilterValues";
    getFilenames(filenames_I);

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
      throw std::invalid_argument("standards_indices and/or qcs_indices argument is empty. Returning");
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

  std::vector<std::string> EstimateFeatureQCValues::getRequirements() const
  {
    return { "sequence", "traML" };
  }

  ParameterSet EstimateFeatureQCValues::getParameterSchema() const
  {
    return ParameterSet();
  }

  void EstimateFeatureQCValues::process(
    SequenceSegmentHandler& sequenceSegmentHandler_IO,
    const SequenceHandler& sequenceHandler_I,
    const ParameterSet& params_I,
    Filenames& filenames_I
  ) const
  {
    LOGD << "START estimateFeatureQCValues";
    getFilenames(filenames_I);

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
      throw std::invalid_argument("standards_indices and/or qcs_indices argument is empty.");
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

  std::vector<std::string> TransferLOQToFeatureFilters::getRequirements() const
  {
    return { "sequence", "traML" };
  }

  ParameterSet TransferLOQToFeatureFilters::getParameterSchema() const
  {
    return ParameterSet();
  }

  void TransferLOQToFeatureFilters::process(
    SequenceSegmentHandler& sequenceSegmentHandler_IO,
    const SequenceHandler& sequenceHandler_I,
    const ParameterSet& params_I,
    Filenames& filenames_I
  ) const
  {
    LOGD << "START TransferLOQToFeatureFilters";
    getFilenames(filenames_I);

    // check if there are any quantitation methods
    if (sequenceSegmentHandler_IO.getQuantitationMethods().empty()) {
      throw std::invalid_argument("quantitation methods is empty.");
    }

    OpenMS::MRMFeatureFilter featureFilter;
    featureFilter.TransferLLOQAndULOQToCalculatedConcentrationBounds(
      sequenceSegmentHandler_IO.getQuantitationMethods(),
      sequenceSegmentHandler_IO.getFeatureFilter()
    );

    LOGD << "END TransferLOQToFeatureFilters";
  }

  std::vector<std::string> TransferLOQToFeatureQCs::getRequirements() const
  {
    return { "sequence", "traML" };
  }

  ParameterSet TransferLOQToFeatureQCs::getParameterSchema() const
  {
    return ParameterSet();
  }

  void TransferLOQToFeatureQCs::process(
    SequenceSegmentHandler& sequenceSegmentHandler_IO,
    const SequenceHandler& sequenceHandler_I,
    const ParameterSet& params_I,
    Filenames& filenames_I
  ) const
  {
    LOGD << "START TransferLOQToFeatureQCs";
    getFilenames(filenames_I);

    // check if there are any quantitation methods
    if (sequenceSegmentHandler_IO.getQuantitationMethods().empty()) {
      throw std::invalid_argument("quantitation methods is empty.");
    }

    OpenMS::MRMFeatureFilter featureFilter;
    featureFilter.TransferLLOQAndULOQToCalculatedConcentrationBounds(
      sequenceSegmentHandler_IO.getQuantitationMethods(),
      sequenceSegmentHandler_IO.getFeatureQC()
    );

    LOGD << "END TransferLOQToFeatureQCs";
  }

  std::vector<std::string> EstimateFeatureRSDs::getRequirements() const
  {
    return { "sequence", "traML" };
  }

  ParameterSet EstimateFeatureRSDs::getParameterSchema() const
  {
    return ParameterSet();
  }

  void EstimateFeatureRSDs::process(
    SequenceSegmentHandler& sequenceSegmentHandler_IO,
    const SequenceHandler& sequenceHandler_I,
    const ParameterSet& params_I,
    Filenames& filenames_I
  ) const
  {
    LOGD << "START EstimateFeatureRSDs";
    getFilenames(filenames_I);

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
      throw std::invalid_argument("qcs_indices argument is empty.");
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

  std::vector<std::string> EstimateFeatureBackgroundInterferences::getRequirements() const
  {
    return { "sequence", "traML" };
  }

  ParameterSet EstimateFeatureBackgroundInterferences::getParameterSchema() const
  {
    return ParameterSet();
  }

  void EstimateFeatureBackgroundInterferences::process(
    SequenceSegmentHandler& sequenceSegmentHandler_IO,
    const SequenceHandler& sequenceHandler_I,
    const ParameterSet& params_I,
    Filenames& filenames_I
  ) const
  {
    LOGD << "START EstimateFeatureBackgroundInterferences";
    getFilenames(filenames_I);

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
      throw std::invalid_argument("blanks_indices argument is empty.");
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

  std::vector<std::string> LoadFeatureRSDEstimations::getRequirements() const
  {
    return { "sequence", "traML" };
  }

  ParameterSet LoadFeatureRSDEstimations::getParameterSchema() const
  {
    return ParameterSet();
  }

  void LoadFeatureRSDEstimations::getFilenames(Filenames& filenames) const
  {
    if (feature_filter_mode_ & FeatureFiltersUtilsMode::EFeatureFiltersModeGroup)
    {
      filenames.addFileName("featureRSDEstimationComponentGroups",
        constructFilename("featureRSDEstimationComponentGroups.csv", static_filenames_),
        "Feature RSD Estimation Component Groups",
        true,
        true);
    }
    else if (feature_filter_mode_ & FeatureFiltersUtilsMode::EFeatureFiltersModeComponent)
    {
      filenames.addFileName("featureRSDEstimationComponents",
        constructFilename("featureRSDEstimationComponents.csv", static_filenames_),
        "Feature RSD Estimation Component",
        true,
        true);
    }
  };

  void LoadFeatureRSDEstimations::process(
    SequenceSegmentHandler& sequenceSegmentHandler_IO,
    const SequenceHandler& sequenceHandler_I,
    const ParameterSet& params_I,
    Filenames& filenames_I
  ) const
  {
    LOGD << "START loadFeatureRSDEstimation";
    getFilenames(filenames_I);
    FeatureFiltersUtils::loadFeatureFilters(
      "featureRSDEstimationComponents",
      "featureRSDEstimationComponentGroups",
      filenames_I,
      sequenceSegmentHandler_IO.getFeatureRSDEstimations(),
      nullptr,
      nullptr,
      feature_filter_mode_
    );
    LOGD << "END loadFeatureRSDEstimation";
  }

  bool StoreFeatureRSDEstimations::onFilePicked(const std::filesystem::path& filename, ApplicationHandler* application_handler)
  {
    Filenames filenames;
    if (!FeatureFiltersUtils::onFilePicked(
      filename,
      application_handler,
      filenames,
      "featureRSDEstimationComponents",
      "featureRSDEstimationComponentGroups",
      feature_filter_mode_))
    {
      return false;
    }
    sequence_segment_observable_ = &application_handler->sequenceHandler_;
    process(application_handler->sequenceHandler_.getSequenceSegments()[0], SequenceHandler(), {}, filenames);
    return true;
  }

  std::vector<std::string> StoreFeatureRSDEstimations::getRequirements() const
  {
    return { "sequence", "traML" };
  }

  ParameterSet StoreFeatureRSDEstimations::getParameterSchema() const
  {
    return ParameterSet();
  }

  void StoreFeatureRSDEstimations::getFilenames(Filenames& filenames) const
  {
    if (feature_filter_mode_ & FeatureFiltersUtilsMode::EFeatureFiltersModeGroup)
    {
      filenames.addFileName("featureRSDEstimationComponentGroups",
        constructFilename("featureRSDEstimationComponentGroups.csv", static_filenames_),
        "Feature RSD Estimation Component Groups",
        true,
        true);
    }
    else if (feature_filter_mode_ & FeatureFiltersUtilsMode::EFeatureFiltersModeComponent)
    {
      filenames.addFileName("featureRSDEstimationComponents",
        constructFilename("featureRSDEstimationComponents.csv", static_filenames_),
        "Feature RSD Estimation Component",
        true,
        true);
    }
  };

  void StoreFeatureRSDEstimations::process(
    SequenceSegmentHandler& sequenceSegmentHandler_IO,
    const SequenceHandler& sequenceHandler_I,
    const ParameterSet& params_I,
    Filenames& filenames_I
  ) const
  {
    LOGD << "START storeFeatureRSDEstimation";
    getFilenames(filenames_I);
    FeatureFiltersUtils::storeFeatureFilters(
      "featureRSDEstimationComponents",
      "featureRSDEstimationComponentGroups",
      filenames_I,
      sequenceSegmentHandler_IO.getFeatureRSDEstimations(),
      feature_filter_mode_);
    LOGD << "END storeFeatureRSDEstimation";
  }

  std::vector<std::string> LoadFeatureBackgroundEstimations::getRequirements() const
  {
    return { "sequence", "traML" };
  }

  ParameterSet LoadFeatureBackgroundEstimations::getParameterSchema() const
  {
    return ParameterSet();
  }

  void LoadFeatureBackgroundEstimations::getFilenames(Filenames& filenames) const
  {
    if (feature_filter_mode_ & FeatureFiltersUtilsMode::EFeatureFiltersModeGroup)
    {
      filenames.addFileName("featureBackgroundEstimationComponentGroups",
        constructFilename("featureBackgroundEstimationComponentGroups.csv", static_filenames_),
        "Feature Background Estimation Groups",
        true,
        true
      );
    }
    else if (feature_filter_mode_ & FeatureFiltersUtilsMode::EFeatureFiltersModeComponent)
    {
      filenames.addFileName("featureBackgroundEstimationComponents",
        constructFilename("featureBackgroundEstimationComponents.csv", static_filenames_),
        "Feature Background Estimation",
        true,
        true
      );
    }
  };

  void LoadFeatureBackgroundEstimations::process(
    SequenceSegmentHandler& sequenceSegmentHandler_IO,
    const SequenceHandler& sequenceHandler_I,
    const ParameterSet& params_I,
    Filenames& filenames_I
  ) const
  {
    LOGD << "START loadFeatureBackgroundEstimation";
    getFilenames(filenames_I);
    FeatureFiltersUtils::loadFeatureFilters(
      "featureBackgroundEstimationComponents",
      "featureBackgroundEstimationComponentGroups",
      filenames_I,
      sequenceSegmentHandler_IO.getFeatureBackgroundEstimations(),
      nullptr,
      nullptr,
      feature_filter_mode_
    );
    LOGD << "END loadFeatureBackgroundEstimation";
  }

  bool StoreFeatureBackgroundEstimations::onFilePicked(const std::filesystem::path& filename, ApplicationHandler* application_handler)
  {
    Filenames filenames;
    if (!FeatureFiltersUtils::onFilePicked(
      filename,
      application_handler,
      filenames,
      "featureBackgroundEstimationComponents",
      "featureBackgroundEstimationComponentGroups",
      feature_filter_mode_))
    {
      return false;
    }
    sequence_segment_observable_ = &application_handler->sequenceHandler_;
    process(application_handler->sequenceHandler_.getSequenceSegments()[0], SequenceHandler(), {}, filenames);
    return true;
  }

  std::vector<std::string> StoreFeatureBackgroundEstimations::getRequirements() const
  {
    return { "sequence", "traML" };
  }

  ParameterSet StoreFeatureBackgroundEstimations::getParameterSchema() const
  {
    return ParameterSet();
  }

  void StoreFeatureBackgroundEstimations::getFilenames(Filenames& filenames) const
  {
    if (feature_filter_mode_ & FeatureFiltersUtilsMode::EFeatureFiltersModeGroup)
    {
      filenames.addFileName("featureBackgroundEstimationComponentGroups",
        constructFilename("featureBackgroundEstimationComponentGroups.csv", static_filenames_),
        "Feature Background Estimation Groups",
        true,
        true
      );
    }
    else if (feature_filter_mode_ & FeatureFiltersUtilsMode::EFeatureFiltersModeComponent)
    {
      filenames.addFileName("featureBackgroundEstimationComponents",
        constructFilename("featureBackgroundEstimationComponents.csv", static_filenames_),
        "Feature Background Estimation",
        true,
        true
        );
    }
  };

  void StoreFeatureBackgroundEstimations::process(
    SequenceSegmentHandler& sequenceSegmentHandler_IO,
    const SequenceHandler& sequenceHandler_I,
    const ParameterSet& params_I,
    Filenames& filenames_I
  ) const
  {
    LOGD << "START storeFeatureBackgroundEstimation";
    getFilenames(filenames_I);
    FeatureFiltersUtils::storeFeatureFilters(
      "featureBackgroundEstimationComponents",
      "featureBackgroundEstimationComponentGroups",
      filenames_I,
      sequenceSegmentHandler_IO.getFeatureBackgroundEstimations(),
      feature_filter_mode_);
    LOGD << "END storeFeatureBackgroundEstimation";
  }
}
