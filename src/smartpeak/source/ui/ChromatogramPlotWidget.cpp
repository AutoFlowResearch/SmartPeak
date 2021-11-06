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

#include <SmartPeak/ui/ChromatogramPlotWidget.h>

namespace SmartPeak
{
  void ChromatogramPlotWidget::updateData()
  {
    const std::set<std::string> sample_names = getSelectedSampleNames();
    const std::set<std::string> component_names = getSelectedTransitions();

    if ((refresh_needed_) || // data changed
       ((input_component_names_ != component_names) || (input_sample_names_ != sample_names))) // user select different items
    {
      // we may recompute the RT window, get the whole graph area
      session_handler_.getChromatogramScatterPlot(sequence_handler_, graph_viz_data_, std::make_pair(0, 1800), sample_names, component_names);
      updateRanges();
      input_sample_names_ = sample_names;
      input_component_names_ = component_names;
      refresh_needed_ = false;
    }
    else if (input_range_ != current_range_) // user zoom in/out
    {
      session_handler_.getChromatogramScatterPlot(sequence_handler_, graph_viz_data_, current_range_, sample_names, component_names);
      input_range_ = current_range_;
    }
  };

  void ChromatogramPlotWidget::onSequenceUpdated()
  {
    refresh_needed_ = true;
  }

}
