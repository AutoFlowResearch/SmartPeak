/**TODO:  Add copyright*/

#pragma once

#include <iostream>
#include <fstream>
#include <vector>

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
    int writeDataInRow(T first, T last)
    {
      // Open the file in truncate mode if first line, else in append mode
      std::ofstream ofs(filename_, line_count_ ? std::ios::app : std::ios::trunc);

      if (!ofs.is_open()) {
        std::cerr << "Cannot open file: " << filename_ << "\n";
        return -1;
      }

      int cnt {0};

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