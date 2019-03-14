// TODO: Add copyright

#include <SmartPeak/core/SequenceHandler.h>
#include <SmartPeak/io/SequenceParser.h>
#ifndef CSV_IO_NO_THREAD
#define CSV_IO_NO_THREAD
#endif
#include <SmartPeak/io/csv.h>
#include <SmartPeak/io/CSVWriter.h>
#include <SmartPeak/io/InputDataValidation.h>
#include <ctime>

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

    if (pathname.empty()) {
      std::cout << "readSequenceFile(): pathname is empty\n";
      return;
    }

    if (!InputDataValidation::fileExists(pathname)) {
      std::cout << "readSequenceFile(): file not found\n";
      return;
    }

    const std::string s_sample_name {"sample_name"};
    const std::string s_sample_group_name {"sample_group_name"};
    const std::string s_sequence_segment_name {"sequence_segment_name"};
    const std::string s_sample_type {"sample_type"};
    const std::string s_original_filename {"original_filename"};
    const std::string s_proc_method_name {"proc_method_name"};
    const std::string s_rack_number {"rack_number"};
    const std::string s_plate_number {"plate_number"};
    const std::string s_pos_number {"pos_number"};
    const std::string s_inj_number {"inj_number"};
    const std::string s_dilution_factor {"dilution_factor"};
    const std::string s_acq_method_name {"acq_method_name"};
    const std::string s_operator_name {"operator_name"};
    const std::string s_acquisition_date_and_time {"acquisition_date_and_time"};
    const std::string s_inj_volume {"inj_volume"};
    const std::string s_inj_volume_units {"inj_volume_units"};
    const std::string s_batch_name {"batch_name"};

    const std::string s_comma {","};
    const std::string s_semicolon {";"};
    const std::string s_tab {"\t"};

    io::CSVReader<17, io::trim_chars<>, io::no_quote_escape<','>> in_comma(pathname);
    io::CSVReader<17, io::trim_chars<>, io::no_quote_escape<';'>> in_semicolon(pathname);
    io::CSVReader<17, io::trim_chars<>, io::no_quote_escape<'\t'>> in_tab(pathname);

    if (delimiter == s_comma) {
      in_comma.read_header(
        io::ignore_extra_column/* | io::ignore_missing_column*/,
        s_sample_name,
        s_sample_group_name,
        s_sequence_segment_name,
        s_sample_type,
        s_original_filename,
        s_proc_method_name,
        s_rack_number,
        s_plate_number,
        s_pos_number,
        s_inj_number,
        s_dilution_factor,
        s_acq_method_name,
        s_operator_name,
        s_acquisition_date_and_time,
        s_inj_volume,
        s_inj_volume_units,
        s_batch_name
      );
    } else if (delimiter == s_semicolon) {
      in_semicolon.read_header(
        io::ignore_extra_column/* | io::ignore_missing_column*/,
        s_sample_name,
        s_sample_group_name,
        s_sequence_segment_name,
        s_sample_type,
        s_original_filename,
        s_proc_method_name,
        s_rack_number,
        s_plate_number,
        s_pos_number,
        s_inj_number,
        s_dilution_factor,
        s_acq_method_name,
        s_operator_name,
        s_acquisition_date_and_time,
        s_inj_volume,
        s_inj_volume_units,
        s_batch_name
      );
    } else if (delimiter == s_tab) {
      in_tab.read_header(
        io::ignore_extra_column/* | io::ignore_missing_column*/,
        s_sample_name,
        s_sample_group_name,
        s_sequence_segment_name,
        s_sample_type,
        s_original_filename,
        s_proc_method_name,
        s_rack_number,
        s_plate_number,
        s_pos_number,
        s_inj_number,
        s_dilution_factor,
        s_acq_method_name,
        s_operator_name,
        s_acquisition_date_and_time,
        s_inj_volume,
        s_inj_volume_units,
        s_batch_name
      );
    } else {
      throw std::invalid_argument("Delimiter \"" + delimiter + "\" is not supported.");
    }

    MetaDataHandler t; // as in temporary
    std::string t_date;
    std::string t_sample_type;
    size_t row_number = 1;
    bool inj_numbers_equal_row_numbers = false;

    while (true) {
      bool is_valid = false;

      if (delimiter == s_comma) {
        is_valid = in_comma.read_row(t.sample_name, t.sample_group_name,
          t.sequence_segment_name, t_sample_type, t.original_filename,
          t.proc_method_name, t.rack_number, t.plate_number, t.pos_number,
          t.inj_number, t.dilution_factor, t.acq_method_name, t.operator_name,
          t_date, t.inj_volume, t.inj_volume_units, t.batch_name);
      } else if (delimiter == s_semicolon) {
        is_valid = in_semicolon.read_row(t.sample_name, t.sample_group_name,
          t.sequence_segment_name, t_sample_type, t.original_filename,
          t.proc_method_name, t.rack_number, t.plate_number, t.pos_number,
          t.inj_number, t.dilution_factor, t.acq_method_name, t.operator_name,
          t_date, t.inj_volume, t.inj_volume_units, t.batch_name);
      } else if (delimiter == s_tab) {
        is_valid = in_tab.read_row(t.sample_name, t.sample_group_name,
          t.sequence_segment_name, t_sample_type, t.original_filename,
          t.proc_method_name, t.rack_number, t.plate_number, t.pos_number,
          t.inj_number, t.dilution_factor, t.acq_method_name, t.operator_name,
          t_date, t.inj_volume, t.inj_volume_units, t.batch_name);
      }

      if (!is_valid)
        break;

      t.sample_type = MetaDataHandler::stringToSampleType(t_sample_type);
      std::tm& adt = t.acquisition_date_and_time;
      std::stringstream iss(t_date, std::ios_base::in);
      iss >> adt.tm_mday >> adt.tm_mon >> adt.tm_year >> adt.tm_hour >> adt.tm_min >> adt.tm_sec;

      if (t.inj_number <= 0) {
        t.inj_number = row_number;
        inj_numbers_equal_row_numbers = true;
      }

      sequenceHandler.addSampleToSequence(t, OpenMS::FeatureMap());

      t.clear();
      t_date.clear();
      t_sample_type.clear();
      ++row_number;
    }

    // std::cout << InputDataValidation::getSequenceInfo(sequenceHandler, delimiter);

    if (inj_numbers_equal_row_numbers) {
      std::cout << "One or more inj_number values were not found. These have been replaced with their row numbers.\n";
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
    std::vector<std::string> headers = {
      "sample_name", "sample_type", "component_group_name", "component_name", "batch_name",
      "rack_number", "plate_number", "pos_number", "inj_number", "dilution_factor", "inj_volume",
      "inj_volume_units", "operator_name", "acq_method_name", "proc_method_name",
      "original_filename", "acquisition_date_and_time", "injection_name", "used_"
    };
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

    const std::string delimiter {"_____"};

    list_dict.clear();
    for (const InjectionHandler& sampleHandler : sequenceHandler.getSequence()) {
      const MetaDataHandler& mdh = sampleHandler.getMetaData();
      const MetaDataHandler::SampleType st = mdh.getSampleType();
      if (sample_types.count(st) == 0)
        continue;
      const std::string& sample_name = mdh.getSampleName();

      // feature_map_history_ is needed in order to export all "used_" = true and false features
      for (const OpenMS::Feature& feature : sampleHandler.getRawData().getFeatureMapHistory()) {
        if (!feature.metaValueExists(s_PeptideRef) || feature.getMetaValue(s_PeptideRef).isEmpty()) {
          // std::cout << "component_group_name is absent or empty. Skipping this feature." << std::endl;
          // TODO: Log it, instead
          continue;
        }
        const std::string component_group_name = feature.getMetaValue(s_PeptideRef);
        for (const OpenMS::Feature& subordinate : feature.getSubordinates()) {
          std::map<std::string,std::string> row;
          row.emplace("sample_type", MetaDataHandler::SampleTypeToString(st));
          row.emplace("sample_name", sample_name);
          row.emplace("component_group_name", component_group_name);
          if (!subordinate.metaValueExists(s_native_id) ||
              subordinate.getMetaValue(s_native_id).isEmpty() ||
              subordinate.getMetaValue(s_native_id).toString().empty()) {
            std::cout << "component_name is absent or empty. Skipping this subordinate." << std::endl;
            continue;
          }
          const std::string component_name = subordinate.getMetaValue(s_native_id);
          row.emplace("component_name", component_name);
          row.emplace("proc_method_name", mdh.proc_method_name);
          row.emplace("rack_number", std::to_string(mdh.rack_number));
          row.emplace("plate_number", std::to_string(mdh.plate_number));
          row.emplace("pos_number", std::to_string(mdh.pos_number));
          row.emplace("inj_number", std::to_string(mdh.inj_number));
          row.emplace("dilution_factor", std::to_string(mdh.dilution_factor));
          row.emplace("acq_method_name", mdh.acq_method_name);
          row.emplace("operator_name", mdh.operator_name);
          row.emplace("original_filename", mdh.original_filename);
          row.emplace("acquisition_date_and_time", mdh.getAcquisitionDateAndTimeAsString());
          row.emplace("inj_volume", std::to_string(mdh.inj_volume));
          row.emplace("inj_volume_units", mdh.inj_volume_units);
          row.emplace("batch_name", mdh.batch_name);
          row.emplace("injection_name", mdh.getInjectionName());
          row.emplace(
            "used_",
            subordinate.metaValueExists("used_") ? subordinate.getMetaValue("used_").toString() : ""
          );
          for (const std::string& meta_value_name : meta_data) {
            if (subordinate.metaValueExists(meta_value_name) &&
                (meta_value_name == "QC_transition_message" ||
                 meta_value_name == "QC_transition_group_message")) {

              OpenMS::StringList messages = subordinate.getMetaValue(meta_value_name).toStringList();
              row.emplace(
                meta_value_name,
                Utilities::join(messages.begin(), messages.end(), delimiter)
              );
            } else {
              Utilities::CastValue datum = SequenceHandler::getMetaValue(feature, subordinate, meta_value_name);
              if (datum.getTag() == Utilities::CastValue::FLOAT && datum.f_ != 0.0) {
                // NOTE: to_string() rounds at 1e-6. Therefore, some precision might be lost.
                row.emplace(meta_value_name, std::to_string(datum.f_));
              } else {
                row.emplace(meta_value_name, "");
              }
            }
          }
          list_dict.push_back(row);
        }
      }
    }
  }

  bool SequenceParser::writeDataTableFromMetaValue(
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
    const int cnt = writer.writeDataInRow(headers.cbegin(), headers.cend());

    if (cnt < headers.size()) {
      return false;
    }

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

    return true;
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

    for (const InjectionHandler& sampleHandler : sequenceHandler.getSequence()) {
      const MetaDataHandler& mdh = sampleHandler.getMetaData();
      const MetaDataHandler::SampleType st = mdh.getSampleType();
      if (sample_types.count(st) == 0) {
        continue;
      }
      const std::string& sample_name = mdh.getSampleName();
      data_dict.insert({sample_name, std::map<Row,float,Row_less>()});
      for (const std::string& meta_value_name : meta_data) {

        // feature_map_history is not needed here as we are only interested in the current/"used" features
        for (const OpenMS::Feature& feature : sampleHandler.getRawData().getFeatureMap()) {
          const std::string component_group_name = feature.getMetaValue(s_PeptideRef).toString();
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

  bool SequenceParser::writeDataMatrixFromMetaValue(
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
    const int cnt = writer.writeDataInRow(headers.cbegin(), headers.cend());

    if (cnt < headers.size()) {
      return false;
    }

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

    return true;
  }
}
