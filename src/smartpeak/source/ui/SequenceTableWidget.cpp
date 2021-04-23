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

#include <SmartPeak/ui/SequenceTableWidget.h>

namespace SmartPeak
{
  bool SequenceTableWidget::isEditable(const size_t row, const size_t col) const
  {
    return (col == 3);
  }

  void SequenceTableWidget::onEdit(const size_t row, const size_t col)
  {
    ImGui::OpenPopup("Edit Sequence");
  }

  void SequenceTableWidget::drawPopups()
  {
    drawXEditor();
  }

  void SequenceTableWidget::drawXEditor()
  {
    float popup_width = 400;

    if (!ImGui::BeginPopupModal("Edit Sequence", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
      return;
    }
    // one documented way to set popup width
    const auto cursor_pos = ImGui::GetCursorPosX();
    ImGui::SetCursorPosX(popup_width);
    ImGui::SetCursorPosX(cursor_pos);

    ImGui::Text("Placeholder");
    std::vector<std::string> possible_values = { "test1","test2","test3" };
    if (!possible_values.empty() && ImGui::BeginCombo("Value", input_text_field_.data()))
    {
      for (const auto valid_string : possible_values)
      {
        if (ImGui::Selectable(valid_string.c_str()))
        {
          setInputTextField(valid_string);
        }
      }
      ImGui::EndCombo();
    }
    ImGui::Separator();


    if (ImGui::Button("OK"))
    {
      ImGui::CloseCurrentPopup();
    }

    ImGui::SameLine();
    if (ImGui::Button("Cancel"))
    {
      ImGui::CloseCurrentPopup();
    }

    ImGui::EndPopup();
  }

  void SequenceTableWidget::setInputTextField(const std::string& value)
  {
    std::fill(input_text_field_.begin(), input_text_field_.end(), 0);
    if (value.size() < input_text_field_.size())
    {
      std::copy(value.begin(), value.end(), input_text_field_.data());
    }
    else
    {
      // else we are in trouble, abnormal long value parameter, do not display.
      // LOGE << "ParameterEditorWidget : Parameter value for " << parameter_.getName() << " is too long to display. (" << value.size() << ")";
      std::string long_value_placeholder("# unable to display");
      std::copy(long_value_placeholder.begin(), long_value_placeholder.end(), input_text_field_.data());
    };
  }
}
