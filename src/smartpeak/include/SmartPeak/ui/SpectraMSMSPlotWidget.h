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
#include <SmartPeak/ui/ChromatogramXICPlotWidget.h>

#include <string>
#include <utility>
#include <vector>

namespace SmartPeak
{

  /**
    @brief Class for plotting MS1 and MS2 Spectra
  */
  class SpectraMSMSPlotWidget : 
    public GraphicDataVizWidget,
    public ISequenceObserver
  {
  public:
    SpectraMSMSPlotWidget(SessionHandler& session_handler,
      ApplicationHandler& application_handler,
      const std::string& id,
      const std::string& title,
      std::shared_ptr<ChromatogramXICPlotWidget> chromatogram_xic_widget,
      const int ms_level,
      SequenceObservable& sequence_observable) :
      GraphicDataVizWidget(session_handler, application_handler, id, title),
      chromatogram_xic_widget_(chromatogram_xic_widget),
      ms_level_(ms_level)
    {
      use_markers_ = (chromatogram_xic_widget != nullptr);
      is_spectra_ = true;
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

    void setCurrentRT(const float current_rt);
    float getCurrentRT() const;

  protected:
    virtual void updateData() override;

  private:
    int getScanIndexFromRetentionTime(const float& retention_time) const;

  protected:
    // input used to create the graph
    std::set<std::string> input_sample_names_;
    std::set<std::string> input_scan_names_;
    std::set<std::string> input_component_group_names_;
    int input_z_ = 0;
    std::shared_ptr<ChromatogramXICPlotWidget> chromatogram_xic_widget_;
    int ms_level_;
    float current_rt_ = 0.0f;
  };

}
