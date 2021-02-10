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
// $Authors: Douglas McCloskey $
// --------------------------------------------------------------------------

#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <plog/Log.h>

namespace SmartPeak
{

  /**
    @brief CSVWriter

    based on the following:
        http://thispointer.com/how-to-write-data-in-a-csv-file-in-c/
  */
  class CSVWriter
  {
public:
    CSVWriter(const std::string& filename, const std::string& delm = ",") :
      filename_(filename), delimeter_(delm) {}
    CSVWriter()                            = default; ///< Default constructor
    ~CSVWriter()                           = default; ///< Default destructor
    CSVWriter(const CSVWriter&)            = default;
    CSVWriter& operator=(const CSVWriter&) = default;
    CSVWriter(CSVWriter&&)                 = default;
    CSVWriter& operator=(CSVWriter&&)      = default;

    void setFilename(const std::string& filename); ///< filename setter
    std::string getFilename() const; ///< filename getter

    void setDelimeter(const std::string& delimeter); ///< delimeter setter
    std::string getDelimeter() const; ///< delimeter getter

    void setLineCount(const int line_count); ///< line_count setter
    int getLineCount() const; ///< line_count getter

    /**
      @brief This Function accepts a range and appends all the elements in the range
        to the last row, seperated by delimeter (Default is comma)

      @param first Iterator to the first element
      @param last Iterator to the last element
    */
    template<typename T>
    size_t writeDataInRow(T first, T last)
    {
      // Open the file in truncate mode if first line, else in append mode
      std::ofstream ofs(filename_, line_count_ ? std::ios::app : std::ios::trunc);

      if (!ofs.is_open()) {
        LOGE << "Cannot open file: " << filename_;
        return -1;
      }

      size_t cnt {0};

      if (first != last) {
        ofs << *first++;
        ++cnt;
      }
      while (first != last) {
        ofs << delimeter_ << *first++;
        ++cnt;
      }

      ofs << "\n";
      ++line_count_;
      return cnt;
    }

private:
    std::string filename_;
    std::string delimeter_;
    int line_count_ = 0;
  };
}