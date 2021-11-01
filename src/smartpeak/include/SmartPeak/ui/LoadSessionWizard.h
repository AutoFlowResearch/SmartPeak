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
// $Authors: Douglas McCloskey, Bertrand Boudaud $
// --------------------------------------------------------------------------
#pragma once

#include <SmartPeak/iface/IFilePickerHandler.h>
#include <SmartPeak/ui/SessionFilesWidget.h>
#include <string>
#include <memory>

/* 
* This class will check if a file session popup needs to be done before to load the session
* (in case of missing files)
*/
namespace SmartPeak
{
  struct LoadSessionWizard : IFilePickerHandler
  {
    LoadSessionWizard(std::shared_ptr<SessionFilesWidget>& session_files_widget_manage,
                      IApplicationProcessorObserver* application_observer,
                      const std::vector<IPropertiesHandler*>& properties_handlers,
                      WorkflowManager& workflow_manager,
                      ISequenceProcessorObserver* sequence_processor_observer = nullptr,
                      ISequenceSegmentProcessorObserver* sequence_segment_processor_observer = nullptr,
                      ISampleGroupProcessorObserver* sample_group_processor_observer = nullptr) :
      application_observer_(application_observer),
      session_files_widget_manage_(session_files_widget_manage),
      properties_handlers_(properties_handlers),
      workflow_manager_(workflow_manager),
      sequence_processor_observer_(sequence_processor_observer),
      sequence_segment_processor_observer_(sequence_segment_processor_observer),
      sample_group_processor_observer_(sample_group_processor_observer)
    {};

    /**
    IFilePickerHandler
    */
    bool onFilePicked(const std::filesystem::path& filename, ApplicationHandler* application_handler) override;
  protected:
    IApplicationProcessorObserver* application_observer_ = nullptr;
    std::shared_ptr<SessionFilesWidget> session_files_widget_manage_;
    const std::vector<IPropertiesHandler*>& properties_handlers_;
    WorkflowManager& workflow_manager_;
    ISequenceProcessorObserver* sequence_processor_observer_;
    ISequenceSegmentProcessorObserver* sequence_segment_processor_observer_;
    ISampleGroupProcessorObserver* sample_group_processor_observer_;
  };
}
