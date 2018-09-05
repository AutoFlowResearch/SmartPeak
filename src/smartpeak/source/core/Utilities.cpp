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
        std::cout << "Utilities::updateParameters(): parameter \"" << name << "\" not found." << std::endl;
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
              const std::string& value = Param_IO.getValue(name).toString();
              std::string lowercase_value;
              std::transform(value.begin(), value.end(), lowercase_value.begin(), ::tolower);
              if (lowercase_value == "true" || lowercase_value == "false") {
                c = lowercase_value == "true";
              } else {
                c = value;
              }
              break;
            }
          case OpenMS::DataValue::DOUBLE_LIST:
            c = std::vector<float>();
            for (const double n : Param_IO.getValue(name).toDoubleList()) {
              c.fl_.push_back(n);
            }
            break;
          case OpenMS::DataValue::INT_LIST:
            c = std::vector<int>();
            for (const int n : Param_IO.getValue(name).toIntList()) {
              c.il_.push_back(n);
            }
            break;
          case OpenMS::DataValue::STRING_LIST:
            {
              bool strings_are_bools = false;
              if (Param_IO.getValue(name).toStringList().size()) {
                const std::string& value = Param_IO.getValue(name).toStringList().front();
                std::string lowercase_value;
                std::transform(value.begin(), value.end(), lowercase_value.begin(), ::tolower);
                if (lowercase_value == "true" || lowercase_value == "false") {
                  c = std::vector<bool>();
                  strings_are_bools = true;
                }
              } else {
                c = std::vector<std::string>();
              }

              for (const std::string& s : Param_IO.getValue(name).toStringList()) {
                if (strings_are_bools) {
                  std::string lowercase_value;
                  std::transform(s.begin(), s.end(), lowercase_value.begin(), ::tolower);
                  c.bl_.push_back(s == "true");
                } else {
                  c.sl_.push_back(s);
                }
              }

              break;
            }
          default:
            c.setTagAndData(CastValue::UNKNOWN, Param_IO.getValue(name).toString());
        }
      }

      std::string description;
      if (param.count("description")) {
        description = param.at("description");
      } else {
        try {
          description = Param_IO.getDescription(name);
        } catch (const OpenMS::Exception::ElementNotFound&) {
          // leave description as an empty string
        }
      }

      OpenMS::StringList tags;
      if (param.count("tags")) {
        const std::string& line = param.at("tags");
        std::regex re_tag("[^'\",]+");
        std::sregex_iterator matches_begin = std::sregex_iterator(line.begin(), line.end(), re_tag);
        std::sregex_iterator matches_end = std::sregex_iterator();
        for (std::sregex_iterator it = matches_begin; it != matches_end; ++it) {
          tags.push_back(it->str());
        }
      } else {
        tags = Param_IO.getTags(name);
      }
      // # update the params
      switch (c.getTag()) {
        case CastValue::BOOL:
          Param_IO.setValue(name, c.b_ ? "true" : "false", description, tags);
          break;
        case CastValue::FLOAT:
          Param_IO.setValue(name, static_cast<double>(c.f_), description, tags);
          break;
        case CastValue::INT:
          Param_IO.setValue(name, c.i_, description, tags);
          break;
        case CastValue::STRING:
          Param_IO.setValue(name, c.s_, description, tags);
          break;
        case CastValue::FLOAT_LIST:
          {
            std::vector<double> dl;
            for (const float f : c.fl_) {
              dl.push_back(f);
            }
            Param_IO.setValue(name, dl, description, tags);
            break;
          }
        case CastValue::INT_LIST:
          {
            std::vector<int> il;
            for (const int i : c.il_) {
              il.push_back(i);
            }
            Param_IO.setValue(name, il, description, tags);
            break;
          }
        case CastValue::STRING_LIST:
          {
            OpenMS::StringList sl;
            for (const std::string& s : c.sl_) {
              sl.push_back(s);
            }
            Param_IO.setValue(name, sl, description, tags);
            break;
          }
        case CastValue::BOOL_LIST:
          {
            OpenMS::StringList sl;
            for (const bool b : c.bl_) {
              sl.push_back(b ? "true" : "false");
            }
            Param_IO.setValue(name, sl, description, tags);
            break;
          }
        case CastValue::UNKNOWN:
          Param_IO.setValue(name, c.s_, description, tags);
          break;
        default:
          throw "case not handled in updateParameters()";
      }
    }
  }

  void Utilities::parseString(const std::string& str_I, CastValue& cast)
  {
    std::regex re_integer_number("[+-]?\\d+");
    std::regex re_float_number("[+-]?\\d+(?:\\.\\d+)?"); // can match also integers: check for integer before checking for float
    std::regex re_bool("true|false", std::regex::icase);
    std::regex re_s("\"([^,]+)\"");
    std::smatch m;

    const std::string not_of_set = " ";
    size_t str_I_start = str_I.find_first_not_of(not_of_set);
    size_t str_I_end = str_I.find_last_not_of(not_of_set);
    std::string trimmed;
    if (str_I_start != std::string::npos && str_I_start <= str_I_end) {
      trimmed = str_I.substr(str_I_start, str_I_end - str_I_start + 1);
    }

    try {
      if (std::regex_match(trimmed, m, re_integer_number)) { // integer
        cast = static_cast<int>(std::strtol(m[0].str().c_str(), NULL, 10));
      } else if (std::regex_match(trimmed, m, re_float_number)) { // float
        cast = std::strtof(m[0].str().c_str(), NULL);
      } else if (std::regex_match(trimmed, m, re_bool)) { // bool
        cast = m[0].str()[0] == 't' || m[0].str()[0] == 'T';
      } else if (trimmed.front() == '[' && trimmed.back() == ']') { // list
        std::string stripped;
        std::for_each(trimmed.cbegin() + 1, trimmed.cend() - 1, [&stripped](const char c) { // removing spaces to simplify regexs
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
          parseList(trimmed, re_s, cast);
        }
      } else if (trimmed.front() == trimmed.back() && (trimmed.front() == '"' || trimmed.front() == '\'')) { // string
        parseString(trimmed.substr(1, trimmed.size() - 2), cast);
      } else {                                                                                       // default to string
        cast = trimmed;
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
