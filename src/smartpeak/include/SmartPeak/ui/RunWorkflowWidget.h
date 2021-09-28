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
#pragma once

#include <SmartPeak/ui/Widget.h>
#include <SmartPeak/core/ApplicationHandler.h>
#include <SmartPeak/core/WorkflowManager.h>
#include <SmartPeak/ui/FilePicker.h>

#include <string>
#include <vector>

namespace SmartPeak
{
  class RunWorkflowWidget final : public Widget
  {
  public:
    RunWorkflowWidget(ApplicationHandler& application_handler,
      SessionHandler& session_handler, 
      WorkflowManager& workflow_manager, 
      IApplicationProcessorObserver& application_processor_observer,
      ISequenceProcessorObserver& sequence_processor_observer,
      ISequenceSegmentProcessorObserver& sequence_segment_processor_observer,
      ISampleGroupProcessorObserver& sample_group_processor_observer) :
      Widget("Run Workflow"),
      application_processor_observer_(application_processor_observer),
      sequence_processor_observer_(sequence_processor_observer),
      sequence_segment_processor_observer_(sequence_segment_processor_observer),
      sample_group_processor_observer_(sample_group_processor_observer),
      application_handler_(application_handler),
      session_handler_(session_handler),
      workflow_manager_(workflow_manager)
    {
    };

    void draw() override;

  protected:
    std::optional<std::tuple<std::string, std::shared_ptr<IFilePickerHandler>>> popup_file_picker_;
    FilePicker file_picker_;
    IApplicationProcessorObserver& application_processor_observer_;
    ISequenceProcessorObserver& sequence_processor_observer_;
    ISequenceSegmentProcessorObserver& sequence_segment_processor_observer_;
    ISampleGroupProcessorObserver& sample_group_processor_observer_;
    ApplicationHandler& application_handler_;
    SessionHandler& session_handler_;
    WorkflowManager& workflow_manager_;
    std::string mzML_dir_edit_;
    std::string features_in_dir_edit_;
    std::string features_out_dir_edit_;
    std::string mzML_dir_old_;
    std::string features_in_dir_old_;
    std::string features_out_dir_old_;
  };
}
