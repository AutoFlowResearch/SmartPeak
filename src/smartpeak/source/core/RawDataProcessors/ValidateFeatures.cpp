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

#include <SmartPeak/algorithm/MRMFeatureValidator.h>

#include <plog/Log.h>

#include <algorithm>
#include <exception>

namespace SmartPeak
{

  std::vector<std::string> ValidateFeatures::getRequirements() const
  {
    return { "sequence", "traML" };
  }

  ParameterSet ValidateFeatures::getParameterSchema() const
  {
    std::map<std::string, std::vector<std::map<std::string, std::string>>> param_struct({
    {"MRMFeatureValidator.validate_MRMFeatures", {
      {
        {"name", "Tr_window"},
        {"type", "float"},
        {"value", "1"},
        {"description", "retention time difference threshold"},
      },
    }},
    });
    return ParameterSet(param_struct);
  }

  void ValidateFeatures::process(
    RawDataHandler& rawDataHandler_IO,
    const ParameterSet& params_I,
    Filenames& filenames_I
  ) const
  {
    LOGD << "START validateFeatures";
    getFilenames(filenames_I);
    // Complete user parameters with schema
    ParameterSet params(params_I);
    params.merge(getParameterSchema());

    OpenMS::FeatureMap mapped_features;
    std::map<std::string, float> validation_metrics; // keys: accuracy, recall, precision

    MRMFeatureValidator::validate_MRMFeatures(
      rawDataHandler_IO.getReferenceData(),
      rawDataHandler_IO.getFeatureMap(),
      rawDataHandler_IO.getMetaData().getInjectionName(),
      mapped_features,
      validation_metrics,
      std::stof(params.at("MRMFeatureValidator.validate_MRMFeatures").front().getValueAsString())
      // TODO: While this probably works, it might be nice to add some check that the parameter passed is the desired one
    );

    rawDataHandler_IO.setFeatureMap(mapped_features);
    rawDataHandler_IO.setValidationMetrics(validation_metrics);

    LOGD << "END validateFeatures";
  }

}
