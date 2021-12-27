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
// $Authors: Bertrand Boudaud $
// --------------------------------------------------------------------------

#include <SmartPeak/ui/SequenceGroupsEditorWidget.h>
#include <misc/cpp/imgui_stdlib.h>

namespace SmartPeak
{

  void SequenceGroupsEditorWidget::open(const std::set<std::string>& choices, 
                                        const std::string& current_choice, 
                                        std::function<void(const std::string&)> ok_callback)
  {
    action_choice_ = EActionChoice_MoveSegment;
    ok_callback_ = ok_callback;
    groups_ = choices;
    new_group_.clear();
    current_choice_ = current_choice;
    ImGui::OpenPopup(title_.c_str());
  }

  void SequenceGroupsEditorWidget::draw()
  {
    float popup_width = 400;

    if (!ImGui::BeginPopupModal(title_.c_str(), NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
      return;
    }

    // one documented way to set popup width
    const auto cursor_pos = ImGui::GetCursorPosX();
    ImGui::SetCursorPosX(popup_width);
    ImGui::SetCursorPosX(cursor_pos);

    ImGui::RadioButton(move_option_message_.c_str(), &action_choice_, EActionChoice_MoveSegment);
    ImGui::SameLine();
    ImGui::RadioButton(create_option_message_.c_str(), &action_choice_, EActionChoice_CreateSegment);

    if (action_choice_ == EActionChoice_CreateSegment)
    {
      ImGui::InputText(move_action_message_.c_str(), &new_group_);
    }

    if ((action_choice_ == EActionChoice_MoveSegment))
    {
      if (!groups_.empty() && ImGui::BeginCombo(create_action_message_.c_str(), current_choice_.c_str()))
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
    }

    static const std::vector<char> forbiden_chars = { ';',',' };
    bool new_group_name_is_valid = !std::any_of(forbiden_chars.begin(), forbiden_chars.end(),
                                                  [&](const auto& c) { return new_group_.find(c) != std::string::npos; });
    if (!new_group_name_is_valid)
    {
      ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
      std::ostringstream os;
      os << "Chars not allowed: [";
      for (const auto& c : forbiden_chars)
      {
        os << c;
      }
      os << "]";
      ImGui::Text(os.str().c_str());
      ImGui::PopStyleColor();
    }
    ImGui::Separator();
    bool valid = (action_choice_ == EActionChoice_MoveSegment ||
                 (action_choice_ == EActionChoice_CreateSegment && new_group_name_is_valid));
    if (!valid) ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
    if (ImGui::Button("OK") && valid)
    {
      if (action_choice_ == EActionChoice_CreateSegment)
      {
        ok_callback_(new_group_);
        ImGui::CloseCurrentPopup();
      }
      else if (action_choice_ == EActionChoice_MoveSegment)
      {
        ok_callback_(current_choice_);
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
