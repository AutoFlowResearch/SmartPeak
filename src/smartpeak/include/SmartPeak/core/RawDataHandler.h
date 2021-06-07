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

#pragma once

#include <OpenMS/ANALYSIS/MAPMATCHING/TransformationDescription.h>
#include <OpenMS/ANALYSIS/OPENSWATH/MRMFeatureQC.h>
#include <OpenMS/ANALYSIS/QUANTITATION/AbsoluteQuantitationMethod.h>
#include <OpenMS/ANALYSIS/TARGETED/TargetedExperiment.h>
#include <OpenMS/KERNEL/FeatureMap.h>
#include <OpenMS/KERNEL/MSExperiment.h>
#include <OpenMS/FORMAT/MzTab.h>

#include <SmartPeak/core/MetaDataHandler.h>
#include <SmartPeak/core/CastValue.h>
#include <SmartPeak/core/Parameters.h>

#include <map>
#include <vector>

namespace SmartPeak
{

  class RawDataHandler
  {
public:
    RawDataHandler(); ///< constructor required to initialize shared_ptr members

    void setFeatureMap(const OpenMS::FeatureMap& feature_map);
    OpenMS::FeatureMap& getFeatureMap();
    const OpenMS::FeatureMap& getFeatureMap() const;

    void setMetaData(const MetaDataHandler& meta_data);
    void setMetaData(std::shared_ptr<MetaDataHandler>& meta_data);
    MetaDataHandler& getMetaData();
    const MetaDataHandler& getMetaData() const;
    std::shared_ptr<MetaDataHandler>& getMetaDataShared();

    void setParameters(const ParameterSet& parameters);
    void setParameters(std::shared_ptr<ParameterSet>& parameters);
    ParameterSet& getParameters();
    const ParameterSet& getParameters() const;
    std::shared_ptr<ParameterSet>& getParametersShared();

    void setTargetedExperiment(const OpenMS::TargetedExperiment& targeted_exp);
    void setTargetedExperiment(std::shared_ptr<OpenMS::TargetedExperiment>& targeted_exp);
    OpenMS::TargetedExperiment& getTargetedExperiment();
    const OpenMS::TargetedExperiment& getTargetedExperiment() const;
    std::shared_ptr<OpenMS::TargetedExperiment>& getTargetedExperimentShared();

    void setReferenceData(const std::vector<std::map<std::string, CastValue>>& reference_data);
    void setReferenceData(std::shared_ptr<std::vector<std::map<std::string, CastValue>>>& reference_data);
    std::vector<std::map<std::string, CastValue>>& getReferenceData();
    const std::vector<std::map<std::string, CastValue>>& getReferenceData() const;
    std::shared_ptr<std::vector<std::map<std::string, CastValue>>>& getReferenceDataShared();

    void setQuantitationMethods(const std::vector<OpenMS::AbsoluteQuantitationMethod>& quantitation_methods);
    void setQuantitationMethods(std::shared_ptr<std::vector<OpenMS::AbsoluteQuantitationMethod>>& quantitation_methods);
    std::vector<OpenMS::AbsoluteQuantitationMethod>& getQuantitationMethods();
    const std::vector<OpenMS::AbsoluteQuantitationMethod>& getQuantitationMethods() const;
    std::shared_ptr<std::vector<OpenMS::AbsoluteQuantitationMethod>>& getQuantitationMethodsShared();

    void setFeatureFilter(const OpenMS::MRMFeatureQC& feature_filter);
    void setFeatureFilter(std::shared_ptr<OpenMS::MRMFeatureQC>& feature_filter);
    OpenMS::MRMFeatureQC& getFeatureFilter();
    const OpenMS::MRMFeatureQC& getFeatureFilter() const;
    std::shared_ptr<OpenMS::MRMFeatureQC>& getFeatureFilterShared();

    void setFeatureQC(const OpenMS::MRMFeatureQC& feature_qc);
    void setFeatureQC(std::shared_ptr<OpenMS::MRMFeatureQC>& feature_qc);
    OpenMS::MRMFeatureQC& getFeatureQC();
    const OpenMS::MRMFeatureQC& getFeatureQC() const;
    std::shared_ptr<OpenMS::MRMFeatureQC>& getFeatureQCShared();

    void setFeatureRSDFilter(const OpenMS::MRMFeatureQC& feature_rsd_filter);
    void setFeatureRSDFilter(std::shared_ptr<OpenMS::MRMFeatureQC>& feature_rsd_filter);
    OpenMS::MRMFeatureQC& getFeatureRSDFilter();
    const OpenMS::MRMFeatureQC& getFeatureRSDFilter() const;
    std::shared_ptr<OpenMS::MRMFeatureQC>& getFeatureRSDFilterShared();

    void setFeatureRSDQC(const OpenMS::MRMFeatureQC& feature_rsd_qc);
    void setFeatureRSDQC(std::shared_ptr<OpenMS::MRMFeatureQC>& feature_rsd_qc);
    OpenMS::MRMFeatureQC& getFeatureRSDQC();
    const OpenMS::MRMFeatureQC& getFeatureRSDQC() const;
    std::shared_ptr<OpenMS::MRMFeatureQC>& getFeatureRSDQCShared();

    void setFeatureBackgroundFilter(const OpenMS::MRMFeatureQC& feature_background_filter);
    void setFeatureBackgroundFilter(std::shared_ptr<OpenMS::MRMFeatureQC>& feature_background_filter);
    OpenMS::MRMFeatureQC& getFeatureBackgroundFilter();
    const OpenMS::MRMFeatureQC& getFeatureBackgroundFilter() const;
    std::shared_ptr<OpenMS::MRMFeatureQC>& getFeatureBackgroundFilterShared();

    void setFeatureBackgroundQC(const OpenMS::MRMFeatureQC& feature_background_qc);
    void setFeatureBackgroundQC(std::shared_ptr<OpenMS::MRMFeatureQC>& feature_background_qc);
    OpenMS::MRMFeatureQC& getFeatureBackgroundQC();
    const OpenMS::MRMFeatureQC& getFeatureBackgroundQC() const;
    std::shared_ptr<OpenMS::MRMFeatureQC>& getFeatureBackgroundQCShared();

    void setFeatureRSDEstimations(const OpenMS::MRMFeatureQC& feature_rsd_estimations);
    void setFeatureRSDEstimations(std::shared_ptr<OpenMS::MRMFeatureQC>& feature_rsd_estimations);
    OpenMS::MRMFeatureQC& getFeatureRSDEstimations();
    const OpenMS::MRMFeatureQC& getFeatureRSDEstimations() const;
    std::shared_ptr<OpenMS::MRMFeatureQC>& getFeatureRSDEstimationsShared();

    void setFeatureBackgroundEstimations(const OpenMS::MRMFeatureQC& feature_background_estimations);
    void setFeatureBackgroundEstimations(std::shared_ptr<OpenMS::MRMFeatureQC>& feature_background_estimations);
    OpenMS::MRMFeatureQC& getFeatureBackgroundEstimations();
    const OpenMS::MRMFeatureQC& getFeatureBackgroundEstimations() const;
    std::shared_ptr<OpenMS::MRMFeatureQC>& getFeatureBackgroundEstimationsShared();

    void setFeatureMapHistory(const OpenMS::FeatureMap& feature_map_history);
    OpenMS::FeatureMap& getFeatureMapHistory();
    const OpenMS::FeatureMap& getFeatureMapHistory() const;

    void setExperiment(const OpenMS::MSExperiment& experiment);
    OpenMS::MSExperiment& getExperiment();
    const OpenMS::MSExperiment& getExperiment() const;

    void setChromatogramMap(const OpenMS::MSExperiment& chromatogram_map);
    OpenMS::MSExperiment& getChromatogramMap();
    const OpenMS::MSExperiment& getChromatogramMap() const;

    void setTransformationDescription(const OpenMS::TransformationDescription& trafo);
    OpenMS::TransformationDescription& getTransformationDescription();
    const OpenMS::TransformationDescription& getTransformationDescription() const;

    void setSWATH(const OpenMS::MSExperiment& swath);
    OpenMS::MSExperiment& getSWATH();
    const OpenMS::MSExperiment& getSWATH() const;

    void setValidationMetrics(const std::map<std::string, float>& validation_metrics);
    std::map<std::string, float>& getValidationMetrics();
    const std::map<std::string, float>& getValidationMetrics() const;

    void setMzTab(const OpenMS::MzTab& mz_tab);
    OpenMS::MzTab& getMzTab();
    const OpenMS::MzTab& getMzTab() const;

    void clear();
    void clearNonSharedData();

    /**
    @brief Update the Feature map history based on the
      filtered, selected, or new features in the current featureMap.

      All new features are initialized as "used_" = True with the "modified_" attribute set to the current time-stamp.
      All removed features are changed to "used_" = False with the "modified_" attribute set to the current time-stamp.

      ASSUMPTIONS: the unique ID attribute is set within OpenMS
    */
    void updateFeatureMapHistory();

    /** 
    @brief Make the feature map from the feature map history by
      filtering out all features/subordinates where "used_" = False.
    */
    void makeFeatureMapFromHistory();

    std::vector<OpenMS::MSSpectrum> annotated_spectra_;

private:
    // input
    OpenMS::MSExperiment experiment_;  ///< Raw MS data derived from the mzML file
    OpenMS::MSExperiment chromatogram_map_;  ///< MS data annotated with transition information derived from the TraML file
    OpenMS::TransformationDescription trafo_;  ///< Mapping of retention time values; currently not used (maybe shared between all raw data handlers)
    OpenMS::MSExperiment swath_;

    // output
    OpenMS::FeatureMap feature_map_; ///< The most recently generated set of features for the experiment
    OpenMS::FeatureMap feature_map_history_; ///< A record of all changes that have occured to the features in the experiment
    std::shared_ptr<MetaDataHandler> meta_data_;  ///< sample meta data; shared between the injection handler and the raw data handler
    std::map<std::string, float> validation_metrics_;
    OpenMS::MzTab mz_tab_;

    // input (reused between RawDataHandlers)
    std::shared_ptr<ParameterSet> parameters_ = nullptr;  ///< algorithm parameters; shared between all raw data handlers in the sequence
    std::shared_ptr<OpenMS::TargetedExperiment> targeted_exp_ = nullptr;  ///< transitions for the SRM experiments; shared between all raw data handlers in the sequence
    std::shared_ptr<std::vector<std::map<std::string, CastValue>>> reference_data_ = nullptr;  ///< Reference data to compare algorithm accuracy; shared between all raw data handlers in the sequence
    std::shared_ptr<std::vector<OpenMS::AbsoluteQuantitationMethod>> quantitation_methods_ = nullptr;  ///< Transition quantitation methods; shared between all raw data handlers in the sequence segment
    std::shared_ptr<OpenMS::MRMFeatureQC> feature_filter_ = nullptr;  ///< Feature Filters; shared between all raw data handlers in the sequence segment
    std::shared_ptr<OpenMS::MRMFeatureQC> feature_qc_ = nullptr;  ///< Feature QCs; shared between all raw data handlers in the sequence segment
    std::shared_ptr<OpenMS::MRMFeatureQC> feature_rsd_filter_ = nullptr;  ///< Feature Filters; shared between all raw data handlers in the sequence segment
    std::shared_ptr<OpenMS::MRMFeatureQC> feature_rsd_qc_ = nullptr;  ///< Feature QCs; shared between all raw data handlers in the sequence segment
    std::shared_ptr<OpenMS::MRMFeatureQC> feature_background_filter_ = nullptr;  ///< Feature Filters; shared between all raw data handlers in the sequence segment
    std::shared_ptr<OpenMS::MRMFeatureQC> feature_background_qc_ = nullptr;  ///< Feature QCs; shared between all raw data handlers in the sequence segment
    std::shared_ptr<OpenMS::MRMFeatureQC> feature_rsd_estimations_ = nullptr;  ///< Percent RSD estimations; shared between all raw data handlers in the sequence segment
    std::shared_ptr<OpenMS::MRMFeatureQC> feature_background_estimations_ = nullptr;  ///< Background interference estimations; shared between all raw data handlers in the sequence segment
  };
}
