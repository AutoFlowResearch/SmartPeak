#include <SmartPeak/core/SequenceHandler.h>
#include <SmartPeak/io/SequenceParser.h>
#ifndef CSV_IO_NO_THREAD
#define CSV_IO_NO_THREAD
#endif
#include <SmartPeak/io/csv.h>
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
      RawDataHandler rdh;
      rdh.setMetaData(mdh);
      SampleHandler sampleHandler;
      sampleHandler.setRawData(rdh);
      sequence.push_back(sampleHandler);
    }

    sequenceHandler.setSequence(sequence);
  }

  void SequenceParser::parseSequenceFile(SequenceHandler& sequenceHandler)
  {
    for (const SampleHandler& sampleHandler : sequenceHandler.getSequence()) {
      const RawDataHandler& rdh = sampleHandler.getRawData();
      sequenceHandler.addSampleToSequence(rdh.getMetaData(), rdh.getFeatureMap());
    }
  }
}

// class SequenceWriter():
//     """A class to write SequenceHandlers"""

//     def makeDataTableFromMetaValue(
//         self,
//         sequenceHandler_I,
//         meta_data=['calculated_concentration'],
//         sample_types=['Unknown']
//     ):
//         """make a data matrix from a feature metaValue for 
//             specified sample types

//         Args:
//             sequenceHandler_I (SequenceHandler)
//             meta_data (list): 
//                 list of strings specifying the name of the feature metaValue
//             sample_types (list): list of strings corresponding to sample types

//         Returns:
//             list: rows: list of dicts
//             list: list of headers in the order to write to .csv
//         """
//         # assign the headers
//         header = ["sample_name", "sample_type", "component_group_name", "component_name"]
//         meta_data.sort()
//         header += meta_data

//         # collect the metaValues
//         list_dict = []
//         for d in sequenceHandler_I.sequence:
//             if d.meta_data['sample_type'] in sample_types:
//                 sample_name = d.meta_data['sample_name']
//                 for feature in d.raw_data.featureMap:
//                     component_group_name = feature.getMetaValue(
//                         "PeptideRef").decode('utf-8')
//                     for subordinate in feature.getSubordinates():
//                         row_dict = {}
//                         row_dict["sample_type"] = d.meta_data['sample_type']
//                         row_dict["sample_name"] = sample_name
//                         row_dict["component_group_name"] = component_group_name
//                         row_dict["component_name"] = subordinate.getMetaValue(
//                             'native_id').decode('utf-8')
//                         for meta_value in meta_data:
//                             datum = sequenceHandler_I.getMetaValue(
//                                 feature, subordinate, meta_value)
//                             if datum and datum is not None:                                
//                                 if isinstance(datum, bytes):
//                                     datum = datum.decode('utf-8')
//                                 row_dict[meta_value] = datum
//                             else:
//                                 row_dict[meta_value] = None                    
//                         list_dict.append(row_dict)
//         return list_dict, header

//     def write_dataTableFromMetaValue(
//         self,
//         sequenceHandler_I,
//         filename,
//         meta_data=['calculated_concentration'],
//         sample_types=['Unknown']
//     ):
//         """export data table from feature metaValue to .csv

//         Args:
//             sequenceHandler_I (SequenceHandler)
//             filename (string): name of the file
//             ...

//         """

//         # fixed header order
//         data_O, header = [], []
//         data_O, header = self.makeDataTableFromMetaValue(
//             sequenceHandler_I,
//             meta_data=meta_data, sample_types=sample_types)        

//         # write dict to csv
//         with open(filename, 'w', newline='') as f:
//             writer = csv.DictWriter(f, fieldnames=header)
//             try:
//                 writer.writeheader()
//                 writer.writerows(data_O)
//             except csv.Error as e:
//                 sys.exit(e)

//     def makeDataMatrixFromMetaValue(
//         self,
//         sequenceHandler_I,
//         meta_data=['calculated_concentration'],
//         sample_types=['Unknown']
//     ):
//         """make a data matrix from a feature metaValue for 
//             specified sample types

//         Args:
//             sequenceHandler_I (SequenceHandler)
//             meta_data (list): 
//                 list of strings specifying the name of the feature metaValue
//             sample_types (list): list of strings corresponding to sample types

//         Returns:
//             list: columns: list of sample_names
//             list: rows: 
//                 list of tuples corresponding to component_name and component_group_name
//             np.array: data: numpy data array of metaValues

//         """

//         columns = set()
//         rows = set()

//         # collect the metaValues
//         data_dict = {}
//         for d in sequenceHandler_I.sequence:
//             if d.meta_data['sample_type'] in sample_types:
//                 sample_name = d.meta_data['sample_name']
//                 data_dict[sample_name] = {}
//                 for meta_value in meta_data:
//                     for feature in d.raw_data.featureMap:
//                         component_group_name = feature.getMetaValue(
//                             "PeptideRef").decode('utf-8')
//                         for subordinate in feature.getSubordinates():
//                             if subordinate.getMetaValue("used_") is None \
//                             or subordinate.getMetaValue("used_").decode("utf-8") == "true":
//                                 row_tuple_name = (
//                                     component_group_name, 
//                                     subordinate.getMetaValue(
//                                         'native_id').decode('utf-8'), meta_value)
//                                 datum = sequenceHandler_I.getMetaValue(
//                                     feature, subordinate, meta_value)
//                                 if datum and datum is not None:                                
//                                     if isinstance(datum, bytes):
//                                         datum = datum.decode('utf-8')
//                                     data_dict[sample_name][row_tuple_name] = datum
//                                     columns.add(sample_name)
//                                     rows.add(row_tuple_name)
//         columns = list(columns)
//         columns.sort()
//         rows = list(rows)
//         rows.sort()

//         # make the data matrix
//         data = np.empty((len(rows), len(columns)))
//         data.fill(np.nan)
//         for i, r in enumerate(rows):
//             for j, c in enumerate(columns):
//                 if c in data_dict.keys() and r in data_dict[c].keys():
//                     data[i, j] = data_dict[c][r]

//         return columns, rows, data

//     def write_dataMatrixFromMetaValue(
//         self,
//         sequenceHandler_I,
//         filename,
//         meta_data=['calculated_concentration'],
//         sample_types=['Unknown']
//     ):
//         """export data matrix from feature metaValue to .csv

//         Args:
//             sequenceHandler_I (SequenceHandler)
//             filename (string): name of the file
//             meta_data (list): 
//                 list of strings specifying the name of the feature metaValue
//             sample_types (list): list of strings corresponding to sample types

//         """

//         data_O = []
//         columns, rows, data = self.makeDataMatrixFromMetaValue(
//             sequenceHandler_I,
//             meta_data=meta_data, sample_types=sample_types)
//         header = ['component_group_name', 'component_name', 'meta_value'] + columns
//         for i, r in enumerate(rows):
//             data_O.append(dict(zip(header, list(r) + list(data[i, :]))))
        
//         # write dict to csv
//         with open(filename, 'w', newline='') as f:
//             writer = csv.DictWriter(f, fieldnames=header)
//             try:
//                 writer.writeheader()
//                 writer.writerows(data_O)
//             except csv.Error as e:
//                 sys.exit(e)
