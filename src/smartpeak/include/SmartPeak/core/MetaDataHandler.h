/**TODO:  Add copyright*/

#pragma once

#include <string>

namespace SmartPeak
{

  class MetaDataHandler
  {
public:    
    MetaDataHandler() = default;  ///< Default constructor
    ~MetaDataHandler() = default;  ///< Default destructor    
    
    void setSampleName(const std::string& sample_name_I);
    std::string getSampleName() const;

    void setSampleGroupName(const std::string& sample_group_name_I);      
    std::string getSampleGroupName() const;

    void setSequenceSegmentName(const std::string& sequence_segment_name_I);      
    std::string getSequenceSegmentName() const;

    void setFilename(const std::string& filename_I);      
    std::string getFilename() const;
      
    void setSampleType(const std::string& sample_type_I);
    std::string getSampleType() const;

    static bool checkSampleType(const std::string& sample_type_I);

    void clear();

private:
    std::string sample_name_ = "";
    std::string sample_group_name_ = "";
    std::string sequence_segment_name_ = "";
    std::string filename_ = "";
    std::string sample_type_ = ""; // [TODO: Refactor to use ENUM class]
  };
}