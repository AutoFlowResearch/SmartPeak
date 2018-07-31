// TODO: Add copyright

#pragma once

#include <OpenMS/KERNEL/FeatureMap.h>

namespace SmartPeak
{
  class Utilities
  {
public:
    Utilities() = default;
    ~Utilities() = default;

    struct CastValue
    {
      enum{BOOL, FLOAT, INT, STRING, UNKNOWN} tag;
      union
      {
        bool b;
        float f;
        int i;
        std::string s;
      };
    };

    /**
      Cast a string to the desired type and return the evaluation

      The type check is case insensitive.

      @param[in] value input string
      @param[in] type desired type
      @param[out] cast result
    */
    static void castString(
      const std::string& value,
      const std::string& type,
      CastValue& cast
    )
    {
      std::string lowercase_value;
      std::transform(value.begin(), value.end(), lowercase_value.begin(), ::tolower);
      std::string lowercase_type;
      std::transform(type.begin(), type.end(), lowercase_type.begin(), ::tolower);

      if (lowercase_type == "int") {
        cast.tag = CastValue::INT;
        cast.i = std::stoi(value);
      } else if (lowercase_type == "float") {
        cast.tag = CastValue::FLOAT;
        cast.f = (float)std::stof(value);
      } else if ((lowercase_type == "bool" || lowercase_type == "string") && lowercase_value == "true") {
        cast.tag = CastValue::BOOL;
        cast.b = true;
      } else if ((lowercase_type == "bool" || lowercase_type == "string") && lowercase_value == "false") {
        cast.tag = CastValue::BOOL;
        cast.b = false;
      } else if (lowercase_type == "string") {
        cast.tag = CastValue::STRING;
        cast.s = value;
      } else {
        std::cout << type << " type not supported." << std::endl;
        cast.tag = CastValue::UNKNOWN;
        cast.s = value;
        throw; // TODO: specify exception
      }
    }

      // """Update a Param object
      // Args:
      //     Param_IO (pyopenms.Param): Param object to update
      //     parameters_I (list): list of parameters to update
          
      // Returns:
      //     pyopenms.Param: Param_IO: updated Param object
      
      // """
    static void updateParameters(
      OpenMS::Param& Param_IO,
      const std::vector<std::map<std::string, std::string>>& parameters_I
    )
    {
      for (const std::map<std::string,std::string>& param : parameters_I) {
        const std::string& name {param.at("name")};
        // # #test:
        // # if name == 'rt_extraction_window'.encode('utf-8'):
        // #     print('check')
        // # check if the param exists
        if (!Param_IO.exists(name)) {
          std::cout << "parameter not found: " << name << std::endl;
          continue;
        }
        // # check supplied user parameters
        CastValue c;
        if (param.count("value")) {
          if (param.count("type")
            self.castString(param["value"], param["type"], c);
          else:
            self.parseString(param['value'], c);
          // # if not self.checkParameterValue(value):
          // #     continue
        } else {
          OpenMS::DataValue::DataType dt = Param_IO.getValue(name).valueType();
          switch (dt) {
            case OpenMS::DataValue::DOUBLE_VALUE:
              c.tag = CastValue::FLOAT;
              c.f = static_cast<float>(Param_IO.getValue(name));
              break;
            case OpenMS::DataValue::INT_VALUE:
              c.tag = CastValue::INT;
              c.i = Param_IO.getValue(name);
              break;
            case OpenMS::DataValue::STRING_VALUE:
              const std::string& value = Param_IO.getValue(name);
              std::string lowercase_value;
              std::transform(value.begin(), value.end(), lowercase_value.begin(), ::tolower);
              if (lowercase_value == "true" || lowercase_value == "false") {
                c.tag = CastValue::BOOL;
                c.b = return lowercase_value == "true";
              } else {
                c.tag = CastValue::STRING;
                c.s = Param_IO.getValue(name);
              }
              break;
            default:
              c.tag = CastValue::UNKNOWN;
              c.s = Param_IO.getValue(name).toString();
          }
        }

        std::string description;
        if (param.count("description")) {
          description = param.at("description");
        } else {
          description = Param_IO.getDescription(name);
        }

        std::vector<std::string> tags;
        if (param.count("tags")) {
          tags = param.at("tags");
        } else {
          tags = Param_IO.getTags(name);
        }
        // # update the params
        Param_IO.setValue(name, value, description, tags);
      }
    }

    // """Parse string and return the eval
    
    // Args:
    //     str_I (str): input string
    //     encode_str_I (bool): encode string as utf-8?
        
    // Returns:
    //     str: str_O: evaluated string
        
    // """
    static void parseString(str_I, encode_str_I=True)
    {
      import csv
      from io import StringIO

      auto isFloat = [](const std::string& s) -> bool {
        try {
          std::stof(s);
          return true;
        } catch (const std::invalid_argument&) {
          return false;
        }
      };

      auto isBool = [](const std::string& s) -> bool { return s == "True" || s == "False"; };

      str_O = None
      try:
          if str_I.isdigit():
              str_O = int(str_I)
          elif str_I[0] == '-' and str_I[1:].isdigit():
              str_O = int(str_I)
          elif isfloat(str_I):
              str_O = float(str_I)
          // # elif str_I.isdecimal():
          // #     str_O = float(str_I)
          // # elif str_I[0]=='-' and str_I[1:].isdecimal():
          // #     str_O = float(str_I)
          elif isBool(str_I):
              str_O = eval(str_I)
          elif str_I[0] == '[' and str_I[-1] == ']':
              str_O = []
              f = StringIO(str_I[1:-1])
              reader = csv.reader(f, delimiter=',')
              for row in reader:
                  for item in row:
                      str_O.append(self.parseString(item, encode_str_I=encode_str_I))
          elif str_I[0] == '{' and str_I[-1] == '}':
              str_O = dict(str_I[1:-1])
          elif str_I[0] == '(' and str_I[-1] == ')':
              str_O = tuple(str_I[1:-1])
          elif str_I[0] in ["'", '"'] and str_I[-1] in ["'", '"']:
              str_O = eval(str_I)
              if encode_str_I:
                  str_O = str_I.encode('utf-8')
          else:
              str_O = str_I
              if encode_str_I:
                  str_O = str_I.encode('utf-8')
      except Exception as e:
          print(e)
      return str_O
    }

private:
  };
}
