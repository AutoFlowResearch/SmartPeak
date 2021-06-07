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
// $Maintainer: Douglas McCloskey, Ahmed Khalil, Bertrand Boudaud $
// $Authors: Douglas McCloskey, Pasquale Domenico Colaianni $
// --------------------------------------------------------------------------

#include <SmartPeak/ui/GraphicDataVizWidget.h>
#include <imgui.h>
#include <imgui_internal.h>
#include <implot.h>

namespace SmartPeak
{

  void GraphicDataVizWidget::draw()
  {
    updateData();
    drawSliders();
    drawGraph();
  }

  void GraphicDataVizWidget::drawSliders()
  {
    // Widget's controls - that ImGui does not support natively
    const ImGuiSliderFlags slider_flags = ImGuiSliderFlags_AlwaysClamp;
    float controls_pos_start_y = ImGui::GetCursorPosY();
    ImGui::SliderFloat((std::string("min ") + chrom_.x_axis_title_).c_str(), &current_range_.first, slider_min_max_.first, current_range_.second, "%.4f", slider_flags);
    ImGui::SameLine();
    ImGui::Checkbox("Compact View", &compact_view_);
    ImGui::SliderFloat((std::string("max ") + chrom_.x_axis_title_).c_str(), &current_range_.second, current_range_.first, slider_min_max_.second, "%.4f", slider_flags);
    ImGui::SameLine();
    ImGui::Checkbox("Legend", &show_legend_);
    if (chrom_.z_axis_title_)
    {
      std::ostringstream os;
      os << chrom_.z_data_area_[current_z_];
      ImGui::SliderInt(chrom_.z_axis_title_->c_str(), &current_z_, 0, chrom_.z_data_area_.size() - 1, os.str().c_str(), ImGuiSliderFlags_NoInput);
    }
    float controls_pos_end_y = ImGui::GetCursorPosY();
    sliders_height_ = (controls_pos_end_y - controls_pos_start_y);
  }

  std::tuple<float, float, float, float> GraphicDataVizWidget::plotLimits() const
  {
    // compute plot limit by adding padding
    float border_padding_x = (chrom_.x_max_ - chrom_.x_min_) * 0.01f;
    float border_padding_y = (chrom_.y_max_ - chrom_.y_min_) * 0.01f;
    auto plot_min_x = current_range_.first - border_padding_x;
    auto plot_max_x = current_range_.second + border_padding_x;
    auto plot_min_y = chrom_.y_min_ - border_padding_y;
    auto plot_max_y = chrom_.y_max_ * 1.1f;
    return std::make_tuple(plot_min_x, plot_max_x, plot_min_y, plot_max_y);
  }

  void GraphicDataVizWidget::drawGraph()
  {
    // Main graphic
    if (chrom_.points_overflow)
    {
      ImGui::Text("Unable to draw: too much points. Please reduce scope or unselect data.");
    }
    else if (chrom_.x_data_area_.empty() && chrom_.y_data_area_.empty())
    {
      ImGui::Text("No data to display select data to render or adjust ranges.");
    }
    else
    {
      auto [plot_min_x, plot_max_x, plot_min_y, plot_max_y] = plotLimits();
      ImPlot::SetNextPlotLimits(plot_min_x, plot_max_x, plot_min_y, plot_max_y, ImGuiCond_Always);
      ImPlotFlags plotFlags = show_legend_ ? ImPlotFlags_Default | ImPlotFlags_Legend : ImPlotFlags_Default & ~ImPlotFlags_Legend;
      plotFlags |= ImPlotFlags_Crosshairs;
      float graphic_height = height_ - sliders_height_;
      if (ImPlot::BeginPlot(plot_title_.c_str(), chrom_.x_axis_title_.c_str(), chrom_.y_axis_title_.c_str(), ImVec2(width_ - 25, graphic_height - 40), plotFlags)) {
        int i = 0;
        for (const auto& serie_name_scatter : chrom_.series_names_area_)
        {
          if (chrom_.x_data_area_.size())
          {
            assert(chrom_.x_data_area_.at(i).size() == chrom_.y_data_area_.at(i).size());
            ImPlot::PushStyleVar(ImPlotStyleVar_Marker, ImPlotMarker_None);
            if (!is_spectra_)
            {
              ImPlot::PlotLine(serie_name_scatter.c_str(),
                               chrom_.x_data_area_.at(i).data(),
                               chrom_.y_data_area_.at(i).data(),
                               chrom_.x_data_area_.at(i).size());
            }
            else
            {
              static const float bar_width_in_pixel = 3.0f;
              auto bar_width = ImPlot::PixelsToPlot({ bar_width_in_pixel ,0.0f }).x - ImPlot::PixelsToPlot({ 0.0f,0.0f }).x;
              ImPlot::PlotBars(serie_name_scatter.c_str(),
                               chrom_.x_data_area_.at(i).data(), 
                               chrom_.y_data_area_.at(i).data(), 
                               chrom_.x_data_area_.at(i).size(),
                               bar_width);
            }
          }
          ImPlotMarker plot_marker = ImPlotMarker_Circle;
          int feature_index = 0;
          for (int j = 0; j < chrom_.x_data_scatter_.size(); ++j) {
            // Corresponding serie names are supposed to start with same name as the scatter name
            if (chrom_.series_names_scatter_.at(j).rfind(serie_name_scatter) == 0)
            {
              assert(chrom_.x_data_scatter_.at(j).size() == chrom_.y_data_scatter_.at(j).size());
              ImPlot::PushStyleVar(ImPlotStyleVar_Marker, plot_marker);
              std::string legend_text = serie_name_scatter;
              if (!compact_view_)
              {
                legend_text = chrom_.series_names_scatter_.at(j) + "::" + std::to_string(feature_index);
              }
              ImPlot::PlotScatter(legend_text.c_str(), 
                                  chrom_.x_data_scatter_.at(j).data(), 
                                  chrom_.y_data_scatter_.at(j).data(),
                                  chrom_.x_data_scatter_.at(j).size());
              plot_marker <<= 1;
              if (plot_marker > ImPlotMarker_Asterisk) plot_marker = ImPlotMarker_Circle;
              ++feature_index;
            }
          }
          ++i;
        }
        drawMarkers();
        ImPlot::EndPlot();
      }
    }
  }

  void GraphicDataVizWidget::drawMarkers()
  {
    // draw placed marker
    if (use_markers_)
    {
      const auto marker_position = getMarkerPosition();
      auto [plot_min_x, plot_max_x, plot_min_y, plot_max_y] = plotLimits();
      if (marker_position)
      {
        float marker_line_x[] = { *marker_position, *marker_position };
        float marker_line_y[] = { plot_min_y, plot_max_y };
        ImPlot::PushStyleColor(0, ImGui::GetStyle().Colors[ImGuiCol_Text]);
        ImPlot::PlotLine("", marker_line_x, marker_line_y, 2);
        ImPlot::PopStyleColor();
      }
      // mouse hover marker
      if (ImPlot::IsPlotHovered())
      {
        auto plot_point = ImPlot::GetPlotMousePos();
        auto nearest_point = getNearestPoint(plot_point.x);
        if (nearest_point)
        {
          float marker_line_x[] = { *nearest_point, *nearest_point };
          float marker_line_y[] = { plot_min_y, plot_max_y };
          ImPlot::PushStyleColor(0, ImGui::GetStyle().Colors[ImGuiCol_FrameBgActive]);
          ImPlot::PlotLine("", marker_line_x, marker_line_y, 2);
          ImPlot::PopStyleColor();
          if (ImGui::IsMouseClicked(0))
          {
            setMarkerPosition(nearest_point);
          }
        }
      }
    }
  }

  std::optional<float> GraphicDataVizWidget::getMarkerPosition() const
  {
    return marker_position_;
  }

  void GraphicDataVizWidget::setMarkerPosition(const std::optional<float>& marker_position)
  {
    marker_position_ = marker_position;
  }

  std::set<std::string> GraphicDataVizWidget::getSelectedSampleNames() const
  {
    std::set<std::string> sample_names;
    Eigen::Tensor<std::string, 1> selected_sample_names = session_handler_.getSelectSampleNamesPlot();
    for (int i = 0; i < selected_sample_names.size(); ++i) {
      if (!selected_sample_names(i).empty())
        sample_names.insert(selected_sample_names(i));
    }
    return sample_names;
  }

  std::set<std::string> GraphicDataVizWidget::getSelectedTransitions() const
  {
    std::set<std::string> transitions_names;
    Eigen::Tensor<std::string, 1> selected_transitions = session_handler_.getSelectTransitionsPlot();
    for (int i = 0; i < selected_transitions.size(); ++i) {
      if (!selected_transitions(i).empty())
        transitions_names.insert(selected_transitions(i));
    }
    return transitions_names;
  }

  std::set<std::string> GraphicDataVizWidget::getSelectedSpectrum() const
  {
    std::set<std::string> scan_names;
    Eigen::Tensor<std::string, 1> selected_scans = session_handler_.getSelectSpectrumPlot();
    for (int i = 0; i < selected_scans.size(); ++i) {
      if (!selected_scans(i).empty())
        scan_names.insert(selected_scans(i));
    }
    return scan_names;
  }

  std::set<std::string> GraphicDataVizWidget::getSelectedTransitionGroups() const
  {
    std::set<std::string> component_group_names;
    Eigen::Tensor<std::string, 1> selected_transition_groups = session_handler_.getSelectTransitionGroupsPlot();
    for (int i = 0; i < selected_transition_groups.size(); ++i) {
      if (!selected_transition_groups(i).empty())
        component_group_names.insert(selected_transition_groups(i));
    }
    return component_group_names;
  }

  std::optional<float> GraphicDataVizWidget::getNearestPoint(float in_x) const
  {
    std::optional<float> result = {};
    float current_distance = std::numeric_limits<float>::max();
    size_t nearest_serie_index = -1;
    for (const auto& scatter_x : chrom_.x_data_area_)
    {
      for (const auto& pos_x : scatter_x)
      {
        auto distance_test = std::abs(pos_x - in_x);
        if (distance_test < current_distance)
        {
          current_distance = distance_test;
          result = pos_x;
        }
      }
    }
    return result;
  }
  
}
