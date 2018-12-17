/**TODO:  Add copyright*/

#include <SmartPeak/core/MetaDataHandler.h>

#include <vector>
#include <algorithm>

namespace SmartPeak
{
  const char* MetaDataHandler::SampleTypeToString(const SampleType sample_type)
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
      case SampleType::Unrecognized:
        return "Unrecognized";
    }
  }

  MetaDataHandler::SampleType MetaDataHandler::stringToSampleType(const std::string& sample_type)
  {
    if (sample_type == "Unknown")
      return SampleType::Unknown;
    if (sample_type == "Standard")
      return SampleType::Standard;
    if (sample_type == "QC")
      return SampleType::QC;
    if (sample_type == "Blank")
      return SampleType::Blank;
    if (sample_type == "Double Blank")
      return SampleType::DoubleBlank;
    if (sample_type == "Solvent")
      return SampleType::Solvent;
    if (sample_type == "Unrecognized")
      return SampleType::Unrecognized;
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
    original_filename_ = filename_I;
  }

  std::string& MetaDataHandler::getFilename()
  {
    return original_filename_;
  }

  std::string MetaDataHandler::getFilename() const
  {
    return original_filename_;
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
    // std::vector<std::string> sample_types {"Unknown", "Standard", "QC", "Blank", "Double Blank", "Solvent"};

    bool is_valid { true };

    if (meta_data.getSampleName().empty()) {
      std::cerr << "SequenceFile Error: sample_name must be specified." << std::endl;
      is_valid = false;
    }

    if (meta_data.getSampleGroupName().empty()) {
      std::cerr << "SequenceFile Error: sample_group_name must be specified." << std::endl;
      is_valid = false;
    }

    if (meta_data.getSequenceSegmentName().empty()) {
      std::cerr << "SequenceFile Error: sequence_segment_name must be specified." << std::endl;
      is_valid = false;
    }

    if (meta_data.getFilename().empty()) {
      std::cerr << "SequenceFile Error: original_filename must be specified." << std::endl;
      is_valid = false;
    }

    if (meta_data.getSampleType() == SampleType::Unrecognized) {
      std::cerr << "SequenceFile Error: sample type could not be recognized." << std::endl;
      is_valid = false;
    }

    if (meta_data.acq_method_name.empty()) {
      std::cerr << "SequenceFile Error: acq_method_name must be specified." << std::endl;
      is_valid = false;
    }

    if (meta_data.inj_volume == -1.0) {
      std::cerr << "SequenceFile Error: no value for inj_volume." << std::endl;
      is_valid = false;
    }

    if (meta_data.inj_volume_units.empty()) {
      std::cerr << "SequenceFile Error: inj_volume_units must be specified." << std::endl;
      is_valid = false;
    }

    if (meta_data.batch_name.empty()) {
      std::cerr << "SequenceFile Error: batch_name must be specified." << std::endl;
      is_valid = false;
    }

    return is_valid;
  }

  void MetaDataHandler::clear()
  {
    sample_name_.clear();
    sample_group_name_.clear();
    sequence_segment_name_.clear();
    original_filename_.clear();
    sample_type_ = SampleType::Unknown;
    acq_method_name.clear();
    inj_volume = -1.0;
    inj_volume_units.clear();
    batch_name.clear();

    // optional
    rack_number = -1;
    plate_number = -1;
    pos_number = -1;
    inj_number = -1;
    dilution_factor = -1.0;
    instrument.clear();
    operator_name.clear();
    proc_method_name.clear();
    acquisition_date_and_time = {0, 0, 0, 0, 0, 0, 0, 0, 0};
    calculated_concentration_units.clear();
  }
}
