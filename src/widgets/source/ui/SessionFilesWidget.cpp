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
// $Maintainer: Douglas McCloskey, Bertrand Boudaud $
// $Authors: Douglas McCloskey, Bertrand Boudaud $
// --------------------------------------------------------------------------

#include <SmartPeak/ui/SessionFilesWidget.h>
#include <SmartPeak/core/SharedProcessors.h>
#include <SmartPeak/core/SequenceProcessor.h>
#include <SmartPeak/core/ApplicationProcessors/LoadSession.h>

#include <imgui.h>
#include <plog/Log.h>

#include <memory>

namespace SmartPeak
{
  std::vector<std::string> SessionFilesWidget::file_modes_strings_ =
  {
    "Store in Session file",
    "Use external file",
    "Not used"
  };

  enum FileModeEnum
  {
    EEmbedded,
    EExternal,
    ENotUsed
  };

  SessionFilesWidget::SessionFilesWidget(
    ApplicationHandler& application_handler,
    SessionFilesWidget::Mode mode,
    WorkflowManager& workflow_manager,
    IApplicationProcessorObserver* application_processor_observer,
    ISequenceProcessorObserver* sequence_processor_observer,
    ISequenceSegmentProcessorObserver* sequence_segment_processor_observer,
    ISampleGroupProcessorObserver* sample_group_processor_observer) :
    Widget("Session Files"),
    application_handler_(application_handler),
    mode_(mode),
    workflow_manager_(workflow_manager),
    application_processor_observer_(application_processor_observer),
    sequence_processor_observer_(sequence_processor_observer),
    sequence_segment_processor_observer_(sequence_segment_processor_observer),
    sample_group_processor_observer_(sample_group_processor_observer)
  {
    setHints(application_handler_.loading_processors_);
  };

  struct SetInputFile : IFilePickerHandler
  {
    explicit SetInputFile(std::string& input_file_editor) : input_file_editor_(input_file_editor) { }

    /**
    IFilePickerHandler
    */
    bool onFilePicked(const std::filesystem::path& filename, ApplicationHandler* application_handler) override
    {
      input_file_editor_ = filename.generic_string();
      return true;
    };

    std::string& input_file_editor_;
  };

  void SessionFilesWidget::draw()
  {
    showQuickHelpToolTip("WorkflowStepWindow");

    float popup_width = 600;
    float description_box_height = 200;

    if (!ImGui::BeginPopupModal(title_.c_str(), NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
      return;
    }

    ImGui::PushItemWidth(popup_width);

    id_ = 1;

    // headers
    const ImGuiTableFlags table_flags = ImGuiTableFlags_Reorderable | ImGuiTableFlags_NoSavedSettings;
    static std::vector<std::string> header_names
    {
      " ",
      "Description",
      "Mode",
      " ",
      "Path"
    };

    if (one_missing_requirement_)
    {
      ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
      ImGui::Text("One or more files is missing to run the workflow. Please correct the files setting of the Session.");
      ImGui::PopStyleColor();
    }
    else if (one_invalid_)
    {
      ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
      ImGui::Text("One or more files is missing. Please correct the files setting of the Session.");
      ImGui::PopStyleColor();
    }
    else
    {
      // Empty text to keep dialog layout
      ImGui::Text("");
    }
    ImGui::Separator();

    updateFieldEditorsStatus();

    if (ImGui::BeginTable("file settings", header_names.size(), table_flags))
    {
      // First row header_names_
      for (auto header : header_names)
      {
        ImGui::TableSetupColumn(header.c_str());
      }
      ImGui::TableSetupScrollFreeze(header_names.size(), 1);
      ImGui::TableHeadersRow();

      one_invalid_ = false;
      one_missing_requirement_ = false;

      for (const auto& file_id : filenames_.getFileIds())
      {
        FileEditorFields& fef = file_editor_fields_.at(file_id);
        
        ImGui::TableNextRow();
        drawSavedStatusColumn(file_id);
        drawDescriptionColumn(file_id, fef);
        drawFileModeColumn(file_id, fef);
        drawModifiedFlagColumn(file_id);
        drawPathColumn(file_id, fef);
      }

      ImGui::EndTable();

      ImGui::Separator();

      if (one_invalid_) ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
      if (ImGui::Button("Ok"))
      {
        if (!one_invalid_)
        {
          doUpdateSession();
          visible_ = false;
          ImGui::CloseCurrentPopup();
        }
      }
      if (one_invalid_) ImGui::PopStyleVar();

      ImGui::SameLine();
      if (ImGui::Button("Cancel"))
      {
        visible_ = false;
        ImGui::CloseCurrentPopup();
      }
    }

    if (popup_file_picker_)
    {
      file_picker_.open(filenames_.getDescription(*popup_file_picker_),
        std::make_shared<SetInputFile>(file_editor_fields_.at(*popup_file_picker_).text_editor_),
        FilePicker::Mode::EFileRead,
        application_handler_);
      popup_file_picker_ = std::nullopt;
    }

    file_picker_.draw();
    ImGui::EndPopup();
  }

  SessionFilesWidget::FileEditorFields SessionFilesWidget::createEditorFields(std::string file_id)
  {
    const auto full_path_name = filenames_.getFullPath(file_id);
    std::string displayed_path;
    std::filesystem::path main_dir_path(filenames_.getTagValue(Filenames::Tag::MAIN_DIR));
    std::string relative_path;
    if (!full_path_name.empty())
    {
      relative_path = std::filesystem::relative(full_path_name, main_dir_path).generic_string();
    }
    bool is_embedded = filenames_.isEmbedded(file_id);
    bool file_exists = std::filesystem::exists(full_path_name);
    if (mode_ == Mode::EModification)
    {
      // we are reviewing an existing session
      if (!filenames_.isEmbedded(file_id))
      {
        displayed_path = relative_path;
      }
    }
    else  // == we are creating a new session
    {
      if (file_exists)
      {
        // prefill with existing file
        displayed_path = relative_path;
      }
      else if (is_embedded)
      {
        is_embedded = false;
      }
    }
    FileEditorFields fef = {
      hints_.at(file_id).generic_string(),
      displayed_path,
      displayed_path,
      displayed_path,
      is_embedded,
      is_embedded,
      file_exists
    };
    fef.combo_file_mode_ = fileModeAsString(fef);
    return fef;
  }

  void SessionFilesWidget::setHints(const std::vector<std::shared_ptr<IFilenamesHandler>>& processor_input_files)
  {
    if (hints_.empty())
    {
      Filenames filenames;
      for (const auto& filename_handler : processor_input_files)
      {
        filename_handler->getFilenames(filenames);
      }
      for (const auto& file_id : filenames.getFileIds())
      {
        const auto relative_path = filenames.getFullPath(file_id).filename();
        hints_.emplace(file_id, relative_path);
      }
    }
  }

  void SessionFilesWidget::open(const Filenames& filenames, const std::set<std::string>& requirements)
  {
    requirements_ = requirements;
    filenames_ = filenames;
    file_editor_fields_.clear();
    updateFieldEditorsStatus();
    visible_ = true;
  }

  bool SessionFilesWidget::isModified(const std::string& file_id) const
  {
    return ((file_editor_fields_.at(file_id).original_filepath_ != (file_editor_fields_.at(file_id).text_editor_))
      || (file_editor_fields_.at(file_id).original_embedded_ != (file_editor_fields_.at(file_id).embedded_)));
  }

  bool SessionFilesWidget::isToBeSaved(const std::string& file_id) const
  {
    return (mode_ == Mode::EModification) && (!application_handler_.isFileSaved(file_id));
  }

  bool SessionFilesWidget::isMissingRequirement(const std::string& file_id) const
  {
    if (std::find(std::begin(requirements_), std::end(requirements_), file_id) != std::end(requirements_))
    {
      const auto& fef = file_editor_fields_.at(file_id);
      return (!fef.embedded_) && (!fef.file_exists_);
    }
    return false;
  }

  bool SessionFilesWidget::isInvalid(const std::string& file_id) const
  {
    const auto& fef = file_editor_fields_.at(file_id);
    const auto& mode = fef.combo_file_mode_;
    bool file_must_exist = false;
    bool file_can_be_empty = false;
    if (mode == file_modes_strings_[EEmbedded])
    {
      if (!fef.original_embedded_)
      {
        // we changed to embedded
        file_must_exist = true;
        file_can_be_empty = false;
      }
      else
      {
        // we don't changed embedded status but we may have changed url
        if (fef.original_filepath_ != fef.text_editor_)
        {
          file_must_exist = true;
          file_can_be_empty = true;
        }
        else
        {
          file_must_exist = false;
          file_can_be_empty = true;
        }
      }
    }
    else if (mode == file_modes_strings_[ENotUsed])
    {
      file_must_exist = false;
      file_can_be_empty = true;
    }
    else if (mode == file_modes_strings_[EExternal])
    {
      file_must_exist = true;
      file_can_be_empty = false;
    }
    return (  (!file_can_be_empty && fef.text_editor_.empty())
           || (file_must_exist && !fef.file_exists_));
  }

  void SessionFilesWidget::doUpdateSession()
  {
    // reconstruct filenames to be used to construct the session
    Filenames filenames;
    for (const auto& [tag_name, tag] : filenames_.getTagNames())
    {
      filenames.setTagValue(tag, filenames_.getTagValue(tag));
    }
    filenames.getSessionDB() = filenames_.getSessionDB();
    for (const auto& fef : file_editor_fields_)
    {
      std::string path = fef.second.text_editor_;
      if ((!path.empty()) && (std::filesystem::path(path).is_relative()))
      {
        path = "${MAIN_DIR}/" + path;
      }
      filenames.addFileName(fef.first, path);
      filenames.setEmbedded(fef.first, fef.second.embedded_);
    }
    application_handler_.closeSession();
    application_handler_.filenames_ = filenames;
    application_handler_.main_dir_ = filenames_.getTagValue(Filenames::Tag::MAIN_DIR);
    LoadSession load_session(application_handler_, workflow_manager_);
    load_session.addApplicationProcessorObserver(application_processor_observer_);
    load_session.filenames_ = filenames;
    // When modifiying one file, we load it using all files are marked as external, to eventually import them.
    for (const auto& fef : file_editor_fields_)
    {
      if (isModified(fef.first))
      {
        load_session.filenames_->setEmbedded(fef.first, false);
      }
    }
    load_session.notifyApplicationProcessorStart({ load_session.getName() }); // we need to use the workflow manager to profit from the progressbar
    if (load_session.process())
    {
      // Update saved state
      for (const auto& fef : file_editor_fields_)
      {
        if (fef.second.embedded_)
        {
          if (mode_ == Mode::ECreation)
          {
            // if the file is embedded and we create a new session, 
            // we need to set it as not saved, 
            // so that the file will be embedded in the session db
            application_handler_.setFileSavedState(fef.first, false);
          }
          else
          {
            application_handler_.setFileSavedState(fef.first, !(isModified(fef.first)));
          }
        }
        else
        {
          // The file is not embedded, we don't want to export it - set it to saved.
          application_handler_.setFileSavedState(fef.first, true);
        }
      }
    }
    else
    {
      // load failed
      application_handler_.closeSession();
      load_session.notifyApplicationProcessorError("Failed to load session.");
    }
    load_session.notifyApplicationProcessorEnd();
  }

  void SessionFilesWidget::updateFieldEditorsStatus()
  {
    for (const auto& filename_handler : application_handler_.loading_processors_)
    {
      filename_handler->getFilenames(filenames_);
    }

    for (const auto& file_id : filenames_.getFileIds())
    {
      if (file_editor_fields_.count(file_id) == 0)
      {
        file_editor_fields_[file_id] = createEditorFields(file_id);
      }
    }

    // detect file change and update status
    for (auto& fef_pair : file_editor_fields_)
    {
      auto& fef = fef_pair.second;
      if (fef.text_editor_last_update_ != fef.text_editor_)
      {
        // Check if it exists
        std::filesystem::path main_dir_path(filenames_.getTagValue(Filenames::Tag::MAIN_DIR));
        std::filesystem::path full_path_name;
        auto entered_path = std::filesystem::path(fef.text_editor_);
        if (!entered_path.empty())
        {
          if (entered_path.is_absolute())
          {
            full_path_name = entered_path;
          }
          else
          {
            full_path_name = main_dir_path / entered_path;
          }
          fef.file_exists_ = std::filesystem::exists(full_path_name);
          // Compute relative/absolute name
          std::filesystem::path relative_path = std::filesystem::relative(full_path_name, main_dir_path).generic_string();
          bool is_under_main = (relative_path.begin() != relative_path.end()) && (*relative_path.begin() != "..");
          if (is_under_main)
          {
            fef.text_editor_ = relative_path.generic_string();
          }
          else
          {
            fef.text_editor_ = full_path_name.generic_string();
          }
        }
        else // file path is empty
        {
          clearEntry(fef_pair.first);
        }
        fef.text_editor_last_update_ = fef.text_editor_;
      }
    }
  }

  void SessionFilesWidget::clearEntry(const std::string& file_id)
  {
    auto& fef = file_editor_fields_.at(file_id);
    fef.text_editor_ = "";
    fef.embedded_ = false;
  }

  void SessionFilesWidget::drawSavedStatusColumn(const std::string& file_id)
  {
    ImGui::TableSetColumnIndex(ESavedStatusColumn);
    if (isToBeSaved(file_id))
    {
      ImGui::Text("U");
      if (ImGui::IsItemHovered())
      {
        ImGui::BeginTooltip();
        ImGui::Text("The data for this file has been modified.");
        ImGui::Text("Save session to keep modifications.");
        ImGui::EndTooltip();
      }
    }
    else
    {
      ImGui::Text(" ");
    }
  }

  void SessionFilesWidget::drawDescriptionColumn(const std::string& file_id, FileEditorFields& fef)
  {
    ImGui::TableSetColumnIndex(EDescriptionColumn);
    bool is_set = (fef.embedded_) || (!fef.text_editor_.empty());
    bool is_missing_requirement = isMissingRequirement(file_id);
    one_missing_requirement_ |= is_missing_requirement;
    bool is_invalid = isInvalid(file_id);
    if (is_missing_requirement || is_invalid)
    {
      ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
    }
    else if (!is_set)
    {
      ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetStyle().Colors[ImGuiCol_TextDisabled]);
    }
    ImGui::Text(filenames_.getDescription(file_id).c_str());
    if (!is_set || is_missing_requirement || is_invalid)
    {
      ImGui::PopStyleColor();
    }
  }

  void SessionFilesWidget::drawFileModeColumn(const std::string& file_id, FileEditorFields& fef)
  {
    ImGui::TableSetColumnIndex(EFileModeColumns);
    ImGui::PushID(id_++);
    ImGui::SetNextItemWidth(180);
    if (ImGui::BeginCombo("##file mode", fef.combo_file_mode_.c_str()))
    {
      for (const auto& file_mode : file_modes_strings_)
      {
        if (file_mode != file_modes_strings_[EEmbedded] || (filenames_.isEmbeddable(file_id)))
        {
          if (ImGui::Selectable(file_mode.c_str()))
          {
            if (file_mode == file_modes_strings_[EEmbedded])
            {
              file_editor_fields_.at(file_id).embedded_ = true;
            }
            else if (file_mode == file_modes_strings_[ENotUsed])
            {
              clearEntry(file_id);
            }
            else if (file_mode == file_modes_strings_[EExternal])
            {
              file_editor_fields_.at(file_id).embedded_ = false;
            }
            fef.combo_file_mode_ = file_mode;
          }
        }
      }
      ImGui::EndCombo();
    }
    ImGui::PopID();
  }

  void SessionFilesWidget::drawModifiedFlagColumn(const std::string& file_id)
  {
    ImGui::TableSetColumnIndex(EModifiedFlagColumn);
    if ((mode_ == Mode::EModification) && isModified(file_id))
    {
      ImGui::Text("M");
    }
    else
    {
      ImGui::Text(" ");
    }
  }

  void SessionFilesWidget::drawPathColumn(const std::string& file_id, FileEditorFields& fef)
  {
    ImGui::TableSetColumnIndex(EPathColumn);
    ImGui::PushID(id_++);
    ImGui::PushItemWidth(500);
    if (fef.combo_file_mode_ != file_modes_strings_[ENotUsed])
    {
      bool invalid = isInvalid(file_id);
      one_invalid_ |= invalid;
      if (invalid)
      {
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
      }
      ImGui::InputTextWithHint("",
        fef.text_editor_hint_.c_str(),
        &fef.text_editor_
      );
      if (invalid)
      {
        ImGui::PopStyleColor();
        if (ImGui::IsItemHovered())
        {
          ImGui::SetTooltip("File not found");
        }
      }
      ImGui::PopID();
      ImGui::SameLine();
      ImGui::PushID(id_++);
      std::string select_button_label = "Select";
      if ((filenames_.isEmbeddable(file_id)) && file_editor_fields_.at(file_id).embedded_)
      {
        select_button_label = "Import";
      }
      if (ImGui::Button(select_button_label.c_str()))
      {
        popup_file_picker_ = file_id;
      }
    }
    ImGui::PopID();
  }

  std::string SessionFilesWidget::fileModeAsString(const FileEditorFields& fef) const
  {
    if (fef.embedded_)
    {
      return file_modes_strings_[EEmbedded];
    }
    else
    {
      if (fef.text_editor_.empty())
      {
        return file_modes_strings_[ENotUsed];
      }
      else
      {
        return file_modes_strings_[EExternal];
      }
    }
  }
}
