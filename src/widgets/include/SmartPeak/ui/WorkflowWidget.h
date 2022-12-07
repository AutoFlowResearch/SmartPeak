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
#include <SmartPeak/ui/WorkflowStepWidget.h>
#include <SmartPeak/ui/PresetWorkflowWidget.h>
#include <SmartPeak/ui/SplitWindow.h>
#include <SmartPeak/core/ApplicationHandler.h>
#include <SmartPeak/core/WorkflowManager.h>
#include <SmartPeak/core/ApplicationProcessors/BuildCommandsFromNames.h>
#include <string>
#include <vector>

namespace SmartPeak
{

  struct Canvas
  {
    virtual ImVec2 getScreenPosition();
    virtual ImVec2 getSize();
    virtual float getWidth() { return 0; };
    virtual float getHeight() { return 0; };
    virtual bool isMouseIn();

    std::weak_ptr<Canvas> parent_;
    ImVec2 pos_;
  };

  struct WorfklowStepNodeIO : Canvas
  {
    ImVec2 getInputLinkScreenPosition();
    ImVec2 getOuputLinkScreenPosition();
    virtual float getWidth() override;
    virtual float getHeight() override;
    virtual void draw(bool enable);
    std::string text_;
  };

  struct WorfklowStepNode : public Canvas
  {
    virtual ImVec2 getScreenPosition() override;
    virtual float getWidth() override;
    virtual float getHeight() override;
    virtual void draw(bool enable);
    void layout();
    
    virtual std::string getName() const { return ""; };
    virtual std::string getDescription() const { return ""; };
    virtual ImU32 getColor(float alpha) const { return IM_COL32(255, 0, 255, alpha); };
    virtual std::string getType() const { return ""; };

    int width_;
    ImVec2 drag_draging_mouse_position_;
    bool is_dragging_ = false;
    bool is_mouse_down_ = false;
    std::vector<WorfklowStepNodeIO> outputs_;
    std::vector<WorfklowStepNodeIO> inputs_;
    bool is_close_button_mouse_in_ = false;
    int node_index_;

  protected:
    bool isCloseButtonMouseIn();
    std::tuple<int, int, int, int> getCloseButtonPosition();
  };

  struct WorfklowStepNodeSession : public WorfklowStepNode
  {
    virtual std::string getName() const override;
    virtual ImU32 getColor(float alpha) const override;
    virtual std::string getType() const override;
  };

  struct WorfklowStepNodeCommand : public WorfklowStepNode
  {
    virtual std::string getDescription() const override;
    virtual std::string getName() const override;
    virtual ImU32 getColor(float alpha) const override;
    virtual std::string getType() const override;
    ApplicationHandler::Command command_;
  };

  struct WorfklowStepNodePlaceHolder : public WorfklowStepNodeCommand
  {
    virtual void draw(bool enable);
  };

  struct WorfklowStepNodeGraphContainer : public Canvas
  {
    virtual ImVec2 getSize() override;
    std::vector<std::shared_ptr<WorfklowStepNode>> to_display_;
    void draw(bool enable);
    void layout();
    std::string type_;
  protected:
    std::string getHeaderText() const;
  };

  struct WorfklowStepNodeGraph
  {
    WorfklowStepNodeGraph(ApplicationHandler& application_handler, WorkflowManager& workflow_manager)
      : application_handler_(application_handler),
       workflow_manager_(workflow_manager),
       buildCommandsFromNames_(application_handler)
    {
      place_holder_ = std::make_shared< WorfklowStepNodePlaceHolder>();
    };
    std::vector<std::shared_ptr<WorfklowStepNode>> nodes;
    void draw();
  
  protected:
    virtual void updatecommands();
    void createContainers();
    void layout();
    void setupCurrentSessionNode();

  protected:
    std::shared_ptr<WorfklowStepNode> current_session_node_;
    std::vector<std::shared_ptr<WorfklowStepNode>> to_display_;
    std::vector<std::shared_ptr<WorfklowStepNodeGraphContainer>> containers_;
    std::shared_ptr<WorfklowStepNodeCommand> dragging_node_;
    int place_holder_node_index_ = 0;
    std::shared_ptr<WorfklowStepNodePlaceHolder> place_holder_;
    WorkflowManager& workflow_manager_;
    ApplicationHandler& application_handler_;
    BuildCommandsFromNames buildCommandsFromNames_;
    bool error_building_commands_ = false;
    bool is_graph_hovered_ = false;
  };

  class WorkflowWidget : public Widget, public IPresetWorkflowWidgetObserver
  {
  public:

    WorkflowWidget(const std::string title, 
                  ApplicationHandler& application_handler, 
                  WorkflowManager& workflow_manager,
                  SplitWindow& split_window)
      : Widget(title),
      application_handler_(application_handler),
      workflow_step_widget_(application_handler),
      workflow_manager_(workflow_manager),
      workflow_node_graph_(application_handler, workflow_manager),
      preset_workflow_widget_(application_handler, *this),
      split_window_(split_window)
    {
    };

    void draw() override;

    // from IPresetWorkflowWidgetObserver
    virtual void onPresetWorkflowSelected(const PresetWorkflow& preset_workflow) override;

  protected:
    ApplicationHandler& application_handler_;
    WorkflowStepWidget workflow_step_widget_;
    WorkflowManager& workflow_manager_;
    WorfklowStepNodeGraph workflow_node_graph_;
    PresetWorkflowWidget preset_workflow_widget_;
    SplitWindow& split_window_;
  };
}
