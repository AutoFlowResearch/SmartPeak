#pragma once

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