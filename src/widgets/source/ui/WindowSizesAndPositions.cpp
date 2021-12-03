// --------------------------------------------------------------------------
//   SmartPeak -- Fast and Accurate CE-, GC- and LC-MS(/MS) Data Processing
// --------------------------------------------------------------------------
// Copyright The SmartPeak Team -- Novo Nordisk Foundation 
// Center for Biosustainability, Technical University of Denmark 2018-2021.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL ANY OF THE AUTHORS OR THE CONTRIBUTING
// INSTITUTIONS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
// OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
// WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
// OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
// ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// --------------------------------------------------------------------------
// $Maintainer: Douglas McCloskey $
// $Authors: Douglas McCloskey $
// --------------------------------------------------------------------------

#include <SmartPeak/ui/WindowSizesAndPositions.h>

namespace SmartPeak
{

  std::string WindowSizesAndPositions::getPropertiesHandlerName() const
  {
    return "WindowSizesAndPositions";
  }

  std::map<std::string, CastValue::Type> WindowSizesAndPositions::getPropertiesSchema() const
  {
    std::map<std::string, CastValue::Type> properties;
    properties.emplace("bottom_window_y_perc_", CastValue::Type::FLOAT);
    properties.emplace("left_window_x_perc_", CastValue::Type::FLOAT);
    properties.emplace("right_window_x_perc_", CastValue::Type::FLOAT);
    return properties;
  }

  std::optional<CastValue> WindowSizesAndPositions::getProperty(const std::string& property, const size_t row) const
  {
    if (property == "bottom_window_y_perc_")
    {
      return bottom_window_y_perc_;
    }
    if (property == "left_window_x_perc_")
    {
      return left_window_x_perc_;
    }
    if (property == "right_window_x_perc_")
    {
      return right_window_x_perc_;
    }
    return std::nullopt;
  }

  void WindowSizesAndPositions::setProperty(const std::string& property, const CastValue& value, const size_t row)
  {
    if (property == "bottom_window_y_perc_")
    {
      bottom_window_y_perc_ = value.f_;
    }
    if (property == "left_window_x_perc_")
    {
      left_window_x_perc_ = value.f_;
    }
    if (property == "right_window_x_perc_")
    {
      right_window_x_perc_ = value.f_;
    }
  }

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
  void WindowSizesAndPositions::setLeftWindowXSize(const float& left_window_x_size) {
    left_window_x_perc_ = left_window_x_size / x_size_;
  }
  void WindowSizesAndPositions::setTopWindowYSize(const float& top_window_y_size) {
    bottom_window_y_perc_ = 1 - (top_window_y_size / y_size_);
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
  void WindowSizesAndPositions::setWindowsVisible(const bool& show_top_window, const bool& show_bottom_window, const bool& show_left_window, const bool& show_right_window) {
    // reset perc for bottom if needed
    if (!show_bottom_window_ && show_bottom_window)
    {
      bottom_window_y_perc_ = 0.25;
    }
    show_bottom_window_ = show_bottom_window;

    if (show_top_window && show_bottom_window && show_left_window && show_right_window) 
    {
      setWindowSizesAndPositions_(bottom_window_y_perc_, left_window_x_perc_, right_window_x_perc_);
    }
    else if (show_top_window && show_bottom_window && show_left_window) 
    {
      setWindowSizesAndPositions_(bottom_window_y_perc_, left_window_x_perc_, 0);
    }
    else if (show_top_window && show_bottom_window && show_right_window) 
    {
      setWindowSizesAndPositions_(bottom_window_y_perc_, 0, right_window_x_perc_);
    }
    else if (show_top_window && show_left_window && show_right_window) 
    {
      setWindowSizesAndPositions_(0, left_window_x_perc_, right_window_x_perc_);
    }
    else if (show_bottom_window && show_left_window && show_right_window) 
    {
      setWindowSizesAndPositions_(1, left_window_x_perc_, right_window_x_perc_);
    }
    else if (show_left_window && show_right_window) 
    {
      setWindowSizesAndPositions_(bottom_window_y_perc_, left_window_x_perc_, right_window_x_perc_);
    }
    else if (show_top_window && show_bottom_window) 
    {
      setWindowSizesAndPositions_(bottom_window_y_perc_, 0, 0);
    }
    else if (show_top_window && show_left_window) 
    {
      setWindowSizesAndPositions_(0, left_window_x_perc_, 0);
    }
    else if (show_top_window && show_right_window) 
    {
      setWindowSizesAndPositions_(0, 0, right_window_x_perc_);
    }
    else if (show_bottom_window && show_left_window) 
    {
      setWindowSizesAndPositions_(1, left_window_x_perc_, 0);
    }
    else if (show_bottom_window && show_right_window) 
    {
      setWindowSizesAndPositions_(1, 0, right_window_x_perc_);
    }
    else if (show_top_window) 
    {
      setWindowSizesAndPositions_(0, 0, 0);
    }
    else if (show_bottom_window) 
    {
      setWindowSizesAndPositions_(1, 0, 0);
    }
    else if (show_left_window || show_right_window) 
    {
      setWindowSizesAndPositions_(bottom_window_y_perc_, left_window_x_perc_, right_window_x_perc_);
    }
    else 
    {
      // TODO: logging...
    }
  }
}
