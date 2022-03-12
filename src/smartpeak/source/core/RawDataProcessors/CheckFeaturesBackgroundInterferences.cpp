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
#include <SmartPeak/core/RawDataProcessors/CheckFeaturesBackgroundInterferences.h>
#include <SmartPeak/core/Filenames.h>
#include <SmartPeak/core/Utilities.h>
#include <SmartPeak/core/FeatureFiltersUtils.h>

#include <OpenMS/ANALYSIS/OPENSWATH/MRMFeatureFilter.h>

#include <plog/Log.h>

#include <algorithm>
#include <exception>

namespace SmartPeak
{

  std::set<std::string> CheckFeaturesBackgroundInterferences::getInputs() const
  {
    return { "Feature Background Estimations", "Features Background QCs" };
  }

  std::set<std::string> CheckFeaturesBackgroundInterferences::getOutputs() const
  {
    return { "Features" };
  }

  std::vector<std::string> CheckFeaturesBackgroundInterferences::getRequirements() const
  {
    return { "sequence", "traML" };
  }

  ParameterSet CheckFeaturesBackgroundInterferences::getParameterSchema() const
  {
    OpenMS::MRMFeatureFilter oms_params;
    return ParameterSet({ oms_params });
  }

  void CheckFeaturesBackgroundInterferences::process(
    RawDataHandler& rawDataHandler_IO,
    const ParameterSet& params_I,
    Filenames& filenames_I
  ) const
  {
    LOGD << "START checkFeaturesBackgroundInterferences";
    getFilenames(filenames_I);
    LOGI << "Feature Checker input size: " << rawDataHandler_IO.getFeatureMap().size();

    OpenMS::MRMFeatureFilter featureFilter;
    Utilities::setUserParameters(featureFilter, params_I, "MRMFeatureFilter.filter_MRMFeaturesBackgroundInterferences.qc");

    featureFilter.FilterFeatureMapBackgroundInterference(
      rawDataHandler_IO.getFeatureMap(),
      rawDataHandler_IO.getFeatureBackgroundQC(),
      rawDataHandler_IO.getFeatureBackgroundEstimations()
    );

    rawDataHandler_IO.updateFeatureMapHistory();

    LOGI << "Feature Checker output size: " << rawDataHandler_IO.getFeatureMap().size();
    LOGD << "END checkFeaturesBackgroundInterferences";
  }

}
