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
  static const size_t sample_group_col = 2;
  static const size_t sequence_segment_col = 3;
  static const size_t replicate_group_name_col = 4;
  static const size_t sample_type_col = 5;

  bool SequenceTableWidget::isEditable(const size_t row, const size_t col) const
  {
    return ((col == sequence_segment_col) || 
            (col == sample_group_col) ||
            (col == replicate_group_name_col) ||
            (col == sample_type_col));
  }

  void SequenceTableWidget::onEdit()
  {
    if (selected_cells_.empty())
      return;

    // we get one selected cell to display the default value and the edited column
    auto one_selected_cell = selected_cells_.back();
    size_t row = std::get<0>(one_selected_cell);
    size_t col = std::get<1>(one_selected_cell);

    auto injection = getInjectionFromTable(row, col);
    if (!injection)
    {
      LOGE << "Cannot edit cell (" << row << ", " << col << ")";
      return;
    }

    if (col == sequence_segment_col)
    {
      sequence_segment_editor_.open(getSequenceGroups(sequence_segment_col), injection->getMetaData().getSequenceSegmentName(), 
        [this](const std::string& sequence_segment_name)
      {
        for (const auto selected_cell : selected_cells_)
        {
          auto injection = getInjectionFromTable(std::get<0>(selected_cell), std::get<1>(selected_cell));
          injection->getMetaData().setSequenceSegmentName(sequence_segment_name);
        }
        sequence_handler_->notifySequenceChanged();
      });
    }
    else if (col == sample_group_col)
    {
      sample_group_editor_.open(getSequenceGroups(sample_group_col), injection->getMetaData().getSampleGroupName(), 
        [this](const std::string& sample_group_name)
      {
        for (const auto selected_cell : selected_cells_)
        {
          auto injection = getInjectionFromTable(std::get<0>(selected_cell), std::get<1>(selected_cell));
          injection->getMetaData().setSampleGroupName(sample_group_name);
        }
        sequence_handler_->notifySequenceChanged();
      });
    }
    else if (col == replicate_group_name_col)
    {
      replicate_group_name_editor_.open(getSequenceGroups(replicate_group_name_col), injection->getMetaData().getReplicateGroupName(),
        [this](const std::string& replicate_group_name)
      {
        for (const auto selected_cell : selected_cells_)
        {
          auto injection = getInjectionFromTable(std::get<0>(selected_cell), std::get<1>(selected_cell));
          injection->getMetaData().setReplicateGroupName(replicate_group_name);
        }
        sequence_handler_->notifySequenceChanged();
      });
    }
    else if (col == sample_type_col)
    {
      sample_type_editor_.open(injection->getMetaData().getSampleTypeAsString(),
        [this](const std::string& sample_type_name)
      {
        if (stringToSampleType.find(sample_type_name) != stringToSampleType.end())
        {
          const auto sample_type = stringToSampleType.at(sample_type_name);
          for (const auto selected_cell : selected_cells_)
          {
            auto injection = getInjectionFromTable(std::get<0>(selected_cell), std::get<1>(selected_cell));
            injection->getMetaData().setSampleType(sample_type);
          }          
          sequence_handler_->notifySequenceChanged();
        }
      });
    }
  }

  void SequenceTableWidget::drawPopups()
  {
    sequence_segment_editor_.draw();
    sample_group_editor_.draw();
    replicate_group_name_editor_.draw();
    sample_type_editor_.draw();
  }

  std::set<std::string> SequenceTableWidget::getSequenceGroups(const size_t col)
  {
    std::set<std::string> groups;
    for (size_t row = 0; row < table_data_.body_.dimension(0); ++row) {
      if (checked_rows_.size() <= 0 || (checked_rows_.size() > 0 && checked_rows_(row))) {
        std::string group = table_data_.body_(row, col);
        groups.insert(group);
      }
    }
    return groups;
  }

  InjectionHandler* SequenceTableWidget::getInjectionFromTable(const size_t row, const size_t col)
  {
    InjectionHandler* injection = nullptr;
    std::string sample_name = table_data_.body_(row, 1);
    auto find_it = std::find_if(sequence_handler_->getSequence().begin(),
      sequence_handler_->getSequence().end(),
      [&](const auto& injection_handler) { return injection_handler.getMetaData().getSampleName() == sample_name; });
    if (find_it != sequence_handler_->getSequence().end())
    {
      injection = &(*find_it);
    }
    return injection;
  }

}
