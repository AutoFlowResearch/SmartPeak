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

#include <SmartPeak/ui/WorkflowWidget.h>
#include <imgui.h>
#include <imgui_internal.h>
#include <plog/Log.h>
#include <SmartPeak/core/SharedProcessors.h>

#include <mutex> 

namespace SmartPeak
{
  std::vector<std::tuple<float, float, float>> color_palette =
  {
  { 0.996, 0.231, 0.118},
  { 0.599, 0.186, 0.487 },
  { 0.312, 0.009, 0.923 },
  { 0.177, 0.413, 0.795 },
  { 0.003, 0.649, 0.933 },
  { 0.435, 0.919, 0.999 },
  { 0.032, 0.636, 0.605 },
  { 0.982, 0.624, 0.855 },
  { 0.035, 0.991, 0.798 },
  { 0.633, 0.175, 0.196 },
  { 0.167, 0.401, 0.415 },
  { 0.29,  0.286, 0.343 },
  { 0.555, 0.483, 0.644 },
  { 0.717, 0.753, 0.998 },
  { 0.674, 0.744, 0.612 },
  { 0.509, 0.488, 0.44 },
  { 0.352, 0.233, 0.11 },
  { 0.682, 0.396, 0.028 },
  { 0.968, 0.664, 0.19 },
  { 0.84,  0.626, 0.564 },
  { 0.955, 0.915, 0.36 },
  { 0.606, 0.584, 0.004 },
  { 0.339, 0.384, 0.017 },
  { 0.901, 0.111, 0.968 },
  { 0.07,  0.588, 0.231 },
  { 0.98,  0.185, 0.477 },
  { 0.317, 0.88,  0.076 }
  };

  void WorfklowStepNodePlaceHolder::draw()
  {
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    const ImVec2 screen_pos = ImGui::GetCursorScreenPos();
    ImVec2 node_size = getSize();
    draw_list->AddRect(
      ImVec2(screen_pos.x + pos.x, screen_pos.y + pos.y),
      ImVec2(screen_pos.x + pos.x + node_size.x, screen_pos.y + pos.y + node_size.y),
      IM_COL32(200, 200, 200, 200),
      5.0
    );

  }

  ImVec2 WorfklowStepNode::getSize()
  {
    auto width = ImGui::GetWindowWidth();
    return { width, 30 };
  }

  bool WorfklowStepNode::isMouseIn()
  {
    auto mouse_pos = ImGui::GetMousePos();
    const ImVec2 screen_pos = ImGui::GetCursorScreenPos();
    ImVec2 node_size = getSize();
    return  (mouse_pos.x > screen_pos.x + pos.x) && (mouse_pos.x < screen_pos.x + pos.x + node_size.x) &&
            (mouse_pos.y > screen_pos.y + pos.y) && (mouse_pos.y < screen_pos.y + pos.y + node_size.y);
  }

  void WorfklowStepNode::draw()
  {
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    const ImVec2 screen_pos = ImGui::GetCursorScreenPos();
    ImVec2 node_size = getSize();
    int alpha = isMouseIn() ? 255 : 200;
    auto command_type = command_.getType();
    std::tuple<float, float, float> color_tuple{ 1.0, 0.0, 1.0 }; // default color
    switch (command_type)
    {
    case ApplicationHandler::Command::CommandType::RawDataMethod:
      color_tuple = color_palette[0];
      break;
    case ApplicationHandler::Command::CommandType::SampleGroupMethod:
      color_tuple = color_palette[1];
      break;
    case ApplicationHandler::Command::CommandType::SequenceSegmentMethod:
      color_tuple = color_palette[2];
      break;
    default:
      break;
    };
    auto [r, g, b] = color_tuple;
    ImU32 color = IM_COL32(r*255, g*255, b*255, alpha);
    draw_list->AddRectFilled(
      ImVec2(screen_pos.x + pos.x, screen_pos.y + pos.y),
      ImVec2(screen_pos.x + pos.x + node_size.x, screen_pos.y + pos.y + node_size.y),
      color,
      5.0
    );
    auto text_size = ImGui::CalcTextSize(command_.getName().c_str());
    auto text_pos = ImVec2({ pos.x + (node_size.x - text_size.x) / 2 + screen_pos.x, pos.y + (30 - text_size.y) / 2 + screen_pos.y});
    draw_list->AddText(text_pos, IM_COL32(255, 255, 255, 255), command_.getName().c_str());
  }

  void WorfklowStepNodeGraph::draw()
  {
    static int workflow_step_space = 10;

    // update dragging status
    updatecommands();
    if (error_building_commands_)
    {
      ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
      ImGui::Text("Unable to load workflow. Please check log.");
      ImGui::PopStyleColor();
    }
    else
    {
      int node_index = 0;
      for (auto& node : nodes)
      {
        bool is_mouse_clicked = ImGui::IsMouseDragging(0);
        bool is_mouse_dragging = ImGui::IsMouseDragging(0);
        auto mouse_pos = ImGui::GetMousePos();
        auto window_pos = ImGui::GetWindowPos();
        auto window_size = ImGui::GetWindowSize();
        bool mouse_is_in_window = ((mouse_pos.x > window_pos.x) && (mouse_pos.x < (window_pos.x + window_size.x))
                                && (mouse_pos.y > window_pos.y) && (mouse_pos.y < (window_pos.y + window_size.y)));
        if (!mouse_is_in_window)
        {
          is_mouse_clicked = false;
          is_mouse_dragging = false;
        }
        if (!dragging_node_)
        {
          node.is_clicked_ = (is_mouse_clicked && node.isMouseIn());
        }
        if (!dragging_node_ && is_mouse_dragging && node.is_clicked_)
        {
          // start dragging
          node.is_dragging_ = true;
          dragging_node_ = &node;
          dragging_node_index_ = node_index;
        }
        if (!is_mouse_clicked && node.is_dragging_)
        {
          // release
          node.is_dragging_ = false;
          node.drag_delta_ = { 0, 0 };
          dragging_node_ = nullptr;
          if (mouse_is_in_window)
          {
            const auto tmp = application_handler_.sequenceHandler_.getWorkflow().at(dragging_node_index_);
            application_handler_.sequenceHandler_.getWorkflow().erase(application_handler_.sequenceHandler_.getWorkflow().cbegin() + dragging_node_index_);
            application_handler_.sequenceHandler_.getWorkflow().insert(application_handler_.sequenceHandler_.getWorkflow().cbegin() + place_holder_node_index_, tmp);
          }
        }
        node_index++;
      }

      to_display.clear();
      ImVec2 pos{ 0,0 };
      for (auto& node : nodes)
      {
        if (!node.is_dragging_)
        {
          to_display.push_back(&node);
        }
      }

      // update positions
      for (auto& node : to_display)
      {
        node->pos = pos;
        pos.y += node->getSize().y;
        pos.y += workflow_step_space;
      }

      // insert place holder
      if (dragging_node_)
      {
        std::vector<WorfklowStepNode*> to_display_with_placeholder;
        bool place_holder_found = false;
        int node_index = 0;
        for (auto it = to_display.begin(); it != to_display.end(); it++)
        {
          auto& node = *it;
          if (!place_holder_found)
          {
            if (to_display_with_placeholder.empty())
            {
              if (dragging_node_->pos.y + dragging_node_->getSize().y / 2 < node->pos.y + node->getSize().y / 2)
              {
                to_display_with_placeholder.push_back(&place_holder_);
                place_holder_node_index_ = node_index;
                place_holder_found = true;
              }
            }
            else
            {
              auto previous_node = to_display_with_placeholder.back();
              if ((dragging_node_->pos.y + dragging_node_->getSize().y / 2 > previous_node->pos.y + node->getSize().y / 2)
                && (dragging_node_->pos.y + dragging_node_->getSize().y / 2 < node->pos.y + node->getSize().y / 2))
              {
                to_display_with_placeholder.push_back(&place_holder_);
                place_holder_node_index_ = node_index;
                place_holder_found = true;
              }
            }
            node_index++;
          }
          to_display_with_placeholder.push_back(node);
        }
        if (!place_holder_found)
        {
          place_holder_node_index_ = node_index;
          to_display_with_placeholder.push_back(&place_holder_);
        }
        to_display = to_display_with_placeholder;
      }

      // update positions with placeholder
      pos = { 0,0 };
      for (auto& node : to_display)
      {
        node->pos = pos;
        pos.y += node->getSize().y;
        pos.y += workflow_step_space;
      }

      if (dragging_node_)
      {
        to_display.push_back(dragging_node_);
        auto drag_delta = ImGui::GetMouseDragDelta();
        dragging_node_->pos.x += (drag_delta.x - dragging_node_->drag_delta_.x);
        dragging_node_->pos.y += (drag_delta.y - dragging_node_->drag_delta_.y);
        dragging_node_->drag_delta_ = drag_delta;
      }

      // draw
      for (auto& node : to_display)
      {
        node->draw();
      }
    }
  }

  void WorfklowStepNodeGraph::updatecommands()
  {
    const auto& names = application_handler_.sequenceHandler_.getWorkflow();
    if (buildCommandsFromNames_.names_ != names)
    {
      buildCommandsFromNames_.names_ = names;
      error_building_commands_ = !buildCommandsFromNames_.process();
      nodes.clear();
      for (const auto& command : buildCommandsFromNames_.commands_)
      {
        WorfklowStepNode node;
        node.command_ = command;
        nodes.push_back(node);
      }
    }
  }

  void WorkflowWidget::draw()
  {
    workflow_step_widget_.draw();
    bool editable = workflow_manager_.isWorkflowDone();
    if (editable && ImGui::BeginCombo("Presets", NULL))
    {
      static const char* presets[] = {
        "DDA",
        "LCMS MRM Unknowns",
        "LCMS MRM Standards",
        "HPLC UV Unknowns",
        "HPLC UV Standards",
        "GCMS SIM Unknowns",
        "GCMS Full Scan Unknowns",
        "LCMS MRM Validation - QMIP",
        "LCMS MRM Validation - LP",
        "FIAMS Unknowns"
      };
      for (const char* s : presets)
      {
        if (ImGui::Selectable(s))
        {
          std::vector<std::string> ids;
          const std::string s_string { s };
          if (s_string == "DDA")
            ids = { "LOAD_RAW_DATA",
                    "PICK_MS2_FEATURES",
                    "SEARCH_SPECTRUM",
                    "DDA",
                    "STORE_FEATURES"};
          else if (s_string == "LCMS MRM Unknowns")
            ids = { "LOAD_RAW_DATA", 
                    "MAP_CHROMATOGRAMS",
                    "PICK_MRM_FEATURES",
                    "QUANTIFY_FEATURES",
                    "CHECK_FEATURES",
                    "SELECT_FEATURES",
                    "STORE_FEATURES" };
          else if (s_string == "LCMS MRM Standards")
            ids = { "LOAD_RAW_DATA",
                    "MAP_CHROMATOGRAMS",
                    "PICK_MRM_FEATURES",
                    "CHECK_FEATURES",
                    "SELECT_FEATURES",
                    "CALCULATE_CALIBRATION",
                    "STORE_QUANTITATION_METHODS",
                    "QUANTIFY_FEATURES",
                    "STORE_FEATURES" };
          else if (s_string == "HPLC UV Unknowns")
            ids = { "LOAD_RAW_DATA",
                    "MAP_CHROMATOGRAMS",
                    "EXTRACT_CHROMATOGRAM_WINDOWS",
                    "ZERO_CHROMATOGRAM_BASELINE",
                    "PICK_MRM_FEATURES",
                    "QUANTIFY_FEATURES",
                    "CHECK_FEATURES",
                    "SELECT_FEATURES",
                    "STORE_FEATURES" };
          else if (s_string == "HPLC UV Standards")
            ids = { "LOAD_RAW_DATA",
                    "MAP_CHROMATOGRAMS",
                    "EXTRACT_CHROMATOGRAM_WINDOWS",
                    "ZERO_CHROMATOGRAM_BASELINE",
                    "PICK_MRM_FEATURES",
                    "CHECK_FEATURES",
                    "SELECT_FEATURES",
                    "CALCULATE_CALIBRATION",
                    "STORE_QUANTITATION_METHODS",
                    "QUANTIFY_FEATURES",
                    "STORE_FEATURES" };
          else if (s_string == "GCMS SIM Unknowns")
            ids = { "LOAD_RAW_DATA",
                    "MAP_CHROMATOGRAMS",
                    "EXTRACT_CHROMATOGRAM_WINDOWS",
                    "ZERO_CHROMATOGRAM_BASELINE",
                    "PICK_MRM_FEATURES",
                    "QUANTIFY_FEATURES",
                    "CHECK_FEATURES",
                    "SELECT_FEATURES",
                    "STORE_FEATURES" };
          else if (s_string == "GCMS Full Scan Unknowns")
            ids = { "LOAD_RAW_DATA",
                    "MAP_CHROMATOGRAMS",
                    "EXTRACT_CHROMATOGRAM_WINDOWS",
                    "ZERO_CHROMATOGRAM_BASELINE",
                    "PICK_MRM_FEATURES",
                    "QUANTIFY_FEATURES",
                    "CHECK_FEATURES",
                    "SELECT_FEATURES",
                    "STORE_FEATURES" };
          else if (s_string == "LCMS MRM Validation - LP")
            ids = { "LOAD_RAW_DATA",
                    "MAP_CHROMATOGRAMS",
                    "EXTRACT_CHROMATOGRAM_WINDOWS",
                    "PICK_MRM_FEATURES",
                    "FILTER_FEATURES",
                    "FILTER_FEATURES",
                    "SELECT_FEATURES",
                    "VALIDATE_FEATURES",
                    "STORE_FEATURES" };
          else if (s_string == "LCMS MRM Validation - QMIP")
            ids = { "LOAD_RAW_DATA",
                    "MAP_CHROMATOGRAMS",
                    "PICK_MRM_FEATURES",
                    "FILTER_FEATURES",
                    "FILTER_FEATURES",
                    "VALIDATE_FEATURES",
                    "STORE_FEATURES" };
          else if (s_string == "FIAMS Unknowns")
            ids = { "LOAD_RAW_DATA",
                    "EXTRACT_SPECTRA_WINDOWS",
                    "MERGE_SPECTRA",
                    "PICK_MS1_FEATURES",
                    "SEARCH_ACCURATE_MASS",
                    "STORE_ANNOTATIONS",
                    "STORE_FEATURES",
                    "ESTIMATE_FEATURE_BACKGROUND_INTERFERENCES",
                    "STORE_FEATURE_BACKGROUND_ESTIMATIONS",
                    "FILTER_FEATURES_BACKGROUND_INTERFERENCES",
                    "MERGE_FEATURES",
                    "MERGE_INJECTIONS",
                    "STORE_FEATURES_SAMPLE_GROUP" };
          application_handler_.sequenceHandler_.setWorkflow(ids);
          application_handler_.sequenceHandler_.notifyWorkflowUpdated();
          LOGI << "Local workflow has been replaced";
        }
      }
      ImGui::EndCombo();
    }
    if (editable)
    {
      if (ImGui::Button("Add step"))
      {
        ImGui::OpenPopup("Add workflow step");
      }
      ImGui::SameLine();
      if (ImGui::Button("Reset"))
      {
        application_handler_.sequenceHandler_.getWorkflow().clear();
      }
    }

    ImGui::Separator();
    ImGui::Text("Steps");
    ImGui::Separator();
    
    ImGui::BeginChild("Workflow Steps");
    if (application_handler_.sequenceHandler_.getWorkflow().empty())
    {
      ImGui::Text("No steps set. Please select a preset and/or add a single method step.");
    }
    else
    {
      workflow_node_graph_.draw();
    }
    ImGui::EndChild();
  }

}
