#pragma once

#include <SmartPeak/ui/AppManager.h>
#include <SmartPeak/ui/FilePicker.h>
#include <string>
#include <vector>

namespace SmartPeak
{
  class AppWindow
  {
    FilePicker         file_picker_;
    FilePickerObserver fpo_;
  public:
    AppManager         am_;

    AppWindow() {
      fpo_.am_ = &am_;
    }

    /**
      Show the application screen
    */
    void showApp();

    void showMainMenuBar(
      // View: Explorer pane
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
      bool& show_app_about
    );

    void showMenuFile(); ///< Show the main menu File options

    static void showMenuEdit(); ///< Show the main menu Edit options

    static void showMenuView(
      // View: Explorer pane
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
      bool& show_log
    ); ///< Show the main menu View options

    static void showMenuAction(); ///< Show the main menu Action options

    static void showMenuHelp(bool& show_app_about); ///< Show the main menu Help options

    static void showExplorerWindow(
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
      bool& show_features_explorer
    );

    static void showMainWindow(
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
      bool& show_sequence_summary_table
    );

    static void showInfoWindow(
      bool& show_output,
      bool& show_info,
      bool& show_log
    );

    static void HelpMarker(const char* desc);
  };
}