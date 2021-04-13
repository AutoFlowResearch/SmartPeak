#pragma once

#include <SmartPeak/ui/Widget.h>
#include <string>
#include <utility>
#include <vector>
#include <imgui.h>

namespace SmartPeak
{

  /**
    @brief Class for Calibrators
  */
  class CalibratorsPlotWidget : public GenericGraphicWidget
  {
  public:
    CalibratorsPlotWidget() {};
    void setValues(
      const std::vector<std::vector<float>>* x_fit_data, const std::vector<std::vector<float>>* y_fit_data,
      const std::vector<std::vector<float>>* x_raw_data, const std::vector<std::vector<float>>* y_raw_data, const std::vector<std::string>* series_names,
      const std::string& x_axis_title, const std::string& y_axis_title, const float& x_min, const float& x_max, const float& y_min, const float& y_max,
      const float& plot_width, const float& plot_height, const std::string& plot_title)
    {
      x_fit_data_ = x_fit_data;
      y_fit_data_ = y_fit_data;
      x_raw_data_ = x_raw_data;
      y_raw_data_ = y_raw_data;
      series_names_ = series_names;
      x_axis_title_ = x_axis_title;
      y_axis_title_ = y_axis_title;
      x_min_ = x_min;
      x_max_ = x_max;
      y_min_ = y_min;
      y_max_ = y_max;
      plot_width_ = plot_width;
      plot_height_ = plot_height;
      plot_title_ = plot_title;
    }
    void draw() override;
  protected:
    const std::vector<std::vector<float>>* x_fit_data_;
    const std::vector<std::vector<float>>* y_fit_data_;
    const std::vector<std::vector<float>>* x_raw_data_;
    const std::vector<std::vector<float>>* y_raw_data_;
    const std::vector<std::string>* series_names_;
    std::string x_axis_title_;
    std::string y_axis_title_;
    float x_min_;
    float x_max_;
    float y_min_;
    float y_max_;
    float plot_width_;
    float plot_height_;
    std::string plot_title_; // used as the ID of the plot as well so this should be unique across the different Widgets
  };

}