// TODO: Add copyright

#pragma once

#include <OpenMS/DATASTRUCTURES/Param.h>
#include <iostream>
#include <regex>
#define maxFunc(a,b) (((a) > (b)) ? (a) : (b))

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
      CastValue() : tag_(UNINITIALIZED), b_(false), is_clear_(true) {}
      CastValue(const std::string& s) : tag_(STRING), s_(s), is_clear_(false) {}
      CastValue(const char *s) : tag_(STRING), s_(s), is_clear_(false) {}
      CastValue(const float f) : tag_(FLOAT), f_(f), is_clear_(true) {}

      CastValue(const CastValue& other) : tag_(UNINITIALIZED), b_(false), is_clear_(true)
      {
        *this = other;
      }

      CastValue(CastValue&& other) : tag_(UNINITIALIZED), b_(false), is_clear_(true)
      {
        *this = std::move(other);
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
          case UNINITIALIZED:
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
            throw "Tag type not managed in copy assignment constructor. Implement it.";
        }
        return *this;
      }

      CastValue& operator=(CastValue&& other)
      {
        if (this == &other)
          return *this;
        clear();
        switch (other.tag_) {
          case UNKNOWN:
          case STRING:
            new (&s_) std::string(std::move(other.s_));
            is_clear_ = false;
            break;
          case UNINITIALIZED:
          case BOOL:
            b_ = other.b_;
            break;
          case FLOAT:
            f_ = other.f_;
            break;
          case INT:
            i_ = other.i_;
            break;
          case BOOL_LIST:
            new (&bl_) std::vector<bool>(std::move(other.bl_));
            is_clear_ = false;
            break;
          case FLOAT_LIST:
            new (&fl_) std::vector<float>(std::move(other.fl_));
            is_clear_ = false;
            break;
          case INT_LIST:
            new (&il_) std::vector<int>(std::move(other.il_));
            is_clear_ = false;
            break;
          case STRING_LIST:
            new (&sl_) std::vector<std::string>(std::move(other.sl_));
            is_clear_ = false;
            break;
          default:
            throw "Tag type not managed in move assignment operator. Implement it.";
        }
        tag_ = other.tag_;
        other.tag_ = UNINITIALIZED;
        other.is_clear_ = true;
        other.b_ = false;
        return *this;
      }

      CastValue& operator=(const bool data)
      {
        setTagAndData(BOOL, data);
        return *this;
      }

      CastValue& operator=(const float data)
      {
        setTagAndData(FLOAT, data);
        return *this;
      }

      CastValue& operator=(const int data)
      {
        setTagAndData(INT, data);
        return *this;
      }

      CastValue& operator=(const char *data)
      {
        setTagAndData(STRING, std::move(std::string(data)));
        return *this;
      }

      CastValue& operator=(const std::string& data)
      {
        setTagAndData(STRING, data);
        return *this;
      }

      CastValue& operator=(const std::vector<bool>& data)
      {
        setTagAndData(BOOL_LIST, data);
        return *this;
      }

      CastValue& operator=(const std::vector<float>& data)
      {
        setTagAndData(FLOAT_LIST, data);
        return *this;
      }

      CastValue& operator=(const std::vector<int>& data)
      {
        setTagAndData(INT_LIST, data);
        return *this;
      }

      CastValue& operator=(const std::vector<std::string>& data)
      {
        setTagAndData(STRING_LIST, data);
        return *this;
      }

      ~CastValue()
      {
        clear();
      }

      void clear()
      {
        if (is_clear_)
          return;

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
        UNINITIALIZED,
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
        is_clear_ = false;
      }

      void setData(const std::vector<bool>& data)
      {
        new (&bl_) std::vector<bool>(data);
        is_clear_ = false;
      }

      void setData(const std::vector<float>& data)
      {
        new (&fl_) std::vector<float>(data);
        is_clear_ = false;
      }

      void setData(const std::vector<int>& data)
      {
        new (&il_) std::vector<int>(data);
        is_clear_ = false;
      }

      void setData(const std::vector<std::string>& data)
      {
        new (&sl_) std::vector<std::string>(data);
        is_clear_ = false;
      }

      Type tag_;
      bool is_clear_;

    public:
      CastValue::Type getTag() const { return tag_; }
    };

    /**
      Cast a string to the desired type and return the evaluation

      The type check is case insensitive.

      @note a value "true" or "false" of type "string" will be converted to bool

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

    static void splitString(
      const std::string& s,
      const char sep,
      std::vector<std::string>& out
    );

    template<typename T>
    static std::map<std::string, float> calculateValidationMetrics(
      const std::vector<T>& y_true,
      const std::vector<T>& y_pred,
      const bool verbose_I = false
    )
    {
      const std::array<size_t, 4> conf = computeConfusionMatrix(y_true, y_pred, verbose_I);
      const size_t TP = conf[0];
      const size_t FP = conf[1];
      const size_t FN = conf[2];
      const size_t TN = conf[3];
      const float accuracy = (TP + TN) / static_cast<float>(TP + FP + FN + TN);
      const float recall = TP / static_cast<float>(TP + FN);
      const float precision = TP / static_cast<float>(TP + FP);

      return {
        {"accuracy", accuracy},
        {"recall", recall},
        {"precision", precision},
      };
    }

    template<typename T>
    static std::array<size_t, 4> computeConfusionMatrix(
      const std::vector<T>& y_true,
      const std::vector<T>& y_pred,
      const bool verbose_I = false
    )
    {
      if (y_true.size() != y_pred.size())
        throw std::invalid_argument("Sizes don't match.");

      // Ref: https://en.wikipedia.org/wiki/Confusion_matrix

      std::array<size_t, 4> conf = {0, 0, 0, 0}; // TP, FP, FN, TN
      size_t& TP = conf[0];
      size_t& FP = conf[1];
      size_t& FN = conf[2];
      size_t& TN = conf[3];
      typename std::vector<T>::const_iterator a = y_true.cbegin(); // a = actual
      typename std::vector<T>::const_iterator p = y_pred.cbegin(); // p = predicted

      for (; a != y_true.cend(); ++a, ++p) {
        if (*p) { // positives
          if (*a)
            ++TP;
          else
            ++FP;
        } else {  // negatives
          if (*a)
            ++FN;
          else
            ++TN;
        }
      }

      if (verbose_I)
        std::cout << "Confusion matrix: [TP, FP, FN, TN] = [" << TP << ", " << FP << ", " << FN << ", " << TN << "]" << std::endl;

      return conf;
    }

    /**
      @brief Test absolute and relative closeness of values

      References: http://realtimecollisiondetection.net/blog/?p=89

      @param: lhs Left Hand Side to compare
      @param: rhs Right Hand Side to Compare
      @param: rel_tol Relative Tolerance threshold
      @param: abs_tol Absolute Tolerance threshold

      @returns True or False
    */
    template<typename T>
    bool assert_close(const T& lhs, const T& rhs, T rel_tol = 1e-4, T abs_tol = 1e-4) {
      return (std::fabs(lhs - rhs) <= maxFunc(abs_tol, rel_tol * maxFunc(std::fabs(lhs), std::fabs(rhs)) ));
    }
  };
}
