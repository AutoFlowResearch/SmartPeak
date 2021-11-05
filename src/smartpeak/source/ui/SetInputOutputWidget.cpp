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
// $Authors: Douglas McCloskey, Bertrand Boudaud $
// --------------------------------------------------------------------------

#include <SmartPeak/ui/SetInputOutputWidget.h>
#include <SmartPeak/core/ApplicationProcessor.h>
#include <imgui.h>
#include <misc/cpp/imgui_stdlib.h>
#include <plog/Log.h>
#include <SmartPeak/core/SharedProcessors.h>
#include <filesystem>

namespace SmartPeak
{

  namespace fs = std::filesystem;

  SetInputOutputWidget::SetInputOutputWidget(ApplicationHandler& application_handler) :
    Widget("Workflow Inputs/Outputs"),
    application_handler_(application_handler)
  {
  };

  void SetInputOutputWidget::draw()
  {
    if (!filenames_)
    {
      return;
    }
    if (ImGui::BeginPopupModal(title_.c_str(), NULL, ImGuiWindowFlags_None))
    {
      if (   mzML_dir_old_ != filenames_->getTagValue(Filenames::Tag::MZML_INPUT_PATH)
          || features_in_dir_old_ != filenames_->getTagValue(Filenames::Tag::FEATURES_INPUT_PATH)
          || features_out_dir_old_ != filenames_->getTagValue(Filenames::Tag::FEATURES_OUTPUT_PATH))
      {
        // InputTextWithHint does not support std::filesystem::path so we have to create temporary strings for the paths
        mzML_dir_old_ = mzML_dir_edit_ = filenames_->getTagValue(Filenames::Tag::MZML_INPUT_PATH);
        features_in_dir_old_ = features_in_dir_edit_ = filenames_->getTagValue(Filenames::Tag::FEATURES_INPUT_PATH);
        features_out_dir_old_ = features_out_dir_edit_ = filenames_->getTagValue(Filenames::Tag::FEATURES_OUTPUT_PATH);
      }

      if (one_missing_directory_)
      {
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
        ImGui::Text("One or more directory cannot be found. Please set them to continue.");
        ImGui::PopStyleColor();
        one_missing_directory_ = false;
      }
      else
      {
        // empty space to keep same layout when no warning is displayed
        ImGui::Text("");
      }

      setEditField(1, "mzML folder", mzML_dir_edit_, set_raw_data_path_name);
      setEditField(2, "Input features folder", features_in_dir_edit_, set_input_features_path_name);
      setEditField(3, "Output features folder", features_out_dir_edit_, set_output_features_path_name);

      if (popup_file_picker_)
      {
        const auto& [title, file_handler] = (*popup_file_picker_);
        file_picker_.open(title, file_handler, FilePicker::Mode::EDirectory, application_handler_);
        popup_file_picker_ = std::nullopt;
      }

      file_picker_.draw();

      ImGui::Separator();
      if (one_missing_directory_) ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
      if (ImGui::Button("OK"))
      {
        if (!one_missing_directory_)
        {
          filenames_->setTagValue(Filenames::Tag::MZML_INPUT_PATH, mzML_dir_edit_);
          filenames_->setTagValue(Filenames::Tag::FEATURES_INPUT_PATH, features_in_dir_edit_);
          filenames_->setTagValue(Filenames::Tag::FEATURES_OUTPUT_PATH, features_out_dir_edit_);
          visible_ = false;
          ImGui::CloseCurrentPopup();
          if (observer_)
          {
            observer_->onInputOutputSet();
          }
        }
      }
      if (one_missing_directory_) ImGui::PopStyleVar();
      ImGui::SameLine();
      if (ImGui::Button("Cancel"))
      {
        visible_ = false;
        ImGui::CloseCurrentPopup();
        if (observer_)
        {
          observer_->onInputOutputCancel();
        }
      }
      ImGui::EndPopup();
    }
  }

  void SetInputOutputWidget::open(Filenames& filenames, ISetInputOutputWidgetObserver* observer)
  {
    observer_ = observer;
    visible_ = true;
    filenames_ = &filenames;
    set_raw_data_path_name = std::make_shared<SetRawDataPathname>(filenames_);
    set_input_features_path_name = std::make_shared<SetInputFeaturesPathname>(filenames_);
    set_output_features_path_name = std::make_shared<SetOutputFeaturesPathname>(filenames_);
  }

  void SetInputOutputWidget::setEditField(int id, std::string label, std::string& edit_box, std::shared_ptr<IFilePickerHandler>& file_picker_handler)
  {
    bool directory_exists = std::filesystem::exists(std::filesystem::path(edit_box));
    ImGui::Text(label.c_str());
    ImGui::PushID(id);
    if (!directory_exists)
    {
      ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
    }
    ImGui::InputTextWithHint("", edit_box.c_str(), &edit_box);
    if (!directory_exists)
    {
      ImGui::PopStyleColor();
    }
    ImGui::PopID();
    ImGui::SameLine();
    ImGui::PushID(id + 100);
    if (ImGui::Button("Select"))
    {
      popup_file_picker_ = std::make_tuple(label, file_picker_handler);
    }
    ImGui::PopID();
    one_missing_directory_ |= !(directory_exists);
  }

}
