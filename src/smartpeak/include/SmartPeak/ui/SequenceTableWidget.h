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

#pragma once

#include <string>
#include <utility>
#include <vector>
#include <functional>
#include <imgui.h>
#include <SmartPeak/core/SessionHandler.h>
#include <SmartPeak/ui/Widget.h>
#include <SmartPeak/ui/SequenceGroupsEditorWidget.h>
#include <SmartPeak/ui/SampleTypeEditorWidget.h>

namespace SmartPeak
{

  class SequenceTableWidget : public GenericTableWidget, public ISequenceObserver
  {
  public:

    SequenceTableWidget(const std::string& table_id,
      const std::string title,
      SessionHandler* session_handler,
      SequenceHandler* sequence_handler,
      DataGetterMethod data_getter = nullptr,
      DataFilterMethod data_filter = nullptr)
      : GenericTableWidget(table_id,
        title,
        session_handler,
        sequence_handler,
        data_getter,
        data_filter),
      sequence_segment_editor_("Edit Sequence Segment", "Move to existing segment", "Create new segment", "New segment", "Select segment"),
      sample_group_editor_("Edit Sample Group", "Move to existing group", "Create new group", "New group", "Select group")
    {
      sequence_handler_->addSequenceObserver(this);
    };

    virtual bool isEditable(const size_t row, const size_t col) const override;
    virtual void onEdit() override;
    virtual void drawPopups();

    /**
    ISequenceObserver
    */
    virtual void sequenceUpdated() override
    {
      data_changed_ = true;
    }

  private:
    std::set<std::string> getSequenceGroups(const size_t col);
    InjectionHandler* getInjectionFromTable(const size_t row, const size_t col);

  protected:
    SequenceGroupsEditorWidget sequence_segment_editor_;
    SequenceGroupsEditorWidget sample_group_editor_;
    SampleTypeEditorWidget sample_type_editor_;
  };

}
