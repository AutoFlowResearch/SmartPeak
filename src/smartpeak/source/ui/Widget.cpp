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

#include <SmartPeak/ui/Widget.h>
#include <algorithm>
#include <map>
#include <string>
#include <utility>
#include <vector>
#include <imgui.h>
#include <imgui_internal.h>
#include <implot.h>

namespace SmartPeak
{
  const ImGuiTableSortSpecs* ImEntry::s_current_sort_specs = NULL;
  const double GenericGraphicWidget::high_value_threeshold_ = 1e20;

  bool GenericTableWidget::searcher(const std::vector<ImEntry>& Im_table_entries, const int& selected_entry,
    const ImGuiTextFilter& filter, const size_t row) const
  {
    bool is_to_filter;
    if (selected_entry > 0) {
      is_to_filter = !filter.PassFilter(Im_table_entries[row].entry_contents[selected_entry - 1].c_str());
    }
    else if (selected_entry == 0) 
    { //ALL
      is_to_filter = std::all_of(Im_table_entries[row].entry_contents.begin(),
        Im_table_entries[row].entry_contents.end(),
        [&filter](auto entry) {return !filter.PassFilter(entry.c_str()); });
    }
    return is_to_filter;
  }

  void GenericTableWidget::updateTableContents(std::vector<ImEntry>& Im_table_entries,
    bool& is_scanned,
    const Eigen::Tensor<std::string, 2>& columns,
    const Eigen::Tensor<bool, 2>& checkbox_columns)
  {
    Im_table_entries.resize(columns.dimension(0), ImEntry());
    if (!Im_table_entries.empty() && is_scanned == false) {
      for (size_t row = 0; row < columns.dimension(0); ++row) {
        ImEntry& Im_table_entry = Im_table_entries[row];
        Im_table_entry.entry_contents.resize(columns.dimension(1) + checkbox_columns.dimension(1), "");
        Im_table_entry.ID = row;
        for (size_t header_idx = 0; header_idx < columns.dimension(1) + checkbox_columns.dimension(1); ++header_idx) {
          if (header_idx < columns.dimension(1)) {
            Im_table_entry.entry_contents[header_idx] = columns(row, header_idx);
          }
          else if (header_idx < columns.dimension(1) + checkbox_columns.dimension(1)) 
          {
            const std::size_t checkbox_idx = header_idx - static_cast<std::size_t>(columns.dimension(1));
            Im_table_entry.entry_contents[header_idx] = checkbox_columns(row, checkbox_idx) == true ? "true" : "false";
          }
        }
      }
      is_scanned = true;
    }
  }

  void GenericTableWidget::sorter(std::vector<ImEntry>& Im_table_entries,
                                  ImGuiTableSortSpecs* sorts_specs, const bool& is_scanned)
  {
    const unsigned int col_idx = static_cast<unsigned int>(sorts_specs->Specs->ColumnIndex);
    if (sorts_specs->SpecsDirty && is_scanned &&
      !std::all_of(Im_table_entries.begin(), Im_table_entries.end(),
        [&, col_idx, Im_table_entries]
    (ImEntry& entry) { return !std::strcmp(entry.entry_contents[col_idx].c_str(), Im_table_entries.begin()->entry_contents[col_idx].c_str()); }))
    {
      ImEntry::s_current_sort_specs = sorts_specs;
      if (Im_table_entries.size() > 1)
        qsort(&Im_table_entries[0], (size_t)Im_table_entries.size(), sizeof(Im_table_entries[0]), ImEntry::CompareWithSortSpecs);
      ImEntry::s_current_sort_specs = NULL;
      sorts_specs->SpecsDirty = false;
    }
  }

  void GenericTableWidget::draw()
  {
    // get data, if getter is provided
    if (session_handler_ && sequence_handler_ && data_getter_)
    {
      session_handler_->setMinimalDataAndFilters(*sequence_handler_);
      (session_handler_->*data_getter_)(*sequence_handler_, table_data_);
      if (data_filter_)
      {
        checked_rows_ = (session_handler_->*data_filter_)(table_data_.body_);
      }
      else
      {
        checked_rows_ = Eigen::Tensor<bool, 1>();
      }
    }

    if (table_data_.headers_.size() <= 0)
      return;

    const ImGuiTableFlags table_flags = ImGuiTableFlags_Resizable | ImGuiTableFlags_Hideable | ImGuiTableFlags_Reorderable |
      ImGuiTableFlags_NoBordersInBody | ImGuiTableFlags_ScrollY | ImGuiTableFlags_NoSavedSettings |
      ImGuiTableFlags_Sortable | ImGuiTableFlags_SortMulti;

    static ImGuiTableColumnFlags column_0_flags = { ImGuiTableColumnFlags_DefaultSort | ImGuiTableColumnFlags_NoHide };
    static ImGuiTableColumnFlags column_any_flags = { ImGuiTableColumnFlags_NoHide };

    static ImGuiTextFilter filter;
    filter.Draw("Find");

    // drop-down list for search field(s)
    cols.resize(table_data_.headers_.size() + 1);
    for (size_t header_name = 0; header_name < table_data_.headers_.size() + 1; ++header_name) {
      if (header_name == 0)
      {
        cols[header_name] = "All";
      }
      else if (header_name > 0)
      {
        cols[header_name] = table_data_.headers_(header_name - 1).c_str();
      }
    }

    ImGui::Combo("In Column(s)", &selected_col, cols.data(), cols.size());

    table_scanned_ = (table_data_.body_.dimension(0) == table_entries_.size() && !data_changed_);

    if (table_data_.body_.dimensions().TotalSize() > 0) {
      updateTableContents(table_entries_, table_scanned_,
        table_data_.body_, Eigen::Tensor<bool, 2>());
    }
    
    if (table_scanned_ && table_entries_.size() > 0) {
      if (table_data_.headers_.size() != table_entries_[0].entry_contents.size()) {
        table_scanned_ = false;
      }
    }
    data_changed_ = false;

    bool edit_cell = false;
    int hovered_col_ = 0;
    int hovered_row_ = 0;
    if (ImGui::BeginTable(table_id_.c_str(), table_data_.headers_.size(), table_flags)) {
      // First row entry_contents
      for (int col = 0; col < table_data_.headers_.size(); col++) {
        ImGui::TableSetupColumn(table_data_.headers_(col).c_str());
      }
      ImGui::TableSetupScrollFreeze(table_data_.headers_.size(), 1);
      ImGui::TableHeadersRow();

      if (table_data_.body_.size() > 0) {
        for (size_t row = 0; row < table_data_.body_.dimension(0); ++row) {
          if (checked_rows_.size() <= 0 || (checked_rows_.size() > 0 && checked_rows_(row))) {

            if (searcher(table_entries_, selected_col, filter, row))
              continue;

            ImGui::TableNextRow();
            for (size_t col = 0; col < table_data_.headers_.size(); ++col) {
              if (table_scanned_ == true && !table_entries_.empty())
              {
                ImGui::TableSetColumnIndex(col);
                ImGui::Text("%s", table_entries_[row].entry_contents[col].c_str());               
                if (ImGui::IsItemHovered())
                {
                  hovered_row_ = row;
                  hovered_col_ = col;
                  if (isEditable(row, col))
                  {
                    ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
                  }
                }
                if (ImGui::IsItemClicked() && isEditable(row, col))
                {
                  edit_cell = true;
                }
              }
            }
          }
        }
      }

      if (ImGuiTableSortSpecs* sorts_specs = ImGui::TableGetSortSpecs())
      {
        sorter(table_entries_, sorts_specs, table_scanned_);
      }

      // we need to call onEdit outside the drawing of the table
      if (edit_cell)
      {
        onEdit(hovered_row_, hovered_col_);
      }
      drawPopups();

      ImGui::EndTable();
    }
  }

  void ExplorerWidget::draw()
  {
    if (table_data_.headers_.size() <= 0)
      return;

    // headers
    const ImGuiTableFlags table_flags = ImGuiTableFlags_Resizable | ImGuiTableFlags_Hideable | ImGuiTableFlags_Reorderable |
      ImGuiTableFlags_NoBordersInBody | ImGuiTableFlags_ScrollY | ImGuiTableFlags_NoSavedSettings |
      ImGuiTableFlags_Sortable | ImGuiTableFlags_SortMulti;

    static ImGuiTableColumnFlags column_0_flags = { ImGuiTableColumnFlags_DefaultSort | ImGuiTableColumnFlags_NoHide };
    static ImGuiTableColumnFlags column_any_flags = { ImGuiTableColumnFlags_NoHide };

    static ImGuiTextFilter filter;
    filter.Draw("Find");

    // drop-down list for search field(s)
    cols.resize(table_data_.headers_.size() + 1);
    for (size_t header_name = 0; header_name < table_data_.headers_.size() + 1; ++header_name) {
      if (header_name == 0)
      {
        cols[header_name] = "All";
      }
      else if (header_name > 0)
      {
        cols[header_name] = table_data_.headers_(header_name - 1).c_str();
      }
    }

    ImGui::Combo("In Column(s)", &selected_col, cols.data(), cols.size());

    if (table_data_.body_.dimension(0) == table_entries_.size())
      table_scanned_ = true;
    else
      table_scanned_ = false;

    if (table_data_.body_.dimensions().TotalSize() > 0) {
      updateTableContents(table_entries_, table_scanned_,
        table_data_.body_, *checkbox_columns_);
    }
    
    if (table_scanned_ && table_entries_.size() > 0) {
      if (table_data_.headers_.size()+checkbox_headers_.size() != table_entries_[0].entry_contents.size()) {
        table_scanned_ = false;
      }
    }

    if (ImGui::BeginTable(table_id_.c_str(), table_data_.headers_.size() + checkbox_headers_.size(), table_flags)) {
      // First row headers
      for (int col = 0; col < table_data_.headers_.size(); col++) {
        ImGui::TableSetupColumn(table_data_.headers_(col).c_str(), col == 0 ? column_0_flags : column_any_flags, -1.0f, col);
      }
      for (int col = 0; col < checkbox_headers_.size(); col++) {
        ImGui::TableSetupColumn(checkbox_headers_(col).c_str());
      }

      ImGui::TableSetupScrollFreeze(table_data_.headers_.size() + checkbox_headers_.size(), 1);
      ImGui::TableHeadersRow();

      if (table_data_.body_.size() > 0) {
        for (size_t row = 0; row < table_data_.body_.dimension(0); ++row) {
          if (checked_rows_.size() <= 0 || (checked_rows_.size() > 0 && checked_rows_(row)))
          {
            if (searcher(table_entries_, selected_col, filter, row))
              continue;

            ImGui::TableNextRow();
            
            for (size_t header_idx = 0; header_idx < table_data_.body_.dimension(1) + checkbox_columns_->dimension(1); ++header_idx)
            {
              if (table_scanned_ == true && !table_entries_.empty())
              {
                if (header_idx < table_data_.body_.dimension(1)) {
                  ImGui::TableSetColumnIndex(header_idx);
                  ImGui::Text("%s", table_entries_[row].entry_contents[header_idx].c_str());
                }
                else if (header_idx < table_data_.body_.dimension(1) + checkbox_columns_->dimension(1))
                {
                  std::string id = table_id_ + std::to_string(header_idx) + std::to_string(row * table_data_.body_.dimension(1));
                  ImGui::TableSetColumnIndex(header_idx);
                  ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
                  
                  std::size_t checkbox_idx = header_idx - static_cast<std::size_t>(table_data_.body_.dimension(1));
                  bool is_checked;
                  if (!std::strcmp(table_entries_[row].entry_contents[header_idx].c_str(), "true"))
                  is_checked = true;
                  else
                  is_checked = false;
                  ImGui::Checkbox(id.c_str(), &is_checked);

                  if (is_checked == true && !std::strcmp(table_entries_[row].entry_contents[header_idx].c_str(), "false"))
                  {
                    table_entries_[row].entry_contents[header_idx] = "true";
                    (*checkbox_columns_)(table_entries_[row].ID, checkbox_idx) = true;
                  }
                  else if (is_checked == false && !std::strcmp(table_entries_[row].entry_contents[header_idx].c_str(), "true"))
                  {
                    table_entries_[row].entry_contents[header_idx] = "false";
                    (*checkbox_columns_)(table_entries_[row].ID, checkbox_idx) = false;
                  }
                  ImGui::PopStyleColor();
                }
              }
            }
          }
        }
      }

      if (ImGuiTableSortSpecs* sorts_specs = ImGui::TableGetSortSpecs())
      {
        sorter(table_entries_, sorts_specs, table_scanned_);
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
    // add some padding
    float border_padding_x = (x_max_ - x_min_) * 0.05f;
    float border_padding_y = (y_max_ - y_min_) * 0.01f;
    ImPlot::SetNextPlotLimits(x_min_- border_padding_x, x_max_+ border_padding_x, y_min_ - border_padding_y, y_max_*1.1f, ImGuiCond_Always);
    const ImPlotFlags imPlotFlags = ImPlotFlags_Legend | ImPlotFlags_Highlight | ImPlotFlags_BoxSelect | ImPlotFlags_ContextMenu;
    const ImPlotAxisFlags imPlotAxisFlagsX = ImPlotAxisFlags_GridLines;
    std::vector<double> ticks_values;
    for (int i = 0; i < x_data_.size(); ++i)
    {
      ticks_values.push_back(static_cast<float>(i));
    }
    ImPlot::SetNextPlotTicksX(ticks_values.data(), x_data_.size());
    bool is_hovered = false;
    ImPlotPoint plot_point;
    ImPlotPoint plot_threshold;
    if (ImPlot::BeginPlot(plot_title_.c_str(),
                          x_axis_title_.c_str(),
                          y_axis_title_.c_str(),
                          ImVec2(width_ - 25, height_ - 40),
                          imPlotFlags,
                          imPlotAxisFlagsX)) {
      for (int i = 0; i < x_data_.dimension(1); ++i) {
        ImPlot::PushStyleVar(ImPlotStyleVar_Marker, ImPlotMarker_Circle);
        Eigen::Tensor<float, 1> x_data = x_data_.chip(i, 1);
        Eigen::Tensor<float, 1> y_data = y_data_.chip(i, 1);
        ImPlot::PlotLine((*series_names_)(i).c_str(), x_data.data(), y_data.data(), x_data_.dimension(0));
      }
      is_hovered = ImPlot::IsPlotHovered();
      if (is_hovered)
      {
        plot_point = ImPlot::GetPlotMousePos();
        // compute hover area
        ImVec2 plot_threshold_pix_zero = {0,0};
        ImPlotPoint plot_threshold_zero = ImPlot::PixelsToPlot(plot_threshold_pix_zero);
        ImVec2 plot_threshold_pix_ref = { 5,5 };
        ImPlotPoint plot_threshold_ref = ImPlot::PixelsToPlot(plot_threshold_pix_ref);
        plot_threshold.x = plot_threshold_ref.x - plot_threshold_zero.x;
        plot_threshold.y = plot_threshold_zero.y - plot_threshold_ref.y;
      }
      ImPlot::EndPlot();
      size_t injection_number = static_cast<size_t>(std::round(plot_point.x));
      if (is_hovered)
      {
        bool tooltip_exists = false;
        if (injection_number >= 0 && injection_number < x_data_.size())
        {
          // see if we are hovering one point
          for (int i = 0; i < x_data_.dimension(1); ++i) {
            Eigen::Tensor<float, 1> y_data = y_data_.chip(i, 1);
            if ((plot_point.y < y_data.data()[injection_number] + plot_threshold.y) && (plot_point.y > y_data.data()[injection_number] - plot_threshold.y) &&
              (plot_point.x < static_cast<float>(injection_number) + plot_threshold.x) && (plot_point.x > static_cast<float>(injection_number) - plot_threshold.x))
            {
              ImGui::BeginTooltip();
              // in case we are hovering multiple points
              if (tooltip_exists)
              {
                ImGui::Separator();
              }
              std::ostringstream os;
              os << "Injection: " << (*x_labels_)(injection_number);
              ImGui::Text(os.str().c_str());
              os.str("");
              os.clear();
              os << "Series: " << (*series_names_)(i);
              ImGui::Text(os.str().c_str());
              os.str("");
              os.clear();
              os << "Value: " << y_data(injection_number);
              ImGui::Text(os.str().c_str());
              ImGui::EndTooltip();
              tooltip_exists = true;
            }
          }
        }
      }
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
    if (chrom_.points_overflow)
    {
      ImGui::Text("Unable to draw: too much points. Please reduce scope or unselect data.");
    }
    else
    {
      float graphic_height = height_ - (controls_pos_end_y - controls_pos_start_y);
      ImPlot::SetNextPlotLimits(current_range_.first, current_range_.second, chrom_.y_min_, chrom_.y_max_, ImGuiCond_Always);
      ImPlotFlags plotFlags = show_legend_ ? ImPlotFlags_Default | ImPlotFlags_Legend : ImPlotFlags_Default & ~ImPlotFlags_Legend;
      plotFlags |= ImPlotFlags_Crosshairs;
      if (ImPlot::BeginPlot(plot_title_.c_str(), chrom_.x_axis_title_.c_str(), chrom_.y_axis_title_.c_str(), ImVec2(width_ - 25, graphic_height - 40), plotFlags)) {
        int i = 0;
        for (const auto& serie_name_scatter : chrom_.series_names_scatter_)
        {
          assert(chrom_.x_data_scatter_.at(i).size() == chrom_.y_data_scatter_.at(i).size());
          ImPlot::PushStyleVar(ImPlotStyleVar_Marker, ImPlotMarker_None);
          ImPlot::PlotLine(serie_name_scatter.c_str(), chrom_.x_data_scatter_.at(i).data(), chrom_.y_data_scatter_.at(i).data(), chrom_.x_data_scatter_.at(i).size());
          ImPlotMarker plot_marker = ImPlotMarker_Circle;
          int feature_index = 0;
          for (int j = 0; j < chrom_.x_data_area_.size(); ++j) {
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
      std::snprintf(label, sizeof label, "MyObject %d", i); // TODO: update for use with sequence list
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
