#pragma once

#include <map>
#include <string>

namespace SmartPeak {
  enum class SampleType {
    Unknown = 1,
    Standard,
    QC,
    Blank,
    DoubleBlank,
    Solvent,
    Unrecognized
  };

  extern const std::map<SampleType, std::string> sampleTypeToString;

  extern const std::map<std::string, SampleType> stringToSampleType;

  constexpr size_t SampleTypeSize { 7 };
}
