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
#include <misc/cpp/imgui_stdlib.h>

namespace SmartPeak
{
  bool SequenceTableWidget::isEditable(const size_t row, const size_t col) const
  {
    return (col == 3);
  }

  void SequenceTableWidget::onEdit(const size_t row, const size_t col)
  {
    ImGui::OpenPopup("Edit Sequence Segment");
  }

  void SequenceTableWidget::drawPopups()
  {
    drawXEditor();
  }

  void SequenceTableWidget::drawXEditor()
  {
    float popup_width = 400;

    if (!ImGui::BeginPopupModal("Edit Sequence Segment", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
      init_sequence_group_popup_ = false;
      return;
    }

    if (!init_sequence_group_popup_)
    {
      getSequenceGroups();
      new_sequence_segment_.clear();
      sample_name = table_data_.body_(hovered_row_, 1);
      auto find_it = std::find_if(sequence_handler_->getSequence().begin(),
                                  sequence_handler_->getSequence().end(), 
        [&](const auto& injection_handler) { return injection_handler.getMetaData().getSampleName() == sample_name; });
      assert(find_it != sequence_handler_->getSequence().end()); // This should never happen
      injection_ = &(*find_it);
      setInputTextField(injection_->getMetaData().getSequenceSegmentName());
      init_sequence_group_popup_ = true;
    }

    // one documented way to set popup width
    const auto cursor_pos = ImGui::GetCursorPosX();
    ImGui::SetCursorPosX(popup_width);
    ImGui::SetCursorPosX(cursor_pos);

    ImGui::RadioButton("Move to existing segment", &action_choice_, EActionChoice_MoveSegment);
    ImGui::SameLine();
    ImGui::RadioButton("Move to new segment", &action_choice_, EActionChoice_CreateSegment);

    if (action_choice_ == EActionChoice_CreateSegment)
    {
      ImGui::InputText("New segment", &new_sequence_segment_);
    }

    if ((action_choice_ == EActionChoice_MoveSegment))
    {
      if (!sequence_groups_.empty() && ImGui::BeginCombo("Select segment", input_text_field_.data()))
      {
        for (const auto valid_string : sequence_groups_)
        {
          if (ImGui::Selectable(valid_string.c_str()))
          {
            setInputTextField(valid_string);
          }
        }
        ImGui::EndCombo();
      }
    }

    ImGui::Separator(); 
    if (ImGui::Button("OK") && (!input_text_field_.empty()))
    {
      if ((action_choice_ == EActionChoice_CreateSegment) && (!new_sequence_segment_.empty()))
      {
        injection_->getMetaData().setSequenceSegmentName(new_sequence_segment_);
        sequence_handler_->notifySequenceChanged();
        ImGui::CloseCurrentPopup();
      }
      else if ((action_choice_ == EActionChoice_MoveSegment) && (!input_text_field_.empty()))
      {
        injection_->getMetaData().setSequenceSegmentName(std::string(input_text_field_.data()));
        sequence_handler_->notifySequenceChanged();
        ImGui::CloseCurrentPopup();
      }
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

  void SequenceTableWidget::getSequenceGroups()
  {
    sequence_groups_.clear();
    for (size_t row = 0; row < table_data_.body_.dimension(0); ++row) {
      if (checked_rows_.size() <= 0 || (checked_rows_.size() > 0 && checked_rows_(row))) {
        const size_t col = 3;
        std::string group = table_data_.body_(row, col);
        sequence_groups_.insert(group);
      }
    }
  }
}
