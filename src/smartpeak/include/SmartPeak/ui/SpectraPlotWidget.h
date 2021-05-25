#pragma once

#include <SmartPeak/ui/Widget.h>
#include <string>
#include <utility>
#include <vector>
#include <imgui.h>

namespace SmartPeak
{

  /**
    @brief Class for plotting Spectra
  */
  class SpectraPlotWidget : 
    public ScatterPlotWidget,
    public ISequenceObserver
  {
  public:
    SpectraPlotWidget(SessionHandler& session_handler,
      SequenceHandler& sequence_handler,
      const std::string& id,
      const std::string& title,
      SequenceObservable& sequence_observable) :
      ScatterPlotWidget(session_handler, sequence_handler, id, title) 
    {
      sequence_observable.addSequenceObserver(this);
    };

    /**
      ISequenceObserver
    */
    virtual void onSequenceUpdated() override;

  protected:
    virtual void updateScatterPlotData() override;
  protected:
    // input used to create the graph
    std::pair<float, float> input_range_ = { 0.0f, 0.0f };
    std::set<std::string> input_sample_names_;
    std::set<std::string> input_scan_names_;
    std::set<std::string> input_component_group_names_;
  };

}