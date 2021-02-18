#pragma once
#include <SmartPeak/iface/IParametersObserver.h>
#include <memory>
#include <vector>

namespace SmartPeak
{
  class ParametersObservable
  {
  public:
    virtual void addParametersObserver(IParametersObserver* observer) { observers_.push_back(observer); };
    virtual void removeParametersObserver(IParametersObserver* observer) { observers_.erase(std::remove(observers_.begin(), observers_.end(), observer), observers_.end()); };
    void notifyParametersObservers()
    {
      for (auto& observer : observers_)
      {
        observer->parametersUpdated();
      }
    }
  protected:
    std::vector<IParametersObserver*> observers_;
  };
}
