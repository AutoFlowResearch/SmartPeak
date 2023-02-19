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
// $Authors: Douglas McCloskey, Pasquale Domenico Colaianni $
// --------------------------------------------------------------------------

#include <SmartPeak/ui/SplitWindow.h>
#include <SmartPeak/core/Utilities.h>
#include <imgui_internal.h>

namespace SmartPeak
{

  void SplitWindow::setupLayoutLoader(LayoutLoader& layout_loader)
  {
    if (reset_layout_)
    {
      current_layout_.clear();
      current_layout_ = new_layout_;
    }
    const auto& top_windows_ = current_layout_.at("top");
    const auto& bottom_windows_ = current_layout_.at("bottom");
    const auto& left_windows_ = current_layout_.at("left");

    for (auto& [window, visible] : top_windows_)
    {
      layout_loader.properties_handlers_.push_back(window.get());
    }
    for (auto& [window, visible] : bottom_windows_)
    {
      layout_loader.properties_handlers_.push_back(window.get());
    }
    for (auto& [window, visible] : left_windows_)
    {
      layout_loader.properties_handlers_.push_back(window.get());
    }
    layout_loader.properties_handlers_.push_back(&win_size_and_pos_);
  }

  void SplitWindow::showWindows(const std::vector<std::tuple<std::shared_ptr<Widget>, bool>>& windows)
  {
    for (auto& [widget, visible] : windows)
    {
      if (widget->visible_)
      {
        if (ImGui::Begin(widget->title_.c_str(), &widget->visible_))
        {
          showQuickHelpToolTip(widget->title_);
          widget->draw();
        }
        ImGui::End();
      }
    }
  }

  void SplitWindow::focusFirstWindow(const std::vector<std::tuple<std::shared_ptr<Widget>, bool>>& windows)
  {
    for (auto& [widget, visible] : windows)
    {
      if (widget->visible_)
      {
        ImGui::SetWindowFocus(widget->title_.c_str());
        break;
      }
    }
  }

  void SplitWindow::draw()
  {
    ImGuiIO& io = ImGui::GetIO();
    static const int menu_height = 18;
    static const int padding_height = 50;
    auto main_view_pos = ImGui::GetMainViewport()->Pos;
    ImGui::SetNextWindowSize(ImVec2(io.DisplaySize.x, io.DisplaySize.y - menu_height));
    ImGui::SetNextWindowPos(ImVec2(main_view_pos.x, main_view_pos.y + menu_height));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0);
    static bool visible = true;
    if (ImGui::Begin("Splitter", &visible, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus))
    {
      if (reset_layout_)
      {
        current_layout_.clear();
        current_layout_ = new_layout_;
      }

      const auto& top_windows = current_layout_.at("top");
      const auto& bottom_windows = current_layout_.at("bottom");
      const auto& left_windows = current_layout_.at("left");

      // Build default docking
      ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
      if (reset_layout_)
      {
        ImGuiID left_node;
        ImGuiID center_node;
        ImGuiID bottom_node;
        ImGui::DockBuilderAddNode(dockspace_id, ImGuiDockNodeFlags_DockSpace);
        ImGui::DockBuilderSetNodeSize(dockspace_id, ImVec2(io.DisplaySize.x, io.DisplaySize.y - menu_height));
        ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Left, 0.3, &left_node, &center_node); // The second parameter defines the direction of the split
        ImGui::DockBuilderSplitNode(center_node, ImGuiDir_Down, 0.5, &bottom_node, &center_node); // The second parameter defines the direction of the split
        for (auto& [widget, visible] : left_windows)
        {
          widget->visible_ = visible;
          ImGui::DockBuilderDockWindow(widget->title_.c_str() , left_node);
        }
        for (auto& [widget, visible] : top_windows)
        {
          widget->visible_ = visible;
          ImGui::DockBuilderDockWindow(widget->title_.c_str(), center_node);
        }
        for (auto& [widget, visible] : bottom_windows)
        {
          widget->visible_ = visible;
          ImGui::DockBuilderDockWindow(widget->title_.c_str(), bottom_node);
        }
        ImGui::DockBuilderFinish(dockspace_id);
      }

      ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None);

      // Instanciate windows
      showWindows(left_windows);
      showWindows(top_windows);
      showWindows(bottom_windows);
      if (reset_layout_)
      {
        focusFirstWindow(left_windows);
        focusFirstWindow(top_windows);
        focusFirstWindow(bottom_windows);
      }
      reset_layout_ = false;
    }
    ImGui::End();
    ImGui::PopStyleVar();
    return;
  }

  void SplitWindow::resetLayout(const std::map<std::string, std::vector<std::tuple<std::shared_ptr<Widget>, bool>>>& layout)
  {
    new_layout_ = layout;
    // complete with the default layout
    for (const auto& default_entry : default_layout_)
    {
      for (const auto& [default_widget, default_visible] : default_entry.second)
      {
        bool found = false;
        for (const auto& new_entry : layout)
        {
          for (const auto& [new_widget, new_visible] : new_entry.second)
          {
            if (default_widget.get() == new_widget.get())
            {
              found = true;
              break;
            }
            if (found)
            {
              break;
            }
          }
        }
        if (!found)
        {
          new_layout_[default_entry.first].push_back(std::make_tuple(default_widget, false)); //  default is not found, add it but invisible
        }
      }
    }
    reset_layout_ = true;
  }

}
