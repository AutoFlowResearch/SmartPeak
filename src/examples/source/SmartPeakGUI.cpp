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
#include <SmartPeak/ui/SequenceTableWidget.h>
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

void checkTitles(const std::vector<std::shared_ptr<Widget>> windows);

struct MainGuiObserver : public ISequenceObserver
{
  MainGuiObserver(SequenceHandler& sequenceHandler_)
  {
    sequenceHandler_.addSequenceObserver(this);
  }

  /**
  ISequenceObserver
  */
  virtual void sequenceUpdated() override
  {
    sequence_updated = true;
  }

  bool sequence_updated = false;
};

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

  ApplicationHandler application_handler_;
  SessionHandler session_handler_;
  WorkflowManager workflow_manager_;
  GuiAppender appender_;

  MainGuiObserver main_gui_observer_(application_handler_.sequenceHandler_);

  // widgets
  FilePicker file_picker_;
  auto quickInfoText_= std::make_shared<InfoWidget>("Info", application_handler_);
  auto report_ = std::make_shared<Report>(application_handler_);
  auto workflow_ = std::make_shared<Workflow>("Workflow", application_handler_);
  auto statistics_ = std::make_shared<StatisticsWidget>("Statistics", application_handler_);
  auto run_workflow_widget_ = std::make_shared<RunWorkflowWidget>(application_handler_, session_handler_, workflow_manager_);
  auto about_widget_ = std::make_shared<AboutWidget>();
  auto log_widget_ = std::make_shared<LogWidget>(appender_, "Log");
  auto parameters_table_widget_ = std::make_shared<ParametersTableWidget>(session_handler_, application_handler_, "ParametersMainWindow", "Parameters");
  auto chromatogram_plot_widget_ = std::make_shared<ChromatogramPlotWidget>(session_handler_, application_handler_.sequenceHandler_, "Chromatograms Main Window", "Chromatograms");
  auto heatmap_plot_widget_ = std::make_shared<Heatmap2DWidget>(session_handler_, application_handler_.sequenceHandler_, "Heatmap Main Window", "Features (heatmap)");
  auto spectra_plot_widget_ = std::make_shared<SpectraPlotWidget>(session_handler_, application_handler_.sequenceHandler_, "Spectra Main Window", "Spectra");
  auto feature_line_plot_ = std::make_shared<LinePlot2DWidget>("Features (line)");
  auto calibrators_line_plot_ = std::make_shared<CalibratorsPlotWidget>("Calibrators");
  auto injections_explorer_window_ = std::make_shared<ExplorerWidget>("InjectionsExplorerWindow", "Injections");
  auto transitions_explorer_window_ = std::make_shared<ExplorerWidget>("TransitionsExplorerWindow", "Transitions");
  auto features_explorer_window_ = std::make_shared<ExplorerWidget>("FeaturesExplorerWindow", "Features");
  auto spectrum_explorer_window_ = std::make_shared<ExplorerWidget>("SpectrumExplorerWindow", "Spectrum");
  auto sequence_main_window_ = std::make_shared<SequenceTableWidget>("SequenceMainWindow", "Sequence",
                                                                      &session_handler_, &application_handler_.sequenceHandler_);
  auto transitions_main_window_ = std::make_shared<GenericTableWidget>("TransitionsMainWindow", "Transitions");
  auto spectrum_main_window_ = std::make_shared<GenericTableWidget>("SpectrumMainWindow", "Spectrum");
  auto quant_method_main_window_ = std::make_shared<GenericTableWidget>("QuantMethodMainWindow", "Quantitation Method",
                                   &session_handler_, &application_handler_.sequenceHandler_,
                                   &SessionHandler::setQuantMethodTable, &SessionHandler::getFiltersTable);
  auto stds_consc_main_window_ = std::make_shared<GenericTableWidget>("StdsConcsMainWindow", "Standards Concentrations",
                                   &session_handler_, &application_handler_.sequenceHandler_,
                                   &SessionHandler::setStdsConcsTable, &SessionHandler::getFiltersTable);
  auto comp_filters_main_window_ = std::make_shared<GenericTableWidget>("CompFiltersMainWindow", "Component Filters",
                                   &session_handler_, &application_handler_.sequenceHandler_,
                                   &SessionHandler::setComponentFiltersTable, &SessionHandler::getFiltersTable);
  auto comp_group_filters_main_window_ = std::make_shared<GenericTableWidget>("CompGroupFiltersMainWindow", "Component Group Filters",
                                   &session_handler_, &application_handler_.sequenceHandler_,
                                   &SessionHandler::setComponentGroupFiltersTable, &SessionHandler::getGroupFiltersTable);
  auto comp_qc_main_window_ = std::make_shared<GenericTableWidget>("CompQCsMainWindow", "Component QCs",
                                   &session_handler_, &application_handler_.sequenceHandler_,
                                   &SessionHandler::setComponentQCsTable, &SessionHandler::getFiltersTable);
  auto comp_group_qc_main_window_ = std::make_shared<GenericTableWidget>("CompGroupQCsMainWindow", "Component Group QCs",
                                   &session_handler_, &application_handler_.sequenceHandler_,
                                   &SessionHandler::setComponentGroupQCsTable, &SessionHandler::getGroupFiltersTable);
  auto comp_rsd_filters_main_window_ = std::make_shared<GenericTableWidget>("CompRSDFiltersMainWindow", "Component RSD Filters",
                                   &session_handler_, &application_handler_.sequenceHandler_,
                                   &SessionHandler::setComponentRSDFiltersTable, &SessionHandler::getFiltersTable);
  auto comp_group_rds_filters_main_window_ = std::make_shared<GenericTableWidget>("CompGroupRSDFiltersMainWindow", "Component Group RSD Filters",
                                   &session_handler_, &application_handler_.sequenceHandler_,
                                   &SessionHandler::setComponentGroupRSDFiltersTable, &SessionHandler::getGroupFiltersTable);
  auto comp_rsdcqcs_main_window_ = std::make_shared<GenericTableWidget>("CompRSDQCsMainWindow", "Component RSD QCs",
                                   &session_handler_, &application_handler_.sequenceHandler_,
                                   &SessionHandler::setComponentRSDQCsTable, &SessionHandler::getFiltersTable);
  auto comp_group_rsdqcs_main_window_ = std::make_shared<GenericTableWidget>("CompGroupRSDQCsMainWindow", "Component Group RSD QCs",
                                   &session_handler_, &application_handler_.sequenceHandler_, 
                                   &SessionHandler::setComponentGroupRSDQCsTable, &SessionHandler::getGroupFiltersTable);
  auto comp_background_filters_main_window_ = std::make_shared<GenericTableWidget>("CompBackgroundFiltersMainWindow", "Component Background Filters",
                                   &session_handler_, &application_handler_.sequenceHandler_, 
                                   &SessionHandler::setComponentBackgroundFiltersTable, &SessionHandler::getFiltersTable);
  auto comp_group_background_filters_main_window_ = std::make_shared<GenericTableWidget>("CompGroupBackgroundFiltersMainWindow", "Component Group Background Filters",
                                   &session_handler_, &application_handler_.sequenceHandler_, 
                                   &SessionHandler::setComponentGroupBackgroundFiltersTable, &SessionHandler::getGroupFiltersTable);
  auto comp_background_qcs_main_window_ = std::make_shared<GenericTableWidget>("CompBackgroundQCsMainWindow", "Component Background QCs",
                                   &session_handler_, &application_handler_.sequenceHandler_, 
                                   &SessionHandler::setComponentBackgroundQCsTable, &SessionHandler::getFiltersTable);
  auto comp_group_background_qcs_main_window_ = std::make_shared<GenericTableWidget>("CompGroupBackgroundQCsMainWindow", "Component Group Background QCs",
                                   &session_handler_, &application_handler_.sequenceHandler_,
                                   &SessionHandler::setComponentGroupBackgroundQCsTable, &SessionHandler::getGroupFiltersTable);
  auto comp_rsd_estimations_main_window_ = std::make_shared<GenericTableWidget>("CompRSDEstimationsMainWindow", "Component RSD Filters",
                                   &session_handler_, &application_handler_.sequenceHandler_,
                                   &SessionHandler::setComponentRSDEstimationsTable, &SessionHandler::getFiltersTable);
  auto comp_group_rsd_estimation_main_window_ = std::make_shared<GenericTableWidget>("CompGroupRSDEstimationsMainWindow", "Component Group RSD Filters",
                                   &session_handler_, &application_handler_.sequenceHandler_,
                                   &SessionHandler::setComponentGroupRSDEstimationsTable, &SessionHandler::getGroupFiltersTable);
  auto comp_background_estimations_main_window_ = std::make_shared<GenericTableWidget>("CompBackgroundEstimationsMainWindow", "Component Background Filters",
                                   &session_handler_, &application_handler_.sequenceHandler_,
                                   &SessionHandler::setComponentBackgroundEstimationsTable, &SessionHandler::getFiltersTable);
  auto comp_group_background_estimations_main_window_ = std::make_shared<GenericTableWidget>("CompGroupBackgroundEstimationsMainWindow", "Component Group Background Filters",
                                   &session_handler_, &application_handler_.sequenceHandler_,
                                   &SessionHandler::setComponentGroupBackgroundEstimationsTable, &SessionHandler::getGroupFiltersTable);
  auto features_table_main_window_ = std::make_shared<GenericTableWidget>("featuresTableMainWindow", "Features table");
  auto feature_matrix_main_window_ = std::make_shared<GenericTableWidget>("featureMatrixMainWindow", "Features matrix");

  // visible on start
  workflow_->visible_ = true;
  quickInfoText_->visible_ = true;
  injections_explorer_window_->visible_ = true;
  transitions_explorer_window_->visible_ = true;

  // windows organization
  std::vector<std::shared_ptr<Widget>> top_windows = {
    statistics_,
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
    log_widget_,
  };

  std::vector<std::shared_ptr<Widget>> left_windows = {
    injections_explorer_window_,
    transitions_explorer_window_,
    features_explorer_window_,
    spectrum_explorer_window_
  };

  // We need titles for all sub windows
  checkTitles(top_windows);
  checkTitles(bottom_windows);
  checkTitles(left_windows);

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
      bool workflow_just_finished = (!workflow_is_done_) && workflow_manager_.isWorkflowDone();
      if (workflow_just_finished)
      {
        workflow_manager_.updateApplicationHandler(application_handler_);
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
      if (workflow_just_finished)
      {
        quickInfoText_->setLastRunTime(workflow_manager_.getLastRunTime());
        quickInfoText_->setRefreshNeeded();
      }

      // Session cache updates from e.g., single file imports
      if (file_loading_is_done_ && file_picker_.fileWasLoaded() && update_session_cache_) {
        if (file_picker_.getProcessorName() == "LoadSequenceParameters") {
          session_handler_.sequence_table.clear();
        }
        else if (file_picker_.getProcessorName() == "LoadSequenceTransitions") {
          session_handler_.transitions_table.clear();
        }
        else if (file_picker_.getProcessorName() == "LoadSequenceSegmentQuantitationMethods") {
          quant_method_main_window_->table_data_.clear();
        }
        else if (file_picker_.getProcessorName() == "LoadSequenceSegmentStandardsConcentrations") {
          stds_consc_main_window_->table_data_.clear();
        }
        else if (file_picker_.getProcessorName() == "LoadSequenceSegmentFeatureFilterComponents") {
          comp_filters_main_window_->table_data_.clear();
        }
        else if (file_picker_.getProcessorName() == "LoadSequenceSegmentFeatureFilterComponentGroups") {
          comp_group_filters_main_window_->table_data_.clear();
        }
        else if (file_picker_.getProcessorName() == "LoadSequenceSegmentFeatureQCComponents") {
          comp_qc_main_window_->table_data_.clear();
        }
        else if (file_picker_.getProcessorName() == "LoadSequenceSegmentFeatureQCComponentGroups") {
          comp_group_qc_main_window_->table_data_.clear();
        }
        update_session_cache_ = false;
      }
      if (main_gui_observer_.sequence_updated)
      {
        session_handler_.sequence_table.clear();
        main_gui_observer_.sequence_updated = false;
      }

    // ======================================
    // Popups
    // ======================================
    if (file_picker_.visible_)
    {
      ImGui::OpenPopup("Pick a pathname");
      file_picker_.draw();
    }
    if (run_workflow_widget_->visible_)
    {
      ImGui::OpenPopup("Run workflow modal");
      run_workflow_widget_->draw();
    }
    if (about_widget_->visible_)
    {
      ImGui::OpenPopup("About");
      about_widget_->draw();
    }
    if (report_->visible_)
    {
      ImGui::OpenPopup("Report dialog");
      report_->draw();
    }

    // ======================================
    // Menu
    // ======================================
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
          report_->visible_ = true;
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

    // ======================================
    // Window size computation
    // ======================================
    bool show_top_window_ = std::find_if(top_windows.begin(), top_windows.end(), [](const auto& w) { return w->visible_; }) != top_windows.end();
    bool show_bottom_window_ = std::find_if(bottom_windows.begin(), bottom_windows.end(), [](const auto& w) { return w->visible_; }) != bottom_windows.end();
    bool show_left_window_ = std::find_if(left_windows.begin(), left_windows.end(), [](const auto& w) { return w->visible_; }) != left_windows.end();
    bool show_right_window_ = false;
    win_size_and_pos.setWindowSizesAndPositions(show_top_window_, show_bottom_window_, show_left_window_, show_right_window_);

    // ======================================
    // Data updates
    //
    // (Widgets should update their data 
    // themself, which is not the case for
    // all of them yet)
    // ======================================
    
    session_handler_.setMinimalDataAndFilters(application_handler_.sequenceHandler_);

    // Sequence
    if (sequence_main_window_->visible_)
    {
      sequence_main_window_->checked_rows_ = session_handler_.getSequenceTableFilters();
      sequence_main_window_->table_data_ = session_handler_.sequence_table;
    }

    // Transitions
    if (transitions_main_window_->visible_)
    {
      transitions_main_window_->checked_rows_ = session_handler_.getTransitionsTableFilters();
      transitions_main_window_->table_data_ = session_handler_.transitions_table;
    }

    // spectrum
    if (spectrum_main_window_->visible_)
    {
      spectrum_main_window_->checked_rows_ = session_handler_.getSpectrumTableFilters();
      spectrum_main_window_->table_data_ = session_handler_.spectrum_table;
    }

    // workflow
    if (workflow_->visible_)
    {
      workflow_->setEditable(workflow_is_done_);
    }

    // feature table
    if (features_table_main_window_->visible_)
    {
      exceeding_table_size_ = !session_handler_.setFeatureTable(application_handler_.sequenceHandler_, features_table_main_window_->table_data_);
      features_table_main_window_->checked_rows_ = Eigen::Tensor<bool, 1>();
    }

    // feature matrix
    if (feature_matrix_main_window_)
    {
        session_handler_.setFeatureMatrix(application_handler_.sequenceHandler_);
        feature_matrix_main_window_->table_data_ = session_handler_.feature_pivot_table;
        feature_matrix_main_window_->checked_rows_ = Eigen::Tensor<bool, 1>();
    }
    
    // Chromatogram
    if (workflow_just_finished)
    {
      chromatogram_plot_widget_->setRefreshNeeded();
    }

    // spectra
    if (workflow_just_finished)
    {
      spectra_plot_widget_->setRefreshNeeded();
    }

    // feature line plot
    if (feature_line_plot_->visible_)
    {
      session_handler_.setFeatureMatrix(application_handler_.sequenceHandler_);
      Eigen::Tensor<float, 2> x_data = session_handler_.feat_sample_data.shuffle(Eigen::array<Eigen::Index, 2>({ 1,0 }));
      Eigen::Tensor<float, 2> y_data = session_handler_.feat_value_data.shuffle(Eigen::array<Eigen::Index, 2>({ 1,0 }));
      feature_line_plot_->setValues(x_data, y_data, &session_handler_.feat_col_labels, &session_handler_.feat_row_labels, session_handler_.feat_line_x_axis_title, session_handler_.feat_line_y_axis_title,
        session_handler_.feat_line_sample_min, session_handler_.feat_line_sample_max, session_handler_.feat_value_min, session_handler_.feat_value_max,
        "FeaturesLineMainWindow");
    }

    // heatmap
    if (workflow_just_finished)
    {
      heatmap_plot_widget_->setRefreshNeeded();
    }

    //statistics
    if (workflow_just_finished)
    {
      statistics_->setRefreshNeeded();
    }
    if (statistics_->visible_)
    {
      statistics_->setInjections(session_handler_.injection_explorer_data.checkbox_body, session_handler_.getInjectionExplorerBody());
      statistics_->setTransitions(&session_handler_.transitions_table.body_, session_handler_.transition_explorer_data.checkbox_body, session_handler_.getTransitionExplorerBody());
    }

    // info
    if (quickInfoText_->visible_)
    {
      quickInfoText_->setTransitions(&session_handler_.transitions_table.body_);
    }

    // injections 
    if (injections_explorer_window_->visible_)
    {
      injections_explorer_window_->table_data_.headers_ = session_handler_.getInjectionExplorerHeader();
      injections_explorer_window_->table_data_.body_ = session_handler_.getInjectionExplorerBody();
      injections_explorer_window_->checked_rows_ = session_handler_.injection_explorer_data.checked_rows;
      injections_explorer_window_->checkbox_headers_ = session_handler_.injection_explorer_data.checkbox_headers;
      injections_explorer_window_->checkbox_columns_ = &session_handler_.injection_explorer_data.checkbox_body;
    }

    // transitions
    if (transitions_explorer_window_->visible_)
    {
      transitions_explorer_window_->table_data_.headers_ = session_handler_.getTransitionExplorerHeader();
      transitions_explorer_window_->table_data_.body_ = session_handler_.getTransitionExplorerBody();
      transitions_explorer_window_->checked_rows_ = session_handler_.transition_explorer_data.checked_rows;
      transitions_explorer_window_->checkbox_headers_ = session_handler_.transition_explorer_data.checkbox_headers;
      transitions_explorer_window_->checkbox_columns_ = &session_handler_.transition_explorer_data.checkbox_body;
    }

    //features
    if (features_explorer_window_->visible_)
    {
      session_handler_.setFeatureExplorer();
      features_explorer_window_->table_data_ = session_handler_.feature_table;
      features_explorer_window_->checked_rows_ = session_handler_.feature_explorer_data.checked_rows;
      features_explorer_window_->checkbox_headers_ = session_handler_.feature_explorer_data.checkbox_headers;
      features_explorer_window_->checkbox_columns_ = &session_handler_.feature_explorer_data.checkbox_body;
    }

    // spectrum
    if (spectrum_explorer_window_->visible_)
    {
      spectrum_explorer_window_->table_data_.headers_ = session_handler_.getSpectrumExplorerHeader();
      spectrum_explorer_window_->table_data_.body_ = session_handler_.getSpectrumExplorerBody();
      spectrum_explorer_window_->checked_rows_ = session_handler_.spectrum_explorer_data.checked_rows;
      spectrum_explorer_window_->checkbox_headers_ = session_handler_.spectrum_explorer_data.checkbox_headers;
      spectrum_explorer_window_->checkbox_columns_ = &session_handler_.spectrum_explorer_data.checkbox_body;
    }

    // calibrators
    if (calibrators_line_plot_->visible_)
    {
      exceeding_plot_points_ = !session_handler_.setCalibratorsScatterLinePlot(application_handler_.sequenceHandler_);
      calibrators_line_plot_->setValues(&session_handler_.calibrators_conc_fit_data, &session_handler_.calibrators_feature_fit_data,
        &session_handler_.calibrators_conc_raw_data, &session_handler_.calibrators_feature_raw_data, &session_handler_.calibrators_series_names,
        session_handler_.calibrators_x_axis_title, session_handler_.calibrators_y_axis_title, session_handler_.calibrators_conc_min, session_handler_.calibrators_conc_max,
        session_handler_.calibrators_feature_min, session_handler_.calibrators_feature_max,
        "CalibratorsMainWindow");
    }


    // ======================================
    // Windows display
    // ======================================
    
    // windfow flags common to top, left and bottom windows
    const ImGuiWindowFlags window_flags =
      ImGuiWindowFlags_NoTitleBar |
      ImGuiWindowFlags_NoMove |
      ImGuiWindowFlags_NoCollapse |
      ImGuiWindowFlags_NoFocusOnAppearing;

    // Left window
    if (show_left_window_) {
      ImGui::SetNextWindowPos(ImVec2(win_size_and_pos.left_window_x_pos_, win_size_and_pos.left_and_right_window_y_pos_));
      ImGui::SetNextWindowSize(ImVec2(win_size_and_pos.left_window_x_size_, win_size_and_pos.left_and_right_window_y_size_));
      ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0);
      ImGui::Begin("Left window", NULL, window_flags);
      if (ImGui::BeginTabBar("Left window tab bar", ImGuiTabBarFlags_Reorderable))
      {
        for (auto& widget : left_windows)
        {
          if (ImGui::BeginTabItem(widget->title_.c_str(), &widget->visible_))
          {
            widget->setWindowSize(win_size_and_pos.left_window_x_size_, win_size_and_pos.left_and_right_window_y_size_);
            widget->draw();
            ImGui::EndTabItem();
          }
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
      ImGui::Begin("Top window", NULL, window_flags);
      if (ImGui::BeginTabBar("Top window tab bar", ImGuiTabBarFlags_Reorderable))
      {
        for (auto& widget : top_windows)
        {
          if (ImGui::BeginTabItem(widget->title_.c_str(), &widget->visible_))
          {
            widget->setWindowSize(win_size_and_pos.bottom_and_top_window_x_size_, win_size_and_pos.top_window_y_size_);
            widget->draw();
            ImGui::EndTabItem();
          }
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
      ImGui::Begin("Bottom window", NULL, window_flags);
      if (ImGui::BeginTabBar("Bottom window tab bar", ImGuiTabBarFlags_Reorderable))
      {
        for (auto& widget : bottom_windows)
        {
          if (ImGui::BeginTabItem(widget->title_.c_str(), &widget->visible_))
          {
            widget->setWindowSize(win_size_and_pos.bottom_and_top_window_x_size_, win_size_and_pos.bottom_window_y_size_);
            widget->draw();
            ImGui::EndTabItem();
          }
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

void checkTitles(const std::vector<std::shared_ptr<Widget>> windows)
{
  for (const auto& window : windows)
  {
    assert(window->title_ != std::string(""));
  }
}
