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

#include <SmartPeak/ui/ExplorerWidget.h>

namespace SmartPeak
{

  void ExplorerWidget::draw()
  {
    if (table_data_.headers_.size() <= 0)
      return;

    // headers
    const ImGuiTableFlags table_flags = ImGuiTableFlags_Resizable | ImGuiTableFlags_Hideable |
      ImGuiTableFlags_NoBordersInBody | ImGuiTableFlags_ScrollY | ImGuiTableFlags_NoSavedSettings |
      ImGuiTableFlags_Sortable | ImGuiTableFlags_SortMulti;

    static ImGuiTableColumnFlags column_0_flags = { ImGuiTableColumnFlags_DefaultSort | ImGuiTableColumnFlags_NoHide };
    static ImGuiTableColumnFlags column_any_flags = { ImGuiTableColumnFlags_NoHide };

    filter_.Draw("Find");
    
    if (table_data_.body_.dimensions().TotalSize() > 0) {
      updateTableContents(table_entries_, table_scanned_,
        table_data_.body_, *checkbox_columns_);
    }

    // drop-down list for search field(s)
    cols_.resize(table_data_.headers_.size() + 1);
    for (size_t header_name = 0; header_name < table_data_.headers_.size() + 1; ++header_name) {
      if (header_name == 0)
      {
        cols_[header_name] = "All";
      }
      else if (header_name > 0)
      {
        cols_[header_name] = table_data_.headers_(header_name - 1).c_str();
      }
    }

    ImGui::Combo("In Column(s)", &selected_col_, cols_.data(), cols_.size());
    
    for (uint checkbox_header_idx = 0; checkbox_header_idx < checkbox_headers_.size(); ++checkbox_header_idx) {
      if (checkbox_headers_(checkbox_header_idx) == "plot") checkbox_columns_plot_col_ = checkbox_header_idx;
    }
    table_entries_plot_col_ = (table_data_.headers_.size() + checkbox_columns_plot_col_);
    
    ImGui::PushButtonRepeat(true);
    if (ImGui::ArrowButton("##left_arrow_plotter", ImGuiDir_Left) && plot_idx_ > 0) {
      plot_switch_ = "stepper";
      plot_idx_--;
      if (!std::strcmp(table_entries_[plot_idx_].entry_contents[table_entries_plot_col_].c_str(), "false")
          && table_scanned_ && plot_idx_ < table_entries_.size()) {
        table_entries_[plot_idx_].entry_contents[table_entries_plot_col_] = "true";
        (*checkbox_columns_)(table_entries_[plot_idx_].ID, checkbox_columns_plot_col_) = true;
        if (!std::strcmp(table_entries_[plot_idx_+1].entry_contents[table_entries_plot_col_].c_str(), "false")) {
          table_entries_[plot_idx_+1].entry_contents[table_entries_plot_col_] = "true";
          (*checkbox_columns_)(table_entries_[plot_idx_+1].ID, checkbox_columns_plot_col_) = true;
        } else if (!std::strcmp(table_entries_[plot_idx_+1].entry_contents[table_entries_plot_col_].c_str(), "true")) {
          table_entries_[plot_idx_+1].entry_contents[table_entries_plot_col_] = "false";
          (*checkbox_columns_)(table_entries_[plot_idx_+1].ID, checkbox_columns_plot_col_) = false;
        }
      } else if (!std::strcmp(table_entries_[plot_idx_].entry_contents[table_entries_plot_col_].c_str(), "true")
               && table_scanned_ && plot_idx_ < table_entries_.size()) {
        if (!plot_unplot_all_deactivated_) {
          table_entries_[plot_idx_].entry_contents[table_entries_plot_col_] = "false";
          (*checkbox_columns_)(table_entries_[plot_idx_].ID, checkbox_columns_plot_col_) = false;
        }
        if (!std::strcmp(table_entries_[plot_idx_+1].entry_contents[table_entries_plot_col_].c_str(), "true")) {
          table_entries_[plot_idx_+1].entry_contents[table_entries_plot_col_] = "false";
          (*checkbox_columns_)(table_entries_[plot_idx_+1].ID, checkbox_columns_plot_col_) = false;
        } else if (!std::strcmp(table_entries_[plot_idx_+1].entry_contents[table_entries_plot_col_].c_str(), "false")
                   && !plot_unplot_all_deactivated_) {
          table_entries_[plot_idx_+1].entry_contents[table_entries_plot_col_] = "true";
          (*checkbox_columns_)(table_entries_[plot_idx_+1].ID, checkbox_columns_plot_col_) = true;
        }
      }
    }
    ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
    if (ImGui::ArrowButton("##right_arrow_plotter", ImGuiDir_Right) && plot_idx_ < (int)table_entries_.size() - 1) {
      plot_switch_ = "stepper";
      plot_idx_++;
      if (!std::strcmp(table_entries_[plot_idx_].entry_contents[table_entries_plot_col_].c_str(), "false")
          && table_scanned_ && plot_idx_ >= 0) {
        table_entries_[plot_idx_].entry_contents[table_entries_plot_col_] = "true";
        (*checkbox_columns_)(table_entries_[plot_idx_].ID, checkbox_columns_plot_col_) = true;
        if (plot_idx_ != 0)
        {
          if (!std::strcmp(table_entries_[plot_idx_-1].entry_contents[table_entries_plot_col_].c_str(), "false")) {
            table_entries_[plot_idx_-1].entry_contents[table_entries_plot_col_] = "true";
            (*checkbox_columns_)(table_entries_[plot_idx_-1].ID, checkbox_columns_plot_col_) = true;
          } else if (!std::strcmp(table_entries_[plot_idx_-1].entry_contents[table_entries_plot_col_].c_str(), "true")) {
            table_entries_[plot_idx_-1].entry_contents[table_entries_plot_col_] = "false";
            (*checkbox_columns_)(table_entries_[plot_idx_-1].ID, checkbox_columns_plot_col_) = false;
          }
        }
      } else if (!std::strcmp(table_entries_[plot_idx_].entry_contents[table_entries_plot_col_].c_str(), "true")
               && table_scanned_ && plot_idx_ > 0) {
        if (!plot_unplot_all_deactivated_) {
          table_entries_[plot_idx_].entry_contents[table_entries_plot_col_] = "false";
          (*checkbox_columns_)(table_entries_[plot_idx_].ID, checkbox_columns_plot_col_) = false;
        }
        if (plot_idx_ != 0)
        {
          if (!std::strcmp(table_entries_[plot_idx_-1].entry_contents[table_entries_plot_col_].c_str(), "true")) {
            table_entries_[plot_idx_-1].entry_contents[table_entries_plot_col_] = "false";
            (*checkbox_columns_)(table_entries_[plot_idx_-1].ID, checkbox_columns_plot_col_) = false;
          } else if (!std::strcmp(table_entries_[plot_idx_-1].entry_contents[table_entries_plot_col_].c_str(), "false")
                     && !plot_unplot_all_deactivated_) {
            table_entries_[plot_idx_-1].entry_contents[table_entries_plot_col_] = "true";
            (*checkbox_columns_)(table_entries_[plot_idx_-1].ID, checkbox_columns_plot_col_) = true;
          }
        }
      }
    }
    ImGui::PopButtonRepeat();
    ImGui::SameLine();
    ImGui::Text("Plot-Stepper");

    ImGui::SameLine();
    if (ImGui::Checkbox("Plot/Unplot All", &plot_all_)) { plot_unplot_all_deactivated_ = false; };
    if (plot_all_) { plot_switch_ = "plotunplotall"; plot_idx_ = 0; }
    if (table_scanned_ && checkbox_columns_->size() > 0 && plot_switch_ != "stepper") {
      if (plot_all_ && !plot_unplot_all_deactivated_) {
        std::for_each(table_entries_.begin(), table_entries_.end(),
                      [&](ImEntry& entry) {
                          entry.entry_contents[table_entries_plot_col_] = "true";
                          (*checkbox_columns_)(entry.ID, checkbox_columns_plot_col_) = true;
         });
      } else if (!plot_all_ && !plot_unplot_all_deactivated_) {
        std::for_each(table_entries_.begin(), table_entries_.end(),
                      [&](ImEntry& entry) {
                          entry.entry_contents[table_entries_plot_col_] = "false";
                          (*checkbox_columns_)(entry.ID, checkbox_columns_plot_col_) = false;
         });
      }
    }

    // restore serialized checkboxes
    auto nb_headers = checkbox_headers_.dimension(0);
    if (serialized_checkboxes_.size())
    {
      for (int h = 0; h < nb_headers; ++h)
      {
        size_t i = 0;
        for (const auto& b : serialized_checkboxes_.at(h))
        {
          table_entries_[i].entry_contents[h+2] = (b ? "true" : "false");
          (*checkbox_columns_)(table_entries_[i].ID, h) = b;
          ++i;
        }
      }
      plot_unplot_all_deactivated_ = true;
      serialized_checkboxes_.clear();
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
            if (searcher(table_entries_, selected_col_, filter_, row))
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
                    plot_unplot_all_deactivated_ = true;
                  }
                  else if (is_checked == false && !std::strcmp(table_entries_[row].entry_contents[header_idx].c_str(), "true"))
                  {
                    table_entries_[row].entry_contents[header_idx] = "false";
                    (*checkbox_columns_)(table_entries_[row].ID, checkbox_idx) = false;
                    plot_unplot_all_deactivated_ = true;
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

  void ExplorerWidget::onCheckboxesChanged()
  {
    table_scanned_ = false;
    plot_unplot_all_deactivated_ = true;
  }

  std::map<std::string, CastValue::Type> ExplorerWidget::getPropertiesSchema() const
  {
    auto properties = Widget::getPropertiesSchema();
    // checkboxes
    auto nb_headers = checkbox_headers_.dimension(0);
    for (int h = 0; h < nb_headers; ++h)
    {
      const auto& header_name = checkbox_headers_(h);
      properties.emplace(header_name, CastValue::Type::BOOL_LIST);
    }
    return properties;
  }

  std::optional<CastValue> ExplorerWidget::getProperty(const std::string& property, const size_t row) const
  {
    auto widget_property = Widget::getProperty(property, row);
    if (widget_property)
    {
      return widget_property;
    }
    // checkboxes
    auto nb_headers = checkbox_headers_.dimension(0);
    for (int h = 0; h < nb_headers; ++h)
    {
      const auto& header_name = checkbox_headers_(h);
      if (property == header_name)
      {
        if (checkbox_columns_)
        {
          std::vector<bool> checkoxes_plot;
          auto nb_checkboxes_plot = (*checkbox_columns_).dimension(0);
          for (int i = 0; i < nb_checkboxes_plot; ++i)
          {
            bool checked = (*checkbox_columns_)(i, h);
            checkoxes_plot.push_back(checked);
          }
          return checkoxes_plot;
        }
      }
    }
    return std::nullopt;
  }

  void ExplorerWidget::setProperty(const std::string& property, const CastValue& value, const size_t row)
  {
    Widget::setProperty(property, value, row);
    // checkboxes
    auto nb_headers = checkbox_headers_.dimension(0);
    for (int h = 0; h < nb_headers; ++h)
    {
      const auto& header_name = checkbox_headers_(h);
      if (property == header_name)
      {
        serialized_checkboxes_.insert_or_assign(h, value.bl_);
        for (int i = 0; i < value.bl_.size(); ++i)
        {
          (*checkbox_columns_)(i, h) = value.bl_[i];
        }
      }
    }
  }

  void ExplorerWidget::onSequenceUpdated()
  {
    table_scanned_ = false;
  }

  void ExplorerWidget::onFeaturesUpdated()
  {
    data_changed_ = true;
  }

}
