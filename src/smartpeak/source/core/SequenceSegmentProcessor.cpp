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

  bool LoadStandardsConcentrations::onFilePicked(const std::filesystem::path& filename, ApplicationHandler* application_handler)
  {
    if (application_handler->sequenceHandler_.getSequence().size() == 0)
    {
      LOGE << "File cannot be loaded without first loading the sequence.";
      return false;
    }
    Filenames filenames;
    filenames.setFullPath("quantitationMethods", filename);
    for (SequenceSegmentHandler& sequenceSegmentHandler : application_handler->sequenceHandler_.getSequenceSegments()) {
      sequence_segment_observable_ = &(application_handler->sequenceHandler_);
      process(sequenceSegmentHandler, SequenceHandler(), {}, filenames);
    }
    return true;
  }

  void LoadStandardsConcentrations::getFilenames(Filenames& filenames) const
  {
    filenames.addFileName("standardsConcentrations", "${MAIN_DIR}/standardsConcentrations.csv", "Standards Concentrations", true, true);
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
      LOGD << "END " << getName();
      return;
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
          return;
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
      LOGE << e.what();
      sequenceSegmentHandler_IO.getStandardsConcentrations().clear();
      LOGI << "Standards concentrations clear";
    }

    LOGD << "END loadStandardsConcentrations";
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
    for (SequenceSegmentHandler& sequenceSegmentHandler : application_handler->sequenceHandler_.getSequenceSegments()) {
      sequence_segment_observable_ = &(application_handler->sequenceHandler_);
      process(sequenceSegmentHandler, SequenceHandler(), {}, filenames);
    }
    return true;
  }

  void StoreStandardsConcentrations::getFilenames(Filenames& filenames) const
  {
    filenames.addFileName("standardsConcentrations", "${MAIN_DIR}/standardsConcentrations.csv", "Standards Concentrations", true, true);
  };

  void StoreStandardsConcentrations::process(
    SequenceSegmentHandler& sequenceSegmentHandler_IO,
    const SequenceHandler& sequenceHandler_I,
    const ParameterSet& params_I,
    Filenames& filenames_I
  ) const
  {
    LOGD << "START StoreStandardsConcentrations";

    try
    {
      if (!InputDataValidation::prepareToStore(filenames_I, "standardsConcentrations"))
      {
        LOGD << "END " << getName();
        return;
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
          return;
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
        // TODO
      }
      filenames_I.setSavedState("standardsConcentrations", true);
    }
    catch (const std::exception& e)
    {
      LOGE << e.what();
    }

    LOGD << "END StoreStandardsConcentrations";
  }

  ParameterSet LoadQuantitationMethods::getParameterSchema() const
  {
    return ParameterSet();
  }

  void LoadQuantitationMethods::getFilenames(Filenames& filenames) const
  {
    filenames.addFileName("quantitationMethods", "${MAIN_DIR}/quantitationMethods.csv", "Quantitation Methods");
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
    Filenames& filenames_I
  ) const
  {
    LOGD << "START loadQuantitationMethods";
    getFilenames(filenames_I);

    if (!InputDataValidation::prepareToLoad(filenames_I, "quantitationMethods"))
    {
      LOGD << "END " << getName();
      return;
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

  ParameterSet StoreQuantitationMethods::getParameterSchema() const
  {
    return ParameterSet();
  }

  void StoreQuantitationMethods::getFilenames(Filenames& filenames) const
  {
    filenames.addFileName("quantitationMethods", "${MAIN_DIR}/quantitationMethods.csv", "Quantitation Methods");
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
      LOGD << "END " << getName();
      return;
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

  ParameterSet LoadFeatureFilters::getParameterSchema() const
  {
    return ParameterSet();
  }

  bool LoadFeatureFilters::onFilePicked(const std::filesystem::path& filename, ApplicationHandler* application_handler)
  {
    if (application_handler->sequenceHandler_.getSequence().size() == 0)
    {
      LOGE << "File cannot be loaded without first loading the sequence.";
      return false;
    }
    Filenames filenames;
    if (component_group_)
    {
      filenames.setFullPath("featureFilterComponents", "");
      filenames.setFullPath("featureFilterComponentGroups", filename);
    }
    else
    {
      filenames.setFullPath("featureFilterComponents", filename);
      filenames.setFullPath("featureFilterComponentGroups", "");
    }
    for (SequenceSegmentHandler& sequenceSegmentHandler : application_handler->sequenceHandler_.getSequenceSegments()) {
      sequence_segment_observable_ = &(application_handler->sequenceHandler_);
      process(sequenceSegmentHandler, SequenceHandler(), {}, filenames);
    }
    return true;
  }

  void LoadFeatureFilters::getFilenames(Filenames& filenames) const
  {
    if (component_group_)
    {
      filenames.addFileName("featureFilterComponentGroups", "${MAIN_DIR}/featureFilterComponentGroups.csv", "Components Group Filters", true, true);
    }
    else
    {
      filenames.addFileName("featureFilterComponents", "${MAIN_DIR}/featureFilterComponents.csv", "Components Filters", true, true);
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
    FeatureFiltersUtils::loadFeatureFiltersFromDB(
      "featureFilterComponents",
      "featureFilterComponentGroups",
      filenames_I,
      sequenceSegmentHandler_IO.getFeatureFilter(),
      [&]() { if (sequence_segment_observable_) sequence_segment_observable_->notifyFeatureFiltersComponentsUpdated(); },
      [&]() { if (sequence_segment_observable_) sequence_segment_observable_->notifyFeatureFiltersComponentGroupsUpdated(); }
    );
    LOGD << "END loadFeatureFilter";
  }

  ParameterSet LoadFeatureQCs::getParameterSchema() const
  {
    return ParameterSet();
  }

  bool LoadFeatureQCs::onFilePicked(const std::filesystem::path& filename, ApplicationHandler* application_handler)
  {
    if (application_handler->sequenceHandler_.getSequence().size() == 0)
    {
      LOGE << "File cannot be loaded without first loading the sequence.";
      return false;
    }
    Filenames filenames;
    if (component_group_)
    {
      filenames.setFullPath("featureQCComponents", "");
      filenames.setFullPath("featureQCComponentGroups", filename);
    }
    else
    {
      filenames.setFullPath("featureQCComponents", filename);
      filenames.setFullPath("featureQCComponentGroups", "");
    }
    for (SequenceSegmentHandler& sequenceSegmentHandler : application_handler->sequenceHandler_.getSequenceSegments()) {
      sequence_segment_observable_ = &(application_handler->sequenceHandler_);
      process(sequenceSegmentHandler, SequenceHandler(), {}, filenames);
    }
    return true;
  }

  void LoadFeatureQCs::getFilenames(Filenames& filenames) const
  {
    if (component_group_)
    {
      filenames.addFileName("featureQCComponentGroups", "${MAIN_DIR}/featureQCComponentGroups.csv", "Components Group QCs", true, true);
    }
    else
    {
      filenames.addFileName("featureQCComponents", "${MAIN_DIR}/featureQCComponents.csv", "Components QCs", true, true);
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
    FeatureFiltersUtils::loadFeatureFiltersFromDB(
      "featureQCComponents",
      "featureQCComponentGroups",
      filenames_I,
      sequenceSegmentHandler_IO.getFeatureQC(),
      [&]() { if (sequence_segment_observable_) sequence_segment_observable_->notifyFeatureQCComponentsUpdated(); },
      [&]() { if (sequence_segment_observable_) sequence_segment_observable_->notifyFeatureQCComponentGroupsUpdated(); }
    );
    LOGD << "END loadFeatureQC";
  }

  ParameterSet StoreFeatureFilters::getParameterSchema() const
  {
    return ParameterSet();
  }

  bool StoreFeatureFilters::onFilePicked(const std::filesystem::path& filename, ApplicationHandler* application_handler)
  {
    if (application_handler->sequenceHandler_.getSequence().size() == 0)
    {
      LOGE << "File cannot be loaded without first loading the sequence.";
      return false;
    }
    Filenames filenames;
    if (component_group_)
    {
      filenames.setFullPath("featureFilterComponents", "");
      filenames.setFullPath("featureFilterComponentGroups", filename);
    }
    else
    {
      filenames.setFullPath("featureFilterComponents", filename);
      filenames.setFullPath("featureFilterComponentGroups", "");
    }
    for (SequenceSegmentHandler& sequenceSegmentHandler : application_handler->sequenceHandler_.getSequenceSegments()) {
      sequence_segment_observable_ = &(application_handler->sequenceHandler_);
      process(sequenceSegmentHandler, SequenceHandler(), {}, filenames);
    }
    return true;
  }

  void StoreFeatureFilters::getFilenames(Filenames& filenames) const
  {
    if (component_group_)
    {
      filenames.addFileName("featureFilterComponentGroups", "${MAIN_DIR}/featureFilterComponentGroups.csv", "Components Group Filters", true, true);
    }
    else
    {
      filenames.addFileName("featureFilterComponents", "${MAIN_DIR}/featureFilterComponents.csv", "Components Filters", true, true);
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
    FeatureFiltersUtils::storeFeatureFiltersInDB(
      "featureFilterComponents",
      "featureFilterComponentGroups",
      filenames_I,
      sequenceSegmentHandler_IO.getFeatureFilter());
    LOGD << "END storeFeatureFilter";
  }

  ParameterSet StoreFeatureQCs::getParameterSchema() const
  {
    return ParameterSet();
  }

  bool StoreFeatureQCs::onFilePicked(const std::filesystem::path& filename, ApplicationHandler* application_handler)
  {
    if (application_handler->sequenceHandler_.getSequence().size() == 0)
    {
      LOGE << "File cannot be loaded without first loading the sequence.";
      return false;
    }
    Filenames filenames;
    if (component_group_)
    {
      filenames.setFullPath("featureQCComponents", "");
      filenames.setFullPath("featureQCComponentGroups", filename);
    }
    else
    {
      filenames.setFullPath("featureQCComponents", filename);
      filenames.setFullPath("featureQCComponentGroups", "");
    }
    for (SequenceSegmentHandler& sequenceSegmentHandler : application_handler->sequenceHandler_.getSequenceSegments()) {
      sequence_segment_observable_ = &(application_handler->sequenceHandler_);
      process(sequenceSegmentHandler, SequenceHandler(), {}, filenames);
    }
    return true;
  }

  void StoreFeatureQCs::getFilenames(Filenames& filenames) const
  {
    if (component_group_)
    {
      filenames.addFileName("featureQCComponentGroups", "${MAIN_DIR}/featureQCComponentGroups.csv", "Components Group QC", true, true);
    }
    else
    {
      filenames.addFileName("featureQCComponents", "${MAIN_DIR}/featureQCComponents.csv", "Components QC", true, true);
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
    FeatureFiltersUtils::storeFeatureFiltersInDB(
      "featureQCComponents",
      "featureQCComponentGroups",
      filenames_I,
      sequenceSegmentHandler_IO.getFeatureQC());
    LOGD << "END storeFeatureQC";
  }

  ParameterSet LoadFeatureRSDFilters::getParameterSchema() const
  {
    return ParameterSet();
  }

  void LoadFeatureRSDFilters::getFilenames(Filenames& filenames) const
  {
    if (component_group_)
    {
      filenames.addFileName("featureRSDFilterComponentGroups", 
                            "${MAIN_DIR}/featureRSDFilterComponentGroups.csv",
                            "Components group %RSD Filters",
                            true,
                            true);
    }
    else
    {
      filenames.addFileName("featureRSDFilterComponents",
                            "${MAIN_DIR}/featureRSDFilterComponents.csv",
                            "Components %RSD Filters",
                            true,
                            true);
    }
  };

  bool LoadFeatureRSDFilters::onFilePicked(const std::filesystem::path& filename, ApplicationHandler* application_handler)
  {
    if (application_handler->sequenceHandler_.getSequence().size() == 0)
    {
      LOGE << "File cannot be loaded without first loading the sequence.";
      return false;
    }
    Filenames filenames;
    if (component_group_)
    {
      filenames.setFullPath("featureRSDFilterComponents", "");
      filenames.setFullPath("featureRSDFilterComponentGroups", filename);
    }
    else
    {
      filenames.setFullPath("featureRSDFilterComponents", filename);
      filenames.setFullPath("featureRSDFilterComponentGroups", "");
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
    Filenames& filenames_I
  ) const
  {
    LOGD << "START loadFeatureRSDFilter";
    getFilenames(filenames_I);
    FeatureFiltersUtils::loadFeatureFiltersFromDB(
      "featureRSDFilterComponents",
      "featureRSDFilterComponentGroups",
      filenames_I,
      sequenceSegmentHandler_IO.getFeatureRSDFilter(),
      [&]() { if (sequence_segment_observable_) sequence_segment_observable_->notifyFeatureRSDFilterComponentsUpdated(); },
      [&]() { if (sequence_segment_observable_) sequence_segment_observable_->notifyFeatureRSDFilterComponentGroupsUpdated(); }
    );
    LOGD << "END loadFeatureRSDFilter";
  }

  ParameterSet LoadFeatureRSDQCs::getParameterSchema() const
  {
    return ParameterSet();
  }

  void LoadFeatureRSDQCs::getFilenames(Filenames& filenames) const
  {
    if (component_group_)
    {
      filenames.addFileName("featureRSDQCComponentGroups",
                            "${MAIN_DIR}/featureRSDQCComponentGroups.csv",
                            "Components Group %RSD QCs",
                            true,
                            true);
    }
    else
    {
      filenames.addFileName("featureRSDQCComponents",
                            "${MAIN_DIR}/featureRSDQCComponents.csv",
                            "Components %RSD QCs",
                            true,
                            true);
    }
  };

  bool LoadFeatureRSDQCs::onFilePicked(const std::filesystem::path& filename, ApplicationHandler* application_handler)
  {
    if (application_handler->sequenceHandler_.getSequence().size() == 0)
    {
      LOGE << "File cannot be loaded without first loading the sequence.";
      return false;
    }
    Filenames filenames;
    if (component_group_)
    {
      filenames.setFullPath("featureRSDQCComponents", "");
      filenames.setFullPath("featureRSDQCComponentGroups", filename);
    }
    else
    {
      filenames.setFullPath("featureRSDQCComponents", filename);
      filenames.setFullPath("featureRSDQCComponentGroups", "");
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
    Filenames& filenames_I
  ) const
  {
    LOGD << "START loadFeatureRSDQC";
    getFilenames(filenames_I);
    FeatureFiltersUtils::loadFeatureFiltersFromDB(
      "featureRSDQCComponents",
      "featureRSDQCComponentGroups",
      filenames_I,
      sequenceSegmentHandler_IO.getFeatureRSDQC(),
      [&]() { if (sequence_segment_observable_) sequence_segment_observable_->notifyFeatureRSDQCComponentsUpdated(); },
      [&]() { if (sequence_segment_observable_) sequence_segment_observable_->notifyFeatureRSDQCComponentGroupsUpdated(); }
    );
    LOGD << "END loadFeatureRSDQC";
  }

  ParameterSet StoreFeatureRSDFilters::getParameterSchema() const
  {
    return ParameterSet();
  }

  bool StoreFeatureRSDFilters::onFilePicked(const std::filesystem::path& filename, ApplicationHandler* application_handler)
  {
    if (application_handler->sequenceHandler_.getSequence().size() == 0)
    {
      LOGE << "File cannot be loaded without first loading the sequence.";
      return false;
    }
    Filenames filenames;
    if (component_group_)
    {
      filenames.setFullPath("featureRSDFilterComponents", "");
      filenames.setFullPath("featureRSDFilterComponentGroups", filename);
    }
    else
    {
      filenames.setFullPath("featureRSDFilterComponents", filename);
      filenames.setFullPath("featureRSDFilterComponentGroups", "");
    }
    for (SequenceSegmentHandler& sequenceSegmentHandler : application_handler->sequenceHandler_.getSequenceSegments()) {
      sequence_segment_observable_ = &(application_handler->sequenceHandler_);
      process(sequenceSegmentHandler, SequenceHandler(), {}, filenames);
    }
    return true;
  }

  void StoreFeatureRSDFilters::getFilenames(Filenames& filenames) const
  {
    if (component_group_)
    {
      filenames.addFileName("featureRSDFilterComponentGroups",
                            "${MAIN_DIR}/featureRSDFilterComponentGroups.csv",
                            "Components group %RSD Filters");
    }
    else
    {
      filenames.addFileName("featureRSDFilterComponents",
                            "${MAIN_DIR}/featureRSDFilterComponents.csv",
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
    FeatureFiltersUtils::storeFeatureFiltersInDB(
      "featureRSDFilterComponents",
      "featureRSDFilterComponentGroups",
      filenames_I,
      sequenceSegmentHandler_IO.getFeatureRSDFilter());
    LOGD << "END storeFeatureRSDFilter";
  }

  ParameterSet StoreFeatureRSDQCs::getParameterSchema() const
  {
    return ParameterSet();
  }

  bool StoreFeatureRSDQCs::onFilePicked(const std::filesystem::path& filename, ApplicationHandler* application_handler)
  {
    if (application_handler->sequenceHandler_.getSequence().size() == 0)
    {
      LOGE << "File cannot be loaded without first loading the sequence.";
      return false;
    }
    Filenames filenames;
    if (component_group_)
    {
      filenames.setFullPath("featureRSDQCComponents", "");
      filenames.setFullPath("featureRSDQCComponentGroups", filename);
    }
    else
    {
      filenames.setFullPath("featureRSDQCComponents", filename);
      filenames.setFullPath("featureRSDQCComponentGroups", "");
    }
    for (SequenceSegmentHandler& sequenceSegmentHandler : application_handler->sequenceHandler_.getSequenceSegments()) {
      sequence_segment_observable_ = &(application_handler->sequenceHandler_);
      process(sequenceSegmentHandler, SequenceHandler(), {}, filenames);
    }
    return true;
  }

  void StoreFeatureRSDQCs::getFilenames(Filenames& filenames) const
  {
    if (component_group_)
    {
      filenames.addFileName("featureRSDQCComponentGroups",
                            "${MAIN_DIR}/featureRSDQCComponentGroups.csv",
                            "Components Group %RSD QCs");
    }
    else
    {
      filenames.addFileName("featureRSDQCComponents",
                            "${MAIN_DIR}/featureRSDQCComponents.csv",
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
    FeatureFiltersUtils::storeFeatureFiltersInDB(
      "featureRSDQCComponents",
      "featureRSDQCComponentGroups",
      filenames_I,
      sequenceSegmentHandler_IO.getFeatureRSDQC());
    LOGD << "END storeFeatureRSDQC";
  }

  ParameterSet LoadFeatureBackgroundFilters::getParameterSchema() const
  {
    return ParameterSet();
  }

  void LoadFeatureBackgroundFilters::getFilenames(Filenames& filenames) const
  {
    if (component_group_)
    {
      filenames.addFileName("featureBackgroundFilterComponentGroups", 
        "${MAIN_DIR}/featureBackgroundFilterComponentGroups.csv", 
        "Components Group % Background Filters",
        true,
        true);
    }
    else
    {
      filenames.addFileName("featureBackgroundFilterComponents", 
        "${MAIN_DIR}/featureBackgroundFilterComponents.csv", 
        "Components % Background Filters",
        true,
        true);
    }
  };

  bool LoadFeatureBackgroundFilters::onFilePicked(const std::filesystem::path& filename, ApplicationHandler* application_handler)
  {
    if (application_handler->sequenceHandler_.getSequence().size() == 0)
    {
      LOGE << "File cannot be loaded without first loading the sequence.";
      return false;
    }
    Filenames filenames;
    if (component_group_)
    {
      filenames.setFullPath("featureBackgroundFilterComponents", "");
      filenames.setFullPath("featureBackgroundFilterComponentGroups", filename);
    }
    else
    {
      filenames.setFullPath("featureBackgroundFilterComponents", filename);
      filenames.setFullPath("featureBackgroundFilterComponentGroups", "");
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
    Filenames& filenames_I
  ) const
  {
    LOGD << "START loadFeatureBackgroundFilter";
    getFilenames(filenames_I);
    FeatureFiltersUtils::loadFeatureFiltersFromDB(
      "featureBackgroundFilterComponents",
      "featureBackgroundFilterComponentGroups",
      filenames_I,
      sequenceSegmentHandler_IO.getFeatureBackgroundFilter(),
      [&]() { if (sequence_segment_observable_) sequence_segment_observable_->notifyFeatureBackgroundFilterComponentsUpdated(); },
      [&]() { if (sequence_segment_observable_) sequence_segment_observable_->notifyFeatureBackgroundFilterComponentGroupsUpdated(); }
    );
    LOGD << "END loadFeatureBackgroundFilter";
  }

  ParameterSet LoadFeatureBackgroundQCs::getParameterSchema() const
  {
    return ParameterSet();
  }

  void LoadFeatureBackgroundQCs::getFilenames(Filenames& filenames) const
  {
    if (component_group_)
    {
      filenames.addFileName("featureBackgroundQCComponentGroups",
                            "${MAIN_DIR}/featureBackgroundQCComponentGroups.csv",
                            "Components Group %Background QCs",
                            true,
                            true);
    }
    else
    {
      filenames.addFileName("featureBackgroundQCComponents",
                            "${MAIN_DIR}/featureBackgroundQCComponents.csv",
                            "Components % Background QCs",
                            true,
                            true);
    }
  };

  bool LoadFeatureBackgroundQCs::onFilePicked(const std::filesystem::path& filename, ApplicationHandler* application_handler)
  {
    if (application_handler->sequenceHandler_.getSequence().size() == 0)
    {
      LOGE << "File cannot be loaded without first loading the sequence.";
      return false;
    }
    Filenames filenames;
    if (component_group_)
    {
      filenames.setFullPath("featureBackgroundQCComponents", "");
      filenames.setFullPath("featureBackgroundQCComponentGroups", filename);
    }
    else
    {
      filenames.setFullPath("featureBackgroundQCComponents", filename);
      filenames.setFullPath("featureBackgroundQCComponentGroups", "");
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
    Filenames& filenames_I
  ) const
  {
    LOGD << "START loadFeatureBackgroundQC";
    getFilenames(filenames_I);
    FeatureFiltersUtils::loadFeatureFiltersFromDB(
      "featureBackgroundQCComponents",
      "featureBackgroundQCComponentGroups",
      filenames_I,
      sequenceSegmentHandler_IO.getFeatureBackgroundQC(),
      [&]() { if (sequence_segment_observable_) sequence_segment_observable_->notifyFeatureBackgroundQCComponentsUpdated(); },
      [&]() { if (sequence_segment_observable_) sequence_segment_observable_->notifyFeatureBackgroundQCComponentGroupsUpdated(); }
    );
    LOGD << "END loadFeatureBackgroundQC";
  }

  ParameterSet StoreFeatureBackgroundFilters::getParameterSchema() const
  {
    return ParameterSet();
  }

  bool StoreFeatureBackgroundFilters::onFilePicked(const std::filesystem::path& filename, ApplicationHandler* application_handler)
  {
    if (application_handler->sequenceHandler_.getSequence().size() == 0)
    {
      LOGE << "File cannot be loaded without first loading the sequence.";
      return false;
    }
    Filenames filenames;
    if (component_group_)
    {
      filenames.setFullPath("featureBackgroundFilterComponents", "");
      filenames.setFullPath("featureBackgroundFilterComponentGroups", filename);
    }
    else
    {
      filenames.setFullPath("featureBackgroundFilterComponents", filename);
      filenames.setFullPath("featureBackgroundFilterComponentGroups", "");
    }
    for (SequenceSegmentHandler& sequenceSegmentHandler : application_handler->sequenceHandler_.getSequenceSegments()) {
      sequence_segment_observable_ = &(application_handler->sequenceHandler_);
      process(sequenceSegmentHandler, SequenceHandler(), {}, filenames);
    }
    return true;
  }

  void StoreFeatureBackgroundFilters::getFilenames(Filenames& filenames) const
  {
    if (component_group_)
    {
      filenames.addFileName("featureBackgroundFilterComponentGroups",
                            "${MAIN_DIR}/featureBackgroundFilterComponentGroups.csv",
                            "Components Group % Background Filters");
    }
    else
    {
      filenames.addFileName("featureBackgroundFilterComponents",
                            "${MAIN_DIR}/featureBackgroundFilterComponents.csv",
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
    FeatureFiltersUtils::storeFeatureFiltersInDB(
      "featureBackgroundFilterComponents",
      "featureBackgroundFilterComponentGroups",
      filenames_I,
      sequenceSegmentHandler_IO.getFeatureBackgroundFilter());
    LOGD << "END storeFeatureBackgroundFilter";
  }

  ParameterSet StoreFeatureBackgroundQCs::getParameterSchema() const
  {
    return ParameterSet();
  }

  bool StoreFeatureBackgroundQCs::onFilePicked(const std::filesystem::path& filename, ApplicationHandler* application_handler)
  {
    if (application_handler->sequenceHandler_.getSequence().size() == 0)
    {
      LOGE << "File cannot be loaded without first loading the sequence.";
      return false;
    }
    Filenames filenames;
    if (component_group_)
    {
      filenames.setFullPath("featureBackgroundQCComponents", "");
      filenames.setFullPath("featureBackgroundQCComponentGroups", filename);
    }
    else
    {
      filenames.setFullPath("featureBackgroundQCComponents", filename);
      filenames.setFullPath("featureBackgroundQCComponentGroups", "");
    }
    for (SequenceSegmentHandler& sequenceSegmentHandler : application_handler->sequenceHandler_.getSequenceSegments()) {
      sequence_segment_observable_ = &(application_handler->sequenceHandler_);
      process(sequenceSegmentHandler, SequenceHandler(), {}, filenames);
    }
    return true;
  }

  void StoreFeatureBackgroundQCs::getFilenames(Filenames& filenames) const
  {
    if (component_group_)
    {
      filenames.addFileName("featureBackgroundQCComponentGroups",
                            "${MAIN_DIR}/featureBackgroundQCComponentGroups.csv",
                            "Components Group %Background QCs");
    }
    else
    {
      filenames.addFileName("featureBackgroundQCComponents",
                            "${MAIN_DIR}/featureBackgroundQCComponents.csv",
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
    FeatureFiltersUtils::storeFeatureFiltersInDB(
      "featureBackgroundQCComponents",
      "featureBackgroundQCComponentGroups",
      filenames_I,
      sequenceSegmentHandler_IO.getFeatureBackgroundQC());
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
    Filenames& filenames_I
  ) const
  {
    LOGD << "START TransferLOQToFeatureFilters";
    getFilenames(filenames_I);

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
    Filenames& filenames_I
  ) const
  {
    LOGD << "START TransferLOQToFeatureQCs";
    getFilenames(filenames_I);

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

  void LoadFeatureRSDEstimations::getFilenames(Filenames& filenames) const
  {
    if (component_group_)
    {
      filenames.addFileName("featureRSDEstimationComponentGroups",
        "${MAIN_DIR}/featureRSDEstimationComponentGroups.csv",
        "Feature RSD Estimation Component Groups",
        true,
        true);
    }
    else
    {
      filenames.addFileName("featureRSDEstimationComponents",
        "${MAIN_DIR}/featureRSDEstimationComponents.csv",
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
    FeatureFiltersUtils::loadFeatureFiltersFromDB(
      "featureRSDEstimationComponents",
      "featureRSDEstimationComponentGroups",
      filenames_I,
      sequenceSegmentHandler_IO.getFeatureRSDEstimations(),
      nullptr,
      nullptr
    );
    LOGD << "END loadFeatureRSDEstimation";
  }

  bool StoreFeatureRSDEstimations::onFilePicked(const std::filesystem::path& filename, ApplicationHandler* application_handler)
  {
    if (application_handler->sequenceHandler_.getSequence().size() == 0)
    {
      LOGE << "File cannot be loaded without first loading the sequence.";
      return false;
    }
    Filenames filenames;
    if (component_group_)
    {
      filenames.setFullPath("featureRSDEstimationComponents", "");
      filenames.setFullPath("featureRSDEstimationComponentGroups", filename);
    }
    else
    {
      filenames.setFullPath("featureRSDEstimationComponents", filename);
      filenames.setFullPath("featureRSDEstimationComponentGroups", "");
    }
    for (SequenceSegmentHandler& sequenceSegmentHandler : application_handler->sequenceHandler_.getSequenceSegments()) {
      sequence_segment_observable_ = &(application_handler->sequenceHandler_);
      process(sequenceSegmentHandler, SequenceHandler(), {}, filenames);
    }
    return true;
  }

  ParameterSet StoreFeatureRSDEstimations::getParameterSchema() const
  {
    return ParameterSet();
  }

  void StoreFeatureRSDEstimations::getFilenames(Filenames& filenames) const
  {
    if (component_group_)
    {
      filenames.addFileName("featureRSDEstimationComponentGroups",
        "${MAIN_DIR}/featureRSDEstimationComponentGroups.csv",
        "Feature RSD Estimation Component Groups",
        true,
        true);
    }
    else
    {
      filenames.addFileName("featureRSDEstimationComponents",
        "${MAIN_DIR}/featureRSDEstimationComponents.csv",
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
    FeatureFiltersUtils::storeFeatureFiltersInDB(
      "featureRSDEstimationComponents",
      "featureRSDEstimationComponentGroups",
      filenames_I,
      sequenceSegmentHandler_IO.getFeatureRSDEstimations());
    LOGD << "END storeFeatureRSDEstimation";
  }

  ParameterSet LoadFeatureBackgroundEstimations::getParameterSchema() const
  {
    return ParameterSet();
  }

  void LoadFeatureBackgroundEstimations::getFilenames(Filenames& filenames) const
  {
    if (component_group_)
    {
      filenames.addFileName("featureBackgroundEstimationComponentGroups",
        "${MAIN_DIR}/featureBackgroundEstimationComponentGroups.csv",
        "Feature Background Estimation Groups",
        true,
        true
      );
    }
    else
    {
      filenames.addFileName("featureBackgroundEstimationComponents",
        "${MAIN_DIR}/featureBackgroundEstimationComponents.csv",
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
    FeatureFiltersUtils::loadFeatureFiltersFromDB(
      "featureBackgroundEstimationComponents",
      "featureBackgroundEstimationComponentGroups",
      filenames_I,
      sequenceSegmentHandler_IO.getFeatureBackgroundEstimations(),
      nullptr,
      nullptr
    );
    LOGD << "END loadFeatureBackgroundEstimation";
  }

  bool StoreFeatureBackgroundEstimations::onFilePicked(const std::filesystem::path& filename, ApplicationHandler* application_handler)
  {
    if (application_handler->sequenceHandler_.getSequence().size() == 0)
    {
      LOGE << "File cannot be loaded without first loading the sequence.";
      return false;
    }
    Filenames filenames;
    if (component_group_)
    {
      filenames.setFullPath("featureBackgroundEstimationComponents", "");
      filenames.setFullPath("featureBackgroundEstimationComponentGroups", filename);
    }
    else
    {
      filenames.setFullPath("featureBackgroundEstimationComponents", filename);
      filenames.setFullPath("featureBackgroundEstimationComponentGroups", "");
    }
    for (SequenceSegmentHandler& sequenceSegmentHandler : application_handler->sequenceHandler_.getSequenceSegments()) {
      sequence_segment_observable_ = &(application_handler->sequenceHandler_);
      process(sequenceSegmentHandler, SequenceHandler(), {}, filenames);
    }
    return true;
  }

  ParameterSet StoreFeatureBackgroundEstimations::getParameterSchema() const
  {
    return ParameterSet();
  }

  void StoreFeatureBackgroundEstimations::getFilenames(Filenames& filenames) const
  {
    if (component_group_)
    {
      filenames.addFileName("featureBackgroundEstimationComponentGroups",
        "${MAIN_DIR}/featureBackgroundEstimationComponentGroups.csv",
        "Feature Background Estimation Groups",
        true,
        true
      );
    }
    else
    {
      filenames.addFileName("featureBackgroundEstimationComponents",
        "${MAIN_DIR}/featureBackgroundEstimationComponents.csv",
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
    FeatureFiltersUtils::storeFeatureFiltersInDB(
      "featureBackgroundEstimationComponents",
      "featureBackgroundEstimationComponentGroups",
      filenames_I,
      sequenceSegmentHandler_IO.getFeatureBackgroundEstimations());
    LOGD << "END storeFeatureBackgroundEstimation";
  }
}
