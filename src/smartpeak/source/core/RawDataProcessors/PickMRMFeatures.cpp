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
#include <SmartPeak/core/RawDataProcessors/PickMRMFeatures.h>
#include <SmartPeak/core/Filenames.h>
#include <SmartPeak/core/Utilities.h>
#include <SmartPeak/core/FeatureFiltersUtils.h>
#include <SmartPeak/io/InputDataValidation.h>

#include <OpenMS/ANALYSIS/OPENSWATH/MRMFeatureFinderScoring.h>

#include <plog/Log.h>

#include <algorithm>
#include <exception>

namespace SmartPeak
{

  std::set<std::string> PickMRMFeatures::getInputs() const
  {
    return { "Chromatogram", "Targeted Experiment", "SWATH"};
  }

  std::set<std::string> PickMRMFeatures::getOutputs() const
  {
    return { "Features" };
  }

  std::vector<std::string> PickMRMFeatures::getRequirements() const
  {
    return { "sequence", "traML" };
  }

  ParameterSet PickMRMFeatures::getParameterSchema() const
  {
    OpenMS::MRMFeatureFinderScoring oms_params;
    return ParameterSet({ oms_params });
  }

  void PickMRMFeatures::process(
    RawDataHandler& rawDataHandler_IO,
    const ParameterSet& params_I,
    Filenames& filenames_I
  ) const
  {
    LOGD << "START PickMRMFeatures";
    getFilenames(filenames_I);
    OpenMS::MRMFeatureFinderScoring featureFinder;
    Utilities::setUserParameters(featureFinder, params_I);

    OpenMS::FeatureMap featureMap;

    featureFinder.pickExperiment(
      rawDataHandler_IO.getChromatogramMap(),
      featureMap,
      rawDataHandler_IO.getTargetedExperiment(),
      rawDataHandler_IO.getTransformationDescription(),
      rawDataHandler_IO.getSWATH()
    );

    // NOTE: setPrimaryMSRunPath() is needed for calculate_calibration
    featureMap.setPrimaryMSRunPath({rawDataHandler_IO.getMetaData().getFilename()});

    LOGD << "setPrimaryMSRunPath: " << rawDataHandler_IO.getMetaData().getFilename();

    rawDataHandler_IO.setFeatureMap(featureMap);
    rawDataHandler_IO.updateFeatureMapHistory();

    LOGI << "Feature Picker output size: " << featureMap.size();
    LOGD << "END PickMRMFeatures";
  }

}
