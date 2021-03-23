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
// $Maintainer: Douglas McCloskey $
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
#include <plog/Log.h>
#include <boost/filesystem.hpp>

namespace fs = boost::filesystem;

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

      OpenMS::StringList tags;
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
            OpenMS::StringList sl;
            for (const std::string& s : c.sl_) {
              sl.push_back(s);
            }
            Param_IO.setValue(name, sl, description, tags);
            break;
          }
        case CastValue::Type::BOOL_LIST:
          {
            OpenMS::StringList sl;
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

  std::array<std::vector<std::string>, 4> Utilities::getPathnameContent(
    const std::string& pathname,
    const bool asc
  )
  {
    std::array<std::vector<std::string>, 4> content;
    boost::system::error_code ec;

    fs::directory_iterator it = fs::directory_iterator(fs::path(pathname), ec);
    if (ec.value()) {
      return content;
    }
    const fs::path p(pathname);
    fs::directory_iterator it_end = fs::directory_iterator();

    for ( ; it != it_end; it++) {
      const fs::directory_entry& entry = *it;

      if (!exists(entry.path())) { // protects from e.g. "dangling" symbolic links
        LOGD << "Path does not exist. Skipping: " << entry.path();
        continue;
      }

      const std::string filename(entry.path().filename().string());
      if (filename == "." || filename == ".." || (*(&filename.at(0))) == '.') {
        continue;
      }

      size_t filesize = fs::is_directory(entry)
        ? directorySize(entry.path().string())
        : fs::file_size(entry, ec);
      if (ec.value()) {
        filesize = 0;
        ec.clear();
      }

      const std::string filetype = fs::is_directory(entry) ? "Directory" : entry.path().extension().string();

      char buff[128];
      const std::time_t t(fs::last_write_time(entry));
      std::strftime(buff, sizeof buff, "%F %T", std::localtime(&t));

      content[0].push_back(filename);
      content[1].push_back(std::to_string(filesize));
      content[2].push_back(filetype);
      content[3].push_back(std::string(buff));
    }

    std::vector<size_t> indices(content[0].size());
    std::iota(indices.begin(), indices.end(), 0);
    const std::vector<std::string>& names = content[0];
    std::sort(
      indices.begin(),
      indices.end(),
      [&names, asc](const size_t l, const size_t r)
      {
        const bool b = is_less_than_icase(names[l], names[r]);
        return asc ? b : !b;
      }
    );
    sortPairs(indices, content[0]);
    sortPairs(indices, content[1]);
    sortPairs(indices, content[2]);
    sortPairs(indices, content[3]);
    return content;
  }

  std::string Utilities::getParentPathname(const std::string& pathname)
  {
    const fs::path p(pathname);
    const fs::directory_entry entry(p);
    return entry.path().parent_path().string();
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

  size_t Utilities::directorySize(const std::string& pathname)
  {
    size_t n { 0 };
    boost::system::error_code ec;

    fs::directory_iterator it = fs::directory_iterator(fs::path(pathname), ec);
    if (ec.value()) {
      return 0;
    }
    fs::directory_iterator it_end = fs::directory_iterator();

    for ( ; it != it_end; it++) {
      const fs::path& filename { it->path().filename() };
      if (filename == fs::path(".") || filename == fs::path("..") || filename.generic_string().at(0)  == '.') {
        continue;
      }
      ++n;
    }

    return n;
  }

  std::pair<std::string, bool> Utilities::getLogFilepath(const std::string& filename)
  {
    std::string path = "";
    bool flag = false;
    char const* logs = getenv("SMARTPEAK_LOGS");
    char const* appdata = getenv("LOCALAPPDATA");
    char const* home = getenv("HOME");
    if (logs || appdata || home)
    {
      auto logdir = fs::path{};
      if (logs)
        logdir = fs::path{ logs };
      else if (appdata)
        logdir = fs::path{ appdata } / "SmartPeak";
      else if (home)
        logdir = fs::path{ home } / ".SmartPeak";

      try
      {
        // Creates directory tree if doesn't exist:
        flag = fs::create_directories(logdir);
      }
      catch (fs::filesystem_error& fe)
      {
        if (fe.code() == boost::system::errc::permission_denied)
          throw std::runtime_error(static_cast<std::ostringstream&&>(
            std::ostringstream() 
              << "Unable to construct path to log file, permission denied while creating directory '" << logdir.string() << "'").str());
        else
          throw std::runtime_error(static_cast<std::ostringstream&&>(
            std::ostringstream()
              << "Unable to construct path to log file, failure while creating directory '" << logdir.string() << "'").str());
      }
      path = (logdir / filename).string();
      // Test if file writeable:
      fs::ofstream file(path);
      if (!file)
        throw std::runtime_error(static_cast<std::ostringstream&&>(
          std::ostringstream()
            << "Unable to create log file '" << path << "', please verify permissions to directory").str());
      else
        file.close();
    }
    else
    {
      throw std::runtime_error(
        "Unable to construct path to log file. Make sure that either \
          HOME, LOCALAPPDATA or SMARTPEAK_LOGS env variable is set. For details refer to user documentation");
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
        size_human_readable_stream << std::fixed << std::setprecision(2) << entry_size;
        size_human_readable = size_human_readable_stream.str() + " Bytes";
      }
      if (entry_size >= 1e3 && entry_size < 1e6)
      {
        size_human_readable_stream << std::fixed << std::setprecision(2) << (entry_size / 1e3);
        size_human_readable = size_human_readable_stream.str() + " KB";
      }
      else if (entry_size >= 1e6 && entry_size < 1e9)
      {
        size_human_readable_stream << std::fixed << std::setprecision(2) << (entry_size / 1e6);
        size_human_readable = size_human_readable_stream.str() + " MB";
      }
      else if (entry_size >= 1e9 && entry_size < 1e12)
      {
        size_human_readable_stream << std::fixed << std::setprecision(2) << (entry_size / 1e9);
        size_human_readable = size_human_readable_stream.str() + " GB";
      }
      else if (entry_size >= 1e12 && entry_size < 1e18)
      {
        size_human_readable_stream << std::fixed << std::setprecision(2) << (entry_size / 1e12);
        size_human_readable = size_human_readable_stream.str() + " TB";
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
    char date_time_buffer [80];
    std::string date_time_buf;
    
    std::time_t current_time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    current_time_tm = std::localtime(&current_time);
    
    std::istringstream entry_date_ss(directory_entry.entry_contents[3]);
    entry_date_ss >> std::get_time(&entry_date_tm, "%F %T");
    
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
}
