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
#include <SmartPeak/core/RawDataProcessors/MergeSpectra.h>
#include <SmartPeak/core/Filenames.h>
#include <SmartPeak/core/Utilities.h>
#include <SmartPeak/core/FeatureFiltersUtils.h>
#include <SmartPeak/io/InputDataValidation.h>

#include <OpenMS/ANALYSIS/OPENSWATH/SpectrumAddition.h>

#include <plog/Log.h>

#include <algorithm>
#include <exception>

namespace SmartPeak
{

  // Parameters used by Spetra related processors
  ParameterSet FIAMSParameters2()
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

  std::vector<std::string> MergeSpectra::getRequirements() const
  {
    return { "sequence", "traML" };
  }

  ParameterSet MergeSpectra::getParameterSchema() const
  {
    return FIAMSParameters2();
  }

  void MergeSpectra::process(
    RawDataHandler& rawDataHandler_IO,
    const ParameterSet& params_I,
    Filenames& filenames_I
  ) const
  {
    LOGD << "START MergeSpectra";
    getFilenames(filenames_I);

    // Complete user parameters with schema
    ParameterSet params(params_I);
    params.merge(getParameterSchema());

    float resolution = 0, max_mz = 0, bin_step = 0;
    if (params.count("FIAMS") && params.at("FIAMS").size()) {
      for (const auto& fia_params : params.at("FIAMS")) {
        if (fia_params.getName() == "max_mz") {
          try {
            max_mz = std::stof(fia_params.getValueAsString());
          }
          catch (const std::exception& e) {
            LOGE << e.what();
          }
        }
        if (fia_params.getName() == "bin_step") {
          try {
            bin_step = std::stof(fia_params.getValueAsString());
          }
          catch (const std::exception& e) {
            LOGE << e.what();
          }
        }
        if (fia_params.getName() == "resolution") {
          try {
            resolution = std::stof(fia_params.getValueAsString());
          }
          catch (const std::exception& e) {
            LOGE << e.what();
          }
        }
      }
    }

    if (resolution == 0 || max_mz == 0 || bin_step == 0) {
      throw std::invalid_argument("Missing parameters");
    }

    // calculate the bin sizes and mass buckets
    int n_bins = max_mz / bin_step;
    std::vector<float> mzs;
    std::vector<float> bin_sizes;
    std::vector<std::vector<OpenMS::MSSpectrum>> binned_spectrum;
    mzs.reserve(n_bins);
    bin_sizes.reserve(n_bins);
    binned_spectrum.resize(n_bins);
    for (int i = 0; i < n_bins; i++) {
      mzs.push_back((i + 1) * bin_step);
      bin_sizes.push_back(mzs.at(i) / (resolution * 4.0));
      binned_spectrum.at(i).resize(rawDataHandler_IO.getExperiment().getSpectra().size());
    }

    // Divide the spectra into mass ranges
    for (int s = 0; s < rawDataHandler_IO.getExperiment().getSpectra().size(); ++s) {
      const OpenMS::MSSpectrum spectrum = rawDataHandler_IO.getExperiment().getSpectra().at(s);
      for (auto it = spectrum.begin(); it != spectrum.end(); ++it) {
        for (int i = 0; i < mzs.size() - 1; ++i) {
          if (it->getMZ() >= mzs.at(i) && it->getMZ() < mzs.at(i + 1)) {
            binned_spectrum.at(i).at(s).push_back(*it);
          }
        }
      }
    }

    // Merge spectra along time for each of the different mass ranges
    OpenMS::MSSpectrum output;
    for (int i = 0; i < mzs.size() - 1; ++i) {
      OpenMS::MSSpectrum full_spectrum = OpenMS::SpectrumAddition::addUpSpectra(
        binned_spectrum.at(i), bin_sizes.at(i), false
      );
      for (auto it = full_spectrum.begin(); it != full_spectrum.end(); ++it) {
        output.push_back(*it);
      }
    }
    output.sortByPosition();

    // Update the metavalue and members
    output.setNativeID("MergeSpectra");
    OpenMS::Peak1D::CoordinateType lowest_observed = 0.0;
    OpenMS::Peak1D::CoordinateType highest_observed = 0.0;
    const auto& spectra = rawDataHandler_IO.getExperiment().getSpectra();
    if (spectra.size())
    {
      output.setMSLevel(spectra.front().getMSLevel());
      output.setType(spectra.front().getType());
      for (auto it_spectra = spectra.begin(); it_spectra != spectra.end(); ++it_spectra)
      {
        if (!(*it_spectra).empty())
        {
          lowest_observed = (*it_spectra).front().getMZ();
          break;
        }
      }
      for (auto it_spectra = spectra.rbegin(); it_spectra != spectra.rend(); ++it_spectra)
      {
        if (!(*it_spectra).empty())
        {
          highest_observed = (*it_spectra).back().getMZ();
          break;
        }
      }
    }
    output.setMetaValue("lowest observed m/z", lowest_observed);
    output.setMetaValue("highest observed m/z", highest_observed);
    output.setMetaValue("base peak m/z", 0.0);
    output.setMetaValue("base peak intensity", 0.0);
    output.setMetaValue("total ion current", 0.0);
    rawDataHandler_IO.getExperiment().setSpectra({ output });

    LOGD << "END MergeSpectra";
  }

}
