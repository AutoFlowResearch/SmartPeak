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
#include <SmartPeak/core/RawDataProcessors/PickMS2Features.h>
#include <SmartPeak/core/Filenames.h>
#include <SmartPeak/core/Utilities.h>
#include <SmartPeak/core/FeatureFiltersUtils.h>
#include <SmartPeak/io/InputDataValidation.h>

#include <OpenMS/FILTERING/DATAREDUCTION/MassTraceDetection.h>
#include <OpenMS/FILTERING/DATAREDUCTION/ElutionPeakDetection.h>
#include <OpenMS/FILTERING/DATAREDUCTION/FeatureFindingMetabo.h>

#include <plog/Log.h>

#include <algorithm>
#include <exception>

namespace SmartPeak
{

  std::vector<std::string> PickMS2Features::getRequirements() const
  {
    return { "sequence", "traML" };
  }

  ParameterSet PickMS2Features::getParameterSchema() const
  {
    OpenMS::MassTraceDetection mass_trace_detection;
    OpenMS::ElutionPeakDetection elution_peak_detection;
    OpenMS::FeatureFindingMetabo feature_finding_metabo;
    ParameterSet parameters({ mass_trace_detection, elution_peak_detection, feature_finding_metabo });
    std::map<std::string, std::vector<std::map<std::string, std::string>>> param_struct({
    {"PickMS2Features", {
    {
      {"name", "enable_elution"},
      {"type", "bool"},
      {"value", "true"},
      {"description", "Enable elution peak detection"},
    },
    {
      {"name", "force_processing"},
      {"type", "bool"},
      {"value", "false"},
      {"description", "To enforce processing of the data when Profile data is provided but spectra is not centroided"},
    },
    {
      {"name", "max_traces"},
      {"type", "int"},
      {"value", "0"},
      {"description", "Max traces to use for MassTraceDetection. 0 means all traces."},
    }
    }} });
    ParameterSet pick_ms2_feature_params(param_struct);
    parameters.merge(pick_ms2_feature_params);
    return parameters;
  }

  void PickMS2Features::process(
    RawDataHandler& rawDataHandler_IO,
    const ParameterSet& params_I,
    Filenames& filenames_I
  ) const
  {
    LOGD << "START PickMS2Features";
    getFilenames(filenames_I);

    // Complete user parameters with schema
    ParameterSet params(params_I);
    params.merge(getParameterSchema());

    //-------------------------------------------------------------
    // set parameters
    //-------------------------------------------------------------
    FunctionParameters pick_ms2_feature_params = params.at("PickMS2Features");

    OpenMS::MassTraceDetection mtdet;
    Utilities::setUserParameters(mtdet, params_I);

    OpenMS::ElutionPeakDetection epdet;
    Utilities::setUserParameters(epdet, params_I);

    OpenMS::FeatureFindingMetabo ffmet;
    Utilities::setUserParameters(ffmet, params_I);
    auto ffmet_parameters = ffmet.getParameters();

    //-------------------------------------------------------------
    // Setting input
    //-------------------------------------------------------------
    OpenMS::PeakMap ms_peakmap;
    for (OpenMS::MSSpectrum& spec : rawDataHandler_IO.getExperiment().getSpectra()) {
      // we want only MS1 specrtra
      if (spec.getMSLevel() == 1)
      {
        ms_peakmap.addSpectrum(spec);
      }
    }
    if (ms_peakmap.empty())
    {
      throw std::invalid_argument("The given file does not contain any conventional peak data, but might"
                                  " contain chromatograms. This tool currently cannot handle them, sorry.");
    }

    // determine type of spectral data (profile or centroided)
    OpenMS::SpectrumSettings::SpectrumType spectrum_type = ms_peakmap[0].getType();

    if (spectrum_type == OpenMS::SpectrumSettings::PROFILE)
    {
      Parameter* force_processing = pick_ms2_feature_params.findParameter("force_processing");
      if (!force_processing || force_processing->getValueAsString() == "false")
      {
        throw std::invalid_argument("Error: Profile data provided but centroided spectra expected. To enforce processing of the data set the force_processing parameter.");
      }
    }

    // make sure the spectra are sorted by m/z
    ms_peakmap.sortSpectra(true);

    std::vector<OpenMS::MassTrace> m_traces;

    //-------------------------------------------------------------
    // configure and run mass trace detection
    //-------------------------------------------------------------

    int max_traces = 0;
    Parameter* max_traces_param = pick_ms2_feature_params.findParameter("max_traces");
    if (max_traces_param)
    {
      try {
        max_traces = std::stoi(max_traces_param->getValueAsString());
      }
      catch (const std::exception& e)
      {
        LOGE << e.what();
        // keep it to 0
      }
    }

    mtdet.run(ms_peakmap, m_traces, max_traces);

    //-------------------------------------------------------------
    // configure and run elution peak detection
    //-------------------------------------------------------------
    std::vector<OpenMS::MassTrace> m_traces_final;
    Parameter* enable_elution = pick_ms2_feature_params.findParameter("enable_elution");
    if (enable_elution && enable_elution->getValueAsString() == "true")
    {
      std::vector<OpenMS::MassTrace> splitted_mtraces;
      // fill mass traces with smoothed data as well .. bad design..
      epdet.detectPeaks(m_traces, splitted_mtraces);
      if (epdet.getParameters().getValue("width_filtering") == "auto")
      {
        m_traces_final.clear();
        epdet.filterByPeakWidth(splitted_mtraces, m_traces_final);
      }
      else
      {
        m_traces_final = splitted_mtraces;
      }
    }
    else // no elution peak detection
    {
      m_traces_final = m_traces;    
      for (auto & trace: m_traces_final) // estimate FWHM, so .getIntensity() can be called later
      {
        trace.estimateFWHM(false);
      }
      if (ffmet_parameters.getValue("use_smoothed_intensities").toBool())
      {
        LOGW << "Without EPD, smoothing is not supported. Setting 'use_smoothed_intensities' to false!";
        ffmet_parameters.setValue("use_smoothed_intensities", "false");
      }
    }

    //-------------------------------------------------------------
    // configure and run feature finding
    //-------------------------------------------------------------

    OpenMS::FeatureMap feat_map;
    std::vector< std::vector< OpenMS::MSChromatogram > > feat_chromatograms;
    ffmet.run(m_traces_final, feat_map, feat_chromatograms);

    size_t trace_count(0);
    for (const auto& feat : feat_map)
    {
      if (!feat.metaValueExists("num_of_masstraces"))
      {
        throw std::invalid_argument("MetaValue 'num_of_masstraces' missing from FFMetabo output!");
      }
      trace_count += (size_t)feat.getMetaValue("num_of_masstraces");
    }
    if (trace_count != m_traces_final.size())
    {
      if (!ffmet_parameters.getValue("remove_single_traces").toBool())
      {
        throw std::invalid_argument("FF-Metabo: Internal error. Not all mass traces have been assembled to features! Aborting.");
      }
      else
      {
        LOGI << "FF-Metabo: " << (m_traces_final.size() - trace_count) << " unassembled traces have been removed.";
      }
    }

    LOGI << "-- FF-Metabo stats --\n"
      << "Input traces:    " << m_traces_final.size() << "\n"
      << "Output features: " << feat_map.size() << " (total trace count: " << trace_count << ")";

    // merge chromatograms: convert vector of vector of chromatograms to vector of chromatograms;
    std::vector<OpenMS::Chromatogram> merged_chromatograms;
    for (auto& chromatogram_list : feat_chromatograms)
    {
      OpenMS::Chromatogram c = chromatogram_list[0];
      for (auto chromatogram_it = chromatogram_list.begin() + 1; chromatogram_it != chromatogram_list.end(); ++chromatogram_it)
      {
        for (OpenMS::ChromatogramPeak& peak : (*chromatogram_it))
        {
          c.push_back(peak);
        }
      }
      merged_chromatograms.push_back(c);
    }

    // filter features with zero intensity (this can happen if the FWHM is zero (bc of overly skewed shape) and no peaks end up being summed up)
    std::vector<bool> to_filter;
    auto intensity_zero = [&](OpenMS::Feature& f) { return f.getIntensity() == 0; };
    auto remove_it = remove_if(feat_map.begin(), feat_map.end(), intensity_zero);
    // clear corresponding chromatograms as well
    for (auto feat_map_it = remove_it; feat_map_it != feat_map.end(); ++feat_map_it)
    {
      auto chromatogram_id_match = [&](OpenMS::Chromatogram& c) {
        // extract feature_id from chromatogram native_id which is feature_id underscore index
        auto chromatogram_id = c.getNativeID();
        auto pos = chromatogram_id.find("_");
        if (pos != std::string::npos)
        {
          chromatogram_id = chromatogram_id.substr(0, pos);
        }
        return (chromatogram_id == OpenMS::String((*feat_map_it).getUniqueId()));
      };
      merged_chromatograms.erase(remove_if(merged_chromatograms.begin(), merged_chromatograms.end(), chromatogram_id_match), merged_chromatograms.end());
    }
    feat_map.erase(remove_it, feat_map.end());

    // store ionization mode of spectra (useful for post-processing by AccurateMassSearch tool)
    if (!feat_map.empty())
    {
      std::set<OpenMS::IonSource::Polarity> pols;
      for (const auto &peakmap: ms_peakmap)
      {
        pols.insert(peakmap.getInstrumentSettings().getPolarity());
      }
      // concat to single string
      OpenMS::StringList sl_pols;
      for (const auto& pol : pols)
      {
        sl_pols.push_back(OpenMS::String(OpenMS::IonSource::NamesOfPolarity[pol]));
      }
      feat_map[0].setMetaValue("scan_polarity", OpenMS::ListUtils::concatenate(sl_pols, ";"));
    }

    feat_map.setPrimaryMSRunPath({ rawDataHandler_IO.getMetaData().getFilename() });
    LOGD << "setPrimaryMSRunPath: " << rawDataHandler_IO.getMetaData().getFilename();

    rawDataHandler_IO.setFeatureMap(feat_map);
    rawDataHandler_IO.updateFeatureMapHistory();
    rawDataHandler_IO.getExperiment().setChromatograms(merged_chromatograms);

    LOGD << "END PickMS2Features";
  }

}
