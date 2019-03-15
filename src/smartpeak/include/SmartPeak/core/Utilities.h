// TODO: Add copyright

#pragma once

#include <OpenMS/ANALYSIS/OPENSWATH/MRMFeatureSelector.h>
#include <OpenMS/DATASTRUCTURES/Param.h>

#include <iomanip>
#include <ios>
#include <iostream>
#include <regex>
#include <string>
#include <unordered_set>

#ifndef CSV_IO_NO_THREAD
#define CSV_IO_NO_THREAD
#endif
#include <SmartPeak/io/csv.h>

#define maxFunc(a,b) (((a) > (b)) ? (a) : (b))

namespace SmartPeak
{
  class Utilities
  {
public:
    Utilities()                            = delete;
    ~Utilities()                           = delete;
    Utilities(const Utilities&)            = delete;
    Utilities& operator=(const Utilities&) = delete;
    Utilities(Utilities&&)                 = delete;
    Utilities& operator=(Utilities&&)      = delete;

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

      // TODO: rename to deallocate() or similar
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
          default:
            // nothing to deallocate
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

      Type tag_;

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

      CastValue::Type getTag() const { return tag_; }

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

      bool is_clear_;
    };

    /**
      Cast a string to the desired type and return the evaluation.

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

    /**
      Update a Param object.

      The type check is case insensitive.

      @note a value "true" or "false" of type "string" will be converted to bool

      @param[in,out] Param_IO OpenMS' Param object to update
      @param[in] parameters_I List of parameters to update
    */
    static void updateParameters(
      OpenMS::Param& Param_IO,
      const std::vector<std::map<std::string, std::string>>& parameters_I
    );

    /**
      Parse string and return the eval.

      The type check is case insensitive.

      @note a value "true" or "false" of type "string" will be converted to bool

      @param[in] str_I Input string
      @param[out] cast The evaluated string
    */
    static void parseString(const std::string& str_I, CastValue& cast);

    static void parseList(const std::string& line, std::regex& re, CastValue& cast);

    static std::vector<std::string> splitString(
      const std::string& s,
      const char sep
    );

    static std::string trimString(
      const std::string& s,
      const std::string& whitespaces = " \f\n\r\t\v" // std::isspace()
    );

    template<typename T>
    static std::map<std::string, float> calculateValidationMetrics(
      const std::vector<T>& y_true,
      const std::vector<T>& y_pred,
      const bool verbose_I = false
    )
    {
      if (y_true.empty() || y_pred.empty()) {
        throw std::invalid_argument("Actual and predicted values' vectors cannot be empty.");
      }

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
    static bool assert_close(const T& lhs, const T& rhs, double rel_tol = 1e-4, double abs_tol = 1e-4)
    {
      return (std::fabs(lhs - rhs) <= maxFunc(abs_tol, rel_tol * maxFunc(std::fabs(lhs), std::fabs(rhs)) ));
    }

    static std::vector<OpenMS::MRMFeatureSelector::SelectorParameters> extractSelectorParameters(
      const std::vector<std::map<std::string, std::string>>& params,
      const std::vector<std::map<std::string, std::string>>& score_weights
    );

    template<typename Iterator>
    static std::string join(Iterator first, Iterator last, const std::string& delimiter = "")
    {
      std::ostringstream oss;
      if (first != last) {
        oss << *first;
        for (Iterator it = 1 + first; it != last; ++it) {
          oss << delimiter << *it;
        }
      }
      return oss.str();
    }

    // To validate quantitation methods stored in examples' tests
    static bool testStoredQuantitationMethods(const std::string& pathname);
  };
}
