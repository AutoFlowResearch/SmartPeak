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

#pragma once

#include <SmartPeak/ui/Widget.h>
#include <SmartPeak/core/ApplicationHandler.h>
#include <SmartPeak/iface/ISequenceObserver.h>
#include <string>
#include <vector>
#include <chrono>

namespace SmartPeak
{
  class InfoWidget final : public Widget, public ISequenceObserver
  {

  public:
    InfoWidget(const std::string title = "")
      : Widget(title)
    {};

    /**
     ISequenceObserver
    */
    virtual void sequenceUpdated() override;

  public:
    void draw() override;
    void setRefreshNeeded() { refresh_needed_ = true; };

    void setApplicationHandler(ApplicationHandler& application_handler);
    void setTransitions(const Eigen::Tensor<std::string, 2>* transitions) { transitions = transitions_; };

    void setWorkflowDone(bool done) { workflow_is_done_ = done; };
    void setFileLoadingDone(bool done, bool load_error)  { file_loading_is_done_ = done; file_load_error_ = load_error;};
    void clearErrorMessages() { error_messages_.clear(); };
    void addErrorMessage(const std::string& error_message) { error_messages_.push_back(error_message); };
    void setLastRunTime(const std::chrono::steady_clock::duration& last_run_time) { last_run_time_ = last_run_time; };

  private:
    void drawWorkflowStatus();
    void drawFileloadingStatus();
    void drawErrorMessages();
    void drawChromatograms();
    void drawSpectrums();
    void drawSamples();
    void drawTransition();
    void drawLastRunTime();

  protected:
    ApplicationHandler* application_handler_ = nullptr;
    const Eigen::Tensor<std::string, 2>* transitions_ = nullptr;
    std::chrono::steady_clock::duration last_run_time_ = std::chrono::steady_clock::duration::zero();
    int number_of_chromatograms_ = 0;
    int number_of_spectrums_ = 0;
    int number_of_samples_ = 0;
    int number_of_transitions_ = 0;
    bool workflow_is_done_ = true;
    bool file_loading_is_done_ = true;
    bool file_load_error_ = false;
    bool refresh_needed_ = true;
    std::vector<std::string> error_messages_;
  };
}
