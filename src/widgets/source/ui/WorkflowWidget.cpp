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

  // size / position constants
  static int workflow_step_space = 20;
  static int container_out_margin = 10;
  static int container_in_margin = 20;
  static int container_header_height = 30;
  static int close_button_width = 10;
  static int close_button_height = 10;
  static int close_button_right_margin = 8;
  static int command_text_margin = 8;
  static int workflow_step_height = 30;
  static int input_output_width = 10;
  static int input_output_height = 10;

  // color constants
  static int hovered_alpha = 255;
  static int non_hovered_alpha = 200;
  static ImU32 link_color = 0x88FFFFFF;

  std::map<std::string, std::pair<ImU32,ImU32>> all_possible_input_outputs_to_color;

  ImU32 getNodeColorPalete(int index, int alpha)
  {
    static std::vector<ImU32> color_palette =
    {
      0x5d61a2,
      0x93cdd1,
      0xd598a0,
      0xa7d0b8,
      0xdcdcd7
    };
    index = index % color_palette.size();
    auto color = color_palette[index];
    ImU8 r = (color>>16);
    ImU8 g = (color >> 8) & 0xFF;
    ImU8 b = color & 0xFF;
    return IM_COL32(r, g, b, alpha);
  }

  ImU32 getInputOutputColorPalete(int index, int alpha)
  {
    static std::vector<ImU32> color_palette =
    {
      0x5b836d,
      0xfae9e7,
      0xfaafa0,
      0xffe489,
      0xcedd81,
      0xaec086,
      0xb9c5c7,
      0xd7d2cc,
      0xa08c7d,
      0xb3504b,
      0xd6482f,
      0xc7c4ac,
      0xb06d81
    };
    index = index % color_palette.size();
    auto color = color_palette[index];
    ImU8 r = (color >> 16);
    ImU8 g = (color >> 8) & 0xFF;
    ImU8 b = color & 0xFF;
    return IM_COL32(r, g, b, alpha);
  }

  ImVec2 WorfklowStepNodeIO::getSize()
  {
    return { static_cast<float>(input_output_width), static_cast<float>(input_output_height) };
  }

  ImVec2 WorfklowStepNodeIO::getScreenPosition()
  {
    ImVec2 pos = pos_;
    const ImVec2 screen_pos = ImGui::GetCursorScreenPos();
    ImVec2 node_position;
    auto parent = parent_.lock();
    if (parent)
    {
      node_position = parent->getScreenPosition();
    }
    pos.x += node_position.x;
    pos.y += node_position.y;
    return pos;
  }

  ImVec2 WorfklowStepNodeIO::getInputLinkScreenPosition()
  {
    ImVec2 pos = pos_;
    const ImVec2 screen_pos = ImGui::GetCursorScreenPos();
    ImVec2 node_position;
    auto parent = parent_.lock();
    if (parent)
    {
      node_position = parent->getScreenPosition();
    }
    auto io_size = getSize();
    pos.x += node_position.x + io_size.x/2;
    pos.y += node_position.y - 2;
    return pos;
  }

  ImVec2 WorfklowStepNodeIO::getOuputLinkScreenPosition()
  {
    ImVec2 pos = pos_;
    const ImVec2 screen_pos = ImGui::GetCursorScreenPos();
    ImVec2 node_position;
    auto parent = parent_.lock();
    if (parent)
    {
      node_position = parent->getScreenPosition();
    }
    auto io_size = getSize();
    pos.x += node_position.x + io_size.x / 2;
    pos.y += node_position.y + io_size.y - 2;
    return pos;
  }


  void WorfklowStepNodeIO::draw(bool enable)
  {
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    const ImVec2 pos = getScreenPosition();
    auto output_size = getSize();
    ImVec2 a, b, c;
    a.x = pos.x;
    a.y = pos.y;
    b.x = pos.x + output_size.x;
    b.y = pos.y;
    c.x = pos.x + output_size.x/2;
    c.y = pos.y + output_size.y;
    ImU32 color = 0;
    if (isMouseIn() && enable)
    {
      ImGui::BeginTooltip();
      ImGui::Text("%s", text_.c_str());
      ImGui::EndTooltip();
      color = all_possible_input_outputs_to_color.at(text_).first;
    }
    else
    {
      color = all_possible_input_outputs_to_color.at(text_).second;
    }
    draw_list->AddTriangleFilled(a, b, c, color);
  }

  bool WorfklowStepNodeIO::isMouseIn()
  {
    auto mouse_pos = ImGui::GetMousePos();
    const ImVec2 pos = getScreenPosition();
    ImVec2 node_size = getSize();
    return  (mouse_pos.x > pos.x) && (mouse_pos.x < pos.x + node_size.x) &&
      (mouse_pos.y > pos.y) && (mouse_pos.y < pos.y + node_size.y);
  }

  void WorfklowStepNodePlaceHolder::draw(bool enable)
  {
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    const ImVec2 pos = getScreenPosition();
    ImVec2 node_size = getSize();
    draw_list->AddRect(
      ImVec2(pos.x, pos.y),
      ImVec2(pos.x + node_size.x, pos.y + node_size.y),
      IM_COL32(200, 200, 200, 200),
      5.0
    );
  }

  ImVec2 WorfklowStepNode::getSize()
  {
    return { static_cast<float>(getWidth()), static_cast<float>(getHeight()) };
  }

  ImVec2 WorfklowStepNode::getScreenPosition()
  {
    ImVec2 pos = pos_;
    const ImVec2 screen_pos = ImGui::GetCursorScreenPos();
    ImVec2 container_position;
    auto parent = container_.lock();
    if (parent)
    {
      container_position = parent->pos_;
    }
    pos.x += screen_pos.x + container_position.x;
    pos.y += screen_pos.y + container_position.y;
    return pos;
  }

  int WorfklowStepNode::getWidth()
  {
    return width_;
  }

  int WorfklowStepNode::getHeight()
  {
    return workflow_step_height;
  }

  std::tuple<int,int,int,int> WorfklowStepNode::getCloseButtonPosition()
  {
    auto node_size = getSize();
    auto pos_x = node_size.x - close_button_right_margin - close_button_width;
    auto pos_y = node_size.y - (node_size.y - close_button_height)/2 - close_button_height;
    return std::make_tuple(pos_x, pos_y, close_button_width, close_button_height);
  }

  bool WorfklowStepNode::isCloseButtonMouseIn()
  {
    auto mouse_pos = ImGui::GetMousePos();
    const ImVec2 pos = getScreenPosition();
    auto [btn_pos_x, btn_pos_y, btn_pos_w, btn_pos_h] = getCloseButtonPosition();
    return  (mouse_pos.x > pos.x + btn_pos_x) && (mouse_pos.x < pos.x + btn_pos_x + btn_pos_w) &&
            (mouse_pos.y > pos.y + btn_pos_y) && (mouse_pos.y < pos.y + btn_pos_y + btn_pos_h);
  }

  void WorfklowStepNode::layout()
  {
    int index = 0;
    float io_area_width = (std::max((getWidth() - 20), 0) * 0.75);
    float space_width = io_area_width / (all_possible_input_outputs_to_color.size() + 1);
    float first_io_posx = getWidth() - io_area_width;
    for (auto possible_output : all_possible_input_outputs_to_color)
    {
      for (auto& output : outputs_)
      {
        if (output.text_ == possible_output.first)
        {
          output.pos_.x = first_io_posx + (index + 1) * space_width;
          output.pos_.y = getHeight();
        }
      }
      for (auto& input : inputs_)
      {
        if (input.text_ == possible_output.first)
        {
          input.pos_.x = first_io_posx + (index + 1) * space_width;
          input.pos_.y = -input.getSize().y;
        }
      }
      index++;
    }
  }

  bool WorfklowStepNode::isMouseIn()
  {
    auto mouse_pos = ImGui::GetMousePos();
    const ImVec2 pos = getScreenPosition();
    ImVec2 node_size = getSize();
    return  (mouse_pos.x > pos.x) && (mouse_pos.x < pos.x + node_size.x) &&
            (mouse_pos.y > pos.y) && (mouse_pos.y < pos.y + node_size.y);
  }

  void WorfklowStepNode::draw(bool enable)
  {
    // Node container
    auto node_pos = getScreenPosition();
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    const ImVec2 screen_pos = ImGui::GetCursorScreenPos();
    ImVec2 node_size = getSize();
    int alpha = 0;
    is_close_button_mouse_in_ = isCloseButtonMouseIn();
    if (isMouseIn() && enable)
    {
      if (!is_dragging_ && !is_close_button_mouse_in_)
      {
        auto description = getDescription();
        if (!description.empty())
        {
          ImGui::BeginTooltip();
          ImGui::Text("%s", description.c_str());
          ImGui::EndTooltip();
        }
      }
      alpha = hovered_alpha;
    }
    else
    {
      alpha = non_hovered_alpha;
    }
    ImU32 color = getColor(alpha);
    draw_list->AddRectFilled(
      ImVec2(node_pos.x, node_pos.y),
      ImVec2(node_pos.x + node_size.x, node_pos.y + node_size.y),
      color,
      5.0
    );
    
    // Text
    auto name = getName();
    auto text_size = ImGui::CalcTextSize(name.c_str());
    auto text_pos = ImVec2({ node_pos.x + command_text_margin, node_pos.y + (workflow_step_height - text_size.y) / 2 });
    draw_list->AddText(text_pos, IM_COL32(255, 255, 255, 255), name.c_str());

    // Delete button
    auto [btn_pos_x, btn_pos_y, btn_pos_w, btn_pos_h] = getCloseButtonPosition();
    float thickness = 1.0f;
    if (is_close_button_mouse_in_)
    {
      thickness = 2.0f;
    }
    if (isMouseIn())
    {
      draw_list->AddLine(
        ImVec2(node_pos.x + btn_pos_x, node_pos.y + btn_pos_y ),
        ImVec2(node_pos.x + btn_pos_x + btn_pos_w, node_pos.y + btn_pos_y + btn_pos_h),
        IM_COL32(255, 255, 255, alpha),
        thickness
      );
      draw_list->AddLine(
        ImVec2(node_pos.x + btn_pos_x, node_pos.y + btn_pos_y + btn_pos_h),
        ImVec2(node_pos.x + btn_pos_x + btn_pos_w, node_pos.y + btn_pos_y),
        IM_COL32(255, 255, 255, alpha),
        thickness
      );
    }

    // inputs / outputs
    for (auto& input : inputs_)
    {
      input.draw(enable);
    }
    for (auto& output : outputs_)
    {
      output.draw(enable);
    }
  }

  std::string WorfklowStepNodeSession::getName() const
  {
    return "Current Session";
  };

  ImU32 WorfklowStepNodeSession::getColor(float alpha) const
  {
    return getNodeColorPalete(3, alpha);
  }
  
  std::string WorfklowStepNodeSession::getType() const
  { 
    return "Session";
  };

  std::string WorfklowStepNodeCommand::getDescription() const
  {
    return command_.getDescription();
  }

  std::string WorfklowStepNodeCommand::getName() const
  {
    return command_.getName();
  }
  
  std::string WorfklowStepNodeCommand::getType() const
  {
    std::string result;
    auto command_type = command_.getType();
    switch (command_type)
    {
    case ApplicationHandler::Command::CommandType::RawDataMethod:
      result = "RawDataMethod";
      break;
    case ApplicationHandler::Command::CommandType::SampleGroupMethod:
      result = "SampleGroupMethod";
      break;
    case ApplicationHandler::Command::CommandType::SequenceSegmentMethod:
      result = "SequenceSegmentMethod";
      break;
    default:
      break;
    };
    return result;
  }

  ImU32 WorfklowStepNodeCommand::getColor(float alpha) const
  {
    ImU32 color = IM_COL32(255, 0, 255, alpha); // default color
    auto command_type = command_.getType();
    switch (command_type)
    {
    case ApplicationHandler::Command::CommandType::RawDataMethod:
      color = getNodeColorPalete(0, alpha);
      break;
    case ApplicationHandler::Command::CommandType::SampleGroupMethod:
      color = getNodeColorPalete(1, alpha);
      break;
    case ApplicationHandler::Command::CommandType::SequenceSegmentMethod:
      color = getNodeColorPalete(2, alpha);
      break;
    default:
      break;
    };
    return color;
  }

  void WorfklowStepNodeGraph::createContainers()
  {
    containers_.clear();
    auto container = std::make_shared<WorfklowStepNodeGraphContainer>();
    for (auto node : to_display_)
    {
      auto node_type = node->getType();
      if (container->type_ != node_type)
      {
        if (!container->to_display_.empty())
        {
          containers_.push_back(container);
        }
        container = std::make_shared<WorfklowStepNodeGraphContainer>();
        container->type_ = node_type;
      }
      container->to_display_.push_back(node);
      node->container_ = container;
    }
    if (!container->to_display_.empty())
    {
      containers_.push_back(container);
    }
  }

  void WorfklowStepNodeGraph::layout()
  {
    static std::once_flag once_flag;
    std::call_once(once_flag, []
    {
      int color_index = 0;
      for (const auto& p : n_to_raw_data_method_)
      {
        auto outputs = p.second->getOutputs();
        for (auto output : outputs)
        {
          if (!all_possible_input_outputs_to_color.count(output))
          {
            all_possible_input_outputs_to_color.emplace(output, 
              std::make_pair(getInputOutputColorPalete(color_index, hovered_alpha), getInputOutputColorPalete(color_index, non_hovered_alpha)));
            color_index++;
          }
        }
        auto inputs = p.second->getInputs();
        for (auto input : inputs)
        {
          if (!all_possible_input_outputs_to_color.count(input))
          {
            all_possible_input_outputs_to_color.emplace(input,
              std::make_pair(getInputOutputColorPalete(color_index, hovered_alpha), getInputOutputColorPalete(color_index, non_hovered_alpha)));
            color_index++;
          }
        }
      }
      for (const auto& p : n_to_seq_seg_method_)
      {
        auto outputs = p.second->getOutputs();
        for (auto output : outputs)
        {
          if (!all_possible_input_outputs_to_color.count(output))
          {
            all_possible_input_outputs_to_color.emplace(output,
              std::make_pair(getInputOutputColorPalete(color_index, hovered_alpha), getInputOutputColorPalete(color_index, non_hovered_alpha)));
            color_index++;
          }
        }
        auto inputs = p.second->getInputs();
        for (auto input : inputs)
        {
          if (!all_possible_input_outputs_to_color.count(input))
          {
            all_possible_input_outputs_to_color.emplace(input,
              std::make_pair(getInputOutputColorPalete(color_index, hovered_alpha), getInputOutputColorPalete(color_index, non_hovered_alpha))); 
            color_index++;
          }
        }
      }
      for (const auto& p : n_to_sample_group_method_)
      {
        auto outputs = p.second->getOutputs();
        for (auto output : outputs)
        {
          if (!all_possible_input_outputs_to_color.count(output))
          {
            all_possible_input_outputs_to_color.emplace(output,
              std::make_pair(getInputOutputColorPalete(color_index, hovered_alpha), getInputOutputColorPalete(color_index, non_hovered_alpha)));
            color_index++;
          }
        }
        auto inputs = p.second->getInputs();
        for (auto input : inputs)
        {
          if (!all_possible_input_outputs_to_color.count(input))
          {
            all_possible_input_outputs_to_color.emplace(input,
              std::make_pair(getInputOutputColorPalete(color_index, hovered_alpha), getInputOutputColorPalete(color_index, non_hovered_alpha)));
            color_index++;
          }
        }
      }
    });

    ImVec2 pos{ 0,0 };
    for (auto& container : containers_)
    {
      container->pos_ = pos;
      container->layout();
      pos.y += container->getSize().y;
    }
  }

  void WorfklowStepNodeGraph::setupCurrentSessionNode()
  {
    current_session_node_ = std::make_shared<WorfklowStepNodeSession>();
    for (const auto [output_name, output_color] : all_possible_input_outputs_to_color)
    {
      WorfklowStepNodeIO node_output;
      node_output.text_ = output_name;
      node_output.parent_ = current_session_node_;
      current_session_node_->outputs_.push_back(node_output);
    }
  }
  
  void WorfklowStepNodeGraph::draw()
  {
    updatecommands();

    if (error_building_commands_)
    {
      ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
      ImGui::Text("Unable to load workflow. Please check log.");
      ImGui::PopStyleColor();
    }
    else
    {
      if (is_graph_hovered_)
      {
        int node_index = 0;
        for (auto& node : nodes)
        {
          bool is_mouse_down = ImGui::IsMouseDown(0);
          bool is_mouse_dragging = ImGui::IsMouseDragging(0);
          auto mouse_pos = ImGui::GetMousePos();
          auto window_pos = ImGui::GetWindowPos();
          auto window_size = ImGui::GetWindowSize();
          bool mouse_is_in_window = ((mouse_pos.x > window_pos.x) && (mouse_pos.x < (window_pos.x + window_size.x))
            && (mouse_pos.y > window_pos.y) && (mouse_pos.y < (window_pos.y + window_size.y)));
          if (!mouse_is_in_window)
          {
            is_mouse_down = false;
            is_mouse_dragging = false;
          }
          if (!dragging_node_)
          {
            node->is_mouse_down_ = (is_mouse_down && node->isMouseIn());
          }
          if (!dragging_node_ && is_mouse_dragging && node->is_mouse_down_)
          {
            // start dragging
            node->is_dragging_ = true;
            dragging_node_ = std::dynamic_pointer_cast<WorfklowStepNodeCommand>(node);
            dragging_node_index_ = node_index;
          }
          if (!is_mouse_down && node->is_dragging_)
          {
            // release
            node->is_dragging_ = false;
            node->drag_delta_ = { 0, 0 };
            dragging_node_ = nullptr;
            if (mouse_is_in_window)
            {
              const auto tmp = application_handler_.sequenceHandler_.getWorkflow().at(dragging_node_index_);
              application_handler_.sequenceHandler_.getWorkflow().erase(application_handler_.sequenceHandler_.getWorkflow().cbegin() + dragging_node_index_);
              application_handler_.sequenceHandler_.getWorkflow().insert(application_handler_.sequenceHandler_.getWorkflow().cbegin() + place_holder_node_index_, tmp);
              application_handler_.sequenceHandler_.notifyWorkflowUpdated();
              updatecommands();
            }
          }
          if (ImGui::IsMouseClicked(0) && node->is_close_button_mouse_in_)
          {
            application_handler_.sequenceHandler_.getWorkflow().erase(application_handler_.sequenceHandler_.getWorkflow().cbegin() + node_index);
            application_handler_.sequenceHandler_.notifyWorkflowUpdated();
            updatecommands();
          }
          node_index++;
        }
      }

      setupCurrentSessionNode();
      to_display_.clear();
      to_display_.push_back(current_session_node_);
      for (auto node : nodes)
      {
        if (!node->is_dragging_)
        {
          to_display_.push_back(node);
        }
      }

      // create containers
      createContainers();

      // update positions
      layout();

      // insert place holder
      if (dragging_node_)
      {
        std::vector<std::shared_ptr<WorfklowStepNode>> to_display_with_placeholder;
        bool place_holder_found = false;
        int node_index = 0;
        for (auto& node : to_display_)
        {
          auto node_pos = node->getScreenPosition();
          auto dragging_node_pos = dragging_node_->getScreenPosition();
          if (!place_holder_found)
          {
            if (to_display_with_placeholder.empty())
            {
              if (dragging_node_pos.y < node_pos.y)
              {
                place_holder_->command_ = dragging_node_->command_;
                to_display_with_placeholder.push_back(place_holder_);
                place_holder_node_index_ = node_index;
                place_holder_found = true;
              }
            }
            else
            {
              auto previous_node = to_display_with_placeholder.back();
              auto previous_node_pos = previous_node->getScreenPosition();
              if ((dragging_node_pos.y > previous_node_pos.y)
                && (dragging_node_pos.y < node_pos.y))
              {
                place_holder_->command_ = dragging_node_->command_;
                to_display_with_placeholder.push_back(place_holder_);
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
          to_display_with_placeholder.push_back(place_holder_);
        }
        to_display_ = to_display_with_placeholder;
      }

      // create containers with place holder
      createContainers();

      // update positions with place holder
      layout();

      // draw
      float frame_height = 0;
      for (auto& container : containers_)
      {
        frame_height += container->getSize().y + container_out_margin;
      }
      ImGui::BeginChildFrame(1, { 0, frame_height });
      is_graph_hovered_ = (ImGui::IsWindowHovered());

      // draw containers and nodes
      for (auto& container : containers_)
      {
        container->draw(is_graph_hovered_ && (!dragging_node_));
      }

      // draw links
      for (auto src_node_it = to_display_.begin(); src_node_it != to_display_.end(); src_node_it++)
      {
        auto& src_node = *src_node_it;
        for (auto& src_output : src_node->outputs_)
        {
          bool continue_to_search = true;
          for (auto dest_node_it = src_node_it; dest_node_it != to_display_.end(); dest_node_it++)
          {
            auto& dest_node = *dest_node_it;
            if (dest_node.get() != src_node.get())
            {
              for (auto& dest_input : dest_node->inputs_)
              {
                if (src_output.text_ == dest_input.text_)
                {
                  ImDrawList* draw_list = ImGui::GetWindowDrawList();
                  const ImVec2 src_pos = src_output.getOuputLinkScreenPosition();
                  const ImVec2 dest_pos = dest_input.getInputLinkScreenPosition();
                  draw_list->AddLine(src_pos, dest_pos, link_color);
                }
              }
              for (auto& dest_output : dest_node->outputs_)
              {
                if (src_output.text_ == dest_output.text_)
                {
                  continue_to_search = false;
                }
              }
            }
            if (!continue_to_search)
            {
              break;
            }
          }
        }
      }
      // draw dragging node
      if (dragging_node_)
      {
        auto drag_delta = ImGui::GetMouseDragDelta();
        dragging_node_->pos_.x += (drag_delta.x - dragging_node_->drag_delta_.x);
        dragging_node_->pos_.y += (drag_delta.y - dragging_node_->drag_delta_.y);
        dragging_node_->drag_delta_ = drag_delta;
        dragging_node_->draw(is_graph_hovered_);
      }
      ImGui::EndChildFrame();
    }
  }

  ImVec2 WorfklowStepNodeGraphContainer::getSize()
  {
    auto width = ImGui::GetWindowWidth() - container_out_margin *2;
    auto header_text = getHeaderText();
    int height = 0;
    if (!header_text.empty())
    {
      height = container_header_height;
    }
    for (auto node_it = to_display_.begin(); node_it != to_display_.end(); node_it++)
    {
      auto& node = *node_it;
      height += workflow_step_space;
      height += node->getHeight();
    }
    height += workflow_step_space;
    return { static_cast<float>(width), static_cast<float>(height) };
  }

  std::string WorfklowStepNodeGraphContainer::getHeaderText() const
  {
    std::string header_text;
    if (type_ == "RawDataMethod")
    {
      header_text = "for every injection";
    }
    else if (type_ == "SampleGroupMethod")
    {
      header_text = "for every sample group";
    }
    else if (type_ == "SequenceSegmentMethod")
    {
      header_text = "for every sample sequence segment";
    }
    return header_text;
  }

  void WorfklowStepNodeGraphContainer::layout()
  {
    ImVec2 pos;
    pos.x = container_in_margin;
    auto header_text = getHeaderText();
    if (!header_text.empty())
    {
      pos.y = container_header_height;
    }
    pos.y += workflow_step_space;
    auto container_width = getSize().x;
    for (auto& node : to_display_)
    {
      node->pos_ = pos;
      node->width_ = container_width - container_in_margin * 2;
      pos.y += node->getSize().y;
      pos.y += workflow_step_space;
      node->layout();
    }
  }

  void WorfklowStepNodeGraphContainer::draw(bool enable)
  {
    auto container_size = getSize();
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    const ImVec2 screen_pos = ImGui::GetCursorScreenPos();
    auto header_text = getHeaderText();
    if (!header_text.empty())
    {
      draw_list->AddRect(
        ImVec2(screen_pos.x + pos_.x + container_out_margin, screen_pos.y + pos_.y + container_out_margin),
        ImVec2(screen_pos.x + pos_.x + container_size.x, screen_pos.y + pos_.y + container_size.y),
        IM_COL32(255, 255, 255, 255),
        5.0
      );
      draw_list->AddText(
        ImVec2(screen_pos.x + pos_.x + container_out_margin + 5, screen_pos.y + pos_.y + container_out_margin + 5),
        IM_COL32(255, 255, 255, 255),
        header_text.c_str()
      );
    }
    for (auto& node : to_display_)
    {
      node->draw(enable);
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
        auto node = std::make_shared<WorfklowStepNodeCommand>();
        node->command_ = command;
        for (const auto& output : command.getOutputs())
        {
          WorfklowStepNodeIO node_output;
          node_output.text_ = output;
          node_output.parent_ = node;
          node->outputs_.push_back(node_output);
        }
        for (const auto& input : command.getInputs())
        {
          WorfklowStepNodeIO node_input;
          node_input.text_ = input;
          node_input.parent_ = node;
          node->inputs_.push_back(node_input);
        }
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
          const std::string s_string{ s };
          if (s_string == "DDA")
            ids = { "LOAD_RAW_DATA",
                    "PICK_MS2_FEATURES",
                    "SEARCH_SPECTRUM",
                    "DDA",
                    "STORE_FEATURES" };
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
