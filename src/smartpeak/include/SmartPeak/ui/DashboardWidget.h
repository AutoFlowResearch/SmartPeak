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
#include <string>
#include <vector>

namespace SmartPeak
{
  class Dashboard final : public Widget
  {
    ApplicationHandler* application_handler_ = nullptr;

  public:
    void draw() override;

    void setApplicationHandler(ApplicationHandler& application_handler);

    void setRefreshNeeded() { refresh_needed_ = true; };

    const Eigen::Tensor<std::string, 2>* transitions = nullptr;

  protected:
    struct DashboardChartData
    {
      std::vector<std::string> label_strings_;
      std::vector<const char*> label_char_ptr_;
      std::vector<double> values_;
      std::vector<double> positions_;
      void clear()
      {
        label_strings_.clear();
        label_char_ptr_.clear();
        values_.clear();
        positions_.clear();
      };
    };
    DashboardChartData samples_chart_;
    DashboardChartData transitions_chart_;
    void drawChart(const DashboardChartData& chart_data, const char* title, const char* x_label, const char* y_label) const;
    int number_of_samples_ = 0;
    int number_of_transitions_ = 0;
    bool refresh_needed_ = true;
  };
}
