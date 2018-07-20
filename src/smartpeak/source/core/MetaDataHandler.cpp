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

    std::vector<std::string>::const_iterator it =
      std::find(sample_types.cbegin(), sample_types.cend(), meta_data.getSampleType());

    if (meta_data.getSampleType().empty() || it == sample_types.cend()) {
      std::cout << "SequenceFile Error: sample_type for sample_name "
        << meta_data.getSampleName() << " is not correct." << std::endl;

      std::cout << "Supported samples types are the following: ";

      for (const std::string& sample_type : sample_types) {
        std::cout << sample_type << "; ";
      }

      std::cout << std::endl;
      // throw std::runtime_error("sample type");
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
    sample_type_.clear();
  }
}