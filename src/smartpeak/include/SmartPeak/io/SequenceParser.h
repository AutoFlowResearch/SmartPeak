// TODO: Add copyright

#pragma once

#include <SmartPeak/core/SequenceHandler.h>
#include <SmartPeak/core/SequenceSegmentHandler.h> // TODO: needed?
#include <SmartPeak/core/RawDataHandler.h> // TODO: needed?

namespace SmartPeak
{
  class SequenceParser
  {
public:
    SequenceParser() = delete;
    ~SequenceParser() = delete;
    static void readSequenceFile(SequenceHandler& sequenceHandler, const std::string& pathname);
    static void parseSequenceFile(SequenceHandler& sequenceHandler);

  };
}
