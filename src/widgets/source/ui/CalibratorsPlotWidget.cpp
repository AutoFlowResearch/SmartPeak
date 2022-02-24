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
// $Maintainer: Bertrand Boudaud, Douglas McCloskey $
// $Authors: Bertrand Boudaud $
// --------------------------------------------------------------------------

#include <SmartPeak/ui/CalibratorsPlotWidget.h>
#include <SmartPeak/core/SequenceSegmentProcessors/CalculateCalibration.h>
#include <SmartPeak/core/SequenceSegmentProcessors/OptimizeCalibration.h>
#include <SmartPeak/core/Utilities.h>
#include <implot.h>

namespace SmartPeak
{

  void CalibratorsPlotWidget::recomputeCalibration()
  {
    if (selected_action_ == 0)
    {
      CalculateCalibration calculate_calibration;
      Filenames filenames; // calculate_calibration actually does not use it
      ParameterSet& user_parameters = sequence_handler_.getSequence().at(0).getRawData().getParameters();
      calculate_calibration.process(
        sequence_handler_.getSequenceSegments().at(selected_sequence_segment_),
        sequence_handler_,
        user_parameters,
        filenames);
      onSequenceUpdated();
    }
    else if (selected_action_ == 1)
    {
      OptimizeCalibration optimize_calibration;
      Filenames filenames; // optimize_calibration actually does not use it
      ParameterSet& user_parameters = sequence_handler_.getSequence().at(0).getRawData().getParameters();
      optimize_calibration.process(
        sequence_handler_.getSequenceSegments().at(selected_sequence_segment_),
        sequence_handler_,
        user_parameters,
        filenames);
      onSequenceUpdated();
    }
  }

  std::tuple<std::string, std::string> 
  CalibratorsPlotWidget::getSampleNameAndSerieFromSelectedPoint(
    const std::optional<std::tuple<int, int>>& matching_point,
    const std::optional<std::tuple<int, int>>& outlier_point,
    const std::optional<std::tuple<int, int>>& excluded_point
  ) const
  {
    if (matching_point)
    {
      const auto [serie, index] = *matching_point;
      return std::make_tuple(calibration_data_.matching_points_.injections_[serie][index], calibration_data_.series_names[serie]);
    }
    else if (outlier_point)
    {
      const auto [serie, index] = *outlier_point;
      return std::make_tuple(calibration_data_.outlier_points_.injections_[serie][index], calibration_data_.series_names[serie]);
    }
    else if (excluded_point)
    {
      const auto [serie, index] = *excluded_point;
      return std::make_tuple(calibration_data_.excluded_points_.injections_[serie][index], calibration_data_.series_names[serie]);
    }
    return std::make_tuple("","");
  }

  OpenMS::AbsoluteQuantitationMethod* CalibratorsPlotWidget::getQuantitationMethod(const std::string& component_name)
  {
    auto& sequence_segments = sequence_handler_.getSequenceSegments();
    if (selected_sequence_segment_ >= sequence_segments_.size())
    {
      return nullptr;
    }
    auto& sequence_segment = sequence_segments.at(selected_sequence_segment_);
    for (auto& quantitation_method : sequence_segment.getQuantitationMethods())
    {
      if (quantitation_method.getComponentName() == component_name)
      {
        return &quantitation_method;
      }
    }
    return nullptr;
  }

  void CalibratorsPlotWidget::addParameterRow(std::shared_ptr<Parameter> param, bool editable)
  {
    ImGui::TableNextRow();
    ImGui::TableSetColumnIndex(0);
    ImGui::Text(param->getName().c_str());
    ImGui::TableSetColumnIndex(1);
    auto value_as_string = param->getValueAsString();
    if (value_as_string.empty())
    {
      ImGui::Text("<empty>");
    }
    else
    {
      ImGui::Text("%s", value_as_string.c_str());
    }
    if (editable && ImGui::IsItemHovered())
    {
      ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
    }
    if (editable && ImGui::IsItemClicked())
    {
      param_to_edit_ = param;
    }
  }

  std::shared_ptr<Parameter> CalibratorsPlotWidget::CalibratorParameterToSmartPeakParameter(const OpenMS::Param::ParamEntry& param)
  {
    auto result = std::make_shared<Parameter>(param);
    if (param.name == "symmetric_regression")
    {
      // this parameter is a string type, but accepts FALSE/TRUE which is a little bit annoying.
      const auto valid_strings = std::make_shared<std::vector<CastValue>>();
      valid_strings->push_back("FALSE");
      valid_strings->push_back("TRUE");
      result->setConstraintsList(valid_strings);
    }
    else if (param.name == "y_weight")
    {
      const auto valid_strings = std::make_shared<std::vector<CastValue>>();
      valid_strings->push_back("1/y");
      valid_strings->push_back("1/y2");
      valid_strings->push_back("ln(y)");
      valid_strings->push_back("no weight");
      result->setConstraintsList(valid_strings);
    }
    else if (param.name == "x_weight")
    {
      const auto valid_strings = std::make_shared<std::vector<CastValue>>();
      valid_strings->push_back("1/x");
      valid_strings->push_back("1/x2");
      valid_strings->push_back("ln(x)");
      valid_strings->push_back("no weight");
      result->setConstraintsList(valid_strings);
    }
    return result;
  }

  void CalibratorsPlotWidget::displayParameters()
  {
    if (component_cstr_.empty())
    {
      return;
    }

    if (sequence_segments_cstr_.empty())
    {
      return;
    }

    static const std::vector<std::string> output_parameters =
    {
      "llod",
      "ulod",
      "lloq",
      "uloq",
      "correlation_coefficient",
      "n_points",
      "intercept",
      "slope"
    };

    ImGui::Begin("Calibrator Parameters");

    ImGui::Combo("Action", &selected_action_, &actions_cstr_[0], actions_cstr_.size());
    ImGui::Separator();

    parameter_editor_widget_.draw();

    ImGui::Combo("Component", &selected_component_, &component_cstr_[0], component_cstr_.size());
    ImGui::Combo("Seq. Segment", &selected_sequence_segment_, &sequence_segments_cstr_[0], sequence_segments_cstr_.size());

    auto quantitation_methods = getQuantitationMethod(calibration_data_.series_names[selected_component_]);

    ImGuiTableFlags table_flags = ImGuiTableFlags_None;
    if (ImGui::BeginTable("Calibrator Parameters Table", 2, table_flags))
    {
      // Edit Parameter widget needs to be open outside the table construction.
      std::string parameter_to_edit_function;

      ImGui::TableNextRow();
      ImGui::TableSetColumnIndex(0);
      ImGui::Text("IS name");
      ImGui::TableSetColumnIndex(1);
      ImGui::Text(quantitation_methods->getISName().c_str());

      ImGui::TableNextRow();
      ImGui::TableSetColumnIndex(0);
      ImGui::Text("Feature name");
      ImGui::TableSetColumnIndex(1);
      ImGui::Text(quantitation_methods->getFeatureName().c_str());

      ImGui::EndTable();
    }

    ImGui::Separator();
    ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetStyle().Colors[ImGuiCol_TextDisabled]);
    ImGui::Text("Input parameters");
    ImGui::PopStyleColor();
    {
      if (ImGui::BeginTable("Calibrator input parameters", 2, table_flags))
      {
        // CalculateCalibration params
        if (selected_action_ == 0)
        {
          CalculateCalibration calculate_calibration;
          auto calculate_calibration_params_schema = calculate_calibration.getParameterSchema();
          auto user_params = sequence_handler_.getSequence().at(0).getRawData().getParameters();
          calculate_calibration_params_schema.setAsSchema(true);
          user_params.merge(calculate_calibration_params_schema);
          for (auto& calculate_calibration_fct : calculate_calibration_params_schema)
          {
            for (auto& calculate_calibration_param : calculate_calibration_fct.second)
            {
              auto user_param = user_params.findParameter(calculate_calibration_fct.first, calculate_calibration_param.getName());
              if (user_param)
              {
                addParameterRow(std::make_shared<Parameter>(*user_param), true);
              }
            }
          }
        }

        // Transformation model Params (inputs)
        const auto& params = quantitation_methods->getTransformationModelParams();
        for (const auto& param : params)
        {
          if (std::find(output_parameters.begin(), output_parameters.end(), param.name) == output_parameters.end())
          {
            addParameterRow(CalibratorParameterToSmartPeakParameter(param), true);
          }
        }

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("transformation model");
        ImGui::TableSetColumnIndex(1);
        ImGui::Text(quantitation_methods->getTransformationModel().c_str());

        ImGui::EndTable();
      }
    }

    ImGui::Separator();
    ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetStyle().Colors[ImGuiCol_TextDisabled]);
    ImGui::Text("Output parameters");
    ImGui::PopStyleColor();
    {
      if (ImGui::BeginTable("Calibrator output parameters", 2, table_flags))
      {
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("llod");
        ImGui::TableSetColumnIndex(1);
        ImGui::Text("%f", quantitation_methods->getLLOD());

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("ulod");
        ImGui::TableSetColumnIndex(1);
        ImGui::Text("%f", quantitation_methods->getULOD());

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("lloq");
        ImGui::TableSetColumnIndex(1);
        ImGui::Text("%f", quantitation_methods->getLLOQ());

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("uloq");
        ImGui::TableSetColumnIndex(1);
        ImGui::Text("%f", quantitation_methods->getULOQ());

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("correlation coefficient");
        ImGui::TableSetColumnIndex(1);
        ImGui::Text("%f", quantitation_methods->getCorrelationCoefficient());

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("nb points");
        ImGui::TableSetColumnIndex(1);
        ImGui::Text("%d", quantitation_methods->getNPoints());

        // Transformation model Params (outputs)
        const auto& params = quantitation_methods->getTransformationModelParams();
        for (const auto& param : params)
        {
          if (std::find(output_parameters.begin(), output_parameters.end(), param.name) != output_parameters.end())
          {
            addParameterRow(std::make_shared<Parameter>(param), false);
          }
        }
        ImGui::EndTable();
      }
    }

    if (param_to_edit_)
    {
      parameter_editor_widget_.setParameter(param_to_edit_->getName(), (*param_to_edit_));
      ImGui::OpenPopup("Edit Parameter");
    }
    ImGui::End();
  }

  void CalibratorsPlotWidget::onParameterSet(const std::string& function_parameter, const Parameter& parameter)
  {
    // find back the parameter and set it
    
    // try to find in the quantitation methods parameters
    auto quantitation_methods = getQuantitationMethod(calibration_data_.series_names[selected_component_]);
    auto& params = quantitation_methods->getTransformationModelParams();
    OpenMS::Param new_params; // params iterator returns const parameters. so we need to reconstruct and set the whole list
    for (auto & param_entry : params)
    {
      OpenMS::Param::ParamEntry new_param_entry = param_entry;
      if (param_entry.name == parameter.getName())
      {
        CastValue c;
        auto value_as_string = parameter.getValueAsString();
        // special case for x_weight and y_weight
        if (value_as_string == "no weight")
        {
          value_as_string = "";
        }
        Utilities::parseString(value_as_string, c);
        // try some cast
        if ((c.getTag() == CastValue::Type::INT) && (parameter.getType() == std::string("float")))
        {
          new_param_entry.value = static_cast<float>(c.i_);
        }
        else
        {
          new_param_entry.value = value_as_string;
        }
        LOGD << "Set " << new_param_entry.name << " value to " << new_param_entry.value;
      }
      new_params.setValue(new_param_entry.name, new_param_entry.value);
    }
    quantitation_methods->setTransformationModelParams(new_params);

    // try to find in the user parameters
    ParameterSet& user_parameters = sequence_handler_.getSequence().at(0).getRawData().getParameters();
    for (auto& user_param_functions : user_parameters)
    {
      for (auto& user_param : user_param_functions.second)
      {
        if (user_param.getName() == parameter.getName())
        {
          user_param.setValueFromString(parameter.getValueAsString(), false);
        }
      }
    }

    recomputeCalibration();
  }

  void CalibratorsPlotWidget::plotPoints(bool show_flag, const SessionHandler::CalibrationData::Points& points, int marker_style)
  {
    if (show_flag)
    {
      for (int i = 0; i < points.concentrations_.size(); ++i) {
        assert(points.concentrations_.at(i).size() == points.features_.at(i).size());
        ImPlot::PushStyleVar(ImPlotStyleVar_Marker, marker_style);
        ImPlot::PlotScatter((calibration_data_.series_names.at(i)).c_str(),
          points.concentrations_.at(i).data(),
          points.features_.at(i).data(),
          points.concentrations_.at(i).size());
        ImPlot::PopStyleVar();
      }
    }
  }
    
  void CalibratorsPlotWidget::plotSelectedPoint(
    const std::optional<std::tuple<int, int>>& hovered_point,
    const SessionHandler::CalibrationData::Points& points,
    int marker_style)
  {
    if (hovered_point)
    {
      auto [i, j] = *hovered_point;
      ImPlot::PushStyleColor(0, ImVec4(ImColor(255, 255, 255)));
      ImPlot::PushStyleVar(ImPlotStyleVar_Marker, marker_style);
      ImPlot::PlotScatter("", // do not appear in legend
        points.concentrations_.at(i).data() + j,
        points.features_.at(i).data() + j,
        1);
      ImPlot::PopStyleColor();
      ImPlot::PopStyleVar();
    }
  }

  void CalibratorsPlotWidget::displayPlot()
  {
    ImGui::Begin("Calibrator Plot");
    ImGui::Checkbox("Legend", &show_legend_);
    ImGui::SameLine();
    ImGui::Checkbox("Fit line", &show_fit_line_);
    ImGui::SameLine();
    ImGui::Checkbox("Points", &show_matching_points_);
    ImGui::SameLine();
    ImGui::Checkbox("Outlier", &show_outlier_points_);
    ImGui::SameLine();
    ImGui::Checkbox("Excluded", &show_excluded_points_);
    ImGui::SameLine();
    if (ImGui::Button("Fit Zoom"))
    {
      reset_zoom_ = true;
    }

    // Main graphic
    ImGuiCond cond;
    if (reset_zoom_)
    {
      cond = ImGuiCond_Always;
      reset_zoom_ = false;
    }
    else
    {
      cond = ImGuiCond_Once;
    }
    // add margin around the plot
    float x_margin = (calibration_data_.conc_max - calibration_data_.conc_min) * 0.05;
    float y_margin = (calibration_data_.feature_max - calibration_data_.feature_min) * 0.05;
    ImPlot::SetNextPlotLimits(calibration_data_.conc_min - x_margin,
                              calibration_data_.conc_max + x_margin,
                              calibration_data_.feature_min - y_margin,
                              calibration_data_.feature_max + y_margin,
                              cond);
    auto window_size = ImGui::GetWindowSize();
    ImPlotFlags plotFlags = show_legend_ ? ImPlotFlags_Default | ImPlotFlags_Legend : ImPlotFlags_Default & ~ImPlotFlags_Legend;
    plotFlags |= ImPlotFlags_Crosshairs;
    if (ImPlot::BeginPlot(plot_title_.c_str(), 
                          calibration_data_.x_axis_title.c_str(), 
                          calibration_data_.y_axis_title.c_str(), 
                          ImVec2(window_size.x - 25, window_size.y - 58),
                          plotFlags)) {
      if (show_fit_line_)
      {
        for (int i = 0; i < calibration_data_.conc_fit_data.size(); ++i) {
          assert(calibration_data_.conc_fit_data.at(i).size() == calibration_data_.feature_fit_data.at(i).size());
          ImPlot::PushStyleVar(ImPlotStyleVar_LineWeight, ImPlot::GetStyle().LineWeight);
          ImPlot::PushStyleVar(ImPlotStyleVar_Marker, ImPlotMarker_None);
          ImPlot::PlotLine((calibration_data_.series_names.at(i)).c_str(), 
                            calibration_data_.conc_fit_data.at(i).data(), 
                            calibration_data_.feature_fit_data.at(i).data(),
                            calibration_data_.conc_fit_data.at(i).size());
          ImPlot::PopStyleVar();
          ImPlot::PopStyleVar();
        }
      }

      plotPoints(show_matching_points_, calibration_data_.matching_points_, ImPlotMarker_Circle);
      plotPoints(show_outlier_points_, calibration_data_.outlier_points_, ImPlotMarker_Square);
      plotPoints(show_excluded_points_, calibration_data_.excluded_points_, ImPlotMarker_Cross);

      plotSelectedPoint(hovered_matching_point_, calibration_data_.matching_points_, ImPlotMarker_Circle);
      plotSelectedPoint(hovered_outlier_point_, calibration_data_.outlier_points_, ImPlotMarker_Square);
      plotSelectedPoint(hovered_excluded_point_, calibration_data_.excluded_points_, ImPlotMarker_Cross);

      plotSelectedPoint(clicked_matching_point_, calibration_data_.matching_points_, ImPlotMarker_Circle);
      plotSelectedPoint(clicked_outlier_point_, calibration_data_.outlier_points_, ImPlotMarker_Square);
      plotSelectedPoint(clicked_excluded_point_, calibration_data_.excluded_points_, ImPlotMarker_Cross);

      // legend hover management
      int i = 0;
      for (const auto& serie_name : calibration_data_.series_names)
      {
        if (ImPlot::IsLegendEntryHovered(serie_name.c_str()))
        {
          selected_component_ = i;
        }
        ++i;
      }

      // Compute hovered points and tooltips
      bool open_context_menu = false;
      hovered_matching_point_ = std::nullopt;
      hovered_outlier_point_ = std::nullopt;
      if (ImPlot::IsPlotHovered())
      {
        auto mouse_plot_point = ImPlot::GetPlotMousePos();
        ImVec2 mouse_point(mouse_plot_point.x, mouse_plot_point.y);
        // Pixels zone around which the point will be considered hovered
        auto zero_plot_point = ImPlot::PixelsToPlot(ImVec2(0, 0));
        auto threshold_plot_point = ImPlot::PixelsToPlot(ImVec2(5, 5));
        ImVec2 threshold_point(threshold_plot_point.x - zero_plot_point.x, zero_plot_point.y - threshold_plot_point.y);
        getSelectedPoint(mouse_point, threshold_point);
        if (hovered_outlier_point_ || hovered_matching_point_ || hovered_excluded_point_)
        {
          if (ImGui::IsMouseClicked(1))
          {
            clicked_matching_point_ = hovered_matching_point_;
            clicked_outlier_point_ = hovered_outlier_point_;
            clicked_excluded_point_ = hovered_excluded_point_;
            open_context_menu = true;
          }
          else if (!ImGui::IsPopupOpen("Point Actions")) // just hovered
          {
            ImGui::BeginTooltip();
            auto [sample_name, serie_name] = getSampleNameAndSerieFromSelectedPoint(hovered_matching_point_, hovered_outlier_point_, hovered_excluded_point_);
            ImGui::Text("%s", sample_name.c_str());
            ImGui::Text("%s", serie_name.c_str());
            OpenMS::AbsoluteQuantitationStandards::featureConcentration* feature_concentration = nullptr;
            OpenMS::AbsoluteQuantitationMethod* quantitation_methods = nullptr;
            if (hovered_matching_point_)
            {
              const auto [serie, index] = *hovered_matching_point_;
              feature_concentration = &calibration_data_.matching_points_.feature_concentrations_[serie][index];
              quantitation_methods = getQuantitationMethod(calibration_data_.series_names[serie]);
            }
            else if (hovered_outlier_point_)
            {
              const auto [serie, index] = *hovered_outlier_point_;
              feature_concentration = &calibration_data_.outlier_points_.feature_concentrations_[serie][index];
              quantitation_methods = getQuantitationMethod(calibration_data_.series_names[serie]);
            }
            else if (hovered_excluded_point_)
            {
              const auto [serie, index] = *hovered_excluded_point_;
              feature_concentration = &calibration_data_.excluded_points_.feature_concentrations_[serie][index];
              quantitation_methods = getQuantitationMethod(calibration_data_.series_names[serie]);
            }
            if (feature_concentration && quantitation_methods)
            {
              ImGui::Text("actual concentration: %f (%s)", feature_concentration->actual_concentration, feature_concentration->concentration_units.c_str());
              ImGui::Text("dilution factor: %f", feature_concentration->dilution_factor);
              ImGui::Text("IS name: %s", quantitation_methods->getISName().c_str());
              ImGui::Text("IS actual concentration: %f (%s)", feature_concentration->IS_actual_concentration, feature_concentration->concentration_units.c_str());
              const auto& feature_name = quantitation_methods->getFeatureName();
              if (feature_name == "intensity")
              {
                ImGui::Text("Intensity: %f", feature_concentration->feature.getIntensity());
                ImGui::Text("IS Intensity: %f", feature_concentration->IS_feature.getIntensity());
              }
              else
              {
                if (feature_concentration->feature.metaValueExists(feature_name))
                {
                  ImGui::Text("feature %s: %f", 
                    feature_name.c_str(), 
                    static_cast<double>(feature_concentration->feature.getMetaValue(feature_name)));
                }
                else
                {
                  ImGui::Text("feature %s: Metadata does not exists!", feature_name.c_str());
                }
                if (feature_concentration->IS_feature.metaValueExists(feature_name))
                {
                  ImGui::Text("IS feature %s: %f", 
                    feature_name.c_str(), 
                    static_cast<double>(feature_concentration->IS_feature.getMetaValue(feature_name)));
                }
                else
                {
                  ImGui::Text("IS feature %s: Metadata does not exists!", feature_name.c_str());
                }
              }
            }
            ImGui::EndTooltip();
          }
        }
      }
      // try to detect label hovered.
      if (ImGui::IsItemHovered() && !ImPlot::IsPlotHovered())
      {
        auto mouse_pos = ImGui::GetMousePos();
        auto plot_pos = ImPlot::GetPlotPos();
        auto plot_size = ImPlot::GetPlotSize();
        if ((mouse_pos.x < plot_pos.x)
          && (mouse_pos.y < plot_pos.y + plot_size.y)
          && (mouse_pos.y > plot_pos.y))
        {
          // we are in the left label part
          if (!calibration_data_.series_names.empty())
          {
            ImGui::BeginTooltip();
            auto quantitation_methods = getQuantitationMethod(calibration_data_.series_names[0]);
            const auto& feature_name = quantitation_methods->getFeatureName();
            ImGui::Text("%s / IS %s", feature_name.c_str(), feature_name.c_str());
            ImGui::EndTooltip();
          }
        }
        if ((mouse_pos.x > plot_pos.x)
          && (mouse_pos.y > plot_pos.y + plot_size.y)
          && (mouse_pos.x < plot_pos.x + plot_size.x))
        {
          // we are in the bottom label part
          ImGui::BeginTooltip();
          ImGui::Text("actual concentration / IS actual concentration / dilution_factor");
          ImGui::EndTooltip();
        }
      }
      ImPlot::EndPlot();
      if (open_context_menu)
      {
        ImGui::OpenPopup("Point Actions");
      }
      if (ImGui::BeginPopup("Point Actions"))
      {
        if (ImGui::Selectable("Show chromatogram"))
        {
          auto [sample_name, serie_name] = getSampleNameAndSerieFromSelectedPoint(clicked_matching_point_, clicked_outlier_point_, clicked_excluded_point_);
          if (!sample_name.empty())
          {
            showChromatogram(sample_name);
          }
        }
        // Exlude/Include point
        auto [sample_name, serie_name] = getSampleNameAndSerieFromSelectedPoint(clicked_matching_point_, clicked_outlier_point_, clicked_excluded_point_);
        if (!sample_name.empty())
        {
          std::ostringstream os;
          os << sample_name << ";" << serie_name;
          ParameterSet& user_parameters = sequence_handler_.getSequence().at(0).getRawData().getParameters();
          Parameter* existing_parameter = user_parameters.findParameter("CalculateCalibration", "excluded_points");
          if ((!existing_parameter) || (!existing_parameter->isInList(os.str())))
          {
            if (ImGui::Selectable("Exclude from calibration"))
            {
              if (existing_parameter)
              {
                existing_parameter->addToList(os.str());
                recomputeCalibration();
              }
              else
              {
                std::ostringstream os;
                os << "[\'" << sample_name << ";" << calibration_data_.series_names[selected_component_] << "\']";
                Parameter new_param(
                  { { "name", "excluded_points" },
                    { "type", "list" },
                    { "value", os.str() } });
                user_parameters.addParameter("CalculateCalibration", new_param);
                recomputeCalibration();
              }
            }
          }
          else if (existing_parameter && existing_parameter->isInList(os.str()))
          {
            if (ImGui::Selectable("Include to calibration"))
            {
              existing_parameter->removeFromList(os.str());
              recomputeCalibration();
            }
          }
        }
        ImGui::EndPopup();
      }
      else
      {
        clicked_matching_point_ = std::nullopt;
        clicked_outlier_point_ = std::nullopt;
        clicked_excluded_point_ = std::nullopt;
      }
    }
    ImGui::End();
  }

  void CalibratorsPlotWidget::draw()
  {
    // update data
    std::vector<bool> selected_transitions;
    int test_i = session_handler_.transition_explorer_data.checkbox_body.dimension(0);
    for (int i = 0; i < session_handler_.transition_explorer_data.checkbox_body.dimension(0); ++i)
    {
      selected_transitions.push_back(session_handler_.transition_explorer_data.checkbox_body(i, 0));
    }
    if (previous_transition_selection_ != selected_transitions)
    {
      refresh_needed_ = true;
      previous_transition_selection_ = selected_transitions;
    }
    if (refresh_needed_)
    {
      SessionHandler::CalibrationData calibration_data;
      session_handler_.setCalibratorsScatterLinePlot(sequence_handler_, calibration_data);
      setCalibrationData(calibration_data, "CalibratorsMainWindow");
      refresh_needed_ = false;
    }

    param_to_edit_ = nullptr;

    showQuickHelpToolTip("CalibratorsPlotWidget");

    // Build default docking
    ImGuiID dockspace_id = ImGui::GetID("CalibratorsPlotWidgetDockSpace");
    if (reset_layout_)
    {
      ImGuiID left_node;
      ImGuiID right_node;
      ImGui::DockBuilderAddNode(dockspace_id, ImGuiDockNodeFlags_DockSpace);
      ImGui::DockBuilderSetNodeSize(dockspace_id, ImGui::GetCurrentWindow()->Size);
      ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Left, 0.5f, &left_node, &right_node); // The second parameter defines the direction of the split
      ImGui::DockBuilderDockWindow("Calibrator Parameters", left_node);
      ImGui::DockBuilderDockWindow("Calibrator Plot", right_node);
      ImGui::DockBuilderFinish(dockspace_id);
      reset_layout_ = false;
    }
    ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_AutoHideTabBar);

    displayParameters();
    displayPlot();

  }

  void CalibratorsPlotWidget::setCalibrationData(
    const SessionHandler::CalibrationData& calibration_data,
    const std::string& plot_title)
  {
    reset_zoom_ = true;
    components_.clear();
    component_cstr_.clear();
    int i = 0;
    for (const auto& component : calibration_data.series_names)
    {
      components_.push_back(component);
      component_cstr_.push_back(components_.at(i).c_str());
      ++i;
    }
    selected_component_ = 0;

    sequence_segments_.clear();
    sequence_segments_cstr_.clear();
    i = 0;
    for (const auto& sequence_segment : sequence_handler_.getSequenceSegments())
    {
      sequence_segments_.push_back(sequence_segment.getSequenceSegmentName());
      sequence_segments_cstr_.push_back(sequence_segments_.at(i).c_str());
      ++i;
    }
    selected_sequence_segment_ = 0;

    calibration_data_ = calibration_data;
    plot_title_ = plot_title;
  }

  void CalibratorsPlotWidget::getSelectedPoint(ImVec2 point, ImVec2 threshold_point)
  {
    hovered_matching_point_ = getSelectedPoint(point, threshold_point, calibration_data_.matching_points_);
    if (!hovered_matching_point_)
    {
      hovered_outlier_point_ = getSelectedPoint(point, threshold_point, calibration_data_.outlier_points_);
    }
    if (!hovered_outlier_point_)
    {
      hovered_excluded_point_ = getSelectedPoint(point, threshold_point, calibration_data_.excluded_points_);
    }
  }

  std::optional<std::tuple<int, int>> CalibratorsPlotWidget::getSelectedPoint(
    ImVec2 point,
    ImVec2 threshold_point,
    const SessionHandler::CalibrationData::Points& points)
  {
    for (int i = 0; i < points.concentrations_.size(); ++i)
    {
      const auto& concentrations = points.concentrations_[i];
      const auto& features = points.features_[i];
      for (int j = 0; j < concentrations.size(); ++j)
      {
        const auto& concentration = concentrations[j];
        const auto& feature = features[j];
        if ((concentration > (point.x - threshold_point.x))
          && (concentration < (point.x + threshold_point.x))
          && (feature > (point.y - threshold_point.y))
          && (feature < (point.y + threshold_point.y)))
        {
          return std::make_tuple(i,j);
        }
      }
    }
    return std::nullopt;
  }

  void CalibratorsPlotWidget::showChromatogram(const std::string& sample_name)
  {
    LOGD << "Show chromatogram for " << sample_name;
    // clear selection
    for (int i = 0; i < session_handler_.injection_explorer_data.checkbox_body.dimension(0); ++i)
    {
      session_handler_.injection_explorer_data.checkbox_body(i, 1) = false;
    }
    // check plot for this sample
    for (int i = 0; i < session_handler_.injection_explorer_data.checked_rows.dimension(0); ++i)
    {
      if (std::string(session_handler_.getInjectionExplorerBody()(i, 1)) == sample_name)
      {
        session_handler_.injection_explorer_data.checkbox_body(i, 1) = true;
        explorer_widget_->onCheckboxesChanged();
        chromatogram_widget_->visible_ = true;
        break;
      }
    }
  }

  void CalibratorsPlotWidget::onSequenceUpdated()
  {
    refresh_needed_ = true;
  }
}
