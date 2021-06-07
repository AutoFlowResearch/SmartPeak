#pragma once

#include <SmartPeak/ui/Widget.h>

#include <string>
#include <utility>
#include <vector>
#include <imgui.h>

namespace SmartPeak
{

  /**
    @brief Class for plotting heatmaps
  */
  class Heatmap2DWidget : 
    public GenericGraphicWidget,
    public ISequenceObserver
  {
  public:
    Heatmap2DWidget(SessionHandler& session_handler,
                    SequenceHandler& sequence_handler,
                    const std::string& id,
                    const std::string& title,
                    SequenceObservable& sequence_observable)
      : GenericGraphicWidget(title),
        session_handler_(session_handler),
        sequence_handler_(sequence_handler),
        plot_title_(id) 
    {
      sequence_observable.addSequenceObserver(this);
    };
    void draw() override;

  public:
    /**
     ISequenceObserver
    */
    virtual void onSequenceUpdated() override;

  private:
    bool compareInput(const Eigen::Tensor<std::string, 1>& l, const Eigen::Tensor<std::string, 1>& r) const;
  protected:
    SessionHandler& session_handler_;
    SequenceHandler& sequence_handler_;
    SessionHandler::HeatMapData heatmap_data_;
    std::string plot_title_; // used as the ID of the plot as well so this should be unique across the different Widgets
    std::string selected_feature_;
    bool invalid_data_;
    bool data_mismatch_;
    bool refresh_needed_ = false;
  };

}