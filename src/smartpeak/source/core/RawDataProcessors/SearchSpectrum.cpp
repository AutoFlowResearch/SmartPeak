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
#include <SmartPeak/core/RawDataProcessors/SearchSpectrum.h>
#include <SmartPeak/core/Filenames.h>
#include <SmartPeak/core/Utilities.h>
#include <SmartPeak/core/FeatureFiltersUtils.h>
#include <SmartPeak/io/InputDataValidation.h>

#include <OpenMS/ANALYSIS/OPENSWATH/TargetedSpectraExtractor.h>

#include <plog/Log.h>

#include <algorithm>
#include <exception>

namespace SmartPeak
{

  std::set<std::string> SearchSpectrum::getInputs() const
  {
    return { "Features" };
  }

  std::set<std::string> SearchSpectrum::getOutputs() const
  {
    return { "Features" };
  }

  std::vector<std::string> SearchSpectrum::getRequirements() const
  {
    return { "sequence", "traML" };
  }

  ParameterSet SearchSpectrum::getParameterSchema() const
  {
    OpenMS::TargetedSpectraExtractor oms_params;
    return ParameterSet({ oms_params });
  }
  void SearchSpectrum::doProcess(RawDataHandler& rawDataHandler_IO,
    const ParameterSet& params_I,
    Filenames& filenames_I
  ) const
  {
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

    OpenMS::FeatureMap feat_map_output;
    targeted_spectra_extractor.searchSpectrum(rawDataHandler_IO.getFeatureMap(), feat_map_output);
    rawDataHandler_IO.setFeatureMap(feat_map_output);
  }

}
