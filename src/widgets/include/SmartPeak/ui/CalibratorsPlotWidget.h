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
#include <SmartPeak/ui/ParameterEditorWidget2.h>
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
    public IParameterEditorWidgetObserver
  {
  public:
    CalibratorsPlotWidget(SessionHandler& session_handler,
                          SequenceHandler& sequence_handler,
                          std::shared_ptr<ExplorerWidget> explorer_widget,
                          std::shared_ptr<ChromatogramPlotWidget> chromatogram_widget,
                          const std::string title = "") :
      GenericGraphicWidget(title),
      session_handler_(session_handler),
      sequence_handler_(sequence_handler),
      parameter_editor_widget_(*this),
      explorer_widget_(explorer_widget),
      chromatogram_widget_(chromatogram_widget)
    {};
    void setValues(const SessionHandler::CalibrationData& calibration_data, const std::string& plot_title);
    void draw() override;

    bool reset_layout_ = true;

    /* IParameterEditorWidgetObserver */
    virtual void onParameterSet(const Parameter& parameter);

  protected:
    OpenMS::AbsoluteQuantitationMethod* getQuantitationMethod(const std::string& component_name);
    void displayParameters();
    void displayPlot();
    void recomputeCalibration();
    void addParameterRow(std::shared_ptr<Parameter> param);
    void getSelectedPoint(ImVec2 point, ImVec2 threshold_point);
    void showChromatogram(const std::string& sample_name);
    std::optional<std::tuple<int, int>> getSelectedPoint(
      ImVec2 point,
      ImVec2 threshold_point,
      std::vector<std::vector<float>> concentrations_serie,
      std::vector<std::vector<float>> feature_serie);
    std::shared_ptr<Parameter> CalibratorParameterToSmartPeakParameter(const OpenMS::Param::ParamEntry& param);
    SessionHandler::CalibrationData calibration_data_;
    std::string plot_title_; // used as the ID of the plot as well so this should be unique across the different Widgets
    bool show_legend_ = true;
    bool show_fit_line_ = true;
    bool show_points_ = true;
    bool show_outlier_points_ = true;
    std::string current_component_;
    std::vector<std::string> components_;
    std::vector<const char*> component_cstr_;
    int selected_component_ = 0;
    bool reset_zoom_ = true;
    SessionHandler& session_handler_;
    SequenceHandler& sequence_handler_;
    ParameterEditorWidget2 parameter_editor_widget_;
    ParameterSet user_params_;
    std::shared_ptr<Parameter> param_to_edit_;
    std::optional<std::tuple<int, int>> selected_point_;
    std::optional<std::tuple<int, int>> selected_outlier_point_;
    std::optional<std::tuple<int, int>> clicked_point_;
    std::optional<std::tuple<int, int>> clicked_outlier_point_;
    std::shared_ptr<ExplorerWidget> explorer_widget_;
    std::shared_ptr<ChromatogramPlotWidget> chromatogram_widget_;
  };

}