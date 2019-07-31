#pragma once

#include <SmartPeak/core/AppState.h>

namespace SmartPeak {
  class WorkflowManager {
  public:
    /**
      Copies the passed state and sets up the async run of the workflow. Only one
      workflow is managed at a time

      @param[in,out] The state that gets copied and then updated at the end of the workflow run
    */
    void addWorkflow(AppState& source_state);

    /**
      If this returns false, new workflows can't run and the following menu items
      are disabled in the UI:
      - File -> Load session from sequence
      - Edit -> Workflow
      - Action -> Run command
      - Action -> Run workflow
    */
    bool isWorkflowDone() const;

  private:
    /**
      Spawns a thread that runs the workflow, and waits for it to finish. The
      modified state is copied back to the source, keeping the state of the app
      up-to-date

      @param[in,out] state Points to the class' state member
      @param[in,out] done Points to the class' done member
      @param[out] source_state The modified state is copied back here
    */
    static void run_and_join(AppState& state, bool& done, AppState& source_state);

    AppState state_; ///< The workflow is run on this copy
    bool done_ = true;
  };
}
