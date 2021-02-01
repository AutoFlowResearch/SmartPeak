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

#pragma once

#include <SmartPeak/core/Parameters.h>

#include <map>
#include <string>
#include <vector>

namespace SmartPeak
{
  class FileReader
  {
public:
    FileReader()                             = delete;
    ~FileReader()                            = delete;
    FileReader(const FileReader&)            = delete;
    FileReader& operator=(const FileReader&) = delete;
    FileReader(FileReader&&)                 = delete;
    FileReader& operator=(FileReader&&)      = delete;

    /**
      Parse parameters from csv file.

      @param[in] filename Input csv file
      @param[out] parameters Output of the form
        {
          {
            "ConvertTSVToTraML",
            {
              {
                {"name", "-in"},
                {"value", "IsolateA1.csv"}
              },
              {
                {"name", "-out"},
                {"value", "IsolateA1.traML"}
              }
            }
          },
          {
            "MRMMapper",
            {
              {
                {"name", "-in"},
                {"value", "IsolateA1.mzML"}
              },
              {
                {"name", "-tr"},
                {"value", "IsolateA1.traML"}
              },
              {
                {"name", "-out"},
                {"value", "IsolateA1_features.mzML"}
              },
              {
                {"name", "-precursor_tolerance"},
                {"value", "0.5"}
              },
              {
                {"name", "-product_tolerance"},
                {"value", "0.5"}
              },
              {
                {"name", "-no-strict"},
                {"value", ""}
              }
            }
          }
        }
    */
    static void parseOpenMSParams(
      const std::string& filename,
      ParameterSet& parameters
    );
  };
}
