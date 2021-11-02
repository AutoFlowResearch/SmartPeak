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

#include <SmartPeak/ui/SplitWindow.h>

namespace SmartPeak
{

  void SplitWindow::setupLayoutLoader(LayoutLoader& layout_loader)
  {
    for (auto& window : top_windows)
    {
      layout_loader.properties_handlers_.push_back(window.get());
    }
    for (auto& window : bottom_windows)
    {
      layout_loader.properties_handlers_.push_back(window.get());
    }
    for (auto& window : left_windows)
    {
      layout_loader.properties_handlers_.push_back(window.get());
    }
    layout_loader.properties_handlers_.push_back(&win_size_and_pos);
  }

  void SplitWindow::draw()
  {
    // ======================================
    // Window size computation
    // ======================================
    bool show_top_window_ = std::find_if(top_windows.begin(), top_windows.end(), [](const auto& w) { return w->visible_; }) != top_windows.end();
    bool show_bottom_window_ = std::find_if(bottom_windows.begin(), bottom_windows.end(), [](const auto& w) { return w->visible_; }) != bottom_windows.end();
    bool show_left_window_ = std::find_if(left_windows.begin(), left_windows.end(), [](const auto& w) { return w->visible_; }) != left_windows.end();
    bool show_right_window_ = false;
    win_size_and_pos.setWindowSizesAndPositions(show_top_window_, show_bottom_window_, show_left_window_, show_right_window_);


    // windfow flags common to top, left and bottom windows
    const ImGuiWindowFlags window_flags =
      ImGuiWindowFlags_NoTitleBar |
      ImGuiWindowFlags_NoMove |
      ImGuiWindowFlags_NoCollapse |
      ImGuiWindowFlags_NoFocusOnAppearing;

    // Left window
    if (show_left_window_) {
      ImGui::SetNextWindowPos(ImVec2(win_size_and_pos.left_window_x_pos_, win_size_and_pos.left_and_right_window_y_pos_));
      ImGui::SetNextWindowSize(ImVec2(win_size_and_pos.left_window_x_size_, win_size_and_pos.left_and_right_window_y_size_));
      ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0);
      ImGui::Begin("Left window", NULL, window_flags);
      if (ImGui::BeginTabBar("Left window tab bar", ImGuiTabBarFlags_Reorderable | ImGuiTabBarFlags_AutoSelectNewTabs))
      {
        for (auto& widget : left_windows)
        {
          if (ImGui::BeginTabItem(widget->title_.c_str(), &widget->visible_))
          {
            widget->setWindowSize(win_size_and_pos.left_window_x_size_, win_size_and_pos.left_and_right_window_y_size_);
            showQuickHelpToolTip(widget->title_);
            widget->draw();
            ImGui::EndTabItem();
          }
        }
        ImGui::EndTabBar();
      }
      win_size_and_pos.setLeftWindowXSize(ImGui::GetWindowWidth());
      win_size_and_pos.setWindowSizesAndPositions(show_top_window_, show_bottom_window_, show_left_window_, show_right_window_);
      ImGui::End();
      ImGui::PopStyleVar();
    }

    // Top window
    if (show_top_window_)
    {
      ImGui::SetNextWindowPos(ImVec2(win_size_and_pos.bottom_and_top_window_x_pos_, win_size_and_pos.top_window_y_pos_));
      ImGui::SetNextWindowSize(ImVec2(win_size_and_pos.bottom_and_top_window_x_size_, win_size_and_pos.top_window_y_size_));
      ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0);
      ImGui::Begin("Top window", NULL, window_flags);
      if (ImGui::BeginTabBar("Top window tab bar", ImGuiTabBarFlags_Reorderable | ImGuiTabBarFlags_AutoSelectNewTabs))
      {
        for (auto& widget : top_windows)
        {
          if (ImGui::BeginTabItem(widget->title_.c_str(), &widget->visible_))
          {
            widget->setWindowSize(win_size_and_pos.bottom_and_top_window_x_size_, win_size_and_pos.top_window_y_size_);
            showQuickHelpToolTip(widget->title_);
            widget->draw();
            ImGui::EndTabItem();
          }
        }
        ImGui::EndTabBar();
      }
      win_size_and_pos.setTopWindowYSize(ImGui::GetWindowHeight());
      win_size_and_pos.setLeftWindowXSize(ImGui::GetWindowPos().x);
      win_size_and_pos.setWindowSizesAndPositions(show_top_window_, show_bottom_window_, show_left_window_, show_right_window_);
      ImGui::End();
      ImGui::PopStyleVar();
    }

    // Bottom window
    if (show_bottom_window_)
    {
      ImGui::SetNextWindowPos(ImVec2(win_size_and_pos.bottom_and_top_window_x_pos_, win_size_and_pos.bottom_window_y_pos_));
      ImGui::SetNextWindowSize(ImVec2(win_size_and_pos.bottom_and_top_window_x_size_, win_size_and_pos.bottom_window_y_size_));
      ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0);
      ImGui::Begin("Bottom window", NULL, window_flags);
      if (ImGui::BeginTabBar("Bottom window tab bar", ImGuiTabBarFlags_Reorderable | ImGuiTabBarFlags_AutoSelectNewTabs))
      {
        for (auto& widget : bottom_windows)
        {
          if (ImGui::BeginTabItem(widget->title_.c_str(), &widget->visible_))
          {
            widget->setWindowSize(win_size_and_pos.bottom_and_top_window_x_size_, win_size_and_pos.bottom_window_y_size_);
            widget->draw();
            ImGui::EndTabItem();
          }
        }
        ImGui::EndTabBar();
      }
      ImGui::End();
      ImGui::PopStyleVar();
    }
  }
}
