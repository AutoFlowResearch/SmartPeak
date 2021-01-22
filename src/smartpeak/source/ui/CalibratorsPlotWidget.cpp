#include <SmartPeak/ui/CalibratorsPlotWidget.h>
#include <implot.h>

namespace SmartPeak
{
  void CalibratorsPlotWidget::draw()
  {
    // Main graphic
    ImPlot::SetNextPlotLimits(x_min_, x_max_, y_min_, y_max_, ImGuiCond_Always);
    if (ImPlot::BeginPlot(plot_title_.c_str(), x_axis_title_.c_str(), y_axis_title_.c_str(), ImVec2(plot_width_ - 25, plot_height_ - 40))) {
      for (int i = 0; i < x_raw_data_.size(); ++i) {
        assert(x_raw_data_.at(i).size() == y_raw_data_.at(i).size());
        ImPlot::PushStyleVar(ImPlotStyleVar_Marker, ImPlotMarker_Circle);
        ImPlot::PlotScatter((series_names_.at(i) + "-pts").c_str(), x_raw_data_.at(i).data(), y_raw_data_.at(i).data(), x_raw_data_.at(i).size());
      }
      for (int i = 0; i < x_fit_data_.size(); ++i) {
        assert(x_fit_data_.at(i).size() == y_fit_data_.at(i).size());
        ImPlot::PushStyleVar(ImPlotStyleVar_LineWeight, ImPlot::GetStyle().LineWeight);
        ImPlot::PushStyleVar(ImPlotStyleVar_Marker, ImPlotMarker_Circle);
        ImPlot::PlotLine((series_names_.at(i) + "-fit").c_str(), x_fit_data_.at(i).data(), y_fit_data_.at(i).data(), x_fit_data_.at(i).size());
      }
      ImPlot::EndPlot();
    }
  }
}
