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
// $Maintainer: Douglas McCloskey, Ahmed Khalil $
// $Authors: Douglas McCloskey, Pasquale Domenico Colaianni $
// --------------------------------------------------------------------------

#include <SmartPeak/ui/Widget.h>
#include <algorithm>
#include <map>
#include <string>
#include <utility>
#include <vector>
#include <imgui.h>
#include <implot.h>

namespace SmartPeak
{
  void Widget::FilterPopup(
    const char* popuop_id,
    ImGuiTextFilter& filter,
    const Eigen::Tensor<std::string, 1>& column,
    bool* checked,
    const std::vector<std::pair<std::string, std::vector<size_t>>>& values_indices) {
    if (false == ImGui::BeginPopup(popuop_id))
      return;

    //// Sorting
    //if (ImGui::Button("Asc"))
    //{
    //  // TODO: will need to rethink how this function is called to implement sorting on the entire table
    //  //std::sort(std::begin(column), std::end(column), [](std::string a, std::string b) {return a > b; });
    //  //std::sort(std::begin(values_indices), std::end(values_indices), [](std::pair<std::string, std::vector<int>> a, std::pair<std::string, std::vector<int>> b) {return a.first > b.first; });
    //}
    //ImGui::SameLine();
    //if (ImGui::Button("Desc"))
    //{
    //  // TODO: will need to rethink how this function is called to implement sorting on the entire table
    //  //std::sort(std::begin(column), std::end(column), [](std::string a, std::string b) {return a < b; });
    //  //std::sort(std::begin(values_indices), std::end(values_indices), [](std::pair<std::string, std::vector<int>> a, std::pair<std::string, std::vector<int>> b) {return a.first < b.first; });
    //}
    //ImGui::Separator();

    // Filtering and selecting
    filter.Draw();

    if (ImGui::Button("check all")) {
      for (const std::pair<std::string, std::vector<size_t>>& indexes : values_indices) {
        for (const size_t i: indexes.second) {
          checked[i] = 1;
        }
      }
    }

    ImGui::SameLine();

    if (ImGui::Button("uncheck all")) {
      for (const std::pair<std::string, std::vector<size_t>>& indexes : values_indices) {
        for (const size_t i : indexes.second) {
          checked[i] = 0;
        }
      }
    }

    for (const std::pair<std::string, std::vector<size_t>>& indexes : values_indices) {
      if (filter.PassFilter(indexes.first.c_str())) {
        ImGui::Checkbox(indexes.first.c_str(), &checked[indexes.second.front()]);
      }
    }

    // Update checked rows not explicitly shown
    for (const std::pair<std::string, std::vector<size_t>>& indexes : values_indices) {
      for (const size_t i : indexes.second) {
        checked[i] = checked[indexes.second.front()];
      }
    }

    // NOTE: Appears to apply the filter immediately so this is not needed
    // // Apply filters
    // ImGui::Separator();
    // if (ImGui::Button("Accept"))
    // {
    //   //TODO: apply filter and exit
    // }
    // ImGui::SameLine();
    // if (ImGui::Button("Cancel"))
    // {
    //   //TODO: exit without applying filter
    // }

    ImGui::EndPopup();
  }

  void Widget::makeFilters(
    const Eigen::Tensor<std::string, 1>& headers,
    const Eigen::Tensor<std::string,2>& columns,
    std::vector<std::vector<std::pair<std::string, std::vector<std::size_t>>>>& columns_indices,
    std::vector<ImGuiTextFilter>& filter) {
    for (std::size_t col = 0; col < headers.size(); ++col)
    {
      // Extract out unique columns and replicate indices
      std::map<std::string, std::vector<std::size_t>> value_indices;
      for (std::size_t row = 0; row < columns.dimension(0); ++row) {
        std::vector<std::size_t> index = { row };
        auto found = value_indices.emplace(columns(col,row), index);
        if (!found.second) {
          value_indices.at(columns(col,row)).push_back(row);
        }
      }

      // Copy into a vector of pairs (used for downstream sorting)
      std::vector<std::pair<std::string, std::vector<std::size_t>>> value_indices_v;
      std::copy(
        value_indices.begin(),
        value_indices.end(),
        std::back_inserter(value_indices_v)
      );
      columns_indices.push_back(value_indices_v);

      // Initialize the filters
      static ImGuiTextFilter filter0;
      filter.push_back(filter0);
    }
  }

  void GenericTableWidget::draw()
  {
    if (headers_.size()<=0)
      return;

    // headers
    const ImGuiTableFlags table_flags = ImGuiTableFlags_Resizable | ImGuiTableFlags_Hideable | ImGuiTableFlags_Reorderable |
      ImGuiTableFlags_NoBordersInBody | ImGuiTableFlags_ScrollY | ImGuiTableFlags_SizingFixedSame| ImGuiTableFlags_NoSavedSettings |
      ImGuiTableFlags_Sortable | ImGuiTableFlags_SortMulti;

    if (ImGui::BeginTable(table_id_.c_str(), headers_.size(), table_flags)) {
      // First row headers
      for (int col = 0; col < headers_.size(); col++) {
        ImGui::TableSetupColumn(headers_(col).c_str());
      }
      ImGui::TableSetupScrollFreeze(headers_.size(), 1);
      ImGui::TableHeadersRow();
      
      // Second row to end body
      if (columns_.size() > 0) {
        for (size_t row = 0; row < columns_.dimension(0); ++row) {
          if (checked_rows_.size() <= 0 || (checked_rows_.size() > 0 && checked_rows_(row))) {
            ImGui::TableNextRow();
            for (size_t col = 0; col < headers_.size(); ++col) {
              ImGui::TableSetColumnIndex(col);
              ImGui::Text("%s", columns_(row,col).c_str());
            }
          }
        }
      }
      ImGui::EndTable();
    }
  }

  void ExplorerWidget::draw()
  {
    if (headers_.size()<=0)
      return;

    // headers
    const ImGuiTableFlags table_flags = ImGuiTableFlags_Resizable | ImGuiTableFlags_Hideable | ImGuiTableFlags_Reorderable |
      ImGuiTableFlags_NoBordersInBody | ImGuiTableFlags_ScrollY | ImGuiTableFlags_SizingFixedSame| ImGuiTableFlags_NoSavedSettings |
      ImGuiTableFlags_Sortable | ImGuiTableFlags_SortMulti;

    if (ImGui::BeginTable(table_id_.c_str(), headers_.size() + checkbox_headers_.size(), table_flags)) {
      // First row headers
      for (int col = 0; col < headers_.size(); col++) {
        ImGui::TableSetupColumn(headers_(col).c_str());
      }
      for (int col = 0; col < checkbox_headers_.size(); col++) {
        ImGui::TableSetupColumn(checkbox_headers_(col).c_str());
      }
      ImGui::TableSetupScrollFreeze(headers_.size() + checkbox_headers_.size(), 1);
      ImGui::TableHeadersRow();
      
      // Second row to end body
      if (columns_.size() > 0) {
        for (size_t row = 0; row < columns_.dimension(0); ++row) {
          if (checked_rows_.size() <=0 || (checked_rows_.size() > 0 && checked_rows_(row))) {
            ImGui::TableNextRow();
            for (size_t col = 0; col < headers_.size(); ++col) {
              ImGui::TableSetColumnIndex(col);
              ImGui::Text("%s", columns_(row, col).c_str());
            }
            for (size_t col = 0; col < checkbox_headers_.size(); ++col) {
              std::string id = table_id_ + std::to_string(col) + std::to_string(row*columns_.dimension(1));
              ImGui::TableSetColumnIndex(col + headers_.size());
              ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
              ImGui::Checkbox(id.c_str(), &checkbox_columns_(row,col));
              ImGui::PopStyleColor();
            }
          }
        }
      }
      ImGui::EndTable();
    }
  }

  void GenericGraphicWidget::draw()
  {
  }

  void LinePlot2DWidget::draw()
  {
    // Main graphic
    assert(x_data_.dimensions() == y_data_.dimensions());
    ImPlot::SetNextPlotLimits(x_min_, x_max_, y_min_, y_max_, ImGuiCond_Always);
    if (ImPlot::BeginPlot(plot_title_.c_str(), x_axis_title_.c_str(), y_axis_title_.c_str(), ImVec2(plot_width_-25, plot_height_-40))) {
      for (int i = 0; i < x_data_.dimension(1); ++i) {
        ImPlot::PushStyleVar(ImPlotStyleVar_LineWeight, ImPlot::GetStyle().LineWeight);
        ImPlot::PushStyleVar(ImPlotStyleVar_Marker, ImPlotMarker_Circle);
        Eigen::Tensor<float, 1> x_data = x_data_.chip(i, 1);
        Eigen::Tensor<float, 1> y_data = y_data_.chip(i, 1);
        ImPlot::PlotLine(series_names_(i).c_str(), x_data.data(), y_data.data(), x_data_.dimension(0));
      }
      ImPlot::EndPlot();
    }
  }

  void ScatterPlotWidget::draw()
  {
    updateScatterPlotData();
    // Widget's controls - that ImGui does not support natively
    const ImGuiSliderFlags slider_flags = ImGuiSliderFlags_AlwaysClamp;
    float controls_pos_start_y = ImGui::GetCursorPosY();
    ImGui::SliderFloat((std::string("min ") + chrom_.x_axis_title_).c_str(), &current_range_.first, slider_min_max_.first, current_range_.second, "%.4f", slider_flags);
    ImGui::SameLine();
    ImGui::Checkbox("Compact View", &compact_view_);
    ImGui::SliderFloat((std::string("max ") + chrom_.x_axis_title_).c_str(), &current_range_.second, current_range_.first, slider_min_max_.second, "%.4f", slider_flags);
    ImGui::SameLine();
    ImGui::Checkbox("Legend", &show_legend_);
    float controls_pos_end_y = ImGui::GetCursorPosY();
    // Main graphic
    float graphic_height = plot_height_ - (controls_pos_end_y - controls_pos_start_y);
    ImPlot::SetNextPlotLimits(current_range_.first, current_range_.second, chrom_.y_min_, chrom_.y_max_, ImGuiCond_Always);
    ImPlotFlags plotFlags = show_legend_ ? ImPlotFlags_Default | ImPlotFlags_Legend : ImPlotFlags_Default & ~ImPlotFlags_Legend;
    plotFlags |= ImPlotFlags_Crosshairs;
    if (ImPlot::BeginPlot(plot_title_.c_str(), chrom_.x_axis_title_.c_str(), chrom_.y_axis_title_.c_str(), ImVec2(plot_width_ - 25, graphic_height - 40), plotFlags)) {
      int i = 0;
      for (const auto& serie_name_scatter : chrom_.series_names_scatter_)
      {
        assert(chrom_.x_data_scatter_.at(i).size() == chrom_.y_data_scatter_.at(i).size());
        ImPlot::PushStyleVar(ImPlotStyleVar_Marker, ImPlotMarker_None);
        ImPlot::PlotLine(serie_name_scatter.c_str(), chrom_.x_data_scatter_.at(i).data(), chrom_.y_data_scatter_.at(i).data(), chrom_.x_data_scatter_.at(i).size());
        ImPlotMarker plot_marker = ImPlotMarker_Circle;
        int feature_index = 0;
        for (int  j = 0; j < chrom_.x_data_area_.size(); ++j) {
          // Corresponding serie names are supposed to start with same name as the scatter name
          if (chrom_.series_names_area_.at(j).rfind(serie_name_scatter) == 0)
          {
            assert(chrom_.x_data_area_.at(j).size() == chrom_.y_data_area_.at(j).size());
            ImPlot::PushStyleVar(ImPlotStyleVar_Marker, plot_marker);
            std::string legend_text = serie_name_scatter;
            if (!compact_view_)
            {
              legend_text = chrom_.series_names_area_.at(j) + "::" + std::to_string(feature_index);
            }
            ImPlot::PlotScatter(legend_text.c_str(), chrom_.x_data_area_.at(j).data(), chrom_.y_data_area_.at(j).data(), chrom_.x_data_area_.at(j).size());
            plot_marker <<= 1;
            if (plot_marker > ImPlotMarker_Asterisk) plot_marker = ImPlotMarker_Circle;
            ++feature_index;
          }
        }
        ++i;
      }
      ImPlot::EndPlot();
    }
  }
  
  void GenericTreeWidget::draw()
  {
    // left

    // TODO: search by sample name (or sample_group or sequence_segment)
    // TODO: switch between samples, sample_groups, and sequence_segments
    // TODO: check for samples included in the "current" analysis
    // TODO: tooltip with status information for each sample
    static int selected = 0;
    ImGui::BeginChild("left pane", ImVec2(150, 0), true, ImGuiWindowFlags_HorizontalScrollbar);
    for (int i = 0; i < 100; ++i)
    {
      // TODO: change to button with color codes for the following
      // - pass = green
      // - warn = yellow
      // - fail = red
      // add progress bar to indicate completed
      char label[128];
      sprintf(label, "MyObject %d", i); // TODO: update for use with sequence list
      if (ImGui::Selectable(label, selected == i))
        selected = i;
    }
    ImGui::EndChild();
    ImGui::SameLine();

    // right

    // TODO add ability to search and filter by
    // - sample_name
    // - sequence_group_name
    // - sample_group_name
    // - sample_type
    // - sample_status (i.e., pass, warn, fail)
    // - completion status
    // TODO change row column to/from sample, sample_group, sequence_group depending on selected in left panel
    ImGui::BeginGroup();
    ImGui::BeginChild("item view", ImVec2(0, -ImGui::GetFrameHeightWithSpacing()), false, ImGuiWindowFlags_HorizontalScrollbar); // Leave room for 1 line below us
    ImGui::Text("MyObject: %d", selected); //TODO: update for use with sequence list
    ImGui::Separator();

    // TODO: anlaysis output
    ImGui::TextWrapped("Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. ");

    // TODO: add "tag" dropdown
    // TODO: row of buttons for sequence_group

    // TODO: add "tag" dropdown
    // TODO: row of buttons for sample_group

    // TODO: add "tag" dropdown
    // TODO: drop down for sample type

    // NOTE: each button should have an "x" that deletes the tag
    ImGui::EndChild();
    ImGui::EndGroup();

    // TODO: add selected samples to plot widget
    // TODO: add selected samples to workflow widget
  }

  void WorkflowWidget::draw()
  {
  }

  void GenericTextWidget::draw()
  {
    for (const std::string& line : text_lines) {
      ImGui::TextWrapped("%s", line.c_str());
    }
  }
}
