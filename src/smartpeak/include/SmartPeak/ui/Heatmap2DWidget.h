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
    Heatmap2DWidget(const Eigen::Tensor<float, 2, Eigen::RowMajor>& data, const Eigen::Tensor<std::string, 1>& columns, const Eigen::Tensor<std::string, 1>& rows,
      const std::string& x_axis_title, const std::string& y_axis_title, const float& data_min, const float& data_max,
      const float& plot_width, const float& plot_height, const std::string& plot_title)
      :data_(data), columns_(columns), rows_(rows), x_axis_title_(x_axis_title), y_axis_title_(y_axis_title), data_min_(data_min), data_max_(data_max),
      plot_width_(plot_width), plot_height_(plot_height), plot_title_(plot_title) {};
    void draw() override;
    const Eigen::Tensor<float, 2, Eigen::RowMajor>& data_; // Row major ordering
    const Eigen::Tensor<std::string, 1>& columns_;
    const Eigen::Tensor<std::string, 1>& rows_;
    const std::string& x_axis_title_;
    const std::string& y_axis_title_;
    const float& data_min_;
    const float& data_max_;
    const float& plot_width_;
    const float& plot_height_;
    const std::string plot_title_; // used as the ID of the plot as well so this should be unique across the different Widgets
  };

}