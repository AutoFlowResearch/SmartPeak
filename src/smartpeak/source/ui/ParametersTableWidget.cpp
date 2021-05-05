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
// $Maintainer: Douglas McCloskey, Bertrand Boudaud $
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
    static const std::vector<std::string> headers = { "function", "name", "type", "value", "restrictions" };

    // Edit Parameter widget needs to be open outside the table construction.
    const Parameter  *param_to_edit = nullptr;
    std::string parameter_to_edit_function;

    if (ImGui::BeginTable(table_id_.c_str(), headers.size(), table_flags))
    {
      // First row headers
      for (auto header : headers) {
        ImGui::TableSetupColumn(header.c_str());
      }
      ImGui::TableSetupScrollFreeze(headers.size(), 1);
      ImGui::TableHeadersRow();

      // Second row to end body
      if (parameters_.size() > 0)
      {
        for (const auto& parameter_function : parameters_)
        {
          const std::string function_parameter_name = parameter_function.first;
          for (const auto& parameter : parameter_function.second)
          {
            // compute status
            enum
            {
              EUserOverride,
              EDefault,
              EUnused
            } OverrideStatus;
            int status = EUnused;
            if (parameter.getSchema())
            {
              status = EUserOverride;
            }
            else if (parameter.isSchema())
            {
              status = EDefault;
            }
            const bool valid = parameter.isValid();
            if ((status == EUserOverride) || (show_unused_ && (status == EUnused)) || (show_default_ && (status == EDefault)))
            {
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
                switch (col)
                {
                case EFunctionColumn:
                  ImGui::Text("%s", function_parameter_name.c_str());
                  break;
                case ENameColumn:
                  ImGui::Text("%s", parameter.getName().c_str());
                  break;
                case ETypeColumn:
                  ImGui::Text("%s", parameter.getType().c_str());
                  break;
                case EValueColumn:
                  ImGui::Text("%s", parameter.getValueAsString().c_str());
                  break;
                case ERestrictionColumn:
                  ImGui::Text("%s", parameter.getRestrictionsAsString().c_str());
                  break;
                default:
                  // other will not be displayed
                  break;
                }
                ImGui::PopStyleColor();
                // edit value
                if (col == EValueColumn)
                {
                  if (ImGui::IsItemHovered() && (!valid))
                  {
                    ImGui::BeginTooltip();
                    const std::string constraints = parameter.getRestrictionsAsString();
                    const std::string expected_type = parameter.getType();
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
                if (col == EValueColumn)
                {
                  if (ImGui::IsItemHovered())
                  {
                    ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
                  }
                  if (ImGui::IsItemClicked())
                  {
                    param_to_edit = &parameter;
                    parameter_to_edit_function = function_parameter_name;
                  }
                }
                // tooltip
                if (col == ENameColumn)
                {
                  if (ImGui::IsItemHovered())
                  {
                    const std::string& description = parameter.getDescription();
                    ImGui::BeginTooltip();
                    ImGui::Text(description.c_str());
                    ImGui::EndTooltip();
                  }
                }
              }
            }
          }
        }
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
