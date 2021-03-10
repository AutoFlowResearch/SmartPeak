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

#include <SmartPeak/ui/DashboardWidget.h>
#include <SmartPeak/core/ApplicationProcessor.h>
#include <imgui.h>
#include <imgui_internal.h>
#include <plog/Log.h>
#include <SmartPeak/core/SharedProcessors.h>
#include <implot.h>

namespace SmartPeak
{
  void Dashboard::draw()
  {
    
    if (!application_handler_)
      return;

    std::ostringstream os;
    
    // Number of samples
    if (refresh_needed_)
    {
      number_of_samples_ = 0;
      if (application_handler_)
      {
        number_of_samples_ = application_handler_->sequenceHandler_.getSequence().size();
      }
    }
    os << "Number of samples: " << number_of_samples_;
    ImGui::Text(os.str().c_str());

    // Chart number of features/sample
    if (number_of_samples_ < 50)
    {
      if (refresh_needed_)
      {
        samples_chart_.clear();
        int samples_counter = 0;
        for (const auto& sequence : application_handler_->sequenceHandler_.getSequence())
        {
          samples_chart_.label_strings_.push_back(sequence.getMetaData().getSampleName());
          samples_chart_.positions_.push_back(static_cast<double>(samples_counter++));
          //samples_chart_values_.push_back(sequence.getRawData().getFeatureMap().size());
          samples_chart_.values_.push_back(sequence.getRawData().getFeatureMapHistory().size());
        }
        for (const auto& name : samples_chart_.label_strings_)
        {
          samples_chart_.label_char_ptr_.push_back(name.c_str());
        }
      }
      if (samples_chart_.values_.size() > 0)
      {
        drawChart(samples_chart_, "#Features/Sample", "Sample", "#Features");
      }
    }
    else
    {
      // Too much transitions to display
    }

    // Number of transitions
    os.str("");
    os.clear();
    if (refresh_needed_)
    {
      number_of_transitions_ = 0;
      if (application_handler_)
      {
        number_of_transitions_ = transitions->dimension(0);
      }
    }
    os << "Number of transitions: " << number_of_transitions_;
    ImGui::Text(os.str().c_str());

    // Chart number of features/transition
    if (number_of_transitions_ < 50)
    {
      if (refresh_needed_)
      {
        std::map<std::string, int> feature_counter;
        for (size_t row = 0; row < transitions->dimension(0); ++row)
        {
          const auto& transition_name = (*transitions)(row, 1);
          feature_counter[transition_name] = 0;
        }
        transitions_chart_.clear();
        int transitions_counter = 0;
        for (const auto& sequence : application_handler_->sequenceHandler_.getSequence())
        {
          for (const auto& feature : sequence.getRawData().getFeatureMapHistory())
          {
            for (const auto& subordinate : feature.getSubordinates())
            {
              const auto& native_id = subordinate.getMetaValue("native_id");
              if (native_id.valueType() == OpenMS::DataValue::STRING_VALUE)
              {
                feature_counter[native_id]++;
              }
            }
          }
        }
        for (size_t row = 0; row < transitions->dimension(0); ++row)
        {
          const auto& transition_name = (*transitions)(row, 1);
          transitions_chart_.label_strings_.push_back(transition_name);
          transitions_chart_.positions_.push_back(static_cast<double>(transitions_counter++));
          transitions_chart_.values_.push_back(feature_counter[transition_name]);
        }
        for (const auto& name : transitions_chart_.label_strings_)
        {
          transitions_chart_.label_char_ptr_.push_back(name.c_str());
        }
      }
      if (transitions_chart_.values_.size() > 0)
      {
        drawChart(transitions_chart_, "#Features/Transition", "Transition", "#Features");
      }
    }
    else
    {
      // Too much transitions to display
    }

    ImGui::Separator();

    // Numbers of chromatograms
    os.str("");
    os.clear();
    int number_of_chromatograms = 0;
    if ((application_handler_) && (application_handler_->sequenceHandler_.getSequence().size() > 0))
    {
      number_of_chromatograms = application_handler_->sequenceHandler_.getSequence().at(0).getRawData().getChromatogramMap().getChromatograms().size();
    }
    os << "Number of chromatograms: " << number_of_chromatograms;
    ImGui::Text(os.str().c_str());

    // Numbers of spectrums
    os.str("");
    os.clear();
    int number_of_spectrums = 0;
    if ((application_handler_) && (application_handler_->sequenceHandler_.getSequence().size() > 0))
    {
      number_of_spectrums = application_handler_->sequenceHandler_.getSequence().at(0).getRawData().getExperiment().getSpectra().size();
    }
    os << "Number of spectrums: " << number_of_spectrums;
    ImGui::Text(os.str().c_str());

    refresh_needed_ = false;
  }

  void Dashboard::setApplicationHandler(ApplicationHandler& state)
  {
    LOGD << "Setting state: " << (&state);
    application_handler_ = &state;
  }

  void Dashboard::drawChart(const DashboardChartData& chart_data, const char* title, const char* x_label, const char* y_label) const
  {
    ImPlot::SetNextPlotLimits(-0.5,
      static_cast<double>(chart_data.values_.size() - 1) + 0.5,
      0,
      *std::max_element(std::begin(chart_data.values_), std::end(chart_data.values_)),
      ImGuiCond_Always);
    ImPlot::SetNextPlotTicksX(&chart_data.positions_.front(), chart_data.values_.size()/*, &chart_data.label_char_ptr_.front()*/);
    if (ImPlot::BeginPlot(title, x_label, y_label, ImVec2(0, 0), ImPlotFlags_Default & ~ImPlotFlags_Legend)) {
      ImPlot::PlotBars("#Features", &chart_data.values_.front(), chart_data.values_.size());
      ImPlot::EndPlot();
    }
  }
}
