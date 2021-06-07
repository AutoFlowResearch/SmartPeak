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
// $Maintainer: Douglas McCloskey, Ahmed Khalil, Bertrand Boudaud $
// $Authors: Douglas McCloskey $
// --------------------------------------------------------------------------

#pragma once

#include <SmartPeak/ui/Widget.h>
#include <SmartPeak/core/SessionHandler.h>

#include <string>
#include <utility>
#include <vector>
#include <functional>
#include <optional>

namespace SmartPeak
{

  /**
  @brief Base class for Chromatograms and Spectrograms
  */
  class GraphicDataVizWidget : public GenericGraphicWidget
  {
  public:
    GraphicDataVizWidget(SessionHandler& session_handler,
      SequenceHandler& sequence_handler,
      const std::string& id,
      const std::string& title) :
      GenericGraphicWidget(title),
      session_handler_(session_handler),
      sequence_handler_(sequence_handler),
      plot_title_(id) {};

    void draw() override;
    
    virtual void setMarkerPosition(const std::optional<float>& marker_position);
    virtual std::optional<float> getMarkerPosition() const;

  protected:
    virtual void drawSliders();
    virtual void drawGraph();
    virtual void drawMarkers();
    virtual void updateData() = 0;
    virtual std::tuple<float, float, float, float> plotLimits() const;
    
    // Utility methods
    std::set<std::string> getSelectedSampleNames() const;
    std::set<std::string> getSelectedTransitions() const;
    std::set<std::string> getSelectedSpectrum() const;
    std::set<std::string> getSelectedTransitionGroups() const;
    std::optional<float>  getNearestPoint(float in_x) const;

  protected:
    SessionHandler& session_handler_;
    SequenceHandler& sequence_handler_;
    const std::string plot_title_; // used as the ID of the plot as well so this should be unique across the different Widgets
    bool show_legend_ = true;
    bool compact_view_ = true;
    SessionHandler::GraphVizData chrom_;
    bool refresh_needed_ = false;
    std::pair<float, float> slider_min_max_ = { 0.0f, 0.0f };
    std::pair<float, float> current_range_ = { 0.0f, 0.0f };
    int current_z_ = 0;
    float sliders_height_ = 0.0f;
    std::optional<float> marker_position_;
    bool use_markers_ = false;
    bool is_spectra_ = false;
  };

}
