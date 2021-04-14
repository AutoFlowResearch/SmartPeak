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
  class Heatmap2DWidget : public GenericGraphicWidget
  {
  public:
    Heatmap2DWidget(SessionHandler& session_handler,
                    SequenceHandler& sequence_handler,
                    const std::string& title)
      : GenericGraphicWidget(title),
        session_handler_(session_handler),
        sequence_handler_(sequence_handler),
        plot_title_(title) {};
    void draw() override;
    void setWindowSize(float width, float height) { plot_width_ = width; plot_height_ = height; };
    void setRefreshNeeded() { refresh_needed_ = true; };

  private:
    bool compareInput(const Eigen::Tensor<std::string, 1>& l, const Eigen::Tensor<std::string, 1>& r) const;
  protected:
    SessionHandler& session_handler_;
    SequenceHandler& sequence_handler_;
    SessionHandler::HeatMapData heatmap_data_;
    float plot_width_;
    float plot_height_;
    std::string plot_title_; // used as the ID of the plot as well so this should be unique across the different Widgets
    std::string selected_feature_;
    bool invalid_data_;
    bool data_mismatch_;
    bool refresh_needed_ = false;
  };

}