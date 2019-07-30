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
    void addWorkflow(AppState& source_state);

    bool isWorkflowDone() const;

  private:
    /**
    * Spawn a new thread that runs the workflow, and wait for it to finish
    */
    static void run_and_join(AppState& state, bool& done, AppState& source_state);

    AppState state_;
    bool done_ = true;
  };
}