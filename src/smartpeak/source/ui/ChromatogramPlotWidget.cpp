#include <SmartPeak/ui/ChromatogramPlotWidget.h>

namespace SmartPeak
{
  void ChromatogramPlotWidget::updateScatterPlotData()
  {
    // get the selected sample names
    Eigen::Tensor<std::string, 1> selected_sample_names = session_handler_.getSelectSampleNamesPlot();
    std::set<std::string> sample_names;
    for (int i = 0; i < selected_sample_names.size(); ++i) {
      if (!selected_sample_names(i).empty())
        sample_names.insert(selected_sample_names(i));
    }
    // get the selected transitions
    Eigen::Tensor<std::string, 1> selected_transitions = session_handler_.getSelectTransitionsPlot();
    std::set<std::string> component_names;
    for (int i = 0; i < selected_transitions.size(); ++i) {
      if (!selected_transitions(i).empty())
        component_names.insert(selected_transitions(i));
    }

    if ((refresh_needed_) || // data changed
       ((input_component_names_ != component_names) || (input_sample_names_ != sample_names))) // user select different items
    {
      // we may recompute the RT window, get the whole graph area
      session_handler_.getChromatogramScatterPlot(sequence_handler_, chrom_, std::make_pair(0, 1800), sample_names, component_names);
      if ((std::abs(slider_min_max_.first - chrom_.x_min_) > std::numeric_limits<double>::epsilon()) ||
          (std::abs(slider_min_max_.second - chrom_.x_max_) > std::numeric_limits<double>::epsilon()))
      {
        // min max changed, reset the sliders and current range
        current_range_ = slider_min_max_ = std::make_pair(chrom_.x_min_, chrom_.x_max_);
      }
      input_range_ = std::make_pair(chrom_.x_min_, chrom_.x_max_);
      input_sample_names_ = sample_names;
      input_component_names_ = component_names;
      refresh_needed_ = false;
    }
    else if (input_range_ != current_range_) // user zoom in/out
    {
      session_handler_.getChromatogramScatterPlot(sequence_handler_, chrom_, current_range_, sample_names, component_names);
      input_range_ = current_range_;
    }
  };

}
