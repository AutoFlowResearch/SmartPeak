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

  std::vector<std::string> SelectFeatures::getRequirements() const
  {
    return { "sequence", "traML" };
  }

  void SelectFeatures::process(
    RawDataHandler& rawDataHandler_IO,
    const ParameterSet& params_I,
    Filenames& filenames_I
  ) const
  {
    LOGD << "START selectFeatures";
    getFilenames(filenames_I);
    LOGI << "selectFeatures input size: " << rawDataHandler_IO.getFeatureMap().size();

    const bool qmip_params_passed_but_empty =
      params_I.count("MRMFeatureSelector.schedule_MRMFeatures_qmip") &&
      params_I.at("MRMFeatureSelector.schedule_MRMFeatures_qmip").empty();
    
    const bool score_params_passed_but_empty =
      params_I.count("MRMFeatureSelector.schedule_MRMFeatures_score") &&
      params_I.at("MRMFeatureSelector.schedule_MRMFeatures_score").empty();

    if (qmip_params_passed_but_empty || score_params_passed_but_empty) {
      throw std::invalid_argument("Missing parameters");
    }

    OpenMS::FeatureMap output;

    if (params_I.count("MRMFeatureSelector.schedule_MRMFeatures_qmip")) {
      LOGD << "Using MRMFeatures_qmip";
      std::vector<OpenMS::MRMFeatureSelector::SelectorParameters> p =
        Utilities::extractSelectorParameters(params_I.at("MRMFeatureSelector.schedule_MRMFeatures_qmip"), params_I.at("MRMFeatureSelector.select_MRMFeatures_qmip"));
      OpenMS::MRMBatchFeatureSelector::batchMRMFeaturesQMIP(rawDataHandler_IO.getFeatureMap(), output, p);
    }
    else if (params_I.count("MRMFeatureSelector.schedule_MRMFeatures_score")) 
    {
      LOGD << "Using MRMFeatures_score";
      std::vector<OpenMS::MRMFeatureSelector::SelectorParameters> p =
        Utilities::extractSelectorParameters(params_I.at("MRMFeatureSelector.schedule_MRMFeatures_score"), params_I.at("MRMFeatureSelector.select_MRMFeatures_score"));
      OpenMS::MRMBatchFeatureSelector::batchMRMFeaturesScore(rawDataHandler_IO.getFeatureMap(), output, p);
    }
    else 
    {
      LOGE << "Both arguments 'select params' and 'schedule params' are empty";
      LOGD << "END selectFeatures";
      // TODO: replace throw with return?
      throw std::invalid_argument("Both arguments 'select params' and 'schedule params' are empty.");
    }

    output.setPrimaryMSRunPath({rawDataHandler_IO.getMetaData().getFilename()});

    LOGV << "setPrimaryMSRunPath: " << rawDataHandler_IO.getMetaData().getFilename();

    rawDataHandler_IO.setFeatureMap(output);
    rawDataHandler_IO.updateFeatureMapHistory();

    LOGI << "selectFeatures output size: " << output.size();
    LOGD << "END selectFeatures";
  }

}
