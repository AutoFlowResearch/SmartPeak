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
// $Authors: Douglas McCloskey, Pasquale Domenico Colaianni $
// --------------------------------------------------------------------------

#include <SmartPeak/core/WorkflowManager.h>
#include <SmartPeak/core/ApplicationHandler.h>
#include <SmartPeak/core/ApplicationProcessor.h>
#include <thread>
#include <future>

namespace SmartPeak {

  void WorkflowManager::addWorkflow(ApplicationHandler& source_app_handler, 
    const std::set<std::string>& injection_names, 
    const std::set<std::string>& sequence_segment_names, 
    const std::set<std::string>& sample_group_names, 
    const std::vector<ApplicationHandler::Command>& commands, 
    IApplicationProcessorObserver* application_processor_observer,
    ISequenceProcessorObserver* sequence_processor_observer,
    ISequenceSegmentProcessorObserver* sequence_segment_processor_observer,
    ISampleGroupProcessorObserver* sample_group_processor_observer,
    bool blocking)
  {
    // do not run workflows concurrently
    if (!done_) {
      return;
    }
    application_handler_ = source_app_handler;
    done_ = false;

    // making local copies
    const std::set<std::string> injection_names_(injection_names);
    const std::set<std::string> sequence_segment_names_(sequence_segment_names);
    const std::set<std::string> sample_group_names_(sample_group_names);
    const std::vector<ApplicationHandler::Command> commands_(commands);

    if (!blocking)
    {
      std::thread t(run_and_join, 
                    std::ref(application_handler_),
                    std::ref(done_),
                    injection_names_,
                    sequence_segment_names_,
                    sample_group_names_,
                    commands_,
                    application_processor_observer,
                    sequence_processor_observer,
                    sequence_segment_processor_observer,
                    sample_group_processor_observer);
      LOGD << "Created thread (to be detached): " << t.get_id();
      t.detach();
      LOGD << "Thread has been detached";
    }
    else
    {
      run_and_join(application_handler_, done_, injection_names_,
        sequence_segment_names_, sample_group_names_, commands_, application_processor_observer,
        sequence_processor_observer, sequence_segment_processor_observer, sample_group_processor_observer);
    }
  }

  bool WorkflowManager::isWorkflowDone() const
  {
    return done_;
  }

  void WorkflowManager::run_and_join(ApplicationHandler& application_handler, 
    bool& done, 
    const std::set<std::string>& injection_names, 
    const std::set<std::string>& sequence_segment_names, 
    const std::set<std::string>& sample_group_names, 
    const std::vector<ApplicationHandler::Command>& commands,
    IApplicationProcessorObserver* application_processor_observer,
    ISequenceProcessorObserver* sequence_processor_observer,
    ISequenceSegmentProcessorObserver* sequence_segment_processor_observer,
    ISampleGroupProcessorObserver* sample_group_processor_observer)
  {
    // run workflow asynchronously
    std::future<void> f = std::async(
      std::launch::async,
      ApplicationProcessors::processCommands,
      std::ref(application_handler),
      std::cref(commands),
      std::cref(injection_names), 
      std::cref(sequence_segment_names), 
      std::cref(sample_group_names),
      application_processor_observer,
      sequence_processor_observer,
      sequence_segment_processor_observer,
      sample_group_processor_observer
    );

    LOGD << "Waiting on async operation...";
    f.wait();
    LOGD << "Async operation completed";

    try {
      f.get(); // checking for exception
    } catch (const std::exception& e) {
      LOGE << e.what();
    }

    done = true;
    application_handler.sequenceHandler_.notifySequenceUpdated();
    LOGI << "State updated with workflow's results";
  }

  void WorkflowManager::updateApplicationHandler(ApplicationHandler& source_app_handler)
  {
    // update the status for this workflow, to be used from the main thread (gui, client)
    source_app_handler = std::move(application_handler_);
  }

  std::set<std::string> WorkflowManager::getRequirements(
    const std::vector<ApplicationHandler::Command>& commands) const
  {
    std::set<std::string> requirements;
    for (const auto& command : commands)
    {
      const IProcessorDescription* processor_description = nullptr;
      switch (command.type)
      {
      case ApplicationHandler::Command::RawDataMethod:
        processor_description = command.raw_data_method.get();
        break;
      case ApplicationHandler::Command::SampleGroupMethod:
        processor_description = command.sample_group_method.get();
        break;
      case ApplicationHandler::Command::SequenceSegmentMethod:
        processor_description = command.seq_seg_method.get();
        break;
      default:
        break;
      }
      if (processor_description)
      {
        const auto processor_required = processor_description->getRequirements();
        for (const auto& req_file_id : processor_required)
        {
          requirements.insert(req_file_id);
        }
      }
    }
    return requirements;
  }

  bool WorkflowManager::isMissingRequirements(const Filenames& filenames, const std::set<std::string>& requirements) const
  {
    const auto file_ids = filenames.getFileIds();
    bool missing_requirement = false;
    for (const auto& req_file_id : requirements)
    {
      bool found = false;
      if (std::find(std::begin(file_ids), std::end(file_ids), req_file_id) != std::end(file_ids))
      {
        const auto full_path = filenames.getFullPath(req_file_id);
        found = (filenames.isEmbedded(req_file_id) ||
          (!full_path.empty() && std::filesystem::exists(full_path)));
      }
      if (!found)
      {
        missing_requirement = true;
        break;
      }
    }
    return missing_requirement;
  }
}
