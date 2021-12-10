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
// $Maintainer: Douglas McCloskey, Bertrand Boudaud $
// $Authors: Douglas McCloskey, Bertrand Boudaud $
// --------------------------------------------------------------------------

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
    CalibratorsPlotWidget(const std::string title = ""): GenericGraphicWidget(title) {};
    void setValues(
      const std::vector<std::vector<float>>* x_fit_data, const std::vector<std::vector<float>>* y_fit_data,
      const std::vector<std::vector<float>>* x_raw_data, const std::vector<std::vector<float>>* y_raw_data, const std::vector<std::string>* series_names,
      const std::string& x_axis_title, const std::string& y_axis_title, const float& x_min, const float& x_max, const float& y_min, const float& y_max,
      const std::string& plot_title)
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
      plot_title_ = plot_title;
    }
    void draw() override;
  protected:
    const std::vector<std::vector<float>>* x_fit_data_ = nullptr;
    const std::vector<std::vector<float>>* y_fit_data_ = nullptr;
    const std::vector<std::vector<float>>* x_raw_data_ = nullptr;
    const std::vector<std::vector<float>>* y_raw_data_ = nullptr;
    const std::vector<std::string>* series_names_ = nullptr;
    std::string x_axis_title_;
    std::string y_axis_title_;
    float x_min_;
    float x_max_;
    float y_min_;
    float y_max_;
    std::string plot_title_; // used as the ID of the plot as well so this should be unique across the different Widgets
  };

}