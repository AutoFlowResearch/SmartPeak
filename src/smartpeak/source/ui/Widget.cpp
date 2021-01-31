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
  struct ImTableEntry
  {
    std::vector<const char*> Headers;
    
    static const ImGuiTableSortSpecs* s_current_sort_specs;
    
    static bool is_digit(const char ch)
    {
      return ch >= '0' && ch <= '9';
    }
    
    static bool is_number(const std::string& s)
    {
      return !s.empty() && std::find_if(s.begin(), s.end(), [](unsigned char c) { return !std::isdigit(c); }) == s.end();
    }
    
    static bool has_identical_entries(const std::vector<const char*>& col_values)
    {
      return std::find(col_values.begin(), col_values.end(), col_values[0]) != col_values.end();
    }
    
    static int lexicographical_sort(const char* lhs, const char* rhs)
    {
      std::string lhs_str(lhs), rhs_str(rhs);
      std::string lhs_str_lowercased(lhs_str.length(),' ');
      std::string rhs_str_lowercased(rhs_str.length(),' ');
      std::transform(lhs_str.begin(), lhs_str.end(), lhs_str_lowercased.begin(), tolower);
      std::transform(rhs_str.begin(), rhs_str.end(), rhs_str_lowercased.begin(), tolower);
      lhs = lhs_str_lowercased.c_str();
      rhs = rhs_str_lowercased.c_str();
      
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
    }

    static int IMGUI_CDECL CompareWithSortSpecs(const void* lhs, const void* rhs)
    {
      const ImTableEntry* a = (const ImTableEntry*)lhs;
      const ImTableEntry* b = (const ImTableEntry*)rhs;
      for (int n = 0; n < s_current_sort_specs->SpecsCount; n++)
      {
        const ImGuiTableSortSpecsColumn* sort_spec = &s_current_sort_specs->Specs[n];
        int delta = 0;
        size_t column_id = sort_spec->ColumnIndex;
        if (column_id == 0 && !a->Headers.empty() && !b->Headers.empty())
        {
          if (a->Headers[0] != nullptr && b->Headers[0] != nullptr)
          {
            if ( is_number(a->Headers[0]) && is_number(b->Headers[0]))
            {
              delta = (std::stol(a->Headers[0]) - std::stol(b->Headers[0]));
            }
            else
            {
              delta = lexicographical_sort(a->Headers[0], b->Headers[0]);
            }
          }
        }
        else if (column_id > 0 && !a->Headers.empty() && !b->Headers.empty())
        {
          delta = lexicographical_sort(a->Headers[column_id], b->Headers[column_id]);
        }

        if (delta > 0)
          return (sort_spec->SortDirection == ImGuiSortDirection_Ascending) ? +1 : -1;
        if (delta < 0)
          return (sort_spec->SortDirection == ImGuiSortDirection_Ascending) ? -1 : +1;
      }
      
      if (a->Headers[1] != nullptr && b->Headers[1] != nullptr)
        return (strcmp(a->Headers[1], b->Headers[1]));
      else
        return (a->Headers[0] - a->Headers[0]);
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
    
    static ImGuiTableColumnFlags column_0_flags = { ImGuiTableColumnFlags_DefaultSort | ImGuiTableColumnFlags_NoHide };
    static ImGuiTableColumnFlags column_any_flags = { ImGuiTableColumnFlags_NoHide };
    
    static ImGuiTextFilter filter;
    filter.Draw("Find");
    
    static ImVector<ImTableEntry> workflow_table_entries;
    static ImVector<ImTableEntry> sequence_table_entries;
    static ImVector<ImTableEntry> transition_table_entries;
    static ImVector<ImTableEntry> concentration_table_entries;
    static ImVector<ImTableEntry> parameter_table_entries;
    static ImVector<ImTableEntry> spectrum_table_entries;
    static ImVector<ImTableEntry> quantitation_method_table_entries;
    static bool workflow_scanned;
    static bool sequence_scanned;
    static bool transitions_scanned;
    static bool concentrations_scanned;
    static bool parameters_scanned;
    static bool spectrums_scanned;
    static bool quantitation_methods_scanned;
    
    if (columns_.dimension(0) == workflow_table_entries.size())
      workflow_scanned = true;
    else
      workflow_scanned = false;
        
    if (columns_.dimension(0) == sequence_table_entries.size())
      sequence_scanned = true;
    else
      sequence_scanned = false;
    
    if (columns_.dimension(0) == transition_table_entries.size())
      transitions_scanned = true;
    else
      transitions_scanned = false;
    
    if (columns_.dimension(0) == concentration_table_entries.size())
      concentrations_scanned = true;
    else
      concentrations_scanned = false;
    
    if (columns_.dimension(0) == parameter_table_entries.size())
      parameters_scanned = true;
    else
      parameters_scanned = false;
    
    if (columns_.dimension(0) == spectrum_table_entries.size())
      spectrums_scanned = true;
    else
      spectrums_scanned = false;
    
    if (columns_.dimension(0) == quantitation_method_table_entries.size())
      quantitation_methods_scanned = true;
    else
      quantitation_methods_scanned = false;
    
    if (columns_.dimensions().TotalSize() > 0 && table_id_ == "WorkflowMainWindow") {
      const static Eigen::Tensor<std::string,2> columns__(columns_);
      workflow_table_entries.resize(columns__.dimension(0), ImTableEntry());
      if (!workflow_table_entries.empty() && workflow_scanned == false) {
        for (size_t row = 0; row < columns__.dimension(0); ++row) {
          ImTableEntry& Im_table_entry = workflow_table_entries[row];
          Im_table_entry.Headers.resize(columns__.dimension(1));
          for (size_t header_idx = 0; header_idx < columns__.dimension(1); ++header_idx) {
            Im_table_entry.Headers[header_idx] = columns__(row, header_idx).c_str();
          }
        }
        workflow_scanned = true;
      }
    }
    
    if (columns_.dimensions().TotalSize() > 0 && table_id_ == "SequenceMainWindow") {
      const static Eigen::Tensor<std::string,2> columns__(columns_);
      sequence_table_entries.resize(columns__.dimension(0), ImTableEntry());
      if (!sequence_table_entries.empty() && sequence_scanned == false) {
        for (size_t row = 0; row < columns__.dimension(0); ++row) {
          ImTableEntry& Im_table_entry = sequence_table_entries[row];
          Im_table_entry.Headers.resize(columns__.dimension(1));
          for (size_t header_idx = 0; header_idx < columns__.dimension(1); ++header_idx) {
            Im_table_entry.Headers[header_idx] = columns__(row, header_idx).c_str();
          }
        }
        sequence_scanned = true;
      }
    }
    
    if (columns_.dimensions().TotalSize() > 0 && table_id_ == "TransitionsMainWindow") {
      const static Eigen::Tensor<std::string,2> columns__(columns_);
      transition_table_entries.resize(columns__.dimension(0), ImTableEntry());
      if (!transition_table_entries.empty() && transitions_scanned == false) {
        for (size_t row = 0; row < columns__.dimension(0); ++row) {
          ImTableEntry& Im_table_entry = transition_table_entries[row];
          Im_table_entry.Headers.resize(columns__.dimension(1));
          for (size_t header_idx = 0; header_idx < columns__.dimension(1); ++header_idx) {
            Im_table_entry.Headers[header_idx] = columns__(row, header_idx).c_str();
          }
        }
        transitions_scanned = true;
      }
    }
    
    if (columns_.dimensions().TotalSize() > 0 && table_id_ == "StdsConcsMainWindow") {
      const static Eigen::Tensor<std::string,2> columns__(columns_);
      concentration_table_entries.resize(columns__.dimension(0), ImTableEntry());
      if (!concentration_table_entries.empty() && concentrations_scanned == false) {
        for (size_t row = 0; row < columns__.dimension(0); ++row) {
          ImTableEntry& Im_table_entry = concentration_table_entries[row];
          Im_table_entry.Headers.resize(columns__.dimension(1));
          for (size_t header_idx = 0; header_idx < columns__.dimension(1); ++header_idx) {
            Im_table_entry.Headers[header_idx] = columns__(row, header_idx).c_str();
          }
        }
        concentrations_scanned = true;
      }
    }
    
    if (columns_.dimensions().TotalSize() > 0 && table_id_ == "ParametersMainWindow") {
      const static Eigen::Tensor<std::string,2> columns__(columns_);
      parameter_table_entries.resize(columns__.dimension(0), ImTableEntry());
      if (!parameter_table_entries.empty() && parameters_scanned == false) {
        for (size_t row = 0; row < columns__.dimension(0); ++row) {
          ImTableEntry& Im_table_entry = parameter_table_entries[row];
          Im_table_entry.Headers.resize(columns__.dimension(1));
          for (size_t header_idx = 0; header_idx < columns__.dimension(1); ++header_idx) {
            Im_table_entry.Headers[header_idx] = columns__(row, header_idx).c_str();
          }
        }
        parameters_scanned = true;
      }
    }
    
    if (columns_.dimensions().TotalSize() > 0 && table_id_ == "SpectrumMainWindow") {
      const static Eigen::Tensor<std::string,2> columns__(columns_);
      spectrum_table_entries.resize(columns__.dimension(0), ImTableEntry());
      if (!spectrum_table_entries.empty() && spectrums_scanned == false) {
        for (size_t row = 0; row < columns__.dimension(0); ++row) {
          ImTableEntry& Im_table_entry = spectrum_table_entries[row];
          Im_table_entry.Headers.resize(columns__.dimension(1));
          for (size_t header_idx = 0; header_idx < columns__.dimension(1); ++header_idx) {
            Im_table_entry.Headers[header_idx] = columns__(row, header_idx).c_str();
          }
        }
        spectrums_scanned = true;
      }
    }
    
    if (columns_.dimensions().TotalSize() > 0 && table_id_ == "QuantMethodMainWindow") {
      const static Eigen::Tensor<std::string,2> columns__(columns_);
      quantitation_method_table_entries.resize(columns__.dimension(0), ImTableEntry());
      if (!quantitation_method_table_entries.empty() && quantitation_methods_scanned == false) {
        for (size_t row = 0; row < columns__.dimension(0); ++row) {
          ImTableEntry& Im_table_entry = quantitation_method_table_entries[row];
          Im_table_entry.Headers.resize(columns__.dimension(1));
          for (size_t header_idx = 0; header_idx < columns__.dimension(1); ++header_idx) {
            Im_table_entry.Headers[header_idx] = columns__(row, header_idx).c_str();
          }
        }
        quantitation_methods_scanned = true;
      }
    }
    
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
            if (table_id_ == "WorkflowMainWindow")
            {
              if (!filter.PassFilter(workflow_table_entries[row].Headers[1]))
              {
                continue;
              }
            }
            if (table_id_ == "SequenceMainWindow")
            {
              if (!filter.PassFilter(sequence_table_entries[row].Headers[0]) && !filter.PassFilter(sequence_table_entries[row].Headers[1]) &&
                  !filter.PassFilter(sequence_table_entries[row].Headers[2]) && !filter.PassFilter(sequence_table_entries[row].Headers[3]) &&
                  !filter.PassFilter(sequence_table_entries[row].Headers[4]) && !filter.PassFilter(sequence_table_entries[row].Headers[5]) &&
                  !filter.PassFilter(sequence_table_entries[row].Headers[6]) && !filter.PassFilter(sequence_table_entries[row].Headers[8]) &&
                  !filter.PassFilter(sequence_table_entries[row].Headers[9])
                  )
              {
                continue;
              }
            }
            if (table_id_ == "TransitionsMainWindow")
            {
              if (!filter.PassFilter(transition_table_entries[row].Headers[0]) && !filter.PassFilter(transition_table_entries[row].Headers[1]))
              {
                continue;
              }
            }
            if (table_id_ == "StdsConcsMainWindow")
            {
              if (!filter.PassFilter(concentration_table_entries[row].Headers[0]) && !filter.PassFilter(concentration_table_entries[row].Headers[1]) &&
                  !filter.PassFilter(concentration_table_entries[row].Headers[2]) && !filter.PassFilter(concentration_table_entries[row].Headers[5]))
              {
                continue;
              }
            }
            if (table_id_ == "ParametersMainWindow")
            {
              if (!filter.PassFilter(parameter_table_entries[row].Headers[0]) && !filter.PassFilter(parameter_table_entries[row].Headers[1]) &&
                  !filter.PassFilter(parameter_table_entries[row].Headers[2]) && !filter.PassFilter(parameter_table_entries[row].Headers[3])
                  )
              {
                continue;
              }
            }
            if (table_id_ == "SpectrumMainWindow") // not-loaded
            {
              if (!filter.PassFilter(spectrum_table_entries[row].Headers[0]) && !filter.PassFilter(spectrum_table_entries[row].Headers[1]))
              {
                continue;
              }
            }
            if (table_id_ == "QuantMethodMainWindow")
            {
              if (!filter.PassFilter(quantitation_method_table_entries[row].Headers[0]) && !filter.PassFilter(quantitation_method_table_entries[row].Headers[1]) &&
                  !filter.PassFilter(quantitation_method_table_entries[row].Headers[2]) && !filter.PassFilter(quantitation_method_table_entries[row].Headers[3]) &&
                  !filter.PassFilter(quantitation_method_table_entries[row].Headers[5]) && !filter.PassFilter(quantitation_method_table_entries[row].Headers[11])
                  )
              {
                continue;
              }
            }
            
            ImGui::TableNextRow();
            for (size_t col = 0; col < headers_.size(); ++col) {
              if (table_id_ == "WorkflowMainWindow" && workflow_scanned == true && !workflow_table_entries.empty())
              {
                char text_buf[256];
                ImTableEntry* item = &workflow_table_entries[row];
                sprintf(text_buf, "%s", item->Headers[col]);
                ImGui::TableSetColumnIndex(col);
                ImGui::Text("%s", text_buf);
                memset(text_buf, 0, sizeof(text_buf));
              }
              else if (table_id_ == "SequenceMainWindow" && sequence_scanned == true && !sequence_table_entries.empty())
              {
                char text_buf[2048];
                ImTableEntry* item = &sequence_table_entries[row];
                sprintf(text_buf, "%s", item->Headers[col]);
                ImGui::TableSetColumnIndex(col);
                ImGui::Text("%s", text_buf);
                memset(text_buf, 0, sizeof(text_buf));
              }
              else if (table_id_ == "TransitionsMainWindow" && transitions_scanned == true && !transition_table_entries.empty())
              {
                char text_buf[2048];
                ImTableEntry* item = &transition_table_entries[row];
                sprintf(text_buf, "%s", item->Headers[col]);
                ImGui::TableSetColumnIndex(col);
                ImGui::Text("%s", text_buf);
                memset(text_buf, 0, sizeof(text_buf));
              }
              else if (table_id_ == "StdsConcsMainWindow" && concentrations_scanned == true && !concentration_table_entries.empty())
              {
                char text_buf[2048];
                ImTableEntry* item = &concentration_table_entries[row];
                sprintf(text_buf, "%s", item->Headers[col]);
                ImGui::TableSetColumnIndex(col);
                ImGui::Text("%s", text_buf);
                memset(text_buf, 0, sizeof(text_buf));
              }
              else if (table_id_ == "ParametersMainWindow" && parameters_scanned == true && !parameter_table_entries.empty())
              {
                char text_buf[2048];
                ImTableEntry* item = &parameter_table_entries[row];
                sprintf(text_buf, "%s", item->Headers[col]);
                ImGui::TableSetColumnIndex(col);
                ImGui::Text("%s", text_buf);
                memset(text_buf, 0, sizeof(text_buf));
              }
              else if (table_id_ == "SpectrumMainWindow" && spectrums_scanned == true && !spectrum_table_entries.empty())
              {
                char text_buf[2048];
                ImTableEntry* item = &spectrum_table_entries[row];
                sprintf(text_buf, "%s", item->Headers[col]);
                ImGui::TableSetColumnIndex(col);
                ImGui::Text("%s", text_buf);
                memset(text_buf, 0, sizeof(text_buf));
              }
              else if (table_id_ == "QuantMethodMainWindow" && quantitation_methods_scanned == true && !quantitation_method_table_entries.empty())
              {
                char text_buf[2048];
                ImTableEntry* item = &quantitation_method_table_entries[row];
                sprintf(text_buf, "%s", item->Headers[col]);
                ImGui::TableSetColumnIndex(col);
                ImGui::Text("%s", text_buf);
                memset(text_buf, 0, sizeof(text_buf));
              }
            }
          }
        }
      }
      
      if (ImGuiTableSortSpecs* sorts_specs = ImGui::TableGetSortSpecs())
      {
        if (sorts_specs->SpecsDirty && (workflow_scanned == true ) && (table_id_ == "WorkflowMainWindow"))
        {
          ImTableEntry::s_current_sort_specs = sorts_specs;
          if (workflow_table_entries.Size > 1)
              qsort(&workflow_table_entries[0], (size_t)workflow_table_entries.Size, sizeof(workflow_table_entries[0]), ImTableEntry::CompareWithSortSpecs);
          ImTableEntry::s_current_sort_specs = NULL;
          sorts_specs->SpecsDirty = false;
        }
        if (sorts_specs->SpecsDirty && (sequence_scanned == true) && (table_id_ == "SequenceMainWindow"))
        {
          ImTableEntry::s_current_sort_specs = sorts_specs;
          if (sequence_table_entries.Size > 1)
              qsort(&sequence_table_entries[0], (size_t)sequence_table_entries.Size, sizeof(sequence_table_entries[0]), ImTableEntry::CompareWithSortSpecs);
          ImTableEntry::s_current_sort_specs = NULL;
          sorts_specs->SpecsDirty = false;
        }
        if (sorts_specs->SpecsDirty && (transitions_scanned == true) && (table_id_ == "TransitionsMainWindow"))
        {
          ImTableEntry::s_current_sort_specs = sorts_specs;
          if (transition_table_entries.Size > 1)
              qsort(&transition_table_entries[0], (size_t)transition_table_entries.Size, sizeof(transition_table_entries[0]), ImTableEntry::CompareWithSortSpecs);
          ImTableEntry::s_current_sort_specs = NULL;
          sorts_specs->SpecsDirty = false;
        }
        if (sorts_specs->SpecsDirty && (concentrations_scanned == true ) && (table_id_ == "StdsConcsMainWindow"))
        {
          ImTableEntry::s_current_sort_specs = sorts_specs;
          if (concentration_table_entries.Size > 1)
              qsort(&concentration_table_entries[0], (size_t)concentration_table_entries.Size, sizeof(concentration_table_entries[0]), ImTableEntry::CompareWithSortSpecs);
          ImTableEntry::s_current_sort_specs = NULL;
          sorts_specs->SpecsDirty = false;
        }
        if (sorts_specs->SpecsDirty && (parameters_scanned == true ) && (table_id_ == "ParametersMainWindow"))
        {
          ImTableEntry::s_current_sort_specs = sorts_specs;
          if (parameter_table_entries.Size > 1)
              qsort(&parameter_table_entries[0], (size_t)parameter_table_entries.Size, sizeof(parameter_table_entries[0]), ImTableEntry::CompareWithSortSpecs);
          ImTableEntry::s_current_sort_specs = NULL;
          sorts_specs->SpecsDirty = false;
        }
        if (sorts_specs->SpecsDirty && (spectrums_scanned == true) && (table_id_ == "SpectrumMainWindow"))
        {
          ImTableEntry::s_current_sort_specs = sorts_specs;
          if (spectrum_table_entries.Size > 1)
              qsort(&spectrum_table_entries[0], (size_t)spectrum_table_entries.Size, sizeof(spectrum_table_entries[0]), ImTableEntry::CompareWithSortSpecs);
          ImTableEntry::s_current_sort_specs = NULL;
          sorts_specs->SpecsDirty = false;
        }
        if (sorts_specs->SpecsDirty && (quantitation_methods_scanned == true) && (table_id_ == "QuantMethodMainWindow"))
        {
          ImTableEntry::s_current_sort_specs = sorts_specs;
          if (quantitation_method_table_entries.Size > 1)
              qsort(&quantitation_method_table_entries[0], (size_t)quantitation_method_table_entries.Size, sizeof(quantitation_method_table_entries[0]), ImTableEntry::CompareWithSortSpecs);
          ImTableEntry::s_current_sort_specs = NULL;
          sorts_specs->SpecsDirty = false;
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
      const static Eigen::Tensor<std::string,2> columns__(columns_);
      const static Eigen::Tensor<bool,2> checkbox_columns__(checkbox_columns_);
      transition_table_entries.resize(columns__.dimension(0), ImTableEntry());
      if (!transition_table_entries.empty() && transitions_scanned == false) {
        for (size_t row = 0; row < columns__.dimension(0); ++row) {
          ImTableEntry& Im_table_entry = transition_table_entries[row];
          Im_table_entry.Headers.resize(columns__.dimension(1) + checkbox_columns__.dimension(1));
          for (size_t header_idx = 0; header_idx < columns__.dimension(1) + checkbox_columns__.dimension(1); ++header_idx) {
            if (header_idx < columns__.dimension(1))
              Im_table_entry.Headers[header_idx] = columns__(row, header_idx).c_str();
            else if (header_idx < columns__.dimension(1) + checkbox_columns__.dimension(1))
              Im_table_entry.Headers[header_idx] = checkbox_columns__(row,header_idx) == true ? "true" : "false";
          }
        }
        transitions_scanned = true;
      }
    }
    
    if (columns_.dimensions().TotalSize() > 0 && table_id_ == "InjectionsExplorerWindow") {
      const static Eigen::Tensor<std::string,2> columns__(columns_);
      const static Eigen::Tensor<bool,2> checkbox_columns__(checkbox_columns_);
      injection_table_entries.resize(columns__.dimension(0), ImTableEntry());
      if (!injection_table_entries.empty() && injections_scanned == false) {
        for (size_t row = 0; row < columns__.dimension(0); ++row) {
          ImTableEntry& Im_table_entry = injection_table_entries[row];
          Im_table_entry.Headers.resize(columns__.dimension(1) + checkbox_columns__.dimension(1));
          for (size_t header_idx = 0; header_idx < columns__.dimension(1) + checkbox_columns__.dimension(1); ++header_idx) {
            if (header_idx < columns__.dimension(1))
              Im_table_entry.Headers[header_idx] = columns__(row, header_idx).c_str();
            else if (header_idx < columns__.dimension(1) + checkbox_columns__.dimension(1))
              Im_table_entry.Headers[header_idx] = checkbox_columns__(row,header_idx) == true ? "true" : "false";
          }
        }
        injections_scanned = true;
      }
    }
    
    if (columns_.dimensions().TotalSize() > 0 && table_id_ == "FeaturesExplorerWindow") {
      const static Eigen::Tensor<std::string,2> columns__(columns_);
      const static Eigen::Tensor<bool,2> checkbox_columns__(checkbox_columns_);
      feature_table_entries.resize(columns__.dimension(0), ImTableEntry());
      if (!feature_table_entries.empty() && features_scanned == false) {
        for (size_t row = 0; row < columns__.dimension(0); ++row) {
          ImTableEntry& Im_table_entry = feature_table_entries[row];
          Im_table_entry.Headers.resize(columns__.dimension(1) + checkbox_columns__.dimension(1));
          for (size_t header_idx = 0; header_idx < columns__.dimension(1) + checkbox_columns__.dimension(1); ++header_idx) {
            if (header_idx < columns__.dimension(1))
              Im_table_entry.Headers[header_idx] = columns__(row, header_idx).c_str();
            else if (header_idx < columns__.dimension(1) + checkbox_columns__.dimension(1))
              Im_table_entry.Headers[header_idx] = checkbox_columns__(row,header_idx) == true ? "true" : "false";
          }
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
          if (checked_rows_.size() <=0 || (checked_rows_.size() > 0 && checked_rows_(row)))
          {
            if (table_id_ == "FeaturesExplorerWindow")
            {
              if (!filter.PassFilter(feature_table_entries[row].Headers[0]))
              {
                continue;
              }
            }
            if (table_id_ == "InjectionsExplorerWindow")
            {
              if (!filter.PassFilter(injection_table_entries[row].Headers[0]) && !filter.PassFilter(injection_table_entries[row].Headers[1]))
              {
                continue;
              }
            }
            if (table_id_ == "TransitionsExplorerWindow")
            {
              if (!filter.PassFilter(transition_table_entries[row].Headers[0]) && !filter.PassFilter(transition_table_entries[row].Headers[1]))
              {
                continue;
              }
            }
            ImGui::TableNextRow();
            for (size_t col = 0; col < headers_.size(); ++col)
            {
              if (table_id_ == "InjectionsExplorerWindow" && injections_scanned == true && !injection_table_entries.empty())
              {
                char injection_text_buf[256];
                ImTableEntry* item = &injection_table_entries[row];
                sprintf(injection_text_buf, "%s", item->Headers[col]);
                ImGui::TableSetColumnIndex(col);
                ImGui::Text("%s", injection_text_buf);
                memset(injection_text_buf, 0, sizeof(injection_text_buf));
                  
              }
              else if (table_id_ == "TransitionsExplorerWindow" && transitions_scanned == true && !transition_table_entries.empty())
              {
                char transition_text_buf[2048];
                ImTableEntry* item = &transition_table_entries[row];
                sprintf(transition_text_buf, "%s", item->Headers[col]);
                ImGui::TableSetColumnIndex(col);
                ImGui::Text("%s", transition_text_buf);
                memset(transition_text_buf, 0, sizeof(transition_text_buf));
              }
              else if (table_id_ == "FeaturesExplorerWindow" && features_scanned == true && !feature_table_entries.empty())
              {
                char feature_text_buf[256];
                ImTableEntry* item = &feature_table_entries[row];
                sprintf(feature_text_buf, "%s", item->Headers[col]);
                ImGui::TableSetColumnIndex(col);
                ImGui::Text("%s", feature_text_buf);
                memset(feature_text_buf, 0, sizeof(feature_text_buf));
              }
            }
            
            for (size_t col = 0; col < checkbox_headers_.size(); ++col) {
              std::string id = table_id_ + std::to_string(col) + std::to_string(row*columns_.dimension(1));
              ImGui::TableSetColumnIndex(col + headers_.size());
              ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
              if (table_id_ == "InjectionsExplorerWindow" && injections_scanned==true && !injection_table_entries.empty())
              {
                size_t checkbox_idx = col + headers_.size();
                bool is_checked;
                if (!strcmp(injection_table_entries[row].Headers[checkbox_idx], "true"))
                  is_checked = true;
                else
                  is_checked = false;
                ImGui::Checkbox(id.c_str(), &is_checked);
                
                if (is_checked == true && strcmp(injection_table_entries[row].Headers[checkbox_idx], "true"))
                  injection_table_entries[row].Headers[checkbox_idx] = "true";
                else if (is_checked == false && strcmp(injection_table_entries[row].Headers[checkbox_idx], "false"))
                  injection_table_entries[row].Headers[checkbox_idx] = "false";
              }
              else if (table_id_ == "TransitionsExplorerWindow" && transitions_scanned==true && !transition_table_entries.empty())
              {
                size_t checkbox_idx = col + headers_.size();
                bool is_checked;
                if (!strcmp(transition_table_entries[row].Headers[checkbox_idx], "true"))
                  is_checked = true;
                else
                  is_checked = false;
                ImGui::Checkbox(id.c_str(), &is_checked);
                
                if (is_checked == true && strcmp(transition_table_entries[row].Headers[checkbox_idx], "true"))
                  transition_table_entries[row].Headers[checkbox_idx] = "true";
                else if (is_checked == false && strcmp(transition_table_entries[row].Headers[checkbox_idx], "false"))
                  transition_table_entries[row].Headers[checkbox_idx] = "false";
              }
              else if (table_id_ == "FeaturesExplorerWindow" && features_scanned==true && !feature_table_entries.empty())
              {
                size_t checkbox_idx = col + headers_.size();
                bool is_checked;
                if (!strcmp(feature_table_entries[row].Headers[checkbox_idx], "true"))
                  is_checked = true;
                else
                  is_checked = false;
                ImGui::Checkbox(id.c_str(), &is_checked);
                
                if (is_checked == true && strcmp(feature_table_entries[row].Headers[checkbox_idx], "true"))
                  feature_table_entries[row].Headers[checkbox_idx] = "true";
                else if (is_checked == false && strcmp(feature_table_entries[row].Headers[checkbox_idx], "false"))
                  feature_table_entries[row].Headers[checkbox_idx] = "false";
              }
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
