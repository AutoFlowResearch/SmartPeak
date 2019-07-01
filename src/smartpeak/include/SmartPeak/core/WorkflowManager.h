#pragma once

#include <SmartPeak/core/AppState.h>
#include <mutex>
#include <vector>

namespace SmartPeak {
  class WorkflowManager {
  public:
    /**
    * Append the passed state in a vector and run a list of commands on it
    */
    void addWorkflow(AppState state);

    const std::vector<char>& getWorkflowsStatus() const;

  private:
    /**
    * Spawn a new thread that runs the workflow, and wait for it to finish
    */
    static void run_and_join(AppState& state, char& done);

    std::vector<AppState> states_;
    std::vector<char> done_;
    std::mutex mutex_;
  };
}