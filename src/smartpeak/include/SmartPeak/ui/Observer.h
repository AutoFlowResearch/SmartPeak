#pragma once

#include <SmartPeak/ui/AppManager.h>
#include <SmartPeak/ui/FilePicker.h>

namespace SmartPeak
{
  template<typename T>
  struct Observer {
    virtual void notification(
      T& source,
      const std::string& message
    ) = 0;
  };


  template<typename T>
  class Observable {
    std::vector<Observer<T>*> observers;
  public:

    void notify(
      T& source,
      const std::string& message
    )
    {
      for (auto observer : observers) {
        observer->notification(source, message);
      }
    }

    void subscribe(Observer<T>& observer)
    {
      observers.push_back(&observer);
    }

    void unsubscribe(Observer<T>& observer)
    {
      observers.erase(
        std::remove(observers.begin(), observers.end(), &observer),
        observers.end()
      );
    }
  };
}
