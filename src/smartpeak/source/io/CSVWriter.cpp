/**TODO:  Add copyright*/

#include <SmartPeak/io/CSVWriter.h>

#include <fstream>

namespace SmartPeak
{  
  CSVWriter::CSVWriter(const std::string& filename, std::string delm) :
      filename_(filename), delimeter_(delm), line_count_(0)
  {} 
  
  void CSVWriter::setFilename(const std::string& filename)
  {
    filename_ = filename;    
  }
  std::string CSVWriter::getFilename() const
  {
    return filename_;
  }
  
  void CSVWriter::setDelimeter(const std::string& delimeter)
  {
    delimeter_ = delimeter;    
  }
  std::string CSVWriter::getDelimeter() const
  {
    return delimeter_;
  }

  void CSVWriter::setLineCount(const int& line_count)
  {
    line_count_ = line_count;    
  }
  int CSVWriter::getLineCount() const
  {
    return line_count_;
  }
}