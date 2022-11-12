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
      current_layout_ = new_layout;
    }
    const std::vector<std::shared_ptr<Widget>>& top_windows_ = current_layout_.at("top");
    const std::vector<std::shared_ptr<Widget>>& bottom_windows_ = current_layout_.at("bottom");
    const std::vector<std::shared_ptr<Widget>>& left_windows_ = current_layout_.at("left");

    for (auto& window : top_windows_)
    {
      layout_loader.properties_handlers_.push_back(window.get());
    }
    for (auto& window : bottom_windows_)
    {
      layout_loader.properties_handlers_.push_back(window.get());
    }
    for (auto& window : left_windows_)
    {
      layout_loader.properties_handlers_.push_back(window.get());
    }
    layout_loader.properties_handlers_.push_back(&win_size_and_pos_);
  }

  void SplitWindow::showWindows(const std::vector<std::shared_ptr<Widget>>& windows)
  {
    for (auto& widget : windows)
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
        current_layout_ = new_layout;
      }

      const std::vector<std::shared_ptr<Widget>>& top_windows_ = current_layout_.at("top");
      const std::vector<std::shared_ptr<Widget>>& bottom_windows_ = current_layout_.at("bottom");
      const std::vector<std::shared_ptr<Widget>>& left_windows_ = current_layout_.at("left");

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
        for (auto& widget : left_windows_)
        {
          ImGui::DockBuilderDockWindow(widget->title_.c_str() , left_node);
        }
        for (auto& widget : top_windows_)
        {
          ImGui::DockBuilderDockWindow(widget->title_.c_str(), center_node);
        }
        for (auto& widget : bottom_windows_)
        {
          ImGui::DockBuilderDockWindow(widget->title_.c_str(), bottom_node);
        }
        ImGui::DockBuilderFinish(dockspace_id);
        reset_layout_ = false;
      }

      ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None);

      // Instanciate windows
      showWindows(left_windows_);
      showWindows(top_windows_);
      showWindows(bottom_windows_);
    }
    ImGui::End();
    ImGui::PopStyleVar();
    return;
  }

  void SplitWindow::resetLayout(const std::map<std::string, std::vector<std::shared_ptr<Widget>>>& layout)
  {
    new_layout = layout;
    reset_layout_ = true;
  }

}
