// TODO: Add copyright

#pragma once

#include <SmartPeak/core/SampleType.h>

#include <string>
#include <ctime>

namespace SmartPeak {
  class MetaDataHandler {
public:
    /*
    * TODO:
    * All properties are now public.
    * Keeping the getters / setters for retro-compatibility.
    * Given the high number of properties, I prefer to remove the getters / setters
    * at some point because their current implementation represents more a
    * premature optimization than an actual advantage (KISS and YAGNI principles).
    */

    void setSampleName(const std::string& sample_name_I);
    std::string& getSampleName();
    const std::string& getSampleName() const;

    void setSampleGroupName(const std::string& sample_group_name_I);
    std::string& getSampleGroupName();
    const std::string& getSampleGroupName() const;

    void setSequenceSegmentName(const std::string& sequence_segment_name_I);
    std::string& getSequenceSegmentName();
    const std::string& getSequenceSegmentName() const;

    void setFilename(const std::string& filename_I);
    std::string& getFilename();
    const std::string& getFilename() const;

    void setSampleType(SampleType sample_type_I);
    SampleType& getSampleType();
    const SampleType& getSampleType() const;

    static bool validateMetaData(const MetaDataHandler& meta_data);

    void clear();

    std::string getInjectionName() const;

    void setAcquisitionDateAndTimeFromString(
      const std::string& acquisition_date_and_time,
      const std::string& format = "%Y-%m-%d_%H%M%S" // from less specific to more specific.
      // NOTE: Assuming Excel defaults as "%m-%d-%Y %H:%M" and "%m-%d-%Y %H:%M:%S" (locale-dependent)
    );

    std::string getAcquisitionDateAndTimeAsString(const std::string& format = "%Y-%m-%d_%H%M%S") const;

    // required
    std::string sample_name;
    std::string sample_group_name;
    std::string sequence_segment_name;
    std::string original_filename;
    SampleType sample_type = SampleType::Unknown;
    std::string acq_method_name;
    float inj_volume = -1.0;
    std::string inj_volume_units;
    std::string batch_name;

    // optional
    int rack_number = -1;
    int plate_number = -1;
    int pos_number = -1;
    int inj_number = -1; // default: row number (starts from 1)
    float dilution_factor = 1.0;
    std::string instrument;
    std::string operator_name;
    std::string proc_method_name;
    std::tm acquisition_date_and_time = { 0, 0, 0, 1, 0, 0, 0, 0, 0 }; // Need to start at Day 1 of the month
  };

}
