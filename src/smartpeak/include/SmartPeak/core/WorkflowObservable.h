#pragma once
#include <SmartPeak/iface/IWorkflowObserver.h>
#include <memory>
#include <vector>
#include <algorithm>

namespace SmartPeak 
{
  class WorkflowObservable
  {
  public:
    virtual void addWorkflowObserver(IWorkflowObserver* observer) { observers_.push_back(observer); };
    virtual void removeWorkflowObserver(IWorkflowObserver* observer) { observers_.erase(std::remove(observers_.begin(), observers_.end(), observer), observers_.end()); };
    void notifyWorkflowChanged()
    {
      for (auto& observer : observers_)
      {
        observer->workflowUpdated();
      }
    }
  protected:
    std::vector<IWorkflowObserver*> observers_;
  };
}
