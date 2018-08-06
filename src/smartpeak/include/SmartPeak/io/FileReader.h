// TODO: Add copyright

#pragma once

#include <SmartPeak/core/SequenceSegmentHandler.h>

namespace SmartPeak
{
  class FileReader
  {
public:
    FileReader() = default;
    ~FileReader() = default;

    def read_openMSParams(self, filename, delimiter):
        """read table data from csv file representing
        representing the method arguments to run the
        openMS methods

        Args:
            filename (str): header should include the following:
                function,
                name,
                value,
                type (optional),
                tag (optional),
                description (optional)
                used_,
                comment_ (optional)

        """
        self.read_csv(filename, delimiter)
        self.parse_openMSParams()

    void parse_openMSParams(
        const std::string& filename,
        std::map<std::string,std::vector<std::map<std::string,std::string>>>& parameters
    )
    {
      """parse parameters from csv file

      Args:
          data_I (list): e.g. [
          {'function': 'ConvertTSVToTraML', 'name': '-in', 'value': 'IsolateA1.csv'},
          {'function': 'ConvertTSVToTraML', 'name': '-out', 'value': 'IsolateA1.traML'},
          {'function': 'MRMMapper', 'name': '-in', 'value': 'IsolateA1.mzML'},
          {'function': 'MRMMapper', 'name': '-tr', 'value': 'IsolateA1.traML'},
          {'function': 'MRMMapper', 'name': '-out', 'value': 'IsolateA1.csv'},
          {'function': 'MRMMapper', 'name': '-precursor_tolerance','value':0.5},
          {'function': 'MRMMapper', 'name': '-product_tolerance','value':0.5},
          {'function': 'MRMMapper', 'name': '-no-strict','value':''},
          ]

      Returns:
          dict: data_O: e.g. {
              'ConvertTSVToTraML':[
                  {'name':'-in','value':'IsolateA1.csv'},
                  {'name':'-out','value':'IsolateA1.traML'}
              ]},
              {'MRMMapper':[
                  {'name':'-in','value':'IsolateA1.mzML'},
                  {'name':'-tr','value':'IsolateA1.traML'},
                  {'name':'-out','value':'IsolateA1_features.mzML'},
                  {'name':'-precursor_tolerance','value':0.5},
                  {'name':'-product_tolerance','value':0.5},
                  {'name':'-no-strict','value':''}
              ]}
      """

      parameters.clear();
      io::CSVReader<7> in(filename);
      in.read_header(
        io::ignore_extra_column,
        "function",
        "name",
        "value",
        "tag",
        "description",
        "type",
        "used_"
      );
      bool has_tags = in.has_column("tags");
      if (!in.has_column("used_"))
        return;
      if (!in.has_column("tags"))
        return;
      std::string func;
      std::string name;
      std::string value;
      std::string tag;
      std::string descr;
      std::string type;
      std::string used;
      while (in.read_row(func, name, value, tag, descr, type, used)) {
        std::transform(used.begin(), used.end(), used.begin(), ::tolower);
        if (used == "false")
          continue;
        std::map<std::string,std::string> m;
        m.emplace("name", name);
        m.emplace("value", value);
        if (in.has_column()
      }

      data_I = self.getData()
      data_O = {}
      function_current = ''
      function_params = {}
      for i, d in enumerate(data_I):
          // # skip non-used lines
          if not d['used_'] or d['used_'] == "FALSE":
              continue
          // # update function_current
          function_current = d['function']
          if function_current not in data_O.keys():
              data_O[function_current] = []
          // # make the function parameter line
          function_param = {}
          function_param['name'] = d['name']
          function_param['value'] = d['value']
          if 'tags' in d.keys():
              function_param['tags'] = d['tags']
          if 'description' in d.keys():
              function_param['description'] = d['description']
          if 'type' in d.keys():
              function_param['type'] = d['type']
          data_O[function_current].append(function_param)
      self.setData(data_O)
    }
private:
  };
}
