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
  enum ContentColumnID
  {
    ContentColumnID_ID,
    ContentColumnID_1,
    ContentColumnID_2,
    ContentColumnID_3,
    ContentColumnID_4,
    ContentColumnID_5,
    ContentColumnID_6,
    ContentColumnID_7
  };

  struct ImTableEntry
  {
    //int         Header_ID;
    const char* Header_ID;
    const char* Header_1;
    const char* Header_2;
    const char* Header_3;
    const char* Header_4;
    const char* Header_5;
    const char* Header_6;
    const char* Header_7;
    
    static const ImGuiTableSortSpecs* s_current_sort_specs;
    
    static bool is_digit(const char ch)
    {
      return ch >= '0' && ch <= '9';
    }
    
    static bool is_number(const std::string& s)
    {
      return !s.empty() && std::find_if(s.begin(), s.end(), [](unsigned char c) { return !std::isdigit(c); }) == s.end();
    }
    
    static int lexicographical_sort(const char* lhs, const char* rhs)
    {
      enum SearchMode { STRING, NUMBER } search_mode = STRING;
      while (*lhs && *rhs)
      {
        if (search_mode == STRING)
        {
          char lhs_char, rhs_char;
          while ((lhs_char = *lhs) && (rhs_char = *rhs))
          {
            const bool lhs_digit = is_digit(lhs_char), rhs_digit = is_digit(rhs_char);
            if(lhs_digit && rhs_digit)
            {
              search_mode = NUMBER;
              break;
            }
  
            if(lhs_digit)
              return -1;
  
            if(rhs_digit)
              return +1;
            
            const int char_diff = lhs_char - rhs_char;
            if(char_diff != 0)
              return char_diff;
            
            ++lhs;
            ++rhs;
          }
        }
        else if (search_mode == NUMBER)
        {
          unsigned long lhs_int = 0;
          while (*lhs && is_digit(*lhs))
          {
            lhs_int = lhs_int*10 + *lhs-'0';
            ++lhs;
          }
          unsigned long rhs_int = 0;
          while (*rhs && is_digit(*rhs))
          {
            rhs_int = rhs_int*10 + *rhs-'0';
            ++rhs;
          }
      
          const long int_diff = lhs_int - rhs_int;
          if(int_diff != 0)
            return int_diff;

          search_mode = STRING;
        }
      }
      
      std::string a_str, b_str;
      a_str = lhs;
      b_str = rhs;
      std::string a_str_lowercased(a_str.length(),' ');
      std::string b_str_lowercased(b_str.length(),' ');
      std::transform(a_str.begin(), a_str.end(), a_str_lowercased.begin(), tolower);
      std::transform(b_str.begin(), b_str.end(), b_str_lowercased.begin(), tolower);

      if  (!a_str_lowercased.empty() && !b_str_lowercased.empty() && a_str_lowercased.front()
           == b_str_lowercased.front()) {
        return a_str.compare(b_str);
      }
      else
      {
        // case-insensitive lexicographic sort
        return a_str_lowercased.compare(b_str_lowercased);
      }
    }

    static int IMGUI_CDECL CompareWithSortSpecs(const void* lhs, const void* rhs)
    {
      const ImTableEntry* a = (const ImTableEntry*)lhs;
      const ImTableEntry* b = (const ImTableEntry*)rhs;
      for (int n = 0; n < s_current_sort_specs->SpecsCount; n++)
      {
        const ImGuiTableSortSpecsColumn* sort_spec = &s_current_sort_specs->Specs[n];
        int delta = 0;
        
        auto column_id = sort_spec->ColumnUserID;
        switch (column_id)
        {
          case ContentColumnID_ID:
          {
            if (a->Header_ID != nullptr && b->Header_ID != nullptr) {
              if ( is_number(a->Header_ID) && is_number(b->Header_ID) ) {
                delta = (std::stol(a->Header_ID) - std::stol(b->Header_ID));
              }
              else
              {
                delta = lexicographical_sort(a->Header_ID, b->Header_ID);
              }
            }
            break;
          }

          case ContentColumnID_1:
            delta = lexicographical_sort(a->Header_1, b->Header_1);
            break;
            
          case ContentColumnID_2:
            delta = lexicographical_sort(a->Header_2, b->Header_2);
            break;
            
          case ContentColumnID_3:
            delta = lexicographical_sort(a->Header_3, b->Header_3);
            break;
            
          case ContentColumnID_4:
            delta = lexicographical_sort(a->Header_4, b->Header_4);
            break;
            
          case ContentColumnID_5:
            delta = lexicographical_sort(a->Header_5, b->Header_5);
            break;
            
          case ContentColumnID_6:
            delta = lexicographical_sort(a->Header_6, b->Header_6);
            break;
            
          case ContentColumnID_7:
            delta = lexicographical_sort(a->Header_7, b->Header_7);
            break;
            
          default: IM_ASSERT(0); break;
        }
        if (delta > 0)
          return (sort_spec->SortDirection == ImGuiSortDirection_Ascending) ? +1 : -1;
        if (delta < 0)
          return (sort_spec->SortDirection == ImGuiSortDirection_Ascending) ? -1 : +1;
      }
      
      if (a->Header_1 != nullptr && b->Header_1 != nullptr)
        return (strcmp(a->Header_1, b->Header_1));
      else
        return (a->Header_ID - a->Header_ID);
    }
  };

  const ImGuiTableSortSpecs* ImTableEntry::s_current_sort_specs = NULL;

  void Widget::FilterPopup(
    const char* popuop_id,
    ImGuiTextFilter& filter,
    const Eigen::Tensor<std::string, 1>& column,
    bool* checked,
    const std::vector<std::pair<std::string, std::vector<size_t>>>& values_indices) {
    if (false == ImGui::BeginPopup(popuop_id))
      return;

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

  void ExplorerWidget::draw()
  {
    if (headers_.size()<=0)
      return;

    // headers
    const ImGuiTableFlags table_flags = ImGuiTableFlags_Resizable | ImGuiTableFlags_Hideable | ImGuiTableFlags_Reorderable |
      ImGuiTableFlags_NoBordersInBody | ImGuiTableFlags_Scroll | ImGuiTableFlags_SizingPolicyFixedX | ImGuiTableFlags_NoSavedSettings |
      ImGuiTableFlags_Sortable | ImGuiTableFlags_MultiSortable;
    
    static ImGuiTableColumnFlags column_0_flags = { ImGuiTableColumnFlags_DefaultSort | ImGuiTableColumnFlags_NoHide };
    static ImGuiTableColumnFlags column_any_flags = { ImGuiTableColumnFlags_NoHide };

    static ImGuiTextFilter filter;
    filter.Draw("Find");
    static ImVector<ImTableEntry> injection_table_entries;
    static ImVector<ImTableEntry> transition_table_entries;
    static ImVector<ImTableEntry> feature_table_entries;
    static bool injections_scanned;
    static bool transitions_scanned;
    static bool features_scanned;
        
    if (columns_.dimension(0) == injection_table_entries.size())
      injections_scanned = true;
    else
      injections_scanned = false;
    
    if (columns_.dimension(0) == transition_table_entries.size())
      transitions_scanned = true;
    else
      transitions_scanned = false;
    
    if (columns_.dimension(0) == feature_table_entries.size())
      features_scanned = true;
    else
      features_scanned = false;
    
    if (columns_.dimensions().TotalSize() > 0 && table_id_ == "TransitionsExplorerWindow") {
      transition_table_entries.resize(columns_.dimension(0), ImTableEntry());
      if (!transition_table_entries.empty() && transitions_scanned == false) {
        for (size_t row = 0; row < columns_.dimension(0); ++row) {
          ImTableEntry& Im_table_entry = transition_table_entries[row];
          Im_table_entry.Header_1 = columns_(row, 0).c_str();
          Im_table_entry.Header_2 = columns_(row, 1).c_str();
        }
        transitions_scanned = true;
      }
    }
    
    if (columns_.dimensions().TotalSize() > 0 && table_id_ == "InjectionsExplorerWindow") {
      injection_table_entries.resize(columns_.dimension(0), ImTableEntry());
      if (!injection_table_entries.empty() && injections_scanned == false) {
        for (size_t row = 0; row < columns_.dimension(0); ++row) {
          ImTableEntry& Im_table_entry = injection_table_entries[row];
//          Im_table_entry.Header_ID = std::stol(columns_(row, 0).c_str());
          Im_table_entry.Header_ID = columns_(row, 0).c_str();
          Im_table_entry.Header_1 = columns_(row, 1).c_str();
        }
        injections_scanned = true;
      }
    }
    
    if (columns_.dimensions().TotalSize() > 0 && table_id_ == "FeaturesExplorerWindow") {
      feature_table_entries.resize(columns_.dimension(0), ImTableEntry());
      if (!feature_table_entries.empty() && features_scanned == false) {
        for (size_t row = 0; row < columns_.dimension(0); ++row) {
          ImTableEntry& Im_table_entry = feature_table_entries[row];
          Im_table_entry.Header_ID = columns_(row, 0).c_str();
        }
        features_scanned = true;
      }
    }
    
    if (ImGui::BeginTable(table_id_.c_str(), headers_.size() + checkbox_headers_.size(), table_flags)) {
      // First row headers
      for (int col = 0; col < headers_.size(); col++) {
        ImGui::TableSetupColumn(headers_(col).c_str(), col == 0 ? column_0_flags : column_any_flags, -1.0f, col);
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
            if (table_id_ == "FeaturesExplorerWindow") {
              if (!filter.PassFilter(columns_(row, 0).c_str())) {
                continue;
              }
            }
            if (table_id_ == "InjectionsExplorerWindow" || table_id_ == "TransitionsExplorerWindow") {
              if (!filter.PassFilter(columns_(row, 1).c_str())) {
                continue;
              }
            }
            ImGui::TableNextRow();
            for (size_t col = 0; col < headers_.size(); ++col) {
              if (table_id_ == "InjectionsExplorerWindow" && injections_scanned==true && !injection_table_entries.empty() )
              {
                char injection_text_buf[256];
                ImTableEntry* item = &injection_table_entries[row];
                
                if (col == 0)
                {
                  sprintf(injection_text_buf, "%s", item->Header_ID);
                }
                else if (col == 1 )
                {
                  sprintf(injection_text_buf, "%s", item->Header_1);
                }
                
                ImGui::TableSetColumnIndex(col);
                ImGui::Text("%s", injection_text_buf);
                memset(injection_text_buf, 0, sizeof(injection_text_buf));
                  
              }
              else if (table_id_ == "TransitionsExplorerWindow" && transitions_scanned == true && !transition_table_entries.empty())
              {
                char transition_text_buf[2048];
                ImTableEntry* item = &transition_table_entries[row];
                  
                if (col == 0)
                {
                  sprintf(transition_text_buf, "%s", item->Header_1);
                }
                else if (col == 1 )
                {
                  sprintf(transition_text_buf, "%s", item->Header_2);
                }

                ImGui::TableSetColumnIndex(col);
                ImGui::Text("%s", transition_text_buf);
                memset(transition_text_buf, 0, sizeof(transition_text_buf));
              }
              else if (table_id_ == "FeaturesExplorerWindow" && features_scanned == true && !feature_table_entries.empty())
              {
                char feature_text_buf[256];
                ImTableEntry* item = &feature_table_entries[row];
                  
                if (col == 0)
                {
                  sprintf(feature_text_buf, "%s", item->Header_ID);
                }

                ImGui::TableSetColumnIndex(col);
                ImGui::Text("%s", feature_text_buf);
                memset(feature_text_buf, 0, sizeof(feature_text_buf));
              }
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

      if (ImGuiTableSortSpecs* sorts_specs = ImGui::TableGetSortSpecs())
      {
        if (sorts_specs->SpecsDirty && (injections_scanned == true ) && (table_id_ == "InjectionsExplorerWindow"))
        {
          ImTableEntry::s_current_sort_specs = sorts_specs;
          if (injection_table_entries.Size > 1)
              qsort(&injection_table_entries[0], (size_t)injection_table_entries.Size, sizeof(injection_table_entries[0]), ImTableEntry::CompareWithSortSpecs);
          ImTableEntry::s_current_sort_specs = NULL;
          sorts_specs->SpecsDirty = false;
        }
        if (sorts_specs->SpecsDirty && (transitions_scanned == true) && (table_id_ == "TransitionsExplorerWindow"))
        {
          ImTableEntry::s_current_sort_specs = sorts_specs;
          if (transition_table_entries.Size > 1)
              qsort(&transition_table_entries[0], (size_t)transition_table_entries.Size, sizeof(transition_table_entries[0]), ImTableEntry::CompareWithSortSpecs);
          ImTableEntry::s_current_sort_specs = NULL;
          sorts_specs->SpecsDirty = false;
        }
        if (sorts_specs->SpecsDirty && (features_scanned == true) && (table_id_ == "FeaturesExplorerWindow"))
        {
          ImTableEntry::s_current_sort_specs = sorts_specs;
          if (feature_table_entries.Size > 1)
              qsort(&feature_table_entries[0], (size_t)feature_table_entries.Size, sizeof(feature_table_entries[0]), ImTableEntry::CompareWithSortSpecs);
          ImTableEntry::s_current_sort_specs = NULL;
          sorts_specs->SpecsDirty = false;
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
    // Main graphic
    ImPlot::SetNextPlotLimits(x_min_, x_max_, y_min_, y_max_, ImGuiCond_Always);
    if (ImPlot::BeginPlot(plot_title_.c_str(), x_axis_title_.c_str(), y_axis_title_.c_str(), ImVec2(plot_width_ - 25, plot_height_ - 40))) {
      for (int i = 0; i < x_data_scatter_.size(); ++i) {
        assert(x_data_scatter_.at(i).size() == y_data_scatter_.at(i).size());
        ImPlot::PushStyleVar(ImPlotStyleVar_Marker, ImPlotMarker_None);
        ImPlot::PlotLine(series_names_scatter_.at(i).c_str(), x_data_scatter_.at(i).data(), y_data_scatter_.at(i).data(), x_data_scatter_.at(i).size());
      }
      for (int i = 0; i < x_data_area_.size(); ++i) {
        assert(x_data_area_.at(i).size() == y_data_area_.at(i).size());
        ImPlot::PushStyleVar(ImPlotStyleVar_Marker, ImPlotMarker_Circle);
        ImPlot::PlotScatter(series_names_area_.at(i).c_str(), x_data_area_.at(i).data(), y_data_area_.at(i).data(), x_data_area_.at(i).size());
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
