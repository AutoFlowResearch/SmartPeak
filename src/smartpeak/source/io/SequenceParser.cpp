// TODO: Add copyright

#include <SmartPeak/core/SequenceHandler.h>
#include <SmartPeak/io/SequenceParser.h>
#ifndef CSV_IO_NO_THREAD
#define CSV_IO_NO_THREAD
#endif
#include <SmartPeak/io/csv.h>
#include <fstream>

namespace SmartPeak
{
  void SequenceParser::readSequenceFile(SequenceHandler& sequenceHandler, const std::string& pathname)
  {
    const std::string s_sample_name {"sample_name"};
    const std::string s_sample_group_name {"sample_group_name"};
    const std::string s_sequence_segment_name {"sequence_segment_name"};
    const std::string s_sample_type {"sample_type"};
    const std::string s_filename {"filename"};

    io::CSVReader<
      5,
      io::trim_chars<' ', '\t'>,
      io::no_quote_escape<','>, // io::no_quote_escape<','>, // io::double_quote_escape<',', '\"'>,
      io::no_comment // io::single_line_comment<'#'>
    > in(pathname);

    in.read_header(
      io::ignore_extra_column, // io::ignore_extra_column | io::ignore_missing_column
      s_sample_name,
      s_sample_group_name,
      s_sequence_segment_name,
      s_sample_type,
      s_filename
    );

    std::string sample_name;
    std::string sample_group_name;
    std::string sequence_segment_name;
    std::string sample_type;
    std::string filename;

    std::vector<SampleHandler> sequence;

    while (in.read_row(sample_name, sample_group_name, sequence_segment_name, sample_type, filename)) {
      MetaDataHandler mdh;
      mdh.setSampleName(sample_name);
      mdh.setSampleGroupName(sample_group_name);
      mdh.setSequenceSegmentName(sequence_segment_name);
      mdh.setSampleType(MetaDataHandler::stringToSampleType(sample_type));
      mdh.setFilename(filename);
      sequenceHandler.addSampleToSequence(mdh, OpenMS::FeatureMap());
    }
  }

  void makeDataTableFromMetaValue(
    const SequenceHandler& sequenceHandler,
    std::vector<std::map<std::string,std::string>>& list_dict,
    std::vector<std::string>& headers_out,
    const std::vector<std::string>& meta_data_unsorted,
    const std::set<MetaDataHandler::SampleType>& sample_types // TODO: can overload with a vector of strings
  )
  {
    std::vector<std::string> meta_data = meta_data_unsorted;
    std::stable_sort(meta_data.begin(), meta_data.end());
    std::vector<std::string> headers = {"sample_name", "sample_type", "component_group_name", "component_name"}; // TODO: replace literals with variables
    headers.insert(headers.end(), meta_data.cbegin(), meta_data.cend());

    list_dict.clear();
    for (const SampleHandler& sampleHandler : sequenceHandler.getSequence()) {
      const MetaDataHandler& mdh = sampleHandler.getMetaData();
      const MetaDataHandler::SampleType st = mdh.getSampleType(); // TODO: can skip this?
      if (sample_types.count(st) == 0)
        continue;
      const std::string& sample_name = mdh.getSampleName();
      for (const OpenMS::Feature& feature : sampleHandler.getRawData().getFeatureMap()) {
        const std::string& component_group_name = feature.getMetaValue("PeptireRef").toString();
        for (const OpenMS::Feature& subordinate : feature.getSubordinates()) {
          std::map<std::string,std::string> row;
          row.emplace("sample_type", MetaDataHandler::SampleTypeToString(st));
          row.emplace("sample_name", sample_name);
          row.emplace("component_group_name", component_group_name);
          row.emplace("component_name", subordinate.getMetaValue("native_id").toString());
          for (const std::string& meta_value_name : meta_data) {
            const float datum = sequenceHandler.getMetaValue(feature, subordinate, meta_value_name);
            row.emplace(meta_value_name, std::to_string(datum)); // TODO: please compare this with code in SequenceWriter.py
          }
          list_dict.push_back(row);
        }
      }
    }
    headers_out = headers;
  }

  void write_dataTableFromMetaValue(
    const SequenceHandler& sequenceHandler,
    const std::string& filename,
    const std::vector<std::string>& meta_data_unsorted,
    const std::set<MetaDataHandler::SampleType>& sample_types
  )
  {
    std::vector<std::map<std::string,std::string>> list_dict;
    std::vector<std::string> headers;
    makeDataTableFromMetaValue(sequenceHandler, list_dict, headers, meta_data_unsorted, sample_types);
    std::ofstream f;
    f.open(filename);
    if (!f.is_open())
      throw "could not open file\n";
    std::string line;
    for (const std::string& s : headers) {
      line.append(s);
      line.push_back(',');
    }
    if (line.size()) {
      line.pop_back();
    } else {
      f.close();
      throw "headers is empty";
    }
    line.push_back('\n');
    f << line;
    for (const std::map<std::string,std::string>& m : list_dict) {
      line.clear();
      for (const std::string& h : headers) {
        line.append(m.at(h));
        line.push_back(',');
      }
      if (line.size()) {
        line.pop_back();
      } else {
        f.close();
        throw "line (map) is empty";
      }
      line.push_back('\n');
      f << line;
    }
    f.close();
  }

  void makeDataMatrixFromMetaValue(
    const SequenceHandler& sequenceHandler,
    std::vector<std::vector<float>>& data_out,
    std::set<std::string>& columns_out,
    std::set<std::string>& rows_out,
    const std::vector<std::string>& meta_data,
    const std::set<MetaDataHandler::SampleType>& sample_types
  )
  {
    std::set<std::string> columns;
    std::set<std::string> rows;
    std::map<std::string,std::map<std::string,float>> data_dict;
    for (const SampleHandler& sampleHandler : sequenceHandler.getSequence()) {
      const MetaDataHandler& mdh = sampleHandler.getMetaData();
      const MetaDataHandler::SampleType st = mdh.getSampleType(); // TODO: can skip this?
      if (sample_types.count(st) == 0)
        continue;
      const std::string& sample_name = mdh.getSampleName();
      data_dict.insert({sample_name, std::map<std::string,float>()});
      for (const std::string& meta_value_name : meta_data) {
        for (const OpenMS::Feature& feature : sampleHandler.getRawData().getFeatureMap()) {
          const std::string& component_group_name = feature.getMetaValue("PeptireRef").toString();
          for (const OpenMS::Feature& subordinate : feature.getSubordinates()) {
            if (!subordinate.metaValueExists("used_"))
              continue;
            const std::string used = subordinate.getMetaValue("used_").toString();
            if (used.empty())
              continue;
            if (used[0] == 'f' || used[0] == 'F')
              continue;
            const std::string row_tuple_name = component_group_name + "_" + subordinate.getMetaValue("native_id").toString() + "_" + meta_value_name;
            const float datum = sequenceHandler.getMetaValue(feature, subordinate, meta_value_name); // TODO: please compare this with code in SequenceWriter.py IT is assumed that datum is present and valid
            if (data_dict.count(sample_name))
              data_dict.emplace(sample_name, std::map<std::string,float>());
            data_dict[sample_name].emplace(row_tuple_name, datum);
            columns.insert(sample_name);
            rows.insert(row_tuple_name);
          }
        }
      }
    }
    size_t i, j;
    i = j = 0;
    std::vector<std::vector<float>> data(columns.size(), std::vector<float>(rows.size(), NAN));
    for (const std::string& row : rows) {
      for (const std::string& column : columns) {
        if (data_dict.count(column) && data_dict[column].count(row)) {
          data[i][j] = data_dict[column][row];
        }
        ++j;
      }
      ++i;
    }
    data_out = data;
    columns_out = columns;
    rows_out = rows;
  }

  void write_dataMatrixFromMetaValue(
    const SequenceHandler& sequenceHandler,
    const std::string& filename,
    const std::vector<std::string>& meta_data_unsorted,
    const std::set<MetaDataHandler::SampleType>& sample_types
  )
  {
    std::vector<std::vector<float>> data;
    std::set<std::string> columns;
    std::set<std::string> rows;
    makeDataMatrixFromMetaValue(sequenceHandler, data, columns, rows, meta_data_unsorted, sample_types);
    std::vector<std::string> headers = {"component_group_name", "component_name", "meta_value"};
    headers.insert(headers.end(), columns.begin(), columns.end());

    // std::cout << rows;

    // std::ofstream f;
    // f.open(filename);
    // if (!f.is_open())
    //   throw "could not open file\n";
    // std::string line;
    // for (const std::string& s : headers) {
    //   line.append(s);
    //   line.push_back(',');
    // }
    // if (line.size()) {
    //   line.pop_back();
    // } else {
    //   f.close();
    //   throw "headers is empty";
    // }
    // line.push_back('\n');
    // f << line;
    // size_t i = 0;
    // for (const std::string& row : rows) {
    //   line.clear();
    //   for (const std::string& h : headers) {
    //     line.append(data.at(h));
    //     line.push_back(',');
    //   }
    //   if (line.size()) {
    //     line.pop_back();
    //   } else {
    //     f.close();
    //     throw "line (map) is empty";
    //   }
    //   line.push_back('\n');
    //   f << line;
    // }
    // f.close();
  }
}
