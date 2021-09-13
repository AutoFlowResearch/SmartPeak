#pragma once

#include <SmartPeak/ui/GraphicDataVizWidget.h>
#include <string>
#include <utility>
#include <vector>

namespace SmartPeak
{

  /**
    @brief Class for plotting Spectra
  */
  class SpectraPlotWidget : 
    public GraphicDataVizWidget,
    public ISequenceObserver
  {
  public:
    SpectraPlotWidget(SessionHandler& session_handler,
      ApplicationHandler& application_handler,
      const std::string& id,
      const std::string& title,
      SequenceObservable& sequence_observable) :
      GraphicDataVizWidget(session_handler, application_handler, id, title)
    {
      is_spectra_ = true;
      sequence_observable.addSequenceObserver(this);
    };

    /**
      ISequenceObserver
    */
    virtual void onSequenceUpdated() override;

  protected:
    virtual void updateData() override;

  protected:
    // input used to create the graph
    std::set<std::string> input_sample_names_;
    std::set<std::string> input_scan_names_;
    std::set<std::string> input_component_group_names_;
  };

}
