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
// $Maintainer: Douglas McCloskey, Ahmed Khalil, Bertrand Boudaud $
// $Authors: Douglas McCloskey, Pasquale Domenico Colaianni $
// --------------------------------------------------------------------------

#include <stdio.h>
#include <chrono>
#include <string>
#include <vector>
#include <SmartPeak/core/ApplicationHandler.h>
#include <SmartPeak/core/ApplicationProcessor.h>
#include <SmartPeak/core/WorkflowManager.h>
#include <SmartPeak/core/SessionHandler.h>
#include <SmartPeak/core/Utilities.h>
#include <SmartPeak/ui/FilePicker.h>
#include <SmartPeak/ui/GuiAppender.h>
#include <SmartPeak/ui/Heatmap2DWidget.h>
#include <SmartPeak/ui/CalibratorsPlotWidget.h>
#include <SmartPeak/ui/ChromatogramPlotWidget.h>
#include <SmartPeak/ui/SpectraPlotWidget.h>
#include <SmartPeak/ui/ParametersTableWidget.h>
#include <SmartPeak/ui/Report.h>
#include <SmartPeak/ui/Workflow.h>
#include <SmartPeak/ui/StatisticsWidget.h>
#include <SmartPeak/ui/InfoWidget.h>
#include <SmartPeak/ui/WindowSizesAndPositions.h>
#include <plog/Log.h>
#include <plog/Appenders/ConsoleAppender.h>
#include <boost/filesystem.hpp>
#include <SDL.h>
#include <SDL_opengl.h>
#include <imgui.h>
#include <backends/imgui_impl_sdl.h>
#include <backends/imgui_impl_opengl2.h>
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

std::shared_ptr<ExplorerWidget>& getExplorerWidget(std::map<std::string, std::shared_ptr<ExplorerWidget>>& explorer_widgets, const std::string& id)
{
  if (explorer_widgets.count(id))
  {
    return explorer_widgets.at(id);
  }
  else
  {
    explorer_widgets[id] = std::make_shared<ExplorerWidget>(id);
    return explorer_widgets[id];
  }
};

std::shared_ptr<GenericTableWidget>& getGenericTableWidget(std::map<std::string, std::shared_ptr<GenericTableWidget>>& generic_table_widgets, const std::string& id)
{
  if (generic_table_widgets.count(id))
  {
    return generic_table_widgets.at(id);
  }
  else
  {
    generic_table_widgets[id] = std::make_shared<GenericTableWidget>(id);
    return generic_table_widgets[id];
  }
};


int main(int argc, char** argv)
// `int argc, char **argv` are required on Win to link against the proper SDL2/OpenGL implementation
{
  bool show_top_window_ = false;
  bool show_bottom_window_ = false;
  bool show_left_window_ = false;
  bool show_right_window_ = false;

  // to disable buttons, display info, and update the session cache
  bool workflow_is_done_ = true;
  bool file_loading_is_done_ = true;
  bool exceeding_plot_points_ = false;
  bool exceeding_table_size_ = false;
  bool ran_integrity_check_ = false;
  bool integrity_check_failed_ = false;
  bool update_session_cache_ = false;

  // View: Bottom window
  bool show_info_ = true;
  bool show_log_ = false;

  // View: left or right windows (i.e., Explorer pane)
  bool show_injection_explorer = true; // list of injection names with advanced options for filtering
  bool show_transitions_explorer = true; // list of transition names (e.g., subset of the features for non-targeted case) with advanced options for filtering
  bool show_features_explorer = false; // list of features with advanced options for filtering
  bool show_spectrum_explorer = false; // list of spectrum with advanced options for filtering

  // View: Top window
  bool show_sequence_table = false;
  bool show_transitions_table = false;
  bool show_spectrum_table = false;
  bool show_workflow_table = true;
  bool show_parameters_table = false;
  bool show_quant_method_table = false;
  bool show_stds_concs_table = false;
  bool show_comp_filters_table = false;
  bool show_comp_group_filters_table = false;
  bool show_comp_qcs_table = false;
  bool show_comp_group_qcs_table = false;
  bool show_comp_rsd_filters_table = false;
  bool show_comp_group_rsd_filters_table = false;
  bool show_comp_rsd_qcs_table = false;
  bool show_comp_group_rsd_qcs_table = false;
  bool show_comp_background_filters_table = false;
  bool show_comp_group_background_filters_table = false;
  bool show_comp_background_qcs_table = false;
  bool show_comp_group_background_qcs_table = false;
  bool show_comp_rsd_estimations_table = false;
  bool show_comp_group_rsd_estimations_table = false;
  bool show_comp_background_estimations_table = false;
  bool show_comp_group_background_estimations_table = false;
  bool show_feature_table = false; // table of all injections, features, and metavalues
  bool show_feature_pivot_table = false; // injection vs. feature for a particular metavalue
  bool show_chromatogram_line_plot = false; // time vs. intensity for the raw data and annotated feature
  bool show_spectra_line_plot = false; // time vs. intensity for the raw data and annotated feature
  bool show_feature_line_plot = false; // injection vs. metavalue for n selected features
  bool show_feature_heatmap_plot = false; // injection vs. feature for a particular metavalue
  bool show_calibrators_line_plot = false; // peak area/height ratio vs. concentration ratio
  bool show_satistics_widget = false;

  // Chromatogram display option
  bool chromatogram_initialized = false;
  std::unique_ptr<ChromatogramPlotWidget> chromatogram_plot_widget;

  std::map<std::string, std::shared_ptr<ExplorerWidget>> explorer_widgets;
  std::map<std::string, std::shared_ptr<GenericTableWidget>> generic_table_widgets;

  // Spectra display option
  bool spectra_initialized = false;
  std::unique_ptr<SpectraPlotWidget> spectra_plot_widget;

  // Popup modals
  bool popup_about_ = false;
  bool popup_run_workflow_ = false;
  bool popup_file_picker_ = false;

  ApplicationHandler application_handler_;
  SessionHandler session_handler_;
  WorkflowManager manager_;
  GuiAppender appender_;

  // widgets
  InfoWidget quickInfoText_;
  FilePicker file_picker_;
  Report     report_;
  Workflow   workflow_;
  StatisticsWidget  statistics_;

  std::unique_ptr<ParametersTableWidget> parameters_table_widget;
  report_.setApplicationHandler(application_handler_);
  workflow_.setApplicationHandler(application_handler_);
  statistics_.setApplicationHandler(application_handler_);
  quickInfoText_.setApplicationHandler(application_handler_);

  // Create log path
  const std::time_t t = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
  char filename[128];
  strftime(filename, 128, "smartpeak_log_%Y-%m-%d_%H-%M-%S.csv", std::localtime(&t));

  auto logfilepath = std::string{};
  auto logdirpath = std::string{};
  auto logdir_created = false;
  auto error_msg = std::string{};
  try
  {
    std::tie(logfilepath, logdir_created) = Utilities::getLogFilepath(filename);
    logdirpath = fs::path(logfilepath).parent_path().string();
  }
  catch (const std::runtime_error& re)
  {
    error_msg = re.what();
  }

  // Add .csv appender: 32 MiB per file, max. 100 log files
  plog::RollingFileAppender<plog::CsvFormatter>
    fileAppender(logfilepath.c_str(), 1024 * 1024 * 32, 100);

  // Add console appender, instead of only the file one
  plog::ConsoleAppender<plog::TxtFormatter> consoleAppender;

  // Init logger with all the appenders
  plog::init(plog::debug, &fileAppender)
    .addAppender(&consoleAppender)
    .addAppender(&appender_);

  // Log SmartPeak launch initiated:
  LOG_INFO << "Start SmartPeak version " << Utilities::getSmartPeakVersion();

  if (error_msg.empty())
  {
    if (logdir_created) { LOG_DEBUG << "Log directory created: " << logdirpath; }
    LOG_INFO << "Log file at: " << logfilepath;
  }
  else
  {
    // In this case it will only use console appender
    LOG_WARNING << error_msg;
  }

  // Setup SDL
  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0)
  {
    printf("Error: %s\n", SDL_GetError());
    return -1;
  }

  // Setup window
  auto smartpeak_window_title = static_cast<std::ostringstream&&>(
    std::ostringstream() << "SmartPeak v" << Utilities::getSmartPeakVersion()).str();
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
  SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
  SDL_DisplayMode current;
  SDL_GetCurrentDisplayMode(0, &current);
  SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
  SDL_Window* window = SDL_CreateWindow(smartpeak_window_title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, window_flags);
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

      if ((!workflow_is_done_) && manager_.isWorkflowDone())
      {
        manager_.updateApplicationHandler(application_handler_);
        quickInfoText_.setLastRunTime(manager_.getLastRunTime());
      }
      workflow_is_done_ = manager_.isWorkflowDone();
      file_loading_is_done_ = file_picker_.fileLoadingIsDone();

      // Make the quick info text
      quickInfoText_.setWorkflowDone(workflow_is_done_);
      quickInfoText_.setFileLoadingDone(file_loading_is_done_, file_picker_.errorLoadingFile());
      quickInfoText_.clearErrorMessages();
      if (exceeding_plot_points_) quickInfoText_.addErrorMessage("Plot rendering limit reached.  Not plotting all selected data.");
      if (exceeding_table_size_) quickInfoText_.addErrorMessage("Table rendering limit reached.  Not showing all selected data.");
      if (ran_integrity_check_ && integrity_check_failed_) quickInfoText_.addErrorMessage("Integrity check failed.  Check the `Information` log.");
      if (ran_integrity_check_ && !integrity_check_failed_) quickInfoText_.addErrorMessage("Integrity check passed.");

      // Session cache updates from e.g., single file imports
      if (file_loading_is_done_ && file_picker_.fileWasLoaded() && update_session_cache_) {
        if (file_picker_.getProcessorName() == "LoadSequenceParameters") {
          session_handler_.sequence_table_headers.resize(0);
          session_handler_.sequence_table_body.resize(0, 0);
        }
        else if (file_picker_.getProcessorName() == "LoadSequenceTransitions") {
          session_handler_.transitions_table_headers.resize(0);
          session_handler_.transitions_table_body.resize(0, 0);
        }
        else if (file_picker_.getProcessorName() == "LoadSequenceSegmentQuantitationMethods") {
          session_handler_.quant_method_table_headers.resize(0);
          session_handler_.quant_method_table_body.resize(0, 0);
        }
        else if (file_picker_.getProcessorName() == "LoadSequenceSegmentStandardsConcentrations") {
          session_handler_.stds_concs_table_headers.resize(0);
          session_handler_.stds_concs_table_body.resize(0, 0);
        }
        else if (file_picker_.getProcessorName() == "LoadSequenceSegmentFeatureFilterComponents") {
          session_handler_.comp_filters_table_headers.resize(0);
          session_handler_.comp_filters_table_body.resize(0, 0);
        }
        else if (file_picker_.getProcessorName() == "LoadSequenceSegmentFeatureFilterComponentGroups") {
          session_handler_.comp_group_filters_table_headers.resize(0);
          session_handler_.comp_group_filters_table_body.resize(0, 0);
        }
        else if (file_picker_.getProcessorName() == "LoadSequenceSegmentFeatureQCComponents") {
          session_handler_.comp_qcs_table_headers.resize(0);
          session_handler_.comp_qcs_table_body.resize(0, 0);
        }
        else if (file_picker_.getProcessorName() == "LoadSequenceSegmentFeatureQCComponentGroups") {
          session_handler_.comp_group_qcs_table_headers.resize(0);
          session_handler_.comp_group_qcs_table_body.resize(0, 0);
        }
        update_session_cache_ = false;
      }

    // Pop-ups
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
        BuildCommandsFromNames buildCommandsFromNames(application_handler_);
        buildCommandsFromNames.names_ = application_handler_.sequenceHandler_.getWorkflow();
        if (!buildCommandsFromNames.process()) {
          LOGE << "Failed to create Commands, aborting.";
        }
        else {
          for (auto& cmd : buildCommandsFromNames.commands_)
          {
            for (auto& p : cmd.dynamic_filenames)
            {
              Filenames::updateDefaultDynamicFilenames(
                application_handler_.mzML_dir_,
                application_handler_.features_in_dir_,
                application_handler_.features_out_dir_,
                p.second
              );
            }
          }
          const std::set<std::string> injection_names = session_handler_.getSelectInjectionNamesWorkflow(application_handler_.sequenceHandler_);
          const std::set<std::string> sequence_segment_names = session_handler_.getSelectSequenceSegmentNamesWorkflow(application_handler_.sequenceHandler_);
          const std::set<std::string> sample_group_names = session_handler_.getSelectSampleGroupNamesWorkflow(application_handler_.sequenceHandler_);
          manager_.addWorkflow(application_handler_, injection_names, sequence_segment_names, sample_group_names, buildCommandsFromNames.commands_);
        }
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
      ImGui::OpenPopup("About");
      popup_about_ = false;
    }
    if (ImGui::BeginPopupModal("About", NULL, ImGuiWindowFlags_AlwaysAutoResize))
    {
      ImGui::Text("Fast, accurate, and automated processing of CE-, GC- and LC-MS(/MS) and HPLC data.");
      ImGui::Text("SmartPeak %s", Utilities::getSmartPeakVersion().c_str());
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
    if (ImGui::BeginMainMenuBar())
    {
      if (ImGui::BeginMenu("File"))
      {
        ImGui::MenuItem("Session", NULL, false, false);
        //if (ImGui::MenuItem("New Session", NULL, false, false))
        //{
        //  //TODO: Session (see AUT-280)
        //}
        //if (ImGui::MenuItem("Load Session", NULL, false, false))
        //{
        //  //TODO: Session (see AUT-280)
        //}
        if (ImGui::MenuItem("Load session from sequence", NULL, false, workflow_is_done_ && file_loading_is_done_)) {
          static LoadSessionFromSequence processor(application_handler_);
          file_picker_.setProcessor(processor);
          popup_file_picker_ = true;
        }
        //if (ImGui::MenuItem("Save Session", NULL, false, false))
        //{
        //  //TODO: Session (see AUT-280)
        //}
        //if (ImGui::MenuItem("Save Session As...", NULL, false, false))
        //{
        //  //TODO: Session (see AUT-280)
        //}
        ImGui::Separator();
        ImGui::MenuItem("Text file", NULL, false, false);
        if (ImGui::BeginMenu("Import File"))
        {
          if (ImGui::MenuItem("Transitions")) {
            static LoadSequenceTransitions processor(application_handler_);
            file_picker_.setProcessor(processor);
            popup_file_picker_ = true;
            update_session_cache_ = true;
          }
          if (ImGui::MenuItem("Parameters")) {
            static LoadSequenceParameters processor(application_handler_);
            file_picker_.setProcessor(processor);
            popup_file_picker_ = true;
            update_session_cache_ = true;
          }
          if (ImGui::MenuItem("Workflow", NULL, false, workflow_is_done_)) {
            static LoadSequenceWorkflow processor(application_handler_);
            file_picker_.setProcessor(processor);
            popup_file_picker_ = true;
            update_session_cache_ = true;
          }
          if (ImGui::MenuItem("Reference data")) {
            static LoadSequenceValidationData processor(application_handler_);
            file_picker_.setProcessor(processor);
            popup_file_picker_ = true;
            update_session_cache_ = true;
          }
          if (ImGui::MenuItem("Quant Method")) {
            static LoadSequenceSegmentQuantitationMethods processor(application_handler_);
            file_picker_.setProcessor(processor);
            popup_file_picker_ = true;
            update_session_cache_ = true;
          }
          if (ImGui::MenuItem("Standards Conc")) {
            static LoadSequenceSegmentStandardsConcentrations processor(application_handler_);
            file_picker_.setProcessor(processor);
            popup_file_picker_ = true;
            update_session_cache_ = true;
          }
          if (ImGui::MenuItem("Comp Filters")) {
            static LoadSequenceSegmentFeatureFilterComponents processor(application_handler_);
            file_picker_.setProcessor(processor);
            popup_file_picker_ = true;
            update_session_cache_ = true;
          }
          if (ImGui::MenuItem("Comp Group Filters")) {
            static LoadSequenceSegmentFeatureFilterComponentGroups processor(application_handler_);
            file_picker_.setProcessor(processor);
            popup_file_picker_ = true;
            update_session_cache_ = true;
          }
          if (ImGui::MenuItem("Comp QCs")) {
            static LoadSequenceSegmentFeatureQCComponents processor(application_handler_);
            file_picker_.setProcessor(processor);
            popup_file_picker_ = true;
            update_session_cache_ = true;
          }
          if (ImGui::MenuItem("Comp Group QCs")) {
            static LoadSequenceSegmentFeatureQCComponentGroups processor(application_handler_);
            file_picker_.setProcessor(processor);
            popup_file_picker_ = true;
            update_session_cache_ = true;
          }
          if (ImGui::MenuItem("Comp %RSD Filters")) {
            static LoadSequenceSegmentFeatureRSDFilterComponents processor(application_handler_);
            file_picker_.setProcessor(processor);
            popup_file_picker_ = true;
            update_session_cache_ = true;
          }
          if (ImGui::MenuItem("Comp Group %RSD Filters")) {
            static LoadSequenceSegmentFeatureRSDFilterComponentGroups processor(application_handler_);
            file_picker_.setProcessor(processor);
            popup_file_picker_ = true;
            update_session_cache_ = true;
          }
          if (ImGui::MenuItem("Comp %RSD QCs")) {
            static LoadSequenceSegmentFeatureRSDQCComponents processor(application_handler_);
            file_picker_.setProcessor(processor);
            popup_file_picker_ = true;
            update_session_cache_ = true;
          }
          if (ImGui::MenuItem("Comp Group %RSD QCs")) {
            static LoadSequenceSegmentFeatureRSDQCComponentGroups processor(application_handler_);
            file_picker_.setProcessor(processor);
            popup_file_picker_ = true;
            update_session_cache_ = true;
          }
          if (ImGui::MenuItem("Comp %Background Filters")) {
            static LoadSequenceSegmentFeatureBackgroundFilterComponents processor(application_handler_);
            file_picker_.setProcessor(processor);
            popup_file_picker_ = true;
            update_session_cache_ = true;
          }
          if (ImGui::MenuItem("Comp Group %Background Filters")) {
            static LoadSequenceSegmentFeatureBackgroundFilterComponentGroups processor(application_handler_);
            file_picker_.setProcessor(processor);
            popup_file_picker_ = true;
            update_session_cache_ = true;
          }
          if (ImGui::MenuItem("Comp %Background QCs")) {
            static LoadSequenceSegmentFeatureBackgroundQCComponents processor(application_handler_);
            file_picker_.setProcessor(processor);
            popup_file_picker_ = true;
            update_session_cache_ = true;
          }
          if (ImGui::MenuItem("Comp Group %Background QCs")) {
            static LoadSequenceSegmentFeatureBackgroundQCComponentGroups processor(application_handler_);
            file_picker_.setProcessor(processor);
            popup_file_picker_ = true;
            update_session_cache_ = true;
          }
          ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Export File"))
        {
          //if (ImGui::MenuItem("Sequence")) {} // TODO: updated sequence file
          //if (ImGui::MenuItem("Transitions")) {} // TODO: updated transitions file
          //if (ImGui::MenuItem("Parameters")) {} // TODO: updated parameters file
          //if (ImGui::MenuItem("Standards Conc")) {} // TODO: updated standards concentration file
          if (ImGui::MenuItem("Workflow")) {
            static StoreSequenceWorkflow processor(application_handler_);
            file_picker_.setProcessor(processor);
            popup_file_picker_ = true;
          }
          if (ImGui::MenuItem("Sequence Analyst")) {
            static StoreSequenceFileAnalyst processor(application_handler_);
            processor.process();
            // TODO: modal to allow for changing the filename and directory of where to write the file
          }
          if (ImGui::MenuItem("Sequence MassHunter")) {
            static StoreSequenceFileMasshunter processor(application_handler_);
            processor.process();
            // TODO: modal to allow for changing the filename and directory of where to write the file
          }
          if (ImGui::MenuItem("Sequence Xcalibur")) {
            static StoreSequenceFileXcalibur processor(application_handler_);
            processor.process();
            // TODO: modal to allow for changing the filename and directory of where to write the file
          }
          ImGui::EndMenu();
        }
        ImGui::EndMenu();
      }
      if (ImGui::BeginMenu("Edit"))
      {
        ImGui::MenuItem("Settings", NULL, false, false);
        if (ImGui::MenuItem("Tables", NULL, false, false)) {} // TODO: modal of settings
        if (ImGui::MenuItem("Plots", NULL, false, false)) {} // TODO: modal of settings
        if (ImGui::MenuItem("Explorer", NULL, false, false)) {} // TODO: modal of settings
        if (ImGui::MenuItem("Parameters", NULL, false, false)) {} // TODO: modal of settings
        ImGui::EndMenu();
      }
      if (ImGui::BeginMenu("View"))
      {
        ImGui::MenuItem("Explorer window", NULL, false, false);
        if (ImGui::MenuItem("Injections", NULL, &show_injection_explorer)) {} // TODO: search field
        if (ImGui::MenuItem("Transitions", NULL, &show_transitions_explorer)) {} // TODO: search field
        if (ImGui::MenuItem("Features", NULL, &show_features_explorer)) {}
        if (ImGui::MenuItem("Scans", NULL, &show_spectrum_explorer)) {}
        ImGui::Separator(); // Primary input
        ImGui::MenuItem("Main window (Tables)", NULL, false, false);
        if (ImGui::MenuItem("Sequence", NULL, &show_sequence_table)) {}
        if (ImGui::MenuItem("Transitions", NULL, &show_transitions_table)) {}
        if (ImGui::MenuItem("Scans", NULL, &show_spectrum_table)) {}
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
          if (ImGui::MenuItem("Comp RSD Filters", NULL, &show_comp_rsd_filters_table)) {}
          if (ImGui::MenuItem("Comp Group RSD Filters", NULL, &show_comp_group_rsd_filters_table)) {}
          if (ImGui::MenuItem("Comp RSD QCs", NULL, &show_comp_rsd_qcs_table)) {}
          if (ImGui::MenuItem("Comp Group RSD QCs", NULL, &show_comp_group_rsd_qcs_table)) {}
          if (ImGui::MenuItem("Comp Background Filters", NULL, &show_comp_background_filters_table)) {}
          if (ImGui::MenuItem("Comp Group Background Filters", NULL, &show_comp_group_background_filters_table)) {}
          if (ImGui::MenuItem("Comp Background QCs", NULL, &show_comp_background_qcs_table)) {}
          if (ImGui::MenuItem("Comp Group Background QCs", NULL, &show_comp_group_background_qcs_table)) {}
          if (ImGui::MenuItem("Comp RSD Estimations", NULL, &show_comp_rsd_estimations_table)) {}
          if (ImGui::MenuItem("Comp Group RSD Estimations", NULL, &show_comp_group_rsd_estimations_table)) {}
          if (ImGui::MenuItem("Comp Background Estimations", NULL, &show_comp_background_estimations_table)) {}
          if (ImGui::MenuItem("Comp Group Background Estimations", NULL, &show_comp_group_background_estimations_table)) {}
          // TODO: missing workflow setting tables...
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
        if (ImGui::MenuItem("Statistics", NULL, &show_satistics_widget)) {}
        ImGui::MenuItem("Info window", NULL, false, false);
        if (ImGui::MenuItem("Info", NULL, &show_info_)) {}
        if (ImGui::MenuItem("Log", NULL, &show_log_)) {}
        ImGui::EndMenu();
      }
      if (ImGui::BeginMenu("Actions"))
      {
        if (ImGui::MenuItem("Run workflow"))
        {
          if (application_handler_.sequenceHandler_.getWorkflow().empty())
          {
            LOGW << "Workflow has no steps to run. Please set the workflow's steps.";
          }
          initializeDataDirs(application_handler_);
          popup_run_workflow_ = true;
        }
        if (ImGui::BeginMenu("Integrity checks"))
        {
          if (ImGui::MenuItem("Sample consistency")) {
            ran_integrity_check_ = true;
            integrity_check_failed_ = !InputDataValidation::sampleNamesAreConsistent(application_handler_.sequenceHandler_);
          }
          if (ImGui::MenuItem("Comp consistency")) {
            ran_integrity_check_ = true;
            integrity_check_failed_ = !InputDataValidation::componentNamesAreConsistent(application_handler_.sequenceHandler_);
          }
          if (ImGui::MenuItem("Comp Group consistency")) {
            ran_integrity_check_ = true;
            integrity_check_failed_ = !InputDataValidation::componentNameGroupsAreConsistent(application_handler_.sequenceHandler_);
          }
          if (ImGui::MenuItem("IS consistency")) {
            ran_integrity_check_ = true;
            integrity_check_failed_ = !InputDataValidation::heavyComponentsAreConsistent(application_handler_.sequenceHandler_);
          }
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
        if (ImGui::MenuItem("Documentation")) {
          // TODO: Render the SmartPeak documentation (See AUT-178)
        }
        ImGui::EndMenu();
      }
      ImGui::EndMainMenuBar();
    }

    show_top_window_ = show_sequence_table || show_transitions_table || show_spectrum_table || show_workflow_table || show_parameters_table
      || show_quant_method_table || show_stds_concs_table
      || show_comp_filters_table || show_comp_group_filters_table || show_comp_qcs_table || show_comp_group_qcs_table
      || show_comp_rsd_filters_table || show_comp_group_rsd_filters_table || show_comp_rsd_qcs_table || show_comp_group_rsd_qcs_table
      || show_comp_background_filters_table || show_comp_group_background_filters_table || show_comp_background_qcs_table || show_comp_group_background_qcs_table
      || show_comp_rsd_estimations_table || show_comp_group_rsd_estimations_table || show_comp_background_estimations_table || show_comp_group_background_estimations_table
      || show_feature_table || show_feature_pivot_table
      || show_chromatogram_line_plot || show_spectra_line_plot || show_feature_line_plot || show_feature_heatmap_plot || show_calibrators_line_plot;
    show_bottom_window_ = show_info_ || show_log_;
    show_left_window_ = show_injection_explorer || show_transitions_explorer || show_features_explorer || show_spectrum_explorer;
    win_size_and_pos.setWindowSizesAndPositions(show_top_window_, show_bottom_window_, show_left_window_, show_right_window_);

    // Left window
    if (show_left_window_) {
      ImGui::SetNextWindowPos(ImVec2(win_size_and_pos.left_window_x_pos_, win_size_and_pos.left_and_right_window_y_pos_));
      ImGui::SetNextWindowSize(ImVec2(win_size_and_pos.left_window_x_size_, win_size_and_pos.left_and_right_window_y_size_));
      ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0);
      const ImGuiWindowFlags left_window_flags =
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoFocusOnAppearing;
      ImGui::Begin("Left window", NULL, left_window_flags);
      if (ImGui::BeginTabBar("Left window tab bar", ImGuiTabBarFlags_Reorderable))
      {
        if (show_injection_explorer && ImGui::BeginTabItem("Injections", &show_injection_explorer))
        {
          session_handler_.setMinimalDataAndFilters(application_handler_.sequenceHandler_);
          auto widget = getExplorerWidget(explorer_widgets, "InjectionsExplorerWindow");
          /* these calls can be replaced by one method call, or, the widget uses session handler to get the data */
          /* to be more clean, the checkbox should be actually not be in session handlers but in the widget */
          widget->headers_ = session_handler_.getInjectionExplorerHeader();
          widget->columns_ = session_handler_.getInjectionExplorerBody();
          widget->checked_rows_ = session_handler_.injection_explorer_checked_rows;
          widget->checkbox_headers_ = session_handler_.injection_explorer_checkbox_headers;
          widget->checkbox_columns_ = &session_handler_.injection_explorer_checkbox_body;
          //show_chromatogram_line_plot ? widget->active_plot_ = "Chromatograms" : widget->active_plot_ = "";
          widget->draw();
          ImGui::EndTabItem();
        }
        if (show_transitions_explorer && ImGui::BeginTabItem("Transitions", &show_transitions_explorer))
        {
          session_handler_.setMinimalDataAndFilters(application_handler_.sequenceHandler_);
          auto widget = getExplorerWidget(explorer_widgets, "TransitionsExplorerWindow");
          widget->headers_ = session_handler_.getTransitionExplorerHeader();
          widget->columns_ = session_handler_.getTransitionExplorerBody();
          widget->checked_rows_ = session_handler_.transition_explorer_checked_rows;
          widget->checkbox_headers_ = session_handler_.transition_explorer_checkbox_headers;
          widget->checkbox_columns_ = &session_handler_.transition_explorer_checkbox_body;
          //show_feature_line_plot ? widget->active_plot_ = "Features (line)" : widget->active_plot_ = "";
          widget->draw();
          ImGui::EndTabItem();
        }
        if (show_features_explorer && ImGui::BeginTabItem("Features", &show_features_explorer))
        {
          session_handler_.setFeatureExplorer();
          auto widget = getExplorerWidget(explorer_widgets, "FeaturesExplorerWindow");
          widget->headers_ = session_handler_.feature_explorer_headers;
          widget->columns_ = session_handler_.feature_explorer_body;
          widget->checked_rows_ = session_handler_.feature_explorer_checked_rows;
          widget->checkbox_headers_ = session_handler_.feature_explorer_checkbox_headers;
          widget->checkbox_columns_ = &session_handler_.feature_explorer_checkbox_body;
          widget->draw();
          ImGui::EndTabItem();
        }
        if (show_spectrum_explorer && ImGui::BeginTabItem("Spectrum", &show_spectrum_explorer))
        {
          session_handler_.setMinimalDataAndFilters(application_handler_.sequenceHandler_);
          auto widget = getExplorerWidget(explorer_widgets, "SpectrumExplorerWindow");
          widget->headers_ = session_handler_.getSpectrumExplorerHeader();
          widget->columns_ = session_handler_.getSpectrumExplorerBody();
          widget->checked_rows_ = session_handler_.spectrum_explorer_checked_rows;
          widget->checkbox_headers_ = session_handler_.spectrum_explorer_checkbox_headers;
          widget->checkbox_columns_ = &session_handler_.spectrum_explorer_checkbox_body;
          widget->draw();
          ImGui::EndTabItem();
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
      const ImGuiWindowFlags top_window_flags =
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoFocusOnAppearing;
      ImGui::Begin("Top window", NULL, top_window_flags);
      if (ImGui::BeginTabBar("Top window tab bar", ImGuiTabBarFlags_Reorderable))
      {
        if (show_sequence_table && ImGui::BeginTabItem("Sequence", &show_sequence_table))
        {
          session_handler_.setMinimalDataAndFilters(application_handler_.sequenceHandler_);
          Eigen::Tensor<bool, 1> table_filters = session_handler_.getSequenceTableFilters();
          auto widget = getGenericTableWidget(generic_table_widgets, "SequenceMainWindow");
          widget->headers_ = session_handler_.sequence_table_headers;
          widget->columns_ = session_handler_.sequence_table_body;
          widget->checked_rows_ = table_filters;
          widget->draw();
          ImGui::EndTabItem();
        }
        if (show_transitions_table && ImGui::BeginTabItem("Transitions", &show_transitions_table))
        {
          session_handler_.setMinimalDataAndFilters(application_handler_.sequenceHandler_);
          Eigen::Tensor<bool, 1> table_filters = session_handler_.getTransitionsTableFilters();
          auto widget = getGenericTableWidget(generic_table_widgets, "TransitionsMainWindow");
          widget->headers_ = session_handler_.transitions_table_headers;
          widget->columns_ = session_handler_.transitions_table_body;
          widget->checked_rows_ = table_filters;
          widget->draw();
          ImGui::EndTabItem();
        }
        if (show_spectrum_table && ImGui::BeginTabItem("Spectrum", &show_spectrum_table))
        {
          session_handler_.setMinimalDataAndFilters(application_handler_.sequenceHandler_);
          Eigen::Tensor<bool, 1> table_filters = session_handler_.getSpectrumTableFilters();
          auto widget = getGenericTableWidget(generic_table_widgets, "SpectrumMainWindow");
          widget->headers_ = session_handler_.spectrum_table_headers;
          widget->columns_ = session_handler_.spectrum_table_body;
          widget->checked_rows_ = table_filters;
          widget->draw();
          ImGui::EndTabItem();
        }
        if (show_workflow_table && ImGui::BeginTabItem("Workflow", &show_workflow_table))
        {
          workflow_.setEditable(workflow_is_done_);
          workflow_.draw();
          ImGui::EndTabItem();
        }
        if (show_parameters_table && ImGui::BeginTabItem("Parameters", &show_parameters_table))
        {
          if (!parameters_table_widget)
          {
            parameters_table_widget = std::make_unique<ParametersTableWidget>(
              session_handler_,
              application_handler_,
              "ParametersMainWindow");
          }
          parameters_table_widget->draw();
          ImGui::EndTabItem();
        }
        if (show_quant_method_table && ImGui::BeginTabItem("Quantitation Method", &show_quant_method_table))
        {
          session_handler_.setMinimalDataAndFilters(application_handler_.sequenceHandler_);
          session_handler_.setQuantMethodTable(application_handler_.sequenceHandler_);
          Eigen::Tensor<bool, 1> table_filters = session_handler_.getQuantMethodsTableFilters();
          auto widget = getGenericTableWidget(generic_table_widgets, "QuantMethodMainWindow");
          widget->headers_ = session_handler_.quant_method_table_headers;
          widget->columns_ = session_handler_.quant_method_table_body;
          widget->checked_rows_ = table_filters;
          widget->draw();
          ImGui::EndTabItem();
        }
        if (show_stds_concs_table && ImGui::BeginTabItem("Standards Concentrations", &show_stds_concs_table))
        {
          session_handler_.setMinimalDataAndFilters(application_handler_.sequenceHandler_);
          session_handler_.setStdsConcsTable(application_handler_.sequenceHandler_);
          auto widget = getGenericTableWidget(generic_table_widgets, "StdsConcsMainWindow");
          widget->headers_ = session_handler_.stds_concs_table_headers;
          widget->columns_ = session_handler_.stds_concs_table_body;
          widget->checked_rows_ = Eigen::Tensor<bool, 1>();
          widget->draw();
          ImGui::EndTabItem();
        }
        if (show_comp_filters_table && ImGui::BeginTabItem("Component Filters", &show_comp_filters_table))
        {
          session_handler_.setMinimalDataAndFilters(application_handler_.sequenceHandler_);
          session_handler_.setComponentFiltersTable(application_handler_.sequenceHandler_);
          Eigen::Tensor<bool, 1> table_filters = session_handler_.getComponentFiltersTableFilters();
          auto widget = getGenericTableWidget(generic_table_widgets, "CompFiltersMainWindow");
          widget->headers_ = session_handler_.comp_filters_table_headers;
          widget->columns_ = session_handler_.comp_filters_table_body;
          widget->checked_rows_ = table_filters;
          widget->draw();
          ImGui::EndTabItem();
        }
        if (show_comp_group_filters_table && ImGui::BeginTabItem("Component Group Filters", &show_comp_group_filters_table))
        {
          session_handler_.setMinimalDataAndFilters(application_handler_.sequenceHandler_);
          session_handler_.setComponentGroupFiltersTable(application_handler_.sequenceHandler_);
          Eigen::Tensor<bool, 1> table_filters = session_handler_.getComponentGroupFiltersTableFilters();
          auto widget = getGenericTableWidget(generic_table_widgets, "CompGroupFiltersMainWindow");
          widget->headers_ = session_handler_.comp_group_filters_table_headers;
          widget->columns_ = session_handler_.comp_group_filters_table_body;
          widget->checked_rows_ = table_filters;
          widget->draw();
          ImGui::EndTabItem();
        }
        if (show_comp_qcs_table && ImGui::BeginTabItem("Component QCs", &show_comp_qcs_table))
        {
          session_handler_.setMinimalDataAndFilters(application_handler_.sequenceHandler_);
          session_handler_.setComponentQCsTable(application_handler_.sequenceHandler_);
          Eigen::Tensor<bool, 1> table_filters = session_handler_.getComponentQCsTableFilters();
          auto widget = getGenericTableWidget(generic_table_widgets, "CompQCsMainWindow");
          widget->headers_ = session_handler_.comp_qcs_table_headers;
          widget->columns_ = session_handler_.comp_qcs_table_body;
          widget->checked_rows_ = table_filters;
          widget->draw();
          ImGui::EndTabItem();
        }
        if (show_comp_group_qcs_table && ImGui::BeginTabItem("Component Group QCs", &show_comp_group_qcs_table))
        {
          session_handler_.setMinimalDataAndFilters(application_handler_.sequenceHandler_);
          session_handler_.setComponentGroupQCsTable(application_handler_.sequenceHandler_);
          Eigen::Tensor<bool, 1> table_filters = session_handler_.getComponentGroupQCsTableFilters();
          auto widget = getGenericTableWidget(generic_table_widgets, "CompGroupQCsMainWindow");
          widget->headers_ = session_handler_.comp_group_qcs_table_headers;
          widget->columns_ = session_handler_.comp_group_qcs_table_body;
          widget->checked_rows_ = table_filters;
          widget->draw();
          ImGui::EndTabItem();
        }
        if (show_feature_table && ImGui::BeginTabItem("Features table", &show_feature_table))
        {
          session_handler_.setMinimalDataAndFilters(application_handler_.sequenceHandler_);
          exceeding_table_size_ = !session_handler_.setFeatureTable(application_handler_.sequenceHandler_);
          auto widget = getGenericTableWidget(generic_table_widgets, "featuresTableMainWindow");
          widget->headers_ = session_handler_.feature_table_headers;
          widget->columns_ = session_handler_.feature_table_body;
          widget->checked_rows_ = Eigen::Tensor<bool, 1>();
          widget->draw();
          ImGui::EndTabItem();
        }
        if (show_comp_rsd_filters_table && ImGui::BeginTabItem("Component RSD Filters", &show_comp_rsd_filters_table))
        {
          session_handler_.setMinimalDataAndFilters(application_handler_.sequenceHandler_);
          session_handler_.setComponentRSDFiltersTable(application_handler_.sequenceHandler_);
          Eigen::Tensor<bool, 1> table_filters = session_handler_.getComponentRSDFiltersTableFilters();
          auto widget = getGenericTableWidget(generic_table_widgets, "CompRSDFiltersMainWindow");
          widget->headers_ = session_handler_.comp_rsd_filters_table_headers;
          widget->columns_ = session_handler_.comp_rsd_filters_table_body;
          widget->checked_rows_ = table_filters;
          widget->draw();
          ImGui::EndTabItem();
        }
        if (show_comp_group_rsd_filters_table && ImGui::BeginTabItem("Component Group RSD Filters", &show_comp_group_rsd_filters_table))
        {
          session_handler_.setMinimalDataAndFilters(application_handler_.sequenceHandler_);
          session_handler_.setComponentGroupRSDFiltersTable(application_handler_.sequenceHandler_);
          Eigen::Tensor<bool, 1> table_filters = session_handler_.getComponentGroupRSDFiltersTableFilters();
          auto widget = getGenericTableWidget(generic_table_widgets, "CompGroupRSDFiltersMainWindow");
          widget->headers_ = session_handler_.comp_group_rsd_filters_table_headers;
          widget->columns_ = session_handler_.comp_group_rsd_filters_table_body;
          widget->checked_rows_ = table_filters;
          widget->draw();
          ImGui::EndTabItem();
        }
        if (show_comp_rsd_qcs_table && ImGui::BeginTabItem("Component RSD QCs", &show_comp_rsd_qcs_table))
        {
          session_handler_.setMinimalDataAndFilters(application_handler_.sequenceHandler_);
          session_handler_.setComponentRSDQCsTable(application_handler_.sequenceHandler_);
          Eigen::Tensor<bool, 1> table_filters = session_handler_.getComponentRSDQCsTableFilters();
          auto widget = getGenericTableWidget(generic_table_widgets, "CompRSDQCsMainWindow");
          widget->headers_ = session_handler_.comp_rsd_qcs_table_headers;
          widget->columns_ = session_handler_.comp_rsd_qcs_table_body;
          widget->checked_rows_ = table_filters;
          widget->draw();
          ImGui::EndTabItem();
        }
        if (show_comp_group_rsd_qcs_table && ImGui::BeginTabItem("Component Group RSD QCs", &show_comp_group_rsd_qcs_table))
        {
          session_handler_.setMinimalDataAndFilters(application_handler_.sequenceHandler_);
          session_handler_.setComponentGroupRSDQCsTable(application_handler_.sequenceHandler_);
          Eigen::Tensor<bool, 1> table_filters = session_handler_.getComponentGroupRSDQCsTableFilters();
          auto widget = getGenericTableWidget(generic_table_widgets, "CompGroupRSDQCsMainWindow");
          widget->headers_ = session_handler_.comp_group_rsd_qcs_table_headers;
          widget->columns_ = session_handler_.comp_group_rsd_qcs_table_body;
          widget->checked_rows_ = table_filters;
          widget->draw();
          ImGui::EndTabItem();
        }
        if (show_comp_background_filters_table && ImGui::BeginTabItem("Component Background Filters", &show_comp_background_filters_table))
        {
          session_handler_.setMinimalDataAndFilters(application_handler_.sequenceHandler_);
          session_handler_.setComponentBackgroundFiltersTable(application_handler_.sequenceHandler_);
          Eigen::Tensor<bool, 1> table_filters = session_handler_.getComponentBackgroundFiltersTableFilters();
          auto widget = getGenericTableWidget(generic_table_widgets, "CompBackgroundFiltersMainWindow");
          widget->headers_ = session_handler_.comp_background_filters_table_headers;
          widget->columns_ = session_handler_.comp_background_filters_table_body;
          widget->checked_rows_ = table_filters;
          widget->draw();
          ImGui::EndTabItem();
        }
        if (show_comp_group_background_filters_table && ImGui::BeginTabItem("Component Group Background Filters", &show_comp_group_background_filters_table))
        {
          session_handler_.setMinimalDataAndFilters(application_handler_.sequenceHandler_);
          session_handler_.setComponentGroupBackgroundFiltersTable(application_handler_.sequenceHandler_);
          Eigen::Tensor<bool, 1> table_filters = session_handler_.getComponentGroupBackgroundFiltersTableFilters();
          auto widget = getGenericTableWidget(generic_table_widgets, "CompGroupBackgroundFiltersMainWindow");
          widget->headers_ = session_handler_.comp_group_background_filters_table_headers;
          widget->columns_ = session_handler_.comp_group_background_filters_table_body;
          widget->checked_rows_ = table_filters;
          widget->draw();
          ImGui::EndTabItem();
        }
        if (show_comp_background_qcs_table && ImGui::BeginTabItem("Component Background QCs", &show_comp_background_qcs_table))
        {
          session_handler_.setMinimalDataAndFilters(application_handler_.sequenceHandler_);
          session_handler_.setComponentBackgroundQCsTable(application_handler_.sequenceHandler_);
          Eigen::Tensor<bool, 1> table_filters = session_handler_.getComponentBackgroundQCsTableFilters();
          auto widget = getGenericTableWidget(generic_table_widgets, "CompBackgroundQCsMainWindow");
          widget->headers_ = session_handler_.comp_background_qcs_table_headers;
          widget->columns_ = session_handler_.comp_background_qcs_table_body;
          widget->checked_rows_ = table_filters;
          widget->draw();
          ImGui::EndTabItem();
        }
        if (show_comp_group_background_qcs_table && ImGui::BeginTabItem("Component Group Background QCs", &show_comp_group_background_qcs_table))
        {
          session_handler_.setMinimalDataAndFilters(application_handler_.sequenceHandler_);
          session_handler_.setComponentGroupBackgroundQCsTable(application_handler_.sequenceHandler_);
          Eigen::Tensor<bool, 1> table_filters = session_handler_.getComponentGroupBackgroundQCsTableFilters();
          auto widget = getGenericTableWidget(generic_table_widgets, "CompGroupBackgroundQCsMainWindow");
          widget->headers_ = session_handler_.comp_group_background_qcs_table_headers;
          widget->columns_ = session_handler_.comp_group_background_qcs_table_body;
          widget->checked_rows_ = table_filters;
          widget->draw();
          ImGui::EndTabItem();
        }
        if (show_comp_rsd_estimations_table && ImGui::BeginTabItem("Component RSD Filters", &show_comp_rsd_estimations_table))
        {
          session_handler_.setMinimalDataAndFilters(application_handler_.sequenceHandler_);
          session_handler_.setComponentRSDEstimationsTable(application_handler_.sequenceHandler_);
          Eigen::Tensor<bool, 1> table_estimations = session_handler_.getComponentRSDEstimationsTableFilters();
          auto widget = getGenericTableWidget(generic_table_widgets, "CompRSDEstimationsMainWindow");
          widget->headers_ = session_handler_.comp_rsd_estimations_table_headers;
          widget->columns_ = session_handler_.comp_rsd_estimations_table_body;
          widget->checked_rows_ = table_estimations;
          widget->draw();
          ImGui::EndTabItem();
        }
        if (show_comp_group_rsd_estimations_table && ImGui::BeginTabItem("Component Group RSD Filters", &show_comp_group_rsd_estimations_table))
        {
          session_handler_.setMinimalDataAndFilters(application_handler_.sequenceHandler_);
          session_handler_.setComponentGroupRSDEstimationsTable(application_handler_.sequenceHandler_);
          Eigen::Tensor<bool, 1> table_estimations = session_handler_.getComponentGroupRSDEstimationsTableFilters();
          auto widget = getGenericTableWidget(generic_table_widgets, "CompGroupRSDEstimationsMainWindow");
          widget->headers_ = session_handler_.comp_group_rsd_estimations_table_headers;
          widget->columns_ = session_handler_.comp_group_rsd_estimations_table_body;
          widget->checked_rows_ = table_estimations;
          widget->draw();
          ImGui::EndTabItem();
        }
        if (show_comp_background_estimations_table && ImGui::BeginTabItem("Component Background Filters", &show_comp_background_estimations_table))
        {
          session_handler_.setMinimalDataAndFilters(application_handler_.sequenceHandler_);
          session_handler_.setComponentBackgroundEstimationsTable(application_handler_.sequenceHandler_);
          Eigen::Tensor<bool, 1> table_estimations = session_handler_.getComponentBackgroundEstimationsTableFilters();
          auto widget = getGenericTableWidget(generic_table_widgets, "CompBackgroundEstimationsMainWindow");
          widget->headers_ = session_handler_.comp_background_estimations_table_headers;
          widget->columns_ = session_handler_.comp_background_estimations_table_body;
          widget->checked_rows_ = table_estimations;
          widget->draw();
          ImGui::EndTabItem();
        }
        if (show_comp_group_background_estimations_table && ImGui::BeginTabItem("Component Group Background Filters", &show_comp_group_background_estimations_table))
        {
          session_handler_.setMinimalDataAndFilters(application_handler_.sequenceHandler_);
          session_handler_.setComponentGroupBackgroundEstimationsTable(application_handler_.sequenceHandler_);
          Eigen::Tensor<bool, 1> table_estimations = session_handler_.getComponentGroupBackgroundEstimationsTableFilters();
          auto widget = getGenericTableWidget(generic_table_widgets, "CompGroupBackgroundEstimationsMainWindow");
          widget->headers_ = session_handler_.comp_group_background_estimations_table_headers;
          widget->columns_ = session_handler_.comp_group_background_estimations_table_body;
          widget->checked_rows_ = table_estimations;
          widget->draw();
          ImGui::EndTabItem();
        }
        if (show_feature_pivot_table && ImGui::BeginTabItem("Features matrix", &show_feature_pivot_table))
        {
          session_handler_.setMinimalDataAndFilters(application_handler_.sequenceHandler_);
          session_handler_.setFeatureMatrix(application_handler_.sequenceHandler_);
          auto widget = getGenericTableWidget(generic_table_widgets, "featureMatrixMainWindow");
          widget->headers_ = session_handler_.feature_pivot_table_headers;
          widget->columns_ = session_handler_.feature_pivot_table_body;
          widget->checked_rows_ = Eigen::Tensor<bool, 1>();
          widget->draw();
          ImGui::EndTabItem();
        }
        if (show_chromatogram_line_plot && ImGui::BeginTabItem("Chromatograms", &show_chromatogram_line_plot))
        {
          if (!chromatogram_plot_widget)
          {
            chromatogram_plot_widget = std::make_unique<ChromatogramPlotWidget>(
              session_handler_,
              application_handler_.sequenceHandler_,
              "Chromatograms Main Window");
          }
          chromatogram_plot_widget->setWindowSize(win_size_and_pos.bottom_and_top_window_x_size_, win_size_and_pos.top_window_y_size_);
          if (!workflow_is_done_)
          {
            chromatogram_initialized = false;
          }
          else // workflow_is_done_
          {
            if (!chromatogram_initialized)
            {
              chromatogram_plot_widget->setRefreshNeeded();
              chromatogram_initialized = true;
            }
          }
          // The actual plot
          chromatogram_plot_widget->draw();
          ImGui::EndTabItem();
        }
        if (show_spectra_line_plot && ImGui::BeginTabItem("Spectra", &show_spectra_line_plot))
        {
          if (!spectra_plot_widget)
          {
            spectra_plot_widget = std::make_unique<SpectraPlotWidget>(
              session_handler_,
              application_handler_.sequenceHandler_,
              "Spectra Main Window");
          }
          spectra_plot_widget->setWindowSize(win_size_and_pos.bottom_and_top_window_x_size_, win_size_and_pos.top_window_y_size_);
          if (!workflow_is_done_)
          {
            spectra_initialized = false;
          }
          else // workflow_is_done_
          {
            if (!spectra_initialized)
            {
              spectra_plot_widget->setRefreshNeeded();
              spectra_initialized = true;
            }
          }
          // The actual plot
          spectra_plot_widget->draw();
          ImGui::EndTabItem();
        }
        if (show_feature_line_plot && ImGui::BeginTabItem("Features (line)", &show_feature_line_plot))
        {
          session_handler_.setMinimalDataAndFilters(application_handler_.sequenceHandler_);
          session_handler_.setFeatureMatrix(application_handler_.sequenceHandler_);
          Eigen::Tensor<float, 2> x_data = session_handler_.feat_sample_data.shuffle(Eigen::array<Eigen::Index, 2>({ 1,0 }));
          Eigen::Tensor<float, 2> y_data = session_handler_.feat_value_data.shuffle(Eigen::array<Eigen::Index, 2>({ 1,0 }));
          LinePlot2DWidget plot2d(x_data, y_data, session_handler_.feat_heatmap_row_labels, session_handler_.feat_line_x_axis_title, session_handler_.feat_line_y_axis_title,
            session_handler_.feat_line_sample_min, session_handler_.feat_line_sample_max, session_handler_.feat_value_min, session_handler_.feat_value_max,
            win_size_and_pos.bottom_and_top_window_x_size_, win_size_and_pos.top_window_y_size_, "FeaturesLineMainWindow");
          plot2d.draw();
          ImGui::EndTabItem();
        }
        if (show_feature_heatmap_plot && ImGui::BeginTabItem("Features (heatmap)", &show_feature_heatmap_plot))
        {
          session_handler_.setMinimalDataAndFilters(application_handler_.sequenceHandler_);
          session_handler_.setFeatureMatrix(application_handler_.sequenceHandler_);
          Heatmap2DWidget plot2d(session_handler_.feat_heatmap_data, session_handler_.feat_heatmap_col_labels, session_handler_.feat_heatmap_row_labels,
            session_handler_.feat_heatmap_x_axis_title, session_handler_.feat_heatmap_y_axis_title, session_handler_.feat_value_min, session_handler_.feat_value_max,
            win_size_and_pos.bottom_and_top_window_x_size_, win_size_and_pos.top_window_y_size_, "FeaturesHeatmapMainWindow");
          plot2d.draw();
          ImGui::EndTabItem();
        }
        if (show_calibrators_line_plot && ImGui::BeginTabItem("Calibrators", &show_calibrators_line_plot))
        {
          session_handler_.setMinimalDataAndFilters(application_handler_.sequenceHandler_);
          exceeding_plot_points_ = !session_handler_.setCalibratorsScatterLinePlot(application_handler_.sequenceHandler_);
          CalibratorsPlotWidget plot2d(session_handler_.calibrators_conc_fit_data, session_handler_.calibrators_feature_fit_data,
            session_handler_.calibrators_conc_raw_data, session_handler_.calibrators_feature_raw_data, session_handler_.calibrators_series_names,
            session_handler_.calibrators_x_axis_title, session_handler_.calibrators_y_axis_title, session_handler_.calibrators_conc_min, session_handler_.calibrators_conc_max,
            session_handler_.calibrators_feature_min, session_handler_.calibrators_feature_max, win_size_and_pos.bottom_and_top_window_x_size_, win_size_and_pos.top_window_y_size_,
            "CalibratorsMainWindow");
          plot2d.draw();

          ImGui::EndTabItem();
        }
        if (show_satistics_widget && ImGui::BeginTabItem("Statistics", &show_satistics_widget))
        {
          static bool statistics_initialized = false;
          if (!workflow_is_done_)
          {
            statistics_initialized = false;
          }
          else // workflow_is_done_
          {
            if (!statistics_initialized)
            {
              statistics_.setRefreshNeeded();
              statistics_initialized = true;
            }
          }
          statistics_.setInjections(session_handler_.injection_explorer_checkbox_body, session_handler_.getInjectionExplorerBody());
          statistics_.setTransitions(&session_handler_.transitions_table_body, session_handler_.transition_explorer_checkbox_body, session_handler_.getTransitionExplorerBody());
          statistics_.setWindowSize(win_size_and_pos.bottom_and_top_window_x_size_, win_size_and_pos.top_window_y_size_);
          statistics_.draw();
          ImGui::EndTabItem();
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
          quickInfoText_.setTransitions(&session_handler_.transitions_table_body);
          quickInfoText_.draw();
          ImGui::EndChild();
          ImGui::EndTabItem();
        }
        if (show_log_ && ImGui::BeginTabItem("Log", &show_log_))
        {
          const char* items[] = { "NONE", "FATAL", "ERROR", "WARN", "INFO", "DEBUG", "VERB" }; // reflects the strings in plog's Severity.h
          static int selected_severity = 4;
          static plog::Severity severity = plog::Severity::info;

          if (ImGui::Combo("Level", &selected_severity, items, IM_ARRAYSIZE(items)))
          {
            severity = plog::severityFromString(items[selected_severity]);
          }

          ImGui::Separator();
          ImGui::BeginChild("Log child");
          const std::vector<plog::util::nstring> message_list = appender_.getMessageList(severity);
          int message_list_start = (message_list.size() > 500) ? message_list.size() - 500 : 0;
          for (int i= message_list_start;i< message_list.size();++i)
          {
            std::string str(message_list.at(i).data(), message_list.at(i).data() + message_list.at(i).size());
            ImGui::Text("%s", str.c_str());
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
