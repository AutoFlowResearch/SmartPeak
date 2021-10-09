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
// $Maintainer: Ahmed Khalil $
// $Authors: Ahmed Khalil $
// --------------------------------------------------------------------------

#include <SmartPeak/ui/ServerDialogueWidget.h>
#include <SmartPeak/core/ApplicationProcessor.h>
#include <SmartPeak/core/FeatureMetadata.h>
#include <SmartPeak/core/SampleType.h>
#include <SmartPeak/io/SequenceParser.h>
#include <algorithm>
#include <future>
#include <imgui.h>
#include <imgui_internal.h>
#include <misc/cpp/imgui_stdlib.h>
#include <plog/Log.h>

namespace SmartPeak
{
  ServerDialogueWidget::ServerDialogueWidget(ApplicationHandler& application_handler) :
    application_handler_(application_handler)
  {
    
  }

  void ServerDialogueWidget::draw()
  {
    if (!ImGui::BeginPopupModal("Connect to server", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
      return;
    }
    
    static std::string server_ip_address = "localhost:50051";
    static std::string sequence_file_path = "";

    ImGui::Text("Setup a connection to a SmartPeak Server Instance");
    ImGui::Separator();
    
    ImGui::Text("Server IP:PORT");
    ImGui::SameLine();
    ImGui::PushID(1);
    ImGui::InputTextWithHint("", "localhost:50051", &server_ip_address);
    ImGui::PopID();
    
    ImGui::Text("Sequence File ");
    ImGui::SameLine();
    ImGui::PushID(2);
    ImGui::InputTextWithHint("", sequence_file_path.c_str(), &sequence_file_path);
    ImGui::PopID();
    
    ImGui::PushID(3);
    if (ImGui::Button("Done"))
    {
      LOGI << "ServerDialogueWidget window is closed.";
      server_ip_address_ = server_ip_address;
      sequence_file_path_ = sequence_file_path;
      if (!server_ip_address_.empty() && !sequence_file_path_.empty()) fields_set_ = true;
      visible_ = false;
      SmartPeak::run_on_server = true;
      ImGui::CloseCurrentPopup();
    }
    ImGui::PopID();
    
    ImGui::PushID(33);
    ImGui::SameLine();
    if (ImGui::Button("Reset"))
    {
      LOGI << "Server connection parameters reset.";
      server_ip_address.clear();
      sequence_file_path.clear();
      ImGui::CloseCurrentPopup();
    }
    ImGui::PopID();

    ImGui::EndPopup();
  }
}
