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

    static void parseOpenMSParams(
        const std::string& filename,
        std::map<std::string,std::vector<std::map<std::string,std::string>>>& parameters
    );
  };
}
