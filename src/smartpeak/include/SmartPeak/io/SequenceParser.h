// TODO: Add copyright

#pragma once

#include <SmartPeak/core/SequenceHandler.h>

namespace SmartPeak
{
  class SequenceParser
  {
public:
    SequenceParser() = delete;
    ~SequenceParser() = delete;

    static void readSequenceFile(SequenceHandler& sequenceHandler, const std::string& pathname, const std::string& delimiter);

    // TODO: the following 4 methods are incomplete and untested

    static void makeDataTableFromMetaValue(
      const SequenceHandler& sequenceHandler,
      std::vector<std::map<std::string,std::string>>& list_dict,
      std::vector<std::string>& headers_out,
      const std::set<std::string>& meta_data = std::set<std::string>({"calculated_concentration"}),
      const std::set<MetaDataHandler::SampleType>& sample_types = std::set<MetaDataHandler::SampleType>({MetaDataHandler::SampleType::Unknown})
    );

    static void write_dataTableFromMetaValue(
      const SequenceHandler& sequenceHandler,
      const std::string& filename,
      const std::set<std::string>& meta_data = std::set<std::string>({"calculated_concentration"}),
      const std::set<MetaDataHandler::SampleType>& sample_types = std::set<MetaDataHandler::SampleType>({MetaDataHandler::SampleType::Unknown})
    );

    static void makeDataMatrixFromMetaValue(
      const SequenceHandler& sequenceHandler,
      std::vector<std::vector<float>>& data_out,
      std::set<std::string>& columns_out,
      std::set<std::string>& rows_out,
      const std::set<std::string>& meta_data = std::set<std::string>({"calculated_concentration"}),
      const std::set<MetaDataHandler::SampleType>& sample_types = std::set<MetaDataHandler::SampleType>({MetaDataHandler::SampleType::Unknown})
    );

    static void write_dataMatrixFromMetaValue(
      const SequenceHandler& sequenceHandler,
      const std::string& filename,
      const std::set<std::string>& meta_data = std::set<std::string>({"calculated_concentration"}),
      const std::set<MetaDataHandler::SampleType>& sample_types = std::set<MetaDataHandler::SampleType>({MetaDataHandler::SampleType::Unknown})
    );
  };
}
