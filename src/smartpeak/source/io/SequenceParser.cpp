// TODO: Add copyright

#include <SmartPeak/core/SequenceHandler.h>
#include <SmartPeak/io/SequenceParser.h>
#ifndef CSV_IO_NO_THREAD
#define CSV_IO_NO_THREAD
#endif
#include <SmartPeak/io/csv.h>
#include <SmartPeak/io/CSVWriter.h>

namespace SmartPeak
{
  void SequenceParser::readSequenceFile(
    SequenceHandler& sequenceHandler,
    const std::string& pathname,
    const std::string& delimiter,
    const bool verbose
  )
  {
    if (verbose) {
      std::cout << "==== START readSequenceFile()"
        << "\nreadSequenceFile(): loading " << pathname << std::endl;
    }

    const std::string s_sample_name {"sample_name"};
    const std::string s_sample_group_name {"sample_group_name"};
    const std::string s_sequence_segment_name {"sequence_segment_name"};
    const std::string s_sample_type {"sample_type"};
    const std::string s_filename {"filename"};
    const std::string s_comma {","};
    const std::string s_semicolon {";"};
    const std::string s_tab {"\t"};

    io::CSVReader<5, io::trim_chars<>, io::no_quote_escape<','>> in_comma(pathname);
    io::CSVReader<5, io::trim_chars<>, io::no_quote_escape<';'>> in_semicolon(pathname);
    io::CSVReader<5, io::trim_chars<>, io::no_quote_escape<'\t'>> in_tab(pathname);

    if (delimiter == s_comma) {
      in_comma.read_header(
        io::ignore_extra_column,
        s_sample_name,
        s_sample_group_name,
        s_sequence_segment_name,
        s_sample_type,
        s_filename
      );
    } else if (delimiter == s_semicolon) {
      in_semicolon.read_header(
        io::ignore_extra_column,
        s_sample_name,
        s_sample_group_name,
        s_sequence_segment_name,
        s_sample_type,
        s_filename
      );
    } else if (delimiter == s_tab) {
      in_tab.read_header(
        io::ignore_extra_column,
        s_sample_name,
        s_sample_group_name,
        s_sequence_segment_name,
        s_sample_type,
        s_filename
      );
    } else {
      throw std::invalid_argument("Delimiter \"" + delimiter + "\" is not supported.");
    }

    std::string sample_name;
    std::string sample_group_name;
    std::string sequence_segment_name;
    std::string sample_type;
    std::string filename;

    while (true) {
      bool is_valid = false;
      if (delimiter == s_comma)
        is_valid = in_comma.read_row(sample_name, sample_group_name, sequence_segment_name, sample_type, filename);
      else if (delimiter == s_semicolon)
        is_valid = in_semicolon.read_row(sample_name, sample_group_name, sequence_segment_name, sample_type, filename);
      else if (delimiter == s_tab)
        is_valid = in_tab.read_row(sample_name, sample_group_name, sequence_segment_name, sample_type, filename);
      if (!is_valid)
        break;
      MetaDataHandler mdh;
      mdh.setSampleName(sample_name);
      mdh.setSampleGroupName(sample_group_name);
      mdh.setSequenceSegmentName(sequence_segment_name);
      mdh.setSampleType(MetaDataHandler::stringToSampleType(sample_type));
      mdh.setFilename(filename);
      sequenceHandler.addSampleToSequence(mdh, OpenMS::FeatureMap());
    }

    if (verbose) {
      std::cout << "==== END   readSequenceFile()" << std::endl;
    }
  }

  void SequenceParser::makeDataTableFromMetaValue(
    const SequenceHandler& sequenceHandler,
    std::vector<std::map<std::string,std::string>>& list_dict,
    std::vector<std::string>& headers_out,
    const std::vector<std::string>& meta_data,
    const std::set<MetaDataHandler::SampleType>& sample_types // TODO: can overload with a vector of strings
  )
  {
    std::vector<std::string> headers = {"sample_name", "sample_type", "component_group_name", "component_name"};
    headers.insert(headers.end(), meta_data.cbegin(), meta_data.cend());
    for (size_t i = 0; i < headers.size() - 1; ++i) { // checking headers are unique, stable (maintaining the same positions)
      for (size_t j = i + 1; j < headers.size(); ) {
        if (headers[i] == headers[j])
          headers.erase(headers.begin() + j);
        else
          ++j;
      }
    }
    headers_out = headers;

    list_dict.clear();
    for (const SampleHandler& sampleHandler : sequenceHandler.getSequence()) {
      const MetaDataHandler& mdh = sampleHandler.getMetaData();
      const MetaDataHandler::SampleType st = mdh.getSampleType();
      if (sample_types.count(st) == 0)
        continue;
      const std::string& sample_name = mdh.getSampleName();
      const RawDataHandler& rawDataHandler = sampleHandler.getRawData();
      for (const OpenMS::Feature& feature : rawDataHandler.getFeatureMap()) {
      // for (const OpenMS::Feature& feature : sampleHandler.getRawData().getFeatureMap()) {
        if (!feature.metaValueExists(s_PeptideRef) || feature.getMetaValue(s_PeptideRef).isEmpty()) {
          // std::cout << "component_group_name is absent or empty. Skipping this feature." << std::endl; // Log it, instead
          continue;
        }
        const std::string component_group_name = feature.getMetaValue(s_PeptideRef);
        for (const OpenMS::Feature& subordinate : feature.getSubordinates()) {
          std::map<std::string,std::string> row;
          row.emplace("sample_type", MetaDataHandler::SampleTypeToString(st));
          row.emplace("sample_name", sample_name);
          row.emplace("component_group_name", component_group_name);
          if (!subordinate.metaValueExists(s_native_id) || subordinate.getMetaValue(s_native_id).isEmpty()) {
            std::cout << "component_name is absent or empty. Skipping this subordinate." << std::endl;
            continue;
          }
          const std::string component_name = subordinate.getMetaValue(s_native_id);
          row.emplace("component_name", component_name);
          for (const std::string& meta_value_name : meta_data) {
            Utilities::CastValue datum = SequenceHandler::getMetaValue(feature, subordinate, meta_value_name);
            if (datum.getTag() == Utilities::CastValue::FLOAT && datum.f_ != 0.0)
              // NOTE: to_string() rounds at 1e-6. Therefore, some precision might be lost.
              row.emplace(meta_value_name, std::to_string(datum.f_));
            else
              row.emplace(meta_value_name, "");
          }
          list_dict.push_back(row);
        }
      }
    }
  }

  void SequenceParser::writeDataTableFromMetaValue(
    const SequenceHandler& sequenceHandler,
    const std::string& filename,
    const std::vector<std::string>& meta_data,
    const std::set<MetaDataHandler::SampleType>& sample_types,
    const bool verbose
  )
  {
    if (verbose) {
      std::cout << "==== START writeDataTableFromMetaValue()"
        << "\nwriteDataTableFromMetaValue(): storing " << filename << std::endl;
    }

    std::vector<std::map<std::string,std::string>> list_dict;
    std::vector<std::string> headers;
    makeDataTableFromMetaValue(sequenceHandler, list_dict, headers, meta_data, sample_types);

    CSVWriter writer(filename, ",");
    writer.writeDataInRow(headers.cbegin(), headers.cend());

    for (const std::map<std::string,std::string>& m : list_dict) {
      std::vector<std::string> line;
      for (const std::string& h : headers) {
        line.push_back(m.at(h));
      }
      writer.writeDataInRow(line.cbegin(), line.cend());
    }

    if (verbose) {
      std::cout << "==== END   writeDataTableFromMetaValue()" << std::endl;
    }
  }

  void SequenceParser::makeDataMatrixFromMetaValue(
    const SequenceHandler& sequenceHandler,
    std::vector<std::vector<float>>& data_out,
    std::vector<std::string>& columns_out,
    std::vector<Row>& rows_out,
    const std::vector<std::string>& meta_data,
    const std::set<MetaDataHandler::SampleType>& sample_types
  )
  {
    std::set<std::string> columns;
    std::set<Row,Row_less> rows;
    std::map<std::string,std::map<Row,float,Row_less>> data_dict;

    for (const SampleHandler& sampleHandler : sequenceHandler.getSequence()) {
      const MetaDataHandler& mdh = sampleHandler.getMetaData();
      const MetaDataHandler::SampleType st = mdh.getSampleType();
      if (sample_types.count(st) == 0) {
        continue;
      }
      const std::string& sample_name = mdh.getSampleName();
      data_dict.insert({sample_name, std::map<Row,float,Row_less>()});
      for (const std::string& meta_value_name : meta_data) {
        const RawDataHandler& rawDataHandler = sampleHandler.getRawData();
        for (const OpenMS::Feature& feature : rawDataHandler.getFeatureMap()) {
        // for (const OpenMS::Feature& feature : sampleHandler.getRawData().getFeatureMap()) {
          const std::string& component_group_name = feature.getMetaValue(s_PeptideRef).toString();
          for (const OpenMS::Feature& subordinate : feature.getSubordinates()) {
            if (subordinate.metaValueExists("used_")) {
              const std::string used = subordinate.getMetaValue("used_").toString();
                if (used.empty() || used[0] == 'f' || used[0] == 'F')
                  continue;
            }
            const Row row_tuple_name(
              component_group_name,
              subordinate.getMetaValue(s_native_id).toString(),
              meta_value_name
            );
            Utilities::CastValue datum = SequenceHandler::getMetaValue(feature, subordinate, meta_value_name);
            if (datum.getTag() == Utilities::CastValue::FLOAT && datum.f_ != 0.0) {
              data_dict[sample_name].emplace(row_tuple_name, datum.f_);
              columns.insert(sample_name);
              rows.insert(row_tuple_name);
            }
          }
        }
      }
    }

    rows_out.clear();
    columns_out.clear();

    rows_out.insert(rows_out.cbegin(), rows.cbegin(), rows.cend());
    columns_out.insert(columns_out.cbegin(), columns.cbegin(), columns.cend());

    std::vector<std::vector<float>> data(rows_out.size(), std::vector<float>(columns_out.size(), NAN));

    for (size_t r = 0; r < rows_out.size(); ++r) {
      const Row& row = rows_out[r];
      for (size_t c = 0; c < columns_out.size(); ++c) {
        const std::string& column = columns_out[c];
        if (data_dict.count(column) && data_dict[column].count(row)) {
          data[r][c] = data_dict[column][row];
        }
      }
    }

    data_out = std::move(data);
  }

  void SequenceParser::writeDataMatrixFromMetaValue(
    const SequenceHandler& sequenceHandler,
    const std::string& filename,
    const std::vector<std::string>& meta_data,
    const std::set<MetaDataHandler::SampleType>& sample_types,
    const bool verbose
  )
  {
    if (verbose) {
      std::cout << "==== START writeDataMatrixFromMetaValue()"
        << "\nwriteDataMatrixFromMetaValue(): storing " << filename << std::endl;
    }

    std::vector<std::vector<float>> data;
    std::vector<std::string> columns;
    std::vector<Row> rows;
    makeDataMatrixFromMetaValue(sequenceHandler, data, columns, rows, meta_data, sample_types);

    std::vector<std::string> headers = {"component_group_name", "component_name", "meta_value"};
    headers.insert(headers.end(), columns.begin(), columns.end());

    CSVWriter writer(filename, ",");
    writer.writeDataInRow(headers.cbegin(), headers.cend());

    for (size_t i = 0; i < rows.size(); ++i) {
      std::vector<std::string> line;
      line.push_back(rows[i].component_group_name);
      line.push_back(rows[i].component_name);
      line.push_back(rows[i].meta_value_name);
      for (size_t j = 0; j < data.at(i).size(); ++j) {
        // NOTE: to_string() rounds at 1e-6. Therefore, some precision might be lost.
        line.emplace_back(std::to_string(data[i][j]));
      }
      writer.writeDataInRow(line.cbegin(), line.cend());
    }

    if (verbose) {
      std::cout << "==== END   writeDataMatrixFromMetaValue()" << std::endl;
    }
  }
}
