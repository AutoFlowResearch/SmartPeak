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
// $Maintainer: Douglas McCloskey, Bertrand Boudaud, Ahmed Khalil $
// $Authors: Douglas McCloskey, Pasquale Domenico Colaianni $
// --------------------------------------------------------------------------

#include <SmartPeak/core/ApplicationProcessor.h>
#include <SmartPeak/ui/ParametersTableWidget.h>
#include <algorithm>
#include <map>
#include <string>
#include <utility>
#include <vector>
#include <imgui.h>

namespace SmartPeak
{

  ParametersTableWidget::ParametersTableWidget(SessionHandler& session_handler, ApplicationHandler& application_handler, const std::string& table_id, const std::string title)
    : Widget(title),
    session_handler_(session_handler),
    application_handler_(application_handler),
    table_id_(table_id),
    parameter_editor_widget_(application_handler)
  {
    application_handler_.sequenceHandler_.addParametersObserver(this);
    application_handler_.sequenceHandler_.addWorkflowObserver(this);
  };

  ParametersTableWidget::~ParametersTableWidget()
  {
    application_handler_.sequenceHandler_.removeParametersObserver(this);
    application_handler_.sequenceHandler_.removeWorkflowObserver(this);
  }

  void ParametersTableWidget::parametersUpdated()
  {
    refresh_needed_ = true;
  }

  void ParametersTableWidget::workflowUpdated()
  {
    refresh_needed_ = true;
  }

  void ParametersTableWidget::updateParametersTable()
  {
    parameters_.clear();
    if (application_handler_.sequenceHandler_.getSequence().size() > 0)
    {
      parameters_ = application_handler_.sequenceHandler_.getSequence().at(0).getRawData().getParameters();
      std::vector<std::string> command_names = application_handler_.sequenceHandler_.getWorkflow();
      BuildCommandsFromNames buildCommandsFromNames(application_handler_);
      buildCommandsFromNames.names_ = command_names;
      buildCommandsFromNames.process();

      // Construct schema based on the current workflow's command list
      ParameterSet schema_params;
      for (const auto& command : buildCommandsFromNames.commands_)
      {
        schema_params.merge(command.getParameterSchema());
      }
      schema_params.merge(ApplicationProcessors::getParameterSchema()); // Application processor will be used also
      schema_params.setAsSchema(true);

      // Construct parameters merging schema and user defined
      parameters_.merge(schema_params);
    }
  }

  void ParametersTableWidget::draw()
  {
    showQuickHelpToolTip("ParametersTableWidget");
    
    if (application_handler_.sequenceHandler_.getSequence().size() == 0) {
      return;
    }
    parameter_editor_widget_.draw();
    if (refresh_needed_)
    {
      updateParametersTable();
      refresh_needed_ = false;
    }
    ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetStyle().Colors[ImGuiCol_TextDisabled]);
    ImGui::Checkbox("Show default, not set by user", &show_default_);
    ImGui::PopStyleColor();
    ImGui::SameLine();
    ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetStyle().Colors[ImGuiCol_TabActive]);
    ImGui::Checkbox("Show unused by the workflow", &show_unused_);
    ImGui::PopStyleColor();
    ImGui::SameLine();
    int parameters_count = 0;
    for (const auto& parameter_function : parameters_)
      for (const auto& parameter : parameter_function.second)
        parameters_count++;
    std::ostringstream os;
    os << " | " << parameters_count << " parameters.";
    ImGui::Text("%s", os.str().c_str());

    // headers
    const ImGuiTableFlags table_flags = ImGuiTableFlags_Resizable | ImGuiTableFlags_Hideable | ImGuiTableFlags_Reorderable |
      ImGuiTableFlags_NoBordersInBody | ImGuiTableFlags_ScrollY | ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_NoSavedSettings |
      ImGuiTableFlags_Sortable | ImGuiTableFlags_SortMulti;
    enum
    {
      EFunctionColumn,
      ENameColumn,
      ETypeColumn,
      EValueColumn,
      ERestrictionColumn,
      ELastColumn
    } ParametersTableWidgetHeader;

    // Edit Parameter widget needs to be open outside the table construction.
    const Parameter  *param_to_edit = nullptr;
    std::string parameter_to_edit_function;
    
    static ImGuiTextFilter filter;
    filter.Draw("Find");
    
    std::vector<const char*> searcher_dropdown_items_;
    searcher_dropdown_items_.resize(header_names_.size() + 1);
    for (size_t header_name = 0; header_name < header_names_.size() + 1; ++header_name) {
      if (header_name == 0)
      {
        searcher_dropdown_items_[header_name] = "All";
      }
      else if (header_name > 0)
      {
        searcher_dropdown_items_[header_name] = header_names_.at(header_name - 1).c_str();
      }
    }

    ImGui::Combo("In Column(s)", &selected_col_, searcher_dropdown_items_.data(), searcher_dropdown_items_.size());

    if (ImGui::BeginTable(table_id_.c_str(), header_names_.size(), table_flags))
    {
      // First row header_names_
      for (auto header : header_names_) {
        ImGui::TableSetupColumn(header.c_str());
      }
      ImGui::TableSetupScrollFreeze(header_names_.size(), 1);
      ImGui::TableHeadersRow();
      
      // ParameterSet to vec<ImEntry>
      if (parameters_.size() > 0 && is_scanned_ == false) {
        parameters_table_entries_.resize(parameters_count, ImEntry());
        size_t sub_param_idx = 0;
        for (auto it = parameters_.begin(); it != parameters_.end(); it++) {
          auto params = it->second.getParameters();
          for (size_t param_val_idx = 0; param_val_idx < params.size(); ++param_val_idx) {
            ImEntry& param_entry = parameters_table_entries_[sub_param_idx];
            param_entry.entry_contents.resize(header_names_.size(), "");
            param_entry.ID = sub_param_idx;
            param_entry.entry_contents[0] = it->first;
            std::vector<std::string> parameter_row_vals {getAllValues_(&params[param_val_idx])};
            for (int i = 0; i < parameter_row_vals.size(); i++)
            {
              param_entry.entry_contents[i+1] = parameter_row_vals[i];
            }
            sub_param_idx++;
          }
        }
        is_scanned_ = true;
      }

      // Second row to end body
      if (parameters_table_entries_.size() > 0)
      {
        for (auto param_idx = 0; param_idx < parameters_table_entries_.size(); param_idx++)
        {
          {
            const std::string function_parameter_name = parameters_table_entries_[param_idx].entry_contents[0];
            const std::string parameter_name = parameters_table_entries_[param_idx].entry_contents[1];
            auto parameter = parameters_.findParameter(function_parameter_name, parameter_name);
            
            // compute status
            enum
            {
              EUserOverride,
              EDefault,
              EUnused
            } OverrideStatus;
            int status = EUnused;
            if (parameter->getSchema())
            {
              status = EUserOverride;
            }
            else if (parameter->isSchema())
            {
              status = EDefault;
            }
            const bool valid = parameter->isValid();
            if ((status == EUserOverride) || (show_unused_ && (status == EUnused)) || (show_default_ && (status == EDefault)))
            {
              std::vector<std::string> parameter_row_vals {getAllValues_(parameter)};
              parameter_row_vals.insert(parameter_row_vals.begin(), function_parameter_name);

              if (selected_col_ > 0)
              {
                if (!filter.PassFilter(parameter_row_vals[selected_col_-1].c_str()))
                  continue;
              }
              else if (selected_col_ == 0) //ALL
              {
                if (std::all_of(parameter_row_vals.begin(), parameter_row_vals.end(),
                                [&](auto param_to_filter) { return !filter.PassFilter(param_to_filter.c_str()); }))
                  continue;
              }
              
              ImGui::TableNextRow();
              for (int col = 0; col < ELastColumn; ++col) {
                ImGui::TableSetColumnIndex(col);
                // text color
                if (status == EUserOverride)
                {
                  if ((col == EValueColumn) && (!valid))
                  {
                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
                  }
                  else
                  {
                    ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetStyle().Colors[ImGuiCol_Text]);
                  }
                }
                else if (status == EUnused)
                {
                  ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetStyle().Colors[ImGuiCol_TabActive]);
                }
                else if (status == EDefault)
                {
                  ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetStyle().Colors[ImGuiCol_TextDisabled]);
                }
                // text
                ImGui::Text("%s", parameters_table_entries_[param_idx].entry_contents[col].c_str());
                ImGui::PopStyleColor();
                // edit value
                if (col == EValueColumn)
                {
                  if (ImGui::IsItemHovered() && (!valid))
                  {
                    ImGui::BeginTooltip();
                    const std::string constraints = parameter->getRestrictionsAsString();
                    const std::string expected_type = parameter->getType();
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
                if (col == EValueColumn)
                {
                  if (ImGui::IsItemHovered())
                  {
                    ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
                  }
                  if (ImGui::IsItemClicked())
                  {
                    param_to_edit = parameter;
                    parameter_to_edit_function = function_parameter_name;
                  }
                }
                // tooltip
                if (col == ENameColumn)
                {
                  if (ImGui::IsItemHovered())
                  {
                    const std::string& description = parameter->getDescription();
                    ImGui::BeginTooltip();
                    ImGui::Text("%s", description.c_str());
                    ImGui::EndTooltip();
                  }
                }
              }
            }
          }
        }
      }
      
      if (ImGuiTableSortSpecs* sorts_specs = ImGui::TableGetSortSpecs())
      {
        if (sorts_specs->SpecsDirty && is_scanned_)
        {
          ImEntry::s_current_sort_specs = sorts_specs;
          if (parameters_table_entries_.size() > 1)
            qsort(&parameters_table_entries_[0], (size_t)parameters_table_entries_.size(), sizeof(parameters_table_entries_[0]),
                  ImEntry::CompareWithSortSpecs);
          ImEntry::s_current_sort_specs = NULL;
          sorts_specs->SpecsDirty = false;
        }
      }
      
      if (is_scanned_ && parameter_editor_widget_.isTableScanRequired()) {
        is_scanned_ = false;
        parameter_editor_widget_.setTableScanNotRequired();
      }
    }
    
    ImGui::EndTable();
    if (param_to_edit)
    {
      parameter_editor_widget_.setParameter(parameter_to_edit_function, *param_to_edit);
      ImGui::OpenPopup("Edit Parameter");
    }
  }

}
