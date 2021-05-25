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
#include <SmartPeak/core/ApplicationProcessorObservable.h>
#include <SmartPeak/core/SequenceProcessorObservable.h>
#include <SmartPeak/core/SequenceSegmentProcessorObservable.h>
#include <SmartPeak/core/SampleGroupProcessorObservable.h>
#include <SmartPeak/core/ProgressInfo.h>
#include <SmartPeak/iface/ISequenceObserver.h>
#include <SmartPeak/core/ProgressInfo.h>

#include <string>
#include <vector>
#include <chrono>

namespace SmartPeak
{
  class InfoWidget final : 
    public Widget, 
    public ISequenceObserver
  {

  public:
    InfoWidget(const std::string title, 
               ApplicationHandler & application_handler,
               ApplicationProcessorObservable& application_processor_observable,
               SequenceProcessorObservable& sequence_processor_observable,
               SequenceSegmentProcessorObservable& sequence_segment_processor_observable,
               SampleGroupProcessorObservable& sample_group_processor_observable,
               SequenceObservable& sequence_observable)
      : Widget(title),
        application_handler_(application_handler),
        progress_info_(application_processor_observable,
                       sequence_processor_observable,
                       sequence_segment_processor_observable,
                       sample_group_processor_observable)
    {
      sequence_observable.addSequenceObserver(this);
    };

    /**
      ISequenceObserver
    */
    virtual void onSequenceUpdated() override;

  public:
    void draw() override;

    void setTransitions(const Eigen::Tensor<std::string, 2>* transitions) { transitions_ = transitions; };

    void setFileLoadingDone(bool done, bool load_error)  { file_loading_is_done_ = done; file_load_error_ = load_error;};
    void clearErrorMessages() { error_messages_.clear(); };
    void addErrorMessage(const std::string& error_message) { error_messages_.push_back(error_message); };

  private:
    void drawWorkflowStatus();
    void drawFileloadingStatus();
    void drawErrorMessages();
    void drawChromatograms();
    void drawSpectrums();
    void drawSamples();
    void drawTransition();
    void drawLastRunTime();
    void drawWorkflowProgress();
    std::string formattedTime(const std::chrono::steady_clock::duration& duration) const;

  protected:
    ApplicationHandler& application_handler_;
    const Eigen::Tensor<std::string, 2>* transitions_ = nullptr;
    int number_of_chromatograms_ = 0;
    int number_of_spectrums_ = 0;
    int number_of_samples_ = 0;
    int number_of_transitions_ = 0;
    bool file_loading_is_done_ = true;
    bool file_load_error_ = false;
    bool refresh_needed_ = true;
    std::vector<std::string> error_messages_;
    int spinner_counter_ = 0;
    ProgressInfo progress_info_;
  };
}
