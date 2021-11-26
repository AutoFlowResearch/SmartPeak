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

#include <SmartPeak/ui/WorkflowWidget.h>
#include <imgui.h>
#include <imgui_internal.h>
#include <plog/Log.h>
#include <SmartPeak/core/SharedProcessors.h>

namespace SmartPeak
{
  void WorkflowWidget::draw()
  {

    workflow_step_widget_.draw();
    bool editable = workflow_manager_.isWorkflowDone();
    if (editable && ImGui::BeginCombo("Presets", NULL))
    {
      static const char* presets[] = {
        "DDA",
        "LCMS MRM Unknowns",
        "LCMS MRM Standards",
        "HPLC UV Unknowns",
        "HPLC UV Standards",
        "GCMS SIM Unknowns",
        "GCMS Full Scan Unknowns",
        "LCMS MRM Validation - QMIP",
        "LCMS MRM Validation - LP",
        "FIAMS Unknowns"
      };
      for (const char* s : presets)
      {
        if (ImGui::Selectable(s))
        {
          std::vector<std::string> ids;
          const std::string s_string { s };
          if (s_string == "DDA")
            ids = { "LOAD_RAW_DATA",
                    "PICK_3D_FEATURES",
                    "SEARCH_SPECTRUM",
                    "DDA",
                    "STORE_FEATURES"};
          else if (s_string == "LCMS MRM Unknowns")
            ids = { "LOAD_RAW_DATA", 
                    "MAP_CHROMATOGRAMS",
                    "PICK_MRM_FEATURES",
                    "QUANTIFY_FEATURES",
                    "CHECK_FEATURES",
                    "SELECT_FEATURES",
                    "STORE_FEATURES" };
          else if (s_string == "LCMS MRM Standards")
            ids = { "LOAD_RAW_DATA",
                    "MAP_CHROMATOGRAMS",
                    "PICK_MRM_FEATURES",
                    "CHECK_FEATURES",
                    "SELECT_FEATURES",
                    "CALCULATE_CALIBRATION",
                    "STORE_QUANTITATION_METHODS",
                    "QUANTIFY_FEATURES",
                    "STORE_FEATURES" };
          else if (s_string == "HPLC UV Unknowns")
            ids = { "LOAD_RAW_DATA",
                    "MAP_CHROMATOGRAMS",
                    "EXTRACT_CHROMATOGRAM_WINDOWS",
                    "ZERO_CHROMATOGRAM_BASELINE",
                    "PICK_MRM_FEATURES",
                    "QUANTIFY_FEATURES",
                    "CHECK_FEATURES",
                    "SELECT_FEATURES",
                    "STORE_FEATURES" };
          else if (s_string == "HPLC UV Standards")
            ids = { "LOAD_RAW_DATA",
                    "MAP_CHROMATOGRAMS",
                    "EXTRACT_CHROMATOGRAM_WINDOWS",
                    "ZERO_CHROMATOGRAM_BASELINE",
                    "PICK_MRM_FEATURES",
                    "CHECK_FEATURES",
                    "SELECT_FEATURES",
                    "CALCULATE_CALIBRATION",
                    "STORE_QUANTITATION_METHODS",
                    "QUANTIFY_FEATURES",
                    "STORE_FEATURES" };
          else if (s_string == "GCMS SIM Unknowns")
            ids = { "LOAD_RAW_DATA",
                    "MAP_CHROMATOGRAMS",
                    "EXTRACT_CHROMATOGRAM_WINDOWS",
                    "ZERO_CHROMATOGRAM_BASELINE",
                    "PICK_MRM_FEATURES",
                    "QUANTIFY_FEATURES",
                    "CHECK_FEATURES",
                    "SELECT_FEATURES",
                    "STORE_FEATURES" };
          else if (s_string == "GCMS Full Scan Unknowns")
            ids = { "LOAD_RAW_DATA",
                    "MAP_CHROMATOGRAMS",
                    "EXTRACT_CHROMATOGRAM_WINDOWS",
                    "ZERO_CHROMATOGRAM_BASELINE",
                    "PICK_MRM_FEATURES",
                    "QUANTIFY_FEATURES",
                    "CHECK_FEATURES",
                    "SELECT_FEATURES",
                    "STORE_FEATURES" };
          else if (s_string == "LCMS MRM Validation - LP")
            ids = { "LOAD_RAW_DATA",
                    "MAP_CHROMATOGRAMS",
                    "EXTRACT_CHROMATOGRAM_WINDOWS",
                    "PICK_MRM_FEATURES",
                    "FILTER_FEATURES",
                    "FILTER_FEATURES",
                    "SELECT_FEATURES",
                    "VALIDATE_FEATURES",
                    "STORE_FEATURES" };
          else if (s_string == "LCMS MRM Validation - QMIP")
            ids = { "LOAD_RAW_DATA",
                    "MAP_CHROMATOGRAMS",
                    "PICK_MRM_FEATURES",
                    "FILTER_FEATURES",
                    "FILTER_FEATURES",
                    "VALIDATE_FEATURES",
                    "STORE_FEATURES" };
          else if (s_string == "FIAMS Unknowns")
            ids = { "LOAD_RAW_DATA",
                    "EXTRACT_SPECTRA_WINDOWS",
                    "MERGE_SPECTRA",
                    "PICK_2D_FEATURES",
                    "SEARCH_ACCURATE_MASS",
                    "STORE_ANNOTATIONS",
                    "STORE_FEATURES",
                    "ESTIMATE_FEATURE_BACKGROUND_INTERFERENCES",
                    "STORE_FEATURE_BACKGROUND_ESTIMATIONS",
                    "FILTER_FEATURES_BACKGROUND_INTERFERENCES",
                    "MERGE_FEATURES",
                    "MERGE_INJECTIONS",
                    "STORE_FEATURES_SAMPLE_GROUP" };
          application_handler_.sequenceHandler_.setWorkflow(ids);
          application_handler_.sequenceHandler_.notifyWorkflowUpdated();
          LOGI << "Local workflow has been replaced";
        }
      }
      ImGui::EndCombo();
    }
    if (editable)
    {
      if (ImGui::Button("Add step"))
      {
        ImGui::OpenPopup("Add workflow step");
      }
      ImGui::SameLine();
      if (ImGui::Button("Reset"))
      {
        application_handler_.sequenceHandler_.getWorkflow().clear();
      }
    }

    ImGui::Separator();
    ImGui::Text("Steps");
    ImGui::Separator();

    ImGui::BeginChild("Workflow Steps");

    if (application_handler_.sequenceHandler_.getWorkflow().empty())
    {
      ImGui::Text("No steps set. Please select a preset and/or add a single method step.");
    }
    else
    {
      updatecommands();
      if (!error_building_commands_)
      {
        int i = 0;
        for (const auto& command: buildCommandsFromNames_.commands_) {
          ImGui::PushID(i + 1 ); // avoid hashing an id := 0, not sure it's necessary
          std::ostringstream os;
          os << "[" << (i + 1) << "] " << command.getName().c_str();
          ImGui::Button(os.str().c_str());
          if (editable && ImGui::BeginDragDropSource())
          {
            ImGui::SetDragDropPayload("DND_STEP", &i, sizeof(int));
            ImGui::Text("Moving %s", application_handler_.sequenceHandler_.getWorkflow().at(i).c_str());
            ImGui::EndDragDropSource();
          }
          else if (ImGui::IsItemHovered())
          {
            ImGui::BeginTooltip();
            ImGui::Text("%s", command.getDescription().c_str());
            ImGui::EndTooltip();
          }
          if (editable && ImGui::BeginDragDropTarget())
          {
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_STEP"))
            {
              IM_ASSERT(payload->DataSize == sizeof(int));
              int source_n = *(const int*)payload->Data;
              const auto tmp = application_handler_.sequenceHandler_.getWorkflow().at(source_n);
              application_handler_.sequenceHandler_.getWorkflow().erase(application_handler_.sequenceHandler_.getWorkflow().cbegin() + source_n);
              application_handler_.sequenceHandler_.getWorkflow().insert(application_handler_.sequenceHandler_.getWorkflow().cbegin() + i, tmp);
            }
            ImGui::EndDragDropTarget();
          }
          if (editable)
          {
            ImGui::SameLine();
          }
          if (editable && ImGui::Button("x")) {
            application_handler_.sequenceHandler_.getWorkflow().erase(application_handler_.sequenceHandler_.getWorkflow().cbegin() + i);
            application_handler_.sequenceHandler_.notifyWorkflowUpdated();
          }
          else 
          {
            ++i;
          }
          ImGui::PopID();
        }
      }
      else
      {
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
        ImGui::Text("Unable to load workflow. Please check log.");
        ImGui::PopStyleColor();
      }
    }

    ImGui::EndChild();
  }

  void WorkflowWidget::updatecommands()
  {
    const auto& names = application_handler_.sequenceHandler_.getWorkflow();
    if (buildCommandsFromNames_.names_ != names)
    {
      buildCommandsFromNames_.names_ = names;
      error_building_commands_ = !buildCommandsFromNames_.process();
    }
  }

}
