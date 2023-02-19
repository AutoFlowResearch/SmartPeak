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
// $Maintainer: Douglas McCloskey, Ahmed Khalil $
// $Authors: Douglas McCloskey, Pasquale Domenico Colaianni $
// --------------------------------------------------------------------------
#include <SmartPeak/core/RawDataProcessors/FitFeaturesEMG.h>
#include <SmartPeak/core/Filenames.h>
#include <SmartPeak/core/Utilities.h>
#include <SmartPeak/core/FeatureFiltersUtils.h>

#include <OpenMS/MATH/MISC/EmgGradientDescent.h>
#include <OpenMS/ANALYSIS/OPENSWATH/PeakIntegrator.h>

#include <plog/Log.h>

#include <algorithm>
#include <exception>

namespace SmartPeak
{

  std::set<std::string> FitFeaturesEMG::getInputs() const
  {
    return { "Features", "Chromatogram"};
  }

  std::set<std::string> FitFeaturesEMG::getOutputs() const
  {
    return { "Features" };
  }

  std::vector<std::string> FitFeaturesEMG::getFilenameRequirements() const
  {
    return { "sequence", "traML" };
  }

  ParameterSet FitFeaturesEMG::getParameterSchema() const
  {
    OpenMS::EmgGradientDescent oms_params;
    return ParameterSet({ oms_params });
  }

  void FitFeaturesEMG::doProcess(
    RawDataHandler& rawDataHandler_IO,
    const ParameterSet& params_I,
    Filenames& filenames_I
  ) const
  {
    getFilenames(filenames_I);
    OpenMS::EmgGradientDescent emg;
    Utilities::setUserParameters(emg, params_I);

    OpenMS::FeatureMap& featureMap = rawDataHandler_IO.getFeatureMap();

    const std::vector<OpenMS::MSChromatogram>& chromatograms {
      rawDataHandler_IO.getChromatogramMap().getChromatograms() };

    auto getChromatogramByName = [&chromatograms](const OpenMS::String& name) -> const OpenMS::MSChromatogram&
    {
      const std::vector<OpenMS::MSChromatogram>::const_iterator it =
        std::find_if(chromatograms.cbegin(), chromatograms.cend(), [&name](const OpenMS::MSChromatogram& chrom){
          return name == chrom.getNativeID();
        });
      if (it == chromatograms.cend()) {
        throw std::string("Can't find a chromatogram with NativeID == ") + name;
      }
      return *it;
    };

    for (OpenMS::Feature& feature : featureMap) {
      LOGD << "NEW FEATURE";
      const double left { feature.getMetaValue("leftWidth") };
      const double right { feature.getMetaValue("rightWidth") };
      LOGD << "Boundaries: [" << left << ", " << right << "]";
      std::vector<OpenMS::Feature>& subordinates { feature.getSubordinates() };
      LOGD << "n. subordinates: " << subordinates.size();
      for (OpenMS::Feature& subfeature : subordinates) {
        LOGD << "NEW SUBFEATURE";
        const OpenMS::String name = subfeature.getMetaValue("native_id");
        LOGD << "Subordinate name: " << name;
        const OpenMS::MSChromatogram& chromatogram = getChromatogramByName(name);
        LOGD << "Chromatogram found!";
        std::vector<double> x;
        std::vector<double> y;
        extractPointsIntoVectors(chromatogram, left, right, x, y);
        LOGD << "Extracted n. points: " << x.size();

        if (x.size() < 3) {
          LOGD << "Less than 2 points. Skipping: " << name;
          continue;
        }

        // EMG parameter estimation with gradient descent
        double h, mu, sigma, tau;
        LOGD << "Estimating EMG parameters...";
        emg.estimateEmgParameters(x, y, h, mu, sigma, tau);

        // Estimate the intensities for each point
        std::vector<double> out_xs;
        std::vector<double> out_ys;
        LOGD << "Applying estimated parameters...";
        emg.applyEstimatedParameters(x, h, mu, sigma, tau, out_xs, out_ys);

        // integrate area and estimate background, update the subfeature

        LOGD << "emg peak # points: " << out_xs.size();
        OpenMS::ConvexHull2D::PointArrayType hull_points(out_xs.size());
        OpenMS::MSChromatogram emg_chrom;
        for (size_t i = 0; i < out_xs.size(); ++i) {
          emg_chrom.push_back(OpenMS::ChromatogramPeak(out_xs[i], out_ys[i]));
          hull_points[i][0] = out_xs[i];
          hull_points[i][1] = out_ys[i];
        }
        OpenMS::ConvexHull2D hull;
        hull.addPoints(hull_points);
        subfeature.getConvexHulls().push_back(hull); 

        OpenMS::PeakIntegrator pi;
        LOGD << "Updating ranges...";
        emg_chrom.updateRanges();
        LOGD << "Ranges updated.";
        const double emg_chrom_left { emg_chrom.getMinRT() };
        const double emg_chrom_right { emg_chrom.getMaxRT() };
        LOGD << "Positions calculated.";
        OpenMS::PeakIntegrator::PeakArea pa = pi.integratePeak(emg_chrom, emg_chrom_left, emg_chrom_right);
        LOGD << "Area calculated.";
        OpenMS::PeakIntegrator::PeakBackground pb = pi.estimateBackground(emg_chrom, emg_chrom_left, emg_chrom_right, pa.apex_pos);
        LOGD << "Background calculated.";
        double peak_integral { pa.area - pb.area };
        double peak_apex_int { pa.height - pb.height };
        if (peak_integral < 0) { peak_integral = 0; }
        if (peak_apex_int < 0) { peak_apex_int = 0; }

        LOGD << "Intensity: " << subfeature.getIntensity() << "\t" << peak_integral;
        LOGD << "peak_apex_position: " << subfeature.getMetaValue("peak_apex_position") << "\t" << pa.apex_pos;
        LOGD << "peak_apex_int: " << subfeature.getMetaValue("peak_apex_int") << "\t" << peak_apex_int;
        LOGD << "area_background_level: " << subfeature.getMetaValue("area_background_level") << "\t" << pb.area;
        LOGD << "noise_background_level: " << subfeature.getMetaValue("noise_background_level") << "\t" << pb.height;

        subfeature.setIntensity(peak_integral);
        subfeature.setMetaValue("peak_apex_position", pa.apex_pos);
        subfeature.setMetaValue("peak_apex_int", peak_apex_int);
        subfeature.setMetaValue("area_background_level", pb.area);
        subfeature.setMetaValue("noise_background_level", pb.height);
      }
    }
    rawDataHandler_IO.updateFeatureMapHistory();
  }

  void FitFeaturesEMG::extractPointsIntoVectors(
    const OpenMS::MSChromatogram& chromatogram,
    const double left,
    const double right,
    std::vector<double>& x,
    std::vector<double>& y
  ) const
  {
    x.clear();
    y.clear();
    OpenMS::MSChromatogram::ConstIterator it = chromatogram.PosBegin(left);
    const OpenMS::MSChromatogram::ConstIterator end = chromatogram.PosEnd(right);
    LOGD << "empty range: " << (it == end);
    for (; it != end; ++it) {
      x.push_back(it->getPos());
      y.push_back(it->getIntensity());
    }
  }

}
