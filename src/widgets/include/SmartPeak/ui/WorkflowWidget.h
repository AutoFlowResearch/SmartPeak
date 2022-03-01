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
#include <SmartPeak/ui/WorkflowStepWidget.h>
#include <SmartPeak/core/ApplicationHandler.h>
#include <SmartPeak/core/WorkflowManager.h>
#include <SmartPeak/core/ApplicationProcessors/BuildCommandsFromNames.h>
#include <string>
#include <vector>

namespace SmartPeak
{
  struct WorfklowStepNode
  {
    ImVec2 pos;
    ImVec2 drag_delta_;
    ApplicationHandler::Command command_;
    bool is_dragging_ = false;
    bool is_clicked_ = false;
    virtual ImVec2 getSize();
    virtual void draw();
    bool isMouseIn();
  };

  struct WorfklowStepNodePlaceHolder : public WorfklowStepNode
  {
    virtual void draw();
  };

  struct WorfklowStepNodeGraph
  {
    WorfklowStepNodeGraph(ApplicationHandler& application_handler, WorkflowManager& workflow_manager)
      : application_handler_(application_handler),
       workflow_manager_(workflow_manager),
       buildCommandsFromNames_(application_handler)
    {
    };
    std::vector<WorfklowStepNode> nodes;
    void draw();
  
  protected:
    virtual void updatecommands();

  protected:
    std::vector<WorfklowStepNode*> to_display;
    WorfklowStepNode* dragging_node_ = nullptr;
    int dragging_node_index_ = 0;
    int place_holder_node_index_ = 0;
    WorfklowStepNodePlaceHolder place_holder_;
    WorkflowManager& workflow_manager_;
    ApplicationHandler& application_handler_;
    BuildCommandsFromNames buildCommandsFromNames_;
    bool error_building_commands_ = false;
  };

  class WorkflowWidget : public Widget
  {
  public:

    WorkflowWidget(const std::string title, ApplicationHandler& application_handler, WorkflowManager& workflow_manager)
      : Widget(title),
      application_handler_(application_handler),
      workflow_step_widget_(application_handler),
      workflow_manager_(workflow_manager),
      workflow_node_graph_(application_handler, workflow_manager)
    {
    };

    void draw() override;

  protected:
    ApplicationHandler& application_handler_;
    WorkflowStepWidget workflow_step_widget_;
    WorkflowManager& workflow_manager_;
//    BuildCommandsFromNames buildCommandsFromNames_;
//    bool error_building_commands_ = false;
    WorfklowStepNodeGraph workflow_node_graph_;
  };
}
