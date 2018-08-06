// TODO: Add copyright

#pragma once

namespace SmartPeak
{
  class FileReader
  {
public:
    FileReader() = delete;
    ~FileReader() = delete;

    static void parse_OpenMSParams(
        const std::string& filename,
        std::map<std::string,std::vector<std::map<std::string,std::string>>>& parameters
    );
  };
}
