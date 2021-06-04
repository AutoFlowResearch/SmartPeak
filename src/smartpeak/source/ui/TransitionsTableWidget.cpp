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

#include <SmartPeak/ui/TransitionsTableWidget.h>
#include <misc/cpp/imgui_stdlib.h>

namespace SmartPeak
{
  static const size_t retention_time_col = 2;
  static const size_t precursor_mz_col = 3;
  static const size_t product_mz_col = 4;

  bool TransitionsTableWidget::isEditable(const size_t row, const size_t col) const
  {
    return ((col == retention_time_col) ||
            (col == precursor_mz_col) ||
            (col == product_mz_col));
  }

  void TransitionsTableWidget::onEdit()
  {
    if (selected_cells_.empty())
      return;

    // we get one selected cell to display the default value and the edited column
    auto one_selected_cell = selected_cells_.back();
    size_t row = std::get<0>(one_selected_cell);
    size_t col = std::get<1>(one_selected_cell);

    auto transition = getTransitionFromTable(row, col);
    if (!transition)
    {
      LOGE << "Cannot edit cell (" << row << ", " << col << ")";
      return;
    }

    if (col == retention_time_col)
    {
      auto& targeted_exp = sequence_handler_->getSequence().at(0).getRawData().getTargetedExperiment();
      double rt = targeted_exp.getPeptideByRef(transition->getPeptideRef()).getRetentionTime();
      retention_time_editor_.open(static_cast<float>(rt),
        [this, &targeted_exp](const float& value)
      {
        for (const auto selected_cell : selected_cells_)
        {
          auto transition = getTransitionFromTable(std::get<0>(selected_cell), std::get<1>(selected_cell));
          if (transition)
          {
//            OpenMS::TargetedExperimentHelper::RetentionTime rt = targeted_exp.getPeptideByRef(transition->getPeptideRef()).rts[0];
//            rt.setRT(value);
//            targeted_exp.getPeptideByRef(transition->getPeptideRef()).rts.cle[0] = rt;
            //rt.setRT(value);
//            OpenMS::ReactionMonitoringTransition modified_transition = *transition;
//            modified_transition.setRetentionTime()
          }
//          injection->getMetaData().setSequenceSegmentName(sequence_segment_name);
        }
        sequence_handler_->notifyTransitionsUpdated();
      });
    }
  }

  void TransitionsTableWidget::drawPopups()
  {
    retention_time_editor_.draw();
  }

  const OpenMS::ReactionMonitoringTransition* TransitionsTableWidget::getTransitionFromTable(const size_t row, const size_t col)
  {
    if (sequence_handler_ == nullptr || sequence_handler_->getSequence().size() == 0)
    {
      return nullptr;
    }
    const OpenMS::ReactionMonitoringTransition* result = nullptr;
    std::string transition_name = table_entries_[row].entry_contents[1];
    auto& transitions = sequence_handler_->getSequence().at(0).getRawData().getTargetedExperiment().getTransitions();
    auto find_it = std::find_if(transitions.begin(), transitions.end(),
      [&](auto& transition) { return transition.getNativeID() == transition_name; });
    if (find_it != transitions.end())
    {
      result = &(*find_it);
    }
    return result;
  }

}
