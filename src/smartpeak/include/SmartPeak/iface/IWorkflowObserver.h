#pragma once
#include <memory>

namespace SmartPeak 
{
  struct IWorkflowObserver 
  {
    /**
      Workflow has been updated
    */
    virtual void workflowUpdated() = 0;
  };
}