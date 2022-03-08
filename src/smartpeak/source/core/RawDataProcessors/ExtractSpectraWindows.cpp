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
#include <SmartPeak/core/RawDataProcessors/ExtractSpectraWindows.h>
#include <SmartPeak/core/Filenames.h>
#include <SmartPeak/core/Utilities.h>
#include <SmartPeak/core/FeatureFiltersUtils.h>

#include <OpenMS/KERNEL/SpectrumHelper.h>

#include <plog/Log.h>

#include <algorithm>
#include <exception>

namespace SmartPeak
{

  // Parameters used by Spetra related processors
  ParameterSet FIAMSParameters()
  {
    std::map<std::string, std::vector<std::map<std::string, std::string>>> param_struct({
    {"FIAMS", {
      {
        {"name", "acquisition_start"},
        {"type", "float"},
        {"value", "0"},
        {"description", "The start time to use when extracting out the spectra windows from the MSExperiment"},
        {"min", "0"}
      },
      {
        {"name", "acquisition_end"},
        {"type", "float"},
        {"value", "30"},
        {"description", "The end time to use when extracting out the spectra windows from the MSExperiment"},
        {"min", "0"}
      },
      {
        {"name", "resolution"},
        {"type", "float"},
        {"value", "12000"},
        {"description", "The instrument settings: resolution"}
      },
      {
        {"name", "max_mz"},
        {"type", "float"},
        {"value", "1500"},
        {"description", "Maximum mz"}
      },
      {
        {"name", "bin_step"},
        {"type", "float"},
        {"value", "20"},
        {"description", "The size of the step to recalculated the bin size used for adding up spectra along the time axis"}
      }
    }} });
    return ParameterSet(param_struct);
  }

  std::set<std::string> ExtractSpectraWindows::getInputs() const
  {
    return { "Spectra" };
  }

  std::set<std::string> ExtractSpectraWindows::getOutputs() const
  {
    return { "Spectra" };
  }

  std::vector<std::string> ExtractSpectraWindows::getRequirements() const
  {
    return { "sequence", "traML" };
  }

  ParameterSet ExtractSpectraWindows::getParameterSchema() const
  {
    return FIAMSParameters();
  }

  void ExtractSpectraWindows::process(
    RawDataHandler& rawDataHandler_IO,
    const ParameterSet& params_I,
    Filenames& filenames_I
  ) const
  {
    LOGD << "START ExtractSpectraWindows";
    getFilenames(filenames_I);
    // Complete user parameters with schema
    ParameterSet params(params_I);
    params.merge(getParameterSchema());

    float start = 0, stop = 0;
    if (params.count("FIAMS") && params.at("FIAMS").size())
    {
      for (const auto& fia_params: params.at("FIAMS")) {
        if (fia_params.getName() == "acquisition_start") 
        {
          try {
            start = std::stof(fia_params.getValueAsString());
          }
          catch (const std::exception& e) {
            LOGE << e.what();
          }
        }
        if (fia_params.getName() == "acquisition_end") 
        {
          try {
            stop = std::stof(fia_params.getValueAsString());
          }
          catch (const std::exception& e) {
            LOGE << e.what();
          }
        }
      }
    }

    if (stop == 0) {
      throw std::invalid_argument("No parameters passed to ExtractSpectraWindows.  Spectra will not be extracted.");
    }

    std::vector<OpenMS::MSSpectrum> output;
    for (OpenMS::MSSpectrum& spec : rawDataHandler_IO.getExperiment().getSpectra()) {
      if (spec.getRT() >= start && spec.getRT() <= stop) {
        output.push_back(spec);
      }
    }

    if (output.empty()) {
      LOGW << "No spectra was extracted.  Check that the specified start and stop retention times in the parameters are compatible with the acquired spectra.";
    }
    rawDataHandler_IO.getExperiment().setSpectra(output);

    LOGD << "END ExtractSpectraWindows";
  }

}
