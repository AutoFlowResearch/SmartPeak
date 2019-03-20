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
    void showApp();

  private:
    // View: Explorer pane
    bool show_injections_search_ = false;  ///< Show the sample groups widget
    bool show_samples_search_ = false;  ///< Show the sample groups widget
    bool show_sequence_segments_search_ = false;  ///< Show the sequence segment widget
    bool show_sample_groups_search_ = false;  ///< Show the sample groups widget
    bool show_components_search_ = false;  ///< Show the sample groups widget
    bool show_component_groups_search_ = false;  ///< Show the sample groups widget
    bool show_workflow_steps_search_ = false;  ///< Show the sample groups widget
    bool show_sequence_explorer_ = false;  ///< Show the sequence widget
    bool show_transitions_explorer_ = false;  ///< Show the sequence widget
    bool show_experiment_explorer_ = false;  ///< Show the sequence widget
    bool show_features_explorer_ = false;  ///< Show the sequence widget

    // View: Main pane
    bool show_sequence_table_ = true;  ///< Show the sequence widget
    bool show_transitions_table_ = false;  ///< Show the sequence widget
    bool show_workflow_table_ = false;  ///< Show the workflow wizard widget
    bool show_parameters_table_ = false;  ///< Show the workflow wizard widget
    bool show_quant_method_table_ = false;  ///< Show the workflow wizard widget
    bool show_stds_concs_table_ = false;  ///< Show the workflow wizard widget
    bool show_comp_filters_table_ = false;  ///< Show the workflow wizard widget
    bool show_comp_group_filters_table_ = false;  ///< Show the workflow wizard widget
    bool show_comp_qcs_table_ = false;  ///< Show the workflow wizard widget
    bool show_comp_group_qcs_table_ = false;  ///< Show the workflow wizard widget
    bool show_feature_plot_ = false;  ///< Show the feature plot widget
    bool show_line_plot_ = false;  ///< Show the metric plot widget
    bool show_heatmap_plot_ = false;  ///< Show the metric plot widget
    bool show_feature_summary_table_ = false; ///< Show the feature summary widget
    bool show_sequence_summary_table_ = false; ///< Show the sequence summary widget

    // View: Info pane
    bool show_output_ = false;   ///< Show the log widget
    bool show_info_ = false;   ///< Show the log widget
    bool show_log_ = false;   ///< Show the log widget

    // Help
    bool show_app_about_ = false;    ///< Show the about widget
  };

  class MainMenu
  {
  public:
    MainMenu() = default;
    ~MainMenu() = default;

    void showMainMenuBar(
      // View: Explorer pane
      bool& show_injections_search,
      bool& show_samples_search,
      bool& show_sequence_segments_search,
      bool& show_sample_groups_search,
      bool& show_components_search,
      bool& show_component_groups_search,
      bool& show_workflow_steps_search,
      bool& show_sequence_explorer,
      bool& show_transitions_explorer,
      bool& show_experiment_explorer,
      bool& show_features_explorer,
      // View: Main pane
      bool& show_sequence_table,
      bool& show_transitions_table,
      bool& show_workflow_table,
      bool& show_parameters_table,
      bool& show_quant_method_table,
      bool& show_stds_concs_table,
      bool& show_comp_filters_table,
      bool& show_comp_group_filters_table,
      bool& show_comp_qcs_table,
      bool& show_comp_group_qcs_table,
      bool& show_feature_plot,
      bool& show_line_plot,
      bool& show_heatmap_plot,
      bool& show_feature_summary_table,
      bool& show_sequence_summary_table,
      // View: Info pane
      bool& show_output,
      bool& show_info,
      bool& show_log,
      // Help
      bool& show_app_about); ///< Show the main menu

    void showMenuFile(); ///< Show the main menu File options

    void showMenuEdit(); ///< Show the main menu Edit options

    void showMenuView(
      // View: Explorer pane
      bool& show_injections_search,
      bool& show_samples_search,
      bool& show_sequence_segments_search,
      bool& show_sample_groups_search,
      bool& show_components_search,
      bool& show_component_groups_search,
      bool& show_workflow_steps_search,
      bool& show_sequence_explorer,
      bool& show_transitions_explorer,
      bool& show_experiment_explorer,
      bool& show_features_explorer,
      // View: Main pane
      bool& show_sequence_table,
      bool& show_transitions_table,
      bool& show_workflow_table,
      bool& show_parameters_table,
      bool& show_quant_method_table,
      bool& show_stds_concs_table,
      bool& show_comp_filters_table,
      bool& show_comp_group_filters_table,
      bool& show_comp_qcs_table,
      bool& show_comp_group_qcs_table,
      bool& show_feature_plot,
      bool& show_line_plot,
      bool& show_heatmap_plot,
      bool& show_feature_summary_table,
      bool& show_sequence_summary_table,
      // View: Info pane
      bool& show_output,
      bool& show_info,
      bool& show_log); ///< Show the main menu View options

    void showMenuAction(); ///< Show the main menu Action options

    void showMenuHelp(bool& show_app_about); ///< Show the main menu Help options
  };

  class MainWindow
  {
  public:
    MainWindow() = default;
    ~MainWindow() = default;

    void showMainWindow(
      bool& show_sequence_table,
      bool& show_transitions_table,
      bool& show_workflow_table,
      bool& show_parameters_table,
      bool& show_quant_method_table,
      bool& show_stds_concs_table,
      bool& show_comp_filters_table,
      bool& show_comp_group_filters_table,
      bool& show_comp_qcs_table,
      bool& show_comp_group_qcs_table,
      bool& show_feature_plot,
      bool& show_line_plot,
      bool& show_heatmap_plot,
      bool& show_feature_summary_table,
      bool& show_sequence_summary_table);
  };
}