#pragma once
#include <SmartPeak/iface/ISequenceObserver.h>
#include <memory>
#include <vector>
#include <algorithm>

namespace SmartPeak 
{
  class SequenceObservable
  {
  public:
    virtual void addSequenceObserver(ISequenceObserver* observer) { observers_.push_back(observer); };
    virtual void removeSequenceObserver(ISequenceObserver* observer) { observers_.erase(std::remove(observers_.begin(), observers_.end(), observer), observers_.end()); };
    void notifySequenceChanged()
    {
      for (auto& observer : observers_)
      {
        observer->sequenceUpdated();
      }
    }
  protected:
    std::vector<ISequenceObserver*> observers_;
  };
}
