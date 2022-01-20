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
// $Maintainer: Bertrand Boudaud, Douglas McCloskey $
// $Authors: Bertrand Boudaud $
// --------------------------------------------------------------------------

#include <SmartPeak/ui/CalibratorsPlotWidget.h>
#include <implot.h>

namespace SmartPeak
{
  void CalibratorsPlotWidget::displayParameters()
  {
    ImGui::Begin("Calibrator Parameters");
    const auto& quantitation_methods = calibration_data_.quant_method;

    ImGui::LabelText("name", quantitation_methods.getISName().c_str());
    ImGui::LabelText("component name", quantitation_methods.getComponentName().c_str());

    ImGui::LabelText("llod", "%f", quantitation_methods.getLLOD());
    ImGui::LabelText("ulod", "%f", quantitation_methods.getULOD());

    ImGui::LabelText("lloq", "%f", quantitation_methods.getLLOQ());
    ImGui::LabelText("uloq", "%f", quantitation_methods.getULOQ());

    ImGui::LabelText("correlation coefficient", "%f", quantitation_methods.getCorrelationCoefficient());
    ImGui::LabelText("nb points", "%d", quantitation_methods.getNPoints());

    ImGui::LabelText("transformation model", quantitation_methods.getTransformationModel().c_str());

    const auto& params = quantitation_methods.getTransformationModelParams();
    for (const auto& param : params)
    {
      const auto& value = param.value;
      ImGui::LabelText(param.name.c_str(), "%s", value.toString().c_str());
    }
    ImGui::End();
  }

  void CalibratorsPlotWidget::displayPlot()
  {
    ImGui::Begin("Calibrator Plot");
    ImGui::Checkbox("Legend", &show_legend_);
    // Main graphic
    ImPlot::SetNextPlotLimits(calibration_data_.calibrators_conc_min, calibration_data_.calibrators_conc_max, calibration_data_.calibrators_feature_min, calibration_data_.calibrators_feature_max);
    auto window_size = ImGui::GetWindowSize();
    ImPlotFlags plotFlags = show_legend_ ? ImPlotFlags_Default | ImPlotFlags_Legend : ImPlotFlags_Default & ~ImPlotFlags_Legend;
    if (ImPlot::BeginPlot(plot_title_.c_str(), 
                          calibration_data_.calibrators_x_axis_title.c_str(), 
                          calibration_data_.calibrators_y_axis_title.c_str(), 
                          ImVec2(window_size.x - 25, window_size.y - 58),
                          plotFlags)) {
      for (int i = 0; i < calibration_data_.calibrators_conc_fit_data.size(); ++i) {
        assert(calibration_data_.calibrators_conc_fit_data.at(i).size() == calibration_data_.calibrators_feature_fit_data.at(i).size());
        ImPlot::PushStyleVar(ImPlotStyleVar_LineWeight, ImPlot::GetStyle().LineWeight);
        ImPlot::PushStyleVar(ImPlotStyleVar_Marker, ImPlotMarker_Circle);
        ImPlot::PlotLine((calibration_data_.calibrators_series_names.at(i) + "-fit").c_str(), calibration_data_.calibrators_conc_fit_data.at(i).data(), calibration_data_.calibrators_feature_fit_data.at(i).data(), calibration_data_.calibrators_conc_fit_data.at(i).size());
      }
      for (int i = 0; i < calibration_data_.calibrators_conc_raw_data.size(); ++i) {
        assert(calibration_data_.calibrators_conc_raw_data.at(i).size() == calibration_data_.calibrators_feature_raw_data.at(i).size());
        ImPlot::PushStyleVar(ImPlotStyleVar_Marker, ImPlotMarker_Circle);
        ImPlot::PlotScatter((calibration_data_.calibrators_series_names.at(i) + "-pts").c_str(), calibration_data_.calibrators_conc_raw_data.at(i).data(), calibration_data_.calibrators_feature_raw_data.at(i).data(), calibration_data_.calibrators_conc_raw_data.at(i).size());
      }
      ImPlot::EndPlot();
    }
    ImGui::End();
  }

  void CalibratorsPlotWidget::draw()
  {
    if (!calibration_data_.calibrators_conc_raw_data.size())
    {
      return;
    }
    showQuickHelpToolTip("CalibratorsPlotWidget");

    // Build default docking
    ImGuiID dockspace_id = ImGui::GetID("CalibratorsPlotWidgetDockSpace");
    if (reset_layout_)
    {
      ImGuiID left_node;
      ImGuiID center_node;
      ImGuiID bottom_node;
      ImGui::DockBuilderAddNode(dockspace_id, ImGuiDockNodeFlags_DockSpace);
      ImGui::DockBuilderSetNodeSize(dockspace_id, ImVec2(400,400));
      ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Left, 0.3, &left_node, &center_node); // The second parameter defines the direction of the split
      ImGui::DockBuilderDockWindow("Calibrator Parameters", left_node);
      ImGui::DockBuilderDockWindow("Calibrator Plot", center_node);
      ImGui::DockBuilderFinish(dockspace_id);
      reset_layout_ = false;
    }
    ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_AutoHideTabBar);
    
    displayParameters();
    displayPlot();
  }
}
