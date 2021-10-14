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

#include <SmartPeak/core/ProgressInfo.h>
#include <plog/Log.h>

namespace SmartPeak
{

  void ProgressInfo::onApplicationProcessorStart(const std::vector<std::string>& commands)
  {
    running_ = true;
    commands_step_ = 0;
    all_commands_ = commands;
    application_processor_start_time_ = std::chrono::steady_clock::now();
    errors_.clear();
  }

  void ProgressInfo::onApplicationProcessorCommandStart(size_t command_index, const std::string& command_name)
  {
    running_commands_.push_back(std::make_tuple(command_index, command_name));
  }

  void ProgressInfo::onApplicationProcessorCommandEnd(size_t command_index, const std::string& command_name)
  {
    running_commands_.erase(std::remove(running_commands_.begin(), 
                                        running_commands_.end(), 
                                        std::make_tuple(command_index, command_name)), 
                                        running_commands_.end());
    commands_step_++;
  }

  void ProgressInfo::onApplicationProcessorEnd()
  {
    last_run_time_ = runningTime();
    running_batch_ = nullptr;
    running_ = false;
  }

  void ProgressInfo::onApplicationProcessorError(const std::string& error)
  {
    errors_.push_back(error);
  }

  void ProgressInfo::onSequenceProcessorStart(const size_t nb_injections)
  {
    running_batch_ = std::make_shared<RunningBatch>(nb_injections);
  }

  void ProgressInfo::onSequenceProcessorSampleStart(const std::string& sample_name)
  {
    running_batch_->running_items_.push_back(sample_name);
  }

  void ProgressInfo::onSequenceProcessorSampleEnd(const std::string& sample_name)
  {
    onRunningBatchEndItem(sample_name);
  }

  void ProgressInfo::onSequenceProcessorEnd()
  {
  }

  void ProgressInfo::onSequenceProcessorError(const std::string& sample_name, const std::string& processor_name, const std::string& error)
  {
    errors_.push_back(sample_name + ", " + processor_name + ": " + error);
  }

  void ProgressInfo::onSequenceSegmentProcessorStart(const size_t nb_segments)
  {
    running_batch_ = std::make_shared<RunningBatch>(nb_segments);
  }

  void ProgressInfo::onSequenceSegmentProcessorSampleStart(const std::string& segment_name)
  {
    running_batch_->running_items_.push_back(segment_name);
  }

  void ProgressInfo::onSequenceSegmentProcessorSampleEnd(const std::string& segment_name)
  {
    onRunningBatchEndItem(segment_name);
  }

  void ProgressInfo::onSequenceSegmentProcessorEnd()
  {
  }

  void ProgressInfo::onSequenceSegmentProcessorError(const std::string& segment_name, const std::string& processor_name, const std::string& error)
  {
    errors_.push_back(segment_name + ", " + processor_name + ": " + error);
  }

  void ProgressInfo::onSampleGroupProcessorStart(const size_t nb_groups)
  {
    running_batch_ = std::make_shared<RunningBatch>(nb_groups);
  }

  void ProgressInfo::onSampleGroupProcessorSampleStart(const std::string& group_name)
  {
    running_batch_->running_items_.push_back(group_name);
  }

  void ProgressInfo::onSampleGroupProcessorSampleEnd(const std::string& group_name)
  {
    onRunningBatchEndItem(group_name);
  }

  void ProgressInfo::onSampleGroupProcessorEnd()
  {
  }

  void ProgressInfo::onSampleGroupProcessorError(const std::string& group_name, const std::string& processor_name, const std::string& error)
  {
    errors_.push_back(group_name + ", " + processor_name + ": " + error);
  }

  void ProgressInfo::onRunningBatchEndItem(const std::string& item_name)
  {
    running_batch_->running_items_.erase(std::remove(running_batch_->running_items_.begin(),
      running_batch_->running_items_.end(),
      item_name),
      running_batch_->running_items_.end());
    running_batch_->current_step_++;
    running_batch_->last_item_end_time_point_ = std::chrono::steady_clock::now();
  }

  std::chrono::steady_clock::duration ProgressInfo::runningTime() const
  {
    if (isRunning())
    {
      return std::chrono::steady_clock::now() - application_processor_start_time_;
    }
    else
    {
      return std::chrono::steady_clock::duration::zero();
    }
  }

  std::optional<std::chrono::steady_clock::duration> ProgressInfo::estimatedRemainingTime() const
  {
    if (running_batch_  && running_batch_->current_step_ > 0)
    {
      auto time_per_steps = ((running_batch_->last_item_end_time_point_ - application_processor_start_time_) / running_batch_->current_step_);
      auto estimated_time = time_per_steps * running_batch_->max_steps_ - runningTime();
      // If we have other commands pending after this batch of commands, we will make a very rough estimation
      // (whatever type they are : sequence, sequence segment or sample groups)
      if (running_commands_.size() > 0)
      {
        auto time_per_command = (time_per_steps * running_batch_->max_steps_) / running_commands_.size();
        auto nb_remaining_commands = (all_commands_.size() - running_commands_.size() - commands_step_);
        estimated_time += time_per_command * nb_remaining_commands;
      }
      return estimated_time;
    }
    else
    {
      // cannot estimate time right now, not enough information
      return std::nullopt;
    }
  }

  float ProgressInfo::progressValue() const
  {
    float progress = 0.0f;
    if (all_commands_.size() > 0)
    {
      // executed commands so far
      progress = ((1.0 / all_commands_.size()) * commands_step_);
      if (running_batch_ && (running_batch_->max_steps_ > 0))
      {
        // actual batch
        progress += (((1.0 / all_commands_.size()) * running_commands_.size()) * ((1.0 / running_batch_->max_steps_) * running_batch_->current_step_));
      }
    }
    return progress;
  }

  const std::vector<std::tuple<size_t, std::string>>& ProgressInfo::runningCommands() const
  {
    return running_commands_;
  }

  const std::vector<std::string>& ProgressInfo::runningBatch() const
  {
    return running_batch_->running_items_;
  }

}
