// --------------------------------------------------------------------------
//   SmartPeak -- Fast and Accurate CE-, GC- and LC-MS(/MS) Data Processing
// --------------------------------------------------------------------------
// Copyright The SmartPeak Team -- Novo Nordisk Foundation 
// Center for Biosustainability, Technical University of Denmark 2018-2021.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL ANY OF THE AUTHORS OR THE CONTRIBUTING
// INSTITUTIONS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
// OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
// WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
// OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
// ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// --------------------------------------------------------------------------
// $Maintainer: Douglas McCloskey, Ahmed Khalil $
// $Authors: Douglas McCloskey, Pasquale Domenico Colaianni $
// --------------------------------------------------------------------------

#include <SmartPeak/core/Utilities.h>
#include <SmartPeak/core/CastValue.h>
#include <SmartPeak/core/Parameters.h>
#include <SmartPeak/smartpeak_package_version.h>
#include <OpenMS/DATASTRUCTURES/Param.h>
#include <algorithm>
#include <iostream>
#include <numeric>
#include <regex>
#include <sstream>
#include <unordered_set>
#include <chrono>
#include <plog/Log.h>

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
    } else if (lowercase_type == "long") {
      cast = std::stol(value);
    } else if ((lowercase_type == "bool" || lowercase_type == "boolean" || lowercase_type == "string")
               && (lowercase_value == "false" || lowercase_value == "true")) {
      cast = lowercase_value == "true";
    } else if (lowercase_type == "string") {
      cast = value;
    } else if (lowercase_type == "list") {
      parseString(value, cast);
    } else {
      LOGW << "Type not supported: " << type;
      cast.setTagAndData(CastValue::Type::UNKNOWN, value);
    }
  }

  void Utilities::setUserParameters(
    OpenMS::DefaultParamHandler& Param_handler_IO,
    const ParameterSet& user_parameters_I,
    const std::string param_handler_name
  )
  {
    std::string function_parameters_name = (param_handler_name.empty() ? Param_handler_IO.getName() : param_handler_name);
    if (user_parameters_I.count(function_parameters_name))
    {
      OpenMS::Param parameters = Param_handler_IO.getParameters();
      Utilities::updateParameters(parameters, user_parameters_I.at(function_parameters_name));
      Param_handler_IO.setParameters(parameters);
    }
  }

  void Utilities::updateParameters(
    OpenMS::Param& Param_IO,
    const FunctionParameters& parameters_I
  )
  {
    for (const auto& param : parameters_I) {
      const std::string& name = param.getName();
      if (!Param_IO.exists(name)) {
        LOGW << "Parameter not found: " << name;
        continue;
      }
      // check supplied user parameters
      CastValue c = param.getValue();
      if ((c.getTag() == CastValue::Type::UNINITIALIZED) ||
          (c.getTag() == CastValue::Type::UNKNOWN))
      {
        OpenMS::ParamValue::ValueType dt = Param_IO.getValue(name).valueType();
        switch (dt) {
          case OpenMS::ParamValue::DOUBLE_VALUE:
            c = static_cast<float>(Param_IO.getValue(name));
            break;
          case OpenMS::ParamValue::INT_VALUE:
            c = static_cast<int>(Param_IO.getValue(name));
            break;
          case OpenMS::ParamValue::STRING_VALUE:
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
          case OpenMS::ParamValue::DOUBLE_LIST:
            c = std::vector<float>();
            for (const double n : Param_IO.getValue(name).toDoubleVector()) {
              c.fl_.push_back(n);
            }
            break;
          case OpenMS::ParamValue::INT_LIST:
            c = std::vector<int>();
            for (const int n : Param_IO.getValue(name).toIntVector()) {
              c.il_.push_back(n);
            }
            break;
          case OpenMS::ParamValue::STRING_LIST:
            {
              bool strings_are_bools = false;
              if (Param_IO.getValue(name).toStringVector().size()) {
                const std::string& value = Param_IO.getValue(name).toStringVector().front();
                std::string lowercase_value;
                std::transform(value.begin(), value.end(), lowercase_value.begin(), ::tolower);
                if (lowercase_value == "true" || lowercase_value == "false") {
                  c = std::vector<bool>();
                  strings_are_bools = true;
                }
              } else {
                c = std::vector<std::string>();
              }

              for (const std::string& s : Param_IO.getValue(name).toStringVector()) {
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
            c.setTagAndData(CastValue::Type::UNKNOWN, Param_IO.getValue(name).toString());
        }
      }

      std::string description = param.getDescription();
      if (description.empty())
      {
        try {
          description = Param_IO.getDescription(name);
        }
        catch (const OpenMS::Exception::ElementNotFound&) {
          // leave description as an empty string
        }
      }

      std::vector<std::string> tags;
      const auto& params_tag = param.getTags();
      if (!params_tag.empty())
      {
        for (const auto& tag : params_tag)
        {
          tags.push_back(tag);
        }
      }
      else
      {
        tags = Param_IO.getTags(name);
      }
      // update the params
      switch (c.getTag()) {
        case CastValue::Type::BOOL:
          Param_IO.setValue(name, c.b_ ? "true" : "false", description, tags);
          break;
        case CastValue::Type::FLOAT:
          Param_IO.setValue(name, static_cast<double>(c.f_), description, tags);
          break;
        case CastValue::Type::INT:
          Param_IO.setValue(name, c.i_, description, tags);
          break;
        case CastValue::Type::STRING:
          Param_IO.setValue(name, c.s_, description, tags);
          break;
        case CastValue::Type::FLOAT_LIST:
          {
            std::vector<double> dl;
            for (const float f : c.fl_) {
              dl.push_back(f);
            }
            Param_IO.setValue(name, dl, description, tags);
            break;
          }
        case CastValue::Type::INT_LIST:
          {
            std::vector<int> il;
            for (const int i : c.il_) {
              il.push_back(i);
            }
            Param_IO.setValue(name, il, description, tags);
            break;
          }
        case CastValue::Type::STRING_LIST:
          {
            Param_IO.setValue(name, c.sl_, description, tags);
            break;
          }
        case CastValue::Type::BOOL_LIST:
          {
            std::vector<std::string> sl;
            for (const bool b : c.bl_) {
              sl.push_back(b ? "true" : "false");
            }
            Param_IO.setValue(name, sl, description, tags);
            break;
          }
        case CastValue::Type::UNKNOWN:
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
    std::regex re_s("[\"']([^,]+)[\"']");
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
      if (cast.getTag() == CastValue::Type::BOOL_LIST) {
        cast.bl_.push_back(it->str()[0] == 't' || it->str()[0] == 'T');
      } else if (cast.getTag() == CastValue::Type::FLOAT_LIST) {
        cast.fl_.push_back(std::strtof(it->str().c_str(), NULL));
      } else if (cast.getTag() == CastValue::Type::INT_LIST) {
        cast.il_.push_back(std::stoi(it->str()));
      } else if (cast.getTag() == CastValue::Type::STRING_LIST) {
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
      {;}

    if (l == s.size()) {
      return std::string();
    }

    // reaching here implies that s contains at least one non-whitespace character
    for (r = s.size() - 1; r > l && characters.count(s[r]); --r)
      {;}

    return s.substr(l, r - l + 1);
  }

  std::vector<OpenMS::MRMFeatureSelector::SelectorParameters> Utilities::extractSelectorParameters(
    const FunctionParameters& params,
    const FunctionParameters& score_weights
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
    // auto it = std::find_if(params.cbegin(), params.cend(), [](auto& m){ return m.getName() == "segment_window_lengths"; });
    // int n_elems = std::count(it->value.cbegin(), it->value.cend(), ',') + 1;

    std::map<std::string, std::vector<std::string>> params_map;

    for (const auto& m : params) {
      if (params_map.count(m.getName())) {
        throw std::invalid_argument("Did not expect to see the same info (\"" + m.getName() + "\") defined multiple times.");
      }
      const std::string s = m.getValueAsString();
      std::vector<std::string> values = splitString(s.substr(1, s.size() - 2), ',');
      params_map.emplace(m.getName(), values);
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
        if (param_name == "nn_thresholds") {
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

      for (const auto& sw : score_weights) {
        OpenMS::MRMFeatureSelector::LambdaScore ls;
        if (sw.getValueAsString() == "lambda score: score*1.0") {
          ls = OpenMS::MRMFeatureSelector::LambdaScore::LINEAR;
        } else if (sw.getValueAsString() == "lambda score: 1/score") {
          ls = OpenMS::MRMFeatureSelector::LambdaScore::INVERSE;
        } else if (sw.getValueAsString() == "lambda score: log(score)") {
          ls = OpenMS::MRMFeatureSelector::LambdaScore::LOG;
        } else if (sw.getValueAsString() == "lambda score: 1/log(score)") {
          ls = OpenMS::MRMFeatureSelector::LambdaScore::INVERSE_LOG;
        } else if (sw.getValueAsString() == "lambda score: 1/log10(score)") {
          ls = OpenMS::MRMFeatureSelector::LambdaScore::INVERSE_LOG10;
        } else {
          throw std::invalid_argument("lambda score not recognized: " + sw.getValueAsString() + "\n");
        }
        parameters.score_weights.emplace(sw.getName(), ls);
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

  std::array<std::vector<std::string>, 4> Utilities::getFolderContents(const std::filesystem::path& folder_path, bool only_directories)
  {
    // name, ext, size, date
    std::array<std::vector<std::string>, 4> directory_entries;
    std::vector<std::tuple<std::string, uintmax_t, std::string, std::time_t>> entries_temp;
    
    try {
      for (auto & p : std::filesystem::directory_iterator(folder_path, std::filesystem::directory_options::skip_permission_denied)) {
        if (!isHiddenEntry(p))
        {
          auto last_write_time = std::filesystem::last_write_time(p.path());
          auto sctp = std::chrono::time_point_cast<std::chrono::system_clock::duration>(last_write_time
            - std::filesystem::file_time_type::clock::now()
            + std::chrono::system_clock::now());
          std::time_t cftime = std::chrono::system_clock::to_time_t(sctp);
          if (p.is_regular_file() && (!only_directories))
          {
            entries_temp.push_back(std::make_tuple(p.path().filename().string(), p.file_size(), p.path().extension().string(), cftime));
          }
          else if (p.is_directory())
          {
            std::tuple<float, uintmax_t> directory_info;
            getDirectoryInfo(p, directory_info);
            entries_temp.push_back(std::make_tuple(p.path().filename().string(), std::get<1>(directory_info), "Directory", cftime));
          }
        }
      }
    }
    catch (const std::exception& e) {
      LOGE << "Utilities::getFolderContents : " << typeid(e).name() << " : " << e.what();
    }
    
    if (entries_temp.size() > 1)
    {
      for (uintmax_t i = 0; i < entries_temp.size(); i++)
      {
        directory_entries[0].push_back(std::get<0>(entries_temp[i]));
        directory_entries[1].push_back((std::get<1>(entries_temp[i]) == 0 ? "0" : std::to_string(std::get<1>(entries_temp[i])) ));
        directory_entries[2].push_back(std::get<2>(entries_temp[i]));
        char buff[128];
        std::strftime(buff, sizeof(buff), "%Y-%m-%d %H:%M:%S", std::localtime(&std::get<3>(entries_temp[i])));
        directory_entries[3].push_back(buff);
      }
    }
    else if (entries_temp.size() == 1)
    {
      directory_entries[0].push_back(std::get<0>(entries_temp[0]));
      directory_entries[1].push_back((std::get<1>(entries_temp[0]) == 0 ? "0" : std::to_string(std::get<1>(entries_temp[0])) ));
      directory_entries[2].push_back(std::get<2>(entries_temp[0]));
      char buff[128];
      std::strftime(buff, sizeof(buff), "%Y-%m-%d %H:%M:%S", std::localtime(&std::get<3>(entries_temp[0])));
      directory_entries[3].push_back(buff);
    }
    
    return directory_entries;
  }

  std::string Utilities::getParentPath(const std::filesystem::path& p)
  {
    std::filesystem::path parent_path;
    try {
      if (p.string() == ".") {
        std::filesystem::path working_dir(std::filesystem::current_path());
        parent_path = (working_dir.parent_path());
      } else if (p.has_parent_path()) {
        parent_path = (p.parent_path());
      }
    } catch (const std::exception& e) {
      LOGE << "Utilities::getParentPath : " << typeid(e).name() << " : " << e.what();
    }
    return parent_path.string();
  }

  bool Utilities::is_less_than_icase(const std::string& a, const std::string& b)
  {
    std::string a_lowercase, b_lowercase;
    a_lowercase.resize(a.size());
    b_lowercase.resize(b.size());
    std::transform(a.begin(), a.end(), a_lowercase.begin(), ::tolower);
    std::transform(b.begin(), b.end(), b_lowercase.begin(), ::tolower);
    return a_lowercase.compare(b_lowercase) < 0;
  }

  void Utilities::getDirectoryInfo(const std::filesystem::path& folder_path, std::tuple<float, uintmax_t>& directory_info)
  {
    float size_in_bytes = 0;
    uintmax_t entries = 0;
    if (!isHiddenEntry(folder_path)) {
      for (auto & p : std::filesystem::directory_iterator(folder_path, std::filesystem::directory_options::skip_permission_denied)) {
        std::filesystem::path::string_type entry_name = p.path().filename();
        if (p.is_regular_file() && !p.is_directory() && !isHiddenEntry(p)) {
          size_in_bytes += p.file_size();
          entries += 1;
        } else if (p.is_directory() && !isHiddenEntry(p)) {
          entries += 1;
        }
      }
    }
    std::get<0>(directory_info) = size_in_bytes;
    std::get<1>(directory_info) = entries;
  }

  bool Utilities::isHiddenEntry(const std::filesystem::path& entry_path)
  {
    bool is_hidden = false;
    std::string entry_name = entry_path.filename().string();
    if (entry_name == "." || (std::string)entry_name == ".." || entry_name[0] == '.') is_hidden = true;
    return is_hidden;
  }

  std::pair<std::filesystem::path, bool> Utilities::getLogFilepath(const std::string& filename)
  {
    std::filesystem::path path{};
    bool flag = false;
    std::string env_val_smartpeak_logs  = "SMARTPEAK_LOGS";
    std::string env_val_localappdata    = "LOCALAPPDATA";
    std::string env_val_home            = "HOME";
    
    std::string logs_path, appsdata_path, home_path;

    getEnvVariable(env_val_smartpeak_logs.c_str(), &logs_path);
    getEnvVariable(env_val_localappdata.c_str(), &appsdata_path);
    getEnvVariable(env_val_home.c_str(), &home_path);
    
    if (!logs_path.empty() || !appsdata_path.empty() || !home_path.empty())
    {
      auto logdir = std::filesystem::path();
      if (!logs_path.empty())
        logdir = std::filesystem::path(logs_path);
      else if (!appsdata_path.empty())
        logdir = std::filesystem::path(appsdata_path) / "SmartPeak";
      else if (!home_path.empty())
        logdir = std::filesystem::path(home_path) / ".SmartPeak";

      try
      {
        // Creates directory tree if doesn't exist:
        flag = std::filesystem::create_directories(logdir);
      }
      catch (std::filesystem::filesystem_error& fe)
      {
        if (fe.code() == std::errc::permission_denied)
          throw std::runtime_error(static_cast<std::ostringstream&&>(
            std::ostringstream()
              << "Unable to create path to log file, permission denied while creating directory '"
              << logdir.string() << "'").str());
        else
          throw std::runtime_error(static_cast<std::ostringstream&&>(
            std::ostringstream()
              << "Unable to create path to log file, failure while creating directory '"
              << logdir.string() << "'").str());
      }
      path = (logdir / filename);
      // Test if file writeable:
      std::ofstream file(path, std::ofstream::out);
      if (!file)
        throw std::runtime_error(static_cast<std::ostringstream&&>(
          std::ostringstream()
            << "Unable to create log file " << path << ", please verify directory permissions").str());
      else
        file.close();
    }
    else
    {
      throw std::runtime_error(
        "Unable to create log file. Please make sure that either "
        "HOME, LOCALAPPDATA or SMARTPEAK_LOGS env variable is set. For details refer to user documentation");
    }
    return std::make_pair(path, flag);
  }

  std::string Utilities::getSmartPeakVersion()
  {
#if defined(SMARTPEAK_PACKAGE_VERSION)
    return static_cast<std::ostringstream&&>(std::ostringstream()
      << SMARTPEAK_PACKAGE_VERSION).str();
#else
    return "Unknown";
#endif
  }

  void Utilities::getEnvVariable(const char *env_name, std::string *path)
  {
#ifdef _WIN32
    char* var_buffer;
    size_t buffer_size;
    errno_t error_val = _dupenv_s(&var_buffer, &buffer_size, env_name);
    
    if (error_val != 0) {
      throw std::runtime_error(
      "LOCALAPPDATA or SMARTPEAK_LOGS env variable is not set for the log files to be saved in these locations.");
    } else {
      if (var_buffer != nullptr) {
        *path = std::string(var_buffer);
        free(var_buffer);
      }
    }
#else
    char* var_buffer = getenv(env_name);
    if (var_buffer != nullptr) {
      *path = std::string(var_buffer);
    }
#endif
  }

  void Utilities::makeHumanReadable(ImEntry& directory_entry)
  {
    // prettify size [1]
    std::string entry_size_string = directory_entry.entry_contents[1];
    if (directory_entry.entry_contents[2] != "Directory" && !directory_entry.entry_contents[1].empty()
        && entry_size_string.find("Bytes") == std::string::npos
        && entry_size_string.find("KB") == std::string::npos
        && entry_size_string.find("MB") == std::string::npos
        && entry_size_string.find("GB") == std::string::npos
        && entry_size_string.find("TB") == std::string::npos
        )
    {
      double entry_size = std::stod(entry_size_string);
      std::string size_human_readable = "";
      std::stringstream size_human_readable_stream;
      
      if (entry_size >= 0 && entry_size < 1e3)
      {
        size_human_readable_stream << entry_size;
        size_human_readable = size_human_readable_stream.str() + " Bytes";
      }
      if (entry_size >= 1e3 && entry_size < 1e6)
      {
        size_human_readable_stream << std::fixed << std::setprecision(2) << (entry_size / 1e3);
        auto size_human_readable_str = size_human_readable_stream.str();
        removeTrailing(size_human_readable_str, ".00");
        size_human_readable = size_human_readable_str + " KB";
      }
      else if (entry_size >= 1e6 && entry_size < 1e9)
      {
        size_human_readable_stream << std::fixed << std::setprecision(2) << (entry_size / 1e6);
        auto size_human_readable_str = size_human_readable_stream.str();
        removeTrailing(size_human_readable_str, ".00");
        size_human_readable = size_human_readable_str + " MB";
      }
      else if (entry_size >= 1e9 && entry_size < 1e12)
      {
        size_human_readable_stream << std::fixed << std::setprecision(2) << (entry_size / 1e9);
        auto size_human_readable_str = size_human_readable_stream.str();
        removeTrailing(size_human_readable_str, ".00");
        size_human_readable = size_human_readable_str + " GB";
      }
      else if (entry_size >= 1e12 && entry_size < 1e18)
      {
        size_human_readable_stream << std::fixed << std::setprecision(2) << (entry_size / 1e12);
        auto size_human_readable_str = size_human_readable_stream.str();
        removeTrailing(size_human_readable_str, ".00");
        size_human_readable = size_human_readable_str + " TB";
      }
    
      directory_entry.entry_contents[1] = size_human_readable;
    }
    else if (directory_entry.entry_contents[2] == "Directory")
    {
      directory_entry.entry_contents[1] = entry_size_string + " Item(s)";
    }
    
    // prettify extension [2]
    std::string* extension = &directory_entry.entry_contents[2];
    extension->erase(std::remove_if(extension->begin(), extension->end(),
                                   [](unsigned char c){ return std::ispunct(c); }), extension->end());
    
    // prettify date [3]
    std::tm * current_time_tm, entry_date_tm;
    char date_time_buffer[80];
    std::string date_time_buf;
    
    std::time_t current_time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    current_time_tm = std::localtime(&current_time);
    
    std::istringstream entry_date_ss(directory_entry.entry_contents[3]);
    entry_date_ss >> std::get_time(&entry_date_tm, "%Y-%m-%d %T");
    
    entry_date_tm.tm_isdst = current_time_tm->tm_isdst;    
    time_t entry_date_time = std::mktime(&entry_date_tm);
        
    if (entry_date_tm.tm_mday == current_time_tm->tm_mday
        && entry_date_tm.tm_mon == current_time_tm->tm_mon
        && entry_date_tm.tm_year == current_time_tm->tm_year)
    {
      std::strftime(date_time_buffer, sizeof date_time_buffer, "%R", std::localtime(&entry_date_time));
      directory_entry.entry_contents[3] = "Today at " + std::string(date_time_buffer);
    }
    else
    {
      std::strftime(date_time_buffer, sizeof date_time_buffer, "%c", std::localtime(&entry_date_time));
      directory_entry.entry_contents[3] = date_time_buffer;
    }
  }

  std::string Utilities::str2upper(const std::string& str)
  {
    auto str_ = str;
    std::transform(str_.begin(), str_.end(), str_.begin(), ::toupper);
    return str_;
  }

  void Utilities::removeTrailing(std::string& str, std::string to_remove)
  {
    auto it = str.find(to_remove);
    if (it != std::string::npos) str.erase(it, str.length());
  }

  Filenames Utilities::buildFilenamesFromDirectory(ApplicationHandler& application_handler, const std::filesystem::path& path)
  {
    Filenames filenames;
    filenames.setTag(Filenames::Tag::MAIN_DIR, path.generic_string());
    for (const auto& filename_handler : application_handler.loading_processors_)
    {
      filename_handler->getFilenames(filenames);
    }
    for (auto& file_id : filenames.getFileIds())
    {
      filenames.setEmbedded(file_id, false);
      const auto& full_path = filenames.getFullPath(file_id);
      if (!std::filesystem::exists(full_path))
      {
        LOGI << "Non existing file, will not be used: " << full_path.generic_string();
        filenames.setFullPath(file_id, "");
      }
    }
    return filenames;
  }

  std::filesystem::path Utilities::createEmptyTempDirectory()
  {
    std::filesystem::path path;
    uint i;
    uint max_tries = 1000;
    while (true)
    {
      path = std::tmpnam(nullptr);
      if (std::filesystem::create_directory(path))
      {
        break;
      }
      if (i == max_tries)
      {
        throw std::runtime_error("could not find non-existing directory");
      }
      i++;
    }
    return path;
  }
}

