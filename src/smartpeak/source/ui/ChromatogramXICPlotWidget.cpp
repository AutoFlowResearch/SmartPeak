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

#include <SmartPeak/ui/ChromatogramXICPlotWidget.h>
#include <SmartPeak/ui/SpectraMSMSPlotWidget.h>
#include <implot.h>

namespace SmartPeak
{
  void ChromatogramXICPlotWidget::updateData()
  {
    // Update marker with sub graph's value
    if (use_markers_)
    {
      auto rt = spectra_msms_plot_widget_->getCurrentRT();
      marker_position_ = getNearestPoint(rt);
    }

    const std::set<std::string> sample_names = getSelectedSampleNames();
    const std::set<std::string> transitions_names = getSelectedTransitions();

    if ((refresh_needed_) || // data changed
       ((input_component_names_ != transitions_names) || (input_sample_names_ != sample_names))) // user select different items
    {
      // we may recompute the RT window, get the whole graph area
      if (!SmartPeak::run_on_server) {
      session_handler_.getChromatogramXIC(sequence_handler_, graph_viz_data_, std::make_pair(0, 1800), sample_names, transitions_names, current_mz_);
      }
      updateRanges();
      input_mz_ = current_mz_;
      input_sample_names_ = sample_names;
      input_component_names_ = transitions_names;
      refresh_needed_ = false;
    }
    else if ((input_range_ != current_range_) || (input_mz_ != current_mz_)) // user zoom in/out
    {
      if (!SmartPeak::run_on_server) {
        session_handler_.getChromatogramXIC(sequence_handler_, graph_viz_data_, current_range_, sample_names, transitions_names, current_mz_);
      }
      input_range_ = current_range_;
      input_mz_ = current_mz_;
    }
    graph_viz_data_.y_min_ = 0.0f; // bottom line will start from 0.0
  };

  void ChromatogramXICPlotWidget::onSequenceUpdated()
  {
    refresh_needed_ = true;
  }

  void ChromatogramXICPlotWidget::setMarkerPosition(const std::optional<float>& marker_position)
  {
    GraphicDataVizWidget::setMarkerPosition(marker_position);
    if (marker_position)
    {
      spectra_msms_plot_widget_->setCurrentRT(*marker_position);
      spectra_msms_plot_widget_->visible_ = true;
    }
  }

  std::optional<float> ChromatogramXICPlotWidget::getMarkerPosition() const
  {
    if (!spectra_msms_plot_widget_->visible_)
    {
      return {};
    }
    else
    {
      return GraphicDataVizWidget::getMarkerPosition();
    }
  }

}
