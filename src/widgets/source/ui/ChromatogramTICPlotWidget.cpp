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

#include <SmartPeak/ui/ChromatogramTICPlotWidget.h>
#include <implot.h>

namespace SmartPeak
{
  void ChromatogramTICPlotWidget::updateData()
  {
    // Update marker with sub graph's value
    if (use_markers_)
    {
      auto rt = spectra_msms_plot_widget_->getCurrentRT();
      marker_position_ = getNearestPoint(rt);
    }

    const std::set<std::string> sample_names = getSelectedSampleNames();
    const std::set<std::string> scan_names = getSelectedSpectrum();

    if ((refresh_needed_) || // data changed
       ((input_scan_names_ != scan_names) || (input_sample_names_ != sample_names))) // user select different items
    {
      // get the whole graph area
      session_handler_.getChromatogramTIC(sequence_handler_, graph_viz_data_, std::make_pair(0, 1800), sample_names, scan_names);
      updateRanges();
      input_sample_names_ = sample_names;
      input_scan_names_ = scan_names;
      refresh_needed_ = false;
    }
    graph_viz_data_.y_min_ = 0.0f; // bottom line will start from 0.0
  };

  void ChromatogramTICPlotWidget::onSequenceUpdated()
  {
    refresh_needed_ = true;
  }

  void ChromatogramTICPlotWidget::setMarkerPosition(const std::optional<float>& marker_position)
  {
    GraphicDataVizWidget::setMarkerPosition(marker_position);
    if (marker_position)
    {
      spectra_msms_plot_widget_->setCurrentRT(*marker_position);
      spectra_msms_plot_widget_->visible_ = true;
    }
  }

  std::optional<float> ChromatogramTICPlotWidget::getMarkerPosition() const
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
