#pragma once

#include <SmartPeak/core/ApplicationHandler.h>

namespace SmartPeak {
  class WorkflowManager {
  public:
    /**
      Copies the passed application_handler and sets up the async run of the workflow. Only one
      workflow is managed at a time

      @param[in,out] The application_handler that gets copied and then updated at the end of the workflow run
      @param[in] injection_names Injection names to use for Sequence Processing
      @param[in] sequence_segment_names Sequence Segment Names to use for Sequence Segment Processing
      @param[in] sample_group_names Sample Group Names to use for Sample Group Processing
    */
    void addWorkflow(ApplicationHandler& source_state, const std::set<std::string>& injection_names, const std::set<std::string>& sequence_segment_names, const std::set<std::string>& sample_group_names);

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
      modified application_handler is copied back to the source, keeping the application_handler of the app
      up-to-date

      @param[in,out] application_handler Points to the class' application_handler member
      @param[in,out] done Points to the class' done member
      @param[out] source_app_handler The modified application_handler is copied back here
      @param[in] injection_names Injection names to use for Sequence Processing
      @param[in] sequence_segment_names Sequence Segment Names to use for Sequence Segment Processing
      @param[in] sample_group_names Sample Group Names to use for Sample Group Processing
    */
    static void run_and_join(ApplicationHandler& application_handler, bool& done, ApplicationHandler& source_app_handler, const std::set<std::string>& injection_names, const std::set<std::string>& sequence_segment_names, const std::set<std::string>& sample_group_names);

    ApplicationHandler application_handler_; ///< The workflow is run on this copy
    bool done_ = true;
  };
}
