#include <SmartPeak/ui/SpectraPlotWidget.h>

namespace SmartPeak
{

  void SpectraPlotWidget::updateScatterPlotData()
  {
    // get the selected sample names
    Eigen::Tensor<std::string, 1> selected_sample_names = session_handler_.getSelectSampleNamesPlot();
    std::set<std::string> sample_names;
    for (int i = 0; i < selected_sample_names.size(); ++i) {
      if (!selected_sample_names(i).empty())
        sample_names.insert(selected_sample_names(i));
    }
    // get the selected scans
    Eigen::Tensor<std::string, 1> selected_scans = session_handler_.getSelectSpectrumPlot();
    std::set<std::string> scan_names;
    for (int i = 0; i < selected_scans.size(); ++i) {
      if (!selected_scans(i).empty())
        scan_names.insert(selected_scans(i));
    }
    // get the selected transitions
    Eigen::Tensor<std::string, 1> selected_transition_groups = session_handler_.getSelectTransitionGroupsPlot();
    std::set<std::string> component_group_names;
    for (int i = 0; i < selected_transition_groups.size(); ++i) {
      if (!selected_transition_groups(i).empty())
        component_group_names.insert(selected_transition_groups(i));
    }

    if ((refresh_needed_) || // data changed
       ((input_sample_names_ != sample_names) || (input_scan_names_ != scan_names) || (input_component_group_names_ != component_group_names))) // user select different items
    {
      // get the whole graph area
      session_handler_.getSpectrumScatterPlot(sequence_handler_, chrom_, std::make_pair(0, 2000), sample_names, scan_names, component_group_names);
      if ((std::abs(slider_min_max_.first - chrom_.x_min_) > std::numeric_limits<double>::epsilon()) ||
          (std::abs(slider_min_max_.second - chrom_.x_max_) > std::numeric_limits<double>::epsilon()))
      {
        // min max changed, reset the sliders and current range
        current_range_ = slider_min_max_ = std::make_pair(chrom_.x_min_, chrom_.x_max_);
      }
      input_range_ = std::make_pair(chrom_.x_min_, chrom_.x_max_);
      input_sample_names_ = sample_names;
      input_scan_names_ = scan_names;
      input_component_group_names_ = component_group_names;
      refresh_needed_ = false;
    }
    else if (input_range_ != current_range_) // user zoom in/out
    {
      session_handler_.getSpectrumScatterPlot(sequence_handler_, chrom_, current_range_, sample_names, scan_names,component_group_names);
      input_range_ = current_range_;
    }
  };

}
