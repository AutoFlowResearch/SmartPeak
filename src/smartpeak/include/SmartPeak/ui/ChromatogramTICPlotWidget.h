#pragma once

#include <SmartPeak/ui/GraphicDataVizWidget.h>
#include <SmartPeak/iface/ISequenceObserver.h>
#include <SmartPeak/ui/SpectraMSMSPlotWidget.h>

#include <string>
#include <utility>
#include <vector>
#include <imgui.h>

namespace SmartPeak
{
  /**
    @brief Class for plotting Total ion current (TIC) Chromatograms
  */
  class ChromatogramTICPlotWidget : 
    public GraphicDataVizWidget,
    public ISequenceObserver
  {
  public:
    ChromatogramTICPlotWidget(SessionHandler& session_handler,
      SequenceHandler& sequence_handler,
      const std::string& id,
      const std::string& title,
      std::shared_ptr<SpectraMSMSPlotWidget> spectra_ms1_plot_widget,
      SequenceObservable& sequence_observable) :
      GraphicDataVizWidget(session_handler, sequence_handler, id, title),
      spectra_msms_plot_widget_(spectra_ms1_plot_widget)
    {
      use_markers_ = true;
      sequence_observable.addSequenceObserver(this);
    };

    /**
      GraphicDataVizWidget
    */
    virtual void setMarkerPosition(const std::optional<float>& marker_position) override;
    virtual std::optional<float> getMarkerPosition() const override;

    /**
      ISequenceObserver
    */
    virtual void onSequenceUpdated() override;

  protected:
    virtual void updateData() override;

  protected:
    // input used to create the graph
    std::pair<float, float> input_range_ = { 0.0f, 0.0f };
    std::set<std::string> input_sample_names_;
    std::set<std::string> input_scan_names_;
    std::shared_ptr<SpectraMSMSPlotWidget> spectra_msms_plot_widget_;
  };
}