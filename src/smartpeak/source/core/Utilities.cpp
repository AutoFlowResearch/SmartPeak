// TODO: Add copyright

#include <SmartPeak/core/Utilities.h>
#include <SmartPeak/core/Table.h>
#include <SmartPeak/core/CastValue.h>
#include <OpenMS/DATASTRUCTURES/Param.h>
#include <algorithm>
#include <iostream>
#include <regex>
#include <unordered_set>
#include <plog/Log.h>

#include <sys/types.h>
#include <sys/stat.h>

#ifdef _WIN32
  #include "dirent.h"
  auto mystat = &_stat;
#else
  #include <dirent.h>
  #include <unistd.h>
  auto mystat = &stat;
#endif

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
    } else if ((lowercase_type == "bool" || lowercase_type == "string")
               && (lowercase_value == "false" || lowercase_value == "true")) {
      cast = lowercase_value == "true";
    } else if (lowercase_type == "string") {
      cast = value;
    } else {
      LOGW << "Type not supported: " << type;
      cast.setTagAndData(CastValue::UNKNOWN, value);
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
        LOGW << "Parameter not found: " << name;
        continue;
      }
      // check supplied user parameters
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
      // update the params
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
      } else if (trimmed.size() && trimmed.front() == '[' && trimmed.back() == ']') { // list
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
      } else if (trimmed.size() && trimmed.front() == trimmed.back() && (trimmed.front() == '"' || trimmed.front() == '\'')) { // string
        parseString(trimmed.substr(1, trimmed.size() - 2), cast);
      } else { // default to string
        cast = trimmed;
      }
    } catch (const std::exception& e) {
      LOGE << e.what();
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
        cast.il_.push_back(std::stoi(it->str()));
      } else if (cast.getTag() == CastValue::STRING_LIST) {
        cast.sl_.push_back((*it)[1].str());
      } else {
        throw std::invalid_argument("unexcepted tag type");
      }
    }
  }

  std::vector<std::string> Utilities::splitString(
    const std::string& s,
    const char sep
  )
  {
    std::vector<std::string> words;
    size_t l, r;
    l = r = 0;
    const size_t len = s.size();
    while (r < len) {
      if (s[r] == sep) {
        if (r - l) {
          words.emplace_back(s.substr(l, r - l));
        }
        l = r + 1;
      }
      ++r;
    }
    if (r != 0 && r - l) {
      words.emplace_back(s.substr(l));
    }
    return words;
  }

  std::string Utilities::trimString(
    const std::string& s,
    const std::string& whitespaces
  )
  {
    if (s.empty()) {
      return s;
    }

    const std::unordered_set<char> characters(whitespaces.cbegin(), whitespaces.cend());

    size_t l, r;

    for (l = 0; l < s.size() && characters.count(s[l]); ++l)
      ;

    if (l == s.size()) {
      return std::string();
    }

    // reaching here implies that s contains at least one non-whitespace character
    for (r = s.size() - 1; r > l && characters.count(s[r]); --r)
      ;

    return s.substr(l, r - l + 1);
  }

  std::vector<OpenMS::MRMFeatureSelector::SelectorParameters> Utilities::extractSelectorParameters(
    const std::vector<std::map<std::string, std::string>>& params,
    const std::vector<std::map<std::string, std::string>>& score_weights
  )
  {
    // const std::vector<std::string> param_names = {
    //   "nn_thresholds",
    //   "locality_weights",
    //   "select_transition_groups",
    //   "segment_window_lengths",
    //   "segment_step_lengths",
    //   "select_highest_counts",
    //   "variable_types",
    //   "optimal_thresholds"
    // };
    std::vector<OpenMS::MRMFeatureSelector::SelectorParameters> v;
    // auto it = std::find_if(params.cbegin(), params.cend(), [](auto& m){ return m.at("name") == "segment_window_lengths"; });
    // int n_elems = std::count(it->value.cbegin(), it->value.cend(), ',') + 1;

    std::map<std::string, std::vector<std::string>> params_map;

    for (const std::map<std::string, std::string>& m : params) {
      if (params_map.count(m.at("name"))) {
        throw std::invalid_argument("Did not expect to see the same info (\"" + m.at("name") + "\") defined multiple times.");
      }
      const std::string s = m.at("value");
      std::vector<std::string> values = splitString(s.substr(1, s.size() - 2), ',');
      params_map.emplace(m.at("name"), values);
    }

    /*
    * The user might erroneously provide different numbers of values for each parameter.
    * e.g. 4 elements for `nn_thresholds`, but only 2 for `select_transition_groups`.
    * In that case, the following loop extracts the lowest `.size()` between all
    * user-provided parameters.
    */
    // TODO: throw instead, and update parameters.csv files in all folders
    size_t n_elems = std::numeric_limits<size_t>::max();
    for (const std::pair<std::string, std::vector<std::string>>& p : params_map) {
      n_elems = std::min(n_elems, p.second.size());
    }

    for (size_t i = 0; i < n_elems; ++i) {
      OpenMS::MRMFeatureSelector::SelectorParameters parameters;
      for (const std::pair<std::string, std::vector<std::string>>& p : params_map) {
        const std::string& param_name = p.first;
        if (param_name == "nn_thresholds" ) {
          parameters.nn_threshold = std::stoi(p.second[i]);
        } else if (param_name == "locality_weights") {
          parameters.locality_weight = p.second[i].front() == 'T' || p.second[i].front() == 't';
        } else if (param_name == "select_transition_groups") {
          parameters.select_transition_group = p.second[i].front() == 'T' || p.second[i].front() == 't';
        } else if (param_name == "segment_window_lengths") {
          parameters.segment_window_length = std::stoi(p.second[i]);
        } else if (param_name == "segment_step_lengths") {
          parameters.segment_step_length = std::stoi(p.second[i]);
        } else if (param_name == "select_highest_counts") {
          // not implemented in OpenMS::MRMFeatureSelector
          // TODO: enable the throw? It would need updating all .csv parameters files
          // throw std::invalid_argument("extractSelectorParameters(): the parameter 'select_highest_counts' is not supported.\n");
        } else if (param_name == "variable_types") {
          parameters.variable_type = p.second[i].front() == 'C' || p.second[i].front() == 'c'
                                     ? OpenMS::MRMFeatureSelector::VariableType::CONTINUOUS
                                     : OpenMS::MRMFeatureSelector::VariableType::INTEGER;
        } else if (param_name == "optimal_thresholds") {
          parameters.optimal_threshold = std::stod(p.second[i]);
        } else {
          throw std::invalid_argument("param_name not valid. Check that selector's settings are properly set. Check: " + param_name + "\n");
        }
      }

      for (const std::map<std::string, std::string>& sw : score_weights) {
        OpenMS::MRMFeatureSelector::LambdaScore ls;
        if (sw.at("value") == "lambda score: score*1.0") {
          ls = OpenMS::MRMFeatureSelector::LambdaScore::LINEAR;
        } else if (sw.at("value") == "lambda score: 1/score") {
          ls = OpenMS::MRMFeatureSelector::LambdaScore::INVERSE;
        } else if (sw.at("value") == "lambda score: log(score)") {
          ls = OpenMS::MRMFeatureSelector::LambdaScore::LOG;
        } else if (sw.at("value") == "lambda score: 1/log(score)") {
          ls = OpenMS::MRMFeatureSelector::LambdaScore::INVERSE_LOG;
        } else if (sw.at("value") == "lambda score: 1/log10(score)") {
          ls = OpenMS::MRMFeatureSelector::LambdaScore::INVERSE_LOG10;
        } else {
          throw std::invalid_argument("lambda score not recognized: " + sw.at("value") + "\n");
        }
        parameters.score_weights.emplace(sw.at("name"), ls);
      }

      v.emplace_back(parameters);
    }

    return v;
  }

  bool Utilities::testStoredQuantitationMethods(const std::string& pathname)
  {
    io::CSVReader<
      4,
      io::trim_chars<' ', '\t'>,
      io::no_quote_escape<','>, // io::no_quote_escape<','>, // io::double_quote_escape<',', '\"'>,
      io::no_comment // io::single_line_comment<'#'>
    > in(pathname);
    const std::string s_is_name {"IS_name"};
    const std::string s_component_name {"component_name"};
    const std::string s_transformation_model_param_slope {"transformation_model_param_slope"};
    const std::string s_transformation_model_param_intercept {"transformation_model_param_intercept"};

    in.read_header(
      io::ignore_extra_column,
      s_is_name,
      s_component_name,
      s_transformation_model_param_slope,
      s_transformation_model_param_intercept
    );

    std::string is_name;
    std::string component_name;
    double slope;
    double intercept;
    bool is_ok {false};

    std::cout <<
      "IS_name                       \t"
      "component_name                \t"
      "slope                         \t"
      "intercept                     \n";

    while (!is_ok && in.read_row(is_name, component_name, slope, intercept)) {
      std::cout <<
        std::setw(30) << std::left << is_name << "\t" <<
        std::setw(30) << std::left << component_name << "\t" <<
        std::setw(30) << slope << "\t" <<
        std::setw(30) << intercept << std::endl;
      if (slope != 1.0 || intercept != 0.0) {
        std::cout << "is_ok!!!\n";
        is_ok = true;
      }
    }

    return is_ok;
  }

  bool Utilities::endsWith(
    std::string str,
    std::string suffix,
    const bool case_sensitive
  )
  {
    if (str.size() < suffix.size())
      return false;

    if (!case_sensitive)
    {
      std::transform(str.begin(), str.end(), str.begin(), ::tolower);
      std::transform(suffix.begin(), suffix.end(), suffix.begin(), ::tolower);
    }

    if (str.rfind(suffix) == str.size() - suffix.size())
      return true;

    return false;
  }

  void Utilities::getPathnameContent(
    const std::string& pathname,
    Table& content,
    const bool only_directories
  )
  {
    // printf("getPathnameContent(): %s\n", pathname.c_str());
    content.clear();
    const size_t names_idx = content.addColumn("Name");
    const size_t sizes_idx = content.addColumn("Size");
    const size_t types_idx = content.addColumn("Type");
    const size_t dates_idx = content.addColumn("Date Modified");

    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir(pathname.c_str()))) {
      while ((ent = readdir(dir))) {
        if (!strcmp(ent->d_name, ".") || !strcmp(ent->d_name, "..")) {
          continue;
        }
        if (only_directories && ent->d_type != DT_DIR) {
          continue;
        }
        const std::string d_name = std::string(ent->d_name);
        content.push(names_idx, d_name);
        struct stat info;
        const std::string full_name = pathname + "/" + d_name;
        mystat(full_name.c_str(), &info);
        if (ent->d_type == DT_DIR) {
          DIR *dir_subfolder;
          struct dirent *ent_subfolder;
          int n_items {-2}; // removes "." and ".." from the count
          if ((dir_subfolder = opendir(full_name.c_str()))) {
            while ((ent_subfolder = readdir(dir_subfolder))) {
              ++n_items;
            }
            closedir(dir_subfolder);
          } else { // i.e. permission denied
            n_items = 0;
          }
          content.push(sizes_idx, n_items);
          content.push(types_idx, "Directory");
        } else {
          content.push(sizes_idx, info.st_size);
          const std::string::size_type pos = d_name.rfind(".");
          content.push(types_idx, pos == std::string::npos ? "Unknown" : d_name.substr(pos));
        }
        char buff[128];
        strftime(buff, sizeof buff, "%Y/%m/%d %H:%M:%S", localtime(&(info.st_mtime)));
        content.push(dates_idx, std::string(buff));
      }
      closedir(dir);
      content.sort("Name");
    } else {
      perror("");
    }
  }

  std::string Utilities::getParentPathname(std::string pathname)
  {
    std::string parent;
    std::string suffix;

    std::replace(pathname.begin(), pathname.end(), '\\', '/');

    // Remove consecutive duplicates of character '/', because it might break the algorithm.
    // It helps with pathnames of the form "/home/user//some////folder"
    std::string::iterator it = std::unique(
      pathname.begin(),
      pathname.end(),
      [](const char a, const char b) { return a == b && a == '/'; }
    );
    pathname.erase(it, pathname.end());

    // Extract suffix, and update pathname
    // Example: it could be "C:/" on Windows, and "/" on Linux
    // suffix stays empty if the pathname is relative
    if (pathname.size() && pathname[0] == '/') { // Linux
      suffix = "/";
      pathname = pathname.substr(1);
    } else if (pathname.size() >= 2 && pathname[1] == ':') { // Windows
      if (pathname.size() == 2) {
        suffix = pathname + "/";
        pathname.clear();
      } else {
        suffix = pathname.substr(0, 3);
        pathname = pathname.substr(3);
      }
    }

    // If present, remove slash at the end
    if (pathname.size() && pathname.back() == '/')
      pathname.pop_back();

    // Extract parent pathname
    const size_t pos = pathname.find_last_of("/");
    if (pos != std::string::npos) {
      pathname = pathname.substr(0, pos);
    }

    parent = suffix + pathname;

    if (parent.empty()) {
#ifdef _WIN32
      parent = "C:/";
#else
      parent = "/";
#endif
    }

    return parent;
  }
}
