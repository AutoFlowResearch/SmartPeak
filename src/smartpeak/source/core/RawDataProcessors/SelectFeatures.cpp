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
#include <SmartPeak/core/RawDataProcessors/SelectFeatures.h>
#include <SmartPeak/core/Filenames.h>
#include <SmartPeak/core/Utilities.h>
#include <SmartPeak/core/FeatureFiltersUtils.h>
#include <SmartPeak/io/InputDataValidation.h>

#include <OpenMS/ANALYSIS/OPENSWATH/MRMBatchFeatureSelector.h>

#include <plog/Log.h>

#include <algorithm>
#include <exception>

namespace SmartPeak
{

  std::set<std::string> SelectFeatures::getInputs() const
  {
    return { "Features" };
  }

  std::set<std::string> SelectFeatures::getOutputs() const
  {
    return { "Features" };
  }

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
