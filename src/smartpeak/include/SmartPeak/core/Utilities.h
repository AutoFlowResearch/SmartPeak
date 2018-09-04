// TODO: Add copyright

#pragma once

#include <regex>
#include <OpenMS/DATASTRUCTURES/Param.h>
#include <iostream> // TODO: remove this

namespace SmartPeak
{
  class Utilities
  {
public:
    Utilities() = delete;
    ~Utilities() = delete;

    class CastValue
    {
    public:
      CastValue() : tag_(UNKNOWN), s_(), is_clear_(false) {}

      CastValue(const CastValue& other)
      {
        *this = other;
      }

      CastValue& operator=(const CastValue& other)
      {
        if (this == &other)
          return *this;
        switch (other.tag_) {
          case UNKNOWN:
          case STRING:
            setTagAndData(other.tag_, other.s_);
            break;
          case BOOL:
            setTagAndData(other.tag_, other.b_);
            break;
          case FLOAT:
            setTagAndData(other.tag_, other.f_);
            break;
          case INT:
            setTagAndData(other.tag_, other.i_);
            break;
          case BOOL_LIST:
            setTagAndData(other.tag_, other.bl_);
            break;
          case FLOAT_LIST:
            setTagAndData(other.tag_, other.fl_);
            break;
          case INT_LIST:
            setTagAndData(other.tag_, other.il_);
            break;
          case STRING_LIST:
            setTagAndData(other.tag_, other.sl_);
            break;
          default:
            throw "Tag type not managed in copy constructor. Implement it.";
        }
      }

      CastValue& operator=(const bool data)
      {
        setTagAndData(BOOL, data);
      }

      CastValue& operator=(const float data)
      {
        setTagAndData(FLOAT, data);
      }

      CastValue& operator=(const int data)
      {
        setTagAndData(INT, data);
      }

      CastValue& operator=(const std::string& data)
      {
        setTagAndData(STRING, data);
      }

      CastValue& operator=(const std::vector<bool>& data)
      {
        setTagAndData(BOOL_LIST, data);
      }

      CastValue& operator=(const std::vector<float>& data)
      {
        setTagAndData(FLOAT_LIST, data);
      }

      CastValue& operator=(const std::vector<int>& data)
      {
        setTagAndData(INT_LIST, data);
      }

      CastValue& operator=(const std::vector<std::string>& data)
      {
        setTagAndData(STRING_LIST, data);
      }

      ~CastValue()
      {
        if (!is_clear_)
          clear();
      }

      void clear()
      {
        switch (tag_) {
          case UNKNOWN:
          case STRING:
            s_.~basic_string();
            break;
          case BOOL_LIST:
            bl_.~vector();
            break;
          case FLOAT_LIST:
            fl_.~vector();
            break;
          case INT_LIST:
            il_.~vector();
            break;
          case STRING_LIST:
            sl_.~vector();
            break;
        }
        is_clear_ = true;
      }

      enum Type {
        UNKNOWN,
        BOOL,
        FLOAT,
        INT,
        STRING,
        BOOL_LIST,
        FLOAT_LIST,
        INT_LIST,
        STRING_LIST
      };

      union {
        bool b_;
        float f_;
        int i_;
        std::string s_;
        std::vector<bool> bl_;
        std::vector<float> fl_;
        std::vector<int> il_;
        std::vector<std::string> sl_;
      };

      template<typename T>
      void setTagAndData(const CastValue::Type type, const T& data)
      {
        clear();
        tag_ = type;
        setData(data);
        is_clear_ = false;
      }

    private:
      void setData(const bool data)
      {
        b_ = data;
      }

      void setData(const float data)
      {
        f_ = data;
      }

      void setData(const int data)
      {
        i_ = data;
      }

      void setData(const std::string& data)
      {
        new (&s_) std::string(data);
      }

      void setData(const std::vector<bool>& data)
      {
        new (&bl_) std::vector<bool>(data);
      }

      void setData(const std::vector<float>& data)
      {
        new (&fl_) std::vector<float>(data);
      }

      void setData(const std::vector<int>& data)
      {
        new (&il_) std::vector<int>(data);
      }

      void setData(const std::vector<std::string>& data)
      {
        new (&sl_) std::vector<std::string>(data);
      }

      Type tag_;
      bool is_clear_;

    public:
      CastValue::Type getTag() const { return tag_; }
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
    );

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
    );

    // """Parse string and return the eval
    
    // Args:
    //     str_I (str): input string
    //     encode_str_I (bool): encode string as utf-8?
        
    // Returns:
    //     str: str_O: evaluated string
        
    // """
    static void parseString(const std::string& str_I, CastValue& cast);

    static void parseList(const std::string& line, std::regex& re, CastValue& cast);
  };
}
