#pragma once

namespace SmartPeak 
{
  struct ISequenceObserver
  {
    /**
      Sequence has been updated
    */
    virtual void sequenceUpdated() = 0;
  };
}