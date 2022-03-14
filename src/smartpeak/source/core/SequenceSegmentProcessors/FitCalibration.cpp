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
    OpenMS::AbsoluteQuantitation oms_params;
    ParameterSet parameters({ oms_params });

    std::map<std::string, std::vector<std::map<std::string, std::string>>> param_struct({
    {"FitCalibration", {
    {
      {"name", "excluded_points"},
      {"type", "list"},
      {"value", "[]"},
      {"description", "List of point to exclude from calibration. Each item of the list is composed of sample_name;component_name"},
    }
    }} });
    ParameterSet fit_calibration_params(param_struct);
    parameters.merge(fit_calibration_params);
    return parameters;
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
      
    // get all standards
    std::vector<size_t> all_standards_indices;
    this->getSampleIndicesBySampleType(
      sequenceSegmentHandler_IO,
      sequenceHandler_I,
      SampleType::Standard,
      all_standards_indices
    );

    // check if there are any standards to calculate the calibrators from
    if (all_standards_indices.empty()) {
      throw std::invalid_argument("standards_indices argument is empty.");
    }

    // add in the method parameters
    OpenMS::AbsoluteQuantitation absoluteQuantitation;
    Utilities::setUserParameters(absoluteQuantitation, params_I);

    absoluteQuantitation.setQuantMethods(sequenceSegmentHandler_IO.getQuantitationMethods());
    std::map<std::string, std::vector<OpenMS::AbsoluteQuantitationStandards::featureConcentration>> components_to_concentrations;
    std::map<std::string, std::vector<OpenMS::AbsoluteQuantitationStandards::featureConcentration>> outlier_components_to_concentrations;
    std::map<std::string, std::vector<OpenMS::AbsoluteQuantitationStandards::featureConcentration>> excluded_components_to_concentrations;
    for (const OpenMS::AbsoluteQuantitationMethod& row : sequenceSegmentHandler_IO.getQuantitationMethods())
    {
      std::vector<size_t> standards_indices = all_standards_indices;

      // construct excluded points
      std::vector<size_t> excluded_standards_indices;
      for (const auto [sample_name, component] : excluded_points)
      {
        if (component == row.getComponentName())
        {
          for (const size_t index : sequenceSegmentHandler_IO.getSampleIndices())
          {
            if (sequenceHandler_I.getSequence().at(index).getMetaData().getSampleName() == sample_name)
            {
              standards_indices.erase(std::remove(standards_indices.begin(), standards_indices.end(), index), standards_indices.end());
              excluded_standards_indices.push_back(index);
            }
          }
        }
      }

      std::vector<OpenMS::FeatureMap> standards_featureMaps;
      for (const size_t index : standards_indices) {
        standards_featureMaps.push_back(sequenceHandler_I.getSequence().at(index).getRawData().getFeatureMap());
      }

      std::vector<OpenMS::FeatureMap> excluded_standards_featureMaps;
      for (const size_t index : excluded_standards_indices) {
        excluded_standards_featureMaps.push_back(sequenceHandler_I.getSequence().at(index).getRawData().getFeatureMap());
      }

      // map standards to features
      OpenMS::AbsoluteQuantitationStandards absoluteQuantitationStandards;
      std::vector<OpenMS::AbsoluteQuantitationStandards::featureConcentration> feature_concentrations;

      absoluteQuantitationStandards.getComponentFeatureConcentrations(
        sequenceSegmentHandler_IO.getStandardsConcentrations(),
        standards_featureMaps,
        row.getComponentName(),
        feature_concentrations
      );

      std::vector<OpenMS::AbsoluteQuantitationStandards::featureConcentration> excluded_feature_concentrations;
      absoluteQuantitationStandards.getComponentFeatureConcentrations(
        sequenceSegmentHandler_IO.getStandardsConcentrations(),
        excluded_standards_featureMaps,
        row.getComponentName(),
        excluded_feature_concentrations
      );

      auto feature_concentrations_pruned = sequenceSegmentHandler_IO.getFeatureConcentrationsPruned(feature_concentrations);

      // remove components without any points
      if (feature_concentrations_pruned.empty()) {
        continue;
      }

      // Keep a copy to compute outer points
      auto all_feature_concentrations = feature_concentrations_pruned;

      try
      {
        absoluteQuantitation.optimizeSingleCalibrationCurve(
          row.getComponentName(),
          feature_concentrations_pruned
        );
      }
      catch (OpenMS::Exception::DivisionByZero&)
      {
        LOGW << "Warning: '" << row.getComponentName() << "' cannot be analysed - division by zero\n";
        continue;
      }
      catch (...)
      {
        LOGW << "Warning: '" << row.getComponentName() << "' cannot be analysed.\n";
        continue;
      }

      // Compute outer points
      std::vector<OpenMS::AbsoluteQuantitationStandards::featureConcentration> outlier_feature_concentrations;

      // Calibration not found: all the points will stay untouched by absoluteQuantitation.optimizeSingleCalibrationCurve
      // but we would like to have them in the outlier point list instead.
      bool optimize_single_calibration_curve_succeed = false;
      for (const auto& result_quant_method : absoluteQuantitation.getQuantMethods())
      {
        if (result_quant_method.getComponentName() == row.getComponentName())
        {
          optimize_single_calibration_curve_succeed = (result_quant_method.getNPoints() > 0);
          break;
        }
      }
      if (!optimize_single_calibration_curve_succeed)
      {
        outlier_feature_concentrations = feature_concentrations_pruned;
        feature_concentrations_pruned.clear();
      }
      else // Calibration curve has succeed
      {
        for (const auto& feature : all_feature_concentrations)
        {
          bool found = false;
          for (const auto& feature_pruned : feature_concentrations_pruned)
          {
            if ((feature.IS_feature == feature_pruned.IS_feature)
              && (std::abs(feature.actual_concentration - feature_pruned.actual_concentration) < 1e-9)
              && (std::abs(feature.IS_actual_concentration - feature_pruned.IS_actual_concentration) < 1e-9)
              && (std::abs(feature.dilution_factor - feature_pruned.dilution_factor) < 1e-9))
            {
              found = true;
              break;
            }
          }
          if (!found)
          {
            outlier_feature_concentrations.push_back(feature);
          }
        }
      }
      components_to_concentrations.erase(row.getComponentName());
      components_to_concentrations.insert({ row.getComponentName(), feature_concentrations_pruned });
      outlier_components_to_concentrations.erase(row.getComponentName());
      outlier_components_to_concentrations.insert({ row.getComponentName(), outlier_feature_concentrations });
      excluded_components_to_concentrations.erase(row.getComponentName());
      excluded_components_to_concentrations.insert({ row.getComponentName(), excluded_feature_concentrations });
    }
    // store results
    sequenceSegmentHandler_IO.setComponentsToConcentrations(components_to_concentrations);
    sequenceSegmentHandler_IO.setOutlierComponentsToConcentrations(outlier_components_to_concentrations);
    sequenceSegmentHandler_IO.setExcludedComponentsToConcentrations(excluded_components_to_concentrations);
    sequenceSegmentHandler_IO.getQuantitationMethods() = absoluteQuantitation.getQuantMethods();
    //sequenceSegmentHandler_IO.setQuantitationMethods(absoluteQuantitation.getQuantMethods());
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
