#pragma once
#include <memory>

namespace SmartPeak 
{
  struct IParametersObserver
  {
    /**
      Parameters have been updated
    */
    virtual void parametersUpdated() = 0;
  };
}