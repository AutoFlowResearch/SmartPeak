#pragma once

#include <SmartPeak/ui/GraphicDataVizWidget.h>
#include <SmartPeak/iface/ISequenceObserver.h>

#include <string>
#include <utility>
#include <vector>

namespace SmartPeak
{
  class SpectraMSMSPlotWidget;

  /**
    @brief Class for plotting Extracted-ion chromatogram (XIC) Chromatograms
  */
  class ChromatogramXICPlotWidget : 
    public GraphicDataVizWidget,
    public ISequenceObserver
  {
  public:
    ChromatogramXICPlotWidget(SessionHandler& session_handler,
      SequenceHandler& sequence_handler,
      const std::string& id,
      const std::string& title,
      std::shared_ptr<SpectraMSMSPlotWidget> spectra_ms1_plot_widget,
      SequenceObservable& sequence_observable) :
      GraphicDataVizWidget(session_handler, sequence_handler, id, title),
      spectra_msms_plot_widget_(spectra_ms1_plot_widget)
    {
      use_markers_ = (spectra_ms1_plot_widget != nullptr);
      sequence_observable.addSequenceObserver(this);
    };

    /**
      ISequenceObserver
    */
    virtual void onSequenceUpdated() override;

    /**
      GraphicDataVizWidget
    */
    virtual void setMarkerPosition(const std::optional<float>& marker_position) override;
    virtual std::optional<float> getMarkerPosition() const override;

    void setMZ(float mz)
    {
      current_mz_ = mz;
      refresh_needed_ = true; // this will reset the range
    }

    float getMZ()
    {
      return current_mz_;
    }

  protected:
    virtual void updateData() override;

  protected:
    // input used to create the graph
    std::pair<float, float> input_range_ = { 0.0f, 0.0f };
    std::set<std::string> input_sample_names_;
    std::set<std::string> input_component_names_;
    std::shared_ptr<SpectraMSMSPlotWidget> spectra_msms_plot_widget_;
    float current_mz_ = 0.0f;
    float input_mz_ = 0.0f;
  };
}