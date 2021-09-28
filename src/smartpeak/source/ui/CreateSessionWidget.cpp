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

#include <SmartPeak/ui/CreateSessionWidget.h>

#include <misc/cpp/imgui_stdlib.h>
#include <imgui.h>
#include <plog/Log.h>

namespace SmartPeak
{
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

  void CreateSessionWidget::draw()
  {
    float popup_width = 600;
    float description_box_height = 200;

    if (!ImGui::BeginPopupModal(title_.c_str(), NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
      return;
    }

    ImGui::PushItemWidth(popup_width);

    int id = 1;
  
    ImGui::Text("Choose the Session directory");
    ImGui::Text("The directory can be empty or already filled with some files.");

    ImGui::Separator();

    ImGui::PushID(id++);
    ImGui::PushItemWidth(500);
    ImGui::InputTextWithHint("", "Session directory", &main_dir_);
    ImGui::PopID();
    ImGui::SameLine();
    if (ImGui::Button("Select"))
    {
      file_picker_.open("Session directory", std::make_shared<SetInputFile>(main_dir_), FilePicker::Mode::EDirectory, application_handler_);
    }

    ImGui::Separator();

    bool valid = (!main_dir_.empty() && std::filesystem::exists(main_dir_) && std::filesystem::is_directory(main_dir_));
    if (!valid) ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
    if (ImGui::Button("Ok"))
    {
      if (valid)
      {
        // reconstruct filenames to be used to construct the session
        visible_ = false;
        ImGui::CloseCurrentPopup();
        Filenames filenames = Utilities::buildFilenamesFromDirectory(application_handler_, main_dir_);
        session_file_widget_->open(filenames);
      }
    }
    if (!valid) ImGui::PopStyleVar();

    ImGui::SameLine();
    if (ImGui::Button("Cancel"))
    {
      visible_ = false;
      ImGui::CloseCurrentPopup();
    }

    file_picker_.draw();
    ImGui::EndPopup();
  }

}
