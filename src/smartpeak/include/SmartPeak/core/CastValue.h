// TODO: Add copyright

#pragma once

#include <ostream>
#include <string>
#include <vector>
#include <plog/Log.h>

namespace SmartPeak
{
  class CastValue
  {
  public:
    enum Type {
      UNINITIALIZED,
      UNKNOWN,
      BOOL,
      FLOAT,
      INT,
      LONG_INT,
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
      long int li_;
      std::string s_;
      std::vector<bool> bl_;
      std::vector<float> fl_;
      std::vector<int> il_;
      std::vector<std::string> sl_;
    };

    CastValue() : b_(false), tag_(UNINITIALIZED), is_clear_(true) {}
    CastValue(const std::string& s) : s_(s), tag_(STRING), is_clear_(false) {}
    CastValue(const char *s) : s_(s), tag_(STRING), is_clear_(false) {}
    CastValue(const float f) : f_(f), tag_(FLOAT), is_clear_(true) {}
    CastValue(const int i) : i_(i), tag_(INT), is_clear_(true) {}
    CastValue(const long int li) : li_(li), tag_(LONG_INT), is_clear_(true) {}

    CastValue(const CastValue& other) : b_(false), tag_(UNINITIALIZED), is_clear_(true)
    {
      *this = other;
    }

    CastValue(CastValue&& other) : b_(false), tag_(UNINITIALIZED), is_clear_(true)
    {
      *this = std::move(other);
    }

    ~CastValue()
    {
      clear();
    }

    CastValue& operator=(const CastValue& other);
    CastValue& operator=(CastValue&& other);
    CastValue& operator=(const bool data);
    CastValue& operator=(const float data);
    CastValue& operator=(const int data);
    CastValue& operator=(const long int data);
    CastValue& operator=(const char *data);
    CastValue& operator=(const std::string& data);
    CastValue& operator=(const std::vector<bool>& data);
    CastValue& operator=(const std::vector<float>& data);
    CastValue& operator=(const std::vector<int>& data);
    CastValue& operator=(const std::vector<std::string>& data);

    operator std::string() const
    {
      std::ostringstream oss(*this);
      return oss.str();
    }

    // TODO: rename to deallocate() or similar
    void clear();

    bool is_less_than(const CastValue& other, const bool case_sensitive = true);

    CastValue::Type getTag() const;

    template<typename T>
    void setTagAndData(const CastValue::Type type, const T& data)
    {
      clear();
      tag_ = type;
      setData(data);
    }

    friend std::ostream& operator<<(std::ostream& os, const CastValue& cv)
    {
      switch (cv.getTag()) {
        case cv.UNKNOWN:
        case cv.STRING:
          os << cv.s_;
          break;
        case cv.UNINITIALIZED:
        case cv.BOOL:
          os << cv.b_;
          break;
        case cv.FLOAT:
          os << cv.f_;
          break;
        case cv.INT:
          os << cv.i_;
          break;
        case cv.LONG_INT:
          os << cv.li_;
          break;
        default:
          throw "Tag type not managed in operator<<. Implement it.";
      }
      return os;
    }

  private:
    void setData(const bool data);
    void setData(const float data);
    void setData(const int data);
    void setData(const long int data);
    void setData(const std::string& data);
    void setData(const std::vector<bool>& data);
    void setData(const std::vector<float>& data);
    void setData(const std::vector<int>& data);
    void setData(const std::vector<std::string>& data);

    Type tag_;
    bool is_clear_;
  };
}
