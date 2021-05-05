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
// $Maintainer: Douglas McCloskey, Bertrand Boudaud $
// $Authors: Douglas McCloskey, Pasquale Domenico Colaianni $
// --------------------------------------------------------------------------

#pragma once

#include <SmartPeak/core/CastValue.h>
#include <SmartPeak/core/Parameters.h>
#include <SmartPeak/ui/ImEntry.h>
#include <OpenMS/ANALYSIS/OPENSWATH/MRMFeatureSelector.h>
#include <OpenMS/DATASTRUCTURES/Param.h>

#include <regex>
#include <string>

#ifndef CSV_IO_NO_THREAD
#define CSV_IO_NO_THREAD
#endif
#include <SmartPeak/io/csv.h>
#include <plog/Log.h>

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
      Update a OpenMS' DefaultParamHandler object with user parameters.

      @param[in,out] Param_handler_IO OpenMS' DefaultParamHandler object to update
      @param[in] parameters_I user parameters
      @param[in] param_handler_name set if user parameter have different name entry.
    */
    static void setUserParameters(
      OpenMS::DefaultParamHandler& Param_handler_IO,
      const ParameterSet& user_parameters_I,
      const std::string param_handler_name_I = ""
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
      const FunctionParameters& parameters_I
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
      const std::vector<T>& y_pred
    )
    {
      if (y_true.empty() || y_pred.empty()) {
        LOGD << "Actual size " << y_true.size() << ", predicted size " << y_pred.size();
        return {
          {"accuracy", 0},
          {"recall", 0},
          {"precision", 0},
          {"n_features", 0},
        };
      }

      const std::array<size_t, 4> conf = computeConfusionMatrix(y_true, y_pred);
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
        {"n_features", y_true.size()},
      };
    }

    template<typename T>
    static std::array<size_t, 4> computeConfusionMatrix(
      const std::vector<T>& y_true,
      const std::vector<T>& y_pred
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

      LOGD << "Confusion matrix: [TP, FP, FN, TN] = [" << TP << ", " << FP << ", " << FN << ", " << TN << "]";

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
      const FunctionParameters& params,
      const FunctionParameters& score_weights
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

    /**
      @brief Check if str ends with suffix

      Useful for filtering filenames by the extension

      @param[in] str Input string
      @param[in] suffix Suffix
      @param[in] case_sensitive Case sensitive string comparison

      @returns True if str ends with suffix. Otherwise false.
    */
    static bool endsWith(
      std::string str,
      std::string suffix,
      const bool case_sensitive = true
    );

    /**
      @brief Get information about name, size, type and modified date of all
      entries in a folder

      @note Elements "." and ".." are not part of the content

      @param[in] pathname Folder pathname
      @param[in] asc Sort entries in ascending order

      @returns A container with the desired information
    */
    static std::array<std::vector<std::string>, 4> getPathnameContent(
      const std::string& pathname,
      const bool asc = true
    );

    /**
      @brief Get parent pathname from a pathname string

      @param[in] pathname Input string pathname

      @returns A string representation of the parent pathname
    */
    static std::string getParentPathname(const std::string& pathname);

    /**
      @brief Moves the elements in vector v according to indices

      Example:
      indices: {3, 4, 2, 0, 1}
      old v:   {'a', 'b', 'c', 'd', 'e'}
      new v:   {'d', 'e', 'c', 'a', 'b'}

      @throws std::invalid_argument Sizes of indices and v differ

      @param[in] indices The vector of indices that decides the sorting
      @param[in,out] v The vector of elements to be sorted
    */
    template<typename T>
    static void sortPairs(
      const std::vector<size_t>& indices,
      std::vector<T>& v
    )
    {
      if (indices.size() != v.size()) {
        throw std::invalid_argument("sortPairs: arguments' sizes do not match");
      }
      const std::vector<T> cp(v); // a copy of v
      for (size_t i = 0; i < v.size(); ++i) {
        v[i] = cp[indices[i]];
      }
    }

    /**
      @brief Case-insensitive string comparison

      Equivalent to a.compare(b) < 0, ignoring letter case.

      @param[in] a left-side string
      @param[in] b right-side string

      @returns True if 'a' is lexicographically less than 'b'. Otherwise false.
    */
    static bool is_less_than_icase(const std::string& a, const std::string& b);

    /**
      @brief Count the number of elements in a folder

      @note Elements "." and ".." are not counted

      @param[in] pathname Pathname to a folder

      @returns The numbers of elements found
    */
    static size_t directorySize(const std::string& pathname);

    /**
    * @brief Constructs an absolute filepath to an application logs.
    * 
    * Default locations of logs:
    *   - Windows: C:\Users\<user>\AppData\Local\SmartPeak
    *   - Linux and MacOS: ~/.SmartPeak
    * User can change default location and specify directory where the logs are stored by
    * setting SMARTPEAK_LOGS env variable. If directory specified by the path doesn't exist, the function will create one.
    * 
    * @param[in] filename Log filename
    * @returns The absolute path to log file and boolean flag whether the path to directory was created
    */
    static std::pair<std::string, bool> getLogFilepath(const std::string& filename);

    /**
    * @brief Returns the build version of SmartPeak package if available.
    */
    static std::string getSmartPeakVersion();
    
    /**
     @brief Modify ImEntry to a human readable format.
     
     @param[in,out] directory_entry directory entry on which the modification is done.
    */
    static void makeHumanReadable(ImEntry& directory_entry);

    /**
     * Converts an input string to upper case string.
     * 
     * @param[in] str input string
     * @return formated string
    */
    static std::string str2upper(const std::string& str);
  };
}
