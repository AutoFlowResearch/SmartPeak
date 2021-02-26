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

  ParametersTableWidget::ParametersTableWidget(SessionHandler& session_handler, ApplicationHandler& application_handler, const std::string& table_id)
    : session_handler_(session_handler), application_handler_(application_handler), table_id_(table_id)
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

  void ParametersTableWidget::draw()
  {
    if (application_handler_.sequenceHandler_.getSequence().size() == 0) {
      return;
    }
    std::vector<std::string> command_names = application_handler_.sequenceHandler_.getWorkflow();
    ParameterSet user_parameters = application_handler_.sequenceHandler_.getSequence().at(0).getRawData().getParameters();
    if (refresh_needed_)
    {
      BuildCommandsFromNames buildCommandsFromNames(application_handler_);
      buildCommandsFromNames.names_ = command_names;
      buildCommandsFromNames.process(); 
      session_handler_.getParametersTable(user_parameters, buildCommandsFromNames.commands_, headers_, body_);
      refresh_needed_ = false;
    }
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
      if (body_.size() > 0) {
        for (size_t row = 0; row < body_.dimension(0); ++row) {
          const std::string& status = body_(row, 5);
          const std::string& valid = body_(row, 6);
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
                ImGui::Text("%s", body_(row, col).c_str());
                ImGui::PopStyleColor();
                if ((col == 3) && (valid == "false"))
                {
                  if (ImGui::IsItemHovered())
                  {
                    ImGui::BeginTooltip();
                    const std::string& constraints = body_(row, 7);
                    const std::string& expected_type = body_(row, 8);
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
                ImGui::Text("%s", body_(row, col).c_str());
                ImGui::PopStyleColor();
              }
              else if (status == "default")
              {
                ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetStyle().Colors[ImGuiCol_TextDisabled]);
                ImGui::Text("%s", body_(row, col).c_str());
                ImGui::PopStyleColor();
              }
              if (col == 1)
              {
                if (ImGui::IsItemHovered())
                {
                  const std::string& description = body_(row, 4);
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
      ImGui::EndTable();
    }
  }

}
