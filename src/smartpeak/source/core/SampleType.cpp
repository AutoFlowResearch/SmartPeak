// --------------------------------------------------------------------------
//   SmartPeak -- Fast and Accurate CE-, GC- and LC-MS(/MS) Data Processing
// --------------------------------------------------------------------------
// Copyright The SmartPeak Team -- Novo Nordisk Foundation 
// Center for Biosustainability, Technical University of Denmark 2018-2021.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL ANY OF THE AUTHORS OR THE CONTRIBUTING
// INSTITUTIONS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
// OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
// WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
// OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
// ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// --------------------------------------------------------------------------
// $Maintainer: Douglas McCloskey $
// $Authors: Douglas McCloskey, Pasquale Domenico Colaianni $
// --------------------------------------------------------------------------

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
