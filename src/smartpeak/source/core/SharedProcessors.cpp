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
// $Authors: Douglas McCloskey $
// --------------------------------------------------------------------------

#include <SmartPeak/core/SharedProcessors.h>
#include <SmartPeak/core/RawDataProcessor.h>
#include <SmartPeak/core/SequenceSegmentProcessor.h>
#include <SmartPeak/core/SampleGroupProcessor.h>
#include <SmartPeak/core/RawDataProcessors/LoadRawData.h>
#include <SmartPeak/core/RawDataProcessors/LoadFeatures.h>
#include <SmartPeak/core/RawDataProcessors/LoadTransitions.h>
#include <SmartPeak/core/RawDataProcessors/PickMRMFeatures.h>
#include <SmartPeak/core/RawDataProcessors/FilterFeatures.h>
#include <SmartPeak/core/RawDataProcessors/SelectFeatures.h>
#include <SmartPeak/core/RawDataProcessors/ValidateFeatures.h>
#include <SmartPeak/core/RawDataProcessors/QuantifyFeatures.h>
#include <SmartPeak/core/RawDataProcessors/CheckFeatures.h>
#include <SmartPeak/core/RawDataProcessors/StoreFeatures.h>
#include <SmartPeak/core/RawDataProcessors/MapChromatograms.h>
#include <SmartPeak/core/RawDataProcessors/ZeroChromatogramBaseline.h>
#include <SmartPeak/core/RawDataProcessors/ExtractChromatogramWindows.h>
#include <SmartPeak/core/RawDataProcessors/FitFeaturesEMG.h>
#include <SmartPeak/core/RawDataProcessors/FilterFeaturesRSDs.h>
#include <SmartPeak/core/RawDataProcessors/CheckFeaturesRSDs.h>
#include <SmartPeak/core/RawDataProcessors/FilterFeaturesBackgroundInterferences.h>
#include <SmartPeak/core/RawDataProcessors/CheckFeaturesBackgroundInterferences.h>
#include <SmartPeak/core/RawDataProcessors/ExtractSpectraWindows.h>
#include <SmartPeak/core/RawDataProcessors/ExtractSpectraNonTargeted.h>
#include <SmartPeak/core/RawDataProcessors/MergeSpectra.h>
#include <SmartPeak/core/RawDataProcessors/Pick2DFeatures.h>
#include <SmartPeak/core/RawDataProcessors/Pick3DFeatures.h>
#include <SmartPeak/core/RawDataProcessors/SearchAccurateMass.h>
#include <SmartPeak/core/RawDataProcessors/MergeFeatures.h>
#include <SmartPeak/core/RawDataProcessors/LoadAnnotations.h>
#include <SmartPeak/core/RawDataProcessors/SearchSpectrum.h>
#include <SmartPeak/core/RawDataProcessors/ConstructTransitionsList.h>
#include <SmartPeak/core/RawDataProcessors/StoreAnnotations.h>
#include <SmartPeak/core/RawDataProcessors/ClearData.h>
#include <SmartPeak/core/RawDataProcessors/StoreMSP.h>
#include <SmartPeak/core/RawDataProcessors/StoreRawData.h>
#include <SmartPeak/core/RawDataProcessors/CalculateMDVs.h>
#include <SmartPeak/core/RawDataProcessors/IsotopicCorrections.h>
#include <SmartPeak/core/RawDataProcessors/CalculateIsotopicPurities.h>
#include <SmartPeak/core/RawDataProcessors/CalculateMDVAccuracies.h>
#include <SmartPeak/core/SequenceSegmentProcessors/FitCalibration.h>
#include <SmartPeak/core/SequenceSegmentProcessors/EstimateFeatureBackgroundInterferences.h>
#include <SmartPeak/core/SequenceSegmentProcessors/EstimateFeatureFilterValues.h>
#include <SmartPeak/core/SequenceSegmentProcessors/EstimateFeatureQCValues.h>
#include <SmartPeak/core/SequenceSegmentProcessors/EstimateFeatureRSDs.h>
#include <SmartPeak/core/SequenceSegmentProcessors/LoadFeatureBackgroundEstimations.h>
#include <SmartPeak/core/SequenceSegmentProcessors/LoadFeatureBackgroundFilters.h>
#include <SmartPeak/core/SequenceSegmentProcessors/LoadFeatureBackgroundQCs.h>
#include <SmartPeak/core/SequenceSegmentProcessors/LoadFeatureFilters.h>
#include <SmartPeak/core/SequenceSegmentProcessors/LoadFeatureQCs.h>
#include <SmartPeak/core/SequenceSegmentProcessors/LoadFeatureRSDEstimations.h>
#include <SmartPeak/core/SequenceSegmentProcessors/LoadFeatureRSDFilters.h>
#include <SmartPeak/core/SequenceSegmentProcessors/LoadFeatureRSDQCs.h>
#include <SmartPeak/core/SequenceSegmentProcessors/LoadQuantitationMethods.h>
#include <SmartPeak/core/SequenceSegmentProcessors/LoadStandardsConcentrations.h>
#include <SmartPeak/core/SequenceSegmentProcessors/OptimizeCalibration.h>
#include <SmartPeak/core/SequenceSegmentProcessors/StoreFeatureBackgroundEstimations.h>
#include <SmartPeak/core/SequenceSegmentProcessors/StoreFeatureBackgroundFilters.h>
#include <SmartPeak/core/SequenceSegmentProcessors/StoreFeatureBackgroundQCs.h>
#include <SmartPeak/core/SequenceSegmentProcessors/StoreFeatureFilters.h>
#include <SmartPeak/core/SequenceSegmentProcessors/StoreFeatureQCs.h>
#include <SmartPeak/core/SequenceSegmentProcessors/StoreFeatureRSDEstimations.h>
#include <SmartPeak/core/SequenceSegmentProcessors/StoreFeatureRSDFilters.h>
#include <SmartPeak/core/SequenceSegmentProcessors/StoreFeatureRSDQCs.h>
#include <SmartPeak/core/SequenceSegmentProcessors/StoreQuantitationMethods.h>
#include <SmartPeak/core/SequenceSegmentProcessors/StoreStandardsConcentrations.h>
#include <SmartPeak/core/SequenceSegmentProcessors/TransferLOQToFeatureFilters.h>
#include <SmartPeak/core/SequenceSegmentProcessors/TransferLOQToFeatureQCs.h>
#include <SmartPeak/core/SampleGroupProcessors/MergeInjections.h>
#include <SmartPeak/core/SampleGroupProcessors/LoadFeaturesSampleGroup.h>
#include <SmartPeak/core/SampleGroupProcessors/StoreFeaturesSampleGroup.h>


#include <map>
#include <memory>

namespace SmartPeak {
  // TODO: refactor RawDataProcessor and SequenceSegmentProcess to statically declare `getName()`
  //       to avoid duplication of text while still not needing to instantiate each object
  const std::map<std::string, std::shared_ptr<RawDataProcessor>> n_to_raw_data_method_ {
    {"LOAD_RAW_DATA",                             std::make_shared<LoadRawData>()},
    {"LOAD_FEATURES",                             std::make_shared<LoadFeatures>()},
    {"LOAD_TRANSITIONS",                          std::make_shared<LoadTransitions>()},
    {"PICK_MRM_FEATURES",                         std::make_shared<PickMRMFeatures>()},
    {"FILTER_FEATURES",                           std::make_shared<FilterFeatures>()},
    {"SELECT_FEATURES",                           std::make_shared<SelectFeatures>()},
    {"VALIDATE_FEATURES",                         std::make_shared<ValidateFeatures>()},
    {"QUANTIFY_FEATURES",                         std::make_shared<QuantifyFeatures>()},
    {"CHECK_FEATURES",                            std::make_shared<CheckFeatures>()},
    {"STORE_FEATURES",                            std::make_shared<StoreFeatures>()},
    {"MAP_CHROMATOGRAMS",                         std::make_shared<MapChromatograms>()},
    {"ZERO_CHROMATOGRAM_BASELINE",                std::make_shared<ZeroChromatogramBaseline>()},
    {"EXTRACT_CHROMATOGRAM_WINDOWS",              std::make_shared<ExtractChromatogramWindows>()},
    {"FIT_FEATURES_EMG",                          std::make_shared<FitFeaturesEMG>()},
    {"FILTER_FEATURES_RSDS",                      std::make_shared<FilterFeaturesRSDs>()},
    {"CHECK_FEATURES_RSDS",                       std::make_shared<CheckFeaturesRSDs>()},
    {"FILTER_FEATURES_BACKGROUND_INTERFERENCES",  std::make_shared<FilterFeaturesBackgroundInterferences>()},
    {"CHECK_FEATURES_BACKGROUND_INTERFERENCES",   std::make_shared<CheckFeaturesBackgroundInterferences>()},
    {"EXTRACT_SPECTRA_WINDOWS",                   std::make_shared<ExtractSpectraWindows>()},
    {"EXTRACT_SPECTRA_NON_TARGETED",              std::make_shared<ExtractSpectraNonTargeted>()},
    {"MERGE_SPECTRA",                             std::make_shared<MergeSpectra>()},
    {"PICK_2D_FEATURES",                          std::make_shared<Pick2DFeatures>()},
    {"PICK_3D_FEATURES",                          std::make_shared<Pick3DFeatures>()},
    {"SEARCH_ACCURATE_MASS",                      std::make_shared<SearchAccurateMass>()},
    {"MERGE_FEATURES",                            std::make_shared<MergeFeatures>()},
    {"LOAD_ANNOTATIONS",                          std::make_shared<LoadAnnotations>()},
    {"SEARCH_SPECTRUM",                           std::make_shared<SearchSpectrum>()},
    {"CONSTRUCT_TRANSITIONS_LIST",                std::make_shared<ConstructTransitionsList>()},
    {"STORE_ANNOTATIONS",                         std::make_shared<StoreAnnotations>()},
    {"CLEAR_DATA",                                std::make_shared<ClearData>()},
    {"STORE_MSP",                                 std::make_shared<StoreMSP>()},
    {"STORE_RAW_DATA",                            std::make_shared<StoreRawData>()},
    {"CALCULATE_MDVS",                            std::make_shared<CalculateMDVs>()},
    {"ISOTOPIC_CORRECTIONS",                      std::make_shared<IsotopicCorrections>()},
    {"CALCULATE_MDV_ISOTOPIC_PURITIES",           std::make_shared<CalculateIsotopicPurities>()},
    {"CALCULATE_MDV_ACCURACIES",                  std::make_shared<CalculateMDVAccuracies>()}
  };
  const std::map<std::string, std::shared_ptr<SequenceSegmentProcessor>> n_to_seq_seg_method_ {
    {"FIT_CALIBRATION",                     std::make_shared<FitCalibration>()},
    {"OPTIMIZE_CALIBRATION",                      std::make_shared<OptimizeCalibration>()},
    {"STORE_QUANTITATION_METHODS",                std::make_shared<StoreQuantitationMethods>()},
    {"LOAD_QUANTITATION_METHODS",                 std::make_shared<LoadQuantitationMethods>()},
    {"ESTIMATE_FEATURE_FILTER_VALUES",            std::make_shared<EstimateFeatureFilterValues>()},
    {"ESTIMATE_FEATURE_QC_VALUES",                std::make_shared<EstimateFeatureQCValues>()},
    {"TRANSFER_LOQ_TO_FEATURE_FILTERS",           std::make_shared<TransferLOQToFeatureFilters>()},
    {"TRANSFER_LOQ_TO_FEATURE_QCS",               std::make_shared<TransferLOQToFeatureQCs>()},
    {"ESTIMATE_FEATURE_RSDS",                     std::make_shared<EstimateFeatureRSDs>()},
    {"ESTIMATE_FEATURE_BACKGROUND_INTERFERENCES", std::make_shared<EstimateFeatureBackgroundInterferences>()},
    {"STORE_FEATURE_FILTERS",                     std::make_shared<StoreFeatureFilters>()},
    {"LOAD_FEATURE_FILTERS",                      std::make_shared<LoadFeatureFilters>()},
    {"STORE_FEATURE_QCS",                         std::make_shared<StoreFeatureQCs>()},
    {"LOAD_FEATURE_QCS",                          std::make_shared<LoadFeatureQCs>()},
    {"STORE_FEATURE_RSD_FILTERS",                 std::make_shared<StoreFeatureRSDFilters>()},
    {"LOAD_FEATURE_RSD_FILTERS",                  std::make_shared<LoadFeatureRSDFilters>()},
    {"STORE_FEATURE_RSD_QCS",                     std::make_shared<StoreFeatureRSDQCs>()},
    {"LOAD_FEATURE_RSD_QCS",                      std::make_shared<LoadFeatureRSDQCs>()},
    {"STORE_FEATURE_BACKGROUND_FILTERS",          std::make_shared<StoreFeatureBackgroundFilters>()},
    {"LOAD_FEATURE_BACKGROUND_FILTERS",           std::make_shared<LoadFeatureBackgroundFilters>()},
    {"STORE_FEATURE_BACKGROUND_QCS",              std::make_shared<StoreFeatureBackgroundQCs>()},
    {"LOAD_FEATURE_BACKGROUND_QCS",               std::make_shared<LoadFeatureBackgroundQCs>()},
    {"STORE_FEATURE_RSD_ESTIMATIONS",             std::make_shared<StoreFeatureRSDEstimations>()},
    {"LOAD_FEATURE_RSD_ESTIMATIONS",              std::make_shared<LoadFeatureRSDEstimations>()},
    {"STORE_FEATURE_BACKGROUND_ESTIMATIONS",      std::make_shared<StoreFeatureBackgroundEstimations>()},
    {"LOAD_FEATURE_BACKGROUND_ESTIMATIONS",       std::make_shared<LoadFeatureBackgroundEstimations>()}
  };
  const std::map<std::string, std::shared_ptr<SampleGroupProcessor>> n_to_sample_group_method_ {
    {"MERGE_INJECTIONS",                          std::make_shared<MergeInjections>()},
    {"LOAD_FEATURES_SAMPLE_GROUP",                std::make_shared<LoadFeaturesSampleGroup>()},
    {"STORE_FEATURES_SAMPLE_GROUP",               std::make_shared<StoreFeaturesSampleGroup>()}
  };
}
