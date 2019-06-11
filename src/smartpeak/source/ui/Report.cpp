#include <SmartPeak/ui/Report.h>
#include <SmartPeak/core/AppStateProcessor.h>
#include <SmartPeak/core/Metadata.h>
#include <SmartPeak/core/SampleType.h>
#include <SmartPeak/io/SequenceParser.h>
#include <imgui.h>
#include <imgui_internal.h>
#include <plog/Log.h>

namespace SmartPeak
{
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

    {
      ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 3.0f);
      ImGui::BeginChild("SampleTypesChild", ImVec2(ImGui::GetWindowContentRegionWidth() * 0.5f, 400), true);
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
      ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 3.0f);
      ImGui::BeginChild("MetadataChild", ImVec2(0, 400), true);
      size_t i = 0;
      for (const std::pair<Metadata, std::string>& p : metadataToString)
      {
        ImGui::Checkbox(p.second.c_str(), &md_checks_.at(i++));
      }
      ImGui::EndChild();
      ImGui::PopStyleVar();
    }

    ImGui::EndChild();

    ImGui::Separator();

    if (ImGui::Button("Create FeatureSummary.csv"))
    {
      initializeMetadataAndSamleTypes();
      const std::string pathname = state_->main_dir_ + "/FeatureSummary.csv";
      const bool data_was_written = SequenceParser::writeDataTableFromMetaValue(
        state_->sequenceHandler_,
        pathname,
        summaryMetaData_,
        summarySampleTypes_
      );
      if (data_was_written) {
        LOGN << "FeatureSummary.csv file has been stored at: " << pathname;
      } else {
        LOGE << "Error during write. FeatureSummary.csv content is invalid.";
      }
    }

    if (ImGui::Button("Create SequenceSummary.csv"))
    {
      initializeMetadataAndSamleTypes();
      const std::string pathname = state_->main_dir_ + "/SequenceSummary.csv";
      const bool data_was_written = SequenceParser::writeDataMatrixFromMetaValue(
        state_->sequenceHandler_,
        pathname,
        summaryMetaData_,
        summarySampleTypes_
      );
      if (data_was_written) {
        LOGN << "SequenceSummary.csv file has been stored at: " << pathname;
      } else {
        LOGE << "Error during write. SequenceSummary.csv content is invalid.";
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

  void Report::initializeMetadataAndSamleTypes()
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
    for (const std::pair<Metadata, std::string>& p : metadataToString) {
      if (md_checks_.at(i++)) {
        summaryMetaData_.push_back(p.first);
      }
    }
  }
}
