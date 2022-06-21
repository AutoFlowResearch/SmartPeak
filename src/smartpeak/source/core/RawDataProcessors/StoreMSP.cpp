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
#include <SmartPeak/core/RawDataProcessors/StoreMSP.h>
#include <SmartPeak/core/Filenames.h>
#include <SmartPeak/core/Utilities.h>

#include <OpenMS/ANALYSIS/OPENSWATH/TargetedSpectraExtractor.h>

#include <plog/Log.h>

#include <algorithm>
#include <exception>

namespace SmartPeak
{

  std::vector<std::string> StoreMSP::getRequirements() const
  {
    return { "sequence", "traML" };
  }

  std::set<std::string> StoreMSP::getInputs() const
  {
    return { "Experiment", "Spectra" };
  }

  std::set<std::string> StoreMSP::getOutputs() const
  {
    return { };
  }

  ParameterSet StoreMSP::getParameterSchema() const
  {
    OpenMS::TargetedSpectraExtractor oms_params;
    ParameterSet parameters({ oms_params });
    return parameters;
  }

  void StoreMSP::getFilenames(Filenames& filenames) const
  {
    filenames.addFileName("output_ms2", "${FEATURES_OUTPUT_PATH}/${OUTPUT_INJECTION_NAME}.msp");
  };

  void StoreMSP::doProcess(RawDataHandler& rawDataHandler_IO,
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

    auto output_ms2 = filenames_I.getFullPath("output_ms2").generic_string();
    targeted_spectra_extractor.storeSpectraMSP(output_ms2, rawDataHandler_IO.getChromatogramMap());
  }
}
