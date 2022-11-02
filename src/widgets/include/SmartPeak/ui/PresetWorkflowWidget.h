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
// $Authors: Douglas McCloskey, Pasquale Domenico Colaianni $
// --------------------------------------------------------------------------
#pragma once

#include <SmartPeak/ui/Widget.h>
#include <SmartPeak/core/PresetWorkflows/PresetWorkflow.h>
#include <SmartPeak/core/ApplicationHandler.h>
#include <string>
#include <vector>

namespace SmartPeak
{
  struct IPresetWorkflowWidgetObserver
  {
    /**
    * @brief User has set preset workflow and pressed ok button
    */
    virtual void onPresetWorkflowSelected(const PresetWorkflow& preset_workflow) = 0;
  };

  class PresetWorkflowWidget final : public Widget
  {
  public:
    PresetWorkflowWidget(ApplicationHandler& application_handler,
                         IPresetWorkflowWidgetObserver& preset_workflow_observer) :
      application_handler_(application_handler),
      preset_workflow_observer_(preset_workflow_observer),
      selected_preset_index_(0)
      {};

    void draw() override;

  protected:
    ApplicationHandler& application_handler_;
    IPresetWorkflowWidgetObserver& preset_workflow_observer_;
    std::string selected_method_type_;
    std::string selected_method_;
    int selected_preset_index_;
  };
}
