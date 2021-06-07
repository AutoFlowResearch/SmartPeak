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
#include <map>
#include <memory>

namespace SmartPeak {
  // TODO: refactor RawDataProcessor and SequenceSegmentProcess to statically declare `getName()`
  //       to avoid duplication of text while still not needing to instantiate each object
  const std::map<std::string, std::shared_ptr<RawDataProcessor>> n_to_raw_data_method_ {
    {"LOAD_RAW_DATA",                             std::make_shared<LoadRawData>()},
    {"LOAD_FEATURES",                             std::make_shared<LoadFeatures>()},
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
    {"MERGE_SPECTRA",                             std::make_shared<MergeSpectra>()},
    {"PICK_MS1_FEATURES",                         std::make_shared<PickMS1Features>()},
    {"PICK_MS2_FEATURES",                         std::make_shared<PickMS2Features>()},
    {"SEARCH_ACCURATE_MASS",                      std::make_shared<SearchAccurateMass>()},
    {"MERGE_FEATURES",                            std::make_shared<MergeFeatures>()},
    {"LOAD_ANNOTATIONS",                          std::make_shared<LoadAnnotations>()},
    {"STORE_ANNOTATIONS",                         std::make_shared<StoreAnnotations>()},
    {"CLEAR_DATA",                                std::make_shared<ClearData>()},
    {"STORE_RAW_DATA",                            std::make_shared<StoreRawData>()},
    {"CALCULATE_MDVS",                            std::make_shared<CalculateMDVs>()},
    {"ISOTOPIC_CORRECTIONS",                      std::make_shared<IsotopicCorrections>()},
    {"CALCULATE_MDV_ISOTOPIC_PURITIES",           std::make_shared<CalculateIsotopicPurities>()},
    {"CALCULATE_MDV_ACCURACIES",                  std::make_shared<CalculateMDVAccuracies>()},
    {"SEARCH_SPECTRUM",                           std::make_shared<searchSpectrum>()}
  };
  const std::map<std::string, std::shared_ptr<SequenceSegmentProcessor>> n_to_seq_seg_method_ {
    {"CALCULATE_CALIBRATION",                     std::make_shared<CalculateCalibration>()},
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
    {"STORE_FEATURES_SAMPLE_GROUP",               std::make_shared<StoreFeaturesSampleGroup>()},
    {"SELECT_DILUTIONS",                          std::make_shared<SelectDilutions>()}
  };
}
