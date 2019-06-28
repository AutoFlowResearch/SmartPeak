#include <SmartPeak/core/WorkflowManager.h>
#include <SmartPeak/core/AppState.h>
#include <SmartPeak/core/AppStateProcessor.h>
#include <mutex>
#include <thread>
#include <vector>

namespace SmartPeak {
  /**
  * Append the passed state in a vector and run a list of commands on it
  */
  void WorkflowManager::addWorkflow(AppState state)
  {
    std::lock_guard<std::mutex> g(mutex_);
    states_.emplace_back(std::move(state));
    done_.emplace_back(0); // to be regarded as "false"
    std::thread t(run_and_join, std::ref(states_.back()), std::ref(done_.back()));
    LOGD << "Created thread (to be detached): " << t.get_id();
    t.detach();
    LOGD << "Thread has been detached";
  }

  const std::vector<char>& WorkflowManager::getWorkflowsStatus() const
  {
    return done_;
  }

  void WorkflowManager::run_and_join(AppState& state, char& done)
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
  }
}