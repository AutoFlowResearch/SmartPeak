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

  std::vector<std::string> MergeFeatures::getRequirements() const
  {
    return { "sequence", "traML" };
  }

  void MergeFeatures::process(RawDataHandler& rawDataHandler_IO,
    const ParameterSet& params_I,
    Filenames& filenames_I
  ) const
  {
    LOGD << "START MergeFeatures";
    getFilenames(filenames_I);
    LOGI << "MergeFeatures input size: " << rawDataHandler_IO.getFeatureMap().size();

    // Pass 1: organize into a map by combining features and subordinates with the same `identifier`
    OpenMS::FeatureMap fmap;
    std::map<std::string, std::vector<OpenMS::Feature>> fmapmap;
    for (const OpenMS::Feature& f : rawDataHandler_IO.getFeatureMap()) {
      if (f.metaValueExists("identifier")) {
        auto found_f = fmapmap.emplace(f.getMetaValue("identifier").toStringList().at(0), std::vector<OpenMS::Feature>({ f }));
        if (!found_f.second) {
          fmapmap.at(f.getMetaValue("identifier").toStringList().at(0)).push_back(f);
        }
      }
      for (const OpenMS::Feature& s : f.getSubordinates()) {
        if (s.metaValueExists("identifier")) {
          auto found_s = fmapmap.emplace(s.getMetaValue("identifier").toStringList().at(0), std::vector<OpenMS::Feature>({ s }));
          if (!found_s.second) {
            fmapmap.at(s.getMetaValue("identifier").toStringList().at(0)).push_back(s);
          }
        }
      }
    }

    // Pass 2: compute the consensus manually
    for (const auto& f_map : fmapmap) {

      // compute the total intensity for weighting
      double total_intensity = 0;
      for (const auto& f : f_map.second) {
        if (f.metaValueExists("peak_apex_int")) 
          total_intensity += (double)f.getMetaValue("peak_apex_int");
        else 
          total_intensity += f.getIntensity();
      }

      // compute the weighted averages
      double rt = 0.0, m = 0.0, intensity = 0.0, peak_apex_int = 0.0;
      double weighting_factor = 1.0 / f_map.second.size(); // will be updated
      for (const auto& f : f_map.second) {
        // compute the weighting factor
        if (f.metaValueExists("peak_apex_int")) 
          weighting_factor = (double)f.getMetaValue("peak_apex_int") / total_intensity;
        else 
          weighting_factor = f.getIntensity() / total_intensity;

        // compute the weighted averages
        rt += f.getRT() * weighting_factor;
        if (f.getCharge() == 0)
          LOGW << "ConsensusFeature::computeDechargeConsensus() WARNING: Feature's charge is 0! This will lead to M=0!";
        //m += (f.getMZ() * std::abs(f.getCharge()) + (double)f.getMetaValue("dc_charge_adduct_mass")) * weighting_factor; // weighted mz
        m += f.getMZ() * weighting_factor;
        //intensity += f.getIntensity() * weighting_factor; // weighted intensity
        intensity += f.getIntensity();
        if (f.metaValueExists("peak_apex_int")) 
          //peak_apex_int += (double)f.getMetaValue("peak_apex_int") * weighting_factor; // weighted peak_apex_int
        peak_apex_int += (double)f.getMetaValue("peak_apex_int");
      }

      // make the feature map and assign subordinates
      OpenMS::Feature f;
      f.setUniqueId();
      f.setMetaValue("PeptideRef", f_map.first);
      f.setMZ(m);
      f.setRT(rt);
      f.setMetaValue("scan_polarity", f_map.second.front().getMetaValue("scan_polarity"));
      f.setIntensity(intensity);
      f.setMetaValue("peak_apex_int", peak_apex_int);
      f.setSubordinates(f_map.second);
      fmap.push_back(f);
    }
    rawDataHandler_IO.setFeatureMap(fmap);
    rawDataHandler_IO.updateFeatureMapHistory();

    LOGI << "MergeFeatures output size: " << rawDataHandler_IO.getFeatureMap().size();
    LOGD << "END MergeFeatures";
  }

}
