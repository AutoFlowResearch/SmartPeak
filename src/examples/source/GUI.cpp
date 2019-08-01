// Example application for SDL2 + OpenGL
// **DO NOT USE THIS CODE IF YOUR CODE/ENGINE IS USING MODERN OPENGL (SHADERS, VBO, VAO, etc.)**
// **Prefer using the code in the sdl_opengl3_example/ folder**
// See imgui_impl_sdl.cpp for details.

#include <stdio.h>
#include <chrono>
#include <string>
#include <vector>
#include <SmartPeak/core/AppState.h>
#include <SmartPeak/core/AppStateProcessor.h>
#include <SmartPeak/ui/FilePicker.h>
#include <SmartPeak/ui/GuiAppender.h>
#include <SmartPeak/ui/Report.h>
#include <SmartPeak/ui/Workflow.h>
#include <plog/Log.h>
#include <plog/Appenders/ConsoleAppender.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include <imgui.h>
#include <examples/imgui_impl_sdl.h>
#include <examples/imgui_impl_opengl2.h>

using namespace SmartPeak;

void HelpMarker(const char* desc);

void initializeDataDirs(AppState& state);

void initializeDataDir(
  AppState& state,
  const std::string& label,
  std::string& data_dir_member,
  const std::string& default_dir
);

int main(int argc, char **argv)
  // `int argc, char **argv` are required on Win to link against the proper SDL2/OpenGL implementation
{
  std::string quickInfoText_;
  bool show_top_window_ = false;
  bool show_bottom_window_ = false;

  // View: Bottom window
  bool show_info_ = true;
  bool show_log_ = true;
  bool show_about_ = false;

  // View: Explorer pane
  bool show_sequence_explorer = false;
  bool show_transitions_explorer = false;
  bool show_experiment_explorer = false;
  bool show_features_explorer = false;

  // View: Top window
  bool show_sequence_table = false;
  bool show_transitions_table = false;
  bool show_workflow_table = false;
  bool show_parameters_table = false;
  bool show_quant_method_table = false;
  bool show_stds_concs_table = false;
  bool show_comp_filters_table = false;
  bool show_comp_group_filters_table = false;
  bool show_comp_qcs_table = false;
  bool show_comp_group_qcs_table = false;
  bool show_feature_plot = false;
  bool show_line_plot = false;
  bool show_heatmap_plot = false;
  bool show_feature_summary_table = false;
  bool show_sequence_summary_table = false;

  AppState state_;
  GuiAppender appender_;

  // widgets
  FilePicker file_picker_;
  Report     report_;
  Workflow   workflow_;
  report_.setState(state_);
  workflow_.setState(state_);
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
    if (show_about_)
    {
      if (ImGui::Begin("begin_about", &show_about_))
      {
        ImGui::Text("About SmartPeak");
        ImGui::Text("SmartPeak %s", "1.0"); //TODO: define version function
        ImGui::Separator();
        ImGui::Text("By the hardworking SmartPeak developers.");
      }
      ImGui::End();
    }

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

    if (ImGui::BeginMainMenuBar())
    {
      if (ImGui::BeginMenu("File"))
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
          if (ImGui::MenuItem("Reference data")) {}
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
        ImGui::EndMenu();
      }
      if (ImGui::BeginMenu("Edit"))
      {
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
        ImGui::EndMenu();
      }
      if (ImGui::BeginMenu("View"))
      {
        ImGui::MenuItem("Explorer window", NULL, false, false);
        if (ImGui::MenuItem("Sequence", NULL, &show_sequence_explorer)) {}
        if (ImGui::MenuItem("Transitions", NULL, &show_transitions_explorer)) {}
        if (ImGui::MenuItem("Experiment", NULL, &show_experiment_explorer)) {}
        if (ImGui::MenuItem("Features", NULL, &show_features_explorer)) {}  // including metadata?
        ImGui::Separator(); // Primary input
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
        ImGui::MenuItem("Info window", NULL, false, false);
        if (ImGui::MenuItem("Info", NULL, &show_info_)) {}
        if (ImGui::MenuItem("Log", NULL, &show_log_)) {}
        ImGui::EndMenu();
      }
      if (ImGui::BeginMenu("Actions"))
      {
        if (ImGui::MenuItem("Run command"))
        {
          initializeDataDirs(state_);
          // do the rest
        }
        if (ImGui::MenuItem("Run workflow"))
        {
          initializeDataDirs(state_);
          ProcessCommands processCommands(state_);
          // TODO: IO operation -> use another thread
          processCommands(state_.commands_);
          LOGN << "\n\nWorkflow completed.\n";
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
        ImGui::EndMenu();
      }
      if (ImGui::BeginMenu("Help"))
      {
        ImGui::MenuItem("About", NULL, &show_about_);
        if (ImGui::MenuItem("Documentation")) {}
        if (ImGui::MenuItem("Version")) {}
        ImGui::EndMenu();
      }
      ImGui::EndMainMenuBar();
    }

    show_bottom_window_ = show_info_ || show_log_;

    // Bottom window
    if (show_bottom_window_)
    {
      const float bottom_window_y_pos = show_top_window_ ? 400 : 18;
      ImGui::SetNextWindowPos(ImVec2(0, bottom_window_y_pos));
      ImGui::SetNextWindowSize(ImVec2(io.DisplaySize.x, io.DisplaySize.y - bottom_window_y_pos));
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

void initializeDataDirs(AppState& state)
{
  initializeDataDir(state, "mzML", state.mzML_dir_, "mzML");
  initializeDataDir(state, "INPUT features", state.features_in_dir_, "features");
  initializeDataDir(state, "OUTPUT features", state.features_out_dir_, "features");
}

void initializeDataDir(
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
