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
// $Maintainer: Bertrand Boudaud, Ahmed Khalil, Douglas McCloskey $
// $Authors: Bertrand Boudaud $
// --------------------------------------------------------------------------

#include <SmartPeak/ui/Heatmap2DWidget.h>
#include <implot.h>

namespace SmartPeak
{

  void Heatmap2DWidget::draw()
  {
    showQuickHelpToolTip("Heatmap2DWidget");

    Eigen::Tensor<std::string, 2> selected_feature_names = session_handler_.feature_table.body_;

    std::vector<std::string> feature_names;
    for (int i = 0; i < selected_feature_names.size(); ++i) {
      if (std::count(feature_names.begin(), feature_names.end(), selected_feature_names(i)) == 0 && !selected_feature_names(i).empty())
        feature_names.push_back(selected_feature_names(i));
    }
    if (std::find(feature_names.begin(), feature_names.end(), selected_feature_) == feature_names.end())
    {
      if (feature_names.size())
      {
        selected_feature_ = feature_names.front();
      }
      else
      {
        selected_feature_ = "";
      }
    }
    // Main graphic
    if (selected_feature_.empty())
    {
      ImGui::Text("No features to select");
    }
    else
    {
      if (ImGui::BeginCombo("Select feature", selected_feature_.c_str()))
      {
        for (const auto& feature_name : feature_names)
        {
          if (ImGui::Selectable(feature_name.c_str()))
          {
            selected_feature_ = feature_name;
          }
        }
        ImGui::EndCombo();
      }
      
      static FilePicker file_picker_;
      
      if (ImGui::Button("Choose folder"))
      {
        file_picker_.open("Choose folder to export plot", nullptr, SmartPeak::FilePicker::Mode::EDirectory, application_handler_);
      }
      file_picker_.draw();
      ImGui::SameLine();
      static int selected_format = 0;
      static const char* formats[] = { "Save As PNG", "Save As PDF", "Save As HTML", "Save As SVG"};
      ImGui::Combo(" ", &selected_format, formats, IM_ARRAYSIZE(formats));
      ImGui::SameLine();
      if (ImGui::Button("Save Plot"))
      {
        auto exported_plot = std::make_unique<PlotExporter>(
          file_picker_.getPickedPathname().empty() ? application_handler_.main_dir_.string() : file_picker_.getPickedPathname(),
          heatmap_data_, selected_format, PlotExporter::PlotType::HEATMAP);
        exported_plot->plot();
      }
      ImGui::Spacing();
      // Check if we need to refresh the data
      Eigen::Tensor<std::string, 1> selected_sample_names = session_handler_.getSelectSampleNamesPlot();
      Eigen::Tensor<std::string, 1> selected_transitions = session_handler_.getSelectTransitionsPlot();
      Eigen::Tensor<std::string, 1> selected_transition_groups = session_handler_.getSelectTransitionGroupsPlot();
      if ((refresh_needed_) || // data changed
          (selected_feature_ != heatmap_data_.selected_feature_) ||
          !compareInput(selected_sample_names, heatmap_data_.selected_sample_names_) ||
          !compareInput(selected_transitions, heatmap_data_.selected_transitions_) ||
          !compareInput(selected_transition_groups, heatmap_data_.selected_transition_groups_))
      {
        session_handler_.getHeatMap(sequence_handler_, heatmap_data_, selected_feature_);
        // We need to handle "invalid" data - infinite values, or very high - which will crash ImPlot.
        invalid_data_ = false;
        for (int i = 0; i < heatmap_data_.feat_heatmap_row_labels.size(); ++i)
        {
          for (int j = 0; j < heatmap_data_.feat_heatmap_col_labels.size(); ++j)
          {
            int index = i * heatmap_data_.feat_heatmap_row_labels.size() + j;
            if (heatmap_data_.feat_heatmap_data.data()[index] > high_value_threeshold_)
            {
              LOGE << selected_feature_ << heatmap_data_.feat_heatmap_col_labels(j) << " / " << heatmap_data_.feat_heatmap_row_labels(i)
                                        << " contains unexpected high values (" << heatmap_data_.feat_heatmap_data.data()[index] << "), cannot display Heatmap";
              invalid_data_ = true;
              break;
            }
          }
        }
        data_mismatch_ = (heatmap_data_.feat_heatmap_data.dimension(0) != heatmap_data_.feat_heatmap_row_labels.size()
                            || heatmap_data_.feat_heatmap_data.dimension(1) != heatmap_data_.feat_heatmap_col_labels.size());
        if (data_mismatch_)
        {
          LOGE << "Labels and Data mismatch. Cannot display Heatmap.";
        }
        refresh_needed_ = false;
      }
      
      if (invalid_data_)
      {
        ImGui::Text("Some data for this feature seem to be Invalid (unexpected high), cannot display Heatmap");
      }
      else if (data_mismatch_)
      {
        ImGui::Text("Data mismatch, cannot display Heatmap");
      }
      else
      {
        if (heatmap_data_.feat_heatmap_row_labels.size() > 0 && heatmap_data_.feat_heatmap_col_labels.size() > 0)
        {
          if (heatmap_data_.feat_value_min_ == heatmap_data_.feat_value_max_)
          {
            // ImPlot will fail displaying this exceptional case where min and max are equal
            // (which arrive, most of the time, when you select only one sample/transition)
            ImGui::Text("Select more samples or transitions to display the Heatmap.");
          }
          else
          {
            bool is_hovered = false;
            ImPlotPoint plot_point;
            auto window_size = ImGui::GetWindowSize();
            ImPlot::SetColormap(ImPlotColormap_Jet);
            ImPlot::ShowColormapScale(heatmap_data_.feat_value_min_, heatmap_data_.feat_value_max_, window_size.y - 70);
            float available_width = ImGui::GetContentRegionAvailWidth();
            ImGui::SameLine();
            const ImPlotFlags imPlotFlags = ImPlotAxisFlags_LockMin | ImPlotAxisFlags_LockMax | ImPlotAxisFlags_TickLabels;
            if (ImPlot::BeginPlot(plot_title_.c_str(),
              heatmap_data_.feat_heatmap_x_axis_title.c_str(),
              heatmap_data_.feat_heatmap_y_axis_title.c_str(),
              ImVec2(available_width - 80, window_size.y - 70),
              imPlotFlags,
              ImPlotAxisFlags_GridLines,
              ImPlotAxisFlags_GridLines))
            {
              ImPlot::PlotHeatmap(("##" + plot_title_).c_str(),
                heatmap_data_.feat_heatmap_data.data(),
                heatmap_data_.feat_heatmap_row_labels.size(),
                heatmap_data_.feat_heatmap_col_labels.size(),
                heatmap_data_.feat_value_min_,
                heatmap_data_.feat_value_max_);
              is_hovered = ImPlot::IsPlotHovered();
              if (is_hovered)
              {
                plot_point = ImPlot::GetPlotMousePos();
              }
              ImPlot::EndPlot();
            }
            ImPlot::SetColormap(ImPlotColormap_Default);
            if (is_hovered)
            {
              const size_t index_item_x = (size_t)((plot_point.x*(float)heatmap_data_.feat_heatmap_col_labels.size()));
              const size_t index_item_y = (size_t)(((1.0-plot_point.y)*(float)heatmap_data_.feat_heatmap_row_labels.size()));
              if ((index_item_x < heatmap_data_.feat_heatmap_col_labels.size()) &&
                  (index_item_y < heatmap_data_.feat_heatmap_row_labels.size()))
              {
                std::string selected_sample = heatmap_data_.feat_heatmap_col_labels(index_item_x);
                std::string selected_transition = heatmap_data_.feat_heatmap_row_labels(index_item_y);
                ImGui::BeginTooltip();
                std::ostringstream os;
                os << "Injection: " << selected_sample;
                ImGui::Text("%s", os.str().c_str());
                os.str("");
                os.clear();
                os << "Transition: " << selected_transition;
                ImGui::Text("%s", os.str().c_str());
                os.str("");
                os.clear();
                os << "Value: " << heatmap_data_.feat_heatmap_data.data()[index_item_y * heatmap_data_.feat_heatmap_col_labels.size() + index_item_x];
                ImGui::Text("%s", os.str().c_str());
                ImGui::EndTooltip();
              }
            }
          }
        }
        else
        {
          ImGui::Selectable("No data for this feature");
        }
      }
    }
  }

  bool Heatmap2DWidget::compareInput(const Eigen::Tensor<std::string, 1>& l, const Eigen::Tensor<std::string, 1>& r) const
  {
    if (l.size() != r.size())
    {
      return false;
    }
    else
    {
      for (int i = 0 ; i < l.size(); ++i)
      {
        if (l(i) != r(i))
        {
          return false;
        }
      }
      return true;
    }
  }

  void Heatmap2DWidget::onSequenceUpdated()
  {
    refresh_needed_ = true;
  }
}
