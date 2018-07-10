/**TODO:  Add copyright*/

#ifndef SMARTPEAK_CSVWRITER_H
#define SMARTPEAK_CSVWRITER_H

#include <unsupported/Eigen/CXX11/Tensor>

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
    CSVWriter(); ///< Default constructor
    ~CSVWriter(); ///< Default destructor
    CSVWriter(const std::string& filename, std::string delm = ",");

    void setFilename(const std::string& filename); ///< filename setter
    std::string getFilename() const; ///< filename getter

    void setDelimeter(const std::string& delimeter); ///< delimeter setter
    std::string getDelimeter() const; ///< delimeter getter

    void setLineCount(const int& line_count); ///< line_count setter
    int getLineCount() const; ///< line_count getter
 
    /**
      @brief This Function accepts a range and appends all the elements in the range
        to the last row, seperated by delimeter (Default is comma)

      @param first Iterator to the first element
      @param last Iterator to the last element
    */ 
    template<typename T>
    void writeDataInRow(T first, T last)
    {
      std::fstream file;
      // Open the file in truncate mode if first line else in Append Mode
      file.open(filename_, std::ios::out | (line_count_ ? std::ios::app : std::ios::trunc));
    
      // Iterate over the range and add each lement to file seperated by delimeter.
      for (; first != last; )
      {
        file << *first;
        if (++first != last)
          file << delimeter_;
      }
      file << "\n";
      line_count_++;
    
      // Close the file
      file.close();
    }
private:
    std::string filename_;
    std::string delimeter_;
    int line_count_;
  };
}

#endif //SMARTPEAK_CSVWRITER_H