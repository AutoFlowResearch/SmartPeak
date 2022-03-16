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
// $Maintainer: Douglas McCloskey, Bertrand Boudaud $
// $Authors: Douglas McCloskey, Bertrand Boudaud $
// --------------------------------------------------------------------------

#pragma once

#include <SmartPeak/core/SessionHandler.h>
#include <SmartPeak/ui/Widget.h>
#include <SmartPeak/ui/ParameterEditorWidget.h>
#include <SmartPeak/ui/ExplorerWidget.h>
#include <SmartPeak/ui/ChromatogramPlotWidget.h>

#include <string>
#include <utility>
#include <vector>
#include <imgui.h>

namespace SmartPeak
{

  /**
    @brief Class for Calibrators
  */
  class CalibratorsPlotWidget : 
    public GenericGraphicWidget,
    public IParameterEditorWidgetObserver,
    public ISequenceObserver
  {
  public:
    CalibratorsPlotWidget(SessionHandler& session_handler,
                          SequenceHandler& sequence_handler,
                          std::shared_ptr<ExplorerWidget> explorer_widget,
                          std::shared_ptr<ChromatogramPlotWidget> chromatogram_widget,
                          SequenceObservable& sequence_observable,
                          const std::string title = "") :
      GenericGraphicWidget(title),
      session_handler_(session_handler),
      sequence_handler_(sequence_handler),
      parameter_editor_widget_(*this, false),
      explorer_widget_(explorer_widget),
      chromatogram_widget_(chromatogram_widget)
    {
      sequence_observable.addSequenceObserver(this);
    };

    void draw() override;

    bool reset_layout_ = true;

    /* IParameterEditorWidgetObserver */
    virtual void onParameterSet(const std::string& function_parameter, const Parameter& parameter);
    virtual void onParameterRemoved(const std::string& function_parameter, const Parameter& parameter) {};

    /* ISequenceObserver */
    virtual void onSequenceUpdated() override;

  protected:
    void setCalibrationData(const SessionHandler::CalibrationData& calibration_data, const std::string& plot_title);
    void removeExcludedFromMatchingPoints();
    OpenMS::AbsoluteQuantitationMethod* getQuantitationMethod(const std::string& component_name);
    void displayParameters();
    void displayPlot();
    void recomputeCalibration();
    void addParameterRow(std::shared_ptr<Parameter> param, bool editable);
    void getSelectedPoint(ImVec2 point, ImVec2 threshold_point);
    void showChromatogram(const std::string& sample_name);
    void plotPoints(bool show_flag, const SessionHandler::CalibrationData::Points& points, int marker_style);
    void plotSelectedPoint(
      const std::optional<std::tuple<int, int>>& hovered_point,
      const SessionHandler::CalibrationData::Points& points,
      int marker_style);
    std::tuple<std::string, std::string> getSampleNameAndSerieFromSelectedPoint(
      const std::optional<std::tuple<int, int>>& matching_point, 
      const std::optional<std::tuple<int, int>>& outlier_point,
      const std::optional<std::tuple<int, int>>& excluded_point) const;
    std::optional<std::tuple<int, int>> getSelectedPoint(
      ImVec2 point,
      ImVec2 threshold_point,
      const SessionHandler::CalibrationData::Points& points);
    std::shared_ptr<Parameter> CalibratorParameterToSmartPeakParameter(const OpenMS::Param::ParamEntry& param);

  protected:
    SessionHandler::CalibrationData calibration_data_;
    std::string plot_title_; // used as the ID of the plot as well so this should be unique across the different Widgets
    bool show_legend_ = true;
    bool show_fit_line_ = true;
    bool show_matching_points_ = true;
    bool show_outlier_points_ = true;
    bool show_excluded_points_ = true;
    std::string current_component_;
    std::vector<std::string> components_;
    std::vector<const char*> component_cstr_;
    int selected_component_ = 0;
    std::vector<std::string> sequence_segments_;
    std::vector<const char*> sequence_segments_cstr_;
    int selected_sequence_segment_ = 0;
    enum Action
    {
      EActionFitCalibration,
      EActionOptimizeCalibration
    };
    std::vector<const char*> actions_cstr_ = 
    {
      "Fit Calibration",
      "Optimize Calibration"
    };
    int selected_action_ = EActionFitCalibration;
    bool reset_zoom_ = true;
    SessionHandler& session_handler_;
    SequenceHandler& sequence_handler_;
    ParameterEditorWidget parameter_editor_widget_;
    std::shared_ptr<Parameter> param_to_edit_;
    std::optional<std::tuple<int, int>> hovered_matching_point_;
    std::optional<std::tuple<int, int>> hovered_outlier_point_;
    std::optional<std::tuple<int, int>> hovered_excluded_point_;
    std::optional<std::tuple<int, int>> clicked_matching_point_;
    std::optional<std::tuple<int, int>> clicked_outlier_point_;
    std::optional<std::tuple<int, int>> clicked_excluded_point_;
    std::shared_ptr<ExplorerWidget> explorer_widget_;
    std::shared_ptr<ChromatogramPlotWidget> chromatogram_widget_;
    bool refresh_needed_ = true;
    std::vector<bool> previous_transition_selection_;
  };

}