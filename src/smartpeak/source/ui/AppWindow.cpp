#include <SmartPeak/ui/AppWindow.h>
#include <string>
#include <vector>
#include <imgui.h>
#include <algorithm>
#include <SmartPeak/ui/FilePicker.h>

namespace SmartPeak
{
  void AppWindow::showApp()
  {
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
    // Help
    static bool show_app_about_ = false;

    if (file_picker_.show_file_picker_)
    {
      file_picker_.draw();
    }

    if (report_.draw_)
    {
      report_.draw();
    }

    if (workflow_.draw_)
    {
      workflow_.draw();
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
      // Help
      show_app_about_
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

    if (show_info_ || show_log_)
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
      ImGui::BeginChild("Explorer pane", ImVec2(ImGui::GetIO().DisplaySize.x * 0.2f, ImGui::GetIO().DisplaySize.y * 0.75f), false, ImGuiWindowFlags_HorizontalScrollbar);
      ImGui::Text("TODO: search and sort bar");
      ImGui::Text("TODO: list or tree");
      ImGui::EndChild();
      ImGui::SameLine();
    }

    // right top 
    if (show_main_pane) {
      ImGui::BeginChild("Main pane", ImVec2(0, ImGui::GetIO().DisplaySize.y * 0.4f), false);
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
      showInfoWindow();
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
    // Help
    bool& show_app_about
  )
  {
    // Show the widgets
    //SequenceWidget sequenceWidget;
    //if (show_sequence_) sequenceWidget.draw(&show_sequence_);
    //if (show_generic_table) TableWidget(&show_generic_table);
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
          show_sequence_summary_table);
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

  void AppWindow::showMenuFile()
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
      static LoadSessionFromSequence processor(state_);
      file_picker_.setProcessor(processor);
      file_picker_.show_file_picker_ = true;
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
    if (ImGui::MenuItem("Workflow"))
    {
      initializeDataDirs(state_);
      workflow_.draw_ = true;
    }
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
    bool& show_sequence_summary_table
  )
  {
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
    if (ImGui::MenuItem("Info", NULL, &show_info_)) {}
    if (ImGui::MenuItem("Log", NULL, &show_log_)) {}
  }

  void AppWindow::showMenuAction() {
    if (ImGui::MenuItem("Run command"))
    {
      initializeDataDirs(state_);
      // do the rest
    }
    if (ImGui::MenuItem("Run workflow"))
    {
      if (state_.commands_.size()) // ensures that workflow's steps have been set
      {
        initializeDataDirs(state_);
        manager_.addWorkflow(state_);
      }
      else
      {
        LOGE << "Workflow cannot start. Please set the workflow's steps.";
      }
    }
    if (ImGui::BeginMenu("Quick info"))
    { // TODO: bug
      if (ImGui::MenuItem("Sequence")) {
        quickInfoText_ = InputDataValidation::getSequenceInfo(state_.sequenceHandler_);
      }
      if (ImGui::MenuItem("Transitions")) {
        quickInfoText_.clear();
        if (state_.sequenceHandler_.getSequence().size()) {
          quickInfoText_ = InputDataValidation::getTraMLInfo(state_.sequenceHandler_.getSequence().front().getRawData());
        }
      }
      if (ImGui::MenuItem("Quant Method")) {
        quickInfoText_.clear();
        if (state_.sequenceHandler_.getSequenceSegments().size()) {
          quickInfoText_ = InputDataValidation::getQuantitationMethodsInfo(state_.sequenceHandler_.getSequenceSegments().front());
        }
      }
      if (ImGui::MenuItem("Standards Conc")) {
        quickInfoText_.clear();
        if (state_.sequenceHandler_.getSequenceSegments().size()) {
          quickInfoText_ = InputDataValidation::getStandardsConcentrationsInfo(state_.sequenceHandler_.getSequenceSegments().front());
        }
      }
      if (ImGui::MenuItem("Comp (Group) Filters")) {
        quickInfoText_.clear();
        if (state_.sequenceHandler_.getSequence().size()) {
          quickInfoText_ = InputDataValidation::getComponentsAndGroupsInfo(state_.sequenceHandler_.getSequence().front().getRawData(), true);
        }
      }
      if (ImGui::MenuItem("Comp (Group) QCs")) {
        quickInfoText_.clear();
        if (state_.sequenceHandler_.getSequence().size()) {
          quickInfoText_ = InputDataValidation::getComponentsAndGroupsInfo(state_.sequenceHandler_.getSequence().front().getRawData(), false);
        }
      }
      if (ImGui::MenuItem("Parameters")) {
        quickInfoText_.clear();
        if (state_.sequenceHandler_.getSequence().size()) {
          quickInfoText_ = InputDataValidation::getParametersInfo(state_.sequenceHandler_.getSequence().front().getRawData().getParameters());
        }
      }
      if (ImGui::MenuItem("Raw data files")) {
        quickInfoText_ = state_.sequenceHandler_.getRawDataFilesInfo();
      }
      if (ImGui::MenuItem("Analyzed features")) {
        quickInfoText_ = state_.sequenceHandler_.getAnalyzedFeaturesInfo();
      }
      if (ImGui::MenuItem("Selected features")) {
        quickInfoText_ = state_.sequenceHandler_.getSelectedFeaturesInfo();
      }
      if (ImGui::MenuItem("Picked peaks")) {
        quickInfoText_ = state_.sequenceHandler_.getPickedPeaksInfo();
      }
      if (ImGui::MenuItem("Filtered/selected peaks")) {
        quickInfoText_ = state_.sequenceHandler_.getFilteredSelectedPeaksInfo();
      }
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
    if (ImGui::MenuItem("Report"))
    {
      report_.draw_ = true;
    }
  }

  void AppWindow::showMenuHelp(bool& show_app_about) {
    if (ImGui::MenuItem("About")) {
      ImGui::OpenPopup("about");
      if (ImGui::BeginPopupModal("about"))
      {
        //AboutWidget aboutWidget;
        //bool show_about = true;
        //aboutWidget.draw(&show_about);
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
        // TODO: following lines of code keep copying data. it is slow.
        sequenceTable.headers = headers;
        sequenceTable.columns = columns;
        sequenceTable.checked_rows = rows_checked;
        sequenceTable.draw();
        ImGui::EndTabItem();
      }
      if (show_transitions_table && ImGui::BeginTabItem("Transitions", &show_transitions_table))
      {
        ImGui::Text("TODO: transition table");
        ImGui::EndTabItem();
      }
      if (show_workflow_table && ImGui::BeginTabItem("Workflow", &show_workflow_table))
      {
        const std::vector<char>& statuses = manager_.getWorkflowsStatus();
        const size_t statuses_size = statuses.size(); // So one does not depend on compiler optimizations flags
        for (int i = 0; static_cast<size_t>(i) < statuses_size; ++i)
        {
          ImGui::Text("Workflow %d: %s", i, statuses.at(i) ? "done" : "running...");
        }
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
        featurePlot.draw();
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
      if (show_feature_summary_table && ImGui::BeginTabItem("FeatureDB", &show_feature_summary_table))
      {
        ImGui::Text("TODO: FeatureDB table");
        ImGui::EndTabItem();
      }
      if (show_sequence_summary_table && ImGui::BeginTabItem("PivotTable", &show_sequence_summary_table))
      {
        ImGui::Text("TODO: PivotTable table");
        ImGui::EndTabItem();
      }
      ImGui::EndTabBar();
    }
  }

  void AppWindow::showInfoWindow()
  {
    static ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_Reorderable;
    if ((show_info_ || show_log_) &&
        ImGui::BeginTabBar("Info pane tab bar", tab_bar_flags))
    {
      if (show_info_ && ImGui::BeginTabItem("Info", &show_info_))
      {
        ImGui::BeginChild("Info child");
        ImGui::TextWrapped("%s", quickInfoText_.c_str());
        ImGui::EndChild();
        ImGui::EndTabItem();
      }
      if (show_log_ && ImGui::BeginTabItem("Log", &show_log_))
      {
        const char* items[] = { "NONE", "FATAL", "ERROR", "WARN", "INFO", "DEBUG", "VERB" }; // reflects the strings in plog's Severity.h
        static int selected_severity = 5;
        static plog::Severity severity = plog::Severity::debug;

        if (ImGui::Combo("Level", &selected_severity, items, IM_ARRAYSIZE(items)))
        {
          severity = plog::severityFromString(items[selected_severity]);
        }

        ImGui::Separator();
        ImGui::BeginChild("Log child");
        for (const plog::util::nstring& s : appender_.getMessageList(severity))
        {
          ImGui::Text("%s", s.c_str());
        }
        ImGui::EndChild();
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

  // copied from imgui_demo.cpp
  // Helper to display a little (?) mark which shows a tooltip when hovered.
  void AppWindow::HelpMarker(const char* desc)
  {
    ImGui::TextDisabled("(?)");
    if (ImGui::IsItemHovered())
    {
      ImGui::BeginTooltip();
      ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
      ImGui::TextUnformatted(desc);
      ImGui::PopTextWrapPos();
      ImGui::EndTooltip();
    }
  }

  void AppWindow::initializeDataDirs(AppState& state)
  {
    initializeDataDir(state, "mzML", state.mzML_dir_, "mzML");
    initializeDataDir(state, "INPUT features", state.features_in_dir_, "features");
    initializeDataDir(state, "OUTPUT features", state.features_out_dir_, "features");
  }

  void AppWindow::initializeDataDir(
    AppState& state,
    const std::string& label,
    std::string& data_dir_member,
    const std::string& default_dir
  )
  {
    if (data_dir_member.size()) {
      return;
    }
    data_dir_member = state.main_dir_ + "/" + default_dir;
    LOGN << "\n\nGenerated path for '" << label << "':\t" << data_dir_member;
  }
}
