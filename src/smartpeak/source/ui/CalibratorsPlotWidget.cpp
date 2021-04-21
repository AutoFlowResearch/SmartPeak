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
// $Maintainer: Bertrand Boudaud $
// $Authors: Bertrand Boudaud $
// --------------------------------------------------------------------------

#include <SmartPeak/ui/CalibratorsPlotWidget.h>
#include <implot.h>

namespace SmartPeak
{
  void CalibratorsPlotWidget::draw()
  {
    // Main graphic
    ImPlot::SetNextPlotLimits(x_min_, x_max_, y_min_, y_max_, ImGuiCond_Always);
    if (ImPlot::BeginPlot(plot_title_.c_str(), x_axis_title_.c_str(), y_axis_title_.c_str(), ImVec2(width_ - 25, height_ - 40))) {
      for (int i = 0; i < x_raw_data_->size(); ++i) {
        assert(x_raw_data_->at(i).size() == y_raw_data_->at(i).size());
        ImPlot::PushStyleVar(ImPlotStyleVar_Marker, ImPlotMarker_Circle);
        ImPlot::PlotScatter((series_names_->at(i) + "-pts").c_str(), x_raw_data_->at(i).data(), y_raw_data_->at(i).data(), x_raw_data_->at(i).size());
      }
      for (int i = 0; i < x_fit_data_->size(); ++i) {
        assert(x_fit_data_->at(i).size() == y_fit_data_->at(i).size());
        ImPlot::PushStyleVar(ImPlotStyleVar_LineWeight, ImPlot::GetStyle().LineWeight);
        ImPlot::PushStyleVar(ImPlotStyleVar_Marker, ImPlotMarker_Circle);
        ImPlot::PlotLine((series_names_->at(i) + "-fit").c_str(), x_fit_data_->at(i).data(), y_fit_data_->at(i).data(), x_fit_data_->at(i).size());
      }
      ImPlot::EndPlot();
    }
  }
}
