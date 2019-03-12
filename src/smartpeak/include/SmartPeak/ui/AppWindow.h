#pragma once

#include <imgui.h>
#include <string>
#include <vector>

namespace SmartPeak
{
  class AppWindow
  {
  public:
    AppWindow() = default;
    ~AppWindow() = default;

    /**
      Show the application screen
    */
    static void showApp();
  };

  class MainMenu
  {
  public:
    MainMenu() = default;
    ~MainMenu() = default;

    void showMainMenuBar(); ///< Show the main menu
    void showMenuFile(); ///< Show the main menu File options
    void showMenuEdit(); ///< Show the main menu Edit options
    void showMenuView(); ///< Show the main menu View options
    void showMenuAction(); ///< Show the main menu Action options
    void showMenuHelp(); ///< Show the main menu Help options

  private:
    bool show_sequence_ = false;  ///< Show the sequence widget
    bool show_sequence_segments_ = false;  ///< Show the sequence segment widget
    bool show_sample_groups_ = false;  ///< Show the sample groups widget
    bool show_workflow_wizard_ = false;  ///< Show the workflow wizard widget
    bool show_feature_plot_ = false;  ///< Show the feature plot widget
    bool show_metric_plot_ = false;  ///< Show the metric plot widget
    bool show_feature_summary_ = false; ///< Show the feature summary widget
    bool show_sequence_summary_ = false; ///< Show the sequence summary widget
    bool show_log_ = false;   ///< Show the log widget
    bool show_app_about_ = false;    ///< Show the about widget
  };
}