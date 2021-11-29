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
#include <SmartPeak/core/RawDataProcessors/DDA.h>
#include <SmartPeak/core/Filenames.h>
#include <SmartPeak/core/Utilities.h>
#include <SmartPeak/core/FeatureFiltersUtils.h>

#include <OpenMS/ANALYSIS/OPENSWATH/TargetedSpectraExtractor.h>

#include <plog/Log.h>

#include <algorithm>
#include <exception>

namespace SmartPeak
{

  std::vector<std::string> DDA::getRequirements() const
  {
    return { "sequence", "traML" };
  }

  ParameterSet DDA::getParameterSchema() const
  {
    OpenMS::TargetedSpectraExtractor oms_params;
    return ParameterSet({ oms_params });
  }

  void DDA::getFilenames(Filenames& filenames) const
  {
    filenames.addFileName("output_traML", "${FEATURES_OUTPUT_PATH}/${OUTPUT_INJECTION_NAME}.traML");
  };

  void DDA::process(RawDataHandler& rawDataHandler_IO,
    const ParameterSet& params_I,
    Filenames& filenames_I
  ) const
  {
    LOGD << "START DDA";
    getFilenames(filenames_I);

    // Complete user parameters with schema
    ParameterSet params(params_I);
    params.merge(getParameterSchema());
    std::filesystem::path main_path(filenames_I.getTagValue(Filenames::Tag::MAIN_DIR));
    Utilities::prepareFileParameterList(params, "TargetedSpectraExtractor", "AccurateMassSearchEngine:db:mapping", main_path);
    Utilities::prepareFileParameterList(params, "TargetedSpectraExtractor", "AccurateMassSearchEngine:db:struct", main_path);
    Utilities::prepareFileParameter(params, "TargetedSpectraExtractor", "AccurateMassSearchEngine:positive_adducts", main_path);
    Utilities::prepareFileParameter(params, "TargetedSpectraExtractor", "AccurateMassSearchEngine:negative_adducts", main_path);

    OpenMS::TargetedSpectraExtractor targeted_spectra_extractor;
    Utilities::setUserParameters(targeted_spectra_extractor, params);

    // merge features (will be on MS1 spectra)
    OpenMS::FeatureMap& ms1_accurate_mass_found_feature_map = rawDataHandler_IO.getFeatureMap();
    OpenMS::FeatureMap ms1_merged_features;
    targeted_spectra_extractor.mergeFeatures(ms1_accurate_mass_found_feature_map, ms1_merged_features);

    // annotateSpectra :  I would like to annotate my MS2 spectra with the likely MS1 feature that it was derived from
    std::vector<OpenMS::MSSpectrum> annotated_spectra;
    OpenMS::FeatureMap ms2_features; // will be the input of annoteSpectra
    targeted_spectra_extractor.annotateSpectra(rawDataHandler_IO.getExperiment().getSpectra(), ms1_merged_features, ms2_features, annotated_spectra);

    // pickSpectra
    std::vector<OpenMS::MSSpectrum> picked_spectra;
    for (const auto& spectrum : annotated_spectra)
    {
      OpenMS::MSSpectrum picked_spectrum;
      targeted_spectra_extractor.pickSpectrum(spectrum, picked_spectrum);
      picked_spectra.push_back(picked_spectrum);
    }

    // score and select
    std::vector<OpenMS::MSSpectrum> scored_spectra;
    targeted_spectra_extractor.scoreSpectra(annotated_spectra, picked_spectra, scored_spectra);

    std::vector<OpenMS::MSSpectrum> selected_spectra;
    OpenMS::FeatureMap selected_features;
    targeted_spectra_extractor.selectSpectra(scored_spectra, ms2_features, selected_spectra, selected_features, true);

    // searchSpectra (will be on MS2 spectra)
    OpenMS::FeatureMap ms2_accurate_mass_found_feature_map;
    targeted_spectra_extractor.searchSpectrum(selected_features, ms2_accurate_mass_found_feature_map, true);

    // merge features again (on MS2 spectra features)
    OpenMS::FeatureMap ms2_merged_features;
    targeted_spectra_extractor.mergeFeatures(ms2_accurate_mass_found_feature_map, ms2_merged_features);

    // Store - we want to store MS1 and the associated MS2 features 
    OpenMS::Param oms_params = targeted_spectra_extractor.getParameters();
    oms_params.setValue("output_format", "traML");
    oms_params.setValue("deisotoping:use_deisotoper", "true");
    targeted_spectra_extractor.setParameters(oms_params);
    LOGI << "Storing: " << filenames_I.getFullPath("output_traML").generic_string();
    targeted_spectra_extractor.storeSpectraTraML(filenames_I.getFullPath("output_traML").generic_string(), ms1_merged_features, ms2_merged_features);

    // build MS1/MS2 features
    OpenMS::FeatureMap ms1_ms2_features;
    for (const auto& ms1_feature : ms1_merged_features)
    {
      OpenMS::Feature ms_level_annotated_feature = ms1_feature;
      ms_level_annotated_feature.setMetaValue("ms_level", 1);
      ms1_ms2_features.push_back(ms_level_annotated_feature);
    }
    for (const auto& ms2_feature : ms2_merged_features)
    {
      OpenMS::Feature ms_level_annotated_feature = ms2_feature;
      ms_level_annotated_feature.setMetaValue("ms_level", 2);
      ms1_ms2_features.push_back(ms_level_annotated_feature);
    }
    rawDataHandler_IO.setFeatureMap(ms1_ms2_features);
    rawDataHandler_IO.updateFeatureMapHistory();

    LOGD << "END DDA";
  }

}
