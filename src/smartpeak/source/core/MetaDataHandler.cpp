/**TODO:  Add copyright*/

#include <SmartPeak/core/MetaDataHandler.h>

#include <vector>
#include <algorithm>

namespace SmartPeak
{    
  void MetaDataHandler::setSampleName(const std::string& sample_name_I)
  {
    sample_name_ = sample_name_I;
  }

  std::string MetaDataHandler::getSampleName() const
  {
    return sample_name_;
  }

  void MetaDataHandler::setSampleGroupName(const std::string& sample_group_name_I)
  {
    sample_group_name_ = sample_group_name_I;
  }
    
  std::string MetaDataHandler::getSampleGroupName() const
  {
    return sample_group_name_;
  }

  void MetaDataHandler::setSequenceSegmentName(const std::string& sequence_segment_name_I)
  {
    sequence_segment_name_ = sequence_segment_name_I;
  }
    
  std::string MetaDataHandler::getSequenceSegmentName() const
  {
    return sequence_segment_name_;
  }

  void MetaDataHandler::setFilename(const std::string& filename_I)
  {
    filename_ = filename_I;
  }
    
  std::string MetaDataHandler::getFilename() const
  {
    return filename_;
  }
    
  void MetaDataHandler::setSampleType(const std::string& sample_type_I)
  {
    sample_type_ = sample_type_I;
  }

  std::string MetaDataHandler::getSampleType() const
  {
    return sample_type_;
  }

  bool MetaDataHandler::checkSampleType(const std::string& sample_type_I)
  {
    std::vector<std::string> sample_types = {
      "Unknown", "Standard", "QC", "Blank", "Double Blank", "Solvent"};

    if (std::count(sample_types.begin(), sample_types.end(), sample_type_I) == 0)
      return false;
    else
      return true;
  }
}