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

#include <SmartPeak/ui/GenericFloatEditorWidget.h>
#include <misc/cpp/imgui_stdlib.h>

namespace SmartPeak
{

  void GenericFloatEditorWidget::open(const float& value,
    std::function<void(const float&)> ok_callback)
  {
    ok_callback_ = ok_callback;
    std::ostringstream ss;
    ss << value;
    value_ = ss.str();
    ImGui::OpenPopup(title_.c_str());
  }

  void GenericFloatEditorWidget::draw()
  {
    float popup_width = 400;

    if (!ImGui::BeginPopupModal(title_.c_str(), NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
      return;
    }

    // one documented way to set popup width
    const auto cursor_pos = ImGui::GetCursorPosX();
    ImGui::SetCursorPosX(popup_width);
    ImGui::SetCursorPosX(cursor_pos);

    ImGui::InputText("value", &value_);

    ImGui::Separator();
    float value_as_float = 0.0f;
    bool valid = false;
    try
    {
      value_as_float = std::stof(value_);
      valid = true;
    }
    catch (const std::exception& e)
    {
      valid = false;
    }
    if (!valid) ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
    if (ImGui::Button("OK"))
    {
      if (valid)
      {
        ok_callback_(value_as_float);
        ImGui::CloseCurrentPopup();
      }
    }
    if (!valid) ImGui::PopStyleVar();

    ImGui::SameLine();
    if (ImGui::Button("Cancel"))
    {
      ImGui::CloseCurrentPopup();
    }

    ImGui::EndPopup();
  }
}
