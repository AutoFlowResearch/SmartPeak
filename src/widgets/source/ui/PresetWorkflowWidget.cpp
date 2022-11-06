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

#include <SmartPeak/ui/PresetWorkflowWidget.h>
#include <imgui.h>
#include <plog/Log.h>
#include <SmartPeak/core/SharedProcessors.h>
#include <SmartPeak/core/ApplicationProcessors/BuildCommandsFromNames.h>
#include <SmartPeak/core/SharedProcessors.h>

#include <SmartPeak/core/PresetWorkflows/FIAMS_Unknowns.h>
#include <SmartPeak/core/PresetWorkflows/GCMS_Full_Scan_Unknowns.h>
#include <SmartPeak/core/PresetWorkflows/GCMS_SIM_Unknowns.h>
#include <SmartPeak/core/PresetWorkflows/HPLC_UV_Standards.h>
#include <SmartPeak/core/PresetWorkflows/HPLC_UV_Unknowns.h>
#include <SmartPeak/core/PresetWorkflows/LCMS_DDA_Spectra_Library_Construction.h>
#include <SmartPeak/core/PresetWorkflows/LCMS_DDA_Spectra_Library_Matching.h>
#include <SmartPeak/core/PresetWorkflows/LCMS_DDA_Transitions_Library_Construction.h>
#include <SmartPeak/core/PresetWorkflows/LCMS_MRM_Standards.h>
#include <SmartPeak/core/PresetWorkflows/LCMS_MRM_Unknowns.h>
#include <SmartPeak/core/PresetWorkflows/LCMS_MRM_Validation_LP.h>
#include <SmartPeak/core/PresetWorkflows/LCMS_MRM_Validation_QMIP.h>

namespace SmartPeak
{
  void PresetWorkflowWidget::draw()
  {
    showQuickHelpToolTip("WorkflowStepWindow");

    float popup_width = 400;
    float description_box_height = 200;

    static const char* raw_data_method_type_ = "Raw data methods";
    static const char* seq_seg_method_type_ = "Sequence segment methods";
    static const char* sample_group_method_type_ = "Sample group methods";

    if (!ImGui::BeginPopupModal("Select Preset Workflow", NULL)) {
      return;
    }

    static const std::vector<std::shared_ptr<PresetWorkflow>> presets = {
      std::make_shared<FIAMS_Unknowns>(),
      std::make_shared<GCMS_Full_Scan_Unknowns>(),
      std::make_shared<GCMS_SIM_Unknowns>(),
      std::make_shared<HPLC_UV_Standards>(),
      std::make_shared<HPLC_UV_Unknowns>(),
      std::make_shared<LCMS_DDA_Spectra_Library_Construction>(),
      std::make_shared<LCMS_DDA_Spectra_Library_Matching>(),
      std::make_shared<LCMS_DDA_Transitions_Library_Construction>(),
      std::make_shared<LCMS_MRM_Standards>(),
      std::make_shared<LCMS_MRM_Unknowns>(),
      std::make_shared<LCMS_MRM_Validation_LP>(),
      std::make_shared<LCMS_MRM_Validation_QMIP>()
    };
    const auto& current_preset = presets.at(selected_preset_index_);

    ImGui::PushTextWrapPos();
    ImGui::Text(current_preset->getName().c_str());

    ImGui::Separator();

    ImGui::PushTextWrapPos();
    ImGui::Text(current_preset->getDescription().c_str());

    ImGui::Separator();

    if (ImGui::Button("Previous"))
    {
      selected_preset_index_ = ((--selected_preset_index_) % presets.size());
    }
    ImGui::SameLine();
    if (ImGui::Button("Next"))
    {
      selected_preset_index_ = ((++selected_preset_index_) % presets.size());
    }

    ImGui::SameLine();
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
