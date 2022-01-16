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

#include <SmartPeak/ui/Report.h>
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

  bool Report::ReportFilePickerHandler::onFilePicked(const std::filesystem::path& filename, ApplicationHandler* application_handler)
  {
    report_.run_and_join(
      writer_method_,
      title_,
      application_handler->sequenceHandler_,
      filename,
      report_.summaryMetaData_,
      report_.summarySampleTypes_,
      report_.result_message_
    );
    return true;
  }

  Report::Report(ApplicationHandler& application_handler) :
    Widget("Create Report"),
    application_handler_(application_handler)
  {
    std::fill(st_checks_.begin(), st_checks_.end(), false);
    std::fill(md_checks_.begin(), md_checks_.end(), false);
    all_st_checks_ = false; all_st_deactivated_ = true;
    all_md_checks_ = false; all_md_deactivated_ = true;
    feature_db_file_picker_handler_ = std::make_shared<ReportFilePickerHandler>(*this, "Feature DB", SequenceParser::writeDataTableFromMetaValue);
    pivot_table_file_picker_handler_ = std::make_shared<ReportFilePickerHandler>(*this, "Pivot Table", SequenceParser::writeDataMatrixFromMetaValue);
    group_feature_db_file_picker_handler_ = std::make_shared<ReportFilePickerHandler>(*this, "Group Feature DB", SequenceParser::writeGroupDataTableFromMetaValue);
    group_pivot_table_file_picker_handler_ = std::make_shared<ReportFilePickerHandler>(*this, "Group Pivot Table", SequenceParser::writeGroupDataMatrixFromMetaValue);
  }

  void Report::draw()
  {
    if (!ImGui::BeginPopupModal(title_.c_str(), NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
      return;
    }

    report_file_picker_.draw();

    ImGui::BeginChild("ReportContent", ImVec2(600, 400));

    ImGui::Text("Select the information to export in the reports.");

    {
      ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 2.0f);
      ImGui::BeginChild("SampleTypesChild", ImVec2(ImGui::GetWindowContentRegionWidth() * 0.5f, 380), true);
      ImGui::Text("Sample types");
      ImGui::Separator();
      size_t i = 0;

      if (ImGui::Checkbox("All", &all_st_checks_))
      {
        std::fill(st_checks_.begin(), st_checks_.end(), true);
        all_st_deactivated_ = false;
      }
      if (!all_st_checks_ && !all_st_deactivated_)
      {
        std::fill(st_checks_.begin(), st_checks_.end(), false);
      }
      
      for (const std::pair<SampleType, std::string>& p : sampleTypeToString)
      {
        ImGui::Checkbox(p.second.c_str(), &st_checks_.at(i++));
        all_st_deactivated_ = true;
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
      
      if (ImGui::Checkbox("All", &all_md_checks_))
      {
        std::fill(md_checks_.begin(), md_checks_.end(), true);
        all_md_deactivated_ = false;
      }
      if (!all_md_checks_ && !all_md_deactivated_)
      {
        std::fill(md_checks_.begin(), md_checks_.end(), false);
      }
      
      for (const std::pair<FeatureMetadata, std::string>& p : metadataToString)
      {
        ImGui::Checkbox(p.second.c_str(), &md_checks_.at(i++));
        all_md_deactivated_ = true;
      }
      ImGui::EndChild();
      ImGui::PopStyleVar();
    }

    ImGui::EndChild();

    ImGui::Separator();

    ImGui::Text("Create Report:");
    reportButton("Feature DB", feature_db_file_picker_handler_, "FeatureDB.csv");
    ImGui::SameLine();
    reportButton("Pivot Table", pivot_table_file_picker_handler_, "PivotTable.csv");
    ImGui::SameLine();
    reportButton("Group Feature DB", group_feature_db_file_picker_handler_, "GroupFeatureDB.csv");
    ImGui::SameLine();
    reportButton("Group Pivot Table", group_pivot_table_file_picker_handler_, "GroupPivotTable.csv");

    ImGui::Separator();

    if (ImGui::Button("Close"))
    {
      LOGI << "Report window is closed.";
      visible_ = false;
      ImGui::CloseCurrentPopup();
    }

    drawResultMessage();
    ImGui::EndPopup();
  }

  void Report::drawResultMessage()
  {
    static const char* report_creation_result_title = "Report creation result";
    if (result_message_)
    {
      if (!ImGui::IsPopupOpen(report_creation_result_title))
      {
        ImGui::OpenPopup(report_creation_result_title);
      }

      if (!ImGui::BeginPopupModal(report_creation_result_title, NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
        return;
      }

      ImGui::Text(result_message_->c_str());

      ImGui::Separator();

      if (ImGui::Button("OK"))
      {
        result_message_ = std::nullopt;
        ImGui::CloseCurrentPopup();
      }

      ImGui::EndPopup();
    }
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
    bool (*data_writer)(const SequenceHandler&, const std::filesystem::path&, const std::vector<FeatureMetadata>&, const std::set<SampleType>&),
    const std::string& data_writer_label,
    const SequenceHandler sequence,
    const std::filesystem::path& pathname,
    const std::vector<FeatureMetadata>& meta_data,
    const std::set<SampleType>& sample_types,
    std::optional<std::string>& result_message
  )
  {
    std::ostringstream result;

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
      result << e.what() << std::endl;
    }

    if (data_was_written) {
      result << data_writer_label << " file has been stored at: " << pathname.generic_string();
      LOGN << result.str();
    } else {
      result << "Error while writing " << data_writer_label;
      LOGE << result.str();
    }
    result_message = result.str();
  }

  void Report::reportButton(const std::string& title,
    std::shared_ptr<ReportFilePickerHandler> report_file_picker_handler,
    const std::string& default_file_name)
  {
    if (ImGui::Button(title.c_str()))
    {
      const bool checkboxes_check = initializeMetadataAndSampleTypes();
      if (checkboxes_check)
      {
        report_file_picker_.open(
          title.c_str(),
          report_file_picker_handler,
          FilePicker::Mode::EFileCreate,
          application_handler_,
          default_file_name);
      }
      else
      {
         result_message_ = "Select at least one Sample Type and at least one Metadata";
         LOGE << *result_message_;
      }
    }
  }

}
