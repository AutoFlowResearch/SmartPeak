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
// $Authors: Douglas McCloskey, Bertrand Boudaud $
// --------------------------------------------------------------------------

#include <SmartPeak/ui/PresetWorkflowWidget.h>
#include <imgui.h>
#include <plog/Log.h>
#include <SmartPeak/core/SharedProcessors.h>
#include <SmartPeak/core/ApplicationProcessors/BuildCommandsFromNames.h>
#include <SmartPeak/core/SharedProcessors.h>
#include <SmartPeak/PresetWorkflows/PresetWorkflow.h>

namespace SmartPeak
{
  void PresetWorkflowWidget::draw()
  {
    showQuickHelpToolTip("WorkflowStepWindow");

    const float popup_width = 800;
    const float popup_height = 600;
    const float buttons_space = 50;

    static const char* raw_data_method_type_ = "Raw data methods";
    static const char* seq_seg_method_type_ = "Sequence segment methods";
    static const char* sample_group_method_type_ = "Sample group methods";

    ImGui::SetNextWindowSize(ImVec2(popup_width, popup_height));
    if (!ImGui::BeginPopupModal("Select Preset Workflow", NULL)) {
      return;
    }

    ImVec2 vMin = ImGui::GetWindowContentRegionMin();
    ImVec2 vMax = ImGui::GetWindowContentRegionMax();

    ImVec2 preset_list_size = ImVec2(vMax.x * 0.3, vMax.y - buttons_space);
    // compute the largest preset name
    for (const auto& preset : PresetWorkflow::all_presets_)
    {
      ImVec2 preset_text_size = ImGui::CalcTextSize(preset->getName().c_str());
      if (preset_text_size.x > preset_list_size.x)
      {
        preset_list_size.x = preset_text_size.x;
      }
    }
    preset_list_size.x += ImGui::GetStyle().WindowPadding.x * 2;
    ImGui::BeginChild(ImGui::GetID("preset_list"), preset_list_size, ImGuiWindowFlags_NoMove);
    unsigned int counter = 0;
    for (const auto& preset : PresetWorkflow::all_presets_)
    {
      if (ImGui::Selectable(preset->getName().c_str()))
      {
        selected_preset_index_ = counter;
      }
      ++counter;
    }
    ImGui::EndChild();

    const auto& current_preset = PresetWorkflow::all_presets_.at(selected_preset_index_);

    ImVec2 preset_description_size = ImVec2(vMax.x - (preset_list_size.x + ImGui::GetStyle().WindowPadding.x * 2),
                                            vMax.y - buttons_space);
    auto window_pos = ImGui::GetCurrentWindow()->Pos;
    ImGui::SetNextWindowPos(ImVec2((preset_list_size.x + ImGui::GetStyle().WindowPadding.x * 2) + window_pos.x, 
                            0 + window_pos.y + vMin.y));
    ImGui::BeginChild(ImGui::GetID("preset_description"), preset_description_size, ImGuiWindowFlags_NoMove);
    ImGui::PushTextWrapPos();
    ImGui::Text(current_preset->getName().c_str());
    ImGui::Separator();
    ImGui::PushTextWrapPos();
    ImGui::Text(current_preset->getDescription().c_str());
    ImGui::EndChild();

    if (ImGui::Button("Ok"))
    {
      preset_workflow_observer_.onPresetWorkflowSelected(*current_preset);
      ImGui::CloseCurrentPopup();
    }

    ImGui::SameLine();
    if (ImGui::Button("Cancel"))
    {
      ImGui::CloseCurrentPopup();
    }

    ImGui::EndPopup();
  }

}
