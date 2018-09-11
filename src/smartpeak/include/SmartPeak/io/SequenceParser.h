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

    static void makeDataTableFromMetaValue(
      const SequenceHandler& sequenceHandler,
      const std::vector<std::string>& meta_data_unsorted = std::vector<std::string>({"calculated_concentration"}),
      const std::set<MetaDataHandler::SampleType>& sample_types = std::set({MetaDataHandler::SampleType::Unknown}),
      std::vector<std::map<std::string,std::string>>& list_dict,
      std::vector<std::string>& headers_out
    );

    static void write_dataTableFromMetaValue(
      const SequenceHandler& sequenceHandler,
      const std::string& filename,
      const std::vector<std::string>& meta_data_unsorted = std::vector<std::string>({"calculated_concentration"}),
      const std::set<MetaDataHandler::SampleType>& sample_types = std::set({MetaDataHandler::SampleType::Unknown})
    );

    static void makeDataTableFromMetaValue(
      const SequenceHandler& sequenceHandler,
      const std::vector<std::string>& meta_data_unsorted = std::vector<std::string>({"calculated_concentration"}),
      const std::set<MetaDataHandler::SampleType>& sample_types = std::set({MetaDataHandler::SampleType::Unknown}),
      std::vector<std::vector<float>>& data_out,
      std::set& columns_out,
      std::set& rows_out
    );
  };
}
