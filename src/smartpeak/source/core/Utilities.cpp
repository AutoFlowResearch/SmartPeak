// TODO: Add copyright

#include <SmartPeak/core/Utilities.h>
#include <regex>
#include <OpenMS/DATASTRUCTURES/Param.h>
#include <iostream>

namespace SmartPeak
{
  void Utilities::castString(
    const std::string& value,
    const std::string& type,
    CastValue& cast
  )
  {
    std::string lowercase_value;
    std::string lowercase_type;
    lowercase_value.resize(value.size());
    lowercase_type.resize(type.size());
    std::transform(value.begin(), value.end(), lowercase_value.begin(), ::tolower);
    std::transform(type.begin(), type.end(), lowercase_type.begin(), ::tolower);

    if (lowercase_type == "int") {
      cast = std::stoi(value);
    } else if (lowercase_type == "float") {
      cast = std::stof(value);
    } else if ((lowercase_type == "bool" || lowercase_type == "string") // TODO: Should we also support "string" type for an aventual `bool`?
               && (lowercase_value == "false" || lowercase_value == "true")) {
      cast = lowercase_value == "true";
    } else if (lowercase_type == "string") {
      cast = value;
    } else {
      std::cerr << type << " type not supported." << std::endl;
      cast = value;
      cast.tag = CastValue::UNKNOWN;
      throw; // TODO: Should this throw? If so, which exception?
    }
  }

  void Utilities::updateParameters(
    OpenMS::Param& Param_IO,
    const std::vector<std::map<std::string, std::string>>& parameters_I
  )
  {
    for (const std::map<std::string,std::string>& param : parameters_I) {
      const std::string& name {param.at("name")};
      if (!Param_IO.exists(name)) {
        std::cerr << "parameter not found: " << name << std::endl;
        continue;
      }
      // # check supplied user parameters
      CastValue c;
      if (param.count("value")) {
        if (param.count("type")) {
          castString(param.at("value"), param.at("type"), c);
        } else {
          parseString(param.at("value"), c);
        }
      } else {
        OpenMS::DataValue::DataType dt = Param_IO.getValue(name).valueType();
        switch (dt) {
          case OpenMS::DataValue::DOUBLE_VALUE:
            c = static_cast<float>(Param_IO.getValue(name));
            break;
          case OpenMS::DataValue::INT_VALUE:
            c = static_cast<int>(Param_IO.getValue(name));
            break;
          case OpenMS::DataValue::STRING_VALUE:
            {
              const std::string& value = Param_IO.getValue(name);
              std::string lowercase_value;
              std::transform(value.begin(), value.end(), lowercase_value.begin(), ::tolower);
              if (lowercase_value == "true" || lowercase_value == "false") {
                c = lowercase_value == "true";
              } else {
                c = Param_IO.getValue(name).toString();
              }
            }
            break;
          default:
            c = Param_IO.getValue(name).toString();
            c.tag = CastValue::UNKNOWN;
        }
      }

      std::string description;
      if (param.count("description")) {
        description = param.at("description");
      } else {
        description = Param_IO.getDescription(name);
      }

      OpenMS::StringList tags;
      if (param.count("tags")) {
        for (const OpenMS::String& s : param.at("tags"))
          tags.push_back(s);
      } else {
        for (const OpenMS::String& s : Param_IO.getTags(name))
          tags.push_back(s);
      }
      // # update the params
      switch (c.tag) {
        case CastValue::BOOL:
          Param_IO.setValue(name, c.b, description, tags);
          break;
        case CastValue::FLOAT:
          Param_IO.setValue(OpenMS::String(name), c.f, description, tags);
          break;
        case CastValue::INT:
          Param_IO.setValue(name, c.i, description, tags);
          break;
        case CastValue::STRING:
          Param_IO.setValue(name, c.s, description, tags);
          break;
      }
    }
  }

  void Utilities::parseString(const std::string& str_I, CastValue& cast)
  {
    std::cmatch m;
    std::regex re_integer_number("[+-]?\\d+");
    std::regex re_float_number("[+-]?\\d+(?:\\.\\d+)?"); // can match also integers: check for integer before checking for float
    std::regex re_bool("true|false", std::regex::icase);

    CastValue c;
    try {
      if (std::regex_match(str_I.c_str(), m, re_integer_number)) {              // integer
        c = static_cast<int>(std::strtol(m[0].str().c_str(), NULL, 10));
      } else if (std::regex_match(str_I.c_str(), m, re_float_number)) {         // float
        c = std::strtof(m[0].str().c_str(), NULL);
      } else if (std::regex_match(str_I.c_str(), m, re_bool)) {                 // bool
        c = m[0].str()[0] == 't' || m[0].str()[0] == 'T';
      } else if (str_I.front() == '[' && str_I.back() == ']') {                 // list
        std::string stripped;
        std::for_each(str_I.cbegin(), str_I.cend(), [&stripped](const char c) { // removing spaces to simplify regexs
          if (c != ' ')
            stripped.push_back(c);
        });
        const std::regex re_integer_list("\\[[+-]?\\d+(?:,[+-]?\\d+)*\\]");
        const std::regex re_float_list("\\[[+-]?\\d+\\.\\d+(?:,[+-]?\\d+\\.\\d+)*\\]");
        const std::regex re_bool_list("\\[(?:true|false)(?:,(?:true|false))*\\]", std::regex::icase);
        if (std::regex_match(str_I, re_integer_list)) {
          c.tag = CastValue::INT_LIST;
          parseList(stripped, re_integer_number, c);
        } else if (std::regex_match(str_I, re_float_list)) {
          c.tag = CastValue::FLOAT_LIST;
          parseList(stripped, re_float_number, c);
        } else if (std::regex_match(str_I, re_bool_list)) {
          c.tag = CastValue::BOOL_LIST;
          parseList(stripped, re_bool, c);
        } else {
          c.tag = CastValue::STRING_LIST;
          std::regex re_s("[^,\\[\\]\"]+");
          parseList(stripped, re_s, c);
        }
      } else if (str_I.front() == str_I.back() && (str_I.front() == '"' || str_I.front() == '\'')) {
        parseString(str_I.substr(1, str_I.size() - 2), cast);
      } else {
        c.tag = CastValue::STRING;
        c.s = str_I;
      }
    } catch (const std::exception& e) {
      std::cerr << e.what();
    }
  }

  void Utilities::parseList(const std::string& line, std::regex& re, CastValue& cast)
  {
    std::sregex_iterator matches_begin = std::sregex_iterator(line.begin(), line.end(), re);
    std::sregex_iterator matches_end = std::sregex_iterator();
    for (std::sregex_iterator it = matches_begin; it != matches_end; ++it) {
      if (cast.tag == CastValue::BOOL_LIST) {
        cast.bl.push_back(it->str()[0] == 't' || it->str()[0] == 'T');
      } else if (cast.tag == CastValue::FLOAT_LIST) {
        cast.fl.push_back(std::strtof(it->str().c_str(), NULL));
      } else if (cast.tag == CastValue::INT_LIST) {
        cast.il.push_back(std::strtol(it->str().c_str(), NULL, 10));
      } else if (cast.tag == CastValue::STRING_LIST) {
        cast.sl.push_back(it->str());
      } else {
        throw std::invalid_argument("unexcepted tag type"); // TODO: specify which type of exception
      }
    }
  }
}
