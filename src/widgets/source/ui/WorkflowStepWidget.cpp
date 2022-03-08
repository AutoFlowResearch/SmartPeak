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
// $Authors: Douglas McCloskey, Pasquale Domenico Colaianni $
// --------------------------------------------------------------------------

#include <SmartPeak/ui/WorkflowStepWidget.h>
#include <imgui.h>
#include <plog/Log.h>
#include <SmartPeak/core/SharedProcessors.h>
#include <SmartPeak/core/ApplicationProcessors/BuildCommandsFromNames.h>
#include <SmartPeak/core/SharedProcessors.h>

namespace SmartPeak
{
  void WorkflowStepWidget::draw()
  {
    showQuickHelpToolTip("WorkflowStepWindow");

    float popup_width = 400;
    float description_box_height = 200;

    static const char* raw_data_method_type_ = "Raw data methods";
    static const char* seq_seg_method_type_ = "Sequence segment methods";
    static const char* sample_group_method_type_ = "Sample group methods";

    if (!ImGui::BeginPopupModal("Add workflow step", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
      return;
    }

    const char* method_types[]  = { raw_data_method_type_, seq_seg_method_type_, sample_group_method_type_ };
    ImGui::PushItemWidth(popup_width);
    if (ImGui::BeginCombo("Method Type", selected_method_type_.c_str()))
    {
      for (const auto& method_type : method_types)
      {
        if (ImGui::Selectable(method_type))
        {
          selected_method_type_ = method_type;
          selected_method_.clear();
        }
      }
      ImGui::EndCombo();
    }
    
    ImGui::PushItemWidth(popup_width);
    if (ImGui::BeginCombo("Method Name", selected_method_.c_str()))
    {
      if (std::string(selected_method_type_) == raw_data_method_type_)
      {
        for (const auto& p : n_to_raw_data_method_)
        {
          if (ImGui::Selectable(p.second->getName().c_str()))
          {
            selected_method_ = p.second->getName();
          }
        }
      }
      else if (std::string(selected_method_type_) == seq_seg_method_type_)
      {
        for (const auto& p : n_to_seq_seg_method_)
        {
          if (ImGui::Selectable(p.second->getName().c_str()))
          {
            selected_method_ = p.second->getName();
          }
        }
      }
      else if (std::string(selected_method_type_) == sample_group_method_type_)
      {
        for (const auto& p : n_to_sample_group_method_)
        {
          if (ImGui::Selectable(p.second->getName().c_str()))
          {
            selected_method_ = p.second->getName();
          }
        }
      }
      ImGui::EndCombo();
    }

    ImGui::Separator();

    bool command_success = false;
    ImGui::BeginChild("Description", ImVec2(popup_width, description_box_height));
    if (!selected_method_.empty())
    {
      ImGui::PushTextWrapPos();
      BuildCommandsFromNames buildCommandsFromNames(application_handler_);
      buildCommandsFromNames.names_ = { selected_method_ };
      if (buildCommandsFromNames.process() && buildCommandsFromNames.commands_.size() > 0)
      {
        ImGui::Text("%s", buildCommandsFromNames.commands_[0].getDescription().c_str());
        command_success = true;
      }
      else
      {
        ImGui::Text("An error occurred getting information about this command.");
        command_success = false;
      }
      ImGui::PopTextWrapPos();
    }
    ImGui::EndChild();

    ImGui::Separator();

    if (ImGui::Button("Ok"))
    {
      if (!selected_method_.empty() && command_success)
      {
        application_handler_.sequenceHandler_.getWorkflow().push_back(selected_method_);
        application_handler_.sequenceHandler_.notifyWorkflowUpdated();
      }
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
