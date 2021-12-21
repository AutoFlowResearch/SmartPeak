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
// $Maintainer: Douglas McCloskey, Ahmed Khalil, Bertrand Boudaud $
// $Authors: Douglas McCloskey $
// --------------------------------------------------------------------------

#include <SmartPeak/ui/SampleTypeEditorWidget.h>
#include <misc/cpp/imgui_stdlib.h>

namespace SmartPeak
{

  void SampleTypeEditorWidget::open(const std::string& current_choice, std::function<void(const std::string&)> ok_callback)
  {
    ok_callback_ = ok_callback;
    current_choice_ = current_choice;
    ImGui::OpenPopup("Edit Sample Type");
  }

  void SampleTypeEditorWidget::draw()
  {
    float popup_width = 400;

    if (!ImGui::BeginPopupModal("Edit Sample Type", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
      return;
    }

    // one documented way to set popup width
    const auto cursor_pos = ImGui::GetCursorPosX();
    ImGui::SetCursorPosX(popup_width);
    ImGui::SetCursorPosX(cursor_pos);

    if (!groups_.empty() && ImGui::BeginCombo("Select type", current_choice_.c_str()))
    {
      for (const auto valid_string : groups_)
      {
        if (ImGui::Selectable(valid_string.c_str()))
        {
          current_choice_ = valid_string;
        }
      }
      ImGui::EndCombo();
    }

    ImGui::Separator();
    if (ImGui::Button("OK"))
    {
      ok_callback_(current_choice_);
      ImGui::CloseCurrentPopup();
    }

    ImGui::SameLine();
    if (ImGui::Button("Cancel"))
    {
      ImGui::CloseCurrentPopup();
    }

    ImGui::EndPopup();
  }
}
