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

    if (refresh_needed_)
    {
      // get the whole graph area
      current_range_ = std::make_pair(0, 1800);
      session_handler_.getChromatogramScatterPlot(sequence_handler_, chrom_, current_range_, sample_names, component_names);
      current_range_ = slider_min_max_ = input_range_ = std::make_pair(chrom_.x_min_, chrom_.x_max_);
      refresh_needed_ = false;
    }
    if ((input_range_ != current_range_) || (input_component_names_ != component_names) || (input_sample_names_ != sample_names))
    {
      session_handler_.getChromatogramScatterPlot(sequence_handler_, chrom_, current_range_, sample_names, component_names);
      input_range_ = current_range_;
      input_sample_names_ = sample_names;
      input_component_names_ = component_names;
    }
  };

}
