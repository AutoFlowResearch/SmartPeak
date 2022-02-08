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
#include <SmartPeak/core/Utilities.h>
#include <implot.h>

namespace SmartPeak
{

  void CalibratorsPlotWidget::recomputeCalibration()
  {
    CalculateCalibration calculate_calibration;
    Filenames filenames; // calculate_calibration actually does not use it
    calculate_calibration.process(sequence_handler_.getSequenceSegments()[0], sequence_handler_, user_params_, filenames);
  }

  OpenMS::AbsoluteQuantitationMethod* CalibratorsPlotWidget::getQuantitationMethod(const std::string& component_name)
  {
    for (auto& sequence_segment : sequence_handler_.getSequenceSegments())
    {
      for (auto& quantitation_method : sequence_segment.getQuantitationMethods())
      {
        if (quantitation_method.getComponentName() == component_name)
        {
          return &quantitation_method;
        }
      }
    }
    return nullptr;
  }

  void CalibratorsPlotWidget::addParameterRow(std::shared_ptr<Parameter> param)
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
    if (ImGui::IsItemHovered())
    {
      ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
    }
    if (ImGui::IsItemClicked())
    {
      param_to_edit_ = param;
    }
  }

  std::shared_ptr<Parameter> CalibratorsPlotWidget::CalibratorParameterToSmartPeakParameter(const OpenMS::Param::ParamEntry& param)
  {
    auto result = std::make_shared<Parameter>(param);
    if (param.name == "y_weight")
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

    parameter_editor_widget_.draw();

    ImGui::Combo("Component", &selected_component_, &component_cstr_[0], component_cstr_.size());

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

      ImGui::EndTable();
    }

    if (ImGui::TreeNode("Input parameters"))
    {
      if (ImGui::BeginTable("Calibrator input parameters", 2, table_flags))
      {
        // CalculateCalibration params
        CalculateCalibration calculate_calibration;
        auto calculate_calibration_params_schema = calculate_calibration.getParameterSchema();
        user_params_ = sequence_handler_.getSequence().at(0).getRawData().getParameters();
        calculate_calibration_params_schema.setAsSchema(true);
        user_params_.merge(calculate_calibration_params_schema);

        for (auto& calculate_calibration_fct : calculate_calibration_params_schema)
        {
          for (auto& calculate_calibration_param : calculate_calibration_fct.second)
          {
            addParameterRow(std::make_shared<Parameter>(calculate_calibration_param));
          }
        }

        // Transformation model Params (inputs)
        const auto& params = quantitation_methods->getTransformationModelParams();
        for (const auto& param : params)
        {
          if (std::find(output_parameters.begin(), output_parameters.end(), param.name) == output_parameters.end())
          {
            addParameterRow(CalibratorParameterToSmartPeakParameter(param));
          }
        }

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("transformation model");
        ImGui::TableSetColumnIndex(1);
        ImGui::Text(quantitation_methods->getTransformationModel().c_str());

        ImGui::EndTable();
      }
      ImGui::TreePop();
    }

    if (ImGui::TreeNode("Output parameters"))
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
            addParameterRow(std::make_shared<Parameter>(param));
          }
        }
        ImGui::EndTable();
      }
      ImGui::TreePop();
    }

    if (param_to_edit_)
    {
      parameter_editor_widget_.setParameter(param_to_edit_->getName(), (*param_to_edit_));
      ImGui::OpenPopup("Edit Parameter");
    }
    ImGui::End();
  }

  void CalibratorsPlotWidget::onParameterSet(const Parameter& parameter)
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
    for (auto& user_param_functions : user_params_)
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

  void CalibratorsPlotWidget::displayPlot()
  {
    ImGui::Begin("Calibrator Plot");
    ImGui::Checkbox("Legend", &show_legend_);
    ImGui::SameLine();
    ImGui::Checkbox("Fit line", &show_fit_line_);
    ImGui::SameLine();
    ImGui::Checkbox("Points", &show_points_);
    ImGui::SameLine();
    ImGui::Checkbox("Outlier points", &show_outlier_points_);
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
      if (show_points_)
      {
        for (int i = 0; i < calibration_data_.conc_raw_data.size(); ++i) {
          assert(calibration_data_.conc_raw_data.at(i).size() == calibration_data_.feature_raw_data.at(i).size());
          ImPlot::PushStyleVar(ImPlotStyleVar_Marker, ImPlotMarker_Circle);
          ImPlot::PlotScatter((calibration_data_.series_names.at(i)).c_str(),
                               calibration_data_.conc_raw_data.at(i).data(),
                               calibration_data_.feature_raw_data.at(i).data(),
                               calibration_data_.conc_raw_data.at(i).size());
          ImPlot::PopStyleVar();
        }
      }
      if (show_outlier_points_)
      {
        for (int i = 0; i < calibration_data_.conc_raw_data.size(); ++i) {
          assert(calibration_data_.outlier_conc_raw_data.at(i).size() == calibration_data_.outlier_feature_raw_data.at(i).size());
          ImPlot::PushStyleVar(ImPlotStyleVar_Marker, ImPlotMarker_Cross);
          ImPlot::PlotScatter((calibration_data_.series_names.at(i)).c_str(),
                               calibration_data_.outlier_conc_raw_data.at(i).data(),
                               calibration_data_.outlier_feature_raw_data.at(i).data(),
                               calibration_data_.outlier_conc_raw_data.at(i).size());
          ImPlot::PopStyleVar();
        }
      }
      if (selected_point_)
      {
        auto [i, j] = *selected_point_;
        ImPlot::PushStyleColor(0, ImVec4(ImColor(255, 255, 255)));
        ImPlot::PlotScatter("", // do not appear in legend
          calibration_data_.conc_raw_data.at(i).data() + j,
          calibration_data_.feature_raw_data.at(i).data() + j,
          1);
        ImPlot::PopStyleColor();
      }
      if (selected_outlier_point_)
      {
        auto [i, j] = *selected_outlier_point_;
        ImPlot::PushStyleColor(0, ImVec4(ImColor(255, 255, 255)));
        ImPlot::PushStyleVar(ImPlotStyleVar_Marker, ImPlotMarker_Cross);
        ImPlot::PlotScatter("", // do not appear in legend
          calibration_data_.outlier_conc_raw_data.at(i).data() + j,
          calibration_data_.outlier_feature_raw_data.at(i).data() + j,
          1);
        ImPlot::PopStyleVar();
        ImPlot::PopStyleColor();
      }
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

      // Hovered points
      selected_point_ = std::nullopt;
      selected_outlier_point_ = std::nullopt;
      if (ImPlot::IsPlotHovered())
      {
        auto mouse_plot_point = ImPlot::GetPlotMousePos();
        ImVec2 mouse_point(mouse_plot_point.x, mouse_plot_point.y);
        // Pixels zone around which the point will be considered hovered
        auto zero_plot_point = ImPlot::PixelsToPlot(ImVec2(0, 0));
        auto threshold_plot_point = ImPlot::PixelsToPlot(ImVec2(5, 5));
        ImVec2 threshold_point(threshold_plot_point.x - zero_plot_point.x, zero_plot_point.y - threshold_plot_point.y);
        getSelectedPoint(mouse_point, threshold_point);
        if ((selected_outlier_point_ || selected_point_) && ImGui::IsMouseClicked(0))
        {
          clicked_point_ = selected_point_;
          clicked_outlier_point_ = selected_outlier_point_;
          ImGui::OpenPopup("Point Actions");
        }
        if (ImGui::BeginPopup("Point Actions"))
        {
          if (ImGui::Selectable("Show chromatogram"))
          {
            if (clicked_point_)
            {
              const auto [serie, index] = *clicked_point_;
              auto injection_name = calibration_data_.injections[serie][index];
              showChromatogram(injection_name);
            }
            else if (clicked_outlier_point_)
            {
              const auto [serie, index] = *clicked_outlier_point_;
              auto injection_name = calibration_data_.outlier_injections[serie][index];
              showChromatogram(injection_name);
            }
          }
          if (ImGui::Selectable("Exclude from calibration"))
          {
          }
          ImGui::EndPopup();
        }
        else
        {
          clicked_point_ = std::nullopt;
          clicked_outlier_point_ = std::nullopt;
        }
      }
      ImPlot::EndPlot();
    }
    ImGui::End();
  }

  void CalibratorsPlotWidget::draw()
  {
    // update data
    SessionHandler::CalibrationData calibration_data;
    session_handler_.setCalibratorsScatterLinePlot(sequence_handler_, calibration_data);
    setValues(calibration_data, "CalibratorsMainWindow");

    param_to_edit_ = nullptr;

    if (!calibration_data_.conc_raw_data.size())
    {
      return;
    }
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

  void CalibratorsPlotWidget::setValues(
    const SessionHandler::CalibrationData& calibration_data,
    const std::string& plot_title)
  {
    if ((calibration_data_.series_names != calibration_data.series_names)
      || (std::abs(calibration_data_.conc_min - calibration_data.conc_min) > 1e-9)
      || (std::abs(calibration_data_.conc_max - calibration_data.conc_max) > 1e-9)
      || (std::abs(calibration_data_.feature_min - calibration_data.feature_min) > 1e-9)
      || (std::abs(calibration_data_.feature_max - calibration_data.feature_max) > 1e-9)
      )
    {
      reset_zoom_ = true;
      int i = 0;
      components_.clear();
      component_cstr_.clear();
      for (const auto& component : calibration_data.series_names)
      {
        components_.push_back(component);
        component_cstr_.push_back(components_.at(i).c_str());
        ++i;
      }
      selected_component_ = 0;
    }
    calibration_data_ = calibration_data;
    plot_title_ = plot_title;
  }

  void CalibratorsPlotWidget::getSelectedPoint(ImVec2 point, ImVec2 threshold_point)
  {
    selected_point_ = getSelectedPoint(point, threshold_point, calibration_data_.conc_raw_data, calibration_data_.feature_raw_data);
    if (!selected_point_)
    {
      selected_outlier_point_ = getSelectedPoint(point, threshold_point, calibration_data_.outlier_conc_raw_data, calibration_data_.outlier_feature_raw_data);
    }
  }

  std::optional<std::tuple<int, int>> CalibratorsPlotWidget::getSelectedPoint(ImVec2 point,
                                              ImVec2 threshold_point,
                                              std::vector<std::vector<float>> concentrations_serie,
                                              std::vector<std::vector<float>> feature_serie)
  {
    for (int i = 0; i < concentrations_serie.size(); ++i)
    {
      const auto& concentrations = concentrations_serie[i];
      const auto& features = feature_serie[i];
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

}
