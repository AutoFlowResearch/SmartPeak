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
#include <SmartPeak/core/ApplicationHandler.h>
#include <SmartPeak/core/FeatureFiltersUtils.h>

// load/store raw data
#include <OpenMS/FORMAT/FileTypes.h>
#include <OpenMS/FORMAT/ChromeleonFile.h>
#include <OpenMS/FORMAT/TraMLFile.h>  // load traML as well
#include <OpenMS/FORMAT/FileHandler.h>
#include <OpenMS/FORMAT/MzMLFile.h>
#include <OpenMS/FORMAT/MzTabFile.h>
#include <OpenMS/ANALYSIS/OPENSWATH/ChromatogramExtractor.h>
#include <OpenMS/ANALYSIS/OPENSWATH/TargetedSpectraExtractor.h>
#include <OpenMS/ANALYSIS/QUANTITATION/IsotopeLabelingMDVs.h>
#include <OpenMS/ANALYSIS/TARGETED/MRMMapping.h>
#include <OpenMS/KERNEL/SpectrumHelper.h>

#include <OpenMS/FORMAT/MRMFeatureQCFile.h>  // load featureFilter and featureQC
#include <OpenMS/ANALYSIS/OPENSWATH/TransitionTSVFile.h>  // load traML
#include <OpenMS/FORMAT/FeatureXMLFile.h>  // load/store featureXML
#include <SmartPeak/io/InputDataValidation.h> // check filenames and headers

// load validation data and parameters
#include <SmartPeak/io/ParametersParser.h>
#ifndef CSV_IO_NO_THREAD
#define CSV_IO_NO_THREAD
#endif
#include <SmartPeak/io/csv.h>
#include <plog/Log.h>

// feature selection
#include <OpenMS/ANALYSIS/OPENSWATH/MRMFeatureSelector.h>
#include <OpenMS/ANALYSIS/OPENSWATH/MRMBatchFeatureSelector.h>
#include <OpenMS/ANALYSIS/OPENSWATH/PeakIntegrator.h>

#include <SmartPeak/algorithm/MRMFeatureValidator.h>  // feature validaiton
#include <OpenMS/ANALYSIS/OPENSWATH/MRMFeatureFilter.h>  // feature filter/QC
#include <OpenMS/ANALYSIS/OPENSWATH/MRMFeatureFinderScoring.h>  // feature picker
#include <OpenMS/ANALYSIS/QUANTITATION/AbsoluteQuantitation.h> // feature quantification
#include <OpenMS/MATH/MISC/EmgGradientDescent.h>

// FIA-MS
#include <OpenMS/ANALYSIS/OPENSWATH/SpectrumAddition.h> // MergeSpectra
#include <OpenMS/FILTERING/SMOOTHING/SavitzkyGolayFilter.h> // PickMS1Features
#include <OpenMS/TRANSFORMATIONS/RAW2PEAK/PeakPickerHiRes.h> // PickMS1Features
#include <OpenMS/FILTERING/NOISEESTIMATION/SignalToNoiseEstimatorMedianRapid.h> // PickMS1Features
#include <OpenMS/ANALYSIS/ID/AccurateMassSearchEngine.h>

// PickMS2Features
#include <OpenMS/FILTERING/DATAREDUCTION/MassTraceDetection.h>
#include <OpenMS/FILTERING/DATAREDUCTION/ElutionPeakDetection.h>
#include <OpenMS/FILTERING/DATAREDUCTION/FeatureFindingMetabo.h>

#include <algorithm>
#include <exception>

namespace SmartPeak
{

  std::vector<std::string> FitFeaturesEMG::getRequirements() const
  {
    return { "sequence", "traML" };
  }

  ParameterSet FitFeaturesEMG::getParameterSchema() const
  {
    OpenMS::EmgGradientDescent oms_params;
    return ParameterSet({ oms_params });
  }

  void FitFeaturesEMG::process(
    RawDataHandler& rawDataHandler_IO,
    const ParameterSet& params_I,
    Filenames& filenames_I
  ) const
  {
    LOGD << "START FitFeaturesEMG";
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
      std::cout << "NEW FEATURE\n";
      const double left { feature.getMetaValue("leftWidth") };
      const double right { feature.getMetaValue("rightWidth") };
      std::cout << "Boundaries: [" << left << ", " << right << "]\n";
      std::vector<OpenMS::Feature>& subordinates { feature.getSubordinates() };
      std::cout << "n. subordinates: " << subordinates.size() << "\n";
      for (OpenMS::Feature& subfeature : subordinates) {
        std::cout << "NEW SUBFEATURE\n";
        const OpenMS::String name = subfeature.getMetaValue("native_id");
        std::cout << "Subordinate name: " << name << "\n";
        const OpenMS::MSChromatogram& chromatogram = getChromatogramByName(name);
        std::cout << "Chromatogram found!\n";
        std::vector<double> x;
        std::vector<double> y;
        extractPointsIntoVectors(chromatogram, left, right, x, y);
        std::cout << "Extracted n. points: " << x.size() << "\n";

        if (x.size() < 3) {
          std::cout << "Less than 2 points. Skipping: " << name << "\n\n";
          continue;
        }

        // EMG parameter estimation with gradient descent
        double h, mu, sigma, tau;
        std::cout << "Estimating EMG parameters...\n";
        emg.estimateEmgParameters(x, y, h, mu, sigma, tau);

        // Estimate the intensities for each point
        std::vector<double> out_xs;
        std::vector<double> out_ys;
        std::cout << "Applying estimated parameters...\n";
        emg.applyEstimatedParameters(x, h, mu, sigma, tau, out_xs, out_ys);

        // integrate area and estimate background, update the subfeature

        std::cout << "emg peak # points: " << out_xs.size() << "\n";
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
        std::cout << "Updating ranges...\n";
        emg_chrom.updateRanges();
        std::cout << "Ranges updated.\n";
        const double emg_chrom_left { emg_chrom.getMin()[0] };
        const double emg_chrom_right { emg_chrom.getMax()[0] };
        std::cout << "Positions calculated.\n";
        OpenMS::PeakIntegrator::PeakArea pa = pi.integratePeak(emg_chrom, emg_chrom_left, emg_chrom_right);
        std::cout << "Area calculated.\n";
        OpenMS::PeakIntegrator::PeakBackground pb = pi.estimateBackground(emg_chrom, emg_chrom_left, emg_chrom_right, pa.apex_pos);
        std::cout << "Background calculated.\n";
        double peak_integral { pa.area - pb.area };
        double peak_apex_int { pa.height - pb.height };
        if (peak_integral < 0) { peak_integral = 0; }
        if (peak_apex_int < 0) { peak_apex_int = 0; }

        std::cout << "Intensity: " << subfeature.getIntensity() << "\t" << peak_integral << "\n";
        std::cout << "peak_apex_position: " << subfeature.getMetaValue("peak_apex_position") << "\t" << pa.apex_pos << "\n";
        std::cout << "peak_apex_int: " << subfeature.getMetaValue("peak_apex_int") << "\t" << peak_apex_int << "\n";
        std::cout << "area_background_level: " << subfeature.getMetaValue("area_background_level") << "\t" << pb.area << "\n";
        std::cout << "noise_background_level: " << subfeature.getMetaValue("noise_background_level") << "\t" << pb.height << "\n\n";

        subfeature.setIntensity(peak_integral);
        subfeature.setMetaValue("peak_apex_position", pa.apex_pos);
        subfeature.setMetaValue("peak_apex_int", peak_apex_int);
        subfeature.setMetaValue("area_background_level", pb.area);
        subfeature.setMetaValue("noise_background_level", pb.height);
      }
    }
    rawDataHandler_IO.updateFeatureMapHistory();

    LOGD << "END FitFeaturesEMG";
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
    std::cout << "empty range: " << (it == end) << "\n";
    for (; it != end; ++it) {
      x.push_back(it->getPos());
      y.push_back(it->getIntensity());
    }
  }

}
