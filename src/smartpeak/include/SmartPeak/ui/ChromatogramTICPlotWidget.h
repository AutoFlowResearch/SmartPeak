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
// $Maintainer: Douglas McCloskey $
// $Authors: Douglas McCloskey $
// --------------------------------------------------------------------------

#pragma once

#include <SmartPeak/ui/GraphicDataVizWidget.h>
#include <SmartPeak/iface/ISequenceObserver.h>
#include <SmartPeak/ui/SpectraMSMSPlotWidget.h>

#include <string>
#include <utility>
#include <vector>
#include <imgui.h>

namespace SmartPeak
{
  /**
    @brief Class for plotting Total ion current (TIC) Chromatograms
  */
  class ChromatogramTICPlotWidget : 
    public GraphicDataVizWidget,
    public ISequenceObserver
  {
  public:
    ChromatogramTICPlotWidget(SessionHandler& session_handler,
      SequenceHandler& sequence_handler,
      const std::string& id,
      const std::string& title,
      std::shared_ptr<SpectraMSMSPlotWidget> spectra_ms1_plot_widget,
      SequenceObservable& sequence_observable) :
      GraphicDataVizWidget(session_handler, sequence_handler, id, title),
      spectra_msms_plot_widget_(spectra_ms1_plot_widget)
    {
      use_markers_ = true;
      sequence_observable.addSequenceObserver(this);
    };

    /**
      GraphicDataVizWidget
    */
    virtual void setMarkerPosition(const std::optional<float>& marker_position) override;
    virtual std::optional<float> getMarkerPosition() const override;

    /**
      ISequenceObserver
    */
    virtual void onSequenceUpdated() override;

  protected:
    virtual void updateData() override;

  protected:
    // input used to create the graph
    std::set<std::string> input_sample_names_;
    std::set<std::string> input_scan_names_;
    std::shared_ptr<SpectraMSMSPlotWidget> spectra_msms_plot_widget_;
  };
}