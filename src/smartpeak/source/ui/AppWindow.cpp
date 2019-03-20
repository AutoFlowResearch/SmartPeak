#include <SmartPeak/ui/AppWindow.h>
#include <SmartPeak/ui/Widget.h>

namespace SmartPeak
{
  void AppWindow::showApp() {
    // View: Explorer pane
    static bool show_sequence_explorer_ = false;
    static bool show_transitions_explorer_ = false;
    static bool show_experiment_explorer_ = false;
    static bool show_features_explorer_ = false;
    // View: Main pane
    static bool show_sequence_table_ = true;
    static bool show_transitions_table_ = false;
    static bool show_workflow_table_ = false;
    static bool show_parameters_table_ = false;
    static bool show_quant_method_table_ = false;
    static bool show_stds_concs_table_ = false;
    static bool show_comp_filters_table_ = false;
    static bool show_comp_group_filters_table_ = false;
    static bool show_comp_qcs_table_ = false;
    static bool show_comp_group_qcs_table_ = false;
    static bool show_feature_plot_ = false;
    static bool show_line_plot_ = false;
    static bool show_heatmap_plot_ = false;
    static bool show_feature_summary_table_ = false;
    static bool show_sequence_summary_table_ = false;
    // View: Info pane
    static bool show_output_ = true;
    static bool show_info_ = false;
    static bool show_log_ = false;
    // Help
    static bool show_app_about_ = false;

    // Show the main window
    ImGuiWindowFlags window_flags = 0;
    window_flags |= ImGuiWindowFlags_NoTitleBar;
    window_flags |= ImGuiWindowFlags_NoResize;
    window_flags |= ImGuiWindowFlags_NoCollapse;
    window_flags |= ImGuiWindowFlags_MenuBar;
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
    static bool show_main_window = true;
    if (ImGui::Begin("Sequence list", &show_main_window, window_flags))
    {
      // Main menu bar
      MainMenu main_menu;
      main_menu.showMainMenuBar(
        // View: Explorer pane
        show_sequence_explorer_,
        show_transitions_explorer_,
        show_experiment_explorer_,
        show_features_explorer_,
        // View: Main pane
        show_sequence_table_,
        show_transitions_table_,
        show_workflow_table_,
        show_parameters_table_,
        show_quant_method_table_,
        show_stds_concs_table_,
        show_comp_filters_table_,
        show_comp_group_filters_table_,
        show_comp_qcs_table_,
        show_comp_group_qcs_table_,
        show_feature_plot_,
        show_line_plot_,
        show_heatmap_plot_,
        show_feature_summary_table_,
        show_sequence_summary_table_,
        // View: Info pane
        show_output_,
        show_info_,
        show_log_,
        // Help
        show_app_about_);

      // determine what windows will be shown
      bool show_explorer_pane = false;
      bool show_main_pane = true; // always shown
      bool show_info_pane = false;
      if (show_sequence_explorer_ ||
        show_transitions_explorer_ ||
        show_experiment_explorer_ ||
        show_features_explorer_) show_explorer_pane = true;
      if (show_output_ ||
        show_info_ ||
        show_log_) show_info_pane = true;

      // calculate the window sizes
      ImVec2 explorer_pane_size, main_pane_size, info_pane_size;
      if (show_explorer_pane && show_main_pane && show_info_pane) {
        // All panes
        explorer_pane_size = ImVec2(ImGui::GetIO().DisplaySize.x*0.2, ImGui::GetIO().DisplaySize.y*0.75);
        main_pane_size = ImVec2(0, ImGui::GetIO().DisplaySize.y*0.75);
        info_pane_size = ImVec2(0, 0);
      } else if (show_explorer_pane && show_main_pane){
        // No info pane
        explorer_pane_size = ImVec2(ImGui::GetIO().DisplaySize.x*0.2, 0);
        main_pane_size = ImVec2(0, 0);
        info_pane_size = ImVec2(0, 0);
      } else if (show_main_pane) {
        // No explorer and info panes
        explorer_pane_size = ImVec2(0, 0);
        main_pane_size = ImVec2(0, 0);
        info_pane_size = ImVec2(0, 0);
      } else {
        explorer_pane_size = ImVec2(0, 0);
        main_pane_size = ImVec2(0, 0);
        info_pane_size = ImVec2(0, 0);
      }

      // left Top
      if (show_explorer_pane) {
        ImGui::BeginChild("Explorer pane", ImVec2(ImGui::GetIO().DisplaySize.x*0.2, ImGui::GetIO().DisplaySize.y*0.75), false, ImGuiWindowFlags_HorizontalScrollbar);
        ImGui::Text("TODO: search and sort bar");
        ImGui::Text("TODO: list or tree");
        ImGui::EndChild();
        ImGui::SameLine();
      }

      // right top 
      if (show_main_pane) {
        ImGui::BeginChild("Main pane", ImVec2(0, ImGui::GetIO().DisplaySize.y*0.75), false);
        MainWindow mainWindow;
        mainWindow.showMainWindow(show_sequence_table_,
          show_transitions_table_,
          show_workflow_table_,
          show_parameters_table_,
          show_quant_method_table_,
          show_stds_concs_table_,
          show_comp_filters_table_,
          show_comp_group_filters_table_,
          show_comp_qcs_table_,
          show_comp_group_qcs_table_,
          show_feature_plot_,
          show_line_plot_,
          show_heatmap_plot_,
          show_feature_summary_table_,
          show_sequence_summary_table_);
        ImGui::EndChild();
        ImGui::NewLine();
      }

      // Bottom
      if (show_info_pane) {
        ImGui::BeginChild("Info pane", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);
        InfoWindow infoWindow;
        infoWindow.showInfoWindow(show_output_, show_info_, show_log_);
        ImGui::EndChild();
      }
    }
    ImGui::End();
  }

  void MainMenu::showMainMenuBar(
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
    bool& show_app_about) {
    // Show the widgets
    //SequenceWidget sequenceWidget;
    //if (show_sequence_) sequenceWidget.show(&show_sequence_);
    //if (show_generic_table) TableWidget(&show_generic_table);
    //if (show_file_browser) FileBrowserWidget(&show_file_browser);
    //if (show_plot) PlotWidget(&show_plot);
    //if (show_workflow) WorkflowWidget(&show_workflow);

    // Show the main menu bar
    if (ImGui::BeginMenuBar())
    // if (ImGui::BeginMainMenuBar())
    {
      if (ImGui::BeginMenu("File"))
      {
        showMenuFile();
        ImGui::EndMenu();
      }
      if (ImGui::BeginMenu("Edit"))
      {
        showMenuEdit();
        ImGui::EndMenu();
      }
      if (ImGui::BeginMenu("View"))
      {
        showMenuView(
          // View: Explorer pane
          show_sequence_explorer,
          show_transitions_explorer,
          show_experiment_explorer,
          show_features_explorer,
          // View: Main pane
          show_sequence_table,
          show_transitions_table,
          show_workflow_table,
          show_parameters_table,
          show_quant_method_table,
          show_stds_concs_table,
          show_comp_filters_table,
          show_comp_group_filters_table,
          show_comp_qcs_table,
          show_comp_group_qcs_table,
          show_feature_plot,
          show_line_plot,
          show_heatmap_plot,
          show_feature_summary_table,
          show_sequence_summary_table,
          // View: Info pane
          show_output,
          show_info,
          show_log);
        ImGui::EndMenu();
      }
      if (ImGui::BeginMenu("Actions"))
      {
        showMenuAction();
        ImGui::EndMenu();
      }
      if (ImGui::BeginMenu("Help"))
      {
        showMenuHelp(show_app_about);
        ImGui::EndMenu();
      }
      ImGui::EndMenuBar();
      //ImGui::EndMainMenuBar();
    }
  }

  void MainMenu::showMenuFile() {
    ImGui::MenuItem("Session", NULL, false, false);
    if (ImGui::MenuItem("New Session"))
    {
      //TODO: SQL light interface
    }
    if (ImGui::MenuItem("Load Session", "Ctrl+O"))
    {
      //TODO: open file browser modal
    }
    if (ImGui::MenuItem("Save Session", "Ctrl+S"))
    {
      //TODO
    }
    if (ImGui::MenuItem("Save Session As..."))
    {
      //TODO: open save as File browser modal
    }
    ImGui::Separator();
    ImGui::MenuItem("Text file", NULL, false, false);
    if (ImGui::BeginMenu("Import File"))
    {
      if (ImGui::MenuItem("Sequence")) {}
      if (ImGui::MenuItem("Transitions")) {}
      if (ImGui::MenuItem("Parameters")) {}
      if (ImGui::MenuItem("Quant Method")) {}
      if (ImGui::MenuItem("Standards Conc")) {}
      if (ImGui::MenuItem("Comp Filters")) {}
      if (ImGui::MenuItem("Comp Group Filters")) {}
      if (ImGui::MenuItem("Comp QCs")) {}
      if (ImGui::MenuItem("Comp Group QCs")) {}
      ImGui::EndMenu();
    }
    if (ImGui::BeginMenu("Export File"))
    {
      if (ImGui::MenuItem("Sequence")) {}
      if (ImGui::MenuItem("Transitions")) {}
      if (ImGui::MenuItem("Parameters")) {}
      if (ImGui::MenuItem("Quant Method")) {}
      if (ImGui::MenuItem("Standards Conc")) {}
      if (ImGui::MenuItem("Comp Filters")) {}
      if (ImGui::MenuItem("Comp Group Filters")) {}
      if (ImGui::MenuItem("Comp QCs")) {}
      if (ImGui::MenuItem("Comp Group QCs")) {}
      ImGui::EndMenu();
    }
    ImGui::Separator();
    if (ImGui::MenuItem("Quit", "Alt+F4")) {}
  }

  void MainMenu::showMenuEdit() {
    if (ImGui::MenuItem("Undo", "CTRL+Z")) {}
    if (ImGui::MenuItem("Redo", "CTRL+Y", false, false)) {}  // Disabled item
    ImGui::Separator();
    if (ImGui::MenuItem("Cut", "CTRL+X")) {}
    if (ImGui::MenuItem("Copy", "CTRL+C")) {}
    if (ImGui::MenuItem("Paste", "CTRL+V")) {}
    ImGui::Separator();
    if (ImGui::MenuItem("Workflow")) {}
  }

  void MainMenu::showMenuView(
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
    bool& show_log) {
    ImGui::MenuItem("Explorer window", NULL, false, false);
    // Explorer sub windows
    if (ImGui::MenuItem("Sequence", NULL, &show_sequence_explorer)) {}
    if (ImGui::MenuItem("Transitions", NULL, &show_transitions_explorer)) {}
    if (ImGui::MenuItem("Experiment", NULL, &show_experiment_explorer)) {}
    if (ImGui::MenuItem("Features", NULL, &show_features_explorer)) {}  // including metadata?
    // Main pane tabs
    ImGui::Separator();  // Primary input
    ImGui::MenuItem("Main window", NULL, false, false);
    if (ImGui::MenuItem("Sequence", NULL, &show_sequence_table)) {}
    if (ImGui::MenuItem("Transitions", NULL, &show_transitions_table)) {}
    if (ImGui::MenuItem("Workflow", NULL, &show_workflow_table)) {}
    if (ImGui::BeginMenu("Workflow settings"))
    {
      if (ImGui::MenuItem("Parameters", NULL, &show_parameters_table)) {}
      if (ImGui::MenuItem("Quant Method", NULL, &show_quant_method_table)) {}
      if (ImGui::MenuItem("Standards Conc", NULL, &show_stds_concs_table)) {}
      if (ImGui::MenuItem("Comp Filters", NULL, &show_comp_filters_table)) {}
      if (ImGui::MenuItem("Comp Group Filters", NULL, &show_comp_group_filters_table)) {}
      if (ImGui::MenuItem("Comp QCs", NULL, &show_comp_qcs_table)) {}
      if (ImGui::MenuItem("Comp Group QCs", NULL, &show_comp_group_qcs_table)) {}
      ImGui::EndMenu();
    }
    if (ImGui::BeginMenu("Review plots"))
    {
      if (ImGui::MenuItem("Features", NULL, &show_feature_plot)) {}
      if (ImGui::MenuItem("Line plot", NULL, &show_line_plot)) {}
      if (ImGui::MenuItem("Heatmap", NULL, &show_heatmap_plot)) {}
      ImGui::EndMenu();
    }
    if (ImGui::BeginMenu("Reports"))
    {
      if (ImGui::MenuItem("Feature summary", NULL, &show_feature_summary_table)) {}
      if (ImGui::MenuItem("Sequence summary", NULL, &show_sequence_summary_table)) {}
      ImGui::EndMenu();
    }
    // Info pane tabs
    ImGui::MenuItem("Info window", NULL, false, false);
    if (ImGui::MenuItem("Output", NULL, &show_output)) {}
    if (ImGui::MenuItem("Info", NULL, &show_info)) {}
    if (ImGui::MenuItem("Log", NULL, &show_log)) {}
  }

  void MainMenu::showMenuAction() {
    if (ImGui::MenuItem("Run command")) {}
    if (ImGui::MenuItem("Run workflow")) {}
    if (ImGui::BeginMenu("Quick info"))
    { // TODO: bug
      if (ImGui::MenuItem("Sequence")) {}
      if (ImGui::MenuItem("Transitions")) {}
      if (ImGui::MenuItem("Quant Method")) {}
      if (ImGui::MenuItem("Standards Conc")) {}
      if (ImGui::MenuItem("Comp Filters")) {}
      if (ImGui::MenuItem("Comp Group Filters")) {}
      if (ImGui::MenuItem("Comp QCs")) {}
      if (ImGui::MenuItem("Comp Group QCs")) {}
      if (ImGui::MenuItem("Parameters")) {}
      if (ImGui::MenuItem("Raw data files")) {}
      if (ImGui::MenuItem("Analyzed features")) {}
      if (ImGui::MenuItem("Selected features")) {}
      if (ImGui::MenuItem("Picked peaks")) {}
      if (ImGui::MenuItem("Filtered/selected peaks")) {}
      ImGui::EndMenu();
    }
    if (ImGui::BeginMenu("Integrity checks"))
    {  // TODO: bug
      if (ImGui::MenuItem("Sample consistency")) {}
      if (ImGui::MenuItem("Comp consistency")) {}
      if (ImGui::MenuItem("Comp Group consistency")) {}
      if (ImGui::MenuItem("IS consistency")) {}
      ImGui::EndMenu();
    }
    if (ImGui::BeginMenu("Report"))
    {  // TODO: bug
      if (ImGui::MenuItem("Feature summary")) {}
      if (ImGui::MenuItem("Sequence summary")) {}
      ImGui::EndMenu();
    }
  }

  void MainMenu::showMenuHelp(bool& show_app_about) {
    if (ImGui::MenuItem("About")) {
      ImGui::OpenPopup("about");
      if (ImGui::BeginPopupModal("about"))
      {
        //AboutWidget aboutWidget;
        //bool show_about = true;
        //aboutWidget.show(&show_about);
        ImGui::Text("About SmartPeak");
        ImGui::Text("SmartPeak %s", 1.0); //TODO: define version function
        ImGui::Separator();
        ImGui::Text("By the hardworking SmartPeak developers.");
        ImGui::EndPopup();
      }
    };

    if (ImGui::MenuItem("Documentation")) {}
    if (ImGui::MenuItem("Version")) {}


  }

  void MainWindow::showMainWindow(
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
    bool& show_sequence_summary_table)
  {
    static ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_Reorderable;
    if ((show_sequence_table ||
      show_transitions_table ||
      show_workflow_table ||
      show_parameters_table ||
      show_quant_method_table ||
      show_stds_concs_table ||
      show_comp_filters_table ||
      show_comp_group_filters_table ||
      show_comp_qcs_table ||
      show_comp_group_qcs_table ||
      show_feature_plot ||
      show_line_plot ||
      show_heatmap_plot ||
      show_feature_summary_table ||
      show_sequence_summary_table) && ImGui::BeginTabBar("Main pane tab bar", tab_bar_flags))
    {
      if (show_sequence_table && ImGui::BeginTabItem("Sequence", &show_sequence_table))
      {
        ImGui::Text("TODO: sequence table");
        ImGui::EndTabItem();
      }
      if (show_transitions_table && ImGui::BeginTabItem("Transitions", &show_transitions_table))
      {
        ImGui::Text("TODO: transition table");
        ImGui::EndTabItem();
      }
      if (show_workflow_table && ImGui::BeginTabItem("Workflow", &show_workflow_table))
      {
        ImGui::Text("TODO: workflow table");
        ImGui::EndTabItem();
      }
      // TODO...
      ImGui::EndTabBar();
    }
  }

  void InfoWindow::showInfoWindow(bool & show_output, bool & show_info, bool & show_log)
  {
    static ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_Reorderable;
    if ((show_output ||
      show_info ||
      show_log) && ImGui::BeginTabBar("Info pane tab bar", tab_bar_flags))
    {
      if (show_output && ImGui::BeginTabItem("Output", &show_output))
      {
        ImGui::Text("TODO: output text");
        ImGui::EndTabItem();
      }
      if (show_info && ImGui::BeginTabItem("Info", &show_info))
      {
        ImGui::Text("TODO: info text");
        ImGui::EndTabItem();
      }
      if (show_log && ImGui::BeginTabItem("Log", &show_log))
      {
        ImGui::Text("TODO: log text");
        ImGui::EndTabItem();
      }
      // TODO...
      ImGui::EndTabBar();
    }
  }

  void ExplorerWindow::showExplorerWindow(bool & show_injections_search, bool & show_samples_search, bool & show_sequence_segments_search, bool & show_sample_groups_search, bool & show_components_search, bool & show_component_groups_search, bool & show_workflow_steps_search, bool & show_sequence_explorer, bool & show_transitions_explorer, bool & show_experiment_explorer, bool & show_features_explorer)
  {
  }
}