#include <SmartPeak/ui/WindowSizesAndPositions.h>

namespace SmartPeak
{
  void WindowSizesAndPositions::setXAndYSizes(const float & x, const float & y) {
    x_size_ = x; 
    y_size_ = y - main_menu_bar_y_size_;
  }
  void WindowSizesAndPositions::setWindowPercentages(const float& bottom_window_y_perc, const float& left_window_x_perc, const float& right_window_x_perc) {
    bottom_window_y_perc_ = bottom_window_y_perc;
    left_window_x_perc_ = left_window_x_perc;
    right_window_x_perc_ = right_window_x_perc;
    // TODO: check that the percentages equal 1
  }
  void WindowSizesAndPositions::setWindowSizesAndPositions_(const float& bottom_window_y_perc, const float& left_window_x_perc, const float& right_window_x_perc) {
    // Absolute sizes
    bottom_window_y_size_ = bottom_window_y_perc * y_size_;
    top_window_y_size_ = (1 - bottom_window_y_perc) * y_size_;
    bottom_and_top_window_x_size_ = (1 - left_window_x_perc - right_window_x_perc) * x_size_;
    left_and_right_window_y_size_ = y_size_;
    left_window_x_size_ = left_window_x_perc * x_size_;
    right_window_x_size_ = right_window_x_perc * x_size_;
    // Absolute positions
    bottom_window_y_pos_ = top_window_y_size_ + main_menu_bar_y_size_;
    top_window_y_pos_ = main_menu_bar_y_size_;
    bottom_and_top_window_x_pos_ = left_window_x_size_;
    left_and_right_window_y_pos_ = main_menu_bar_y_size_;
    left_window_x_pos_ = 0;
    right_window_x_pos_ = left_window_x_size_ + bottom_and_top_window_x_size_;
  }
  void WindowSizesAndPositions::setWindowSizesAndPositions(const bool& show_top_window, const bool& show_bottom_window, const bool& show_left_window, const bool& show_right_window) {
    if (show_top_window && show_bottom_window && show_left_window && show_right_window) {
      setWindowSizesAndPositions_(bottom_window_y_perc_, left_window_x_perc_, right_window_x_perc_);
    }
    else if (show_top_window && show_bottom_window && show_left_window) {
      setWindowSizesAndPositions_(bottom_window_y_perc_, left_window_x_perc_, 0);
    }
    else if (show_top_window && show_bottom_window && show_right_window) {
      setWindowSizesAndPositions_(bottom_window_y_perc_, 0, right_window_x_perc_);
    }
    else if (show_top_window && show_left_window && show_right_window) {
      setWindowSizesAndPositions_(0, left_window_x_perc_, right_window_x_perc_);
    }
    else if (show_bottom_window && show_left_window && show_right_window) {
      setWindowSizesAndPositions_(1, left_window_x_perc_, right_window_x_perc_);
    }
    else if (show_left_window && show_right_window) {
      setWindowSizesAndPositions_(bottom_window_y_perc_, left_window_x_perc_, right_window_x_perc_);
    }
    else if (show_top_window && show_bottom_window) {
      setWindowSizesAndPositions_(bottom_window_y_perc_, 0, 0);
    }
    else if (show_top_window && show_left_window) {
      setWindowSizesAndPositions_(0, left_window_x_perc_, 0);
    }
    else if (show_top_window && show_right_window) {
      setWindowSizesAndPositions_(0, 0, right_window_x_perc_);
    }
    else if (show_bottom_window && show_left_window) {
      setWindowSizesAndPositions_(1, left_window_x_perc_, 0);
    }
    else if (show_bottom_window && show_right_window) {
      setWindowSizesAndPositions_(1, 0, right_window_x_perc_);
    }
    else if (show_top_window) {
      setWindowSizesAndPositions_(0, 0, 0);
    }
    else if (show_bottom_window) {
      setWindowSizesAndPositions_(1, 0, 0);
    }
    else if (show_left_window || show_right_window) {
      setWindowSizesAndPositions_(bottom_window_y_perc_, left_window_x_perc_, right_window_x_perc_);
    }
    else {
      // TODO: logging...
    }
  }
}