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
// $Maintainer: Bertrand Boudaud, Douglas McCloskey $
// $Authors: Bertrand Boudaud $
// --------------------------------------------------------------------------

#include <SmartPeak/ui/SpectraMSMSPlotWidget.h>
#include <implot.h>

namespace SmartPeak
{

  void SpectraMSMSPlotWidget::updateData()
  {
    // Update marker with sub graph's value
    if (use_markers_)
    {
      auto mz = chromatogram_xic_widget_->getMZ();
      marker_position_ = getNearestPoint(mz);
    }

    const std::set<std::string> sample_names = getSelectedSampleNames();    
    const std::set<std::string> component_group_names = getSelectedTransitionGroups();

    static const auto init_range = std::make_pair(0, 2000);
    if ((refresh_needed_) || // data changed
       ((input_sample_names_ != sample_names) || (input_component_group_names_ != component_group_names)) || // user select different items
       (input_z_ != current_z_) // user select different rt
       ) 
    {
      // get the whole graph area
      if (graph_viz_data_.z_data_area_.size())
      {
        current_rt_ = graph_viz_data_.z_data_area_[current_z_];
      }
      session_handler_.getSpectrumMSMSPlot(sequence_handler_, graph_viz_data_, init_range, sample_names, component_group_names, current_rt_, ms_level_);
      updateRanges();
      input_sample_names_ = sample_names;
      input_component_group_names_ = component_group_names;
      current_z_ = getScanIndexFromRetentionTime(current_rt_);
      input_z_ = current_z_;
      refresh_needed_ = false;
    }
    graph_viz_data_.y_min_ = 0.0f; // bottom line will start from 0.0
  };

  void SpectraMSMSPlotWidget::onSequenceUpdated()
  {
    refresh_needed_ = true;
  }

  void SpectraMSMSPlotWidget::setMarkerPosition(const std::optional<float>& marker_position)
  {
    GraphicDataVizWidget::setMarkerPosition(marker_position);
    if (ms_level_ == 1) // XIC is not supported at the moment for MS level 2
    {
      if (marker_position)
      {
        chromatogram_xic_widget_->setMZ(*marker_position);
        chromatogram_xic_widget_->visible_ = true;
      }
    }
  }

  std::optional<float> SpectraMSMSPlotWidget::getMarkerPosition() const
  {
    if (!chromatogram_xic_widget_->visible_)
    {
      return {};
    }
    else
    {
      return GraphicDataVizWidget::getMarkerPosition();
    }
  }

  int SpectraMSMSPlotWidget::getScanIndexFromRetentionTime(const float& retention_time) const
  {
    if (graph_viz_data_.z_data_area_.empty())
    {
      return 0;
    }
    int result = 0;
    float current_distance = std::numeric_limits<float>::max();
    for (const auto& z : graph_viz_data_.z_data_area_)
    {
      auto distance = std::abs(z - retention_time);
      if (distance > current_distance)
      {
        break;
      }
      current_distance = distance;
      result++;
    }
    // out of range
    if (result == graph_viz_data_.z_data_area_.size())
    {
      result = graph_viz_data_.z_data_area_.size() - 1;
    }
    return result;
  }

  void SpectraMSMSPlotWidget::setCurrentRT(const float current_rt)
  {
    current_z_ = getScanIndexFromRetentionTime(current_rt);
    current_rt_ = current_rt;
    refresh_needed_ = true;
  }

  float SpectraMSMSPlotWidget::getCurrentRT() const
  {
    return current_rt_;
  }
}
