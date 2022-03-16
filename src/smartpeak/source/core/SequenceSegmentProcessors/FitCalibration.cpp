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
      {"description", "List of points to exclude from calibration. Each item of the list is composed of sample_name;component_name"},
    },
    {
      {"name", "included_points"},
      {"type", "list"},
      {"value", "[]"},
      {"description", "List of points to include from calibration. Each item of the list is composed of sample_name;component_name"},
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

    auto excluded_points = getIncludedExcludedPointsFromParameters(params, "excluded_points");
    auto included_points = getIncludedExcludedPointsFromParameters(params, "included_points");

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
      
      auto excluded_feature_concentrations = sequenceSegmentHandler_IO.getExcludedComponentsToConcentrations().at(component_name);
      auto feature_concentrations = sequenceSegmentHandler_IO.getComponentsToConcentrations().at(component_name);

      // Remove user excluded features
      std::vector<OpenMS::AbsoluteQuantitationStandards::featureConcentration> new_feature_concentrations;
      for (auto& feature_concentration : feature_concentrations)
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
          new_feature_concentrations.push_back(feature_concentration);
        }
      }
      feature_concentrations = std::move(new_feature_concentrations);

      // Add user included features
      std::vector<OpenMS::AbsoluteQuantitationStandards::featureConcentration> new_excluded_feature_concentrations;
      for (auto& feature_concentration : excluded_feature_concentrations)
      {
        bool included = false;
        for (auto& standard_concentration : sequenceSegmentHandler_IO.getStandardsConcentrations())
        {
          for (const auto [included_sample_name, included_component_name] : included_points)
          {
            if ((included_component_name == component_name)
              && (included_component_name == standard_concentration.component_name)
              && (included_sample_name == standard_concentration.sample_name)
              && (std::abs(feature_concentration.actual_concentration - standard_concentration.actual_concentration) < 1e-06)
              && (std::abs(feature_concentration.dilution_factor - standard_concentration.dilution_factor) < 1e-06)
              )
            {
              included = true;
              feature_concentrations.push_back(feature_concentration);
            }
          }
        }
        if (!included)
        {
          new_excluded_feature_concentrations.push_back(feature_concentration);
        }
      }
      excluded_feature_concentrations = std::move(new_excluded_feature_concentrations);

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
  FitCalibration::getIncludedExcludedPointsFromParameters(const ParameterSet& params, const std::string& param_name) const
  {
    std::vector<std::tuple<std::string, std::string>> points;
    auto param = params.findParameter("FitCalibration", param_name);
    if ((!param) || (param->getType() != "string_list"))
    {
      throw std::invalid_argument("FitCalibration parameter not found or wrong type");
    }
    CastValue param_points;
    Utilities::parseString(param->getValueAsString(), param_points);
    for (const auto& param_point_value : param_points.sl_)
    {
      auto sep_pos = param_point_value.find(";");
      if (sep_pos != std::string::npos)
      {
        auto sample_name = param_point_value.substr(0, sep_pos);
        auto component_name = param_point_value.substr(sep_pos + 1, param_point_value.size() - sep_pos);
        points.push_back(std::make_tuple(sample_name, component_name));
      }
    }
    return points;
  }
  
}
