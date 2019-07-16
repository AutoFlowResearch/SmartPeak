#include <SmartPeak/ui/Workflow.h>
#include <SmartPeak/core/AppStateProcessor.h>
#include <imgui.h>
#include <imgui_internal.h>
#include <plog/Log.h>

namespace SmartPeak
{
  void Workflow::draw()
  {
    ImGui::OpenPopup("Select workflow's steps");

    if (!state_)
    {
      LOGE << "Workflow widget has no AppState object associated with it";
      draw_ = false; // to avoid flooding the log
      return;
    }

    if (!ImGui::BeginPopupModal("Select workflow's steps", NULL, ImGuiWindowFlags_NoResize)) {
      return;
    }

    if (ImGui::BeginCombo("Presets", NULL))
    {
      AppState::Command cmd;
      CreateCommand createCommand(*state_);
      const char* presets[] = {
        "LCMS MRM Unknowns",
        "LCMS MRM Standards",
        "HPLC UV Unknowns",
        "HPLC UV Standards",
        "GCMS SIM Unknowns",
        "GCMS Full Scan Unknowns",
        "LCMS MRM Validation"
      };
      for (const char* s : presets)
      {
        if (ImGui::Selectable(s))
        {
          BuildCommandsFromIds buildCommandsFromIds(*state_);
          std::string ids;
          if (std::string(s) == "LCMS MRM Unknowns")
            ids = "1 11 3 7 8 5 9";
          if (std::string(s) == "LCMS MRM Standards")
            ids = "1 11 3 8 5 14 15 7 9";
          if (std::string(s) == "HPLC UV Unknowns")
            ids = "1 11 13 12 3 7 8 5 9";
          if (std::string(s) == "HPLC UV Standards")
            ids = "1 11 13 12 3 8 5 14 15 7 9";
          if (std::string(s) == "GCMS SIM Unknowns")
            ids = "1 11 13 12 3 7 8 5 9";
          if (std::string(s) == "GCMS Full Scan Unknowns")
            ids = "1 11 13 12 3 8 5 14 15 7 9";
          if (std::string(s) == "LCMS MRM Validation")
            ids = "1 11 13 3 4 4 5 6 9";
          commands_ = buildCommandsFromIds(ids);
          LOGI << "Local workflow has been replaced";
        }
      }
      ImGui::EndCombo();
    }

    if (ImGui::BeginCombo("Add Raw data method", NULL))
    {
      AppState::Command cmd;
      CreateCommand createCommand(*state_);
      for (const std::pair<int, std::shared_ptr<RawDataProcessor>>& p : state_->n_to_raw_data_method_)
      {
        if (ImGui::Selectable(p.second->getName().c_str()))
        {
          const bool created = createCommand(p.second->getID(), cmd);
          if (created) {
            commands_.push_back(cmd);
          }
        }
      }
      ImGui::EndCombo();
    }

    if (ImGui::BeginCombo("Add Sequence Segment method", NULL))
    {
      AppState::Command cmd;
      CreateCommand createCommand(*state_);
      for (const std::pair<int, std::shared_ptr<SequenceSegmentProcessor>>& p : state_->n_to_seq_seg_method_)
      {
        if (ImGui::Selectable(p.second->getName().c_str()))
        {
          const bool created = createCommand(p.second->getID(), cmd);
          if (created) {
            commands_.push_back(cmd);
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
          AppState::Command tmp = commands_.at(source_n);
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
      state_->commands_ = commands_;
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

  std::vector<AppState::Command> Workflow::getCommands() const
  {
    return commands_;
  }

  void Workflow::setState(AppState& state)
  {
    LOGD << "Setting state: " << (&state);
    state_ = &state;
  }
}
