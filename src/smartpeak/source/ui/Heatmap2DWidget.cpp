#include <SmartPeak/ui/Heatmap2DWidget.h>
#include <implot.h>

namespace SmartPeak
{

  void Heatmap2DWidget::draw()
  {
    Eigen::Tensor<std::string, 2> selected_feature_names = session_handler_.feature_explorer_body;
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
        // We need to handle "invalid" data - infinite values, or vey high - which will crash ImPlot.
        invalid_data_ = false;
        for (int i = 0; i < heatmap_data_.feat_heatmap_row_labels.size(); ++i)
        {
          for (int j = 0; j < heatmap_data_.feat_heatmap_col_labels.size(); ++j)
          {
            int index = i * heatmap_data_.feat_heatmap_row_labels.size() + j;
            if (heatmap_data_.feat_heatmap_data.data()[index] > 1e20)
            {
              LOGE << selected_feature_ << heatmap_data_.feat_heatmap_col_labels(j) << " / " << heatmap_data_.feat_heatmap_row_labels(i)
                                        << " contains unexpected high values (" << heatmap_data_.feat_heatmap_data.data()[index] << "), cannot display Heatmap";
              invalid_data_ = true;
              break;
            }
          }
        }
        refresh_needed_ = false;
      }
      
      if (invalid_data_)
      {
        ImGui::Text("Some data for this feature seem to be Invalid (unexpected high), cannot display Heatmap");
      }
      else
      {
        if (heatmap_data_.feat_heatmap_row_labels.size() > 0 && heatmap_data_.feat_heatmap_col_labels.size() > 0)
        {
          assert(heatmap_data_.feat_heatmap_data.dimension(0) == heatmap_data_.feat_heatmap_row_labels.size()
            && heatmap_data_.feat_heatmap_data.dimension(1) == heatmap_data_.feat_heatmap_col_labels.size());
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
            ImPlot::SetColormap(ImPlotColormap_Jet);
            ImPlot::ShowColormapScale(heatmap_data_.feat_value_min_, heatmap_data_.feat_value_max_, plot_height_ - 70);
            float available_width = ImGui::GetContentRegionAvailWidth();
            ImGui::SameLine();
            const ImPlotFlags imPlotFlags = ImPlotAxisFlags_LockMin | ImPlotAxisFlags_LockMax | ImPlotAxisFlags_TickLabels;
            if (ImPlot::BeginPlot(plot_title_.c_str(),
              heatmap_data_.feat_heatmap_x_axis_title.c_str(),
              heatmap_data_.feat_heatmap_y_axis_title.c_str(),
              ImVec2(available_width - 80, plot_height_ - 70),
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
                os << "Sample: " << selected_sample;
                ImGui::Text(os.str().c_str());
                os.str("");
                os.clear();
                os << "Component: " << selected_transition;
                ImGui::Text(os.str().c_str());
                os.str("");
                os.clear();
                os << "Value: " << heatmap_data_.feat_heatmap_data.data()[index_item_y * heatmap_data_.feat_heatmap_col_labels.size() + index_item_x];
                ImGui::Text(os.str().c_str());
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

}
