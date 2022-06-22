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
#include <SmartPeak/core/RawDataProcessors/CalculateMDVs.h>
#include <SmartPeak/core/Filenames.h>
#include <SmartPeak/core/Utilities.h>
#include <SmartPeak/core/FeatureFiltersUtils.h>

#include <OpenMS/ANALYSIS/QUANTITATION/IsotopeLabelingMDVs.h>

#include <plog/Log.h>

#include <algorithm>
#include <exception>

namespace SmartPeak
{

  std::set<std::string> CalculateMDVs::getInputs() const
  {
    return { "Features" };
  }

  std::set<std::string> CalculateMDVs::getOutputs() const
  {
    return { "Features" };
  }

  std::vector<std::string> CalculateMDVs::getFilenameRequirements() const
  {
    return { "sequence", "traML" };
  }

  ParameterSet CalculateMDVs::getParameterSchema() const
  {
    std::map<std::string, std::vector<std::map<std::string, std::string>>> param_struct({
    {"CalculateMDVs", {
      {
        {"name", "mass_intensity_type"},
        {"type", "string"},
        {"value", "norm_sum"},
        {"description", "Type of intensity"},
        {"valid_strings", "['norm_sum','norm_max']"}
      },
      {
        {"name", "feature_name"},
        {"type", "string"},
        {"value", "intensity"},
        {"description", "The name of the FeatureMap attribute to use. Examples include peak_apex_int, peak_area, and intensity."},
      }
    }} });
    return ParameterSet(param_struct);
  }

  void CalculateMDVs::doProcess(
    RawDataHandler& rawDataHandler_IO,
    const ParameterSet& params_I,
    Filenames& filenames_I
  ) const
  {
    getFilenames(filenames_I);

    // Complete user parameters with schema
    ParameterSet params(params_I);
    params.merge(getParameterSchema());

    OpenMS::IsotopeLabelingMDVs isotopelabelingmdvs;
    OpenMS::Param parameters = isotopelabelingmdvs.getParameters();
    isotopelabelingmdvs.setParameters(parameters);

    OpenMS::FeatureMap normalized_featureMap;
    auto& CalculateMDVs_params = params.at("CalculateMDVs");
      
    std::string feature_name;
    OpenMS::IsotopeLabelingMDVs::MassIntensityType mass_intensity_type;
    for (auto& param : CalculateMDVs_params)
    {
      if (param.getName() == "mass_intensity_type")
      {
        if (param.getValueAsString() == "norm_sum")
        {
          mass_intensity_type = OpenMS::IsotopeLabelingMDVs::MassIntensityType::NORM_SUM;
        }
        else if (param.getValueAsString() == "norm_max")
        {
          mass_intensity_type = OpenMS::IsotopeLabelingMDVs::MassIntensityType::NORM_MAX;
        }
      }
      else if (param.getName() == "feature_name")
      {
        feature_name = param.getValueAsString();
      }
    }
      
    isotopelabelingmdvs.calculateMDVs(rawDataHandler_IO.getFeatureMap(), normalized_featureMap, mass_intensity_type, feature_name);
    rawDataHandler_IO.setFeatureMap(normalized_featureMap);
    rawDataHandler_IO.updateFeatureMapHistory();
  }

}
