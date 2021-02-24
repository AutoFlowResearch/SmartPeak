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

#include <SmartPeak/core/Parameters.h>
#include <SmartPeak/core/Utilities.h>
#include <plog/Log.h>
#include <algorithm>
#include <sstream>
#include <regex>

namespace SmartPeak
{

  Parameter::Parameter(const std::map<std::string, std::string>& attributes)
  {
    // we need a name at least
    if (attributes.count("name"))
    {
      name_ = attributes.at("name");
      if (attributes.count("value")) {
        if (attributes.count("type")) {
          Utilities::castString(attributes.at("value"), attributes.at("type"), value_);
        }
        else {
          Utilities::parseString(attributes.at("value"), value_);
        }
      }
      if (attributes.count("description"))
      {
        setDescription(attributes.at("description"));
      }
      // get min/max constraints
      std::shared_ptr<CastValue> cast_min;
      std::shared_ptr<CastValue> cast_max;
      if (attributes.count("min"))
      {
        auto c = std::make_shared<CastValue>() ;
        Utilities::parseString(attributes.at("min"), *c);
        if ((c->getTag() == CastValue::Type::INT) || (c->getTag() == CastValue::Type::FLOAT))
        {
          cast_min = c;
        }
      }
      if (attributes.count("max"))
      {
        auto c = std::make_shared<CastValue>();
        Utilities::parseString(attributes.at("max"), *c);
        if ((c->getTag() == CastValue::Type::INT) || (c->getTag() == CastValue::Type::FLOAT))
        {
          cast_max = c;
        }
      }
      setConstraintsMinMax(cast_min, cast_max);
      // get list constraints
      if (attributes.count("valid_strings"))
      {
        CastValue c;
        Utilities::parseString(attributes.at("valid_strings"), c);
        if (c.getTag() == CastValue::Type::STRING_LIST)
        {
          auto valid_strings = std::make_shared<std::vector<CastValue>>();
          for (const auto& valid_string : c.sl_)
          {
            valid_strings->push_back(CastValue(valid_string));
          }
          setConstraintsList(valid_strings);
        }
      }
      // get tags
      if (attributes.count("tags"))
      {
        std::vector<std::string> tags;
        CastValue cast_tags;
        Utilities::parseString(attributes.at("tags"), cast_tags);
        switch (cast_tags.getTag()) {
        case CastValue::Type::UNKNOWN:
        case CastValue::Type::UNINITIALIZED:
          break;
        case CastValue::Type::BOOL:
        case CastValue::Type::FLOAT:
        case CastValue::Type::INT:
        case CastValue::Type::LONG_INT:
        {
          tags.push_back(std::string(cast_tags));
        }
        break;
        case CastValue::Type::STRING:
        {
          tags = Utilities::splitString(cast_tags.s_.substr(0, cast_tags.s_.size()), ',');
        }
        break;
        case CastValue::Type::BOOL_LIST:
        {
          for (const auto& t : cast_tags.bl_)
          {
            tags.push_back(t ? "true" : "false");
          }
        }
        break;
        case CastValue::Type::FLOAT_LIST:
          for (const auto& t : cast_tags.fl_)
          {
            tags.push_back(std::to_string(t));
          }
          break;
        case CastValue::Type::INT_LIST:
          for (const auto& t : cast_tags.il_)
          {
            tags.push_back(std::to_string(t));
          }
          break;
        case CastValue::Type::STRING_LIST:
          for (const auto& t : cast_tags.sl_)
          {
            tags.push_back(t);
          }
          break;
        default:
          throw "Tag type not managed in createParameter(). Implement it.";
        }
        setTags(tags);
      }
    }
  }

  std::string Parameter::getType() const
  {
    switch (value_.getTag()) {
    case CastValue::Type::UNKNOWN:
      return "unknown";
    case CastValue::Type::STRING:
      return "string";
    case CastValue::Type::UNINITIALIZED:
      return "uninitialized";
    case CastValue::Type::BOOL:
      return "bool";
    case CastValue::Type::FLOAT:
      return "float";
    case CastValue::Type::INT:
      return "int";
    case CastValue::Type::LONG_INT:
      return "long";
    case CastValue::Type::BOOL_LIST:
      return "bool_list";
    case CastValue::Type::FLOAT_LIST:
      return "float_list";
    case CastValue::Type::INT_LIST:
      return "int_list";
    case CastValue::Type::STRING_LIST:
      return "string_list";
    default:
      throw "Tag type not managed in getType(). Implement it.";
    }
  }

  void Parameter::setValueFromString(const std::string& value_as_string)
  {
    CastValue c;
    Utilities::parseString(value_as_string, c);
    value_ = c;
  }

  const std::string Parameter::getRestrictionsAsString(bool use_schema) const
  {
    if (use_schema && schema_)
    {
      return schema_->getRestrictionsAsString();
    }
    else
    {
      if (constraints_list_)
      {
        std::stringstream ss;
        ss << "[";
        std::string separator;
        for (const auto& s : *constraints_list_)
        {
          ss << separator << s;
          separator = ",";
        }
        ss << "]";
        return ss.str();
      }
      else
      {
        std::stringstream ss;
        std::string separator;
        if (constraints_min_)
        {
          ss << "min:" << *constraints_min_;
          separator = " ";
        }
        if (constraints_max_)
        {
          ss << separator << "max:" << *constraints_max_;
        }
        return ss.str();
      }
    }
  }

  bool Parameter::getFloatValue(const CastValue& value, float& result) const
  {
    switch (value.getTag())
    {
    case CastValue::Type::FLOAT:
      result = static_cast<float>(value.f_);
      return true;
    case CastValue::Type::INT:
      result = static_cast<float>(value.i_);
      return true;
    case CastValue::Type::LONG_INT:
      result = static_cast<float>(value.li_);
      return true;
    case CastValue::Type::BOOL:
    case CastValue::Type::UNKNOWN:
    case CastValue::Type::STRING:
    case CastValue::Type::UNINITIALIZED:
    case CastValue::Type::BOOL_LIST:
    case CastValue::Type::FLOAT_LIST:
    case CastValue::Type::INT_LIST:
    case CastValue::Type::STRING_LIST:
      return false;
    default:
      throw "Tag type not managed in getFloatValue(). Implement it.";
    }
  }

  bool Parameter::isValid(const CastValue& value, bool use_schema) const
  {
    if (use_schema && schema_)
    {
      return schema_->isValid(value);
    }
    if (value_.getTag() != value.getTag())
    {
      return false;
    }
    if (constraints_list_)
    {
      if (value_.getTag() == CastValue::Type::STRING)
      {
        return std::find(constraints_list_->begin(), constraints_list_->end(), value) != constraints_list_->end();
      }
      else if (value_.getTag() == CastValue::Type::STRING_LIST)
      {
        for (const auto& s : value.sl_)
        {
          if (std::find(constraints_list_->begin(), constraints_list_->end(), value) == constraints_list_->end())
          {
            return false;
          }
        }
        return true;
      }
      else
      {
        LOGE << "Invalid type for parameter " << getName() << " (" << getType() << "). Cannot compare with string list.";
        return false;
      }
    }
    else
    {
      bool valid = true;
      if (constraints_min_)
      {
        if (value.getTag() == constraints_min_->getTag())
        {
          valid &= value >= *constraints_min_;
        }
        else
        {
          float l, r;
          valid &= getFloatValue(value, l) && getFloatValue(*constraints_min_, r) && (l >= r);
        }
      }
      if (constraints_max_)
      {
        if (value.getTag() == constraints_min_->getTag())
        {
          valid &= value <= *constraints_max_;
        }
        else
        {
          float l, r;
          valid &= getFloatValue(value, l) && getFloatValue(*constraints_max_, r) && (l <= r);
        }
      }
      return valid;
    }
  }

  bool Parameter::isValid(bool use_scheme) const
  {
    return isValid(value_, use_scheme);
  }

  bool Parameter::operator==(const Parameter& other) const
  {
    return ((value_ == other.value_) && 
            (name_ == other.name_) &&
            (description_ == other.description_) &&
            (tags_ == other.tags_) &&
            ((!constraints_min_ && !other.constraints_min_) 
            || ((constraints_min_ && other.constraints_min_) && (*constraints_min_ == *other.constraints_min_))) && 
            ((!constraints_max_ && !other.constraints_max_) 
            || ((constraints_max_ && other.constraints_max_) && (*constraints_max_ == *other.constraints_max_))) &&
            ((!constraints_list_ && !other.constraints_list_)
            || ((constraints_list_ && other.constraints_list_) && (*constraints_list_ == *other.constraints_list_)))
      );
  }

  FunctionParameters::FunctionParameters(const std::string& function_name, const std::vector<std::map<std::string, std::string>>& function_parameters) :
    function_name_(function_name)
  {
    for (const auto& parameter : function_parameters)
    {
      addParameter(parameter);
    }
  }
  
  FunctionParameters::FunctionParameters(const OpenMS::DefaultParamHandler& param_handler)
  {
    function_name_ = param_handler.getName();
    OpenMS::Param openms_params = param_handler.getParameters();
    for (OpenMS::Param::ParamIterator it = openms_params.begin(); it != openms_params.end(); ++it)
    {
      const auto& p = *it;
      auto p_name = it.getName(); // use iterator name for full name
      switch (p.value.valueType())
      {
      case OpenMS::DataValue::STRING_VALUE:
      {
        OpenMS::StringList openms_strings = p.valid_strings;
        const auto constraints_list = std::make_shared<std::vector<CastValue>>();
        if (openms_strings.size() > 0)
        {
          for (const auto& openms_string : openms_strings) constraints_list->push_back(openms_string);
        }
        // String parameter can be boolean parameter: string "true" or " "false" and constraints ["false","true"]
        bool is_bool = false;
        bool bool_value = false;
        if (constraints_list->size() == 2)
        {
          std::vector<std::string> values;
          values.push_back(p.value.toString());
          values.push_back(std::string(constraints_list->at(0)));
          values.push_back(std::string(constraints_list->at(1)));
          std::vector<bool> parsed;
          for (const auto& v : values)
          {
            std::string lowercase;
            lowercase.resize(v.size());
            std::transform(v.begin(), v.end(), lowercase.begin(), ::tolower);
            bool valid = (lowercase == "true") || (lowercase == "false");
            if (!valid)
              break;
            else
              parsed.push_back((lowercase == "true"));
          }
          if (parsed.size() == 3)
          {
            is_bool = (parsed.size() == 3) && (parsed[1] != parsed[2]);
            bool_value = parsed[0];
          }
        }
        if (is_bool)
        {
          auto param = Parameter(p_name, bool_value);
          param.setDescription(p.description);
          addParameter(param);
        }
        else
        {
          auto param = Parameter(p_name, std::string(p.value));
          if (openms_strings.size() > 0)
          {
            param.setConstraintsList(constraints_list);
          }
          param.setDescription(p.description);
          addParameter(param);
        }
      }
      break;
      case OpenMS::DataValue::INT_VALUE:
      {
        auto param = Parameter(p_name, static_cast<int>(p.value));
        param.setDescription(p.description);
        std::shared_ptr<CastValue> cast_min;
        if (p.min_int != -std::numeric_limits<int>::max())
        {
          cast_min = std::make_shared<CastValue>(p.min_int);
        }
        std::shared_ptr<CastValue> cast_max;
        if (p.max_int != std::numeric_limits<int>::max())
        {
          cast_max = std::make_shared<CastValue>(p.max_int);
        }
        if (cast_min || cast_max)
        {
          param.setConstraintsMinMax(cast_min, cast_max);
        }
        addParameter(param);
      }
      break;
      case OpenMS::DataValue::DOUBLE_VALUE:
      {
        auto param = Parameter(p_name, static_cast<float>(p.value));
        param.setDescription(p.description);
        std::shared_ptr<CastValue> cast_min;
        if (p.min_float != -std::numeric_limits<double>::max())
        {
          cast_min = std::make_shared<CastValue>(static_cast<float>(p.min_float));
        }
        std::shared_ptr<CastValue> cast_max;
        if (p.max_float != std::numeric_limits<double>::max())
        {
          cast_max = std::make_shared<CastValue>(static_cast<float>(p.max_float));
        }
        if (cast_min || cast_max)
        {
          param.setConstraintsMinMax(cast_min, cast_max);
        }
        addParameter(param);
      }
      break;
      case OpenMS::DataValue::STRING_LIST:
      {
        std::vector<std::string> list_strings;
        for (const auto& openms_string : p.value.toStringList()) list_strings.push_back(openms_string);
        auto param = Parameter(p.name, list_strings);
        OpenMS::StringList openms_strings = p.valid_strings;
        if (openms_strings.size() > 0)
        {
          const auto constraints_list = std::make_shared<std::vector<CastValue>>();
          for (const auto& openms_string : openms_strings) constraints_list->push_back(openms_string);
          param.setConstraintsList(constraints_list);
        }
        addParameter(param);
      }
      break;
      case OpenMS::DataValue::INT_LIST:
      {
        std::vector<int> list_ints;
        for (const auto& openms_int : p.value.toIntList()) list_ints.push_back(openms_int);
        auto param = Parameter(p.name, list_ints);
        std::shared_ptr<CastValue> cast_min;
        if (p.min_int != -std::numeric_limits<int>::max())
        {
          cast_min = std::make_shared<CastValue>(p.min_int);
        }
        std::shared_ptr<CastValue> cast_max;
        if (p.max_int != std::numeric_limits<int>::max())
        {
          cast_max = std::make_shared<CastValue>(p.max_int);
        }
        if (cast_min || cast_max)
        {
          param.setConstraintsMinMax(cast_min, cast_max);
        }
        addParameter(param);
      }
      break;
      case OpenMS::DataValue::DOUBLE_LIST:
      {
        std::vector<float> list_doubles;
        for (const auto& openms_double : p.value.toDoubleList()) list_doubles.push_back(openms_double);
        Parameter param = Parameter(p.name, list_doubles);
        std::shared_ptr<CastValue> cast_min;
        if (p.min_float != -std::numeric_limits<double>::max())
        {
          cast_min = std::make_shared<CastValue>(static_cast<float>(p.min_float));
        }
        std::shared_ptr<CastValue> cast_max;
        if (p.max_float != std::numeric_limits<double>::max())
        {
          cast_max = std::make_shared<CastValue>(static_cast<float>(p.max_float));
        }
        if (cast_min || cast_max)
        {
          param.setConstraintsMinMax(cast_min, cast_max);
        }
        addParameter(param);
      }
      break;
      case OpenMS::DataValue::EMPTY_VALUE:
      {
        LOGE << "createFromOpenMS Ignoring empty value " << p_name;
      }
      break;
      }
    }
  }

  void FunctionParameters::addParameter(const Parameter& parameter)
  {
    auto existing_parameter = findParameter(parameter.getName());
    if (!existing_parameter)
    {
      parameters_.push_back(parameter);
    }
  }

  void FunctionParameters::merge(const FunctionParameters& other)
  {
    for (auto& parameter : other)
    {
      auto existing_parameter = findParameter(parameter.getName());
      if (!existing_parameter)
      {
        parameters_.push_back(parameter);
      }
      else
      {
        // If the parameter exists and the other is a schema, 
        // set it automatically as the schema of the other one.
        if ((parameter.isSchema()) && (!existing_parameter->isSchema()))
        {
          existing_parameter->setSchema(parameter);
        }
        // Else: the existing parameter will not be replaced.
      }
    }
  }

  void FunctionParameters::setAsSchema(bool is_schema)
  {
    for (auto& param : parameters_)
    {
      param.setAsSchema(is_schema);
    }
  }

  Parameter* FunctionParameters::findParameter(const std::string& parameter_name)
  {
    auto find_it = std::find_if(
      parameters_.begin(), parameters_.end(),
      [&parameter_name](const auto& param_test)
    { return parameter_name == param_test.getName(); });
    if (find_it != parameters_.end())
      return &(*find_it);
    else
      return nullptr;
  }

  bool FunctionParameters::operator==(const FunctionParameters& other) const
  {
    return ((function_name_ == other.function_name_) && (parameters_ == other.parameters_));
  }

  ParameterSet::ParameterSet(const std::map<std::string, std::vector<std::map<std::string, std::string>>>& functions_map)
  {
    for (const auto& function : functions_map)
    {
      const std::string& function_name = function.first;
      const std::vector<std::map<std::string, std::string>> function_parameters = function.second;
      FunctionParameters new_function_parameter(function_name, function_parameters);
      addFunctionParameters(new_function_parameter);
    }
  }
  
  ParameterSet::ParameterSet(const std::vector<OpenMS::DefaultParamHandler>& open_ms_params)
  {
    for (const auto& open_ms_param : open_ms_params)
    {
      addFunctionParameters(FunctionParameters(open_ms_param));
    }
  }

  void ParameterSet::merge(const ParameterSet& other)
  {
    for (auto& function_parameter : other)
    {
      if (function_parameters_.count(function_parameter.second.getFunctionName()))
      {
        function_parameters_.at(function_parameter.second.getFunctionName()).merge(function_parameter.second);
      }
      else
      {
        addFunctionParameters(function_parameter.second);
      }
    }
  }

  Parameter* ParameterSet::findParameter(const std::string& function_name, const std::string& parameter_name)
  {
    if (!function_parameters_.count(function_name))
    {
      return nullptr;
    }
    else
    {
      return function_parameters_.at(function_name).findParameter(parameter_name);
    }
  }

  void ParameterSet::clear()
  {
    function_parameters_.clear();
  }

  void ParameterSet::addParameter(const std::string& function_name, Parameter& parameter)
  {
    if (function_parameters_.count(function_name))
    {
      function_parameters_.at(function_name).addParameter(parameter);
    }
    else
    {
      FunctionParameters new_function_parameter(function_name);
      new_function_parameter.addParameter(parameter);
      function_parameters_[function_name] = new_function_parameter;
    }
  }

  void ParameterSet::addFunctionParameters(FunctionParameters function_parameter)
  {
    function_parameters_[function_parameter.getFunctionName()] = function_parameter;
  }

  void ParameterSet::setAsSchema(bool is_schema)
  {
    for (auto& function_parameters : function_parameters_) {
      function_parameters.second.setAsSchema(is_schema);
    }
  }

  bool ParameterSet::operator==(const ParameterSet& other) const
  {
    return function_parameters_ == other.function_parameters_;
  }

}