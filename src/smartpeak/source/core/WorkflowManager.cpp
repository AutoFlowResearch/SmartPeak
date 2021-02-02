#include <SmartPeak/core/WorkflowManager.h>
#include <SmartPeak/core/ApplicationHandler.h>
#include <SmartPeak/core/ApplicationProcessor.h>
#include <thread>
#include <future>

namespace SmartPeak {
  void WorkflowManager::addWorkflow(ApplicationHandler& source_app_handler, const std::set<std::string>& injection_names, const std::set<std::string>& sequence_segment_names, const std::set<std::string>& sample_group_names)
  {
    // do not run workflows concurrently
    if (!done_) {
      return;
    }
    application_handler_ = source_app_handler;
    done_ = false;
    std::thread t(run_and_join, std::ref(application_handler_), std::ref(done_), std::ref(source_app_handler), std::ref(injection_names), std::ref(sequence_segment_names), std::ref(sample_group_names));
    LOGD << "Created thread (to be detached): " << t.get_id();
    t.detach();
    LOGD << "Thread has been detached";
  }

  bool WorkflowManager::isWorkflowDone() const
  {
    return done_;
  }

  void WorkflowManager::run_and_join(ApplicationHandler& application_handler, bool& done, ApplicationHandler& source_app_handler, const std::set<std::string>& injection_names, const std::set<std::string>& sequence_segment_names, const std::set<std::string>& sample_group_names)
  {
    // making local copies
    const std::set<std::string> injection_names_(injection_names);
    const std::set<std::string> sequence_segment_names_(sequence_segment_names);
    const std::set<std::string> sample_group_names_(sample_group_names);
    // create commands
    std::vector<ApplicationHandler::Command> commands;
    CreateCommand createCommand(application_handler);
    for (const auto& command_name : application_handler.sequenceHandler_.getWorkflow())
    {
      createCommand.name_ = command_name;
      const bool created = createCommand.process();
      if (created) {
        commands.push_back(createCommand.cmd_);
      }
    }
    // run workflow asynchronously
    std::future<void> f = std::async(
      std::launch::async,
      ApplicationProcessors::processCommands,
      std::ref(application_handler),
      commands,
      injection_names_, sequence_segment_names_, sample_group_names_
    );

    LOGD << "Waiting on async operation...";
    f.wait();
    LOGD << "Async operation completed";

    try {
      f.get(); // checking for exception
    } catch (const std::exception& e) {
      LOGE << e.what();
    }

    // update the status for this workflow, to be used in the gui
    source_app_handler = std::move(application_handler);
    done = true;
    LOGI << "State updated with workflow's results";
  }
}
