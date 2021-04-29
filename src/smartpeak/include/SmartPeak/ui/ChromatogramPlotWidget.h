#pragma once

#include <SmartPeak/ui/Widget.h>
#include <string>
#include <utility>
#include <vector>
#include <imgui.h>

namespace SmartPeak
{
  /**
    @brief Class for plotting Chromatograms
  */
  class ChromatogramPlotWidget : public ScatterPlotWidget
  {
  public:
    ChromatogramPlotWidget(SessionHandler& session_handler,
      SequenceHandler& sequence_handler,
      const std::string& id,
      const std::string& title) :
      ScatterPlotWidget(session_handler, sequence_handler, id, title) {};
  protected:
    virtual void updateScatterPlotData() override;
  protected:
    // input used to create the graph
    std::pair<float, float> input_range_ = { 0.0f, 0.0f };
    std::set<std::string> input_sample_names_;
    std::set<std::string> input_component_names_;
  };
}