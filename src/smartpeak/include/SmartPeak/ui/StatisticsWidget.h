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

#include <SmartPeak/ui/Widget.h>
#include <SmartPeak/core/ApplicationHandler.h>
#include <SmartPeak/iface/ISequenceObserver.h>
#include <string>
#include <vector>
#include <chrono>

namespace SmartPeak
{
  class StatisticsWidget final : public Widget, public ISequenceObserver
  {

  public:
    StatisticsWidget(const std::string title = "")
      : Widget(title)
    {};

    void draw() override;

    void setApplicationHandler(ApplicationHandler& application_handler);

    void setRefreshNeeded() { refresh_needed_ = true; };

    void setWindowSize(float width, float height) { width_ = width; height_ = height; };

    void setTransitions(const Eigen::Tensor<std::string, 2>* transitions, const Eigen::Tensor<bool, 2>& transitions_checkbox, const Eigen::Tensor<std::string, 2>& transitions_columns)
    { 
      transitions_ = transitions;
      transitions_checkbox_ = transitions_checkbox;
      transitions_columns_ = transitions_columns;
    };
    void setInjections(const Eigen::Tensor<bool, 2>& injections_checkbox, const Eigen::Tensor<std::string, 2>& injections_columns)
    {
      injections_checkbox_ = injections_checkbox;
      injections_columns_ = injections_columns;
    }

  public:
    /**
     ISequenceObserver
    */
    virtual void sequenceUpdated() override;

  private:
    struct DashboardChartData
    {
      void clear()
      {
        label_strings_.clear();
        label_char_ptr_.clear();
        values_.clear();
        positions_.clear();
        total_value_ = 0.0;
        error_message_.clear();
      };
      std::vector<std::string> label_strings_;
      std::vector<const char*> label_char_ptr_;
      std::vector<double> values_;
      std::vector<double> positions_;
      int selected_value_index = -1;
      std::string selected_value_name;
      std::vector<double> unselected_values_;
      std::vector<double> selected_values_;
      double total_value_;
      std::string error_message_;
    };
    void drawChart(DashboardChartData& chart_data, const char* title, const char* x_label, const char* y_label) const;

  protected:
    DashboardChartData samples_chart_;
    DashboardChartData transitions_chart_;
    int number_of_samples_ = 0;
    int number_of_transitions_ = 0;
    bool refresh_needed_ = true;
    float width_;
    float height_;
    ApplicationHandler* application_handler_ = nullptr;
    Eigen::Tensor<bool, 2> injections_checkbox_;
    Eigen::Tensor<std::string, 2> injections_columns_;
    const Eigen::Tensor<std::string, 2>* transitions_ = nullptr;
    Eigen::Tensor<bool, 2> transitions_checkbox_;
    Eigen::Tensor<std::string, 2> transitions_columns_;
  };
}
