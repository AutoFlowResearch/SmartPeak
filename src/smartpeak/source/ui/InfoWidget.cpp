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
// $Maintainer: Douglas McCloskey, Bertrand Boudaud $
// $Authors: Douglas McCloskey, Bertrand Boudaud $
// --------------------------------------------------------------------------

#include <SmartPeak/ui/InfoWidget.h>
#include <SmartPeak/core/ApplicationProcessor.h>
#include <imgui.h>
#include <imgui_internal.h>
#include <plog/Log.h>
#include <SmartPeak/core/SharedProcessors.h>
#include <implot.h>
#include <math.h>

namespace SmartPeak
{

  void InfoWidget::draw()
  {
    drawWorkflowStatus();
    drawFileloadingStatus();
    drawLastRunTime();
    drawErrorMessages();

    ImGui::Separator();

    drawSamples();
    drawTransition();
    drawChromatograms();
    drawSpectrums();

    refresh_needed_ = false;
  }

  void InfoWidget::drawWorkflowStatus()
  {
    ImGui::Text(workflow_is_done_ ? "Workflow status: done" : "Workflow status: running...");
  }

  void InfoWidget::drawFileloadingStatus()
  {
    if (file_load_error_)
    {
      std::ostringstream os;
      os << "File loading failed.  Check the `Information` log.";
      ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
      ImGui::Text(os.str().c_str());
      ImGui::PopStyleColor();
    }
    else
    {
      ImGui::Text(file_loading_is_done_ ? "File loading status: done" : "File loading status: running...");
    }
  }

  void InfoWidget::drawChromatograms()
  {
    std::ostringstream os;
    if (refresh_needed_)
    {
      number_of_chromatograms_ = 0;
      if (application_handler_.sequenceHandler_.getSequence().size() > 0)
      {
        number_of_chromatograms_ = application_handler_.sequenceHandler_.getSequence().at(0).getRawData().getChromatogramMap().getChromatograms().size();
      }
    }
    os << "Number of chromatograms: " << number_of_chromatograms_;
    ImGui::Text(os.str().c_str());
  }

  void InfoWidget::drawSpectrums()
  {
    std::ostringstream os;
    if (refresh_needed_)
    {
      number_of_spectrums_ = 0;
      if (application_handler_.sequenceHandler_.getSequence().size() > 0)
      {
        number_of_spectrums_ = application_handler_.sequenceHandler_.getSequence().at(0).getRawData().getExperiment().getSpectra().size();
      }
    }
    os << "Number of spectrums: " << number_of_spectrums_;
    ImGui::Text(os.str().c_str());
  }

  void InfoWidget::drawSamples()
  {
    std::ostringstream os;
    if (refresh_needed_)
    {
      number_of_samples_ = application_handler_.sequenceHandler_.getSequence().size();
    }
    os << "Number of samples: " << number_of_samples_;
    ImGui::Text(os.str().c_str());
  }

  void InfoWidget::drawTransition()
  {
    std::ostringstream os;
    if (refresh_needed_)
    {
      number_of_transitions_ = 0;
      if (transitions_)
      {
        number_of_transitions_ = transitions_->dimension(0);
      }
    }
    os << "Number of transitions: " << number_of_transitions_;
    ImGui::Text(os.str().c_str());
  }

  void InfoWidget::drawErrorMessages()
  {
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
    for (const auto& error_message : error_messages_)
    {
      ImGui::Text(error_message.c_str());
    }
    ImGui::PopStyleColor();
  }

  void InfoWidget::drawLastRunTime()
  {
    std::ostringstream os;
    if (last_run_time_ != std::chrono::steady_clock::duration::zero())
    {
      auto ns = last_run_time_;
      os << "Last time workflow execution: ";
      auto h = std::chrono::duration_cast<std::chrono::hours>(ns);
      ns -= h;
      auto m = std::chrono::duration_cast<std::chrono::minutes>(ns);
      ns -= m;
      auto s = std::chrono::duration_cast<std::chrono::seconds>(ns);
      os << std::setfill('0') << std::setw(2) << h.count() << "h:"
        << std::setw(2) << m.count() << "m:"
        << std::setw(2) << s.count() << 's';
      ImGui::Text(os.str().c_str());
    }
  }
    
  void InfoWidget::sequenceUpdated()
  {
    refresh_needed_ = true;
  }
}
