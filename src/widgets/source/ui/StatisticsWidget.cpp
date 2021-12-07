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

#include <SmartPeak/ui/StatisticsWidget.h>
#include <SmartPeak/core/ApplicationProcessor.h>
#include <imgui.h>
#include <plog/Log.h>
#include <SmartPeak/core/SharedProcessors.h>
#include <implot.h>

namespace SmartPeak
{
  void StatisticsWidget::draw()
  {

    // number of items from which the basic graph 
    // will be replaced by simplified graph to support large number of data
    static const int max_samples = 50;
    static const int max_transitions = 50;

    // Number of samples
    if (refresh_needed_)
    {
      number_of_samples_ = application_handler_.sequenceHandler_.getSequence().size();
    }    // Chart number of features/sample
    if (number_of_samples_ < max_samples)
    {
      if (refresh_needed_)
      {
        samples_chart_.clear();
        int samples_counter = 0;
        int sample_index = 0;
        for (const auto& sequence : application_handler_.sequenceHandler_.getSequence())
        {
          if ((injections_checkbox_)(sample_index, 1))
          {
            samples_chart_.label_strings_.push_back(sequence.getMetaData().getSampleName());
            samples_chart_.positions_.push_back(static_cast<double>(samples_counter++));
            samples_chart_.values_.push_back(sequence.getRawData().getFeatureMapHistory().size());
            samples_chart_.total_value_ += sequence.getRawData().getFeatureMapHistory().size();
          }
          sample_index++;
        }
        for (const auto& name : samples_chart_.label_strings_)
        {
          samples_chart_.label_char_ptr_.push_back(name.c_str());
        }
        if (samples_chart_.values_.empty())
        {
          samples_chart_.error_message_ = "No chart to display, please select samples from the Injections explorer tab.";
        }
      }
    }
    else
    {
      samples_chart_.clear();
      samples_chart_.error_message_ = "Too much points to draw. Try to filter data.";
    }
    drawChart(samples_chart_, "#Features/Sample", "Samples", "#Features");

    ImGui::SameLine();

    // Number of transitions
    if (refresh_needed_)
    {
      number_of_transitions_= transitions_->dimension(0);
    }
    // Chart number of features/transition
    if (number_of_transitions_ < max_transitions)
    {
      if (refresh_needed_)
      {
        std::map<std::string, int> feature_counter;
        for (size_t row = 0; row < transitions_->dimension(0); ++row)
        {
          const auto& transition_name = (*transitions_)(row, 1);
          feature_counter[transition_name] = 0;
        }
        transitions_chart_.clear();
        int transitions_counter = 0;
        int transitions_index = 0;
        for (const auto& sequence : application_handler_.sequenceHandler_.getSequence())
        {
          for (const auto& feature : sequence.getRawData().getFeatureMapHistory())
          {
            for (const auto& subordinate : feature.getSubordinates())
            {
              const auto& native_id = subordinate.getMetaValue("native_id");
              if (native_id.valueType() == OpenMS::ParamValue::STRING_VALUE)
              {
                feature_counter[native_id]++;
              }
            }
          }
        }
        for (size_t row = 0; row < transitions_->dimension(0); ++row)
        {
          if ((transitions_checkbox_)(transitions_index, 0))
          {
            const auto& transition_name = (*transitions_)(row, 1);
            transitions_chart_.label_strings_.push_back(transition_name);
            transitions_chart_.positions_.push_back(static_cast<double>(transitions_counter++));
            transitions_chart_.values_.push_back(feature_counter[transition_name]);
            transitions_chart_.total_value_ += feature_counter[transition_name];
          }
          transitions_index++;
        }
        for (const auto& name : transitions_chart_.label_strings_)
        {
          transitions_chart_.label_char_ptr_.push_back(name.c_str());
        }
        if (transitions_chart_.values_.empty())
        {
          transitions_chart_.error_message_ = "No chart to display, please select transitions from the Transitions explorer tab.";
        }
      }
    }
    else
    {
      transitions_chart_.clear();
      transitions_chart_.error_message_ = "Too much points to draw. Try to filter data.";
    }
    drawChart(transitions_chart_, "#Features/Transition", "Transitions", "#Features");

  }

  void StatisticsWidget::drawChart(DashboardChartData& chart_data, const char* title, const char* x_label, const char* y_label) const
  {
    
    chart_data.selected_values_.clear();
    std::transform(chart_data.values_.begin(), chart_data.values_.end(), std::back_inserter(chart_data.selected_values_),
      [&](const double& value) {  return (&value - &chart_data.values_[0] == chart_data.selected_value_index) ? value : 0.0f; });

    chart_data.unselected_values_.clear();
    std::transform(chart_data.values_.begin(), chart_data.values_.end(), std::back_inserter(chart_data.unselected_values_),
      [&](const double& value) {  return (&value - &chart_data.values_[0] != chart_data.selected_value_index) ? value : 0.0f; });

    bool is_hovered = false;
    ImPlotPoint plot_point;
    if (chart_data.values_.size() > 0)
    {
      ImPlot::SetNextPlotLimits(-0.5,
        static_cast<double>(chart_data.values_.size() - 1) + 0.5,
        0,
        *std::max_element(std::begin(chart_data.values_), std::end(chart_data.values_)),
        ImGuiCond_Always);
      ImPlot::SetNextPlotTicksX(&chart_data.positions_.front(), chart_data.values_.size());
    }
    else
    {
      ImPlot::SetNextPlotLimits(-0.5, +0.5, 0, 1, ImGuiCond_Always);
    }
    if (ImPlot::BeginPlot(title, x_label, y_label, ImVec2((width_/2.0f) - 10, height_ - 40), ImPlotFlags_Default & ~ImPlotFlags_Legend & ~ImPlotFlags_MousePos, ImPlotAxisFlags_GridLines | ImPlotAxisFlags_TickMarks)) {
      if (chart_data.values_.size() > 0)
      {
        ImPlot::PlotBars("Unselected", &chart_data.unselected_values_.front(), chart_data.unselected_values_.size());
        ImPlot::PlotBars("Selected", &chart_data.selected_values_.front(), chart_data.selected_values_.size());
        is_hovered = ImPlot::IsPlotHovered();
        if (is_hovered)
        {
          plot_point = ImPlot::GetPlotMousePos();
        }
      }
      if (!chart_data.error_message_.empty())
      {
        ImPlot::PlotText(chart_data.error_message_.c_str(), -0.5f, 0.0f, false, { 5, -20 });
      }
      ImPlot::EndPlot();
    }
    if (is_hovered)
    {
      const size_t index_item = (size_t)(plot_point.x + 0.5);
      if (index_item <  chart_data.label_strings_.size())
      {
        chart_data.selected_value_name = chart_data.label_strings_.at(index_item);
        ImGui::BeginTooltip();
        ImGui::Text("%s", chart_data.selected_value_name.c_str());
        std::ostringstream os;
        os << chart_data.values_[index_item] << " Features";
        ImGui::Text("%s", os.str().c_str());
        ImGui::EndTooltip();
        chart_data.selected_value_index = index_item;
      }
    }
    else
    {
      chart_data.selected_value_index = -1;
    }
  }

  void StatisticsWidget::onSequenceUpdated()
  {
    refresh_needed_ = true;
  }
}
