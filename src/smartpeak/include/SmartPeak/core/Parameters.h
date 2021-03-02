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

    /**
      @brief uninitialized parameter constructor
    */
    Parameter(const std::string& name)
      : name_(name), value_() { };
    /**
      @brief constructor from CastValue
    */
    Parameter(const std::string& name, const CastValue& value)
      : name_(name), value_(value) { };
    /**
      @brief constructor from map

      example of structure:

      {
        {"name", "param1"},
        {"type", "int"},
        {"value", "23"},
        {"description", "param1 description"},
        {"tags", "tag1,tag2,tag3"}
        {"min","0"}, 
        {"max","10"}
      }

    */
    Parameter(const std::map<std::string, std::string>& properties);

    /**
      @brief returns the type of parameter as string
    */
    std::string getType() const;

    /**
      @brief based constraints of this parameter, return wether the parameter is valid.

      @param[in] use_scheme use a schema to validate the value, if one is assigned to the parameter.
    */
    bool isValid(bool use_scheme = true) const;

    /**
      @brief based constraints of this parameter, return wether the parameter is valid.

      @param[in] value a CastValue to valid with the constraints of this parameter.
      @param[in] use_scheme use a schema's constraint to validate the value, if one is assigned to the parameter.
    */
    bool isValid(const CastValue& value, bool use_scheme = true) const;


    /**
      @brief set name of the parameter.

      @param[in] name the name of the parameter.
    */
    void setName(const std::string& name) { name_ = name; };

    /**
      @brief get name of the parameter.
    */
    const std::string& getName() const { return name_; };

    /**
      @brief set constraints min and max.
    */
    void setConstraintsMinMax(const std::shared_ptr<CastValue>& min, const std::shared_ptr<CastValue>& max) { constraints_min_ = min; constraints_max_ = max; };

    /**
      @brief set constraints list.
    */
    void setConstraintsList(const std::shared_ptr<std::vector<CastValue>>& list) { constraints_list_ = list; };

    /**
      @brief set value of the parameter, from a string representation.
    */
    void setValueFromString(const std::string& value_as_string);

    /**
      @brief return value of the parameter.
    */
    const std::string getValueAsString() const { return std::string(value_); };

    /**
      @brief sets description of the parameter.
    */
    void setDescription(const std::string& description) { description_ = description; };

    /**
      @brief get the description of the parameter.

      @param[in] use_scheme if one schema is associated, use it as description
    */
    const std::string& getDescription(bool use_scheme=true) const { return (use_scheme && schema_) ? schema_->getDescription() : description_; };

    /**
      @brief sets schema to this parameter.
    */
    void setSchema(const Parameter& schema) { schema_ = std::make_shared<Parameter>(schema); }

    /**
      @brief returns schema if this parameter.
    */
    const Parameter* getSchema() const { return schema_.get(); };

    /**
      @brief set tags.

      @param[in] tags the list of tags
    */
    void setTags(const std::vector<std::string>& tags) { tags_ = tags; };

    /**
      @brief get tags.

      @param[in] use_scheme if set, and if a schema is assigned to this parameter, returns the tags of the schema
    */
    const std::vector<std::string>& getTags(bool use_scheme = true) const { return (use_scheme && schema_) ? schema_->getTags() : tags_; };

    /**
      @brief get restrictions of the parameter, as a string representation.

      @param[in] use_scheme if set, and if a schema is assigned to this parameter, returns the restriction of the schema.
    */
    const std::string getRestrictionsAsString(bool use_scheme = true) const;

    /**
      @brief set parameter as schema.
    */
    void setAsSchema(bool is_schema) { is_schema_ = is_schema; };

    /**
      @brief return wether the parameter is schema.
    */
    bool isSchema() const { return is_schema_; };


    /**
      @brief return default value of the parameter (if schema is assigned).
    */
    const std::string getDefaultValueAsString() const { return schema_ ? schema_->getValueAsString() : getValueAsString(); };

    bool operator==(const Parameter & other) const;
    inline bool operator!=(const Parameter& other) const { return !operator==(other); };

  protected:
    friend class Utilities;
    const CastValue& getValue() const { return value_; };

  private:
    // utility method to ease comparison between different types
    // return false if it cannot be converted
    bool getFloatValue(const CastValue& value, float& result) const;

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
    /**
    @brief default constructor
    */
    FunctionParameters() = default;
    /**
    @brief construct empty, named FunctionParameter

    @param[in] function_name the function name
    */
    FunctionParameters(const std::string& function_name) : function_name_(function_name) {};
    /**
    @brief construct from a list of map

    @param[in] function_name the function name
    @param[in] parameters_list list of parmater (map representation)
    */
    FunctionParameters(const std::string& function_name, const std::vector<std::map<std::string, std::string>>& parameters_list);
    /**
    @brief construct from OpenMS ParameterHandler

    @param[in] param_handler the OpenMS ParameterHandler to convert
    */
    FunctionParameters(const OpenMS::DefaultParamHandler& param_handler);

    /**
    @brief Mark all parameters as schema parameter.

    @param[in] is_schema set as schema (true) or not (false)
    */
    void setAsSchema(bool is_schema);

    Parameter* findParameter(const std::string& parameter);
    const std::string& getFunctionName() const { return function_name_; };

    /**
    @brieg Add a parameter. Doesn't Replace if already exists

    @param[in] parameter the parameter to add
    */
    void addParameter(const Parameter& parameter);

    /**
    @brief Merge two FunctionsParameters.
    - If one parameter already exists in the other, use it as Schema for the other parameter.
    - If the parameter does not exists, just add.

    @param[in] other the FunctionParameter to merge with
    */
    void merge(const FunctionParameters& other);

    bool operator==(const FunctionParameters& other) const;
    inline bool operator!=(const FunctionParameters& other) const { return !operator==(other); };

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
    /**
    @brief default constructor
    */
    ParameterSet() = default;
    /**
    @brief construct from from map/vector/map structure
    */
    ParameterSet(const std::map<std::string, std::vector<std::map<std::string, std::string>>>& functions_map);
    /**
    @brief construct list of OpenMS Parameter Handlers
    */
    ParameterSet(const std::vector<OpenMS::DefaultParamHandler>& open_ms_params);

    /**
    @brief Will merge with another ParameterSet

    @param[in] other the ParameterSet to merge with
    */
    void merge(const ParameterSet& other);

    /**
    @brief Find a parameter accross the ParameterSet, given a name and a Parameter name

    @param[in] function_name the function name to search
    @param[in] parameter_name the parameter name to search
    */
    Parameter* findParameter(const std::string& function_name, const std::string& parameter_name);

    /**
    @brief Mark all parameters as schema parameter.

    @param[in] is_schema set as schema (true) or not (false)
    */
    void setAsSchema(bool is_schema);

    /**
    @brief Add FunctionParameter to the ParameterSet. If the function already exists, it will be replaced

    @param[in] parameters the FunctionParameters to add
    */
    void addFunctionParameters(FunctionParameters parameters);

    /**
    @brief Add a Parameter to a FunctionParameter. 
    If the parameter already exists, it will be replaced.
    If the FunctionParameter does not exists, it will be created.

    @param[in] function_name the function name to add to
    @param[in] parameter the parameter to add
    */
    void addParameter(const std::string& function_name, Parameter& parameter);

    bool operator==(const ParameterSet& other) const;
    inline bool operator!=(const ParameterSet& other) const { return !operator==(other); };

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
