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
#pragma once

#include <SmartPeak/ui/Widget.h>
#include <SmartPeak/core/ApplicationHandler.h>
#include <SmartPeak/ui/FilePicker.h>
#include <misc/cpp/imgui_stdlib.h>
#include <string>
#include <vector>

namespace SmartPeak
{
  class SessionFilesWidget : public Widget
  {
  public:

    enum class Mode
    {
      ECreation,
      EModification
    };

    SessionFilesWidget(ApplicationHandler& application_handler, SessionFilesWidget::Mode mode);

    void draw() override;
    void open(const Filenames& filenames, const std::set<std::string>& requirements = {});

  protected:

    struct FileEditorFields
    {
      std::string text_editor_hint_;
      std::string text_editor_;
      std::string text_editor_last_update_; // used to detect a change in the textfield (cb does not work properly)
      std::string original_filepath_;
      bool embedded_ = false;
      bool original_embedded_ = false;
      bool file_exists_ = false;
      std::string combo_file_mode_ = "";
    };

    Filenames filenames_;
    std::optional<std::string> popup_file_picker_;
    FilePicker file_picker_;
    ApplicationHandler& application_handler_;
    std::map<std::string, FileEditorFields> file_editor_fields_;
    std::map<std::string, std::filesystem::path> hints_;
    Mode mode_;
    std::set<std::string> requirements_;

  protected:
    virtual void doUpdateSession();
    virtual bool isModified(const std::string& file_id) const;
    virtual bool isToBeSaved(const std::string& file_id) const;
    virtual void clearEntry(const std::string& file_id);
    virtual bool isMissingRequirement(const std::string& file_id) const;
    virtual bool isInvalid(const std::string& file_id) const;

  private:
    virtual FileEditorFields createEditorFields(std::string file_id);
    virtual void setHints(const std::vector<std::shared_ptr<IFilenamesHandler>>& processor_input_files);
    virtual void updateFieldEditorsStatus();

    enum
    {
      ESavedStatusColumn,
      EDescriptionColumn,
      EFileModeColumns,
      EModifiedFlagColumn,
      EPathColumn,
    } EFileSettingsColumns;

    void drawSavedStatusColumn(const std::string& file_id);
    void drawDescriptionColumn(const std::string& file_id, FileEditorFields& fef);
    void drawFileModeColumn(const std::string& file_id, FileEditorFields& fef);
    void drawModifiedFlagColumn(const std::string& file_id);
    void drawPathColumn(const std::string& file_id, FileEditorFields& fef);
    std::string fileModeAsString(const FileEditorFields& fef) const;

    bool one_missing_requirement_ = false;
    bool one_invalid_ = false;
    int id_ = 1;
    static std::vector<std::string> file_modes_strings_;
  };
}
