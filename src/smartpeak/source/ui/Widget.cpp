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
      ImGuiTableFlags_NoBordersInBody | ImGuiTableFlags_Scroll | ImGuiTableFlags_SizingPolicyFixedX | ImGuiTableFlags_NoSavedSettings |
      ImGuiTableFlags_Sortable | ImGuiTableFlags_MultiSortable;

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

  void ParametersTableWidget::draw()
  {
    if (headers_.size() <= 0)
      return;

    static bool show_default = true;
    static bool show_unused = true;
    ImGui::Checkbox("Show default", &show_default);
    ImGui::SameLine();
    ImGui::Checkbox("Show unused", &show_unused);

    // headers
    const ImGuiTableFlags table_flags = ImGuiTableFlags_Resizable | ImGuiTableFlags_Hideable | ImGuiTableFlags_Reorderable |
      ImGuiTableFlags_NoBordersInBody | ImGuiTableFlags_Scroll | ImGuiTableFlags_SizingPolicyFixedX | ImGuiTableFlags_NoSavedSettings |
      ImGuiTableFlags_Sortable | ImGuiTableFlags_MultiSortable;

    // Columns we want to show up
    std::vector<int> cols_to_show = { 0, 1, 2, 3, 7 };

    if (ImGui::BeginTable(table_id_.c_str(), cols_to_show.size(), table_flags)) {

      // First row headers
      for (auto col : cols_to_show) {
        ImGui::TableSetupColumn(headers_(col).c_str());
      }
      ImGui::TableSetupScrollFreeze(cols_to_show.size(), 1);
      ImGui::TableHeadersRow();

      // Second row to end body
      if (columns_.size() > 0) {
        for (size_t row = 0; row < columns_.dimension(0); ++row) {
          if (checked_rows_.size() <= 0 || (checked_rows_.size() > 0 && checked_rows_(row))) {
            const std::string& status = columns_(row, 5);
            const std::string& valid = columns_(row, 6);
            if ((status == "user_override") || (show_unused && (status == "unused")) || (show_default && (status == "default")))
            {
              ImGui::TableNextRow();
              int col_index = 0;
              for (auto col : cols_to_show) {
                ImGui::TableSetColumnIndex(col_index);
                if (status == "user_override")
                {
                  if ((col == 3) && (valid == "false"))
                  {
                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
                  }
                  else
                  {
                    ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetStyle().Colors[ImGuiCol_Text]);
                  }
                  ImGui::Text("%s", columns_(row, col).c_str());
                  ImGui::PopStyleColor();
                  if ((col == 3) && (valid == "false"))
                  {
                    if (ImGui::IsItemHovered())
                    {
                      ImGui::BeginTooltip();
                      const std::string& constraints = columns_(row, 7);
                      const std::string& expected_type = columns_(row, 8);
                      ImGui::Text("Out of range value.");
                      if (!constraints.empty())
                      {
                        ImGui::Text("Expected values:");
                        ImGui::Text(constraints.c_str());
                      }
                      ImGui::Text("Expected type:");
                      ImGui::Text(expected_type.c_str());
                      ImGui::EndTooltip();
                    }
                  }
                }
                else if (status == "unused")
                {
                  ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetStyle().Colors[ImGuiCol_TabActive]);
                  ImGui::Text("%s", columns_(row, col).c_str());
                  ImGui::PopStyleColor();
                }
                else if (status == "default")
                {
                  ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetStyle().Colors[ImGuiCol_TextDisabled]);
                  ImGui::Text("%s", columns_(row, col).c_str());
                  ImGui::PopStyleColor();
                }
                if (col == 1)
                {
                  if (ImGui::IsItemHovered())
                  {
                    const std::string& description = columns_(row, 4);
                    ImGui::BeginTooltip();
                    ImGui::Text(description.c_str());
                    ImGui::EndTooltip();
                  }
                }
                col_index++;
              }
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
      ImGuiTableFlags_NoBordersInBody | ImGuiTableFlags_Scroll | ImGuiTableFlags_SizingPolicyFixedX | ImGuiTableFlags_NoSavedSettings |
      ImGuiTableFlags_Sortable | ImGuiTableFlags_MultiSortable;

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

  void ChromatogramPlotWidget::draw()
  {
    // Widget's controls - that ImGui does not support natively
    const ImGuiSliderFlags slider_flags = ImGuiSliderFlags_AlwaysClamp;
    float controls_pos_start_y = ImGui::GetCursorPosY();
    ImGui::SliderFloat((std::string("min ") + x_axis_title_).c_str(), &x_min_, range_min_, x_max_, "%.4f", slider_flags);
    ImGui::SameLine();
    ImGui::Checkbox("Compact View", &compact_view_);
    ImGui::SliderFloat((std::string("max ") + x_axis_title_).c_str(), &x_max_, x_min_, range_max_, "%.4f", slider_flags);
    ImGui::SameLine();
    ImGui::Checkbox("Legend", &show_legend_);
    float controls_pos_end_y = ImGui::GetCursorPosY();
    // Main graphic
    float graphic_height = plot_height_ - (controls_pos_end_y - controls_pos_start_y);
    ImPlot::SetNextPlotLimits(x_min_, x_max_, y_min_, y_max_, ImGuiCond_Always);
    ImPlotFlags plotFlags = show_legend_ ? ImPlotFlags_Default | ImPlotFlags_Legend : ImPlotFlags_Default & ~ImPlotFlags_Legend;
    plotFlags |= ImPlotFlags_Crosshairs;
    if (ImPlot::BeginPlot(plot_title_.c_str(), x_axis_title_.c_str(), y_axis_title_.c_str(), ImVec2(plot_width_ - 25, graphic_height - 40), plotFlags)) {
      int i = 0;
      for (const auto& serie_name_scatter : series_names_scatter_)
      {
        assert(x_data_scatter_.at(i).size() == y_data_scatter_.at(i).size());
        ImPlot::PushStyleVar(ImPlotStyleVar_Marker, ImPlotMarker_None);
        ImPlot::PlotLine(serie_name_scatter.c_str(), x_data_scatter_.at(i).data(), y_data_scatter_.at(i).data(), x_data_scatter_.at(i).size());
        ImPlotMarker plot_marker = ImPlotMarker_Circle;
        int feature_index = 0;
        for (int  j = 0; j < x_data_area_.size(); ++j) {
          // Corresponding serie names are supposed to start with same name as the scatter name
          if (series_names_area_.at(j).rfind(serie_name_scatter) == 0)
          {
            assert(x_data_area_.at(j).size() == y_data_area_.at(j).size());
            ImPlot::PushStyleVar(ImPlotStyleVar_Marker, plot_marker);
            std::string legend_text = serie_name_scatter;
            if (!compact_view_)
            {
              legend_text = series_names_area_.at(j) + "::" + std::to_string(feature_index);
            }
            ImPlot::PlotScatter(legend_text.c_str(), x_data_area_.at(j).data(), y_data_area_.at(j).data(), x_data_area_.at(j).size());
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

  void CalibratorsPlotWidget::draw()
  {
    // Main graphic
    ImPlot::SetNextPlotLimits(x_min_, x_max_, y_min_, y_max_, ImGuiCond_Always);
    if (ImPlot::BeginPlot(plot_title_.c_str(), x_axis_title_.c_str(), y_axis_title_.c_str(), ImVec2(plot_width_ - 25, plot_height_ - 40))) {
      for (int i = 0; i < x_raw_data_.size(); ++i) {
        assert(x_raw_data_.at(i).size() == y_raw_data_.at(i).size());
        ImPlot::PushStyleVar(ImPlotStyleVar_Marker, ImPlotMarker_Circle);
        ImPlot::PlotScatter((series_names_.at(i) + "-pts").c_str(), x_raw_data_.at(i).data(), y_raw_data_.at(i).data(), x_raw_data_.at(i).size());
      }
      for (int i = 0; i < x_fit_data_.size(); ++i) {
        assert(x_fit_data_.at(i).size() == y_fit_data_.at(i).size());
        ImPlot::PushStyleVar(ImPlotStyleVar_LineWeight, ImPlot::GetStyle().LineWeight);
        ImPlot::PushStyleVar(ImPlotStyleVar_Marker, ImPlotMarker_Circle);
        ImPlot::PlotLine((series_names_.at(i) + "-fit").c_str(), x_fit_data_.at(i).data(), y_fit_data_.at(i).data(), x_fit_data_.at(i).size());
      }
      ImPlot::EndPlot();
    }
  }

  void Heatmap2DWidget::draw()
  {
    // Main graphic
    if (rows_.size() > 1 || columns_.size() > 1) {
      assert(data_.dimension(0) == rows_.size() && data_.dimension(1) == columns_.size());
      const ImPlotFlags imPlotFlags = ImPlotFlags_MousePos | ImPlotFlags_Highlight | ImPlotFlags_BoxSelect | ImPlotFlags_ContextMenu;
      if (ImPlot::BeginPlot(plot_title_.c_str(), x_axis_title_.c_str(), y_axis_title_.c_str(), ImVec2(plot_width_ - 25, plot_height_ - 40), imPlotFlags)) {
        ImPlot::PlotHeatmap(("##" + plot_title_).c_str(), data_.data(), rows_.size(), columns_.size(), data_min_, data_max_, NULL);
        ImPlot::EndPlot();
      }
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
