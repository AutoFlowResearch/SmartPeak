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
// $Authors: Douglas McCloskey, Pasquale Domenico Colaianni $
// --------------------------------------------------------------------------

#pragma once

#include <SmartPeak/iface/IPropertiesHandler.h>

namespace SmartPeak
{
  struct WindowSizesAndPositions : public IPropertiesHandler {

    /**
      IPropertiesHandler
    */
    virtual std::string getPropertiesHandlerName() const override;
    virtual std::map<std::string, CastValue::Type> getPropertiesSchema() const override;
    virtual std::optional<CastValue> getProperty(const std::string& property, const size_t row) const override;
    virtual void setProperty(const std::string& property, const CastValue& value, const size_t row) override;

    void setXAndYSizes(const float& x, const float& y);
    void setWindowPercentages(const float& bottom_window_y_perc, const float& left_window_x_perc, const float& right_window_x_perc);
    void setWindowSizesAndPositions_(const float& bottom_window_y_perc, const float& left_window_x_perc, const float& right_window_x_perc);
    void setWindowsVisible(const bool& show_top_window, const bool& show_bottom_window, const bool& show_left_window, const bool& show_right_window);
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

  private:
    bool show_bottom_window_ = true;
  };
}