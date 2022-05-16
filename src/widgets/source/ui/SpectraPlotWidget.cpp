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

#include <SmartPeak/ui/SpectraPlotWidget.h>

namespace SmartPeak
{

  void SpectraPlotWidget::updateData()
  {
    const std::set<std::string> sample_names = getSelectedSampleNames();
    const std::set<std::string> component_group_names = getSelectedTransitionGroups();

    if ((refresh_needed_) || // data changed
       ((input_sample_names_ != sample_names) || (input_component_group_names_ != component_group_names))) // user select different items
    {
      // get the whole graph area
      session_handler_.getSpectrumScatterPlot(sequence_handler_, graph_viz_data_, std::make_pair(0, 2000), sample_names, component_group_names);
      updateRanges();
      input_sample_names_ = sample_names;
      input_component_group_names_ = component_group_names;
      refresh_needed_ = false;
    }
  };

  void SpectraPlotWidget::onSequenceUpdated()
  {
    refresh_needed_ = true;
  }
}
