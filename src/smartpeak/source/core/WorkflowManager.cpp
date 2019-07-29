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
    if (std::any_of(done_.begin(), done_.end(), [](bool b){ return b == false; })) {
      return;
    }
    states_.push_back(source_state);
    done_.emplace_back(0); // to be regarded as "false"
    std::thread t(run_and_join, std::ref(states_.back()), std::ref(done_.back()), std::ref(source_state));
    LOGD << "Created thread (to be detached): " << t.get_id();
    t.detach();
    LOGD << "Thread has been detached";
  }

  const std::vector<char>& WorkflowManager::getWorkflowsStatus() const
  {
    return done_;
  }

  void WorkflowManager::run_and_join(AppState& state, char& done, AppState& source_state)
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
    done = 1;
    source_state = std::move(state);
    LOGI << "State updated with workflow's results";
  }
}