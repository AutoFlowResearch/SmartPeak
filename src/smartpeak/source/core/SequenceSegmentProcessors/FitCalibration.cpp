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

#include <SmartPeak/core/SequenceSegmentProcessors/FitCalibration.h>
#include <SmartPeak/core/Filenames.h>
#include <SmartPeak/core/MetaDataHandler.h>
#include <SmartPeak/core/SampleType.h>
#include <SmartPeak/core/SequenceHandler.h>
#include <SmartPeak/core/Utilities.h>
#include <SmartPeak/core/ApplicationHandler.h>
#include <SmartPeak/core/FeatureFiltersUtils.h>
#include <SmartPeak/io/InputDataValidation.h>

#include <OpenMS/ANALYSIS/QUANTITATION/AbsoluteQuantitation.h>

#include <plog/Log.h>

namespace SmartPeak
{
  std::set<std::string> FitCalibration::getInputs() const
  {
    return { "Features", "Standards Concentrations", "Quantitation Methods" };
  }

  std::set<std::string> FitCalibration::getOutputs() const
  {
    return { "Quantitation Methods" };
  }

  std::vector<std::string> FitCalibration::getRequirements() const
  {
    return { "sequence", "traML" };
  }

  ParameterSet FitCalibration::getParameterSchema() const
  {
    std::map<std::string, std::vector<std::map<std::string, std::string>>> param_struct({
    {"FitCalibration", {
    {
      {"name", "excluded_points"},
      {"type", "list"},
      {"value", "[]"},
      {"description", "List of point to exclude from calibration. Each item of the list is composed of sample_name;component_name"},
    },
    {
      {"name", "component_name"},
      {"type", "string"},
      {"value", ""},
      {"description", "Component name to which apply Fit Calibration"},
    }
    }} });
    ParameterSet fit_calibration_params(param_struct);
    return fit_calibration_params;
  }

  void FitCalibration::process(
    SequenceSegmentHandler& sequenceSegmentHandler_IO,
    const SequenceHandler& sequenceHandler_I,
    const ParameterSet& params_I,
    Filenames& filenames_I
  ) const
  {
    LOGD << "START FitCalibration";
    getFilenames(filenames_I);

    // Complete user parameters with schema
    ParameterSet params(params_I);
    params.merge(getParameterSchema());

    std::vector<std::tuple<std::string, std::string>> excluded_points = getExcludedPointsFromParameters(params);

    // add in the method parameters
    OpenMS::AbsoluteQuantitation absoluteQuantitation;
    Utilities::setUserParameters(absoluteQuantitation, params_I);

    auto component_name_param = params.findParameter("FitCalibration", "component_name");
    if (!component_name_param)
    {
      throw std::invalid_argument("component_name argument is empty.");
    }
    std::string component_name = component_name_param->getValueAsString();

    absoluteQuantitation.setQuantMethods(sequenceSegmentHandler_IO.getQuantitationMethods());
    auto excluded_components_to_concentrations = sequenceSegmentHandler_IO.getExcludedComponentsToConcentrations();
    for (OpenMS::AbsoluteQuantitationMethod& row : sequenceSegmentHandler_IO.getQuantitationMethods())
    {
      if (row.getComponentName() != component_name)
      {
        continue;
      }
      
      std::vector<OpenMS::AbsoluteQuantitationStandards::featureConcentration> excluded_feature_concentrations;
      auto feature_concentrations_without_excluded = sequenceSegmentHandler_IO.getComponentsToConcentrations().at(component_name);

      std::vector<OpenMS::AbsoluteQuantitationStandards::featureConcentration> feature_concentrations;
      for (auto& feature_concentration : feature_concentrations_without_excluded)
      {
        bool excluded = false;
        for (auto& standard_concentration : sequenceSegmentHandler_IO.getStandardsConcentrations())
        {
          for (const auto [excluded_sample_name, excluded_component_name] : excluded_points)
          {
            if ( (excluded_component_name == component_name)
              && (excluded_component_name == standard_concentration.component_name)
              && (excluded_sample_name == standard_concentration.sample_name)
              && (std::abs(feature_concentration.actual_concentration - standard_concentration.actual_concentration) < 1e-06)
              && (std::abs(feature_concentration.dilution_factor - standard_concentration.dilution_factor) < 1e-06)
              )
            {
              excluded = true;
              excluded_feature_concentrations.push_back(feature_concentration);
            }
          }
        }
        if (!excluded)
        {
          feature_concentrations.push_back(feature_concentration);
        }
      }

      if (!feature_concentrations.empty())
      {
        auto optimized_params = row.getTransformationModelParams();
        optimized_params = absoluteQuantitation.fitCalibration(
          feature_concentrations,
          row.getFeatureName(),
          row.getTransformationModel(),
          optimized_params);

        // calculate the R2 and bias
        std::vector<double> biases; // not needed (method parameters)
        double correlation_coefficient = 0.0; // not needed (method parameters)
        absoluteQuantitation.calculateBiasAndR(
          feature_concentrations,
          row.getFeatureName(),
          row.getTransformationModel(),
          optimized_params,
          biases,
          correlation_coefficient);

        row.setTransformationModelParams(optimized_params);
        row.setCorrelationCoefficient(correlation_coefficient);
        row.setNPoints(feature_concentrations.size());
      }
      excluded_components_to_concentrations.erase(row.getComponentName());
      excluded_components_to_concentrations.insert({ row.getComponentName(), excluded_feature_concentrations });
    }
    // store results
    sequenceSegmentHandler_IO.setExcludedComponentsToConcentrations(excluded_components_to_concentrations);
    LOGD << "END FitCalibration";
  }

  std::vector<std::tuple<std::string, std::string>>
  FitCalibration::getExcludedPointsFromParameters(const ParameterSet& params) const
  {
    std::vector<std::tuple<std::string, std::string>> excluded_points;
    auto excluded_points_param = params.findParameter("FitCalibration", "excluded_points");
    if ((!excluded_points_param) || (excluded_points_param->getType() != "string_list"))
    {
      throw std::invalid_argument("FitCalibration::excluded_points not found or wrong type");
    }
    CastValue excluded_points_value;
    Utilities::parseString(excluded_points_param->getValueAsString(), excluded_points_value);
    for (const std::string excluded_point_value : excluded_points_value.sl_)
    {
      auto sep_pos = excluded_point_value.find(";");
      if (sep_pos != std::string::npos)
      {
        auto sample_name = excluded_point_value.substr(0, sep_pos);
        auto component_name = excluded_point_value.substr(sep_pos + 1, excluded_point_value.size() - sep_pos);
        excluded_points.push_back(std::make_tuple(sample_name, component_name));
      }
    }
    return excluded_points;
  }
  
}
