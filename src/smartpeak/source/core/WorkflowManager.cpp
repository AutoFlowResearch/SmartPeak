#include <SmartPeak/core/WorkflowManager.h>
#include <SmartPeak/core/AppState.h>
#include <SmartPeak/core/AppStateProcessor.h>
#include <algorithm>
#include <mutex>
#include <thread>
#include <vector>

namespace SmartPeak {
  /**
  * Append the passed state in a vector and run a list of commands on it
  */
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
    // run the workflow on a separate thread t
    std::thread t(
      AppStateProcessors::processCommands,
      std::ref(state),
      state.commands_
    );
    // join the workflow's associated thread object t
    LOGD << "Joining thread: " << t.get_id();
    t.join();
    LOGD << "Thread has been joined";
    // update the status for this workflow, to be used in the gui
    source_state = std::move(state);
    done = true;
    LOGI << "State updated with workflow's results";
  }
}