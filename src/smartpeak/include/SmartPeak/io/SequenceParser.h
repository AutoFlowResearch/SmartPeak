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

    static constexpr char const * const s_PeptideRef {"PeptideRef"};
    static constexpr char const * const s_native_id {"native_id"};

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

    struct Row
    {
      Row() = default;
      Row(const std::string& cgn, const std::string& cn, const std::string& mvn) :
        component_group_name(cgn),
        component_name(cn),
        meta_value_name(mvn) {}
      std::string component_group_name;
      std::string component_name;
      std::string meta_value_name;
    };

    struct Row_less
    {
      bool operator()(const Row& lhs, const Row& rhs) const
      {
        return lhs.component_group_name + lhs.component_name + lhs.meta_value_name <
          rhs.component_group_name + rhs.component_name + rhs.meta_value_name;
      }
    };

    static void makeDataMatrixFromMetaValue(
      const SequenceHandler& sequenceHandler,
      std::vector<std::vector<float>>& data_out,
      std::vector<std::string>& columns_out,
      std::vector<Row>& rows_out,
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
