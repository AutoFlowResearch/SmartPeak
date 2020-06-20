// TODO: Add copyright

#include <stdio.h>
#include <chrono>
#include <string>
#include <vector>
#include <SmartPeak/core/ApplicationHandler.h>
#include <SmartPeak/core/ApplicationProcessor.h>
#include <SmartPeak/core/WorkflowManager.h>
#include <OpenMS/ANALYSIS/QUANTITATION/AbsoluteQuantitation.h>
#include <SmartPeak/io/SequenceParser.h>
#include <SmartPeak/ui/FilePicker.h>
#include <SmartPeak/ui/GuiAppender.h>
#include <SmartPeak/ui/Report.h>
#include <SmartPeak/ui/Workflow.h>
#include <SmartPeak/ui/WindowSizesAndPositions.h>
#include <plog/Log.h>
#include <plog/Appenders/ConsoleAppender.h>
#include <boost/filesystem.hpp>
#include <SDL.h>
#include <SDL_opengl.h>
#include <imgui.h>
#include <examples/imgui_impl_sdl.h>
#include <examples/imgui_impl_opengl2.h>
#include <misc/cpp/imgui_stdlib.h>

using namespace SmartPeak;
namespace fs = boost::filesystem;

void HelpMarker(const char* desc);

void initializeDataDirs(ApplicationHandler& state);

void initializeDataDir(
  ApplicationHandler& state,
  const std::string& label,
  std::string& data_dir_member,
  const std::string& default_dir
);

int main(int argc, char **argv)
  // `int argc, char **argv` are required on Win to link against the proper SDL2/OpenGL implementation
{
  // data for the injection explorer
  static Eigen::Tensor<std::string, 1> injection_explorer_headers;
  static Eigen::Tensor<std::string, 1> injection_explorer_checkbox_headers;
  static Eigen::Tensor<std::string, 2> injection_explorer_body;
  static Eigen::Tensor<bool, 2> injection_explorer_checkbox_body;
  static Eigen::Tensor<bool, 1> injection_explorer_checked_rows;
  // data for the transition explorer
  static Eigen::Tensor<std::string, 1> transition_explorer_headers;
  static Eigen::Tensor<std::string, 1> transition_explorer_checkbox_headers;
  static Eigen::Tensor<std::string, 2> transition_explorer_body;
  static Eigen::Tensor<bool, 2> transition_explorer_checkbox_body;
  Eigen::Tensor<bool, 1> transition_explorer_checked_rows;
  // data for the feature explorer
  static Eigen::Tensor<std::string, 1> feature_explorer_headers;
  static Eigen::Tensor<std::string, 1> feature_explorer_checkbox_headers;
  static Eigen::Tensor<std::string, 2> feature_explorer_body;
  static Eigen::Tensor<bool, 2> feature_explorer_checkbox_body;
  static Eigen::Tensor<bool, 1> feature_explorer_checked_rows;
  // data for the sequence table
  static Eigen::Tensor<std::string, 1> sequence_table_headers;
  static Eigen::Tensor<std::string, 2> sequence_table_body;
  // data for the transitions table
  static Eigen::Tensor<std::string, 1> transitions_table_headers;
  static Eigen::Tensor<std::string, 2> transitions_table_body;
  // data for the workflow table
  static Eigen::Tensor<std::string, 1> workflow_table_headers;
  static Eigen::Tensor<std::string, 2> workflow_table_body;
  // data for the parameters table
  static Eigen::Tensor<std::string, 1> parameters_table_headers;
  static Eigen::Tensor<std::string, 2> parameters_table_body;
  // data for the quant_method table
  static Eigen::Tensor<std::string, 1> quant_method_table_headers;
  static Eigen::Tensor<std::string, 2> quant_method_table_body;
  // data for the stds_concs table
  static Eigen::Tensor<std::string, 1> stds_concs_table_headers;
  static Eigen::Tensor<std::string, 2> stds_concs_table_body;
  // data for the comp_filters table
  static Eigen::Tensor<std::string, 1> comp_filters_table_headers;
  static Eigen::Tensor<std::string, 2> comp_filters_table_body;
  // data for the comp_group_filters table
  static Eigen::Tensor<std::string, 1> comp_group_filters_table_headers;
  static Eigen::Tensor<std::string, 2> comp_group_filters_table_body;
  // data for the comp_qcs table
  static Eigen::Tensor<std::string, 1> comp_qcs_table_headers;
  static Eigen::Tensor<std::string, 2> comp_qcs_table_body;
  // data for the comp_group_qcs table
  static Eigen::Tensor<std::string, 1> comp_group_qcs_table_headers;
  static Eigen::Tensor<std::string, 2> comp_group_qcs_table_body;
  // data for the feature table
  static Eigen::Tensor<std::string, 1> feature_table_headers;
  static Eigen::Tensor<std::string, 2> feature_table_body;
  // data for the feature_pivot table
  static Eigen::Tensor<std::string, 1> feature_pivot_table_headers;
  static Eigen::Tensor<std::string, 2> feature_pivot_table_rows;
  static Eigen::Tensor<std::string, 2> feature_pivot_table_body;
  // data for the chromatogram scatter plot
  static std::vector<std::vector<float>> chrom_time_data, chrom_intensity_data;
  static std::vector<std::string> chrom_series_names;
  static std::string chrom_x_axis_title = "Time (sec)";
  static std::string chrom_y_axis_title = "Intensity (au)";
  static float chrom_time_min = 1e6, chrom_time_max = 0, chrom_intensity_min = 1e6, chrom_intensity_max = 0;
  // data for the feature line plot
  static Eigen::Tensor<float, 2> feat_sample_data, feat_value_data;
  static Eigen::Tensor<std::string, 1> feat_line_series_names;
  static std::string feat_line_x_axis_title = "Inj#";
  static std::string feat_line_y_axis_title = "metadata (au)";
  static float feat_line_sample_min = 1e6, feat_line_sample_max = 0, feat_value_min = 1e6, feat_value_max = 0;
  // data for the feature heatmap (rows/colum labels are derived from feature pivot table)
  static Eigen::Tensor<float, 2, Eigen::RowMajor> feat_heatmap_data; // same as feat_value_data but rowMajor
  static Eigen::Tensor<std::string, 1> feat_heatmap_row_labels, feat_heatmap_col_labels;
  static std::string feat_heatmap_x_axis_title = "Sample name";
  static std::string feat_heatmap_y_axis_title = "Component name";
  // data for the calibrators scatter/line plot
  static std::vector<std::vector<float>> calibrators_conc_raw_data, calibrators_feature_raw_data;
  static std::vector<std::vector<float>> calibrators_conc_fit_data, calibrators_feature_fit_data;
  static std::vector<std::string> calibrators_series_names;
  static std::string calibrators_x_axis_title = "Concentration ratio (?)";
  static std::string calibrators_y_axis_title = "Feature ratio (?)";
  static float calibrators_conc_min = 1e6, calibrators_conc_max = 0, calibrators_feature_min = 1e6, calibrators_feature_max = 0;

  bool show_top_window_ = false;
  bool show_bottom_window_ = false;
  bool show_left_window_ = false;
  bool show_right_window_ = false;

  // to disable buttons
  bool workflow_is_done_ = true;
  bool file_loading_is_done_ = true;

  // View: Bottom window
  bool show_info_ = true;
  bool show_log_ = false;

  // View: left or right windows (i.e., Explorer pane)
  bool show_injection_explorer = true; // list of injection names with advanced options for filtering
  bool show_transitions_explorer = true; // list of transition names (e.g., subset of the features for non-targeted case) with advanced options for filtering
  bool show_features_explorer = false; // list of features with advanced options for filtering

  // View: Top window
  bool show_sequence_table = false;
  bool show_transitions_table = false;
  bool show_workflow_table = true;
  bool show_parameters_table = false;
  bool show_quant_method_table = false;
  bool show_stds_concs_table = false;
  bool show_comp_filters_table = false;
  bool show_comp_group_filters_table = false;
  bool show_comp_qcs_table = false;
  bool show_comp_group_qcs_table = false;
  bool show_feature_table = false; // table of all injections, features, and metavalues
  bool show_feature_pivot_table = false; // injection vs. feature for a particular metavalue
  bool show_chromatogram_line_plot = false; // time vs. intensity for the raw data and annotated feature
  bool show_spectra_line_plot = false; // time vs. intensity for the raw data and annotated feature
  bool show_feature_line_plot = false; // injection vs. metavalue for n selected features
  bool show_feature_heatmap_plot = false; // injection vs. feature for a particular metavalue
  bool show_calibrators_line_plot = false; // peak area/height ratio vs. concentration ratio

  // Popup modals
  bool popup_about_ = false;
  bool popup_run_workflow_ = false;
  bool popup_file_picker_ = false;

  ApplicationHandler application_handler_;
  WorkflowManager manager_;
  GuiAppender appender_;

  // widgets
  GenericTextWidget quickInfoText_;
  FilePicker file_picker_;
  Report     report_;
  Workflow   workflow_;
  report_.setApplicationHandler(application_handler_);
  workflow_.setApplicationHandler(application_handler_);
  const std::time_t t = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
  char filename[128];
  strftime(filename, 128, "smartpeak_log_%Y-%m-%d_%H-%M-%S.csv", std::localtime(&t));

  // Add .csv appender: 32 MiB per file, max. 100 log files
  plog::RollingFileAppender<plog::CsvFormatter>
    fileAppender(filename, 1024 * 1024 * 32, 100);

  // Add console appender, instead of only the file one
  plog::ConsoleAppender<plog::TxtFormatter> consoleAppender;

  // Init logger with all the appenders
  plog::init(plog::debug, &fileAppender)
    .addAppender(&consoleAppender)
    .addAppender(&appender_);

  LOGN << "Log file at: " << filename;

  // Setup SDL
  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0)
  {
    printf("Error: %s\n", SDL_GetError());
    return -1;
  }

  // Setup window
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
  SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
  SDL_DisplayMode current;
  SDL_GetCurrentDisplayMode(0, &current);
  SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
  SDL_Window* window = SDL_CreateWindow("SmartPeak SDL2+OpenGL application", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, window_flags);
  SDL_GLContext gl_context = SDL_GL_CreateContext(window);
  SDL_GL_SetSwapInterval(1); // Enable vsync

  // Setup Dear ImGui context
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO(); (void)io;
  //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls

  // Setup Dear ImGui style
  ImGui::StyleColorsDark();
  //ImGui::StyleColorsClassic();

  // Setup Platform/Renderer bindings
  ImGui_ImplSDL2_InitForOpenGL(window, gl_context);
  ImGui_ImplOpenGL2_Init();

  ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
  WindowSizesAndPositions win_size_and_pos;

  // Main loop
  bool done = false;
  while (!done)
  {
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
      ImGui_ImplSDL2_ProcessEvent(&event);
      if (event.type == SDL_QUIT)
        done = true;
    }

    // Start the Dear ImGui frame
    ImGui_ImplOpenGL2_NewFrame();
    ImGui_ImplSDL2_NewFrame(window);
    ImGui::NewFrame();

    { // keeping this block to easily collapse/expand the bulk of the loop
    // Intialize the window sizes
    win_size_and_pos.setXAndYSizes(io.DisplaySize.x, io.DisplaySize.y);

    workflow_is_done_ = manager_.isWorkflowDone();
    file_loading_is_done_ = file_picker_.fileLoadingIsDone();

    // Make the quick info text
    quickInfoText_.text_lines.clear();
    quickInfoText_.text_lines.push_back(workflow_is_done_ ? "Workflow status: done" : "Workflow status: running...");
    quickInfoText_.text_lines.push_back(file_loading_is_done_ ? "File loading status: done" : "File loading status: running...");

    if (popup_file_picker_)
    {
      ImGui::OpenPopup("Pick a pathname");
      popup_file_picker_ = false;
    }

    file_picker_.draw();

    if (popup_run_workflow_)
    {
      ImGui::OpenPopup("Run workflow modal");
      popup_run_workflow_ = false;
    }

    if (ImGui::BeginPopupModal("Run workflow modal", NULL, ImGuiWindowFlags_AlwaysAutoResize))
    {
      ImGui::Text("mzML folder");
      ImGui::PushID(1);
      ImGui::InputTextWithHint("", application_handler_.mzML_dir_.c_str(), &application_handler_.mzML_dir_);
      ImGui::PopID();
      ImGui::SameLine();
      ImGui::PushID(11);
      if (ImGui::Button("Select"))
      {
        static SetRawDataPathname processor(application_handler_);
        file_picker_.setProcessor(processor);
        popup_file_picker_ = true;
      }
      ImGui::PopID();

      ImGui::Text("Input features folder");
      ImGui::PushID(2);
      ImGui::InputTextWithHint("", application_handler_.features_in_dir_.c_str(), &application_handler_.features_in_dir_);
      ImGui::PopID();
      ImGui::SameLine();
      ImGui::PushID(22);
      if (ImGui::Button("Select"))
      {
        static SetInputFeaturesPathname processor(application_handler_);
        file_picker_.setProcessor(processor);
        popup_file_picker_ = true;
      }
      ImGui::PopID();

      ImGui::Text("Output features folder");
      ImGui::PushID(3);
      ImGui::InputTextWithHint("", application_handler_.features_out_dir_.c_str(), &application_handler_.features_out_dir_);
      ImGui::PopID();
      ImGui::SameLine();
      ImGui::PushID(33);
      if (ImGui::Button("Select"))
      {
        static SetOutputFeaturesPathname processor(application_handler_);
        file_picker_.setProcessor(processor);
        popup_file_picker_ = true;
      }
      ImGui::PopID();

      if (popup_file_picker_)
      {
        ImGui::OpenPopup("Pick a pathname");
        popup_file_picker_ = false;
      }

      file_picker_.draw();

      ImGui::Separator();

      if (ImGui::Button("Run workflow"))
      {
        for (const std::string& pathname : {application_handler_.mzML_dir_, application_handler_.features_in_dir_, application_handler_.features_out_dir_}) {
          fs::create_directories(fs::path(pathname));
        }
        for (ApplicationHandler::Command& cmd : application_handler_.commands_)
        {
          for (std::pair<const std::string, Filenames>& p : cmd.dynamic_filenames)
          {
            Filenames::updateDefaultDynamicFilenames(
              application_handler_.mzML_dir_,
              application_handler_.features_in_dir_,
              application_handler_.features_out_dir_,
              p.second
            );
          }
        }
        manager_.addWorkflow(application_handler_);
        ImGui::CloseCurrentPopup();
      }

      ImGui::SameLine();
      if (ImGui::Button("Close"))
      {
        ImGui::CloseCurrentPopup();
      }

      ImGui::EndPopup();
    }

    if (popup_about_)
    {
      ImGui::OpenPopup("About modal");
      popup_about_ = false;
    }

    if (ImGui::BeginPopupModal("About modal", NULL, ImGuiWindowFlags_AlwaysAutoResize))
    {
      ImGui::Text("About SmartPeak");
      ImGui::Text("SmartPeak %s", "1.0"); //TODO: define version function
      ImGui::Separator();
      ImGui::Text("By the hardworking SmartPeak developers.");
      ImGui::Separator();
      if (ImGui::Button("Close"))
      {
        ImGui::CloseCurrentPopup();
      }
      ImGui::EndPopup();
    }

    if (report_.draw_)
    {
      report_.draw();
    }

    if (workflow_.draw_)
    {
      workflow_.draw();
    }

    if (ImGui::BeginMainMenuBar())
    {
      if (ImGui::BeginMenu("File"))
      {
        ImGui::MenuItem("Session", NULL, false, false);
        if (ImGui::MenuItem("New Session", NULL, false, false))
        {
          //TODO: SQL light interface
        }

        if (ImGui::MenuItem("Load Session", NULL, false, false))
        {
          //TODO: open file browser modal
          // ImGui::OpenPopup("Delete?");
        }

        if (ImGui::MenuItem("Load session from sequence", NULL, false, workflow_is_done_ && file_loading_is_done_)) {
          static LoadSessionFromSequence processor(application_handler_);
          file_picker_.setProcessor(processor);
          popup_file_picker_ = true;
        }

        if (ImGui::MenuItem("Save Session", NULL, false, false))
        {
          //TODO
        }
        if (ImGui::MenuItem("Save Session As...", NULL, false, false))
        {
          //TODO: open save as File browser modal
        }
        ImGui::Separator();
        ImGui::MenuItem("Text file", NULL, false, false);
        if (ImGui::BeginMenu("Import File", false))
        {
          if (ImGui::MenuItem("Sequence")) {}
          if (ImGui::MenuItem("Transitions")) {}
          if (ImGui::MenuItem("Parameters")) {}
          if (ImGui::MenuItem("Reference data")) {}
          if (ImGui::MenuItem("Quant Method")) {}
          if (ImGui::MenuItem("Standards Conc")) {}
          if (ImGui::MenuItem("Comp Filters")) {}
          if (ImGui::MenuItem("Comp Group Filters")) {}
          if (ImGui::MenuItem("Comp QCs")) {}
          if (ImGui::MenuItem("Comp Group QCs")) {}
          if (ImGui::MenuItem("Comp %RSD Filters")) {}
          if (ImGui::MenuItem("Comp Group %RSD Filters")) {}
          if (ImGui::MenuItem("Comp %RSD QCs")) {}
          if (ImGui::MenuItem("Comp Group %RSD QCs")) {}
          if (ImGui::MenuItem("Comp %Background Filters")) {}
          if (ImGui::MenuItem("Comp Group %Background Filters")) {}
          if (ImGui::MenuItem("Comp %Background QCs")) {}
          if (ImGui::MenuItem("Comp Group %Background QCs")) {}
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
          if (ImGui::MenuItem("Comp %RSD Filters")) {}
          if (ImGui::MenuItem("Comp Group %RSD Filters")) {}
          if (ImGui::MenuItem("Comp %RSD QCs")) {}
          if (ImGui::MenuItem("Comp Group %RSD QCs")) {}
          if (ImGui::MenuItem("Comp %Background Filters")) {}
          if (ImGui::MenuItem("Comp Group %Background Filters")) {}
          if (ImGui::MenuItem("Comp %Background QCs")) {}
          if (ImGui::MenuItem("Comp Group %Background QCs")) {}
          ImGui::EndMenu();
        }
        // TODO
        //ImGui::Separator();
        //if (ImGui::MenuItem("Quit", "Alt+F4")) {}
        ImGui::EndMenu();
      }
      if (ImGui::BeginMenu("Edit"))
      {
        ImGui::MenuItem("Session", NULL, false, false);
        if (ImGui::MenuItem("Undo", "CTRL+Z", false, false)) {}
        if (ImGui::MenuItem("Redo", "CTRL+Y", false, false)) {} // Disabled item
        ImGui::Separator();
        ImGui::MenuItem("Settings", NULL, false, false);
        if (ImGui::MenuItem("Tables", NULL, false, false)) {} // TODO: modal of settings
        if (ImGui::MenuItem("Plots", NULL, false, false)) {} // TODO: modal of settings
        if (ImGui::MenuItem("Explorer", NULL, false, false)) {} // TODO: modal of settings
        if (ImGui::MenuItem("Search", NULL, false, false)) {} // TODO: modal of settings
        if (ImGui::MenuItem("Workflow", NULL, false, workflow_is_done_))
        {
          initializeDataDirs(application_handler_);
          workflow_.draw_ = true;
        }
        ImGui::EndMenu();
      }
      if (ImGui::BeginMenu("View"))
      {
        ImGui::MenuItem("Explorer window", NULL, false, false);
        if (ImGui::MenuItem("Injections", NULL, &show_injection_explorer)) {}
        if (ImGui::MenuItem("Transitions", NULL, &show_transitions_explorer)) {}
        if (ImGui::MenuItem("Features", NULL, &show_features_explorer)) {}  // including metadata?
        ImGui::Separator(); // Primary input
        ImGui::MenuItem("Main window (Tables)", NULL, false, false);
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
        if (ImGui::MenuItem("Features (table)", NULL, &show_feature_table)) {}
        if (ImGui::MenuItem("Features (matrix)", NULL, &show_feature_pivot_table)) {}
        ImGui::MenuItem("Main window (Plots)", NULL, false, false);
        if (ImGui::MenuItem("Chromatogram", NULL, &show_chromatogram_line_plot)) {}
        if (ImGui::MenuItem("Spectra", NULL, &show_spectra_line_plot)) {}
        if (ImGui::MenuItem("Features (line)", NULL, &show_feature_line_plot)) {}
        if (ImGui::MenuItem("Features (heatmap)", NULL, &show_feature_heatmap_plot)) {}
        if (ImGui::MenuItem("Calibrators", NULL, &show_calibrators_line_plot)) {}
        ImGui::MenuItem("Info window", NULL, false, false);
        if (ImGui::MenuItem("Info", NULL, &show_info_)) {}
        if (ImGui::MenuItem("Log", NULL, &show_log_)) {}
        ImGui::EndMenu();
      }
      if (ImGui::BeginMenu("Actions"))
      {
        if (ImGui::MenuItem("Run command", NULL, false, workflow_is_done_ && file_loading_is_done_))
        {
          initializeDataDirs(application_handler_);
          // TODO: do the rest
        }
        if (ImGui::MenuItem("Run workflow"))
        {
          if (application_handler_.commands_.empty())
          {
            LOGW << "Workflow has no steps to run. Please set the workflow's steps.";
          }
          initializeDataDirs(application_handler_);
          popup_run_workflow_ = true;
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
        ImGui::EndMenu();
      }
      if (ImGui::BeginMenu("Help"))
      {
        ImGui::MenuItem("About", NULL, &popup_about_);
        if (ImGui::MenuItem("Documentation")) {}
        if (ImGui::MenuItem("Version")) {}
        ImGui::EndMenu();
      }
      ImGui::EndMainMenuBar();
    }

    show_top_window_ = show_sequence_table || show_transitions_table || show_workflow_table || show_parameters_table
      || show_quant_method_table || show_stds_concs_table || show_comp_filters_table || show_comp_group_filters_table
      || show_comp_qcs_table || show_comp_group_qcs_table || show_feature_table || show_feature_pivot_table
      || show_chromatogram_line_plot || show_spectra_line_plot || show_feature_line_plot || show_feature_heatmap_plot || show_calibrators_line_plot;
    show_bottom_window_ = show_info_ || show_log_;
    show_left_window_ = show_injection_explorer || show_transitions_explorer || show_features_explorer;
    win_size_and_pos.setWindowSizesAndPositions(show_top_window_, show_bottom_window_, show_left_window_, show_right_window_);

    // Left window
    if (show_left_window_) {
      ImGui::SetNextWindowPos(ImVec2(win_size_and_pos.left_window_x_pos_, win_size_and_pos.left_and_right_window_y_pos_));
      ImGui::SetNextWindowSize(ImVec2(win_size_and_pos.left_window_x_size_, win_size_and_pos.left_and_right_window_y_size_));
      ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0);
      const ImGuiWindowFlags left_window_flags =
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoFocusOnAppearing;
      ImGui::Begin("Left window", NULL, left_window_flags);
      if (ImGui::BeginTabBar("Left window tab bar", ImGuiTabBarFlags_Reorderable))
      {
        if (show_injection_explorer && ImGui::BeginTabItem("Injections", &show_injection_explorer))
        {
          // Make the injection explorer headers
          if (injection_explorer_headers.size() <= 0 && injection_explorer_checkbox_headers.size() <=0) {
            injection_explorer_headers.resize(2);
            injection_explorer_headers.setValues({"inj#", "sample_name" });
            injection_explorer_checkbox_headers.resize(3);
            injection_explorer_checkbox_headers.setValues({ "workflow", "plot", "table" });
          }
          const int n_cols = injection_explorer_headers.size();
          const int n_rows = application_handler_.sequenceHandler_.getSequence().size();

          // Make the injection explorer body
          if (injection_explorer_body.size() <= 0 && n_rows > 0) {

            injection_explorer_body.resize(n_rows,n_cols);
            int col = 0, row = 0;
            for (const auto& injection : application_handler_.sequenceHandler_.getSequence()) {
              injection_explorer_body(row,col) = std::to_string(injection.getMetaData().inj_number);
              ++col;
              injection_explorer_body(row,col) = injection.getMetaData().sample_name;
              col = 0;
              ++row;
            }
            injection_explorer_checkbox_body.resize(n_rows,(int)injection_explorer_checkbox_headers.size());
            injection_explorer_checkbox_body.setConstant(true);
            injection_explorer_checked_rows.resize(n_rows);
            injection_explorer_checked_rows.setConstant(true);
          }

          // Call the Explorer widget
          ExplorerWidget Explorer;
          Explorer.headers_ = injection_explorer_headers;
          Explorer.columns_ = injection_explorer_body;
          Explorer.checked_rows_ = injection_explorer_checked_rows;
          Explorer.checkbox_headers_ = injection_explorer_checkbox_headers;
          Explorer.checkbox_columns_ = injection_explorer_checkbox_body;
          Explorer.table_id_ = "InjectionsExplorerWindow";
          Explorer.draw();

          ImGui::EndTabItem();
        }
        if (show_transitions_explorer && ImGui::BeginTabItem("Transitions", &show_transitions_explorer))
        {
          // Make the transition explorer headers
          if (transition_explorer_headers.size() <= 0 && transition_explorer_checkbox_headers.size()<=0) {
            transition_explorer_headers.resize(2);
            transition_explorer_headers.setValues({"transition_group","transition_name"});
            transition_explorer_checkbox_headers.resize(3);
            transition_explorer_checkbox_headers.setValues({ "workflow", "plot", "table" });
          }
          const int n_cols = transition_explorer_headers.size();

          // Make the transition table body
          if (application_handler_.sequenceHandler_.getSequence().size() > 0) {
            const auto& targeted_exp = application_handler_.sequenceHandler_.getSequence().at(0).getRawDataShared()->getTargetedExperimentShared();
            const int n_rows = targeted_exp->getTransitions().size();
            if (transition_explorer_body.size() <= 0 && n_rows > 0) {
              transition_explorer_body.resize(n_rows, n_cols);
              int col = 0, row = 0;
              for (const auto& transition : targeted_exp->getTransitions()) {
                transition_explorer_body(row,col) = transition.getPeptideRef();
                ++col;
                transition_explorer_body(row,col) = transition.getNativeID();
                col = 0;
                ++row;
              }
              transition_explorer_checkbox_body.resize(n_rows, (int)transition_explorer_checkbox_headers.size());
              transition_explorer_checkbox_body.setConstant(true);
              transition_explorer_checked_rows.resize(n_rows);
              transition_explorer_checked_rows.setConstant(true);
            }
          }

          // Call the Explorer widget
          ExplorerWidget Explorer;
          Explorer.headers_ = transition_explorer_headers;
          Explorer.columns_ = transition_explorer_body;
          Explorer.checked_rows_ = transition_explorer_checked_rows;
          Explorer.checkbox_headers_ = transition_explorer_checkbox_headers;
          Explorer.checkbox_columns_ = transition_explorer_checkbox_body;
          Explorer.table_id_ = "TransitionsExplorerWindow";
          Explorer.draw();
          ImGui::EndTabItem();
        }
        if (show_features_explorer && ImGui::BeginTabItem("Features", &show_features_explorer))
        {
          // Make the feature explorer headers
          if (feature_explorer_headers.size() <= 0) {
            feature_explorer_headers.resize(1);
            feature_explorer_headers.setValues({ "metavalue" });
            feature_explorer_checkbox_headers.resize(2);
            feature_explorer_checkbox_headers.setValues({ "plot", "table" });
          }
          const int n_cols = feature_explorer_headers.size();
          const int n_rows = metadatafloatToString.size();

          // Make the feature explorer body
          if (feature_explorer_body.size() <= 0 && n_rows > 0) {
            feature_explorer_body.resize(n_rows,n_cols);
            int col = 0, row = 0;
            for (const auto& metadata : metadatafloatToString) {
              feature_explorer_body(row,col) = metadata.second;
              col = 0;
              ++row;
            }
            feature_explorer_checkbox_body.resize(n_rows, (int)feature_explorer_checkbox_headers.size());
            feature_explorer_checkbox_body.setConstant(true);
            feature_explorer_checked_rows.resize(n_rows);
            feature_explorer_checked_rows.setConstant(true);
          }

          // Call the Explorer widget
          ExplorerWidget Explorer;
          Explorer.headers_ = feature_explorer_headers;
          Explorer.columns_ = feature_explorer_body;
          Explorer.checked_rows_ = feature_explorer_checked_rows;
          Explorer.checkbox_headers_ = feature_explorer_checkbox_headers;
          Explorer.checkbox_columns_ = feature_explorer_checkbox_body;
          Explorer.table_id_ = "FeaturesExplorerWindow";
          Explorer.draw();
          ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
      }
      ImGui::End();
      ImGui::PopStyleVar();
    }

    // Top window
    if (show_top_window_)
    {
      ImGui::SetNextWindowPos(ImVec2(win_size_and_pos.bottom_and_top_window_x_pos_, win_size_and_pos.top_window_y_pos_));
      ImGui::SetNextWindowSize(ImVec2(win_size_and_pos.bottom_and_top_window_x_size_, win_size_and_pos.top_window_y_size_));
      ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0);
      const ImGuiWindowFlags top_window_flags =
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoFocusOnAppearing;
      ImGui::Begin("Top window", NULL, top_window_flags);
      if (ImGui::BeginTabBar("Top window tab bar", ImGuiTabBarFlags_Reorderable))
      {
        if (show_sequence_table && ImGui::BeginTabItem("Sequence", &show_sequence_table))
        {
          // Make the sequence table headers
          if (sequence_table_headers.size() <= 0) {
            sequence_table_headers.resize(11);
            sequence_table_headers.setValues({
            "inj_number", "sample_name", "sample_group_name" , "sequence_segment_name" , "original_filename",
            "sample_type", "acq_method_name", "inj_volume", "inj_volume_units", "batch_name", // skipping optional members
            "acquisition_date_and_time" });
          }
          const int n_cols = sequence_table_headers.size();
          const int n_rows = application_handler_.sequenceHandler_.getSequence().size();

          // Make the sequence table body
          if (sequence_table_body.size() <= 0 && n_rows > 0) {
            sequence_table_body.resize(n_rows,n_cols);
            int col = 0, row = 0;
            for (const auto& injection : application_handler_.sequenceHandler_.getSequence()) {
              sequence_table_body(row,col) = std::to_string(injection.getMetaData().inj_number);
              ++col;
              sequence_table_body(row,col) = injection.getMetaData().sample_name;
              ++col;
              sequence_table_body(row,col) = injection.getMetaData().sample_group_name;
              ++col;
              sequence_table_body(row,col) = injection.getMetaData().sequence_segment_name;
              ++col;
              sequence_table_body(row,col) = injection.getMetaData().original_filename;
              ++col;
              sequence_table_body(row,col) = injection.getMetaData().getSampleTypeAsString();
              ++col;
              sequence_table_body(row,col) = injection.getMetaData().acq_method_name;
              ++col;
              sequence_table_body(row,col) = std::to_string(injection.getMetaData().inj_volume);
              ++col;
              sequence_table_body(row,col) = injection.getMetaData().inj_volume_units;
              ++col;
              sequence_table_body(row,col) = injection.getMetaData().batch_name;
              ++col;
              // Skipping optional members
              sequence_table_body(row,col) = injection.getMetaData().getAcquisitionDateAndTimeAsString();
              col = 0;
              ++row;
            }
          }

          // Call the table widget
          GenericTableWidget Table;
          Table.headers_ = sequence_table_headers;
          Table.columns_ = sequence_table_body;
          Table.table_id_ = "SequenceMainWindow";
          Table.draw();
          ImGui::EndTabItem();
        }
        if (show_transitions_table && ImGui::BeginTabItem("Transitions", &show_transitions_table))
        {
          // Make the transition table headers
          if (transitions_table_headers.size() <= 0) {
            transitions_table_headers.resize(9);
            transitions_table_headers.setValues({ // NOTE: only showing a subset
            "transition_group","transition_name","RetentionTime","PrecursorMz","ProductMz",
            "LabelType","quantifying_transition","identifying_transition","detecting_transition" });
          }
          const int n_cols = transitions_table_headers.size();

          // Make the transition table body
          if (application_handler_.sequenceHandler_.getSequence().size() > 0) {
            const auto& targeted_exp = application_handler_.sequenceHandler_.getSequence().at(0).getRawDataShared()->getTargetedExperimentShared();
            const int n_rows = targeted_exp->getTransitions().size();
            if (transitions_table_body.size() <= 0 && n_rows > 0) {
              transitions_table_body.resize(n_rows,n_cols);
              int col = 0, row = 0;
              for (const auto& transition : targeted_exp->getTransitions()) {
                transitions_table_body(row,col) = transition.getPeptideRef();
                ++col;
                transitions_table_body(row,col) = transition.getNativeID();
                ++col;
                transitions_table_body(row,col) = std::to_string(targeted_exp->getPeptideByRef(transition.getPeptideRef()).getRetentionTime());
                ++col;
                transitions_table_body(row,col) = std::to_string(transition.getPrecursorMZ());
                ++col;
                transitions_table_body(row,col) = std::to_string(transition.getProductMZ());
                ++col;
                //transitions_table_body(row,col) = transition.getProduct().getMetaValue("LabelType");
                transitions_table_body(row,col) = "?";
                ++col;
                transitions_table_body(row,col) = std::to_string(transition.isQuantifyingTransition());
                ++col;
                transitions_table_body(row,col) = std::to_string(transition.isIdentifyingTransition());
                ++col;
                transitions_table_body(row,col) = std::to_string(transition.isDetectingTransition());
                col = 0;
                ++row;
              }
            }
          }

          // Call the table widget
          GenericTableWidget Table;
          Table.headers_ = transitions_table_headers;
          Table.columns_ = transitions_table_body;
          Table.table_id_ = "TransitionsMainWindow";
          Table.draw();
          ImGui::EndTabItem();
        }
        if (show_workflow_table && ImGui::BeginTabItem("Workflow", &show_workflow_table))
        {
          // Make the workflow table headers
          if (workflow_table_headers.size() <= 0) {
            workflow_table_headers.resize(2);
            workflow_table_headers.setValues({ "step", "command" });
          }
          const int n_cols = workflow_table_headers.size();
          const int n_rows = workflow_.getCommands().size();

          // Make the workflow table body
          if (workflow_table_body.size() != workflow_.getCommands().size()) { // TODO: does not account for case of different commands of the same length!
            workflow_table_body.resize(n_rows,n_cols);
            int col = 0, row = 0;
            for (const auto& command : workflow_.getCommands()) {
              workflow_table_body(row,col) = std::to_string(row);
              ++col;
              workflow_table_body(row,col) = command.getName();
              col = 0;
              ++row;
            }
          }

          // Call the table widget
          GenericTableWidget Table;
          Table.headers_ = workflow_table_headers;
          Table.columns_ = workflow_table_body;
          Table.table_id_ = "WorkflowMainWindow";
          Table.draw();
          ImGui::EndTabItem();
        }
        if (show_parameters_table && ImGui::BeginTabItem("Parameters", &show_parameters_table))
        {
          if (application_handler_.sequenceHandler_.getSequenceSegments().size() > 0) {
            // Make the parameters table headers
            if (parameters_table_headers.size() <= 0) {
              parameters_table_headers.resize(4);
              parameters_table_headers.setValues({ "function","name","type","value",/*"default","restrictions","description"*/ });
            }
            const int n_cols = parameters_table_headers.size();

            // Make the parameters table body
            const int n_funcs = application_handler_.sequenceHandler_.getSequence().at(0).getRawData().getParameters().size();
            if (parameters_table_body.size() <= 0 && n_funcs > 0) {
              int n_rows = 0;
              for (const auto& parameters : application_handler_.sequenceHandler_.getSequence().at(0).getRawData().getParameters()) {
                n_rows += parameters.second.size();
              }
              parameters_table_body.resize(n_rows,n_cols);
              int col = 0, row = 0;
              for (const auto& parameters : application_handler_.sequenceHandler_.getSequence().at(0).getRawData().getParameters()) {
                for (const auto& parameter : parameters.second) {
                  parameters_table_body(row,col) = parameters.first;
                  ++col;
                  parameters_table_body(row,col) = parameter.at("name");
                  ++col;
                  parameters_table_body(row,col) = parameter.at("type");
                  ++col;
                  parameters_table_body(row,col) = parameter.at("value");
                  col = 0;
                  ++row;
                }
              }
            }
          }

          // Call the table widget
          GenericTableWidget Table;
          Table.headers_ = parameters_table_headers;
          Table.columns_ = parameters_table_body;
          Table.table_id_ = "ParametersMainWindow";
          Table.draw();
          ImGui::EndTabItem();
        }
        if (show_quant_method_table && ImGui::BeginTabItem("Quantitation Method", &show_quant_method_table))
        {
          if (application_handler_.sequenceHandler_.getSequenceSegments().size() > 0) {
            // Make the quant_method table headers
            if (quant_method_table_headers.size() <= 0) {
              quant_method_table_headers.resize(20);
              quant_method_table_headers.setValues({
                "component_name", "sequence_segment_name", "IS_name", "feature_name", "concentration_units",
                "llod", "ulod", "lloq", "uloq", "correlation_coefficient",
                "n_points", "transformation_model",
                "transformation_model_param_slope",
                "transformation_model_param_intercept",
                "transformation_model_param_x_weight",
                "transformation_model_param_y_weight",
                "transformation_model_param_x_datum_min",
                "transformation_model_param_x_datum_max",
                "transformation_model_param_y_datum_min",
                "transformation_model_param_y_datum_max" });
            }
            const int n_cols = quant_method_table_headers.size();

            // Make the quant_method table body
            const int n_rows = application_handler_.sequenceHandler_.getSequenceSegments().at(0).getQuantitationMethodsShared()->size() * application_handler_.sequenceHandler_.getSequenceSegments().size();
            if (quant_method_table_body.size() <= 0 && n_rows > 0) {
              quant_method_table_body.resize(n_rows,n_cols);
              int col = 0, row = 0;
              for (const auto& seq_segment : application_handler_.sequenceHandler_.getSequenceSegments()) {
                for (const auto& quant_method : seq_segment.getQuantitationMethods()) {
                  quant_method_table_body(row,col) = quant_method.getComponentName();
                  ++col;
                  quant_method_table_body(row,col) = seq_segment.getSequenceSegmentName();
                  ++col;
                  quant_method_table_body(row,col) = quant_method.getISName();
                  ++col;
                  quant_method_table_body(row,col) = quant_method.getFeatureName();
                  ++col;
                  quant_method_table_body(row,col) = quant_method.getConcentrationUnits();
                  ++col;
                  quant_method_table_body(row,col) = std::to_string(quant_method.getLLOD());
                  ++col;
                  quant_method_table_body(row,col) = std::to_string(quant_method.getULOD());
                  ++col;
                  quant_method_table_body(row,col) = std::to_string(quant_method.getLLOQ());
                  ++col;
                  quant_method_table_body(row,col) = std::to_string(quant_method.getULOQ());
                  ++col;
                  quant_method_table_body(row,col) = std::to_string(quant_method.getCorrelationCoefficient());
                  ++col;
                  quant_method_table_body(row,col) = std::to_string(quant_method.getNPoints());
                  ++col;
                  quant_method_table_body(row,col) = quant_method.getTransformationModel();
                  ++col;
                  quant_method_table_body(row,col) = std::to_string((double)quant_method.getTransformationModelParams().getValue("slope"));
                  ++col;
                  quant_method_table_body(row,col) = std::to_string((double)quant_method.getTransformationModelParams().getValue("intercept"));
                  ++col;
                  quant_method_table_body(row,col) = (std::string)quant_method.getTransformationModelParams().getValue("x_weight");
                  ++col;
                  quant_method_table_body(row,col) = (std::string)quant_method.getTransformationModelParams().getValue("y_weight");
                  ++col;
                  quant_method_table_body(row,col) = std::to_string((double)quant_method.getTransformationModelParams().getValue("x_datum_min"));
                  ++col;
                  quant_method_table_body(row,col) = std::to_string((double)quant_method.getTransformationModelParams().getValue("x_datum_max"));
                  ++col;
                  quant_method_table_body(row,col) = std::to_string((double)quant_method.getTransformationModelParams().getValue("y_datum_min"));
                  ++col;
                  quant_method_table_body(row,col) = std::to_string((double)quant_method.getTransformationModelParams().getValue("y_datum_max"));
                  col = 0;
                  ++row;
                }
              }
            }
          }

          // Call the table widget
          GenericTableWidget Table;
          Table.headers_ = quant_method_table_headers;
          Table.columns_ = quant_method_table_body;
          Table.table_id_ = "QuantMethodMainWindow";
          Table.draw();
          ImGui::EndTabItem();
        }
        if (show_stds_concs_table && ImGui::BeginTabItem("Standards Concentrations", &show_stds_concs_table))
        {
          // Make the stds_concs table headers
          if (stds_concs_table_headers.size() <= 0) {
            stds_concs_table_headers.resize(7);
            stds_concs_table_headers.setValues({
              "sample_name", "component_name", "IS_component_name", "actual_concentration",
                "IS_actual_concentration", "concentration_units", "dilution_factor" });
          }
          const int n_cols = stds_concs_table_headers.size();

          // Make the stds_concs table body
          if (application_handler_.sequenceHandler_.getSequenceSegments().size() > 0) {
            const auto& stand_concs = application_handler_.sequenceHandler_.getSequenceSegments().at(0).getStandardsConcentrations();
            const int n_rows = stand_concs.size();
            if (stds_concs_table_body.size() <= 0 && n_rows > 0) {
              stds_concs_table_body.resize(n_rows,n_cols);
              int col = 0, row = 0;
              for (const auto& stds_concs : stand_concs) {
                stds_concs_table_body(row,col) = stds_concs.sample_name;
                ++col;
                stds_concs_table_body(row,col) = stds_concs.component_name;
                ++col;
                stds_concs_table_body(row,col) = stds_concs.IS_component_name;
                ++col;
                stds_concs_table_body(row,col) = std::to_string(stds_concs.actual_concentration);
                ++col;
                stds_concs_table_body(row,col) = std::to_string(stds_concs.IS_actual_concentration);
                ++col;
                stds_concs_table_body(row,col) = stds_concs.concentration_units;
                ++col;
                stds_concs_table_body(row,col) = std::to_string(stds_concs.dilution_factor);
                col = 0;
                ++row;
              }
            }
          }

          // Call the table widget
          GenericTableWidget Table;
          Table.headers_ = stds_concs_table_headers;
          Table.columns_ = stds_concs_table_body;
          Table.table_id_ = "StdsConcsMainWindow";
          Table.draw();
          ImGui::EndTabItem();
        }
        if (show_comp_filters_table && ImGui::BeginTabItem("Component Filters", &show_comp_filters_table))
        {
          if (application_handler_.sequenceHandler_.getSequenceSegments().size() > 0) {
            // Make the comp_filters table headers
            if (comp_filters_table_headers.size() <= 0) {
              std::vector<std::string> tmp = { "component_name","retention_time_l","retention_time_u","intensity_l","intensity_u","overall_quality_l","overall_quality_u" };
              for (const auto& meta_data : application_handler_.sequenceHandler_.getSequenceSegments().at(0).getFeatureFilter().component_qcs.at(0).meta_value_qc) {
                tmp.push_back("metaValue_" + meta_data.first + "_l");
                tmp.push_back("metaValue_" + meta_data.first + "_u");
              }
              comp_filters_table_headers.resize((int)tmp.size());
              for (int i=0;i<tmp.size();++i) comp_filters_table_headers(i) = tmp.at(i);
            }
            const int n_cols = comp_filters_table_headers.size();

            // Make the comp_filters table body
            const int n_rows = application_handler_.sequenceHandler_.getSequenceSegments().at(0).getFeatureFilter().component_qcs.size();
            if (comp_filters_table_body.size() <= 0 && n_rows > 0) {
              comp_filters_table_body.resize(n_rows,n_cols);
              int col = 0, row = 0;
              for (const auto& comp_qcs : application_handler_.sequenceHandler_.getSequenceSegments().at(0).getFeatureFilter().component_qcs) {
                comp_filters_table_body(row,col) = comp_qcs.component_name;
                ++col;
                comp_filters_table_body(row,col) = std::to_string(comp_qcs.retention_time_l);
                ++col;
                comp_filters_table_body(row,col) = std::to_string(comp_qcs.retention_time_u);
                ++col;
                comp_filters_table_body(row,col) = std::to_string(comp_qcs.intensity_l);
                ++col;
                comp_filters_table_body(row,col) = std::to_string(comp_qcs.intensity_u);
                ++col;
                comp_filters_table_body(row,col) = std::to_string(comp_qcs.overall_quality_l);
                ++col;
                comp_filters_table_body(row,col) = std::to_string(comp_qcs.overall_quality_u);
                ++col;
                for (const auto& meta_data : comp_qcs.meta_value_qc) {
                  comp_filters_table_body(row,col) = std::to_string(meta_data.second.first);
                  ++col;
                  comp_filters_table_body(row,col) = std::to_string(meta_data.second.second);
                  ++col;
                }
                col = 0;
                ++row;
              }
            }
          }

          // Call the table widget
          GenericTableWidget Table;
          Table.headers_ = comp_filters_table_headers;
          Table.columns_ = comp_filters_table_body;
          Table.table_id_ = "CompFiltersMainWindow";
          Table.draw();
          ImGui::EndTabItem();
        }
        if (show_comp_group_filters_table && ImGui::BeginTabItem("Component Group Filters", &show_comp_group_filters_table))
        {
          if (application_handler_.sequenceHandler_.getSequenceSegments().size() > 0) {
            // Make the comp_group_filters table headers
            if (comp_group_filters_table_headers.size() <= 0) {
              std::vector<std::string> tmp = { "component_group_name", "retention_time_l", "retention_time_u", "intensity_l", "intensity_u", "overall_quality_l", "overall_quality_u",
                "n_heavy_l", "n_heavy_u", "n_light_l", "n_light_u", "n_detecting_l", "n_detecting_u", "n_quantifying_l", "n_quantifying_u", "n_identifying_l", "n_identifying_u", "n_transitions_l", "n_transitions_u",
                "ion_ratio_pair_name_1", "ion_ratio_pair_name_2", "ion_ratio_l", "ion_ratio_u", "ion_ratio_feature_name" };
              for (const auto& meta_data : application_handler_.sequenceHandler_.getSequenceSegments().at(0).getFeatureFilter().component_group_qcs.at(0).meta_value_qc) {
                tmp.push_back("metaValue_" + meta_data.first + "_l");
                tmp.push_back("metaValue_" + meta_data.first + "_u");
              }
              comp_group_filters_table_headers.resize((int)tmp.size());
              for (int i = 0; i < tmp.size(); ++i) comp_group_filters_table_headers(i) = tmp.at(i);
            }
            const int n_cols = comp_group_filters_table_headers.size();

            // Make the comp_group_filters table body
            const int n_rows = application_handler_.sequenceHandler_.getSequenceSegments().at(0).getFeatureFilter().component_group_qcs.size();
            if (comp_group_filters_table_body.size() <= 0 && n_rows > 0) {
              comp_group_filters_table_body.resize(n_rows,n_cols);
              int col = 0, row = 0;
              for (const auto& comp_group_qcs : application_handler_.sequenceHandler_.getSequenceSegments().at(0).getFeatureFilter().component_group_qcs) {
                comp_group_filters_table_body(row,col) = comp_group_qcs.component_group_name;
                ++col;
                comp_group_filters_table_body(row,col) = std::to_string(comp_group_qcs.retention_time_l);
                ++col;
                comp_group_filters_table_body(row,col) = std::to_string(comp_group_qcs.retention_time_u);
                ++col;
                comp_group_filters_table_body(row,col) = std::to_string(comp_group_qcs.intensity_l);
                ++col;
                comp_group_filters_table_body(row,col) = std::to_string(comp_group_qcs.intensity_u);
                ++col;
                comp_group_filters_table_body(row,col) = std::to_string(comp_group_qcs.overall_quality_l);
                ++col;
                comp_group_filters_table_body(row,col) = std::to_string(comp_group_qcs.overall_quality_u);
                ++col;
                comp_group_filters_table_body(row,col) = std::to_string(comp_group_qcs.n_heavy_l);
                ++col;
                comp_group_filters_table_body(row,col) = std::to_string(comp_group_qcs.n_heavy_u);
                ++col;
                comp_group_filters_table_body(row,col) = std::to_string(comp_group_qcs.n_light_l);
                ++col;
                comp_group_filters_table_body(row,col) = std::to_string(comp_group_qcs.n_light_u);
                ++col;
                comp_group_filters_table_body(row,col) = std::to_string(comp_group_qcs.n_detecting_l);
                ++col;
                comp_group_filters_table_body(row,col) = std::to_string(comp_group_qcs.n_detecting_u);
                ++col;
                comp_group_filters_table_body(row,col) = std::to_string(comp_group_qcs.n_quantifying_l);
                ++col;
                comp_group_filters_table_body(row,col) = std::to_string(comp_group_qcs.n_quantifying_u);
                ++col;
                comp_group_filters_table_body(row,col) = std::to_string(comp_group_qcs.n_identifying_l);
                ++col;
                comp_group_filters_table_body(row,col) = std::to_string(comp_group_qcs.n_identifying_u);
                ++col;
                comp_group_filters_table_body(row,col) = std::to_string(comp_group_qcs.n_transitions_l);
                ++col;
                comp_group_filters_table_body(row,col) = std::to_string(comp_group_qcs.n_transitions_u);
                ++col;
                comp_group_filters_table_body(row,col) = comp_group_qcs.ion_ratio_pair_name_1;
                ++col;
                comp_group_filters_table_body(row,col) = comp_group_qcs.ion_ratio_pair_name_2;
                ++col;
                comp_group_filters_table_body(row,col) = std::to_string(comp_group_qcs.ion_ratio_l);
                ++col;
                comp_group_filters_table_body(row,col) = std::to_string(comp_group_qcs.ion_ratio_u);
                ++col;
                comp_group_filters_table_body(row,col) = comp_group_qcs.ion_ratio_feature_name;
                ++col;
                for (const auto& meta_data : comp_group_qcs.meta_value_qc) {
                  comp_group_filters_table_body(row,col) = std::to_string(meta_data.second.first);
                  ++col;
                  comp_group_filters_table_body(row,col) = std::to_string(meta_data.second.second);
                  ++col;
                }
                col = 0;
                ++row;
              }
            }
          }

          // Call the table widget
          GenericTableWidget Table;
          Table.headers_ = comp_group_filters_table_headers;
          Table.columns_ = comp_group_filters_table_body;
          Table.table_id_ = "CompGroupFiltersMainWindow";
          Table.draw();
          ImGui::EndTabItem();
        }
        if (show_comp_qcs_table && ImGui::BeginTabItem("Component QCs", &show_comp_qcs_table))
        {
          if (application_handler_.sequenceHandler_.getSequenceSegments().size() > 0) {
            // Make the comp_qcs table headers
            if (comp_qcs_table_headers.size() <= 0) {
              std::vector<std::string> tmp = { "component_name","retention_time_l","retention_time_u","intensity_l","intensity_u","overall_quality_l","overall_quality_u" };
              for (const auto& meta_data : application_handler_.sequenceHandler_.getSequenceSegments().at(0).getFeatureQC().component_qcs.at(0).meta_value_qc) {
                tmp.push_back("metaValue_" + meta_data.first + "_l");
                tmp.push_back("metaValue_" + meta_data.first + "_u");
              }
              comp_qcs_table_headers.resize((int)tmp.size());
              for (int i = 0; i < tmp.size(); ++i) comp_qcs_table_headers(i) = tmp.at(i);
            }
            const int n_cols = comp_qcs_table_headers.size();

            // Make the comp_qcs table body
            const int n_rows = application_handler_.sequenceHandler_.getSequenceSegments().at(0).getFeatureQC().component_qcs.size();
            if (comp_qcs_table_body.size() <= 0 && n_rows > 0) {
              comp_qcs_table_body.resize(n_rows,n_cols);
              int col = 0, row = 0;
              for (const auto& comp_qcs : application_handler_.sequenceHandler_.getSequenceSegments().at(0).getFeatureQC().component_qcs) {
                comp_qcs_table_body(row,col) = comp_qcs.component_name;
                ++col;
                comp_qcs_table_body(row,col) = std::to_string(comp_qcs.retention_time_l);
                ++col;
                comp_qcs_table_body(row,col) = std::to_string(comp_qcs.retention_time_u);
                ++col;
                comp_qcs_table_body(row,col) = std::to_string(comp_qcs.intensity_l);
                ++col;
                comp_qcs_table_body(row,col) = std::to_string(comp_qcs.intensity_u);
                ++col;
                comp_qcs_table_body(row,col) = std::to_string(comp_qcs.overall_quality_l);
                ++col;
                comp_qcs_table_body(row,col) = std::to_string(comp_qcs.overall_quality_u);
                ++col;
                for (const auto& meta_data : comp_qcs.meta_value_qc) {
                  comp_qcs_table_body(row,col) = std::to_string(meta_data.second.first);
                  ++col;
                  comp_qcs_table_body(row,col) = std::to_string(meta_data.second.second);
                  ++col;
                }
                col = 0;
                ++row;
              }
            }
          }

          // Call the table widget
          GenericTableWidget Table;
          Table.headers_ = comp_qcs_table_headers;
          Table.columns_ = comp_qcs_table_body;
          Table.table_id_ = "CompQCsMainWindow";
          Table.draw();
          ImGui::EndTabItem();
        }
        if (show_comp_group_qcs_table && ImGui::BeginTabItem("Component Group QCs", &show_comp_group_qcs_table))
        {
          if (application_handler_.sequenceHandler_.getSequenceSegments().size() > 0) {
            // Make the comp_group_qcs table headers
            if (comp_group_qcs_table_headers.size() <= 0) {
              std::vector<std::string> tmp = { "component_group_name", "retention_time_l", "retention_time_u", "intensity_l", "intensity_u", "overall_quality_l", "overall_quality_u",
                "n_heavy_l", "n_heavy_u", "n_light_l", "n_light_u", "n_detecting_l", "n_detecting_u", "n_quantifying_l", "n_quantifying_u", "n_identifying_l", "n_identifying_u", "n_transitions_l", "n_transitions_u",
                "ion_ratio_pair_name_1", "ion_ratio_pair_name_2", "ion_ratio_l", "ion_ratio_u", "ion_ratio_feature_name" };
              for (const auto& meta_data : application_handler_.sequenceHandler_.getSequenceSegments().at(0).getFeatureQC().component_group_qcs.at(0).meta_value_qc) {
                tmp.push_back("metaValue_" + meta_data.first + "_l");
                tmp.push_back("metaValue_" + meta_data.first + "_u");
              }
              comp_group_qcs_table_headers.resize((int)tmp.size());
              for (int i = 0; i < tmp.size(); ++i) comp_group_qcs_table_headers(i) = tmp.at(i);
            }
            const int n_cols = comp_group_qcs_table_headers.size();
            const int n_rows = application_handler_.sequenceHandler_.getSequenceSegments().at(0).getFeatureQC().component_group_qcs.size();

            // Make the comp_group_qcs table body
            if (comp_group_qcs_table_body.size() <= 0 && n_rows > 0) {
              comp_group_qcs_table_body.resize(n_rows,n_cols);
              int col = 0, row = 0;
              for (const auto& comp_group_qcs : application_handler_.sequenceHandler_.getSequenceSegments().at(0).getFeatureQC().component_group_qcs) {
                comp_group_qcs_table_body(row,col) = comp_group_qcs.component_group_name;
                ++col;
                comp_group_qcs_table_body(row,col) = std::to_string(comp_group_qcs.retention_time_l);
                ++col;
                comp_group_qcs_table_body(row,col) = std::to_string(comp_group_qcs.retention_time_u);
                ++col;
                comp_group_qcs_table_body(row,col) = std::to_string(comp_group_qcs.intensity_l);
                ++col;
                comp_group_qcs_table_body(row,col) = std::to_string(comp_group_qcs.intensity_u);
                ++col;
                comp_group_qcs_table_body(row,col) = std::to_string(comp_group_qcs.overall_quality_l);
                ++col;
                comp_group_qcs_table_body(row,col) = std::to_string(comp_group_qcs.overall_quality_u);
                ++col;
                comp_group_qcs_table_body(row,col) = std::to_string(comp_group_qcs.n_heavy_l);
                ++col;
                comp_group_qcs_table_body(row,col) = std::to_string(comp_group_qcs.n_heavy_u);
                ++col;
                comp_group_qcs_table_body(row,col) = std::to_string(comp_group_qcs.n_light_l);
                ++col;
                comp_group_qcs_table_body(row,col) = std::to_string(comp_group_qcs.n_light_u);
                ++col;
                comp_group_qcs_table_body(row,col) = std::to_string(comp_group_qcs.n_detecting_l);
                ++col;
                comp_group_qcs_table_body(row,col) = std::to_string(comp_group_qcs.n_detecting_u);
                ++col;
                comp_group_qcs_table_body(row,col) = std::to_string(comp_group_qcs.n_quantifying_l);
                ++col;
                comp_group_qcs_table_body(row,col) = std::to_string(comp_group_qcs.n_quantifying_u);
                ++col;
                comp_group_qcs_table_body(row,col) = std::to_string(comp_group_qcs.n_identifying_l);
                ++col;
                comp_group_qcs_table_body(row,col) = std::to_string(comp_group_qcs.n_identifying_u);
                ++col;
                comp_group_qcs_table_body(row,col) = std::to_string(comp_group_qcs.n_transitions_l);
                ++col;
                comp_group_qcs_table_body(row,col) = std::to_string(comp_group_qcs.n_transitions_u);
                ++col;
                comp_group_qcs_table_body(row,col) = comp_group_qcs.ion_ratio_pair_name_1;
                ++col;
                comp_group_qcs_table_body(row,col) = comp_group_qcs.ion_ratio_pair_name_2;
                ++col;
                comp_group_qcs_table_body(row,col) = std::to_string(comp_group_qcs.ion_ratio_l);
                ++col;
                comp_group_qcs_table_body(row,col) = std::to_string(comp_group_qcs.ion_ratio_u);
                ++col;
                comp_group_qcs_table_body(row,col) = comp_group_qcs.ion_ratio_feature_name;
                ++col;
                for (const auto& meta_data : comp_group_qcs.meta_value_qc) {
                  comp_group_qcs_table_body(row,col) = std::to_string(meta_data.second.first);
                  ++col;
                  comp_group_qcs_table_body(row,col) = std::to_string(meta_data.second.second);
                  ++col;
                }
                col = 0;
                ++row;
              }
            }
          }

          // Call the table widget
          GenericTableWidget Table;
          Table.headers_ = comp_group_qcs_table_headers;
          Table.columns_ = comp_group_qcs_table_body;
          Table.table_id_ = "CompGroupQCsMainWindow";
          Table.draw();
          ImGui::EndTabItem();
        }
        if (show_feature_table && ImGui::BeginTabItem("Features table", &show_feature_table))
        {
          if (application_handler_.sequenceHandler_.getSequence().size() > 0 && 
            application_handler_.sequenceHandler_.getSequence().at(0).getRawData().getFeatureMapHistory().size() > 0) {
            // Make the feature table headers and body
            if (feature_table_body.size() <= 0) {
              std::vector<std::string> meta_data; // TODO: options for the user to select what meta_data
              for (const std::pair<FeatureMetadata, std::string>& p : metadatafloatToString) meta_data.push_back(p.second);
              std::set<SampleType> sample_types; // TODO: options for the user to select what sample_types
              for (const std::pair<SampleType, std::string>& p : sampleTypeToString) sample_types.insert(p.first);
              std::vector<std::vector<std::string>> table;
              std::vector<std::string> headers;
              SequenceParser::makeDataTableFromMetaValue(application_handler_.sequenceHandler_, table, headers, meta_data, sample_types);
              const int n_cols = feature_table_headers.size();
              const int n_rows = feature_table_body.size();
              feature_table_headers.resize(n_cols);
              feature_table_body.resize(n_rows,n_cols);
              for (int row = 0; row < n_rows; ++row) {
                for (int col = 0; col < n_cols; ++col) {
                  if (row == 0) feature_table_headers(col) = headers.at(col);
                  feature_table_body(row, col) = table.at(row).at(col);
                }
              }
            }
          }

          // Call the table widget
          GenericTableWidget Table;
          Table.headers_ = feature_table_headers;
          Table.columns_ = feature_table_body;
          Table.table_id_ = "features(table)MainWindow";
          Table.draw();
          ImGui::EndTabItem();
        }
        if (show_feature_pivot_table && ImGui::BeginTabItem("Features matrix", &show_feature_pivot_table))
        {
          if (application_handler_.sequenceHandler_.getSequence().size() > 0 &&
            application_handler_.sequenceHandler_.getSequence().at(0).getRawData().getFeatureMapHistory().size() > 0) {
            // Make the feature_pivot table headers and body
            if (feat_value_data.size() <= 0 ) {
              // Create the initial data vectors and matrices from the feature map history
              //std::vector<std::string> meta_data; // TODO: options for the user to select what meta_data
              //for (const std::pair<FeatureMetadata, std::string>& p : metadatafloatToString) meta_data.push_back(p.second);
              std::vector<std::string> meta_data = { "calculated_concentration" };
              std::set<SampleType> sample_types; // TODO: options for the user to select what sample_types
              for (const std::pair<SampleType, std::string>& p : sampleTypeToString) sample_types.insert(p.first);
              Eigen::Tensor<std::string, 2> rows_out;
              SequenceParser::makeDataMatrixFromMetaValue(application_handler_.sequenceHandler_, feat_value_data, feat_heatmap_col_labels, rows_out, meta_data, sample_types);
              feat_heatmap_data.resize(feat_value_data.dimensions());
              feat_heatmap_data = feat_value_data.swap_layout().shuffle(Eigen::array<Eigen::Index, 2>({1,0}));

              // make the injection index for the line plot
              const int n_samples = feat_heatmap_col_labels.size();
              const int n_rows = rows_out.dimension(0);
              feat_sample_data.resize(n_rows, n_samples);
              for (float i = 0; i < n_samples; i += 1) feat_sample_data.chip(i, 1) = feat_sample_data.chip(i, 1).constant(i);
              feat_line_sample_min = 0;
              feat_line_sample_max = n_samples - 1;
              const Eigen::Tensor<float,0> feat_value_data_maximum = feat_value_data.maximum();
              feat_value_max = feat_value_data_maximum(0);
              const Eigen::Tensor<float, 0> feat_value_data_minimum = feat_value_data.minimum();
              feat_value_min = feat_value_data_minimum(0);

              // update the pivot table headers with the columns for the pivot table/heatmap rows
              feature_pivot_table_headers.resize((int)feat_heatmap_col_labels.size() + 3);
              feature_pivot_table_headers(0) = "meta_value_name";
              feature_pivot_table_headers(1) = "component_group_name";
              feature_pivot_table_headers(2) = "component_name";
              feature_pivot_table_headers.slice(Eigen::array<Eigen::Index, 1>({ 3 }), Eigen::array<Eigen::Index, 1>({ feat_heatmap_col_labels.size() })) = feat_heatmap_col_labels;
              const int n_cols = feature_pivot_table_headers.size();

              // allocate space for the pivot table body and heatmap row labels
              feat_heatmap_row_labels.resize(n_rows);
              feature_pivot_table_body.resize(n_rows,n_cols);

              // assign the pivot table body data and heatmap row labels
              int col = 0;
              for (int row = 0; row < n_rows;++row) {
                feat_heatmap_row_labels(row) = rows_out(row, 0) + "::" + rows_out(row, 2);
                for (int j = 0; j < rows_out.dimension(1); ++j) {
                  feature_pivot_table_body(row, col) = rows_out(row,j);
                  ++col;
                }
                for (int j = 0; j < feat_value_data.dimension(1); ++j) {
                  feature_pivot_table_body(row, col) = std::to_string(feat_value_data(row, j));
                  ++col;
                }
                col = 0;
              }
            }
          }

          // Call the table widget
          GenericTableWidget Table;
          Table.headers_ = feature_pivot_table_headers;
          Table.columns_ = feature_pivot_table_body;
          Table.table_id_ = "feature(matrix)MainWindow";
          Table.draw();
          ImGui::EndTabItem();
        }
        if (show_chromatogram_line_plot && ImGui::BeginTabItem("Chromatograms", &show_chromatogram_line_plot))
        {
          if (application_handler_.sequenceHandler_.getSequence().size() > 0 &&
            application_handler_.sequenceHandler_.getSequence().at(0).getRawData().getFeatureMapHistory().size() > 0 &&
            application_handler_.sequenceHandler_.getSequence().at(0).getRawData().getChromatogramMap().getChromatograms().size() > 0) {
            // TODO: filter by transition name (i.e., getNativeID() == component_name)
            // TODO: filter by sample or injection name
            if (chrom_time_data.size() <= 0) { // TODO: better check for updates
              for (const auto& injection : application_handler_.sequenceHandler_.getSequence()) {
                // Extract out the raw data for plotting
                for (const auto& chromatogram : injection.getRawData().getChromatogramMap().getChromatograms()) {
                  std::vector<float> x_data, y_data;
                  for (const auto& point : chromatogram) {
                    x_data.push_back(point.getRT());
                    y_data.push_back(point.getIntensity());
                    chrom_time_min = std::min((float)point.getRT(), chrom_time_min);
                    chrom_intensity_min = std::min((float)point.getIntensity(), chrom_intensity_min);
                    chrom_time_max = std::max((float)point.getRT(), chrom_time_max);
                    chrom_intensity_max = std::max((float)point.getIntensity(), chrom_intensity_max);
                  }
                  chrom_time_data.push_back(x_data);
                  chrom_intensity_data.push_back(y_data);
                  chrom_series_names.push_back(injection.getMetaData().getSampleName() + "::" + chromatogram.getNativeID());
                }
                // Extract out the best left/right for plotting
                for (const auto& feature : injection.getRawData().getFeatureMapHistory()) {
                  for (const auto& subordinate : feature.getSubordinates()) {
                    if (subordinate.getMetaValue("used_") == "true") {
                      if (subordinate.metaValueExists("leftWidth") && subordinate.metaValueExists("rightWidth")) {
                        std::vector<float> x_data, y_data;
                        x_data.push_back(subordinate.getMetaValue("leftWidth"));
                        y_data.push_back(0); // TODO: extract out chrom peak intensity
                        x_data.push_back(subordinate.getMetaValue("rightWidth"));
                        y_data.push_back(0); // TODO: extract out chrom peak intensity
                        chrom_time_data.push_back(x_data);
                        chrom_intensity_data.push_back(y_data);
                        chrom_series_names.push_back(injection.getMetaData().getSampleName() + "::" + (std::string)subordinate.getMetaValue("native_id") + "::" + (std::string)subordinate.getMetaValue("timestamp_"));
                      }
                    }
                  }
                }
                // Extract out the smoothed points for plotting
                for (const auto& feature : injection.getRawData().getFeatureMapHistory()) {
                  for (const auto& subordinate : feature.getSubordinates()) {
                    if (subordinate.getMetaValue("used_") == "true") {
                      std::vector<float> x_data, y_data;
                      for (const auto& point : subordinate.getConvexHull().getHullPoints()) {
                        x_data.push_back(point.getX());
                        y_data.push_back(point.getY());
                        chrom_time_min = std::min((float)point.getX(), chrom_time_min);
                        chrom_intensity_min = std::min((float)point.getY(), chrom_intensity_min);
                        chrom_time_max = std::max((float)point.getX(), chrom_time_max);
                        chrom_intensity_max = std::max((float)point.getY(), chrom_intensity_max);
                      }
                      chrom_time_data.push_back(x_data);
                      chrom_intensity_data.push_back(y_data);
                      chrom_series_names.push_back(injection.getMetaData().getSampleName() + "::" + (std::string)subordinate.getMetaValue("native_id") + "::" + (std::string)subordinate.getMetaValue("timestamp_"));
                    }
                  }
                }
              }
            }
          }

          // Show the line plot
          ScatterPlot2DWidget plot2d;
          plot2d.x_data_ = chrom_time_data;
          plot2d.y_data_ = chrom_intensity_data;
          plot2d.x_axis_title_ = chrom_x_axis_title;
          plot2d.y_axis_title_ = chrom_y_axis_title;
          plot2d.series_names_ = chrom_series_names;
          plot2d.plot_title_ = "Chromatograms Main Window";
          plot2d.x_min_ = chrom_time_min;
          plot2d.x_max_ = chrom_time_max;
          plot2d.y_min_ = chrom_intensity_min;
          plot2d.y_max_ = chrom_intensity_max;
          plot2d.plot_width_ = win_size_and_pos.bottom_and_top_window_x_size_;
          plot2d.plot_height_ = win_size_and_pos.top_window_y_size_;
          plot2d.draw();
          ImGui::EndTabItem();
        }
        if (show_spectra_line_plot && ImGui::BeginTabItem("Spectra", &show_spectra_line_plot))
        {
          ImGui::Text("TODO...");
          ImGui::EndTabItem();
        }
        if (show_feature_line_plot && ImGui::BeginTabItem("Features (line)", &show_feature_line_plot))
        {
          // Show the line plot
          LinePlot2DWidget plot2d;
          plot2d.x_data_ = feat_sample_data.shuffle(Eigen::array<Eigen::Index, 2>({ 1,0 }));
          plot2d.y_data_ = feat_value_data.shuffle(Eigen::array<Eigen::Index, 2>({ 1,0 }));
          plot2d.x_axis_title_ = feat_line_x_axis_title;
          plot2d.y_axis_title_ = feat_line_y_axis_title;
          plot2d.series_names_ = feat_heatmap_row_labels;
          plot2d.plot_title_ = "Features(line)MainWindow";
          plot2d.x_min_ = feat_line_sample_min;
          plot2d.x_max_ = feat_line_sample_max;
          plot2d.y_min_ = feat_value_min;
          plot2d.y_max_ = feat_value_max;
          plot2d.plot_width_ = win_size_and_pos.bottom_and_top_window_x_size_;
          plot2d.plot_height_ = win_size_and_pos.top_window_y_size_;
          plot2d.draw();
          ImGui::EndTabItem();
        }
        if (show_feature_heatmap_plot && ImGui::BeginTabItem("Features (heatmap)", &show_feature_heatmap_plot))
        {
          // Show the line plot
          Heatmap2DWidget plot2d;
          plot2d.data_ = feat_heatmap_data;
          plot2d.columns_ = feat_heatmap_col_labels;
          plot2d.rows_ = feat_heatmap_row_labels;
          plot2d.y_axis_title_ = feat_heatmap_x_axis_title;
          plot2d.y_axis_title_ = feat_heatmap_y_axis_title;
          plot2d.plot_title_ = "Features(heatmap)MainWindow";
          plot2d.data_min_ = feat_value_min;
          plot2d.data_max_ = feat_value_max;
          plot2d.plot_width_ = win_size_and_pos.bottom_and_top_window_x_size_;
          plot2d.plot_height_ = win_size_and_pos.top_window_y_size_;
          plot2d.draw();
          ImGui::EndTabItem();
        }
        if (show_calibrators_line_plot && ImGui::BeginTabItem("Calibrators", &show_calibrators_line_plot))
        {
          if (application_handler_.sequenceHandler_.getSequenceSegments().size() > 0 &&
            application_handler_.sequenceHandler_.getSequenceSegments().at(0).getQuantitationMethods().size() > 0 &&
            application_handler_.sequenceHandler_.getSequenceSegments().at(0).getComponentsToConcentrations().size() > 0 &&
            application_handler_.sequenceHandler_.getSequenceSegments().at(0).getStandardsConcentrations().size() > 0) {
            if (calibrators_conc_fit_data.size() <= 0 && calibrators_conc_raw_data.size() <= 0) {
              // Update the axis titles
              calibrators_x_axis_title = "Concentration (" + application_handler_.sequenceHandler_.getSequenceSegments().at(0).getQuantitationMethods().at(0).getConcentrationUnits() + ")";
              calibrators_y_axis_title = application_handler_.sequenceHandler_.getSequenceSegments().at(0).getQuantitationMethods().at(0).getFeatureName() + " (au)";
              // TODO: filtering by the transition (i.e., getNativeID, getComponentName)
              for (const auto& sequence_segment : application_handler_.sequenceHandler_.getSequenceSegments()) {
                // Extract out raw data used to make the calibrators found in `StandardsConcentrations`
                std::map<std::string, std::pair<std::vector<float>, std::vector<std::string>>> stand_concs_map; // map of x_data and sample_name for a component
                for (const auto& stand_concs : sequence_segment.getStandardsConcentrations()) {
                  // Skip components that have not been fitted with a calibration curve
                  if (sequence_segment.getComponentsToConcentrations().count(stand_concs.component_name) > 0 &&
                    sequence_segment.getComponentsToConcentrations().at(stand_concs.component_name).size() > 0) { // TODO: filter out components that have not been fitted
                    const float x_datum = float(stand_concs.actual_concentration / stand_concs.IS_actual_concentration / stand_concs.dilution_factor);
                    auto found = stand_concs_map.emplace(stand_concs.component_name,
                      std::make_pair(std::vector<float>({ x_datum }),
                        std::vector<std::string>({ stand_concs.sample_name })));
                    if (!found.second) {
                      stand_concs_map.at(stand_concs.component_name).first.push_back(x_datum);
                      stand_concs_map.at(stand_concs.component_name).second.push_back(stand_concs.sample_name);
                    }
                  }
                }

                // Make the line of best fit using the `QuantitationMethods`
                for (const auto& quant_method : sequence_segment.getQuantitationMethods()) {
                  // Skip components that have not been fitted with a calibration curve
                  if (sequence_segment.getComponentsToConcentrations().count(quant_method.getComponentName()) > 0 && 
                    sequence_segment.getComponentsToConcentrations().at(quant_method.getComponentName()).size() > 0 &&
                    (double)quant_method.getTransformationModelParams().getValue("slope") != 1.0) { // TODO: filter out components that have not been fitted
                    // Make the line of best fit using the `QuantitationMethods
                    std::vector<float> y_fit_data;
                    for (const auto& ratio : stand_concs_map.at(quant_method.getComponentName()).first) {
                      // TODO: encapsulate in its own method e.g. sequenceSegmentProcessor
                      // TODO: check that the calibration actually found a best fit (and set to all 0 if not)
                      // calculate the absolute concentration
                      OpenMS::TransformationModel::DataPoints data;
                      OpenMS::TransformationDescription tmd(data);
                      tmd.fitModel(quant_method.getTransformationModel(), quant_method.getTransformationModelParams());
                      float calculated_feature_ratio = tmd.apply(ratio);
                      // check for less than zero
                      if (calculated_feature_ratio < 0.0) calculated_feature_ratio = 0.0;
                      y_fit_data.push_back(calculated_feature_ratio);
                      calibrators_conc_min = std::min(ratio, calibrators_conc_min);
                      calibrators_conc_max = std::max(ratio, calibrators_conc_max);
                      calibrators_feature_min = std::min(calculated_feature_ratio, calibrators_feature_min);
                      calibrators_feature_max = std::max(calculated_feature_ratio, calibrators_feature_max);
                    }
                    calibrators_conc_fit_data.push_back(stand_concs_map.at(quant_method.getComponentName()).first);
                    calibrators_feature_fit_data.push_back(y_fit_data);

                    // Extract out the points used to make the line of best fit in `ComponentsToConcentrations`
                    std::vector<float> x_raw_data, y_raw_data;
                    OpenMS::AbsoluteQuantitation absQuant;
                    for (const auto& point : sequence_segment.getComponentsToConcentrations().at(quant_method.getComponentName())) {
                      x_raw_data.push_back(float(point.actual_concentration / point.IS_actual_concentration / point.dilution_factor));
                      float y_datum = absQuant.calculateRatio(point.feature, point.IS_feature, quant_method.getFeatureName());
                      y_raw_data.push_back(y_datum);
                      calibrators_feature_min = std::min(y_datum, calibrators_feature_min);
                      calibrators_feature_max = std::max(y_datum, calibrators_feature_max);
                    }
                    calibrators_conc_raw_data.push_back(x_raw_data);
                    calibrators_feature_raw_data.push_back(y_raw_data);
                    calibrators_series_names.push_back(quant_method.getComponentName());
                  }
                }
              }
            }
          }
          CalibratorsPlotWidget plot2d;
          plot2d.x_fit_data_ = calibrators_conc_fit_data;
          plot2d.y_fit_data_ = calibrators_feature_fit_data;
          plot2d.x_raw_data_ = calibrators_conc_raw_data;
          plot2d.y_raw_data_ = calibrators_feature_raw_data;
          plot2d.x_axis_title_ = calibrators_x_axis_title;
          plot2d.y_axis_title_ = calibrators_y_axis_title;
          plot2d.series_names_ = calibrators_series_names;
          plot2d.plot_title_ = "CalibratorsMainWindow";
          plot2d.x_min_ = calibrators_conc_min;
          plot2d.x_max_ = calibrators_conc_max;
          plot2d.y_min_ = calibrators_feature_min;
          plot2d.y_max_ = calibrators_feature_max;
          plot2d.plot_width_ = win_size_and_pos.bottom_and_top_window_x_size_;
          plot2d.plot_height_ = win_size_and_pos.top_window_y_size_;
          plot2d.draw();

          ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
      }
      ImGui::End();
      ImGui::PopStyleVar();
    }

    // Bottom window
    if (show_bottom_window_)
    {
      ImGui::SetNextWindowPos(ImVec2(win_size_and_pos.bottom_and_top_window_x_pos_, win_size_and_pos.bottom_window_y_pos_));
      ImGui::SetNextWindowSize(ImVec2(win_size_and_pos.bottom_and_top_window_x_size_, win_size_and_pos.bottom_window_y_size_));
      ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0);
      const ImGuiWindowFlags bottom_window_flags =
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoFocusOnAppearing;
      ImGui::Begin("Bottom window", NULL, bottom_window_flags);
      if (ImGui::BeginTabBar("Bottom window tab bar", ImGuiTabBarFlags_Reorderable))
      {
        if (show_info_ && ImGui::BeginTabItem("Info", &show_info_))
        {
          ImGui::BeginChild("Info child");
          quickInfoText_.draw();
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
          // TODO: this does not display correctly
          // BUG: AUT-176
          for (const plog::util::nstring& s : appender_.getMessageList(severity))
          {
            ImGui::Text("%s", s.c_str());
          }
          ImGui::EndChild();
          ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
      }
      ImGui::End();
      ImGui::PopStyleVar();
    }
    }

    // Rendering
    ImGui::Render();
    glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
    glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
    glClear(GL_COLOR_BUFFER_BIT);
    //glUseProgram(0); // You may want this if using this code in an OpenGL 3+ context where shaders may be bound
    ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());
    SDL_GL_SwapWindow(window);
  }

  // Cleanup
  ImGui_ImplOpenGL2_Shutdown();
  ImGui_ImplSDL2_Shutdown();
  ImGui::DestroyContext();

  SDL_GL_DeleteContext(gl_context);
  SDL_DestroyWindow(window);
  SDL_Quit();

  return 0;
}

// copied from imgui_demo.cpp
// Helper to display a little (?) mark which shows a tooltip when hovered.
void HelpMarker(const char* desc)
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

void initializeDataDirs(ApplicationHandler& application_handler)
{
  initializeDataDir(application_handler, "mzML", application_handler.mzML_dir_, "mzML");
  initializeDataDir(application_handler, "INPUT features", application_handler.features_in_dir_, "features");
  initializeDataDir(application_handler, "OUTPUT features", application_handler.features_out_dir_, "features");
}

void initializeDataDir(
  ApplicationHandler& application_handler,
  const std::string& label,
  std::string& data_dir_member,
  const std::string& default_dir
)
{
  if (data_dir_member.size()) {
    return;
  }
  data_dir_member = application_handler.main_dir_ + "/" + default_dir;
  LOGN << "\n\nGenerated path for '" << label << "':\t" << data_dir_member;
}
