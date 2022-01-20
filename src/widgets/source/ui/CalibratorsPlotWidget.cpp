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

    ImGuiTableFlags table_flags = ImGuiTableFlags_None;
    if (ImGui::BeginTable("Calibrator Parameters Table", 2, table_flags))
    {
      /*
      ImGui::TableSetupColumn("Parameter name");
      ImGui::TableSetupColumn("Parameter value");
      ImGui::TableHeadersRow();
      */
      ImGui::TableNextRow(); 
      ImGui::TableSetColumnIndex(0);
      ImGui::Text("name");
      ImGui::TableSetColumnIndex(1);
      ImGui::Text(quantitation_methods.getISName().c_str());

      ImGui::TableNextRow();
      ImGui::TableSetColumnIndex(0);
      ImGui::Text("component name");
      ImGui::TableSetColumnIndex(1);
      ImGui::Text(quantitation_methods.getComponentName().c_str());

      ImGui::TableNextRow();
      ImGui::TableSetColumnIndex(0);
      ImGui::Text("llod");
      ImGui::TableSetColumnIndex(1);
      ImGui::Text("%f", quantitation_methods.getLLOD());

      ImGui::TableNextRow();
      ImGui::TableSetColumnIndex(0);
      ImGui::Text("ulod");
      ImGui::TableSetColumnIndex(1);
      ImGui::Text("%f", quantitation_methods.getULOD());

      ImGui::TableNextRow();
      ImGui::TableSetColumnIndex(0);
      ImGui::Text("lloq");
      ImGui::TableSetColumnIndex(1);
      ImGui::Text("%f", quantitation_methods.getLLOQ());

      ImGui::TableNextRow();
      ImGui::TableSetColumnIndex(0);
      ImGui::Text("uloq");
      ImGui::TableSetColumnIndex(1);
      ImGui::Text("%f", quantitation_methods.getULOQ());

      ImGui::TableNextRow();
      ImGui::TableSetColumnIndex(0);
      ImGui::Text("correlation coefficient");
      ImGui::TableSetColumnIndex(1);
      ImGui::Text("%f", quantitation_methods.getCorrelationCoefficient());

      ImGui::TableNextRow();
      ImGui::TableSetColumnIndex(0);
      ImGui::Text("nb points");
      ImGui::TableSetColumnIndex(1);
      ImGui::Text("%d", quantitation_methods.getNPoints());

      ImGui::TableNextRow();
      ImGui::TableSetColumnIndex(0);
      ImGui::Text("transformation model");
      ImGui::TableSetColumnIndex(1);
      ImGui::Text(quantitation_methods.getTransformationModel().c_str());

      const auto& params = quantitation_methods.getTransformationModelParams();
      for (const auto& param : params)
      {
        const auto& value = param.value;
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text(param.name.c_str());
        ImGui::TableSetColumnIndex(1);
        ImGui::Text("%s", value.toString().c_str());
      }
      ImGui::EndTable();
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
      ImGuiID right_node;
      ImGui::DockBuilderAddNode(dockspace_id, ImGuiDockNodeFlags_DockSpace);
      ImGui::DockBuilderSetNodeSize(dockspace_id, ImGui::GetCurrentWindow()->Size);
      ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Left, 0.5f, &left_node, &right_node); // The second parameter defines the direction of the split
      ImGui::DockBuilderDockWindow("Calibrator Parameters", left_node);
      ImGui::DockBuilderDockWindow("Calibrator Plot", right_node);
      ImGui::DockBuilderFinish(dockspace_id);
      reset_layout_ = false;
    }
    ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_AutoHideTabBar);
    
    displayParameters();
    displayPlot();
  }
}
