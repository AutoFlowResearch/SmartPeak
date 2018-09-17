// TODO: Add copyright

#pragma once

#include <iostream>
#include <string>

namespace SmartPeak
{

  class MetaDataHandler
  {
public:
    MetaDataHandler() = default;
    ~MetaDataHandler() = default;

    enum SampleType
    {
      Unknown = 1,
      Standard,
      QC,
      Blank,
      DoubleBlank,
      Solvent,
      Unrecognized
    };

    static const char* SampleTypeToString(const SampleType sample_type); // TODO: doesn't return a string, fix name or return type
    static SampleType stringToSampleType(const std::string& sample_type);

    void setSampleName(const std::string& sample_name);
    std::string& getSampleName();
    std::string getSampleName() const;

    void setSampleGroupName(const std::string& sample_group_name);
    std::string& getSampleGroupName();
    std::string getSampleGroupName() const;

    void setSequenceSegmentName(const std::string& sequence_segment_name);
    std::string& getSequenceSegmentName();
    std::string getSequenceSegmentName() const;

    void setFilename(const std::string& filename);
    std::string& getFilename();
    std::string getFilename() const;

    void setSampleType(SampleType sample_type);
    SampleType& getSampleType();
    SampleType getSampleType() const;

    static bool validateMetaData(const MetaDataHandler& meta_data);

    void clear();

private:
    std::string sample_name_;
    std::string sample_group_name_;
    std::string sequence_segment_name_;
    std::string filename_;
    SampleType sample_type_ = SampleType::Unknown;
  };
}
