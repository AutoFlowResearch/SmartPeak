#pragma once
#include <memory>

namespace SmartPeak 
{
  struct IProcessorDescription 
  {
    /**
      Get the processor struct ID
    */
    virtual int getID() const = 0;

    /**
      Get the processor struct name
    */
    virtual std::string getName() const = 0;

    /**
      Get the processor struct description
    */
    virtual std::string getDescription() const = 0;

    virtual ~IProcessorDescription() = default;
  };
}