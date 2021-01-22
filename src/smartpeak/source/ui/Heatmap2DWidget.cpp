#include <SmartPeak/ui/Heatmap2DWidget.h>
#include <implot.h>

namespace SmartPeak
{

  void Heatmap2DWidget::draw()
  {
    // Main graphic
    if (rows_.size() > 1 || columns_.size() > 1) {
      assert(data_.dimension(0) == rows_.size() && data_.dimension(1) == columns_.size());
      const ImPlotFlags imPlotFlags = ImPlotFlags_MousePos | ImPlotFlags_Highlight | ImPlotFlags_BoxSelect | ImPlotFlags_ContextMenu;
      if (ImPlot::BeginPlot(plot_title_.c_str(), x_axis_title_.c_str(), y_axis_title_.c_str(), ImVec2(plot_width_ - 25, plot_height_ - 40), imPlotFlags)) {
        ImPlot::PlotHeatmap(("##" + plot_title_).c_str(), data_.data(), rows_.size(), columns_.size(), data_min_, data_max_, NULL);
        ImPlot::EndPlot();
      }
    }
  }

}
