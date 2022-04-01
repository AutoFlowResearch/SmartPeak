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
#include <SmartPeak/core/RawDataProcessors/MatchSpectra.h>
#include <SmartPeak/core/Filenames.h>
#include <SmartPeak/core/Utilities.h>
#include <SmartPeak/core/FeatureFiltersUtils.h>

#include <OpenMS/FORMAT/MSPGenericFile.h>
#include <OpenMS/ANALYSIS/OPENSWATH/TargetedSpectraExtractor.h>

#include <plog/Log.h>

#include <algorithm>
#include <exception>

namespace SmartPeak
{

  std::vector<std::string> MatchSpectra::getRequirements() const
  {
    return { "Spectra" };
  }

  std::set<std::string> MatchSpectra::getInputs() const
  {
    return { "Features" };
  }

  std::set<std::string> MatchSpectra::getOutputs() const
  {
    return { "Feature Background Estimations" };
  }

  ParameterSet MatchSpectra::getParameterSchema() const
  {
    OpenMS::TargetedSpectraExtractor oms_params;
    ParameterSet parameters({ oms_params });
    return parameters;
  }

  void MatchSpectra::getFilenames(Filenames& filenames) const
  {
    filenames.addFileName("cmp_spectra", "${FEATURES_OUTPUT_PATH}/${OUTPUT_INJECTION_NAME}.msp");
  };

  void MatchSpectra::doProcess(RawDataHandler& rawDataHandler_IO,
    const ParameterSet& params_I,
    Filenames& filenames_I
  ) const
  {
    getFilenames(filenames_I);

    // Complete user parameters with schema
    ParameterSet params(params_I);
    params.merge(getParameterSchema());

    OpenMS::TargetedSpectraExtractor targeted_spectra_extractor;
    Utilities::setUserParameters(targeted_spectra_extractor, params);

    // Load spectra for comparison
    auto cmp_spectra_file_name = filenames_I.getFullPath("cmp_spectra").generic_string();
    OpenMS::MSPGenericFile msp_file;
    OpenMS::MSExperiment library;
    msp_file.load(cmp_spectra_file_name, library);
    library.sortSpectra();

    // Compare
    OpenMS::TargetedSpectraExtractor::BinnedSpectrumComparator cmp;
    std::map<OpenMS::String, OpenMS::DataValue> options;
    cmp.init(library.getSpectra(), options);
    targeted_spectra_extractor.targetedMatching(rawDataHandler_IO.getExperiment().getSpectra(), cmp, rawDataHandler_IO.getFeatureMap());

    rawDataHandler_IO.updateFeatureMapHistory();
  }

}
