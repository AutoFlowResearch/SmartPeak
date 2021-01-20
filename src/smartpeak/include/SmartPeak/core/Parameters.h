/**TODO:  Add copyright*/

#pragma once


#include <SmartPeak/core/CastValue.h>
#include <OpenMS/DATASTRUCTURES/DefaultParamHandler.h>
#include <map>
#include <vector>
#include <string>
#include <memory>
#include <iostream>

namespace SmartPeak
{

  /**
   * This class holds parameter to use with the commands.
   * Value can be validated, applaying constraints, through a schema (another Parameter).
   */
  class Parameter
  {
  public:

    // non value
    Parameter(const std::string& name)
      : name_(name), value_() { };
    // from CastValue
    Parameter(const std::string& name, const CastValue& value)
      : name_(name), value_(value) { };
    // from map structure
    Parameter(const std::map<std::string, std::string>& properties);

    std::string getType() const;

    bool isValid(bool use_scheme = true) const;
    bool isValid(const CastValue& value, bool use_scheme = true) const;

    void setName(const std::string& name) { name_ = name; };
    const std::string& getName() const { return name_; };

    void setConstraintsMinMax(const std::shared_ptr<CastValue>& min, const std::shared_ptr<CastValue>& max) { constraints_min_ = min; constraints_max_ = max; };
    void setConstraintsList(const std::shared_ptr<std::vector<CastValue>>& list) { constraints_list_ = list; };

    void setValueFromString(const std::string& value_as_string);
    const std::string getValueAsString() const { return std::string(value_); };

    void setDescription(const std::string& description) { description_ = description; };
    const std::string& getDescription(bool use_scheme=true) const { return (use_scheme && schema_) ? schema_->getDescription() : description_; };

    void setSchema(const Parameter& schema) { schema_ = std::make_shared<Parameter>(schema); }
    const Parameter* getSchema() const { return schema_.get(); };

    void setTags(const std::vector<std::string>& tags) { tags_ = tags; };
    const std::vector<std::string>& getTags(bool use_scheme = true) const { return (use_scheme && schema_) ? schema_->getTags() : tags_; };

    const CastValue& getValue() const { return value_; };
    const CastValue& getDefaultValue() const { return schema_ ? schema_->getValue() : value_; };

    const std::string getRestrictionsAsString(bool use_scheme = true) const;

    void setAsSchema(bool is_schema) { is_schema_ = is_schema; };
    bool isSchema() const { return is_schema_; };
  
  protected:
    std::string name_;
    CastValue value_;
    std::string description_;
    std::shared_ptr <Parameter> schema_;
    std::vector<std::string> tags_;
    std::shared_ptr<CastValue> constraints_min_;
    std::shared_ptr<CastValue> constraints_max_;
    std::shared_ptr<std::vector<CastValue>> constraints_list_;
    bool is_schema_ = false;
  };

  /**
   FunctionParameters holds a name and an associated list of Parameters.
  */
  class FunctionParameters
  {
  public:
    FunctionParameters() {};
    FunctionParameters(const std::string& function_name) : function_name_(function_name) {};
    // from vector/map structure
    FunctionParameters(const std::string& function_name, const std::vector<std::map<std::string, std::string>>& parameters_list);
    // from OpenMS
    FunctionParameters(const OpenMS::DefaultParamHandler& param_handler);

    /**
    Mark all parameters as schema parameter.
    */
    void setAsSchema(bool is_schema);

    Parameter* findParameter(const std::string& parameter);
    const std::string& getFunctionName() const { return function_name_; };
    void addParameter(Parameter& parameter);
    void merge(const FunctionParameters& other);

    // underlying vector accessors
    size_t size() const { return parameters_.size(); };
    std::vector<Parameter>::iterator begin() { return parameters_.begin(); };
    std::vector<Parameter>::iterator end() { return parameters_.end(); };
    std::vector<Parameter>::const_iterator begin() const { return parameters_.begin(); };
    std::vector<Parameter>::const_iterator end() const { return parameters_.end(); };
    bool empty() const { return parameters_.empty(); };
    Parameter& front() { return parameters_.front(); };
    const Parameter& front() const { return parameters_.front(); };
    Parameter& operator[](size_t index) { return parameters_[index]; };
    const Parameter& operator[](size_t index) const { return parameters_[index]; };
    Parameter& at(size_t index) { return parameters_[index]; };
    const Parameter& at(size_t index) const { return parameters_[index]; };

  protected:
    std::string function_name_;
    std::vector<Parameter> parameters_;
  };

  class ParameterSet
  {
  public:
    ParameterSet() = default;
    // from map/vector/map structure
    ParameterSet(const std::map<std::string, std::vector<std::map<std::string, std::string>>>& functions_map);
    // from OpenMS
    ParameterSet(const std::vector<OpenMS::DefaultParamHandler>& open_ms_params);

    /**
    Will merge with another ParameterSet
    */
    void merge(const ParameterSet& other);

    /**
    Find a parameter accross the ParameterSet, given a name and a Parameter name
    */
    Parameter* findParameter(const std::string& function_name, const std::string& parameter_name);

    /**
    Mark all parameters as schema parameter.
    */
    void setAsSchema(bool is_schema);

    void addFunctionParameters(FunctionParameters);
    void addParameter(const std::string& function_name, Parameter& parameter);

    // underlying map accessors
    void clear();
    FunctionParameters& at(const std::string& function_name) { return function_parameters_.at(function_name); };
    const FunctionParameters& at(const std::string& function_name) const { return function_parameters_.at(function_name); };
    size_t count(const std::string& function_name) const { return function_parameters_.count(function_name); };
    bool empty() const { return function_parameters_.empty(); };
    size_t size() const { return function_parameters_.size(); };
    FunctionParameters& operator[](const std::string& function_name) { return function_parameters_[function_name]; };
    std::map<std::string, FunctionParameters>::iterator begin() { return function_parameters_.begin(); };
    std::map<std::string, FunctionParameters>::iterator end() { return function_parameters_.end(); };
    std::map<std::string, FunctionParameters>::const_iterator begin() const { return function_parameters_.begin(); };
    std::map<std::string, FunctionParameters>::const_iterator end() const { return function_parameters_.end(); };

  protected:
    std::map<std::string, FunctionParameters> function_parameters_;
  };
  
}
