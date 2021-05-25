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
// $Authors: Douglas McCloskey, Bertrand Boudaud $
// --------------------------------------------------------------------------

#pragma once

#include <SmartPeak/iface/IApplicationProcessorObserver.h>
#include <SmartPeak/core/ApplicationProcessorObservable.h>
#include <SmartPeak/iface/ISequenceProcessorObserver.h>
#include <SmartPeak/core/SequenceProcessorObservable.h>
#include <SmartPeak/iface/ISampleGroupProcessorObserver.h>
#include <SmartPeak/core/SampleGroupProcessorObservable.h>
#include <SmartPeak/iface/ISequenceSegmentProcessorObserver.h>
#include <SmartPeak/core/SequenceSegmentProcessorObservable.h>

#include <string>
#include <vector>
#include <chrono>
#include <optional>

namespace SmartPeak
{
  /**
    ProgressInfo holds information about current status of a running workflow,
    and provide estimation of remaining running time.
  */
  class ProgressInfo final :
    public IApplicationProcessorObserver,
    public ISequenceProcessorObserver,
    public ISequenceSegmentProcessorObserver,
    public ISampleGroupProcessorObserver
  {

  public:
    ProgressInfo(
      ApplicationProcessorObservable& application_processor_observable,
      SequenceProcessorObservable& sequence_processor_observable,
      SequenceSegmentProcessorObservable& sequence_segment_processor_observable,
      SampleGroupProcessorObservable& sample_group_processor_observable)
    {
      application_processor_observable.addApplicationProcessorObserver(this);
      sequence_processor_observable.addSequenceProcessorObserver(this);
      sequence_segment_processor_observable.addSequenceSegmentProcessorObserver(this);
      sample_group_processor_observable.addSampleGroupProcessorObserver(this);
    };

    /**
      IApplicationProcessorObserver
    */
    virtual void onApplicationProcessorStart(const std::vector<std::string>& commands) override;
    virtual void onApplicationProcessorCommandStart(size_t command_index, const std::string& command_name) override;
    virtual void onApplicationProcessorCommandEnd(size_t command_index, const std::string& command_name) override;
    virtual void onApplicationProcessorEnd() override;

    /**
      ISequenceProcessorObserver
    */
    virtual void onSequenceProcessorStart(const size_t nb_injections) override;
    virtual void onSequenceProcessorSampleStart(const std::string& sample) override;
    virtual void onSequenceProcessorSampleEnd(const std::string& sample) override;
    virtual void onSequenceProcessorEnd() override;

    /**
      ISequenceSegmentProcessorObserver
    */
    virtual void onSequenceSegmentProcessorStart(const size_t nb_segments) override;
    virtual void onSequenceSegmentProcessorSampleStart(const std::string& segment_name) override;
    virtual void onSequenceSegmentProcessorSampleEnd(const std::string& segment_name) override;
    virtual void onSequenceSegmentProcessorEnd() override;

    /**
      ISampleGroupProcessorObserver
    */
    virtual void onSampleGroupProcessorStart(const size_t nb_segments) override;
    virtual void onSampleGroupProcessorSampleStart(const std::string& segment_name) override;
    virtual void onSampleGroupProcessorSampleEnd(const std::string& segment_name) override;
    virtual void onSampleGroupProcessorEnd() override;
  
  public:

    /**
      @brief returns wether the workflow is running 
    */
    bool isRunning() const { return running_; }

    /**
      @brief returns time since workflow has been started
    */
    std::chrono::steady_clock::duration runningTime() const;

    /**
      @brief returns an estimated remaining time until the end of the workflow

      The value can increase or decrease during time, even can go to 0.
    */
    std::optional<std::chrono::steady_clock::duration> estimatedRemainingTime() const;

    /**
      @brief a value between 0.0 and 1.0 of the progress value.

      The value is based on the step progression and always increases.
    */
    float progressValue() const;

    /**
      @brief list of commands (running or not) of the workflow.
    */
    const std::vector<std::string>& allCommands() const { return all_commands_; };

    /**
      @brief list of running commands of the workflow.
    */
    const std::vector<std::tuple<size_t, std::string>>& runningCommands() const;

    /**
      @brief detailed about running injections, segment or sample groups
    */
    const std::vector<std::string>& runningBatch() const;

    /**
      @brief return running time of the last workflow executed
    */
    std::chrono::steady_clock::duration lastRunTime() const { return last_run_time_; };

  private:
    void onRunningBatchEndItem(const std::string& item_name);

  protected:
    bool running_ = false;
    std::vector<std::tuple<size_t, std::string>> running_commands_;
    /**
    * ApplicationProcessor's running commands
    */
    int commands_step_ = 0;
    std::vector<std::string> all_commands_;
    std::chrono::steady_clock::time_point application_processor_start_time_;
    /**
    * @brief RunningBatch used for sequence, sequence segments and sample groups
    */
    struct RunningBatch
    {
      RunningBatch(const size_t nb_items) : max_steps_(nb_items) {}
      std::vector<std::string> running_items_;
      size_t current_step_ = 0;
      size_t max_steps_;
      std::chrono::steady_clock::time_point last_item_end_time_point_;
    };
    std::shared_ptr<RunningBatch> running_batch_;
    std::chrono::steady_clock::duration last_run_time_ = std::chrono::steady_clock::duration::zero();
  };
}
