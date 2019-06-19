#include <SmartPeak/ui/AppWindow.h>
#include <SmartPeak/ui/Widget.h>
#include <imgui.h>

namespace SmartPeak
{
  void AppWindow::showApp() {
    // View: Search pane
    static bool show_injections_search = false;
    static bool show_samples_search = false;
    static bool show_sequence_segments_search = false;
    static bool show_sample_groups_search = false;
    static bool show_components_search = false;
    static bool show_component_groups_search = false;
    static bool show_workflow_steps_search = false;
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

    static bool show_file_picker_ = false;

    if (show_file_picker_)
    {
      ImGui::OpenPopup("modal_file_picker");
    }

    // File picker modal
    if (ImGui::BeginPopupModal("modal_file_picker", NULL, ImGuiWindowFlags_AlwaysAutoResize))
    {
      {
        // ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 1.5f);

        // ImGui::BeginChild("Folders", ImVec2(ImGui::GetWindowContentRegionWidth() * 0.5f, 130));
        ImGui::BeginChild("Folders", ImVec2(170, 300));

        for (int n = 0; n < 50; ++n)
          ImGui::Text("Folder n. %d", n);
        ImGui::EndChild();
      }

      ImGui::SameLine();

      {
        ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 1.5f);
        // ImGui::BeginChild("Folder's content", ImVec2(ImVec2(ImGui::GetWindowContentRegionWidth() * 0.5f, 210)));
        ImGui::BeginChild("Folder's content", ImVec2(ImVec2(600, 300)));
        ImGui::Text("Content of the selected (not yet) folder");
        ImGui::EndChild();
        ImGui::PopStyleVar();
      }

      if (ImGui::Button("Cancel", ImVec2(120, 0))) {
        show_file_picker_ = false;
        ImGui::CloseCurrentPopup();
      }

      ImGui::SameLine();
      if (ImGui::Button("Open", ImVec2(120, 0))) {
        show_file_picker_ = false;
        ImGui::CloseCurrentPopup();
      }

      ImGui::EndPopup();
    }

    // Show the main window
    ImGuiWindowFlags window_flags = 0;
    window_flags |= ImGuiWindowFlags_NoTitleBar;
    // TODO: eventually uncomment following lines
    // window_flags |= ImGuiWindowFlags_NoResize;
    // window_flags |= ImGuiWindowFlags_NoCollapse;
    window_flags |= ImGuiWindowFlags_MenuBar;
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
    static bool show_main_window = true;
    if (!ImGui::Begin("Sequence list", &show_main_window, window_flags))
    {
      // Early out if the window is collapsed, as an optimization.
      ImGui::End();
      return;
    }
    // Main menu bar
    showMainMenuBar(
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
      show_app_about_,
      show_file_picker_
    );

    // determine what windows will be shown
    bool show_explorer_pane = false;
    bool show_main_pane = true; // always shown
    bool show_info_pane = false;
    if (show_sequence_explorer_ ||
        show_transitions_explorer_ ||
        show_experiment_explorer_ ||
        show_features_explorer_)
    {
      show_explorer_pane = true;
    }

    if (show_output_ || show_info_ || show_log_)
    {
      show_info_pane = true;
    }

    // // calculate the window sizes
    // ImVec2 explorer_pane_size, main_pane_size, info_pane_size;
    // if (show_explorer_pane && show_main_pane && show_info_pane) {
    //   // All panes
    //   explorer_pane_size = ImVec2(ImGui::GetIO().DisplaySize.x*0.2, ImGui::GetIO().DisplaySize.y*0.75);
    //   main_pane_size = ImVec2(0, ImGui::GetIO().DisplaySize.y*0.75);
    //   info_pane_size = ImVec2(0, 0);
    // } else if (show_explorer_pane && show_main_pane){
    //   // No info pane
    //   explorer_pane_size = ImVec2(ImGui::GetIO().DisplaySize.x*0.2, 0);
    //   main_pane_size = ImVec2(0, 0);
    //   info_pane_size = ImVec2(0, 0);
    // } else if (show_main_pane) {
    //   // No explorer and info panes
    //   explorer_pane_size = ImVec2(0, 0);
    //   main_pane_size = ImVec2(0, 0);
    //   info_pane_size = ImVec2(0, 0);
    // } else {
    //   explorer_pane_size = ImVec2(0, 0);
    //   main_pane_size = ImVec2(0, 0);
    //   info_pane_size = ImVec2(0, 0);
    // }

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
      showMainWindow(show_sequence_table_,
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
        show_sequence_summary_table_
      );
      ImGui::EndChild();
      ImGui::NewLine();
    }

    // Bottom
    if (show_info_pane) {
      ImGui::BeginChild("Info pane", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);
      showInfoWindow(show_output_, show_info_, show_log_);
      ImGui::EndChild();
    }
    ImGui::End();
  }

  void AppWindow::showMainMenuBar(
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
    bool& show_app_about,
    bool& show_file_picker
  )
  {
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
        showMenuFile(show_file_picker);
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

  void AppWindow::showMenuFile(bool& show_file_picker)
  {
    ImGui::MenuItem("Session", NULL, false, false);
    if (ImGui::MenuItem("New Session"))
    {
      //TODO: SQL light interface
    }

    if (ImGui::MenuItem("Load Session", "Ctrl+O"))
    {
      //TODO: open file browser modal
      // ImGui::OpenPopup("Delete?");
    }

    if (ImGui::MenuItem("Load session from sequence")) {
      show_file_picker = !show_file_picker;
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
    // TODO
    //ImGui::Separator();
    //if (ImGui::MenuItem("Quit", "Alt+F4")) {}
  }

  void AppWindow::showMenuEdit() {
    ImGui::MenuItem("Session", NULL, false, false);
    if (ImGui::MenuItem("Undo", "CTRL+Z")) {}
    if (ImGui::MenuItem("Redo", "CTRL+Y", false, false)) {}  // Disabled item
    ImGui::Separator();
    ImGui::MenuItem("Settings", NULL, false, false);
    if (ImGui::MenuItem("Tables")) {} // TODO: modal of settings 
    if (ImGui::MenuItem("Plots")) {} // TODO: modal of settings 
    if (ImGui::MenuItem("Explorer")) {} // TODO: modal of settings 
    if (ImGui::MenuItem("Search")) {} // TODO: modal of settings 
    if (ImGui::MenuItem("Workflow")) {} // TODO: modal of settings 
  }

  void AppWindow::showMenuView(
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
    if (ImGui::MenuItem("Features", NULL, &show_feature_plot)) {}
    if (ImGui::MenuItem("Metric plot", NULL, &show_line_plot)) {}
    if (ImGui::MenuItem("Heatmap", NULL, &show_heatmap_plot)) {}
    if (ImGui::MenuItem("Features table", NULL, &show_feature_summary_table)) {}
    if (ImGui::MenuItem("Features pivot table", NULL, &show_sequence_summary_table)) {}
    // Info pane tabs
    ImGui::MenuItem("Info window", NULL, false, false);
    if (ImGui::MenuItem("Output", NULL, &show_output)) {}
    if (ImGui::MenuItem("Info", NULL, &show_info)) {}
    if (ImGui::MenuItem("Log", NULL, &show_log)) {}
  }

  void AppWindow::showMenuAction() {
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

  void AppWindow::showMenuHelp(bool& show_app_about) {
    if (ImGui::MenuItem("About")) {
      ImGui::OpenPopup("about");
      if (ImGui::BeginPopupModal("about"))
      {
        //AboutWidget aboutWidget;
        //bool show_about = true;
        //aboutWidget.show(&show_about);
        ImGui::Text("About SmartPeak");
        ImGui::Text("SmartPeak %s", "1.0"); //TODO: define version function
        ImGui::Separator();
        ImGui::Text("By the hardworking SmartPeak developers.");
        ImGui::EndPopup();
      }
    };

    if (ImGui::MenuItem("Documentation")) {}
    if (ImGui::MenuItem("Version")) {}


  }

  void AppWindow::showMainWindow(
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

        // Dummy data for testing
        std::vector<std::string> headers = { "sample_name", "sample_type", "component_name" }; // feature or sample columns to filter on
        std::vector<std::string> sample_name_col = { "S1", "S1", "S2", "S2", "S3", "S3", "S4", "S4" };
        std::vector<std::string> sample_type_col = { "A", "A", "A", "A", "B", "B", "B" ,"B" };
        std::vector<std::string> component_name_col = { "C1", "C2", "C1", "C2", "C1", "C2", "C1", "C2" };
        std::vector<std::vector<std::string>> columns = { sample_name_col, sample_type_col, component_name_col };
        static bool rows_checked[] = { true, true, true, true, true, true, true, true };

        GenericTableWidget sequenceTable;
        // TODO: get the headers, columns, and rows_checked
        // NOTE: rows_checked must be statically declared before calling the GUI!
        sequenceTable.show(headers, columns, rows_checked);
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
      if (show_parameters_table && ImGui::BeginTabItem("Parameters", &show_parameters_table))
      {
        ImGui::Text("TODO: parameters table");
        ImGui::EndTabItem();
      }
      if (show_quant_method_table && ImGui::BeginTabItem("Quant Methods", &show_quant_method_table))
      {
        ImGui::Text("TODO: Quant methods table");
        ImGui::EndTabItem();
      }
      if (show_stds_concs_table && ImGui::BeginTabItem("Stds Concs", &show_stds_concs_table))
      {
        ImGui::Text("TODO: Standards concentrations table");
        ImGui::EndTabItem();
      }
      if (show_comp_filters_table && ImGui::BeginTabItem("Comp Filters", &show_comp_filters_table))
      {
        ImGui::Text("TODO: Component Filters table");
        ImGui::EndTabItem();
      }
      if (show_comp_group_filters_table && ImGui::BeginTabItem("Comp Group Filters", &show_comp_group_filters_table))
      {
        ImGui::Text("TODO: Component group filters table");
        ImGui::EndTabItem();
      }
      if (show_comp_qcs_table && ImGui::BeginTabItem("Comp QCs", &show_comp_qcs_table))
      {
        ImGui::Text("TODO: Component QCs table");
        ImGui::EndTabItem();
      }
      if (show_comp_group_qcs_table && ImGui::BeginTabItem("Comp Group QCs", &show_comp_group_qcs_table))
      {
        ImGui::Text("TODO: Component group qcs table");
        ImGui::EndTabItem();
      }
      if (show_feature_plot && ImGui::BeginTabItem("Feature plot", &show_feature_plot))
      {
        ImGui::Text("TODO: Feature plot");
        GenericGraphicWidget featurePlot;
        featurePlot.show();
        ImGui::EndTabItem();
      }
      if (show_line_plot && ImGui::BeginTabItem("Line plot", &show_line_plot))
      {
        ImGui::Text("TODO: Line plot");
        ImGui::EndTabItem();
      }
      if (show_heatmap_plot && ImGui::BeginTabItem("Heatmap", &show_heatmap_plot))
      {
        ImGui::Text("TODO: Heatmap");
        ImGui::EndTabItem();
      }
      if (show_feature_summary_table && ImGui::BeginTabItem("FeatureSummary", &show_feature_summary_table))
      {
        ImGui::Text("TODO: FeatureSummary table");
        ImGui::EndTabItem();
      }
      if (show_sequence_summary_table && ImGui::BeginTabItem("SequenceSummary", &show_sequence_summary_table))
      {
        ImGui::Text("TODO: SequenceSummary table");
        ImGui::EndTabItem();
      }
      ImGui::EndTabBar();
    }
  }

  void AppWindow::showInfoWindow(
    bool & show_output,
    bool & show_info,
    bool & show_log
  )
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

  void AppWindow::showExplorerWindow(
    bool & show_injections_search,
    bool & show_samples_search,
    bool & show_sequence_segments_search,
    bool & show_sample_groups_search,
    bool & show_components_search,
    bool & show_component_groups_search,
    bool & show_workflow_steps_search,
    bool & show_sequence_explorer,
    bool & show_transitions_explorer,
    bool & show_experiment_explorer,
    bool & show_features_explorer
  )
  {
  }
}