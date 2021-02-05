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

#include <ostream>
#include <string>
#include <vector>
#include <plog/Log.h>

namespace SmartPeak
{
  class CastValue
  {
  public:
    enum class Type {
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

    CastValue() : b_(false), tag_(Type::UNINITIALIZED), is_clear_(true) {}
    CastValue(const std::string& s) : s_(s), tag_(Type::STRING), is_clear_(false) {}
    CastValue(const char *s) : s_(s), tag_(Type::STRING), is_clear_(false) {}
    CastValue(const float f) : f_(f), tag_(Type::FLOAT), is_clear_(true) {}
    CastValue(const int i) : i_(i), tag_(Type::INT), is_clear_(true) {}
    CastValue(const long int li) : li_(li), tag_(Type::LONG_INT), is_clear_(true) {}
    CastValue(const bool b) : b_(b), tag_(Type::BOOL), is_clear_(true) {}
    CastValue(const std::vector<bool>& bl) : bl_(bl), tag_(Type::BOOL_LIST), is_clear_(false) {}
    CastValue(const std::vector<float>& fl) : fl_(fl), tag_(Type::FLOAT_LIST), is_clear_(false) {}
    CastValue(const std::vector<int>& il) : il_(il), tag_(Type::INT_LIST), is_clear_(false) {}
    CastValue(const std::vector<std::string>& sl) : sl_(sl), tag_(Type::STRING_LIST), is_clear_(false) {}

    CastValue(const CastValue& other) : b_(false), tag_(Type::UNINITIALIZED), is_clear_(true)
    {
      *this = other;
    }

    CastValue(CastValue&& other) : b_(false), tag_(Type::UNINITIALIZED), is_clear_(true)
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

    /**
     * Comparison operator will use case sensitive, use dedicated
     * methods (is_less_than, is_greater_than) for non case sensitive.
     */
    bool operator<(const CastValue& other) const;
    bool operator>(const CastValue& other) const;
    bool operator==(const CastValue& other) const;
    bool operator>=(const CastValue& other) const;
    bool operator<=(const CastValue& other) const;

    operator std::string() const
    {
      std::ostringstream oss;
      oss << *this;
      return oss.str();
    }

    // TODO: rename to deallocate() or similar
    void clear();

    bool is_less_than(const CastValue& other, const bool case_sensitive = true) const;
    bool is_greater_than(const CastValue& other, const bool case_sensitive = true) const;
    bool is_equal_to(const CastValue& other, const bool case_sensitive = true) const;

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
      const CastValue::Type tag = cv.getTag();
      switch (tag) {
        case CastValue::Type::UNKNOWN:
        case CastValue::Type::STRING:
          os << cv.s_;
          break;
        case CastValue::Type::UNINITIALIZED:
        case CastValue::Type::BOOL:
          os << std::boolalpha << cv.b_;
          break;
        case CastValue::Type::FLOAT:
          os << cv.f_;
          break;
        case CastValue::Type::INT:
          os << cv.i_;
          break;
        case CastValue::Type::LONG_INT:
          os << cv.li_;
          break;
        case CastValue::Type::BOOL_LIST:
          {
            os << "[";
            std::string sep = "";
            for (const auto& v : cv.bl_)
            {
              os << sep << std::boolalpha << v;
              sep = ",";
            }
            os << "]";
          }
          break;
        case CastValue::Type::FLOAT_LIST:
          {
            os << "[";
            std::string sep = "";
            for (const auto& v : cv.fl_)
            {
              os << sep << v;
              sep = ",";
            }
            os << "]";
          }
          break;
        case CastValue::Type::INT_LIST:
          {
            os << "[";
            std::string sep = "";
            for (const auto& v : cv.il_)
            {
              os << sep << v;
              sep = ",";
            }
            os << "]";
          }
          break;
        case CastValue::Type::STRING_LIST:
          {
            os << "[";
            std::string sep = "";
            for (const auto& v : cv.sl_)
            {
              os << sep << "'" << v << "'";
              sep = ",";
            }
            os << "]";
          }
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
