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

#include <SmartPeak/ui/RunWorkflowWidget.h>
#include <SmartPeak/core/ApplicationProcessor.h>
#include <imgui.h>
#include <misc/cpp/imgui_stdlib.h>
#include <plog/Log.h>
#include <SmartPeak/core/SharedProcessors.h>
#include <filesystem>

namespace SmartPeak
{
  namespace fs = std::filesystem;

  void RunWorkflowWidget::draw()
  {
		ImGui::SetNextWindowSizeConstraints(ImVec2(320, 100), ImVec2(FLT_MAX, FLT_MAX));
    if (ImGui::BeginPopupModal(title_.c_str(), NULL, ImGuiWindowFlags_None))
    {
      if (   mzML_dir_old_ != application_handler_.mzML_dir_.generic_string()
          || features_in_dir_old_ != application_handler_.features_in_dir_.generic_string()
          || features_out_dir_old_ != application_handler_.features_out_dir_.generic_string())
      {
        // InputTextWithHint does not support std::filesystem::path so we have to create temporary strings for the paths
        mzML_dir_old_ = mzML_dir_edit_ = application_handler_.mzML_dir_.generic_string();
        features_in_dir_old_ = features_in_dir_edit_ = application_handler_.features_in_dir_.generic_string();
        features_out_dir_old_ = features_out_dir_edit_ = application_handler_.features_out_dir_.generic_string();
      }

      ImGui::Text("mzML folder");
      ImGui::PushID(1);
      ImGui::InputTextWithHint("", mzML_dir_edit_.c_str(), &mzML_dir_edit_);
      ImGui::PopID();
      ImGui::SameLine();
      ImGui::PushID(11);
      if (ImGui::Button("Select"))
      {
        popup_file_picker_ = std::make_tuple("mzML folder", std::make_shared<SetRawDataPathname>());
      }
      ImGui::PopID();

      ImGui::Text("Input features folder");
      ImGui::PushID(2);
      ImGui::InputTextWithHint("", features_in_dir_edit_.c_str(), &features_in_dir_edit_);
      ImGui::PopID();
      ImGui::SameLine();
      ImGui::PushID(22);
      if (ImGui::Button("Select"))
      {
        popup_file_picker_ = std::make_tuple("Input features folder", std::make_shared<SetInputFeaturesPathname>());
      }
      ImGui::PopID();

      ImGui::Text("Output features folder");
      ImGui::PushID(3);
      ImGui::InputTextWithHint("", features_out_dir_edit_.c_str(), &features_out_dir_edit_);
      ImGui::PopID();

      ImGui::SameLine();
      ImGui::PushID(33);
      if (ImGui::Button("Select"))
      {
        popup_file_picker_ = std::make_tuple("Output features folder", std::make_shared<SetOutputFeaturesPathname>());
      }
      ImGui::PopID();

      if (popup_file_picker_)
      {
        const auto& [title, file_handler] = (*popup_file_picker_);
        file_picker_.open(title, file_handler, FilePicker::Mode::EDirectory, application_handler_);
        popup_file_picker_ = std::nullopt;
      }

      file_picker_.draw();
      
      ImGui::Separator();
      ImGui::Checkbox("Run on Server", &run_on_server);
      ImGui::Text("Server URL");
      ImGui::SameLine();
      ImGui::PushID(4);
      ImGui::InputTextWithHint("", "localhost:50051", &server_url);
      ImGui::PopID();
      
      ImGui::Separator();
      bool run_workflow_clicked = false;
      if ((run_workflow_clicked = ImGui::Button("Run workflow")) && !run_on_server)
      {
        application_handler_.mzML_dir_ = mzML_dir_edit_;
        application_handler_.features_in_dir_ = features_in_dir_edit_;
        application_handler_.features_out_dir_ = features_out_dir_edit_;
        for (const auto& pathname : { application_handler_.mzML_dir_, application_handler_.features_in_dir_, application_handler_.features_out_dir_ }) {
          fs::create_directories(fs::path(pathname));
        }
        BuildCommandsFromNames buildCommandsFromNames(application_handler_);
        buildCommandsFromNames.names_ = application_handler_.sequenceHandler_.getWorkflow();
        if (!buildCommandsFromNames.process()) {
          LOGE << "Failed to create Commands, aborting.";
        }
        else 
        {
          for (auto& cmd : buildCommandsFromNames.commands_)
          {
            for (auto& p : cmd.dynamic_filenames)
            {
              p.second.setTag(Filenames::Tag::MZML_INPUT_PATH, application_handler_.mzML_dir_.generic_string());
              p.second.setTag(Filenames::Tag::FEATURES_INPUT_PATH, application_handler_.features_in_dir_.generic_string());
              p.second.setTag(Filenames::Tag::FEATURES_OUTPUT_PATH, application_handler_.features_out_dir_.generic_string());
            }
          }
          const std::set<std::string> injection_names = session_handler_.getSelectInjectionNamesWorkflow(application_handler_.sequenceHandler_);
          const std::set<std::string> sequence_segment_names = session_handler_.getSelectSequenceSegmentNamesWorkflow(application_handler_.sequenceHandler_);
          const std::set<std::string> sample_group_names = session_handler_.getSelectSampleGroupNamesWorkflow(application_handler_.sequenceHandler_);
            workflow_manager_.addWorkflow(application_handler_,
            injection_names, 
            sequence_segment_names,
            sample_group_names,
            buildCommandsFromNames.commands_,
            &application_processor_observer_,
            &sequence_processor_observer_,
            &sequence_segment_processor_observer_,
            &sample_group_processor_observer_
          );
        }
        visible_ = false;
        ImGui::CloseCurrentPopup();
      }
      if (run_workflow_clicked && !server_url.empty() && run_on_server)
      {
        application_handler_.mzML_dir_ = mzML_dir_edit_;
        application_handler_.features_in_dir_ = features_in_dir_edit_;
        application_handler_.features_out_dir_ = features_out_dir_edit_;

        
        server_fields_set = true;
        visible_ = false;
        ImGui::CloseCurrentPopup();
      }
      ImGui::SameLine();
      if (ImGui::Button("Close"))
      {
        visible_ = false;
        ImGui::CloseCurrentPopup();
      }
      ImGui::EndPopup();
    }
  }

}
