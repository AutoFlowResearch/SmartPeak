/**TODO:  Add copyright*/

#include <SmartPeak/core/MetaDataHandler.h>

#include <vector>
#include <algorithm>

namespace SmartPeak
{
  const char* MetaDataHandler::SampleTypeToString(SampleType sample_type)
  {
    switch (sample_type) {
      case SampleType::Unknown:
        return "Unknown";
      case SampleType::Standard:
        return "Standard";
      case SampleType::QC:
        return "QC";
      case SampleType::Blank:
        return "Blank";
      case SampleType::DoubleBlank:
        return "Double Blank";
      case SampleType::Solvent:
        return "Solvent";
    }
  }

  void MetaDataHandler::setSampleName(const std::string& sample_name_I)
  {
    sample_name_ = sample_name_I;
  }

  std::string& MetaDataHandler::getSampleName()
  {
    return sample_name_;
  }

  std::string MetaDataHandler::getSampleName() const
  {
    return sample_name_;
  }

  void MetaDataHandler::setSampleGroupName(const std::string& sample_group_name_I)
  {
    sample_group_name_ = sample_group_name_I;
  }

  std::string& MetaDataHandler::getSampleGroupName()
  {
    return sample_group_name_;
  }

  std::string MetaDataHandler::getSampleGroupName() const
  {
    return sample_group_name_;
  }

  void MetaDataHandler::setSequenceSegmentName(const std::string& sequence_segment_name_I)
  {
    sequence_segment_name_ = sequence_segment_name_I;
  }

  std::string& MetaDataHandler::getSequenceSegmentName()
  {
    return sequence_segment_name_;
  }

  std::string MetaDataHandler::getSequenceSegmentName() const
  {
    return sequence_segment_name_;
  }

  void MetaDataHandler::setFilename(const std::string& filename_I)
  {
    filename_ = filename_I;
  }

  std::string& MetaDataHandler::getFilename()
  {
    return filename_;
  }

  std::string MetaDataHandler::getFilename() const
  {
    return filename_;
  }

  void MetaDataHandler::setSampleType(SampleType sample_type)
  {
    sample_type_ = sample_type;
  }

  MetaDataHandler::SampleType& MetaDataHandler::getSampleType()
  {
    return sample_type_;
  }

  MetaDataHandler::SampleType MetaDataHandler::getSampleType() const
  {
    return sample_type_;
  }

  bool MetaDataHandler::validateMetaData(const MetaDataHandler& meta_data)
  {
    std::vector<std::string> sample_types =
      {"Unknown", "Standard", "QC", "Blank", "Double Blank", "Solvent"};

    bool is_valid { true };

    if (meta_data.getSampleName().empty()) {
      std::cout << "SequenceFile Error: sample_name must be specified." << std::endl;
      // throw std::runtime_error("sample name");
      is_valid = false;
    }

    if (meta_data.getSampleGroupName().empty()) {
      std::cout << "SequenceFile Error: sample_group_name must be specified." << std::endl;
      // throw std::runtime_error("sample group name");
      is_valid = false;
    }

    if (meta_data.getSequenceSegmentName().empty()) {
      std::cout << "SequenceFile Error: sequence_segment_name must be specified." << std::endl;
      // throw std::runtime_error("sequence segment name");
      is_valid = false;
    }

    if (meta_data.getFilename().empty()) {
      std::cout << "SequenceFile Error: filename must be specified." << std::endl;
      // throw std::runtime_error("filename");
      is_valid = false;
    }

    return is_valid;
  }

  void MetaDataHandler::clear()
  {
    sample_name_.clear();
    sample_group_name_.clear();
    sequence_segment_name_.clear();
    filename_.clear();
    sample_type_ = SampleType::Unknown;
  }
}