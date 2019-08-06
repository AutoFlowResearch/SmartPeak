#include <SmartPeak/core/WorkflowManager.h>
#include <SmartPeak/core/AppState.h>
#include <SmartPeak/core/AppStateProcessor.h>
#include <thread>
#include <future>

namespace SmartPeak {
  void WorkflowManager::addWorkflow(AppState& source_state)
  {
    // do not run workflows concurrently
    if (!done_) {
      return;
    }
    state_ = source_state;
    done_ = false;
    std::thread t(run_and_join, std::ref(state_), std::ref(done_), std::ref(source_state));
    LOGD << "Created thread (to be detached): " << t.get_id();
    t.detach();
    LOGD << "Thread has been detached";
  }

  bool WorkflowManager::isWorkflowDone() const
  {
    return done_;
  }

  void WorkflowManager::run_and_join(AppState& state, bool& done, AppState& source_state)
  {
    // run workflow asynchronously
    std::future<void> f = std::async(
      std::launch::async,
      AppStateProcessors::processCommands,
      std::ref(state),
      state.commands_
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
    source_state = std::move(state);
    done = true;
    LOGI << "State updated with workflow's results";
  }
}
