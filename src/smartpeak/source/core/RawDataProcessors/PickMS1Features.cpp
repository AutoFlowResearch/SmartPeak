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
#include <SmartPeak/core/RawDataProcessor.h>
#include <SmartPeak/core/Filenames.h>
#include <SmartPeak/core/Utilities.h>
#include <SmartPeak/core/FeatureFiltersUtils.h>
#include <SmartPeak/io/InputDataValidation.h>

#include <OpenMS/FILTERING/SMOOTHING/SavitzkyGolayFilter.h>
#include <OpenMS/TRANSFORMATIONS/RAW2PEAK/PeakPickerHiRes.h>
#include <OpenMS/FILTERING/NOISEESTIMATION/SignalToNoiseEstimatorMedianRapid.h>
#include <OpenMS/ANALYSIS/OPENSWATH/PeakIntegrator.h>

#include <plog/Log.h>

#include <algorithm>
#include <exception>

namespace SmartPeak
{

  std::vector<std::string> PickMS1Features::getRequirements() const
  {
    return { "sequence", "traML" };
  }

  ParameterSet PickMS1Features::getParameterSchema() const
  {
    OpenMS::SavitzkyGolayFilter sgfilter;
    OpenMS::PeakPickerHiRes picker;
    OpenMS::PeakIntegrator pi;
    ParameterSet parameters({ sgfilter, picker, pi });

    std::map<std::string, std::vector<std::map<std::string, std::string>>> param_struct({
    {"PickMS1Features", {
    {
      {"name", "frame_length"},
      {"type", "int"},
      {"value", "11"},
      {"description", "SavitzkyGolayFilter parameter. The number of subsequent data points used for smoothing"},
    },
    {
      {"name", "polynomial_order"},
      {"type", "int"},
      {"value", "4"},
      {"description", "SavitzkyGolayFilter parameter. Order or the polynomial that is fitted"},
    },
    {
      {"name", "sne:window"},
      {"type", "float"},
      {"value", "10"},
      {"description", "SignalToNoiseEstimatorMedianRapid parameter. Signal-to-noise estimation window (in mz)"},
    },
    {
      {"name", "write_convex_hull"},
      {"type", "bool"},
      {"value", "false"},
      {"description", "Whether to write out all points of all features into the featureXML"},
    },
    {
      {"name", "compute_peak_shape_metrics"},
      {"type", "bool"},
      {"value", "false"},
      {"description", "Calulates various peak shape metrics (e.g., tailing) that can be used for downstream QC/QA."},
    },
    {
      {"name", "min_intensity"},
      {"type", "float"},
      {"value", "86000"},
      {"description", "All features below the minimum intensity will be discarded"},
    }
    }} });
    ParameterSet pick_ms1_feature_params(param_struct);
    parameters.merge(pick_ms1_feature_params);
    return parameters;
  }

  void PickMS1Features::process(RawDataHandler& rawDataHandler_IO,
    const ParameterSet& params_I,
    Filenames& filenames_I
  ) const
  {
    LOGD << "START PickMS1Features";
    getFilenames(filenames_I);

    // Complete user parameters with schema
    ParameterSet params(params_I);
    params.merge(getParameterSchema());
    
    float sn_window = 0;
    bool compute_peak_shape_metrics = false;
    float min_intensity = 0;
    bool write_convex_hull = false;
    for (const auto& pms1f_params : params.at("PickMS1Features")) {
      if (pms1f_params.getName() == "sne:window") {
        try {
          sn_window = std::stof(pms1f_params.getValueAsString());
        }
        catch (const std::exception& e) {
          LOGE << e.what();
        }
      }
      if (pms1f_params.getName() == "write_convex_hull") {
        try {
          std::string value = pms1f_params.getValueAsString();
          std::transform(value.begin(), value.end(), value.begin(), ::tolower);
          write_convex_hull = (value == "true")?true:false;
        }
        catch (const std::exception& e) {
          LOGE << e.what();
        }
      }
      if (pms1f_params.getName() == "compute_peak_shape_metrics") {
        try {
          std::string value = pms1f_params.getValueAsString();
          std::transform(value.begin(), value.end(), value.begin(), ::tolower);
          compute_peak_shape_metrics = (value == "true") ? true : false;
        }
        catch (const std::exception& e) {
          LOGE << e.what();
        }
      }
      if (pms1f_params.getName() == "min_intensity") {
        try {
          min_intensity = std::stof(pms1f_params.getValueAsString());
        }
        catch (const std::exception& e) {
          LOGE << e.what();
        }
      }
    }
    if (sn_window == 0) {
      throw std::invalid_argument("Missing sne : window parameter for PickMS1Features. Not picking.");
    }
    
    OpenMS::SavitzkyGolayFilter sgfilter;
    Utilities::setUserParameters(sgfilter, params_I, "PickMS1Features");

    OpenMS::PeakPickerHiRes picker;
    Utilities::setUserParameters(picker, params_I, "PickMS1Features");

    OpenMS::PeakIntegrator pi;
    Utilities::setUserParameters(pi, params_I, "PickMS1Features");

    OpenMS::FeatureMap featureMap;
    for (const OpenMS::MSSpectrum& spec : rawDataHandler_IO.getExperiment().getSpectra()) {
      // Smooth and pick
      OpenMS::MSSpectrum input(spec);
      sgfilter.filter(input);
      std::vector<OpenMS::PeakPickerHiRes::PeakBoundary> boundaries;
      OpenMS::MSSpectrum output;
      picker.pick(input, output, boundaries);
      if (output.size() <= 0) continue;

      // Estimate the S/N
      OpenMS::SignalToNoiseEstimatorMedianRapid sne(sn_window);
      std::vector<double> mzs, intensities;
      mzs.reserve(spec.size());
      intensities.reserve(spec.size());
      for (auto it = spec.begin(); it != spec.end(); ++it)
      {
        mzs.push_back(it->getMZ());
        intensities.push_back(it->getIntensity());
      }
      if (mzs.size() <= 2) continue;
      OpenMS::SignalToNoiseEstimatorMedianRapid::NoiseEstimator e = sne.estimateNoise(mzs, intensities);

      // Create the featureMap
      int i = 0;
      for (auto it = output.begin(); it != output.end(); ++it)
      {
        // set the metadata
        OpenMS::Feature f;
        f.setUniqueId();
        f.setMZ(it->getMZ());
        f.setRT(0);
        f.setMetaValue("native_id", spec.getNativeID());
        f.setMetaValue("PeptideRef", "Unknown");
        f.setMetaValue("scan_polarity", rawDataHandler_IO.getMetaData().scan_polarity);
        f.setMetaValue("logSN", std::log(e.get_noise_value(it->getMZ())));
        f.setMetaValue("leftWidth", boundaries.at(i).mz_min);
        f.setMetaValue("rightWidth", boundaries.at(i).mz_max);

        // compute the peak area
        OpenMS::PeakIntegrator::PeakArea pa = pi.integratePeak(input, boundaries.at(i).mz_min, boundaries.at(i).mz_max);
        f.setIntensity(pa.area);
        f.setMetaValue("peak_apex_int", pa.height);
        f.setMetaValue("peak_apex_position", it->getMZ());

        // check the min intensity threshold
        if (pa.height < min_intensity) {
          //LOGD << "Feature: " << f.getUniqueId() << " with peak_apex_int=" << pa.height << " did not pass the `min_threshold`=" << min_intensity;
          ++i;
          continue;
        }

        // Calculate peak shape metrics that will be used for later QC
        OpenMS::PeakIntegrator::PeakShapeMetrics psm = pi.calculatePeakShapeMetrics(input, boundaries.at(i).mz_min, boundaries.at(i).mz_max, pa.height, it->getMZ());
        f.setMetaValue("width_at_50", psm.width_at_50);
        if (compute_peak_shape_metrics)
        {
          f.setMetaValue("width_at_5", psm.width_at_5);
          f.setMetaValue("width_at_10", psm.width_at_10);
          f.setMetaValue("start_position_at_5", psm.start_position_at_5);
          f.setMetaValue("start_position_at_10", psm.start_position_at_10);
          f.setMetaValue("start_position_at_50", psm.start_position_at_50);
          f.setMetaValue("end_position_at_5", psm.end_position_at_5);
          f.setMetaValue("end_position_at_10", psm.end_position_at_10);
          f.setMetaValue("end_position_at_50", psm.end_position_at_50);
          f.setMetaValue("total_width", psm.total_width);
          f.setMetaValue("tailing_factor", psm.tailing_factor);
          f.setMetaValue("asymmetry_factor", psm.asymmetry_factor);
          f.setMetaValue("slope_of_baseline", psm.slope_of_baseline);
          f.setMetaValue("baseline_delta_2_height", psm.baseline_delta_2_height);
          f.setMetaValue("points_across_baseline", psm.points_across_baseline);
          f.setMetaValue("points_across_half_height", psm.points_across_half_height);
        }

        // extract out the convex hull
        if (write_convex_hull) {
          OpenMS::ConvexHull2D hull;
          hull.setHullPoints(pa.hull_points);
          f.setConvexHulls({ hull });
        }
        featureMap.push_back(f);
        ++i;
      }
    }

    // NOTE: setPrimaryMSRunPath() is needed for calculate_calibration
    featureMap.setPrimaryMSRunPath({ rawDataHandler_IO.getMetaData().getFilename() });
    LOGD << "setPrimaryMSRunPath: " << rawDataHandler_IO.getMetaData().getFilename();

    rawDataHandler_IO.setFeatureMap(featureMap);
    rawDataHandler_IO.updateFeatureMapHistory();

    LOGI << "Feature Picker output size: " << featureMap.size();
    LOGD << "END PickMS1Features";
  }

}
