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
// $Authors: Douglas McCloskey, Ahmed Khalil, Pasquale Domenico Colaianni $
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
  struct ImTableEntry
  {
    int ID;
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
        return (std::strcmp(a->Headers[1], b->Headers[1]));
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
    
    // drop-down list for search field(s)
    static int selected_col = 0;
    static std::vector<const char*> cols;
    cols.resize(headers_.size() + 1);
    for (size_t header_name = 0; header_name < headers_.size() + 1; ++header_name) {
      if (header_name == 0)
      {
        cols[header_name] = "All";
      }
      else if (header_name > 0)
      {
        cols[header_name] = headers_(header_name - 1).c_str();
      }
    }
    
    ImGui::Combo("In Column(s)", &selected_col, cols.data(), cols.size() );
    
    static ImVector<ImTableEntry> sequence_table_entries;
    static ImVector<ImTableEntry> transition_table_entries;
    static ImVector<ImTableEntry> concentration_table_entries;
    static ImVector<ImTableEntry> parameter_table_entries;
    static ImVector<ImTableEntry> spectrum_table_entries;
    static ImVector<ImTableEntry> quantitation_method_table_entries;
    static ImVector<ImTableEntry> component_filters_table_entries;
    static ImVector<ImTableEntry> component_group_filters_table_entries;
    static ImVector<ImTableEntry> component_qcs_table_entries;
    static ImVector<ImTableEntry> component_group_qcs_table_entries;
    static ImVector<ImTableEntry> features_table_entries;
    static ImVector<ImTableEntry> component_rsd_filters_table_entries;
    static ImVector<ImTableEntry> component_group_rsd_filter_table_entries;
    static ImVector<ImTableEntry> component_rsd_qcs_table_entries;
    static ImVector<ImTableEntry> component_group_rsd_qcs_table_entries;
    static ImVector<ImTableEntry> component_background_filter_table_entries;
    static ImVector<ImTableEntry> component_group_background_filter_table_entries;
    static ImVector<ImTableEntry> component_background_qcs_table_entries;
    static ImVector<ImTableEntry> component_group_background_qcs_table_entries;
    static ImVector<ImTableEntry> component_rsd_estimations_table_entries;
    static ImVector<ImTableEntry> component_group_rsd_estimations_table_entries;
    static ImVector<ImTableEntry> component_background_estimations_table_entries;
    static ImVector<ImTableEntry> component_group_background_estimations_table_entries;
    static ImVector<ImTableEntry> feature_matrix_entries;
    
    static bool sequence_scanned;
    static bool transitions_scanned;
    static bool concentrations_scanned;
    static bool parameters_scanned;
    static bool spectrums_scanned;
    static bool quantitation_methods_scanned;
    static bool component_filters_scanned;
    static bool component_group_filters_scanned;
    static bool component_qcs_scanned;
    static bool component_group_qcs_scanned;
    static bool features_scanned;
    static bool component_rsd_filters_scanned;
    static bool component_group_rsd_filter_scanned;
    static bool component_rsd_qcs_scanned;
    static bool component_group_rsd_qcs_scanned;
    static bool component_background_filter_scanned;
    static bool component_group_background_filter_scanned;
    static bool component_background_qcs_scanned;
    static bool component_group_background_qcs_scanned;
    static bool component_rsd_estimations_scanned;
    static bool component_group_rsd_estimations_scanned;
    static bool component_background_estimations_scanned;
    static bool component_group_background_estimations_scanned;
    static bool feature_matrix_scanned;
        
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
    
    if (columns_.dimension(0) == component_filters_table_entries.size())
      component_filters_scanned = true;
    else
      component_filters_scanned = false;
    
    if (columns_.dimension(0) == component_group_filters_table_entries.size())
      component_group_filters_scanned = true;
    else
      component_group_filters_scanned = false;
    
    if (columns_.dimension(0) == component_qcs_table_entries.size())
      component_qcs_scanned = true;
    else
      component_qcs_scanned = false;
    
    if (columns_.dimension(0) == component_group_qcs_table_entries.size())
      component_group_qcs_scanned = true;
    else
      component_group_qcs_scanned = false;
    
    if (columns_.dimension(0) == features_table_entries.size())
      features_scanned = true;
    else
      features_scanned = false;
    
    if (columns_.dimension(0) == component_rsd_filters_table_entries.size())
      component_rsd_filters_scanned = true;
    else
      component_rsd_filters_scanned = false;
    
    if (columns_.dimension(0) == component_group_rsd_filter_table_entries.size())
      component_group_rsd_filter_scanned = true;
    else
      component_group_rsd_filter_scanned = false;
    
    if (columns_.dimension(0) == component_rsd_qcs_table_entries.size())
      component_rsd_qcs_scanned = true;
    else
      component_rsd_qcs_scanned = false;
    
    if (columns_.dimension(0) == component_group_rsd_qcs_table_entries.size())
      component_group_rsd_qcs_scanned = true;
    else
      component_group_rsd_qcs_scanned = false;
    
    if (columns_.dimension(0) == component_background_filter_table_entries.size())
      component_background_filter_scanned = true;
    else
      component_background_filter_scanned = false;
    
    if (columns_.dimension(0) == component_group_background_filter_table_entries.size())
      component_group_background_filter_scanned = true;
    else
      component_group_background_filter_scanned = false;
    
    if (columns_.dimension(0) == component_background_qcs_table_entries.size())
      component_background_qcs_scanned = true;
    else
      component_background_qcs_scanned = false;
    
    if (columns_.dimension(0) == component_group_background_qcs_table_entries.size())
      component_group_background_qcs_scanned = true;
    else
      component_group_background_qcs_scanned = false;
    
    if (columns_.dimension(0) == component_rsd_estimations_table_entries.size())
      component_rsd_estimations_scanned = true;
    else
      component_rsd_estimations_scanned = false;
    
    if (columns_.dimension(0) == component_group_rsd_estimations_table_entries.size())
      component_group_rsd_estimations_scanned = true;
    else
      component_group_rsd_estimations_scanned = false;
    
    if (columns_.dimension(0) == component_background_estimations_table_entries.size())
      component_background_estimations_scanned = true;
    else
      component_background_estimations_scanned = false;
    
    if (columns_.dimension(0) == component_group_background_estimations_table_entries.size())
      component_group_background_estimations_scanned = true;
    else
      component_group_background_estimations_scanned = false;
    
    if (columns_.dimension(0) == feature_matrix_entries.size())
      feature_matrix_scanned = true;
    else
      feature_matrix_scanned = false;
    
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
    
    if (columns_.dimensions().TotalSize() > 0 && table_id_ == "CompFiltersMainWindow") {
      const static Eigen::Tensor<std::string,2> columns__(columns_);
      component_filters_table_entries.resize(columns__.dimension(0), ImTableEntry());
      if (!component_filters_table_entries.empty() && component_filters_scanned == false) {
        for (size_t row = 0; row < columns__.dimension(0); ++row) {
          ImTableEntry& Im_table_entry = component_filters_table_entries[row];
          Im_table_entry.Headers.resize(columns__.dimension(1));
          for (size_t header_idx = 0; header_idx < columns__.dimension(1); ++header_idx) {
            Im_table_entry.Headers[header_idx] = columns__(row, header_idx).c_str();
          }
        }
        component_filters_scanned = true;
      }
    }
    
    if (columns_.dimensions().TotalSize() > 0 && table_id_ == "CompGroupFiltersMainWindow") {
      const static Eigen::Tensor<std::string,2> columns__(columns_);
      component_group_filters_table_entries.resize(columns__.dimension(0), ImTableEntry());
      if (!component_group_filters_table_entries.empty() && component_group_filters_scanned == false) {
        for (size_t row = 0; row < columns__.dimension(0); ++row) {
          ImTableEntry& Im_table_entry = component_group_filters_table_entries[row];
          Im_table_entry.Headers.resize(columns__.dimension(1));
          for (size_t header_idx = 0; header_idx < columns__.dimension(1); ++header_idx) {
            Im_table_entry.Headers[header_idx] = columns__(row, header_idx).c_str();
          }
        }
        component_group_filters_scanned = true;
      }
    }
    
    if (columns_.dimensions().TotalSize() > 0 && table_id_ == "CompQCsMainWindow") {
      const static Eigen::Tensor<std::string,2> columns__(columns_);
      component_qcs_table_entries.resize(columns__.dimension(0), ImTableEntry());
      if (!component_qcs_table_entries.empty() && component_qcs_scanned == false) {
        for (size_t row = 0; row < columns__.dimension(0); ++row) {
          ImTableEntry& Im_table_entry = component_qcs_table_entries[row];
          Im_table_entry.Headers.resize(columns__.dimension(1));
          for (size_t header_idx = 0; header_idx < columns__.dimension(1); ++header_idx) {
            Im_table_entry.Headers[header_idx] = columns__(row, header_idx).c_str();
          }
        }
        component_qcs_scanned = true;
      }
    }
    
    if (columns_.dimensions().TotalSize() > 0 && table_id_ == "CompGroupQCsMainWindow") {
      const static Eigen::Tensor<std::string,2> columns__(columns_);
      component_group_qcs_table_entries.resize(columns__.dimension(0), ImTableEntry());
      if (!component_group_qcs_table_entries.empty() && component_group_qcs_scanned == false) {
        for (size_t row = 0; row < columns__.dimension(0); ++row) {
          ImTableEntry& Im_table_entry = component_group_qcs_table_entries[row];
          Im_table_entry.Headers.resize(columns__.dimension(1));
          for (size_t header_idx = 0; header_idx < columns__.dimension(1); ++header_idx) {
            Im_table_entry.Headers[header_idx] = columns__(row, header_idx).c_str();
          }
        }
        component_group_qcs_scanned = true;
      }
    }
    
    if (columns_.dimensions().TotalSize() > 0 && table_id_ == "featuresTableMainWindow") {
      const static Eigen::Tensor<std::string,2> columns__(columns_);
      features_table_entries.resize(columns__.dimension(0), ImTableEntry());
      if (!features_table_entries.empty() && features_scanned == false) {
        for (size_t row = 0; row < columns__.dimension(0); ++row) {
          ImTableEntry& Im_table_entry = features_table_entries[row];
          Im_table_entry.Headers.resize(columns__.dimension(1));
          for (size_t header_idx = 0; header_idx < columns__.dimension(1); ++header_idx) {
            Im_table_entry.Headers[header_idx] = columns__(row, header_idx).c_str();
          }
        }
        features_scanned = true;
      }
    }
    
    if (columns_.dimensions().TotalSize() > 0 && table_id_ == "CompRSDFiltersMainWindow") {
      const static Eigen::Tensor<std::string,2> columns__(columns_);
      component_rsd_filters_table_entries.resize(columns__.dimension(0), ImTableEntry());
      if (!component_rsd_filters_table_entries.empty() && component_rsd_filters_scanned == false) {
        for (size_t row = 0; row < columns__.dimension(0); ++row) {
          ImTableEntry& Im_table_entry = component_rsd_filters_table_entries[row];
          Im_table_entry.Headers.resize(columns__.dimension(1));
          for (size_t header_idx = 0; header_idx < columns__.dimension(1); ++header_idx) {
            Im_table_entry.Headers[header_idx] = columns__(row, header_idx).c_str();
          }
        }
        component_rsd_filters_scanned = true;
      }
    }
    
    if (columns_.dimensions().TotalSize() > 0 && table_id_ == "CompGroupRSDFiltersMainWindow") {
      const static Eigen::Tensor<std::string,2> columns__(columns_);
      component_group_rsd_filter_table_entries.resize(columns__.dimension(0), ImTableEntry());
      if (!component_group_rsd_filter_table_entries.empty() && component_group_rsd_filter_scanned == false) {
        for (size_t row = 0; row < columns__.dimension(0); ++row) {
          ImTableEntry& Im_table_entry = component_group_rsd_filter_table_entries[row];
          Im_table_entry.Headers.resize(columns__.dimension(1));
          for (size_t header_idx = 0; header_idx < columns__.dimension(1); ++header_idx) {
            Im_table_entry.Headers[header_idx] = columns__(row, header_idx).c_str();
          }
        }
        component_group_rsd_filter_scanned = true;
      }
    }
    
    if (columns_.dimensions().TotalSize() > 0 && table_id_ == "CompRSDQCsMainWindow") {
      const static Eigen::Tensor<std::string,2> columns__(columns_);
      component_rsd_qcs_table_entries.resize(columns__.dimension(0), ImTableEntry());
      if (!component_rsd_qcs_table_entries.empty() && component_rsd_qcs_scanned == false) {
        for (size_t row = 0; row < columns__.dimension(0); ++row) {
          ImTableEntry& Im_table_entry = component_rsd_qcs_table_entries[row];
          Im_table_entry.Headers.resize(columns__.dimension(1));
          for (size_t header_idx = 0; header_idx < columns__.dimension(1); ++header_idx) {
            Im_table_entry.Headers[header_idx] = columns__(row, header_idx).c_str();
          }
        }
        component_rsd_qcs_scanned = true;
      }
    }
    
    if (columns_.dimensions().TotalSize() > 0 && table_id_ == "CompGroupRSDQCsMainWindow") {
      const static Eigen::Tensor<std::string,2> columns__(columns_);
      component_group_rsd_qcs_table_entries.resize(columns__.dimension(0), ImTableEntry());
      if (!component_group_rsd_qcs_table_entries.empty() && component_group_rsd_qcs_scanned == false) {
        for (size_t row = 0; row < columns__.dimension(0); ++row) {
          ImTableEntry& Im_table_entry = component_group_rsd_qcs_table_entries[row];
          Im_table_entry.Headers.resize(columns__.dimension(1));
          for (size_t header_idx = 0; header_idx < columns__.dimension(1); ++header_idx) {
            Im_table_entry.Headers[header_idx] = columns__(row, header_idx).c_str();
          }
        }
        component_group_rsd_qcs_scanned = true;
      }
    }
    
    if (columns_.dimensions().TotalSize() > 0 && table_id_ == "CompBackgroundFiltersMainWindow") {
      const static Eigen::Tensor<std::string,2> columns__(columns_);
      component_background_filter_table_entries.resize(columns__.dimension(0), ImTableEntry());
      if (!component_background_filter_table_entries.empty() && component_background_filter_scanned == false) {
        for (size_t row = 0; row < columns__.dimension(0); ++row) {
          ImTableEntry& Im_table_entry = component_background_filter_table_entries[row];
          Im_table_entry.Headers.resize(columns__.dimension(1));
          for (size_t header_idx = 0; header_idx < columns__.dimension(1); ++header_idx) {
            Im_table_entry.Headers[header_idx] = columns__(row, header_idx).c_str();
          }
        }
        component_background_filter_scanned = true;
      }
    }
    
    if (columns_.dimensions().TotalSize() > 0 && table_id_ == "CompGroupBackgroundFiltersMainWindow") {
      const static Eigen::Tensor<std::string,2> columns__(columns_);
      component_group_background_filter_table_entries.resize(columns__.dimension(0), ImTableEntry());
      if (!component_group_background_filter_table_entries.empty() && component_group_background_filter_scanned == false) {
        for (size_t row = 0; row < columns__.dimension(0); ++row) {
          ImTableEntry& Im_table_entry = component_group_background_filter_table_entries[row];
          Im_table_entry.Headers.resize(columns__.dimension(1));
          for (size_t header_idx = 0; header_idx < columns__.dimension(1); ++header_idx) {
            Im_table_entry.Headers[header_idx] = columns__(row, header_idx).c_str();
          }
        }
        component_group_background_filter_scanned = true;
      }
    }
    
    if (columns_.dimensions().TotalSize() > 0 && table_id_ == "CompBackgroundQCsMainWindow") {
      const static Eigen::Tensor<std::string,2> columns__(columns_);
      component_background_qcs_table_entries.resize(columns__.dimension(0), ImTableEntry());
      if (!component_background_qcs_table_entries.empty() && component_background_qcs_scanned == false) {
        for (size_t row = 0; row < columns__.dimension(0); ++row) {
          ImTableEntry& Im_table_entry = component_background_qcs_table_entries[row];
          Im_table_entry.Headers.resize(columns__.dimension(1));
          for (size_t header_idx = 0; header_idx < columns__.dimension(1); ++header_idx) {
            Im_table_entry.Headers[header_idx] = columns__(row, header_idx).c_str();
          }
        }
        component_background_qcs_scanned = true;
      }
    }
    
    if (columns_.dimensions().TotalSize() > 0 && table_id_ == "CompGroupBackgroundQCsMainWindow") {
      const static Eigen::Tensor<std::string,2> columns__(columns_);
      component_group_background_qcs_table_entries.resize(columns__.dimension(0), ImTableEntry());
      if (!component_group_background_qcs_table_entries.empty() && component_group_background_qcs_scanned == false) {
        for (size_t row = 0; row < columns__.dimension(0); ++row) {
          ImTableEntry& Im_table_entry = component_group_background_qcs_table_entries[row];
          Im_table_entry.Headers.resize(columns__.dimension(1));
          for (size_t header_idx = 0; header_idx < columns__.dimension(1); ++header_idx) {
            Im_table_entry.Headers[header_idx] = columns__(row, header_idx).c_str();
          }
        }
        component_group_background_qcs_scanned = true;
      }
    }
    
    if (columns_.dimensions().TotalSize() > 0 && table_id_ == "CompRSDEstimationsMainWindow") {
      const static Eigen::Tensor<std::string,2> columns__(columns_);
      component_rsd_estimations_table_entries.resize(columns__.dimension(0), ImTableEntry());
      if (!component_rsd_estimations_table_entries.empty() && component_rsd_estimations_scanned == false) {
        for (size_t row = 0; row < columns__.dimension(0); ++row) {
          ImTableEntry& Im_table_entry = component_rsd_estimations_table_entries[row];
          Im_table_entry.Headers.resize(columns__.dimension(1));
          for (size_t header_idx = 0; header_idx < columns__.dimension(1); ++header_idx) {
            Im_table_entry.Headers[header_idx] = columns__(row, header_idx).c_str();
          }
        }
        component_rsd_estimations_scanned = true;
      }
    }
    
    if (columns_.dimensions().TotalSize() > 0 && table_id_ == "CompGroupRSDEstimationsMainWindow") {
      const static Eigen::Tensor<std::string,2> columns__(columns_);
      component_group_rsd_estimations_table_entries.resize(columns__.dimension(0), ImTableEntry());
      if (!component_group_rsd_estimations_table_entries.empty() && component_group_rsd_estimations_scanned == false) {
        for (size_t row = 0; row < columns__.dimension(0); ++row) {
          ImTableEntry& Im_table_entry = component_group_rsd_estimations_table_entries[row];
          Im_table_entry.Headers.resize(columns__.dimension(1));
          for (size_t header_idx = 0; header_idx < columns__.dimension(1); ++header_idx) {
            Im_table_entry.Headers[header_idx] = columns__(row, header_idx).c_str();
          }
        }
        component_group_rsd_estimations_scanned = true;
      }
    }
    
    if (columns_.dimensions().TotalSize() > 0 && table_id_ == "CompBackgroundEstimationsMainWindow") {
      const static Eigen::Tensor<std::string,2> columns__(columns_);
      component_background_estimations_table_entries.resize(columns__.dimension(0), ImTableEntry());
      if (!component_background_estimations_table_entries.empty() && component_background_estimations_scanned == false) {
        for (size_t row = 0; row < columns__.dimension(0); ++row) {
          ImTableEntry& Im_table_entry = component_background_estimations_table_entries[row];
          Im_table_entry.Headers.resize(columns__.dimension(1));
          for (size_t header_idx = 0; header_idx < columns__.dimension(1); ++header_idx) {
            Im_table_entry.Headers[header_idx] = columns__(row, header_idx).c_str();
          }
        }
        component_background_estimations_scanned = true;
      }
    }
    
    if (columns_.dimensions().TotalSize() > 0 && table_id_ == "CompGroupBackgroundEstimationsMainWindow") {
      const static Eigen::Tensor<std::string,2> columns__(columns_);
      component_group_background_estimations_table_entries.resize(columns__.dimension(0), ImTableEntry());
      if (!component_group_background_estimations_table_entries.empty() && component_group_background_estimations_scanned == false) {
        for (size_t row = 0; row < columns__.dimension(0); ++row) {
          ImTableEntry& Im_table_entry = component_group_background_estimations_table_entries[row];
          Im_table_entry.Headers.resize(columns__.dimension(1));
          for (size_t header_idx = 0; header_idx < columns__.dimension(1); ++header_idx) {
            Im_table_entry.Headers[header_idx] = columns__(row, header_idx).c_str();
          }
        }
        component_group_background_estimations_scanned = true;
      }
    }
    
    if (columns_.dimensions().TotalSize() > 0 && table_id_ == "featureMatrixMainWindow") {
      const static Eigen::Tensor<std::string,2> columns__(columns_);
      feature_matrix_entries.resize(columns__.dimension(0), ImTableEntry());
      if (!feature_matrix_entries.empty() && feature_matrix_scanned == false) {
        for (size_t row = 0; row < columns__.dimension(0); ++row) {
          ImTableEntry& Im_table_entry = feature_matrix_entries[row];
          Im_table_entry.Headers.resize(columns__.dimension(1));
          for (size_t header_idx = 0; header_idx < columns__.dimension(1); ++header_idx) {
            Im_table_entry.Headers[header_idx] = columns__(row, header_idx).c_str();
          }
        }
        feature_matrix_scanned = true;
      }
    }
    
    if (ImGui::BeginTable(table_id_.c_str(), headers_.size(), table_flags)) {
      // First row headers
      for (int col = 0; col < headers_.size(); col++) {
        ImGui::TableSetupColumn(headers_(col).c_str());
      }
      ImGui::TableSetupScrollFreeze(headers_.size(), 1);
      ImGui::TableHeadersRow();
      
      if (columns_.size() > 0) {
        for (size_t row = 0; row < columns_.dimension(0); ++row) {
          if (checked_rows_.size() <= 0 || (checked_rows_.size() > 0 && checked_rows_(row))) {
            if (table_id_ == "SequenceMainWindow")
            {
              bool is_to_filter;
              if (selected_col > 0)
              {
                is_to_filter = !filter.PassFilter(sequence_table_entries[row].Headers[selected_col - 1]);
              }
              else if (selected_col == 0) //ALL
              {
                is_to_filter = std::all_of(sequence_table_entries[row].Headers.begin(),
                                           sequence_table_entries[row].Headers.end(),
                                           [](const char* entry){return !filter.PassFilter(entry);});
              }
              
              if (is_to_filter)
              {
                continue;
              }
            }
            
            if (table_id_ == "TransitionsMainWindow")
            {
              bool is_to_filter;
              if (selected_col > 0)
              {
                is_to_filter = !filter.PassFilter(transition_table_entries[row].Headers[selected_col - 1]);
              }
              else if (selected_col == 0) //ALL
              {
                is_to_filter = std::all_of(transition_table_entries[row].Headers.begin(),
                                           transition_table_entries[row].Headers.end(),
                                           [](const char* entry){return !filter.PassFilter(entry);});
              }
              
              if (is_to_filter)
              {
                continue;
              }
            }
            
            if (table_id_ == "StdsConcsMainWindow")
            {
              bool is_to_filter;
              if (selected_col > 0)
              {
                is_to_filter = !filter.PassFilter(concentration_table_entries[row].Headers[selected_col - 1]);
              }
              else if (selected_col == 0) //ALL
              {
                is_to_filter = std::all_of(concentration_table_entries[row].Headers.begin(),
                                           concentration_table_entries[row].Headers.end(),
                                           [](const char* entry){return !filter.PassFilter(entry);});
              }
              
              if (is_to_filter)
              {
                continue;
              }
            }
            
            if (table_id_ == "ParametersMainWindow")
            {
              bool is_to_filter;
              if (selected_col > 0)
              {
                is_to_filter = !filter.PassFilter(parameter_table_entries[row].Headers[selected_col - 1]);
              }
              else if (selected_col == 0) //ALL
              {
                is_to_filter = std::all_of(parameter_table_entries[row].Headers.begin(),
                                           parameter_table_entries[row].Headers.end(),
                                           [](const char* entry){return !filter.PassFilter(entry);});
              }
              
              if (is_to_filter)
              {
                continue;
              }
            }
            
            if (table_id_ == "SpectrumMainWindow") // not-loaded
            {
              bool is_to_filter;
              if (selected_col > 0)
              {
                is_to_filter = !filter.PassFilter(spectrum_table_entries[row].Headers[selected_col - 1]);
              }
              else if (selected_col == 0) //ALL
              {
                is_to_filter = std::all_of(spectrum_table_entries[row].Headers.begin(),
                                           spectrum_table_entries[row].Headers.end(),
                                           [](const char* entry){return !filter.PassFilter(entry);});
              }
              
              if (is_to_filter)
              {
                continue;
              }
            }
            
            if (table_id_ == "QuantMethodMainWindow")
            {
              bool is_to_filter;
              if (selected_col > 0)
              {
                is_to_filter = !filter.PassFilter(quantitation_method_table_entries[row].Headers[selected_col - 1]);
              }
              else if (selected_col == 0) //ALL
              {
                is_to_filter = std::all_of(quantitation_method_table_entries[row].Headers.begin(),
                                           quantitation_method_table_entries[row].Headers.end(),
                                           [](const char* entry){return !filter.PassFilter(entry);});
              }
              
              if (is_to_filter)
              {
                continue;
              }
            }
            
            if (table_id_ == "CompFiltersMainWindow")
            {
              bool is_to_filter;
              if (selected_col > 0)
              {
                is_to_filter = !filter.PassFilter(component_filters_table_entries[row].Headers[selected_col - 1]);
              }
              else if (selected_col == 0) //ALL
              {
                is_to_filter = std::all_of(component_filters_table_entries[row].Headers.begin(),
                                           component_filters_table_entries[row].Headers.end(),
                                           [](const char* entry){return !filter.PassFilter(entry);});
              }
              
              if (is_to_filter)
              {
                continue;
              }
            }
            
            
            if (table_id_ == "CompGroupFiltersMainWindow")
            {
              bool is_to_filter;
              if (selected_col > 0)
              {
                is_to_filter = !filter.PassFilter(component_group_filters_table_entries[row].Headers[selected_col - 1]);
              }
              else if (selected_col == 0) //ALL
              {
                is_to_filter = std::all_of(component_group_filters_table_entries[row].Headers.begin(),
                                           component_group_filters_table_entries[row].Headers.end(),
                                           [](const char* entry){return !filter.PassFilter(entry);});
              }
              
              if (is_to_filter)
              {
                continue;
              }
            }
            
            if (table_id_ == "CompQCsMainWindow")
            {
              bool is_to_filter;
              if (selected_col > 0)
              {
                is_to_filter = !filter.PassFilter(component_qcs_table_entries[row].Headers[selected_col - 1]);
              }
              else if (selected_col == 0) //ALL
              {
                is_to_filter = std::all_of(component_qcs_table_entries[row].Headers.begin(),
                                           component_qcs_table_entries[row].Headers.end(),
                                           [](const char* entry){return !filter.PassFilter(entry);});
              }
              
              if (is_to_filter)
              {
                continue;
              }
            }
            
            if (table_id_ == "CompGroupQCsMainWindow")
            {
              bool is_to_filter;
              if (selected_col > 0)
              {
                is_to_filter = !filter.PassFilter(component_group_qcs_table_entries[row].Headers[selected_col - 1]);
              }
              else if (selected_col == 0) //ALL
              {
                is_to_filter = std::all_of(component_group_qcs_table_entries[row].Headers.begin(),
                                           component_group_qcs_table_entries[row].Headers.end(),
                                           [](const char* entry){return !filter.PassFilter(entry);});
              }
              
              if (is_to_filter)
              {
                continue;
              }
            }
            
            if (table_id_ == "featuresTableMainWindow")
            {
              bool is_to_filter;
              if (selected_col > 0)
              {
                is_to_filter = !filter.PassFilter(features_table_entries[row].Headers[selected_col - 1]);
              }
              else if (selected_col == 0) //ALL
              {
                is_to_filter = std::all_of(features_table_entries[row].Headers.begin(),
                                           features_table_entries[row].Headers.end(),
                                           [](const char* entry){return !filter.PassFilter(entry);});
              }
              
              if (is_to_filter)
              {
                continue;
              }
            }
            
            if (table_id_ == "CompRSDFiltersMainWindow")
            {
              bool is_to_filter;
              if (selected_col > 0)
              {
                is_to_filter = !filter.PassFilter(component_rsd_filters_table_entries[row].Headers[selected_col - 1]);
              }
              else if (selected_col == 0) //ALL
              {
                is_to_filter = std::all_of(component_rsd_filters_table_entries[row].Headers.begin(),
                                           component_rsd_filters_table_entries[row].Headers.end(),
                                           [](const char* entry){return !filter.PassFilter(entry);});
              }
              
              if (is_to_filter)
              {
                continue;
              }
            }
            
            if (table_id_ == "CompGroupRSDFiltersMainWindow")
            {
              bool is_to_filter;
              if (selected_col > 0)
              {
                is_to_filter = !filter.PassFilter(component_group_rsd_filter_table_entries[row].Headers[selected_col - 1]);
              }
              else if (selected_col == 0) //ALL
              {
                is_to_filter = std::all_of(component_group_rsd_filter_table_entries[row].Headers.begin(),
                                           component_group_rsd_filter_table_entries[row].Headers.end(),
                                           [](const char* entry){return !filter.PassFilter(entry);});
              }
              
              if (is_to_filter)
              {
                continue;
              }
            }
            
            if (table_id_ == "CompRSDQCsMainWindow")
            {
              bool is_to_filter;
              if (selected_col > 0)
              {
                is_to_filter = !filter.PassFilter(component_rsd_qcs_table_entries[row].Headers[selected_col - 1]);
              }
              else if (selected_col == 0) //ALL
              {
                is_to_filter = std::all_of(component_rsd_qcs_table_entries[row].Headers.begin(),
                                           component_rsd_qcs_table_entries[row].Headers.end(),
                                           [](const char* entry){return !filter.PassFilter(entry);});
              }
              
              if (is_to_filter)
              {
                continue;
              }
            }
            
            if (table_id_ == "CompGroupRSDQCsMainWindow")
            {
              bool is_to_filter;
              if (selected_col > 0)
              {
                is_to_filter = !filter.PassFilter(component_group_rsd_qcs_table_entries[row].Headers[selected_col - 1]);
              }
              else if (selected_col == 0) //ALL
              {
                is_to_filter = std::all_of(component_group_rsd_qcs_table_entries[row].Headers.begin(),
                                           component_group_rsd_qcs_table_entries[row].Headers.end(),
                                           [](const char* entry){return !filter.PassFilter(entry);});
              }
              
              if (is_to_filter)
              {
                continue;
              }
            }
            
            if (table_id_ == "CompBackgroundFiltersMainWindow")
            {
              bool is_to_filter;
              if (selected_col > 0)
              {
                is_to_filter = !filter.PassFilter(component_background_filter_table_entries[row].Headers[selected_col - 1]);
              }
              else if (selected_col == 0) //ALL
              {
                is_to_filter = std::all_of(component_background_filter_table_entries[row].Headers.begin(),
                                           component_background_filter_table_entries[row].Headers.end(),
                                           [](const char* entry){return !filter.PassFilter(entry);});
              }
              
              if (is_to_filter)
              {
                continue;
              }
            }
            
            if (table_id_ == "CompGroupBackgroundFiltersMainWindow")
            {
              bool is_to_filter;
              if (selected_col > 0)
              {
                is_to_filter = !filter.PassFilter(component_group_background_filter_table_entries[row].Headers[selected_col - 1]);
              }
              else if (selected_col == 0) //ALL
              {
                is_to_filter = std::all_of(component_group_background_filter_table_entries[row].Headers.begin(),
                                           component_group_background_filter_table_entries[row].Headers.end(),
                                           [](const char* entry){return !filter.PassFilter(entry);});
              }
              
              if (is_to_filter)
              {
                continue;
              }
            }
            
            if (table_id_ == "CompBackgroundQCsMainWindow")
            {
              bool is_to_filter;
              if (selected_col > 0)
              {
                is_to_filter = !filter.PassFilter(component_background_qcs_table_entries[row].Headers[selected_col - 1]);
              }
              else if (selected_col == 0) //ALL
              {
                is_to_filter = std::all_of(component_background_qcs_table_entries[row].Headers.begin(),
                                           component_background_qcs_table_entries[row].Headers.end(),
                                           [](const char* entry){return !filter.PassFilter(entry);});
              }
              
              if (is_to_filter)
              {
                continue;
              }
            }
            
            if (table_id_ == "CompGroupBackgroundQCsMainWindow")
            {
              bool is_to_filter;
              if (selected_col > 0)
              {
                is_to_filter = !filter.PassFilter(component_group_background_qcs_table_entries[row].Headers[selected_col - 1]);
              }
              else if (selected_col == 0) //ALL
              {
                is_to_filter = std::all_of(component_group_background_qcs_table_entries[row].Headers.begin(),
                                           component_group_background_qcs_table_entries[row].Headers.end(),
                                           [](const char* entry){return !filter.PassFilter(entry);});
              }
              
              if (is_to_filter)
              {
                continue;
              }
            }
            
            if (table_id_ == "CompRSDEstimationsMainWindow")
            {
              bool is_to_filter;
              if (selected_col > 0)
              {
                is_to_filter = !filter.PassFilter(component_rsd_estimations_table_entries[row].Headers[selected_col - 1]);
              }
              else if (selected_col == 0) //ALL
              {
                is_to_filter = std::all_of(component_rsd_estimations_table_entries[row].Headers.begin(),
                                           component_rsd_estimations_table_entries[row].Headers.end(),
                                           [](const char* entry){return !filter.PassFilter(entry);});
              }
              
              if (is_to_filter)
              {
                continue;
              }
            }
            
            if (table_id_ == "CompGroupRSDEstimationsMainWindow")
            {
              bool is_to_filter;
              if (selected_col > 0)
              {
                is_to_filter = !filter.PassFilter(component_group_rsd_estimations_table_entries[row].Headers[selected_col - 1]);
              }
              else if (selected_col == 0) //ALL
              {
                is_to_filter = std::all_of(component_group_rsd_estimations_table_entries[row].Headers.begin(),
                                           component_group_rsd_estimations_table_entries[row].Headers.end(),
                                           [](const char* entry){return !filter.PassFilter(entry);});
              }
              
              if (is_to_filter)
              {
                continue;
              }
            }
            
            if (table_id_ == "CompBackgroundEstimationsMainWindow")
            {
              bool is_to_filter;
              if (selected_col > 0)
              {
                is_to_filter = !filter.PassFilter(component_background_estimations_table_entries[row].Headers[selected_col - 1]);
              }
              else if (selected_col == 0) //ALL
              {
                is_to_filter = std::all_of(component_background_estimations_table_entries[row].Headers.begin(),
                                           component_background_estimations_table_entries[row].Headers.end(),
                                           [](const char* entry){return !filter.PassFilter(entry);});
              }
              
              if (is_to_filter)
              {
                continue;
              }
            }
            
            if (table_id_ == "CompGroupBackgroundEstimationsMainWindow")
            {
              bool is_to_filter;
              if (selected_col > 0)
              {
                is_to_filter = !filter.PassFilter(component_group_background_estimations_table_entries[row].Headers[selected_col - 1]);
              }
              else if (selected_col == 0) //ALL
              {
                is_to_filter = std::all_of(component_group_background_estimations_table_entries[row].Headers.begin(),
                                           component_group_background_estimations_table_entries[row].Headers.end(),
                                           [](const char* entry){return !filter.PassFilter(entry);});
              }
              
              if (is_to_filter)
              {
                continue;
              }
            }
            
            if (table_id_ == "featureMatrixMainWindow")
            {
              bool is_to_filter;
              if (selected_col > 0)
              {
                is_to_filter = !filter.PassFilter(feature_matrix_entries[row].Headers[selected_col - 1]);
              }
              else if (selected_col == 0) //ALL
              {
                is_to_filter = std::all_of(feature_matrix_entries[row].Headers.begin(), feature_matrix_entries[row].Headers.end(),
                                           [](const char* entry){return !filter.PassFilter(entry);});
              }
              
              if (is_to_filter)
              {
                continue;
              }
            }
            
            ImGui::TableNextRow();
            for (size_t col = 0; col < headers_.size(); ++col) {
              if (table_id_ == "SequenceMainWindow" && sequence_scanned == true && !sequence_table_entries.empty())
              {
                ImGui::TableSetColumnIndex(col);
                ImGui::Text("%s", sequence_table_entries[row].Headers[col]);
              }
              else if (table_id_ == "TransitionsMainWindow" && transitions_scanned == true && !transition_table_entries.empty())
              {
                ImGui::TableSetColumnIndex(col);
                ImGui::Text("%s", transition_table_entries[row].Headers[col]);
              }
              else if (table_id_ == "StdsConcsMainWindow" && concentrations_scanned == true && !concentration_table_entries.empty())
              {
                ImGui::TableSetColumnIndex(col);
                ImGui::Text("%s", concentration_table_entries[row].Headers[col]);
              }
              else if (table_id_ == "ParametersMainWindow" && parameters_scanned == true && !parameter_table_entries.empty())
              {
                ImGui::TableSetColumnIndex(col);
                ImGui::Text("%s", parameter_table_entries[row].Headers[col]);
              }
              else if (table_id_ == "SpectrumMainWindow" && spectrums_scanned == true && !spectrum_table_entries.empty())
              {
                ImGui::TableSetColumnIndex(col);
                ImGui::Text("%s", spectrum_table_entries[row].Headers[col]);
              }
              else if (table_id_ == "CompFiltersMainWindow" && component_filters_scanned == true && !component_filters_table_entries.empty())
              {
                ImGui::TableSetColumnIndex(col);
                ImGui::Text("%s", component_filters_table_entries[row].Headers[col]);
              }
              else if (table_id_ == "CompGroupFiltersMainWindow" && component_group_filters_scanned == true &&
                       !component_group_filters_table_entries.empty())
              {
                ImGui::TableSetColumnIndex(col);
                ImGui::Text("%s", component_group_filters_table_entries[row].Headers[col]);
              }
              else if (table_id_ == "CompQCsMainWindow" && component_qcs_scanned == true && !component_qcs_table_entries.empty())
              {
                ImGui::TableSetColumnIndex(col);
                ImGui::Text("%s", component_qcs_table_entries[row].Headers[col]);
              }
              else if (table_id_ == "CompGroupQCsMainWindow" && component_group_qcs_scanned == true &&
                       !component_group_qcs_table_entries.empty())
              {
                ImGui::TableSetColumnIndex(col);
                ImGui::Text("%s", component_group_qcs_table_entries[row].Headers[col]);
              }
              else if (table_id_ == "featuresTableMainWindow" && features_scanned == true && !features_table_entries.empty())
              {
                ImGui::TableSetColumnIndex(col);
                ImGui::Text("%s", features_table_entries[row].Headers[col]);
              }
              else if (table_id_ == "CompRSDFiltersMainWindow" && component_rsd_filters_scanned == true &&
                       !component_rsd_filters_table_entries.empty())
              {
                ImGui::TableSetColumnIndex(col);
                ImGui::Text("%s", component_rsd_filters_table_entries[row].Headers[col]);
              }
              else if (table_id_ == "CompGroupRSDFiltersMainWindow" && component_group_rsd_filter_scanned == true &&
                       !component_group_rsd_filter_table_entries.empty())
              {
                ImGui::TableSetColumnIndex(col);
                ImGui::Text("%s", component_group_rsd_filter_table_entries[row].Headers[col]);
              }
              else if (table_id_ == "CompRSDQCsMainWindow" && component_rsd_qcs_scanned == true &&
                       !component_rsd_qcs_table_entries.empty())
              {
                ImGui::TableSetColumnIndex(col);
                ImGui::Text("%s", component_rsd_qcs_table_entries[row].Headers[col]);
              }
              else if (table_id_ == "CompGroupRSDQCsMainWindow" && component_group_rsd_qcs_scanned == true &&
                       !component_group_rsd_qcs_table_entries.empty())
              {
                ImGui::TableSetColumnIndex(col);
                ImGui::Text("%s", component_group_rsd_qcs_table_entries[row].Headers[col]);
              }
              else if (table_id_ == "CompBackgroundFiltersMainWindow" && component_background_filter_scanned == true &&
                       !component_background_filter_table_entries.empty())
              {
                ImGui::TableSetColumnIndex(col);
                ImGui::Text("%s", component_background_filter_table_entries[row].Headers[col]);
              }
              else if (table_id_ == "CompGroupBackgroundFiltersMainWindow" && component_group_background_filter_scanned == true &&
                       !component_group_background_filter_table_entries.empty())
              {
                ImGui::TableSetColumnIndex(col);
                ImGui::Text("%s", component_group_background_filter_table_entries[row].Headers[col]);
              }
              else if (table_id_ == "CompBackgroundQCsMainWindow" && component_background_qcs_scanned == true &&
                       !component_background_qcs_table_entries.empty())
              {
                ImGui::TableSetColumnIndex(col);
                ImGui::Text("%s", component_background_qcs_table_entries[row].Headers[col]);
              }
              else if (table_id_ == "CompGroupBackgroundQCsMainWindow" && component_group_background_qcs_scanned == true &&
                       !component_group_background_qcs_table_entries.empty())
              {
                ImGui::TableSetColumnIndex(col);
                ImGui::Text("%s", component_group_background_qcs_table_entries[row].Headers[col]);
              }
              else if (table_id_ == "CompRSDEstimationsMainWindow" && component_rsd_estimations_scanned == true &&
                       !component_rsd_estimations_table_entries.empty())
              {
                ImGui::TableSetColumnIndex(col);
                ImGui::Text("%s", component_rsd_estimations_table_entries[row].Headers[col]);
              }
              else if (table_id_ == "CompGroupRSDEstimationsMainWindow" && component_group_rsd_estimations_scanned == true &&
                       !component_group_rsd_estimations_table_entries.empty())
              {
                ImGui::TableSetColumnIndex(col);
                ImGui::Text("%s", component_group_rsd_estimations_table_entries[row].Headers[col]);
              }
              else if (table_id_ == "CompBackgroundEstimationsMainWindow" && component_background_estimations_scanned == true &&
                       !component_background_estimations_table_entries.empty())
              {
                ImGui::TableSetColumnIndex(col);
                ImGui::Text("%s", component_background_estimations_table_entries[row].Headers[col]);
              }
              else if (table_id_ == "CompGroupBackgroundEstimationsMainWindow" && component_group_background_estimations_scanned == true && !component_group_background_estimations_table_entries.empty())
              {
                ImGui::TableSetColumnIndex(col);
                ImGui::Text("%s", component_group_background_estimations_table_entries[row].Headers[col]);
              }
              else if (table_id_ == "featureMatrixMainWindow" && feature_matrix_scanned == true && !feature_matrix_entries.empty())
              {
                ImGui::TableSetColumnIndex(col);
                ImGui::Text("%s", feature_matrix_entries[row].Headers[col]);
              }
            }
          }
        }
      }
      
      if (ImGuiTableSortSpecs* sorts_specs = ImGui::TableGetSortSpecs())
      {
        const unsigned int col_idx = static_cast<unsigned int>(sorts_specs->Specs->ColumnIndex);
        
        if (sorts_specs->SpecsDirty && sequence_scanned == true && table_id_ == "SequenceMainWindow" &&
            !std::all_of(sequence_table_entries.begin(), sequence_table_entries.end(),
                         [&col_idx](ImTableEntry& entry){ return !std::strcmp(entry.Headers[col_idx], sequence_table_entries.begin()->Headers[col_idx]); }))
        {
          ImTableEntry::s_current_sort_specs = sorts_specs;
          if (sequence_table_entries.Size > 1)
              qsort(&sequence_table_entries[0], (size_t)sequence_table_entries.Size, sizeof(sequence_table_entries[0]), ImTableEntry::CompareWithSortSpecs);
          ImTableEntry::s_current_sort_specs = NULL;
          sorts_specs->SpecsDirty = false;
        }
        if (sorts_specs->SpecsDirty && transitions_scanned == true && table_id_ == "TransitionsMainWindow" &&
            !std::all_of(transition_table_entries.begin(), transition_table_entries.end(),
                         [&col_idx](ImTableEntry& entry){ return !std::strcmp(entry.Headers[col_idx], transition_table_entries.begin()->Headers[col_idx]); }))
        {
          ImTableEntry::s_current_sort_specs = sorts_specs;
          if (transition_table_entries.Size > 1)
              qsort(&transition_table_entries[0], (size_t)transition_table_entries.Size, sizeof(transition_table_entries[0]), ImTableEntry::CompareWithSortSpecs);
          ImTableEntry::s_current_sort_specs = NULL;
          sorts_specs->SpecsDirty = false;
        }
        if (sorts_specs->SpecsDirty && concentrations_scanned == true && table_id_ == "StdsConcsMainWindow" &&
            !std::all_of(concentration_table_entries.begin(), concentration_table_entries.end(),
                          [&col_idx](ImTableEntry& entry){ return !std::strcmp(entry.Headers[col_idx], concentration_table_entries.begin()->Headers[col_idx]); }))
        {
          ImTableEntry::s_current_sort_specs = sorts_specs;
          if (concentration_table_entries.Size > 1)
              qsort(&concentration_table_entries[0], (size_t)concentration_table_entries.Size,
                    sizeof(concentration_table_entries[0]), ImTableEntry::CompareWithSortSpecs);
          ImTableEntry::s_current_sort_specs = NULL;
          sorts_specs->SpecsDirty = false;
        }
        if (sorts_specs->SpecsDirty && parameters_scanned == true && table_id_ == "ParametersMainWindow" &&
            !std::all_of(parameter_table_entries.begin(), parameter_table_entries.end(),
                         [&col_idx](ImTableEntry& entry){ return !std::strcmp(entry.Headers[col_idx], parameter_table_entries.begin()->Headers[col_idx]); }))
        {
          ImTableEntry::s_current_sort_specs = sorts_specs;
          if (parameter_table_entries.Size > 1)
              qsort(&parameter_table_entries[0], (size_t)parameter_table_entries.Size,
                    sizeof(parameter_table_entries[0]), ImTableEntry::CompareWithSortSpecs);
          ImTableEntry::s_current_sort_specs = NULL;
          sorts_specs->SpecsDirty = false;
        }
        if (sorts_specs->SpecsDirty && spectrums_scanned == true && table_id_ == "SpectrumMainWindow" &&
            !std::all_of(spectrum_table_entries.begin(), spectrum_table_entries.end(),
                         [&col_idx](ImTableEntry& entry){ return !std::strcmp(entry.Headers[col_idx], spectrum_table_entries.begin()->Headers[col_idx]); }))
        {
          ImTableEntry::s_current_sort_specs = sorts_specs;
          if (spectrum_table_entries.Size > 1)
              qsort(&spectrum_table_entries[0], (size_t)spectrum_table_entries.Size,
                    sizeof(spectrum_table_entries[0]), ImTableEntry::CompareWithSortSpecs);
          ImTableEntry::s_current_sort_specs = NULL;
          sorts_specs->SpecsDirty = false;
        }
        if (sorts_specs->SpecsDirty && quantitation_methods_scanned == true && table_id_ == "QuantMethodMainWindow" &&
            !std::all_of(quantitation_method_table_entries.begin(), quantitation_method_table_entries.end(),
                         [&col_idx](ImTableEntry& entry){ return !std::strcmp(entry.Headers[col_idx], quantitation_method_table_entries.begin()->Headers[col_idx]); }))
        {
          ImTableEntry::s_current_sort_specs = sorts_specs;
          if (quantitation_method_table_entries.Size > 1)
              qsort(&quantitation_method_table_entries[0], (size_t)quantitation_method_table_entries.Size,
                    sizeof(quantitation_method_table_entries[0]), ImTableEntry::CompareWithSortSpecs);
          ImTableEntry::s_current_sort_specs = NULL;
          sorts_specs->SpecsDirty = false;
        }
        if (sorts_specs->SpecsDirty && component_filters_scanned == true && table_id_ == "CompFiltersMainWindow" &&
            !std::all_of(component_filters_table_entries.begin(), component_filters_table_entries.end(),
                         [&col_idx](ImTableEntry& entry){ return !std::strcmp(entry.Headers[col_idx], component_filters_table_entries.begin()->Headers[col_idx]); }))
        {
          ImTableEntry::s_current_sort_specs = sorts_specs;
          if (component_filters_table_entries.Size > 1)
              qsort(&component_filters_table_entries[0], (size_t)component_filters_table_entries.Size,
                    sizeof(component_filters_table_entries[0]), ImTableEntry::CompareWithSortSpecs);
          ImTableEntry::s_current_sort_specs = NULL;
          sorts_specs->SpecsDirty = false;
        }
        if (sorts_specs->SpecsDirty && component_group_filters_scanned == true && table_id_ == "CompGroupFiltersMainWindow" &&
            !std::all_of(component_group_filters_table_entries.begin(), component_group_filters_table_entries.end(),
                         [&col_idx](ImTableEntry& entry){ return !std::strcmp(entry.Headers[col_idx], component_group_filters_table_entries.begin()->Headers[col_idx]); }))
        {
          ImTableEntry::s_current_sort_specs = sorts_specs;
          if (component_group_filters_table_entries.Size > 1)
              qsort(&component_group_filters_table_entries[0], (size_t)component_group_filters_table_entries.Size,
                    sizeof(component_group_filters_table_entries[0]), ImTableEntry::CompareWithSortSpecs);
          ImTableEntry::s_current_sort_specs = NULL;
          sorts_specs->SpecsDirty = false;
        }
        if (sorts_specs->SpecsDirty && component_qcs_scanned == true && table_id_ == "CompQCsMainWindow" &&
            !std::all_of(component_qcs_table_entries.begin(), component_qcs_table_entries.end(),
                         [&col_idx](ImTableEntry& entry){ return !std::strcmp(entry.Headers[col_idx], component_qcs_table_entries.begin()->Headers[col_idx]); }))
        {
          ImTableEntry::s_current_sort_specs = sorts_specs;
          if (component_qcs_table_entries.Size > 1)
              qsort(&component_qcs_table_entries[0], (size_t)component_qcs_table_entries.Size,
                    sizeof(component_qcs_table_entries[0]), ImTableEntry::CompareWithSortSpecs);
          ImTableEntry::s_current_sort_specs = NULL;
          sorts_specs->SpecsDirty = false;
        }
        if (sorts_specs->SpecsDirty && component_group_qcs_scanned == true && table_id_ == "CompGroupQCsMainWindow" &&
            !std::all_of(component_group_qcs_table_entries.begin(), component_group_qcs_table_entries.end(),
                         [&col_idx](ImTableEntry& entry){ return !std::strcmp(entry.Headers[col_idx], component_group_qcs_table_entries.begin()->Headers[col_idx]); }))
        {
          ImTableEntry::s_current_sort_specs = sorts_specs;
          if (component_group_qcs_table_entries.Size > 1)
              qsort(&component_group_qcs_table_entries[0], (size_t)component_group_qcs_table_entries.Size,
                    sizeof(component_group_qcs_table_entries[0]), ImTableEntry::CompareWithSortSpecs);
          ImTableEntry::s_current_sort_specs = NULL;
          sorts_specs->SpecsDirty = false;
        }
        if (sorts_specs->SpecsDirty && features_scanned == true && table_id_ == "featuresTableMainWindow" &&
            !std::all_of(features_table_entries.begin(), features_table_entries.end(),
                         [&col_idx](ImTableEntry& entry){ return !std::strcmp(entry.Headers[col_idx], features_table_entries.begin()->Headers[col_idx]); }))
        {
          ImTableEntry::s_current_sort_specs = sorts_specs;
          if (features_table_entries.Size > 1)
              qsort(&features_table_entries[0], (size_t)features_table_entries.Size,
                    sizeof(features_table_entries[0]), ImTableEntry::CompareWithSortSpecs);
          ImTableEntry::s_current_sort_specs = NULL;
          sorts_specs->SpecsDirty = false;
        }
        if (sorts_specs->SpecsDirty && component_rsd_filters_scanned == true && table_id_ == "CompRSDFiltersMainWindow" &&
            !std::all_of(component_rsd_filters_table_entries.begin(), component_rsd_filters_table_entries.end(),
                         [&col_idx](ImTableEntry& entry){ return !std::strcmp(entry.Headers[col_idx], component_rsd_filters_table_entries.begin()->Headers[col_idx]); }))
        {
          ImTableEntry::s_current_sort_specs = sorts_specs;
          if (component_rsd_filters_table_entries.Size > 1)
              qsort(&component_rsd_filters_table_entries[0], (size_t)component_rsd_filters_table_entries.Size,
                    sizeof(component_rsd_filters_table_entries[0]), ImTableEntry::CompareWithSortSpecs);
          ImTableEntry::s_current_sort_specs = NULL;
          sorts_specs->SpecsDirty = false;
        }
        if (sorts_specs->SpecsDirty && component_group_rsd_filter_scanned == true && table_id_ == "CompGroupRSDFiltersMainWindow" &&
            !std::all_of(component_group_rsd_filter_table_entries.begin(), component_group_rsd_filter_table_entries.end(),
                         [&col_idx](ImTableEntry& entry){ return !std::strcmp(entry.Headers[col_idx], component_group_rsd_filter_table_entries.begin()->Headers[col_idx]); }))
        {
          ImTableEntry::s_current_sort_specs = sorts_specs;
          if (component_group_rsd_filter_table_entries.Size > 1)
              qsort(&component_group_rsd_filter_table_entries[0], (size_t)component_group_rsd_filter_table_entries.Size,
                    sizeof(component_group_rsd_filter_table_entries[0]), ImTableEntry::CompareWithSortSpecs);
          ImTableEntry::s_current_sort_specs = NULL;
          sorts_specs->SpecsDirty = false;
        }
        if (sorts_specs->SpecsDirty && component_rsd_qcs_scanned == true && table_id_ == "CompRSDQCsMainWindow" &&
            !std::all_of(component_rsd_qcs_table_entries.begin(), component_rsd_qcs_table_entries.end(),
                         [&col_idx](ImTableEntry& entry){ return !std::strcmp(entry.Headers[col_idx], component_rsd_qcs_table_entries.begin()->Headers[col_idx]); }))
        {
          ImTableEntry::s_current_sort_specs = sorts_specs;
          if (component_rsd_qcs_table_entries.Size > 1)
              qsort(&component_rsd_qcs_table_entries[0], (size_t)component_rsd_qcs_table_entries.Size,
                    sizeof(component_rsd_qcs_table_entries[0]), ImTableEntry::CompareWithSortSpecs);
          ImTableEntry::s_current_sort_specs = NULL;
          sorts_specs->SpecsDirty = false;
        }
        if (sorts_specs->SpecsDirty && component_group_rsd_qcs_scanned == true && table_id_ == "CompGroupRSDQCsMainWindow" &&
            !std::all_of(component_group_rsd_qcs_table_entries.begin(), component_group_rsd_qcs_table_entries.end(),
                         [&col_idx](ImTableEntry& entry){ return !std::strcmp(entry.Headers[col_idx], component_group_rsd_qcs_table_entries.begin()->Headers[col_idx]); }))
        {
          ImTableEntry::s_current_sort_specs = sorts_specs;
          if (component_group_rsd_qcs_table_entries.Size > 1)
              qsort(&component_group_rsd_qcs_table_entries[0], (size_t)component_group_rsd_qcs_table_entries.Size,
                    sizeof(component_group_rsd_qcs_table_entries[0]), ImTableEntry::CompareWithSortSpecs);
          ImTableEntry::s_current_sort_specs = NULL;
          sorts_specs->SpecsDirty = false;
        }
        if (sorts_specs->SpecsDirty && component_background_filter_scanned == true && table_id_ == "CompBackgroundFiltersMainWindow" &&
            !std::all_of(component_background_filter_table_entries.begin(), component_background_filter_table_entries.end(),
                         [&col_idx](ImTableEntry& entry){ return !std::strcmp(entry.Headers[col_idx], component_background_filter_table_entries.begin()->Headers[col_idx]); }))
        {
          ImTableEntry::s_current_sort_specs = sorts_specs;
          if (component_background_filter_table_entries.Size > 1)
              qsort(&component_background_filter_table_entries[0], (size_t)component_background_filter_table_entries.Size,
                    sizeof(component_background_filter_table_entries[0]), ImTableEntry::CompareWithSortSpecs);
          ImTableEntry::s_current_sort_specs = NULL;
          sorts_specs->SpecsDirty = false;
        }
        if (sorts_specs->SpecsDirty && component_group_background_filter_scanned == true && table_id_ == "CompGroupBackgroundFiltersMainWindow" &&
            !std::all_of(component_group_background_filter_table_entries.begin(), component_group_background_filter_table_entries.end(),
                         [&col_idx](ImTableEntry& entry){ return !std::strcmp(entry.Headers[col_idx], component_group_background_filter_table_entries.begin()->Headers[col_idx]); }))
        {
          ImTableEntry::s_current_sort_specs = sorts_specs;
          if (component_group_background_filter_table_entries.Size > 1)
              qsort(&component_group_background_filter_table_entries[0], (size_t)component_group_background_filter_table_entries.Size, sizeof(component_group_background_filter_table_entries[0]), ImTableEntry::CompareWithSortSpecs);
          ImTableEntry::s_current_sort_specs = NULL;
          sorts_specs->SpecsDirty = false;
        }
        if (sorts_specs->SpecsDirty && component_background_qcs_scanned == true && table_id_ == "CompBackgroundQCsMainWindow" &&
            !std::all_of(component_background_qcs_table_entries.begin(), component_background_qcs_table_entries.end(),
                         [&col_idx](ImTableEntry& entry){ return !std::strcmp(entry.Headers[col_idx], component_background_qcs_table_entries.begin()->Headers[col_idx]); }))
        {
          ImTableEntry::s_current_sort_specs = sorts_specs;
          if (component_background_qcs_table_entries.Size > 1)
              qsort(&component_background_qcs_table_entries[0], (size_t)component_background_qcs_table_entries.Size,
                    sizeof(component_background_qcs_table_entries[0]), ImTableEntry::CompareWithSortSpecs);
          ImTableEntry::s_current_sort_specs = NULL;
          sorts_specs->SpecsDirty = false;
        }
        if (sorts_specs->SpecsDirty && component_group_background_qcs_scanned == true && table_id_ == "CompGroupBackgroundQCsMainWindow" &&
            !std::all_of(component_group_background_qcs_table_entries.begin(), component_group_background_qcs_table_entries.end(),
                         [&col_idx](ImTableEntry& entry){ return !std::strcmp(entry.Headers[col_idx],
                                                                              component_group_background_qcs_table_entries.begin()->Headers[col_idx]); }))
        {
          ImTableEntry::s_current_sort_specs = sorts_specs;
          if (component_group_background_qcs_table_entries.Size > 1)
              qsort(&component_group_background_qcs_table_entries[0], (size_t)component_group_background_qcs_table_entries.Size,
                    sizeof(component_group_background_qcs_table_entries[0]), ImTableEntry::CompareWithSortSpecs);
          ImTableEntry::s_current_sort_specs = NULL;
          sorts_specs->SpecsDirty = false;
        }
        if (sorts_specs->SpecsDirty && component_rsd_estimations_scanned == true && table_id_ == "CompRSDEstimationsMainWindow" &&
            !std::all_of(component_rsd_estimations_table_entries.begin(), component_rsd_estimations_table_entries.end(),
                         [&col_idx](ImTableEntry& entry){ return !std::strcmp(entry.Headers[col_idx], component_rsd_estimations_table_entries.begin()->Headers[col_idx]); }))
        {
          ImTableEntry::s_current_sort_specs = sorts_specs;
          if (component_rsd_estimations_table_entries.Size > 1)
              qsort(&component_rsd_estimations_table_entries[0], (size_t)component_rsd_estimations_table_entries.Size,
                    sizeof(component_rsd_estimations_table_entries[0]), ImTableEntry::CompareWithSortSpecs);
          ImTableEntry::s_current_sort_specs = NULL;
          sorts_specs->SpecsDirty = false;
        }
        if (sorts_specs->SpecsDirty && component_group_rsd_estimations_scanned == true && table_id_ == "CompGroupRSDEstimationsMainWindow" &&
            !std::all_of(component_group_rsd_estimations_table_entries.begin(), component_group_rsd_estimations_table_entries.end(),
                         [&col_idx](ImTableEntry& entry){ return !std::strcmp(entry.Headers[col_idx],
                                                                              component_group_rsd_estimations_table_entries.begin()->Headers[col_idx]); }))
        {
          ImTableEntry::s_current_sort_specs = sorts_specs;
          if (component_group_rsd_estimations_table_entries.Size > 1)
              qsort(&component_group_rsd_estimations_table_entries[0], (size_t)component_group_rsd_estimations_table_entries.Size,
                    sizeof(component_group_rsd_estimations_table_entries[0]), ImTableEntry::CompareWithSortSpecs);
          ImTableEntry::s_current_sort_specs = NULL;
          sorts_specs->SpecsDirty = false;
        }
        if (sorts_specs->SpecsDirty && component_background_estimations_scanned == true && table_id_ == "CompBackgroundEstimationsMainWindow" &&
            !std::all_of(component_background_estimations_table_entries.begin(), component_background_estimations_table_entries.end(),
                         [&col_idx](ImTableEntry& entry){ return !std::strcmp(entry.Headers[col_idx],
                                                                              component_background_estimations_table_entries.begin()->Headers[col_idx]); }))
        {
          ImTableEntry::s_current_sort_specs = sorts_specs;
          if (component_background_estimations_table_entries.Size > 1)
              qsort(&component_background_estimations_table_entries[0], (size_t)component_background_estimations_table_entries.Size,
                    sizeof(component_background_estimations_table_entries[0]), ImTableEntry::CompareWithSortSpecs);
          ImTableEntry::s_current_sort_specs = NULL;
          sorts_specs->SpecsDirty = false;
        }
        if (sorts_specs->SpecsDirty && component_group_background_estimations_scanned == true && table_id_ == "CompGroupBackgroundEstimationsMainWindow" &&
            !std::all_of(component_group_background_estimations_table_entries.begin(), component_group_background_estimations_table_entries.end(),
                         [&col_idx](ImTableEntry& entry){ return !std::strcmp(entry.Headers[col_idx],
                                                                              component_group_background_estimations_table_entries.begin()->Headers[col_idx]); }))
        {
          ImTableEntry::s_current_sort_specs = sorts_specs;
          if (component_group_background_estimations_table_entries.Size > 1)
              qsort(&component_group_background_estimations_table_entries[0], (size_t)component_group_background_estimations_table_entries.Size,
                    sizeof(component_group_background_estimations_table_entries[0]), ImTableEntry::CompareWithSortSpecs);
          ImTableEntry::s_current_sort_specs = NULL;
          sorts_specs->SpecsDirty = false;
        }
        if (sorts_specs->SpecsDirty && feature_matrix_scanned == true && table_id_ == "featureMatrixMainWindow" &&
            !std::all_of(feature_matrix_entries.begin(), feature_matrix_entries.end(),
                         [&col_idx](ImTableEntry& entry){ return !std::strcmp(entry.Headers[col_idx], feature_matrix_entries.begin()->Headers[col_idx]); }))
        {
          ImTableEntry::s_current_sort_specs = sorts_specs;
          if (feature_matrix_entries.Size > 1)
              qsort(&feature_matrix_entries[0], (size_t)feature_matrix_entries.Size,
                    sizeof(feature_matrix_entries[0]), ImTableEntry::CompareWithSortSpecs);
          ImTableEntry::s_current_sort_specs = NULL;
          sorts_specs->SpecsDirty = false;
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
    
    static ImGuiTableColumnFlags column_0_flags = { ImGuiTableColumnFlags_DefaultSort | ImGuiTableColumnFlags_NoHide };
    static ImGuiTableColumnFlags column_any_flags = { ImGuiTableColumnFlags_NoHide };
    
    // Columns we want to show up
    static const std::vector<int> cols_to_show = { 0, 1, 2, 3, 7 };
    
    static ImGuiTextFilter filter;
    filter.Draw("Find");

    // drop-down list for search field(s)
    static int selected_col = 0;
    static std::vector<const char*> cols;
    cols.resize(cols_to_show.size() + 1);
    for (size_t header_name = 0; header_name < headers_.size() + 1; ++header_name) {
      if (header_name == 0)
      {
        cols[header_name] = "All";
      }
      else if (header_name > 0  && header_name < cols_to_show.size() + 1)
      {
        cols[header_name] = headers_(cols_to_show[header_name - 1]).c_str();
      }
    }
    
    ImGui::Combo("In Column(s)", &selected_col, cols.data(), cols.size());
    
    static ImVector<ImTableEntry> parameter_table_entries;
    static bool parameters_scanned;
    
    if (columns_.dimension(0) == parameter_table_entries.size())
      parameters_scanned = true;
    else
      parameters_scanned = false;

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
            
            bool is_to_filter;
            if (selected_col > 0)
            {
              is_to_filter = !filter.PassFilter(parameter_table_entries[row].Headers[selected_col - 1]);
            }
            else if (selected_col == 0) //ALL
            {
              is_to_filter = std::all_of(parameter_table_entries[row].Headers.begin(),
                                         parameter_table_entries[row].Headers.end(),
                                         [](const char* entry){return !filter.PassFilter(entry);});
            }
            
            if (is_to_filter)
            {
              continue;
            }
            
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
                        ImGui::Text("%s", constraints.c_str());
                      }
                      ImGui::Text("Expected type:");
                      ImGui::Text("%s", expected_type.c_str());
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
                    ImGui::Text("%s", description.c_str());
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
    
    static ImGuiTableColumnFlags column_0_flags = { ImGuiTableColumnFlags_DefaultSort | ImGuiTableColumnFlags_NoHide };
    static ImGuiTableColumnFlags column_any_flags = { ImGuiTableColumnFlags_NoHide };

    static ImGuiTextFilter filter;
    filter.Draw("Find");
    
    // drop-down list for search field(s)
    static int selected_col = 0;
    static std::vector<const char*> cols;
    cols.resize(headers_.size() + 1);
    for (size_t header_name = 0; header_name < headers_.size() + 1; ++header_name) {
      if (header_name == 0)
      {
        cols[header_name] = "All";
      }
      else if (header_name > 0)
      {
        cols[header_name] = headers_(header_name - 1).c_str();
      }
    }
    
    ImGui::Combo("In Column(s)", &selected_col, cols.data(), cols.size());
    
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
    
    if (columns_.dimensions().TotalSize() > 0 && table_id_ == "InjectionsExplorerWindow")
    {
      const static Eigen::Tensor<std::string,2> columns__(columns_);
      const static Eigen::Tensor<bool,2> checkbox_columns__(checkbox_columns_);
      injection_table_entries.resize(columns__.dimension(0), ImTableEntry());
      if (!injection_table_entries.empty() && injections_scanned == false)
      {
        for (size_t row = 0; row < columns__.dimension(0); ++row)
        {
          ImTableEntry& Im_table_entry = injection_table_entries[row];
          Im_table_entry.Headers.resize(columns__.dimension(1) + checkbox_columns__.dimension(1));
          Im_table_entry.ID = row;
          for (size_t header_idx = 0; header_idx < columns__.dimension(1) + checkbox_columns__.dimension(1); ++header_idx)
          {
            if (header_idx < columns__.dimension(1))
            {
              Im_table_entry.Headers[header_idx] = columns__(row, header_idx).c_str();
            }
            else if (header_idx < columns__.dimension(1) + checkbox_columns__.dimension(1))
            {
              const std::size_t checkbox_idx = header_idx - static_cast<std::size_t>( checkbox_columns__.dimension(1) ) + 1;
              Im_table_entry.Headers[header_idx] = checkbox_columns__(row, checkbox_idx) == true ? "true" : "false";
            }
          }
        }
        injections_scanned = true;
      }
    }
    
    if (columns_.dimensions().TotalSize() > 0 && table_id_ == "TransitionsExplorerWindow")
    {
      const static Eigen::Tensor<std::string,2> columns__(columns_);
      const static Eigen::Tensor<bool,2> checkbox_columns__(checkbox_columns_);
      transition_table_entries.resize(columns__.dimension(0), ImTableEntry());
      if (!transition_table_entries.empty() && transitions_scanned == false)
      {
        for (size_t row = 0; row < columns__.dimension(0); ++row)
        {
          ImTableEntry& Im_table_entry = transition_table_entries[row];
          Im_table_entry.Headers.resize(columns__.dimension(1) + checkbox_columns__.dimension(1));
          Im_table_entry.ID = row;
          for (size_t header_idx = 0; header_idx < columns__.dimension(1) + checkbox_columns__.dimension(1); ++header_idx)
          {
            if (header_idx < columns__.dimension(1))
            {
              Im_table_entry.Headers[header_idx] = columns__(row, header_idx).c_str();
            }
            else if (header_idx < columns__.dimension(1) + checkbox_columns__.dimension(1))
            {
              const std::size_t checkbox_idx = header_idx - static_cast<std::size_t>( checkbox_columns__.dimension(1) ) ;
              Im_table_entry.Headers[header_idx] = checkbox_columns__(row, checkbox_idx) == true ? "true" : "false";
            }
          }
        }
        transitions_scanned = true;
      }
    }
    
    if (columns_.dimensions().TotalSize() > 0 && table_id_ == "FeaturesExplorerWindow")
    {
      const static Eigen::Tensor<std::string,2> columns__(columns_);
      const static Eigen::Tensor<bool,2> checkbox_columns__(checkbox_columns_);
      feature_table_entries.resize(columns__.dimension(0), ImTableEntry());
      if (!feature_table_entries.empty() && features_scanned == false)
      {
        for (size_t row = 0; row < columns__.dimension(0); ++row)
        {
          ImTableEntry& Im_table_entry = feature_table_entries[row];
          Im_table_entry.Headers.resize(columns__.dimension(1) + checkbox_columns__.dimension(1));
          Im_table_entry.ID = row;
          for (size_t header_idx = 0; header_idx < columns__.dimension(1) + checkbox_columns__.dimension(1); ++header_idx)
          {
            if (header_idx < columns__.dimension(1))
            {
              Im_table_entry.Headers[header_idx] = columns__(row, header_idx).c_str();
            }
            else if (header_idx < columns__.dimension(1) + checkbox_columns__.dimension(1))
            {
              const std::size_t checkbox_idx = header_idx - static_cast<std::size_t>( checkbox_columns__.dimension(1) ) + 1;
              Im_table_entry.Headers[header_idx] = checkbox_columns__(row, checkbox_idx) == true ? "true" : "false";
            }
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
              bool is_to_filter;
              if (selected_col > 0)
              {
                is_to_filter = !filter.PassFilter(feature_table_entries[row].Headers[selected_col - 1]);
              }
              else if (selected_col == 0) //ALL
              {
                is_to_filter = std::all_of(feature_table_entries[row].Headers.begin(), feature_table_entries[row].Headers.end(),
                                           [](const char* entry){return !filter.PassFilter(entry);});
              }
              
              if (is_to_filter)
              {
                continue;
              }
            }
            if (table_id_ == "InjectionsExplorerWindow")
            {
              bool is_to_filter;
              if (selected_col > 0)
              {
                is_to_filter = !filter.PassFilter(injection_table_entries[row].Headers[selected_col - 1]);
              }
              else if (selected_col == 0) //ALL
              {
                is_to_filter = std::all_of(injection_table_entries[row].Headers.begin(), injection_table_entries[row].Headers.end(),
                                           [](const char* entry){return !filter.PassFilter(entry);});
              }
              
              if (is_to_filter)
              {
                continue;
              }
            }
            if (table_id_ == "TransitionsExplorerWindow")
            {
              bool is_to_filter;
              if (selected_col > 0)
              {
                is_to_filter = !filter.PassFilter(transition_table_entries[row].Headers[selected_col - 1]);
              }
              else if (selected_col == 0) //ALL
              {
                is_to_filter = std::all_of(transition_table_entries[row].Headers.begin(), transition_table_entries[row].Headers.end(),
                                           [](const char* entry){return !filter.PassFilter(entry);});
              }
              
              if (is_to_filter)
              {
                continue;
              }
            }
            ImGui::TableNextRow();
            for (size_t col = 0; col < headers_.size(); ++col)
            {
              if (table_id_ == "InjectionsExplorerWindow" && injections_scanned == true && !injection_table_entries.empty())
              {
                ImGui::TableSetColumnIndex(col);
                ImGui::Text("%s", injection_table_entries[row].Headers[col]);
              }
              else if (table_id_ == "TransitionsExplorerWindow" && transitions_scanned == true && !transition_table_entries.empty())
              {
                ImGui::TableSetColumnIndex(col);
                ImGui::Text("%s", transition_table_entries[row].Headers[col]);
              }
              else if (table_id_ == "FeaturesExplorerWindow" && features_scanned == true && !feature_table_entries.empty())
              {
                ImGui::TableSetColumnIndex(col);
                ImGui::Text("%s", feature_table_entries[row].Headers[col]);
              }
            }
            
            for (size_t col = 0; col < checkbox_headers_.size(); ++col) {
              std::string id = table_id_ + std::to_string(col) + std::to_string(row*columns_.dimension(1));
              ImGui::TableSetColumnIndex(col + headers_.size());
              ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
              if (table_id_ == "InjectionsExplorerWindow" && injections_scanned == true && !injection_table_entries.empty())
              {
                size_t checkbox_idx = col + headers_.size();
                bool is_checked;
                if (!std::strcmp(injection_table_entries[row].Headers[checkbox_idx], "true"))
                  is_checked = true;
                else
                  is_checked = false;
                ImGui::Checkbox(id.c_str(), &is_checked);
                
                if (is_checked == true && !std::strcmp(injection_table_entries[row].Headers[checkbox_idx], "false"))
                {
                  injection_table_entries[row].Headers[checkbox_idx] = "true";
                  checkbox_columns_(injection_table_entries[row].ID, checkbox_idx - static_cast<std::size_t>(checkbox_columns_.dimension(1)) + 1) = true;
                }
                else if (is_checked == false && !std::strcmp(injection_table_entries[row].Headers[checkbox_idx], "true"))
                {
                  injection_table_entries[row].Headers[checkbox_idx] = "false";
                  checkbox_columns_(injection_table_entries[row].ID, checkbox_idx - static_cast<std::size_t>(checkbox_columns_.dimension(1)) + 1) = false;
                }
              }
              else if (table_id_ == "TransitionsExplorerWindow" && transitions_scanned == true && !transition_table_entries.empty())
              {
                size_t checkbox_idx = col + headers_.size();
                bool is_checked;
                if (!std::strcmp(transition_table_entries[row].Headers[checkbox_idx], "true"))
                  is_checked = true;
                else
                  is_checked = false;
                ImGui::Checkbox(id.c_str(), &is_checked);
                
                if (is_checked == true && !std::strcmp(transition_table_entries[row].Headers[checkbox_idx], "false"))
                {
                  transition_table_entries[row].Headers[checkbox_idx] = "true";
                  checkbox_columns_(transition_table_entries[row].ID, checkbox_idx - static_cast<std::size_t>(checkbox_columns_.dimension(1)) ) = true;
                }
                else if (is_checked == false && !std::strcmp(transition_table_entries[row].Headers[checkbox_idx], "true"))
                {
                  transition_table_entries[row].Headers[checkbox_idx] = "false";
                  checkbox_columns_(transition_table_entries[row].ID, checkbox_idx - static_cast<std::size_t>(checkbox_columns_.dimension(1)) ) = false;
                }
              }
              else if (table_id_ == "FeaturesExplorerWindow" && features_scanned == true && !feature_table_entries.empty())
              {
                size_t checkbox_idx = col + headers_.size();
                bool is_checked;
                if (!std::strcmp(feature_table_entries[row].Headers[checkbox_idx], "true"))
                  is_checked = true;
                else
                  is_checked = false;
                ImGui::Checkbox(id.c_str(), &is_checked);
                
                if (is_checked == true && !std::strcmp(feature_table_entries[row].Headers[checkbox_idx], "false"))
                {
                  feature_table_entries[row].Headers[checkbox_idx] = "true";
                  checkbox_columns_(feature_table_entries[row].ID, checkbox_idx - static_cast<std::size_t>(checkbox_columns_.dimension(1)) + 1) = true;
                }
                else if (is_checked == false && !std::strcmp(feature_table_entries[row].Headers[checkbox_idx], "true"))
                {
                  feature_table_entries[row].Headers[checkbox_idx] = "false";
                  checkbox_columns_(feature_table_entries[row].ID, checkbox_idx - static_cast<std::size_t>(checkbox_columns_.dimension(1)) + 1) = false;
                }
              }
              ImGui::PopStyleColor();
            }
          }
        }
      }

      if (ImGuiTableSortSpecs* sorts_specs = ImGui::TableGetSortSpecs())
      {
        const unsigned int col_idx = static_cast<unsigned int>(sorts_specs->Specs->ColumnIndex);
        
        if (sorts_specs->SpecsDirty && injections_scanned == true && table_id_ == "InjectionsExplorerWindow" &&
            !std::all_of(injection_table_entries.begin(), injection_table_entries.end(),
                         [&col_idx](ImTableEntry& entry){ return !std::strcmp(entry.Headers[col_idx], "true") ? true : false; }))
        {
          ImTableEntry::s_current_sort_specs = sorts_specs;
          if (injection_table_entries.Size > 1)
              qsort(&injection_table_entries[0], (size_t)injection_table_entries.Size, sizeof(injection_table_entries[0]), ImTableEntry::CompareWithSortSpecs);
          ImTableEntry::s_current_sort_specs = NULL;
          sorts_specs->SpecsDirty = false;
        }
        if (sorts_specs->SpecsDirty && transitions_scanned == true && table_id_ == "TransitionsExplorerWindow" &&
            !std::all_of(transition_table_entries.begin(), transition_table_entries.end(),
                         [&col_idx](ImTableEntry& entry){ return !std::strcmp(entry.Headers[col_idx], "true") ? true : false; }))
        {
          ImTableEntry::s_current_sort_specs = sorts_specs;
          if (transition_table_entries.Size > 1)
              qsort(&transition_table_entries[0], (size_t)transition_table_entries.Size, sizeof(transition_table_entries[0]), ImTableEntry::CompareWithSortSpecs);
          ImTableEntry::s_current_sort_specs = NULL;
          sorts_specs->SpecsDirty = false;
        }
        if (sorts_specs->SpecsDirty && features_scanned == true && table_id_ == "FeaturesExplorerWindow" &&
            !std::all_of(feature_table_entries.begin(), feature_table_entries.end(),
                         [&col_idx](ImTableEntry& entry){ return !std::strcmp(entry.Headers[col_idx], "true") ? true : false; }))
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
