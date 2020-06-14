#include <SmartPeak/ui/Report.h>
#include <SmartPeak/core/ApplicationProcessor.h>
#include <SmartPeak/core/FeatureMetadata.h>
#include <SmartPeak/core/SampleType.h>
#include <SmartPeak/io/SequenceParser.h>
#include <algorithm>
#include <future>
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

    if (!application_handler_)
    {
      LOGE << "Report widget has no ApplicationHandler object associated with it";
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
        const std::string pathname = application_handler_->main_dir_ + "/FeatureDB.csv";
        run_and_join(
          SequenceParser::writeDataTableFromMetaValue,
          "FeatureDB.csv",
          application_handler_->sequenceHandler_,
          pathname,
          summaryMetaData_,
          summarySampleTypes_
        );
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
        const std::string pathname = application_handler_->main_dir_ + "/PivotTable.csv";
        run_and_join(
          SequenceParser::writeDataMatrixFromMetaValue,
          "PivotTable.csv",
          application_handler_->sequenceHandler_,
          pathname,
          summaryMetaData_,
          summarySampleTypes_
        );
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

  void Report::setState(ApplicationHandler& state)
  {
    LOGD << "Setting state: " << (&state);
    application_handler_ = &state;
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

  void Report::run_and_join(
    bool (*data_writer)(const SequenceHandler&, const std::string&, const std::vector<FeatureMetadata>&, const std::set<SampleType>&),
    const std::string& data_writer_label,
    const SequenceHandler sequence,
    const std::string& pathname,
    const std::vector<FeatureMetadata>& meta_data,
    const std::set<SampleType>& sample_types
  )
  {
    std::future<bool> future = std::async(
      std::launch::async,
      data_writer,
      std::ref(sequence),
      std::cref(pathname),
      std::cref(meta_data),
      std::cref(sample_types)
    );
    LOGN << data_writer_label << " file is being stored...";

    bool data_was_written = false;

    try {
      data_was_written = future.get();
    } catch (const std::exception& e) {
      LOGE << e.what();
    }

    if (data_was_written) {
      LOGN << data_writer_label << " file has been stored at: " << pathname;
    } else {
      LOGE << "Error during write. " << data_writer_label << " content is invalid.";
    }
  }
}
