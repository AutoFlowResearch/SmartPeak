#include <SmartPeak/ui/Report.h>
#include <SmartPeak/core/AppStateProcessor.h>
#include <SmartPeak/core/FeatureMetadata.h>
#include <SmartPeak/core/SampleType.h>
#include <SmartPeak/io/SequenceParser.h>
#include <algorithm>
#include <imgui.h>
#include <imgui_internal.h>
#include <plog/Log.h>

namespace SmartPeak
{
  Report::Report()
  {
    std::fill(st_checks_.begin(), st_checks_.end(), false);
    std::fill(md_checks_.begin(), md_checks_.end(), false);
  }

  void Report::draw()
  {
    ImGui::OpenPopup("Report dialog");

    if (!state_)
    {
      LOGE << "Report widget has no AppState object associated with it";
      draw_ = false; // to avoid flooding the log
      return;
    }

    if (!ImGui::BeginPopupModal("Report dialog", NULL, ImGuiWindowFlags_NoResize)) {
      return;
    }

    ImGui::BeginChild("ReportContent", ImVec2(600, 400));

    ImGui::Text("Select the information to export in the reports.");

    {
      ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 2.0f);
      ImGui::BeginChild("SampleTypesChild", ImVec2(ImGui::GetWindowContentRegionWidth() * 0.5f, 380), true);
      ImGui::Text("Sample types");
      ImGui::Separator();
      size_t i = 0;
      for (const std::pair<SampleType, std::string>& p : sampleTypeToString)
      {
        ImGui::Checkbox(p.second.c_str(), &st_checks_.at(i++));
      }
      ImGui::EndChild();
      ImGui::PopStyleVar();
    }
    ImGui::SameLine();
    {
      ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 2.0f);
      ImGui::BeginChild("MetadataChild", ImVec2(0, 380), true);
      ImGui::Text("Metadata");
      ImGui::Separator();
      size_t i = 0;
      for (const std::pair<FeatureMetadata, std::string>& p : metadataToString)
      {
        ImGui::Checkbox(p.second.c_str(), &md_checks_.at(i++));
      }
      ImGui::EndChild();
      ImGui::PopStyleVar();
    }

    ImGui::EndChild();

    ImGui::Separator();

    if (ImGui::Button("Create FeatureDB.csv"))
    {
      const bool checkboxes_check = initializeMetadataAndSampleTypes();
      if (checkboxes_check)
      {
        const std::string pathname = state_->main_dir_ + "/FeatureDB.csv";
        // TODO: IO operation -> use another thread
        const bool data_was_written = SequenceParser::writeDataTableFromMetaValue(
          state_->sequenceHandler_,
          pathname,
          summaryMetaData_,
          summarySampleTypes_
        );
        if (data_was_written) {
          LOGN << "FeatureDB.csv file has been stored at: " << pathname;
        } else {
          LOGE << "Error during write. FeatureDB.csv content is invalid.";
        }
      }
      else
      {
        LOGE << "Select at least one Sample Type and at least one Metadata";
      }
    }

    ImGui::SameLine();

    if (ImGui::Button("Create PivotTable.csv"))
    {
      const bool checkboxes_check = initializeMetadataAndSampleTypes();
      if (checkboxes_check)
      {
        const std::string pathname = state_->main_dir_ + "/PivotTable.csv";
        // TODO: IO operation -> use another thread
        const bool data_was_written = SequenceParser::writeDataMatrixFromMetaValue(
          state_->sequenceHandler_,
          pathname,
          summaryMetaData_,
          summarySampleTypes_
        );
        if (data_was_written) {
          LOGN << "PivotTable.csv file has been stored at: " << pathname;
        } else {
          LOGE << "Error during write. PivotTable.csv content is invalid.";
        }
      }
      else
      {
        LOGE << "Select at least one Sample Type and at least one Metadata";
      }
    }

    ImGui::SameLine();
    if (ImGui::Button("Close"))
    {
      LOGI << "Report window is closed.";
      draw_ = false;
      ImGui::CloseCurrentPopup();
    }

    ImGui::EndPopup();
  }

  void Report::setState(AppState& state)
  {
    LOGD << "Setting state: " << (&state);
    state_ = &state;
  }

  bool Report::initializeMetadataAndSampleTypes()
  {
    summarySampleTypes_.clear();
    summaryMetaData_.clear();

    size_t i = 0;
    for (const std::pair<SampleType, std::string>& p : sampleTypeToString) {
      if (st_checks_.at(i++)) {
        summarySampleTypes_.insert(p.first);
      }
    }

    i = 0;
    for (const std::pair<FeatureMetadata, std::string>& p : metadataToString) {
      if (md_checks_.at(i++)) {
        summaryMetaData_.push_back(p.first);
      }
    }

    return summarySampleTypes_.size() && summaryMetaData_.size();
  }
}
