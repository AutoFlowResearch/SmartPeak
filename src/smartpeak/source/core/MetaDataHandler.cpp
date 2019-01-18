// TODO: Add copyright

#include <SmartPeak/core/MetaDataHandler.h>
#include <iostream>

namespace SmartPeak
{
  std::string MetaDataHandler::SampleTypeToString(const SampleType sample_type)
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
    sample_name = sample_name_I;
  }

  // std::string& MetaDataHandler::getSampleName()
  // {
  //   return sample_name;
  // }

  std::string MetaDataHandler::getSampleName() const
  {
    return sample_name;
  }

  void MetaDataHandler::setSampleGroupName(const std::string& sample_group_name_I)
  {
    sample_group_name = sample_group_name_I;
  }

  std::string& MetaDataHandler::getSampleGroupName()
  {
    return sample_group_name;
  }

  std::string MetaDataHandler::getSampleGroupName() const
  {
    return sample_group_name;
  }

  void MetaDataHandler::setSequenceSegmentName(const std::string& sequence_segment_name_I)
  {
    sequence_segment_name = sequence_segment_name_I;
  }

  std::string& MetaDataHandler::getSequenceSegmentName()
  {
    return sequence_segment_name;
  }

  std::string MetaDataHandler::getSequenceSegmentName() const
  {
    return sequence_segment_name;
  }

  void MetaDataHandler::setFilename(const std::string& filename_I)
  {
    original_filename = filename_I;
  }

  std::string& MetaDataHandler::getFilename()
  {
    return original_filename;
  }

  std::string MetaDataHandler::getFilename() const
  {
    return original_filename;
  }

  void MetaDataHandler::setSampleType(SampleType sample_type_I)
  {
    sample_type = sample_type_I;
  }

  MetaDataHandler::SampleType& MetaDataHandler::getSampleType()
  {
    return sample_type;
  }

  MetaDataHandler::SampleType MetaDataHandler::getSampleType() const
  {
    return sample_type;
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

    if (meta_data.inj_volume <= 0.0) {
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
    sample_name.clear();
    sample_group_name.clear();
    sequence_segment_name.clear();
    original_filename.clear();
    sample_type = SampleType::Unknown;
    acq_method_name.clear();
    inj_volume = -1.0;
    inj_volume_units.clear();
    batch_name.clear();

    // optional
    rack_number = -1;
    plate_number = -1;
    pos_number = -1;
    inj_number = -1;
    dilution_factor = 1.0;
    instrument.clear();
    operator_name.clear();
    proc_method_name.clear();
    acquisition_date_and_time = {0, 0, 0, 0, 0, 0, 0, 0, 0};
  }

  std::string MetaDataHandler::getInjectionName() const
  {
    std::string name(sample_name + "_" + std::to_string(inj_number) + "_" + batch_name + "_");
    char time_repr[64];
    // ISO 8601 date format, without colons (for Windows filesystems compatibility)
    // i.e. "2019-01-23_164055"
    const size_t bytes_written = strftime(time_repr, 64, "%Y-%m-%d_%H%M%S", &acquisition_date_and_time);
    if (bytes_written != 17) {
      throw "Unexpected number of characters written into the array.";
    }
    name.append(time_repr);
    return name;
  }
}
