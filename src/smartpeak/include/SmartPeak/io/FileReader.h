// TODO: Add copyright

#pragma once

#include <map>
#include <string>
#include <vector>

namespace SmartPeak
{
  class FileReader
  {
public:
    FileReader() = delete;
    ~FileReader() = delete;

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
        std::map<std::string,std::vector<std::map<std::string,std::string>>>& parameters
    );
  };
}
