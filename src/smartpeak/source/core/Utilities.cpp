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
      cast.setTagAndData(CastValue::UNKNOWN, value);
      throw; // TODO: Should this throw? If so, which exception?
    }
  }

  void Utilities::updateParameters(
    OpenMS::Param& Param_IO,
    const std::vector<std::map<std::string, std::string>>& parameters_I
  )
  {
    for (const std::map<std::string,std::string>& param : parameters_I) {
      const std::string& name = param.at("name");
      if (!Param_IO.exists(name)) {
        std::cout << "Utilities::updateParameters(): parameter \"" << name << "\"not found." << std::endl;
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
            c.setTagAndData(CastValue::UNKNOWN, Param_IO.getValue(name).toString());
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
      switch (c.getTag()) {
        case CastValue::BOOL:
          Param_IO.setValue(name, c.b_, description, tags);
          break;
        case CastValue::FLOAT:
          Param_IO.setValue(name, c.f_, description, tags);
          break;
        case CastValue::INT:
          Param_IO.setValue(name, c.i_, description, tags);
          break;
        case CastValue::STRING:
          Param_IO.setValue(name, c.s_, description, tags);
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

    try {
      if (std::regex_match(str_I.c_str(), m, re_integer_number)) {              // integer
        cast = static_cast<int>(std::strtol(m[0].str().c_str(), NULL, 10));
      } else if (std::regex_match(str_I.c_str(), m, re_float_number)) {         // float
        cast = std::strtof(m[0].str().c_str(), NULL);
      } else if (std::regex_match(str_I.c_str(), m, re_bool)) {                 // bool
        cast = m[0].str()[0] == 't' || m[0].str()[0] == 'T';
      } else if (str_I.front() == '[' && str_I.back() == ']') {                 // list
        std::string stripped;
        std::for_each(str_I.cbegin() + 1, str_I.cend() - 1, [&stripped](const char c) { // removing spaces to simplify regexs
          if (c != ' ')
            stripped.push_back(c);
        });
        const std::regex re_integer_list("[+-]?\\d+(?:,[+-]?\\d+)*");
        const std::regex re_float_list("[+-]?\\d+(?:\\.\\d+)?(?:,[+-]?\\d+(?:\\.\\d+)?)*");
        const std::regex re_bool_list("(?:true|false)(?:,(?:true|false))*", std::regex::icase);
        if (std::regex_match(stripped, re_integer_list)) {
          cast = std::vector<int>();
          parseList(stripped, re_integer_number, cast);
        } else if (std::regex_match(stripped, re_float_list)) {
          cast = std::vector<float>();
          parseList(stripped, re_float_number, cast);
        } else if (std::regex_match(stripped, re_bool_list)) {
          cast = std::vector<bool>();
          parseList(stripped, re_bool, cast);
        } else {
          cast = std::vector<std::string>();
          std::regex re_s("\"([^,]+)\"");
          parseList(str_I, re_s, cast);
        }
      } else if (str_I.front() == str_I.back() && (str_I.front() == '"' || str_I.front() == '\'')) { // string
        parseString(str_I.substr(1, str_I.size() - 2), cast);
      } else {                                                                                       // default to string
        cast = str_I;
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
      if (cast.getTag() == CastValue::BOOL_LIST) {
        cast.bl_.push_back(it->str()[0] == 't' || it->str()[0] == 'T');
      } else if (cast.getTag() == CastValue::FLOAT_LIST) {
        cast.fl_.push_back(std::strtof(it->str().c_str(), NULL));
      } else if (cast.getTag() == CastValue::INT_LIST) {
        cast.il_.push_back(std::strtol(it->str().c_str(), NULL, 10));
      } else if (cast.getTag() == CastValue::STRING_LIST) {
        cast.sl_.push_back((*it)[1].str());
      } else {
        throw std::invalid_argument("unexcepted tag type"); // TODO: specify which type of exception
      }
    }
  }
}
