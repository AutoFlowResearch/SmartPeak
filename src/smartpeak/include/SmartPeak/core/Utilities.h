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

    struct CastValue
    {
      CastValue() : tag(UNKNOWN), s() {}

      CastValue(const CastValue& other)
      {
        *this = other;
      }

      CastValue& operator=(const CastValue& other)
      {
        if (this == &other)
          return *this;
        clear();
        switch (other.tag) {
          case UNKNOWN:
          case STRING:
            setData(other.s);
            break;
          case BOOL:
            setData(other.b);
            break;
          case FLOAT:
            setData(other.f);
            break;
          case INT:
            setData(other.i);
            break;
          case BOOL_LIST:
            setData(other.bl);
            break;
          case FLOAT_LIST:
            setData(other.fl);
            break;
          case INT_LIST:
            setData(other.il);
            break;
          case STRING_LIST:
            setData(other.sl);
            break;
        }
      }

      void setData(const bool data)
      {
        clear();
        tag = BOOL;
        b = data;
      }

      void setData(const float data)
      {
        clear();
        tag = FLOAT;
        f = data;
      }

      void setData(const int data)
      {
        clear();
        tag = INT;
        i = data;
      }

      void setData(const std::string& data)
      {
        clear();
        new (&s) std::string;
        tag = STRING;
        s = data;
      }

      void setData(const std::vector<bool>& data)
      {
        clear();
        new (&bl) std::vector<bool>;
        tag = BOOL_LIST;
        bl = data;
      }

      void setData(const std::vector<float>& data)
      {
        clear();
        new (&fl) std::vector<float>;
        tag = FLOAT_LIST;
        fl = data;
      }

      void setData(const std::vector<int>& data)
      {
        clear();
        new (&il) std::vector<int>;
        tag = INT_LIST;
        il = data;
      }

      void setData(const std::vector<std::string>& data)
      {
        clear();
        new (&sl) std::vector<std::string>;
        tag = STRING_LIST;
        sl = data;
      }

      ~CastValue()
      {
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
        // tag = UNKNOWN;
        // new (&s) std::string;
        // s = "";
      }

      enum {
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
