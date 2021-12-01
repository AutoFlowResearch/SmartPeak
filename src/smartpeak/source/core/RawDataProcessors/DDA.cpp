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
#include <OpenMS/ANALYSIS/OPENSWATH/TransitionTSVFile.h>
#include <OpenMS/FORMAT/TraMLFile.h>

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
    ParameterSet parameters({ oms_params });
    return parameters;
  }

  void DDA::getFilenames(Filenames& filenames) const
  {
    filenames.addFileName("output_transitions_traML", "${FEATURES_OUTPUT_PATH}/${OUTPUT_INJECTION_NAME}.traML");
    filenames.addFileName("output_transitions_csv", "${FEATURES_OUTPUT_PATH}/${OUTPUT_INJECTION_NAME}.csv");
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
    rawDataHandler_IO.setFeatureMap("ms1_merged_features", ms1_merged_features);

    // extractSpectra
    std::vector<OpenMS::MSSpectrum> annotated_spectra;
    OpenMS::FeatureMap selected_features;
    targeted_spectra_extractor.extractSpectra(rawDataHandler_IO.getExperiment(), ms1_merged_features, annotated_spectra, selected_features, true);

    rawDataHandler_IO.setFeatureMap(selected_features);
    rawDataHandler_IO.updateFeatureMapHistory();

    LOGD << "END DDA";
  }

}
