#include <SmartPeak/core/SampleType.h>
#include <map>
#include <string>

namespace SmartPeak {
  const std::map<SampleType, std::string> sampleTypeToString {
    { SampleType::Unknown,      "Unknown"      },
    { SampleType::Standard,     "Standard"     },
    { SampleType::QC,           "QC"           },
    { SampleType::Blank,        "Blank"        },
    { SampleType::DoubleBlank,  "Double Blank" },
    { SampleType::Solvent,      "Solvent"      },
    { SampleType::Unrecognized, "Unrecognized" }
  };

  const std::map<std::string, SampleType> stringToSampleType {
    { "Unknown",      SampleType::Unknown      },
    { "Standard",     SampleType::Standard     },
    { "QC",           SampleType::QC           },
    { "Blank",        SampleType::Blank        },
    { "Double Blank", SampleType::DoubleBlank  },
    { "Solvent",      SampleType::Solvent      },
    { "Unrecognized", SampleType::Unrecognized }
  };
}
