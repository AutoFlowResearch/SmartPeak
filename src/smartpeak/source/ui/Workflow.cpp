#include <SmartPeak/ui/Workflow.h>
#include <SmartPeak/core/ApplicationProcessor.h>
#include <imgui.h>
#include <imgui_internal.h>
#include <plog/Log.h>
#include <SmartPeak/core/SharedProcessors.h>

namespace SmartPeak
{
  void Workflow::draw()
  {
    ImGui::OpenPopup("Select workflow's steps");

    if (!application_handler_)
    {
      LOGE << "Workflow widget has no ApplicationHandler object associated with it";
      draw_ = false; // to avoid flooding the log
      return;
    }

    if (!ImGui::BeginPopupModal("Select workflow's steps", NULL, ImGuiWindowFlags_NoResize)) {
      return;
    }

    if (ImGui::BeginCombo("Presets", NULL))
    {
      ApplicationHandler::Command cmd;
      CreateCommand createCommand(*application_handler_);
      const char* presets[] = {
        "LCMS MRM Unknowns",
        "LCMS MRM Standards",
        "HPLC UV Unknowns",
        "HPLC UV Standards",
        "GCMS SIM Unknowns",
        "GCMS Full Scan Unknowns",
        "LCMS MRM Validation - QMIP",
        "LCMS MRM Validation - LP"
      };
      for (const char* s : presets)
      {
        if (ImGui::Selectable(s))
        {
          BuildCommandsFromNames buildCommandsFromIds(*application_handler_);
          std::string ids;
          const std::string s_string { s };
          if (s_string == "LCMS MRM Unknowns")
            ids = "LOAD_RAW_DATA MAP_CHROMATOGRAMS PICK_FEATURES QUANTIFY_FEATURES CHECK_FEATURES SELECT_FEATURES STORE_FEATURES";
          else if (s_string == "LCMS MRM Standards")
            ids = "LOAD_RAW_DATA MAP_CHROMATOGRAMS PICK_FEATURES CHECK_FEATURES SELECT_FEATURES CALCULATE_CALIBRATION STORE_QUANTITATION_METHODS QUANTIFY_FEATURES STORE_FEATURES";
          else if (s_string == "HPLC UV Unknowns")
            ids = "LOAD_RAW_DATA MAP_CHROMATOGRAMS EXTRACT_CHROMATOGRAM_WINDOWS ZERO_CHROMATOGRAM_BASELINE PICK_FEATURES QUANTIFY_FEATURES CHECK_FEATURES SELECT_FEATURES STORE_FEATURES";
          else if (s_string == "HPLC UV Standards")
            ids = "LOAD_RAW_DATA MAP_CHROMATOGRAMS EXTRACT_CHROMATOGRAM_WINDOWS ZERO_CHROMATOGRAM_BASELINE PICK_FEATURES CHECK_FEATURES SELECT_FEATURES CALCULATE_CALIBRATION STORE_QUANTITATION_METHODS QUANTIFY_FEATURES STORE_FEATURES";
          else if (s_string == "GCMS SIM Unknowns")
            ids = "LOAD_RAW_DATA MAP_CHROMATOGRAMS EXTRACT_CHROMATOGRAM_WINDOWS ZERO_CHROMATOGRAM_BASELINE PICK_FEATURES QUANTIFY_FEATURES CHECK_FEATURES SELECT_FEATURES STORE_FEATURES";
          else if (s_string == "GCMS Full Scan Unknowns")
            ids = "LOAD_RAW_DATA MAP_CHROMATOGRAMS EXTRACT_CHROMATOGRAM_WINDOWS ZERO_CHROMATOGRAM_BASELINE PICK_FEATURES CHECK_FEATURES SELECT_FEATURES CALCULATE_CALIBRATION STORE_QUANTITATION_METHODS QUANTIFY_FEATURES STORE_FEATURES";
          else if (s_string == "LCMS MRM Validation - LP")
            ids = "LOAD_RAW_DATA MAP_CHROMATOGRAMS EXTRACT_CHROMATOGRAM_WINDOWS PICK_FEATURES FILTER_FEATURES FILTER_FEATURES SELECT_FEATURES VALIDATE_FEATURES STORE_FEATURES";
          else if (s_string == "LCMS MRM Validation - QMIP")
            ids = "LOAD_RAW_DATA MAP_CHROMATOGRAMS PICK_FEATURES FILTER_FEATURES FILTER_FEATURES VALIDATE_FEATURES STORE_FEATURES";
          buildCommandsFromIds.names_ = ids;
          buildCommandsFromIds.process();
          commands_ = buildCommandsFromIds.commands_;
          LOGI << "Local workflow has been replaced";
        }
      }
      ImGui::EndCombo();
    }

    if (ImGui::BeginCombo("Add Raw data method", NULL))
    {
      ApplicationHandler::Command cmd;
      CreateCommand createCommand(*application_handler_);
      for (const auto& p : n_to_raw_data_method_)
      {
        if (ImGui::Selectable(p.second->getName().c_str()))
        {
          createCommand.name_ = p.second->getName();
          const bool created = createCommand.process();
          if (created) {
            commands_.push_back(createCommand.cmd_);
          }
        }
      }
      ImGui::EndCombo();
    }

    if (ImGui::BeginCombo("Add Sequence Segment method", NULL))
    {
      ApplicationHandler::Command cmd;
      CreateCommand createCommand(*application_handler_);
      for (const auto& p : n_to_seq_seg_method_)
      {
        if (ImGui::Selectable(p.second->getName().c_str()))
        {
          createCommand.name_ = p.second->getName();
          const bool created = createCommand.process();
          if (created) {
            commands_.push_back(createCommand.cmd_);
          }
        }
      }
      ImGui::EndCombo();
    }

    ImGui::Separator();
    ImGui::Text("Steps");
    ImGui::Separator();

    ImGui::BeginChild("Workflow Steps", ImVec2(600, 300));

    if (commands_.empty())
      ImGui::Text("No steps set. Please select a preset and/or add a single method step.");

    for (int i = 0; static_cast<size_t>(i) < commands_.size(); ) {
      ImGui::PushID(i + 1); // avoid hashing an id := 0, not sure it's necessary
      char step_label[256];
      sprintf(step_label, "[%02d] %s", i + 1, commands_.at(i).getName().c_str());
      ImGui::Button(step_label);
      if (ImGui::BeginDragDropSource())
      {
        ImGui::SetDragDropPayload("DND_STEP", &i, sizeof(int));
        ImGui::Text("Moving %s", commands_.at(i).getName().c_str());
        ImGui::EndDragDropSource();
      }
      if (ImGui::BeginDragDropTarget())
      {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_STEP"))
        {
          IM_ASSERT(payload->DataSize == sizeof(int));
          int source_n = *(const int*)payload->Data;
          ApplicationHandler::Command tmp = commands_.at(source_n);
          commands_.erase(commands_.cbegin() + source_n);
          commands_.insert(commands_.cbegin() + i, tmp);
        }
        ImGui::EndDragDropTarget();
      }
      ImGui::SameLine();
      if (ImGui::Button("x")) {
        commands_.erase(commands_.cbegin() + i);
      } else {
        ++i;
      }
      ImGui::PopID();
    }

    ImGui::EndChild();

    ImGui::Separator();

    if (ImGui::Button("Ok"))
    {
      application_handler_->commands_ = commands_;
      LOGI << "State of app has been updated with the new workflow.";
      draw_ = false;
      ImGui::CloseCurrentPopup();
    }

    ImGui::SameLine();
    if (ImGui::Button("Cancel"))
    {
      LOGI << "State of app has NOT been modified.";
      draw_ = false;
      ImGui::CloseCurrentPopup();
    }

    ImGui::SameLine();
    if (ImGui::Button("Reset"))
    {
      commands_.clear();
    }

    ImGui::EndPopup();
  }

  std::vector<ApplicationHandler::Command> Workflow::getCommands() const
  {
    return commands_;
  }

  void Workflow::setApplicationHandler(ApplicationHandler& state)
  {
    LOGD << "Setting state: " << (&state);
    application_handler_ = &state;
  }
}
