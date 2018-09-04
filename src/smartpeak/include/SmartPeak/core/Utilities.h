// TODO: Add copyright

#pragma once

#include <regex>
#include <OpenMS/DATASTRUCTURES/Param.h>

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
      CastValue() : tag(UNKNOWN), s(), is_clear(false) {}

      CastValue(const CastValue& other)
      {
        *this = other;
      }

      CastValue& operator=(const CastValue& other)
      {
        if (this == &other)
          return *this;
        switch (other.tag) {
          case UNKNOWN:
          case STRING:
            setTagAndData(other.tag, other.s);
            break;
          case BOOL:
            setTagAndData(other.tag, other.b);
            break;
          case FLOAT:
            setTagAndData(other.tag, other.f);
            break;
          case INT:
            setTagAndData(other.tag, other.i);
            break;
          case BOOL_LIST:
            setTagAndData(other.tag, other.bl);
            break;
          case FLOAT_LIST:
            setTagAndData(other.tag, other.fl);
            break;
          case INT_LIST:
            setTagAndData(other.tag, other.il);
            break;
          case STRING_LIST:
            setTagAndData(other.tag, other.sl);
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
        if (!is_clear)
          clear();
      }

      void clear()
      {
        switch (tag) {
          case UNKNOWN:
          case STRING:
            s.~basic_string();
            break;
          case BOOL_LIST:
            bl.~vector();
            break;
          case FLOAT_LIST:
            fl.~vector();
            break;
          case INT_LIST:
            il.~vector();
            break;
          case STRING_LIST:
            sl.~vector();
            break;
        }
        is_clear = true;
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
      } tag;

      union {
        bool b;
        float f;
        int i;
        std::string s;
        std::vector<bool> bl;
        std::vector<float> fl;
        std::vector<int> il;
        std::vector<std::string> sl;
      };

    private:
      template<typename T>
      void setTagAndData(const CastValue::Type type, const T& data)
      {
        clear();
        tag = type;
        setData(data);
        is_clear = false;
      }

      void setData(const bool data)
      {
        b = data;
      }

      void setData(const float data)
      {
        f = data;
      }

      void setData(const int data)
      {
        i = data;
      }

      void setData(const std::string& data)
      {
        new (&s) std::string(data);
      }

      void setData(const std::vector<bool>& data)
      {
        new (&bl) std::vector<bool>(data);
      }

      void setData(const std::vector<float>& data)
      {
        new (&fl) std::vector<float>(data);
      }

      void setData(const std::vector<int>& data)
      {
        new (&il) std::vector<int>(data);
      }

      void setData(const std::vector<std::string>& data)
      {
        new (&sl) std::vector<std::string>(data);
      }

      bool is_clear;
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
