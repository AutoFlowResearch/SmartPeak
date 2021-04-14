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
#include <SmartPeak/ui/RunWorkflowWidget.h>
#include <SmartPeak/ui/AboutWidget.h>
#include <SmartPeak/ui/LogWidget.h>
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

int main(int argc, char** argv)
// `int argc, char **argv` are required on Win to link against the proper SDL2/OpenGL implementation
{

  // to disable buttons, display info, and update the session cache
  bool workflow_is_done_ = true;
  bool file_loading_is_done_ = true;
  bool exceeding_plot_points_ = false;
  bool exceeding_table_size_ = false;
  bool ran_integrity_check_ = false;
  bool integrity_check_failed_ = false;
  bool update_session_cache_ = false;

  // 
  bool heatmap_initialized = false;
  bool chromatogram_initialized = false;
  bool spectra_initialized = false;

  ApplicationHandler application_handler_;
  SessionHandler session_handler_;
  WorkflowManager workflow_manager_;
  GuiAppender appender_;

  // widgets
  FilePicker file_picker_;
  auto quickInfoText_= std::make_shared<InfoWidget>();
  auto report_ = std::make_shared<Report>();
  auto workflow_ = std::make_shared<Workflow>();
  auto statistics_ = std::make_shared<StatisticsWidget>();
  auto run_workflow_widget_ = std::make_shared<RunWorkflowWidget>();
  auto about_widget_ = std::make_shared<AboutWidget>();
  auto log_widget_ = std::make_shared<LogWidget>(appender_);
  auto parameters_table_widget_ = std::make_shared<ParametersTableWidget>(session_handler_, application_handler_, "ParametersMainWindow");
  auto chromatogram_plot_widget_ = std::make_shared<ChromatogramPlotWidget>(session_handler_, application_handler_.sequenceHandler_, "Chromatograms Main Window");
  auto heatmap_plot_widget_ = std::make_shared<Heatmap2DWidget>(session_handler_, application_handler_.sequenceHandler_, "Heatmap Main Window");
  auto spectra_plot_widget_ = std::make_shared<SpectraPlotWidget>(session_handler_, application_handler_.sequenceHandler_, "Spectra Main Window");
  auto feature_line_plot_ = std::make_shared<LinePlot2DWidget>();
  auto calibrators_line_plot_ = std::make_shared<CalibratorsPlotWidget>();
  auto injections_explorer_window_ = std::make_shared<ExplorerWidget>("InjectionsExplorerWindow");
  auto transitions_explorer_window_ = std::make_shared<ExplorerWidget>("TransitionsExplorerWindow");
  auto features_explorer_window_ = std::make_shared<ExplorerWidget>("FeaturesExplorerWindow");
  auto spectrum_explorer_window_ = std::make_shared<ExplorerWidget>("SpectrumExplorerWindow");
  auto sequence_main_window_ = std::make_shared<GenericTableWidget>("SequenceMainWindow");
  auto transitions_main_window_ = std::make_shared<GenericTableWidget>("TransitionsMainWindow");
  auto spectrum_main_window_ = std::make_shared<GenericTableWidget>("SpectrumMainWindow");
  auto quant_method_main_window_ = std::make_shared<GenericTableWidget>("QuantMethodMainWindow");
  auto stds_consc_main_window_ = std::make_shared<GenericTableWidget>("StdsConcsMainWindow");
  auto comp_filters_main_window_ = std::make_shared<GenericTableWidget>("CompFiltersMainWindow");
  auto comp_group_filters_main_window_ = std::make_shared<GenericTableWidget>("CompGroupFiltersMainWindow");
  auto comp_qc_main_window_ = std::make_shared<GenericTableWidget>("CompQCsMainWindow");
  auto comp_group_qc_main_window_ = std::make_shared<GenericTableWidget>("CompGroupQCsMainWindow");
  auto comp_rsd_filters_main_window_ = std::make_shared<GenericTableWidget>("CompRSDFiltersMainWindow");
  auto comp_group_rds_filters_main_window_ = std::make_shared<GenericTableWidget>("CompGroupRSDFiltersMainWindow");
  auto comp_rsdcqcs_main_window_ = std::make_shared<GenericTableWidget>("CompRSDQCsMainWindow");
  auto comp_group_rsdqcs_main_window_ = std::make_shared<GenericTableWidget>("CompGroupRSDQCsMainWindow"); // TODO Check this one
  auto comp_background_filters_main_window_ = std::make_shared<GenericTableWidget>("CompBackgroundFiltersMainWindow");
  auto comp_group_background_filters_main_window_ = std::make_shared<GenericTableWidget>("CompGroupBackgroundFiltersMainWindow");
  auto comp_background_qcs_main_window_ = std::make_shared<GenericTableWidget>("CompBackgroundQCsMainWindow");
  auto comp_group_background_qcs_main_window_ = std::make_shared<GenericTableWidget>("CompGroupBackgroundQCsMainWindow");
  auto comp_rsd_estimations_main_window_ = std::make_shared<GenericTableWidget>("CompRSDEstimationsMainWindow");
  auto comp_group_rsd_estimation_main_window_ = std::make_shared<GenericTableWidget>("CompGroupRSDEstimationsMainWindow");
  auto comp_background_estimations_main_window_ = std::make_shared<GenericTableWidget>("CompBackgroundEstimationsMainWindow");
  auto comp_group_background_estimations_main_window_ = std::make_shared<GenericTableWidget>("CompGroupBackgroundEstimationsMainWindow");
  auto features_table_main_window_ = std::make_shared<GenericTableWidget>("featuresTableMainWindow");
  auto feature_matrix_main_window_ = std::make_shared<GenericTableWidget>("featureMatrixMainWindow");

  report_->setApplicationHandler(application_handler_);
  workflow_->setApplicationHandler(application_handler_);
  statistics_->setApplicationHandler(application_handler_);
  quickInfoText_->setApplicationHandler(application_handler_);

  // visible on start
  workflow_->visible_ = true;
  quickInfoText_->visible_ = true;
  injections_explorer_window_->visible_ = true;
  transitions_explorer_window_->visible_ = true;

  // windows organization
  std::vector<std::shared_ptr<Widget>> top_windows = {
    sequence_main_window_,
    transitions_main_window_,
    spectrum_main_window_,
    workflow_,
    parameters_table_widget_,
    quant_method_main_window_,
    stds_consc_main_window_,
    comp_filters_main_window_,
    comp_group_filters_main_window_,
    comp_qc_main_window_,
    comp_group_qc_main_window_,
    comp_rsd_filters_main_window_,
    comp_group_rds_filters_main_window_,
    comp_rsdcqcs_main_window_,
    comp_group_rsdqcs_main_window_,
    comp_background_filters_main_window_,
    comp_group_background_filters_main_window_,
    comp_background_qcs_main_window_,
    comp_group_background_qcs_main_window_,
    comp_rsd_estimations_main_window_,
    comp_group_rsd_estimation_main_window_,
    comp_background_estimations_main_window_,
    comp_group_background_estimations_main_window_,
    features_table_main_window_,
    feature_matrix_main_window_,
    chromatogram_plot_widget_,
    spectra_plot_widget_,
    feature_line_plot_,
    heatmap_plot_widget_,
    calibrators_line_plot_
  };

  std::vector<std::shared_ptr<Widget>> bottom_windows = {
    quickInfoText_,
    log_widget_
  };

  std::vector<std::shared_ptr<Widget>> left_windows = {
    injections_explorer_window_,
    transitions_explorer_window_,
    features_explorer_window_,
    spectrum_explorer_window_
  };

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
    if (logdir_created) LOG_DEBUG << "Log directory created: " << logdirpath;
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

      if ((!workflow_is_done_) && workflow_manager_.isWorkflowDone())
      {
        workflow_manager_.updateApplicationHandler(application_handler_);
        quickInfoText_->setLastRunTime(workflow_manager_.getLastRunTime());
      }
      workflow_is_done_ = workflow_manager_.isWorkflowDone();
      file_loading_is_done_ = file_picker_.fileLoadingIsDone();

      // Make the quick info text
      quickInfoText_->setWorkflowDone(workflow_is_done_);
      quickInfoText_->setFileLoadingDone(file_loading_is_done_, file_picker_.errorLoadingFile());
      quickInfoText_->clearErrorMessages();
      if (exceeding_plot_points_) quickInfoText_->addErrorMessage("Plot rendering limit reached.  Not plotting all selected data.");
      if (exceeding_table_size_) quickInfoText_->addErrorMessage("Table rendering limit reached.  Not showing all selected data.");
      if (ran_integrity_check_ && integrity_check_failed_) quickInfoText_->addErrorMessage("Integrity check failed.  Check the `Information` log.");
      if (ran_integrity_check_ && !integrity_check_failed_) quickInfoText_->addErrorMessage("Integrity check passed.");

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
    if (file_picker_.visible_)
    {
      ImGui::OpenPopup("Pick a pathname");
    }
    file_picker_.draw();
    if (run_workflow_widget_->visible_)
    {
      run_workflow_widget_->setApplicationHandler(application_handler_);
      run_workflow_widget_->setSessionHandler(session_handler_);
      run_workflow_widget_->setWorkflowManager(workflow_manager_);
      ImGui::OpenPopup("Run workflow modal");
      run_workflow_widget_->draw();
    }
    if (about_widget_->visible_)
    {
      ImGui::OpenPopup("About");
      about_widget_->draw();
    }
    if (report_->draw_)
    {
      report_->draw();
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
          file_picker_.visible_ = true;
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
            file_picker_.visible_ = true;
            update_session_cache_ = true;
          }
          if (ImGui::MenuItem("Parameters")) {
            static LoadSequenceParameters processor(application_handler_);
            file_picker_.setProcessor(processor);
            file_picker_.visible_ = true;
            update_session_cache_ = true;
          }
          if (ImGui::MenuItem("Workflow", NULL, false, workflow_is_done_)) {
            static LoadSequenceWorkflow processor(application_handler_);
            file_picker_.setProcessor(processor);
            file_picker_.visible_ = true;
            update_session_cache_ = true;
          }
          if (ImGui::MenuItem("Reference data")) {
            static LoadSequenceValidationData processor(application_handler_);
            file_picker_.setProcessor(processor);
            file_picker_.visible_ = true;
            update_session_cache_ = true;
          }
          if (ImGui::MenuItem("Quant Method")) {
            static LoadSequenceSegmentQuantitationMethods processor(application_handler_);
            file_picker_.setProcessor(processor);
            file_picker_.visible_ = true;
            update_session_cache_ = true;
          }
          if (ImGui::MenuItem("Standards Conc")) {
            static LoadSequenceSegmentStandardsConcentrations processor(application_handler_);
            file_picker_.setProcessor(processor);
            file_picker_.visible_ = true;
            update_session_cache_ = true;
          }
          if (ImGui::MenuItem("Comp Filters")) {
            static LoadSequenceSegmentFeatureFilterComponents processor(application_handler_);
            file_picker_.setProcessor(processor);
            file_picker_.visible_ = true;
            update_session_cache_ = true;
          }
          if (ImGui::MenuItem("Comp Group Filters")) {
            static LoadSequenceSegmentFeatureFilterComponentGroups processor(application_handler_);
            file_picker_.setProcessor(processor);
            file_picker_.visible_ = true;
            update_session_cache_ = true;
          }
          if (ImGui::MenuItem("Comp QCs")) {
            static LoadSequenceSegmentFeatureQCComponents processor(application_handler_);
            file_picker_.setProcessor(processor);
            file_picker_.visible_ = true;
            update_session_cache_ = true;
          }
          if (ImGui::MenuItem("Comp Group QCs")) {
            static LoadSequenceSegmentFeatureQCComponentGroups processor(application_handler_);
            file_picker_.setProcessor(processor);
            file_picker_.visible_ = true;
            update_session_cache_ = true;
          }
          if (ImGui::MenuItem("Comp %RSD Filters")) {
            static LoadSequenceSegmentFeatureRSDFilterComponents processor(application_handler_);
            file_picker_.setProcessor(processor);
            file_picker_.visible_ = true;
            update_session_cache_ = true;
          }
          if (ImGui::MenuItem("Comp Group %RSD Filters")) {
            static LoadSequenceSegmentFeatureRSDFilterComponentGroups processor(application_handler_);
            file_picker_.setProcessor(processor);
            file_picker_.visible_ = true;
            update_session_cache_ = true;
          }
          if (ImGui::MenuItem("Comp %RSD QCs")) {
            static LoadSequenceSegmentFeatureRSDQCComponents processor(application_handler_);
            file_picker_.setProcessor(processor);
            file_picker_.visible_ = true;
            update_session_cache_ = true;
          }
          if (ImGui::MenuItem("Comp Group %RSD QCs")) {
            static LoadSequenceSegmentFeatureRSDQCComponentGroups processor(application_handler_);
            file_picker_.setProcessor(processor);
            file_picker_.visible_ = true;
            update_session_cache_ = true;
          }
          if (ImGui::MenuItem("Comp %Background Filters")) {
            static LoadSequenceSegmentFeatureBackgroundFilterComponents processor(application_handler_);
            file_picker_.setProcessor(processor);
            file_picker_.visible_ = true;
            update_session_cache_ = true;
          }
          if (ImGui::MenuItem("Comp Group %Background Filters")) {
            static LoadSequenceSegmentFeatureBackgroundFilterComponentGroups processor(application_handler_);
            file_picker_.setProcessor(processor);
            file_picker_.visible_ = true;
            update_session_cache_ = true;
          }
          if (ImGui::MenuItem("Comp %Background QCs")) {
            static LoadSequenceSegmentFeatureBackgroundQCComponents processor(application_handler_);
            file_picker_.setProcessor(processor);
            file_picker_.visible_ = true;
            update_session_cache_ = true;
          }
          if (ImGui::MenuItem("Comp Group %Background QCs")) {
            static LoadSequenceSegmentFeatureBackgroundQCComponentGroups processor(application_handler_);
            file_picker_.setProcessor(processor);
            file_picker_.visible_ = true;
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
            file_picker_.visible_ = true;
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
        if (ImGui::MenuItem("Injections", NULL, &injections_explorer_window_->visible_)) {} // TODO: search field
        if (ImGui::MenuItem("Transitions", NULL, &transitions_explorer_window_->visible_)) {} // TODO: search field
        if (ImGui::MenuItem("Features", NULL, &features_explorer_window_->visible_)) {}
        if (ImGui::MenuItem("Scans", NULL, &spectrum_explorer_window_->visible_)) {}
        ImGui::Separator(); // Primary input
        ImGui::MenuItem("Main window (Tables)", NULL, false, false);
        if (ImGui::MenuItem("Sequence", NULL, &sequence_main_window_->visible_)) {}
        if (ImGui::MenuItem("Transitions", NULL, &transitions_main_window_->visible_)) {}
        if (ImGui::MenuItem("Scans", NULL, &spectrum_main_window_->visible_)) {}
        if (ImGui::MenuItem("Workflow", NULL, &workflow_->visible_)) {}
        if (ImGui::BeginMenu("Workflow settings"))
        {
          if (ImGui::MenuItem("Parameters", NULL, &parameters_table_widget_->visible_)) {}
          if (ImGui::MenuItem("Quant Method", NULL, &quant_method_main_window_->visible_)) {}
          if (ImGui::MenuItem("Standards Conc", NULL, &stds_consc_main_window_->visible_)) {}
          if (ImGui::MenuItem("Comp Filters", NULL, &comp_filters_main_window_->visible_)) {}
          if (ImGui::MenuItem("Comp Group Filters", NULL, &comp_group_filters_main_window_->visible_)) {}
          if (ImGui::MenuItem("Comp QCs", NULL, &comp_qc_main_window_->visible_)) {}
          if (ImGui::MenuItem("Comp Group QCs", NULL, &comp_group_qc_main_window_->visible_)) {}
          if (ImGui::MenuItem("Comp RSD Filters", NULL, &comp_rsd_filters_main_window_->visible_)) {}
          if (ImGui::MenuItem("Comp Group RSD Filters", NULL, &comp_group_rds_filters_main_window_->visible_)) {}
          if (ImGui::MenuItem("Comp RSD QCs", NULL, &comp_rsdcqcs_main_window_->visible_)) {}
          if (ImGui::MenuItem("Comp Group RSD QCs", NULL, &comp_group_rsdqcs_main_window_->visible_)) {}
          if (ImGui::MenuItem("Comp Background Filters", NULL, &comp_background_filters_main_window_->visible_)) {}
          if (ImGui::MenuItem("Comp Group Background Filters", NULL, &comp_group_background_filters_main_window_->visible_)) {}
          if (ImGui::MenuItem("Comp Background QCs", NULL, &comp_background_qcs_main_window_->visible_)) {}
          if (ImGui::MenuItem("Comp Group Background QCs", NULL, &comp_group_background_qcs_main_window_->visible_)) {}
          if (ImGui::MenuItem("Comp RSD Estimations", NULL, &comp_rsd_estimations_main_window_->visible_)) {}
          if (ImGui::MenuItem("Comp Group RSD Estimations", NULL, &comp_group_rsd_estimation_main_window_->visible_)) {}
          if (ImGui::MenuItem("Comp Background Estimations", NULL, &comp_background_estimations_main_window_->visible_)) {}
          if (ImGui::MenuItem("Comp Group Background Estimations", NULL, &comp_group_background_estimations_main_window_->visible_)) {}
          // TODO: missing workflow setting tables...
          ImGui::EndMenu();
        }
        if (ImGui::MenuItem("Features (table)", NULL, &features_table_main_window_->visible_)) {}
        if (ImGui::MenuItem("Features (matrix)", NULL, &feature_matrix_main_window_->visible_)) {}
        ImGui::Separator();
        ImGui::MenuItem("Main window (Plots)", NULL, false, false);
        if (ImGui::MenuItem("Chromatogram", NULL, &chromatogram_plot_widget_->visible_)) {}
        if (ImGui::MenuItem("Spectra", NULL, &spectra_plot_widget_->visible_)) {}
        if (ImGui::MenuItem("Features (line)", NULL, &feature_line_plot_->visible_)) {}
        if (ImGui::MenuItem("Features (heatmap)", NULL, &heatmap_plot_widget_->visible_)) {}
        if (ImGui::MenuItem("Calibrators", NULL, &calibrators_line_plot_->visible_)) {}
        if (ImGui::MenuItem("Statistics", NULL, &statistics_->visible_)) {}
        ImGui::Separator(); 
        ImGui::MenuItem("Info window", NULL, false, false);
        if (ImGui::MenuItem("Info", NULL, &quickInfoText_->visible_)) {}
        if (ImGui::MenuItem("Log", NULL, &log_widget_->visible_)) {}
        ImGui::EndMenu();
      }
      if (ImGui::BeginMenu("Actions"))
      {
        if (ImGui::MenuItem("Run workflow", NULL, &run_workflow_widget_->visible_))
        {
          if (application_handler_.sequenceHandler_.getWorkflow().empty())
          {
            LOGW << "Workflow has no steps to run. Please set the workflow's steps.";
          }
          initializeDataDirs(application_handler_);
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
          report_->draw_ = true;
        }
        ImGui::EndMenu();
      }
      if (ImGui::BeginMenu("Help"))
      {
        ImGui::MenuItem("About", NULL, &about_widget_->visible_);
        if (ImGui::MenuItem("Documentation")) {
          // TODO: Render the SmartPeak documentation (See AUT-178)
        }
        ImGui::EndMenu();
      }
      ImGui::EndMainMenuBar();
    }

    bool show_top_window_ = std::find_if(top_windows.begin(), top_windows.end(), [](const auto& w) { return w->visible_; }) != top_windows.end();
    bool show_bottom_window_ = std::find_if(bottom_windows.begin(), bottom_windows.end(), [](const auto& w) { return w->visible_; }) != bottom_windows.end();
    bool show_left_window_ = std::find_if(left_windows.begin(), left_windows.end(), [](const auto& w) { return w->visible_; }) != left_windows.end();
    bool show_right_window_ = false;
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
        if (ImGui::BeginTabItem("Injections", &injections_explorer_window_->visible_))
        {
          session_handler_.setMinimalDataAndFilters(application_handler_.sequenceHandler_);
          auto widget = injections_explorer_window_;
          /* these calls can be replaced by one method call, or, the widget uses session handler to get the data */
          /* to be more clean, the checkbox should be actually not be in session handlers but in the widget */
          widget->headers_ = session_handler_.getInjectionExplorerHeader();
          widget->columns_ = session_handler_.getInjectionExplorerBody();
          widget->checked_rows_ = session_handler_.injection_explorer_checked_rows;
          widget->checkbox_headers_ = session_handler_.injection_explorer_checkbox_headers;
          widget->checkbox_columns_ = &session_handler_.injection_explorer_checkbox_body;
          widget->draw();
          ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Transitions", &transitions_explorer_window_->visible_))
        {
          session_handler_.setMinimalDataAndFilters(application_handler_.sequenceHandler_);
          auto widget = transitions_explorer_window_;
          widget->headers_ = session_handler_.getTransitionExplorerHeader();
          widget->columns_ = session_handler_.getTransitionExplorerBody();
          widget->checked_rows_ = session_handler_.transition_explorer_checked_rows;
          widget->checkbox_headers_ = session_handler_.transition_explorer_checkbox_headers;
          widget->checkbox_columns_ = &session_handler_.transition_explorer_checkbox_body;
          widget->draw();
          ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Features", &features_explorer_window_->visible_))
        {
          session_handler_.setFeatureExplorer();
          auto widget = features_explorer_window_;
          widget->headers_ = session_handler_.feature_explorer_headers;
          widget->columns_ = session_handler_.feature_explorer_body;
          widget->checked_rows_ = session_handler_.feature_explorer_checked_rows;
          widget->checkbox_headers_ = session_handler_.feature_explorer_checkbox_headers;
          widget->checkbox_columns_ = &session_handler_.feature_explorer_checkbox_body;
          widget->draw();
          ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Spectrum", &spectrum_explorer_window_->visible_))
        {
          session_handler_.setMinimalDataAndFilters(application_handler_.sequenceHandler_);
          auto widget = spectrum_explorer_window_;
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
        if (ImGui::BeginTabItem("Sequence", &sequence_main_window_->visible_))
        {
          session_handler_.setMinimalDataAndFilters(application_handler_.sequenceHandler_);
          Eigen::Tensor<bool, 1> table_filters = session_handler_.getSequenceTableFilters();
          auto widget = sequence_main_window_;
          widget->headers_ = session_handler_.sequence_table_headers;
          widget->columns_ = session_handler_.sequence_table_body;
          widget->checked_rows_ = table_filters;
          widget->draw();
          ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Transitions", &transitions_main_window_->visible_))
        {
          session_handler_.setMinimalDataAndFilters(application_handler_.sequenceHandler_);
          Eigen::Tensor<bool, 1> table_filters = session_handler_.getTransitionsTableFilters();
          auto widget = transitions_main_window_;
          widget->headers_ = session_handler_.transitions_table_headers;
          widget->columns_ = session_handler_.transitions_table_body;
          widget->checked_rows_ = table_filters;
          widget->draw();
          ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Spectrum", &spectrum_main_window_->visible_))
        {
          session_handler_.setMinimalDataAndFilters(application_handler_.sequenceHandler_);
          Eigen::Tensor<bool, 1> table_filters = session_handler_.getSpectrumTableFilters();
          auto widget = spectrum_main_window_;
          widget->headers_ = session_handler_.spectrum_table_headers;
          widget->columns_ = session_handler_.spectrum_table_body;
          widget->checked_rows_ = table_filters;
          widget->draw();
          ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Workflow", &workflow_->visible_))
        {
          workflow_->setEditable(workflow_is_done_);
          workflow_->draw();
          ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Parameters", &parameters_table_widget_->visible_))
        {
          parameters_table_widget_->draw();
          ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Quantitation Method", &quant_method_main_window_->visible_))
        {
          session_handler_.setMinimalDataAndFilters(application_handler_.sequenceHandler_);
          session_handler_.setQuantMethodTable(application_handler_.sequenceHandler_);
          Eigen::Tensor<bool, 1> table_filters = session_handler_.getQuantMethodsTableFilters();
          auto widget = quant_method_main_window_;
          widget->headers_ = session_handler_.quant_method_table_headers;
          widget->columns_ = session_handler_.quant_method_table_body;
          widget->checked_rows_ = table_filters;
          widget->draw();
          ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Standards Concentrations", &stds_consc_main_window_->visible_))
        {
          session_handler_.setMinimalDataAndFilters(application_handler_.sequenceHandler_);
          session_handler_.setStdsConcsTable(application_handler_.sequenceHandler_);
          auto widget = stds_consc_main_window_;
          widget->headers_ = session_handler_.stds_concs_table_headers;
          widget->columns_ = session_handler_.stds_concs_table_body;
          widget->checked_rows_ = Eigen::Tensor<bool, 1>();
          widget->draw();
          ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Component Filters", &comp_filters_main_window_->visible_))
        {
          session_handler_.setMinimalDataAndFilters(application_handler_.sequenceHandler_);
          session_handler_.setComponentFiltersTable(application_handler_.sequenceHandler_);
          Eigen::Tensor<bool, 1> table_filters = session_handler_.getComponentFiltersTableFilters();
          auto widget = comp_filters_main_window_;
          widget->headers_ = session_handler_.comp_filters_table_headers;
          widget->columns_ = session_handler_.comp_filters_table_body;
          widget->checked_rows_ = table_filters;
          widget->draw();
          ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Component Group Filters", &comp_group_filters_main_window_->visible_))
        {
          session_handler_.setMinimalDataAndFilters(application_handler_.sequenceHandler_);
          session_handler_.setComponentGroupFiltersTable(application_handler_.sequenceHandler_);
          Eigen::Tensor<bool, 1> table_filters = session_handler_.getComponentGroupFiltersTableFilters();
          auto widget = comp_group_filters_main_window_;
          widget->headers_ = session_handler_.comp_group_filters_table_headers;
          widget->columns_ = session_handler_.comp_group_filters_table_body;
          widget->checked_rows_ = table_filters;
          widget->draw();
          ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Component QCs", &comp_qc_main_window_->visible_))
        {
          session_handler_.setMinimalDataAndFilters(application_handler_.sequenceHandler_);
          session_handler_.setComponentQCsTable(application_handler_.sequenceHandler_);
          Eigen::Tensor<bool, 1> table_filters = session_handler_.getComponentQCsTableFilters();
          auto widget = comp_qc_main_window_;
          widget->headers_ = session_handler_.comp_qcs_table_headers;
          widget->columns_ = session_handler_.comp_qcs_table_body;
          widget->checked_rows_ = table_filters;
          widget->draw();
          ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Component Group QCs", &comp_group_qc_main_window_->visible_))
        {
          session_handler_.setMinimalDataAndFilters(application_handler_.sequenceHandler_);
          session_handler_.setComponentGroupQCsTable(application_handler_.sequenceHandler_);
          Eigen::Tensor<bool, 1> table_filters = session_handler_.getComponentGroupQCsTableFilters();
          auto widget = comp_group_qc_main_window_;
          widget->headers_ = session_handler_.comp_group_qcs_table_headers;
          widget->columns_ = session_handler_.comp_group_qcs_table_body;
          widget->checked_rows_ = table_filters;
          widget->draw();
          ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Features table", &features_table_main_window_->visible_))
        {
          session_handler_.setMinimalDataAndFilters(application_handler_.sequenceHandler_);
          exceeding_table_size_ = !session_handler_.setFeatureTable(application_handler_.sequenceHandler_);
          auto widget = features_table_main_window_;
          widget->headers_ = session_handler_.feature_table_headers;
          widget->columns_ = session_handler_.feature_table_body;
          widget->checked_rows_ = Eigen::Tensor<bool, 1>();
          widget->draw();
          ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Component RSD Filters", &comp_rsd_filters_main_window_->visible_))
        {
          session_handler_.setMinimalDataAndFilters(application_handler_.sequenceHandler_);
          session_handler_.setComponentRSDFiltersTable(application_handler_.sequenceHandler_);
          Eigen::Tensor<bool, 1> table_filters = session_handler_.getComponentRSDFiltersTableFilters();
          auto widget = comp_rsd_filters_main_window_;
          widget->headers_ = session_handler_.comp_rsd_filters_table_headers;
          widget->columns_ = session_handler_.comp_rsd_filters_table_body;
          widget->checked_rows_ = table_filters;
          widget->draw();
          ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Component Group RSD Filters", &comp_group_rds_filters_main_window_->visible_))
        {
          session_handler_.setMinimalDataAndFilters(application_handler_.sequenceHandler_);
          session_handler_.setComponentGroupRSDFiltersTable(application_handler_.sequenceHandler_);
          Eigen::Tensor<bool, 1> table_filters = session_handler_.getComponentGroupRSDFiltersTableFilters();
          auto widget = comp_group_rds_filters_main_window_;
          widget->headers_ = session_handler_.comp_group_rsd_filters_table_headers;
          widget->columns_ = session_handler_.comp_group_rsd_filters_table_body;
          widget->checked_rows_ = table_filters;
          widget->draw();
          ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Component RSD QCs", &comp_rsdcqcs_main_window_->visible_))
        {
          session_handler_.setMinimalDataAndFilters(application_handler_.sequenceHandler_);
          session_handler_.setComponentRSDQCsTable(application_handler_.sequenceHandler_);
          Eigen::Tensor<bool, 1> table_filters = session_handler_.getComponentRSDQCsTableFilters();
          auto widget = comp_rsdcqcs_main_window_;
          widget->headers_ = session_handler_.comp_rsd_qcs_table_headers;
          widget->columns_ = session_handler_.comp_rsd_qcs_table_body;
          widget->checked_rows_ = table_filters;
          widget->draw();
          ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Component Group RSD QCs", &comp_group_rsdqcs_main_window_->visible_))
        {
          session_handler_.setMinimalDataAndFilters(application_handler_.sequenceHandler_);
          session_handler_.setComponentGroupRSDQCsTable(application_handler_.sequenceHandler_);
          Eigen::Tensor<bool, 1> table_filters = session_handler_.getComponentGroupRSDQCsTableFilters();
          auto widget = comp_group_rsdqcs_main_window_;
          widget->headers_ = session_handler_.comp_group_rsd_qcs_table_headers;
          widget->columns_ = session_handler_.comp_group_rsd_qcs_table_body;
          widget->checked_rows_ = table_filters;
          widget->draw();
          ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Component Background Filters", &comp_background_filters_main_window_->visible_))
        {
          session_handler_.setMinimalDataAndFilters(application_handler_.sequenceHandler_);
          session_handler_.setComponentBackgroundFiltersTable(application_handler_.sequenceHandler_);
          Eigen::Tensor<bool, 1> table_filters = session_handler_.getComponentBackgroundFiltersTableFilters();
          auto widget = comp_background_filters_main_window_;
          widget->headers_ = session_handler_.comp_background_filters_table_headers;
          widget->columns_ = session_handler_.comp_background_filters_table_body;
          widget->checked_rows_ = table_filters;
          widget->draw();
          ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Component Group Background Filters", &comp_group_background_filters_main_window_->visible_))
        {
          session_handler_.setMinimalDataAndFilters(application_handler_.sequenceHandler_);
          session_handler_.setComponentGroupBackgroundFiltersTable(application_handler_.sequenceHandler_);
          Eigen::Tensor<bool, 1> table_filters = session_handler_.getComponentGroupBackgroundFiltersTableFilters();
          auto widget = comp_group_background_filters_main_window_;
          widget->headers_ = session_handler_.comp_group_background_filters_table_headers;
          widget->columns_ = session_handler_.comp_group_background_filters_table_body;
          widget->checked_rows_ = table_filters;
          widget->draw();
          ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Component Background QCs", &comp_background_qcs_main_window_->visible_))
        {
          session_handler_.setMinimalDataAndFilters(application_handler_.sequenceHandler_);
          session_handler_.setComponentBackgroundQCsTable(application_handler_.sequenceHandler_);
          Eigen::Tensor<bool, 1> table_filters = session_handler_.getComponentBackgroundQCsTableFilters();
          auto widget = comp_background_qcs_main_window_;
          widget->headers_ = session_handler_.comp_background_qcs_table_headers;
          widget->columns_ = session_handler_.comp_background_qcs_table_body;
          widget->checked_rows_ = table_filters;
          widget->draw();
          ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Component Group Background QCs", &comp_group_background_qcs_main_window_->visible_))
        {
          session_handler_.setMinimalDataAndFilters(application_handler_.sequenceHandler_);
          session_handler_.setComponentGroupBackgroundQCsTable(application_handler_.sequenceHandler_);
          Eigen::Tensor<bool, 1> table_filters = session_handler_.getComponentGroupBackgroundQCsTableFilters();
          auto widget = comp_group_background_qcs_main_window_;
          widget->headers_ = session_handler_.comp_group_background_qcs_table_headers;
          widget->columns_ = session_handler_.comp_group_background_qcs_table_body;
          widget->checked_rows_ = table_filters;
          widget->draw();
          ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Component RSD Filters", &comp_rsd_estimations_main_window_->visible_))
        {
          session_handler_.setMinimalDataAndFilters(application_handler_.sequenceHandler_);
          session_handler_.setComponentRSDEstimationsTable(application_handler_.sequenceHandler_);
          Eigen::Tensor<bool, 1> table_estimations = session_handler_.getComponentRSDEstimationsTableFilters();
          auto widget = comp_rsd_estimations_main_window_;
          widget->headers_ = session_handler_.comp_rsd_estimations_table_headers;
          widget->columns_ = session_handler_.comp_rsd_estimations_table_body;
          widget->checked_rows_ = table_estimations;
          widget->draw();
          ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Component Group RSD Filters", &comp_group_rsd_estimation_main_window_->visible_))
        {
          session_handler_.setMinimalDataAndFilters(application_handler_.sequenceHandler_);
          session_handler_.setComponentGroupRSDEstimationsTable(application_handler_.sequenceHandler_);
          Eigen::Tensor<bool, 1> table_estimations = session_handler_.getComponentGroupRSDEstimationsTableFilters();
          auto widget = comp_group_rsd_estimation_main_window_;
          widget->headers_ = session_handler_.comp_group_rsd_estimations_table_headers;
          widget->columns_ = session_handler_.comp_group_rsd_estimations_table_body;
          widget->checked_rows_ = table_estimations;
          widget->draw();
          ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Component Background Filters", &comp_background_estimations_main_window_->visible_))
        {
          session_handler_.setMinimalDataAndFilters(application_handler_.sequenceHandler_);
          session_handler_.setComponentBackgroundEstimationsTable(application_handler_.sequenceHandler_);
          Eigen::Tensor<bool, 1> table_estimations = session_handler_.getComponentBackgroundEstimationsTableFilters();
          auto widget = comp_background_estimations_main_window_;
          widget->headers_ = session_handler_.comp_background_estimations_table_headers;
          widget->columns_ = session_handler_.comp_background_estimations_table_body;
          widget->checked_rows_ = table_estimations;
          widget->draw();
          ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Component Group Background Filters", &comp_group_background_estimations_main_window_->visible_))
        {
          session_handler_.setMinimalDataAndFilters(application_handler_.sequenceHandler_);
          session_handler_.setComponentGroupBackgroundEstimationsTable(application_handler_.sequenceHandler_);
          Eigen::Tensor<bool, 1> table_estimations = session_handler_.getComponentGroupBackgroundEstimationsTableFilters();
          auto widget = comp_group_background_estimations_main_window_;
          widget->headers_ = session_handler_.comp_group_background_estimations_table_headers;
          widget->columns_ = session_handler_.comp_group_background_estimations_table_body;
          widget->checked_rows_ = table_estimations;
          widget->draw();
          ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Features matrix", &feature_matrix_main_window_->visible_))
        {
          session_handler_.setMinimalDataAndFilters(application_handler_.sequenceHandler_);
          session_handler_.setFeatureMatrix(application_handler_.sequenceHandler_);
          auto widget = feature_matrix_main_window_;
          widget->headers_ = session_handler_.feature_pivot_table_headers;
          widget->columns_ = session_handler_.feature_pivot_table_body;
          widget->checked_rows_ = Eigen::Tensor<bool, 1>();
          widget->draw();
          ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Chromatograms", &chromatogram_plot_widget_->visible_))
        {
          chromatogram_plot_widget_->setWindowSize(win_size_and_pos.bottom_and_top_window_x_size_, win_size_and_pos.top_window_y_size_);
          if (!workflow_is_done_)
          {
            chromatogram_initialized = false;
          }
          else // workflow_is_done_
          {
            if (!chromatogram_initialized)
            {
              chromatogram_plot_widget_->setRefreshNeeded();
              chromatogram_initialized = true;
            }
          }
          // The actual plot
          chromatogram_plot_widget_->draw();
          ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Spectra", &spectra_plot_widget_->visible_))
        {
          spectra_plot_widget_->setWindowSize(win_size_and_pos.bottom_and_top_window_x_size_, win_size_and_pos.top_window_y_size_);
          if (!workflow_is_done_)
          {
            spectra_initialized = false;
          }
          else // workflow_is_done_
          {
            if (!spectra_initialized)
            {
              spectra_plot_widget_->setRefreshNeeded();
              spectra_initialized = true;
            }
          }
          // The actual plot
          spectra_plot_widget_->draw();
          ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Features (line)", &feature_line_plot_->visible_))
        {
          session_handler_.setMinimalDataAndFilters(application_handler_.sequenceHandler_);
          session_handler_.setFeatureMatrix(application_handler_.sequenceHandler_);
          Eigen::Tensor<float, 2> x_data = session_handler_.feat_sample_data.shuffle(Eigen::array<Eigen::Index, 2>({ 1,0 }));
          Eigen::Tensor<float, 2> y_data = session_handler_.feat_value_data.shuffle(Eigen::array<Eigen::Index, 2>({ 1,0 }));
          feature_line_plot_->setValues(&x_data, &y_data, &session_handler_.feat_col_labels, &session_handler_.feat_row_labels, session_handler_.feat_line_x_axis_title, session_handler_.feat_line_y_axis_title,
            session_handler_.feat_line_sample_min, session_handler_.feat_line_sample_max, session_handler_.feat_value_min, session_handler_.feat_value_max,
            win_size_and_pos.bottom_and_top_window_x_size_, win_size_and_pos.top_window_y_size_, "FeaturesLineMainWindow");
          feature_line_plot_->draw();
          ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Features (heatmap)", &heatmap_plot_widget_->visible_))
        {
          // To be replace when we have implement events
          static bool heatmap_initialized = false;
          if (!workflow_is_done_)
          {
            heatmap_initialized = false;
          }
          else // workflow_is_done_
          {
            if (!heatmap_initialized)
            {
              heatmap_plot_widget_->setRefreshNeeded();
              heatmap_initialized = true;
            }
          }
          heatmap_plot_widget_->setWindowSize(win_size_and_pos.bottom_and_top_window_x_size_, win_size_and_pos.top_window_y_size_);
          heatmap_plot_widget_->draw();
          ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Calibrators", &calibrators_line_plot_->visible_))
        {
          session_handler_.setMinimalDataAndFilters(application_handler_.sequenceHandler_);
          exceeding_plot_points_ = !session_handler_.setCalibratorsScatterLinePlot(application_handler_.sequenceHandler_);
          calibrators_line_plot_->setValues(&session_handler_.calibrators_conc_fit_data, &session_handler_.calibrators_feature_fit_data,
            &session_handler_.calibrators_conc_raw_data, &session_handler_.calibrators_feature_raw_data, &session_handler_.calibrators_series_names,
            session_handler_.calibrators_x_axis_title, session_handler_.calibrators_y_axis_title, session_handler_.calibrators_conc_min, session_handler_.calibrators_conc_max,
            session_handler_.calibrators_feature_min, session_handler_.calibrators_feature_max, win_size_and_pos.bottom_and_top_window_x_size_, win_size_and_pos.top_window_y_size_,
            "CalibratorsMainWindow");
          calibrators_line_plot_->draw();

          ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Statistics", &statistics_->visible_))
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
              statistics_->setRefreshNeeded();
              statistics_initialized = true;
            }
          }
          statistics_->setInjections(session_handler_.injection_explorer_checkbox_body, session_handler_.getInjectionExplorerBody());
          statistics_->setTransitions(&session_handler_.transitions_table_body, session_handler_.transition_explorer_checkbox_body, session_handler_.getTransitionExplorerBody());
          statistics_->setWindowSize(win_size_and_pos.bottom_and_top_window_x_size_, win_size_and_pos.top_window_y_size_);
          statistics_->draw();
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
        if (ImGui::BeginTabItem("Info", &quickInfoText_->visible_))
        {
          ImGui::BeginChild("Info child");
          quickInfoText_->setTransitions(&session_handler_.transitions_table_body);
          quickInfoText_->draw();
          ImGui::EndChild();
          ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Log", &log_widget_->visible_))
        {
          log_widget_->draw();
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
