#pragma once

namespace SmartPeak
{
  struct WindowSizesAndPositions {
    void setXAndYSizes(const float& x, const float& y);
    void setWindowPercentages(const float& bottom_window_y_perc, const float& left_window_x_perc, const float& right_window_x_perc);
    void setWindowSizesAndPositions_(const float& bottom_window_y_perc, const float& left_window_x_perc, const float& right_window_x_perc);
    void setWindowSizesAndPositions(const bool& show_top_window, const bool& show_bottom_window, const bool& show_left_window, const bool& show_right_window);
    void setLeftWindowXSize(const float& left_window_x_size);
    void setTopWindowYSize(const float& top_window_y_size);

    // Absolute application size
    float main_menu_bar_y_size_ = 18.0f;
    float y_size_ = 0;
    float x_size_ = 0;

    // Absolute window sizes
    float bottom_window_y_size_ = 0;
    float bottom_and_top_window_x_size_ = 0;
    float top_window_y_size_ = 0;
    float left_and_right_window_y_size_ = 0;
    float left_window_x_size_ = 0;
    float right_window_x_size_ = 0;

    // Absolute window positions
    float bottom_window_y_pos_ = 0;
    float bottom_and_top_window_x_pos_ = 0;
    float top_window_y_pos_ = 0;
    float left_and_right_window_y_pos_ = 0;
    float left_window_x_pos_ = 0;
    float right_window_x_pos_ = 0;

    // Relative percent offsets of the window sizes
    float bottom_window_y_perc_ = 0.25;
    float left_window_x_perc_ = 0.25;
    float right_window_x_perc_ = 0;
  };
}