// TODO: Add copyright

#pragma once

#include <SmartPeak/core/Filenames.h>
#include <SmartPeak/core/SequenceHandler.h>
#include <string>

namespace SmartPeak
{
  class InputDataValidation
  {
public:
    InputDataValidation()                                      = delete;
    ~InputDataValidation()                                     = delete;
    InputDataValidation(const InputDataValidation&)            = delete;
    InputDataValidation& operator=(const InputDataValidation&) = delete;
    InputDataValidation(InputDataValidation&&)                 = delete;
    InputDataValidation& operator=(InputDataValidation&&)      = delete;

    struct FilenameInfo {
      enum ValidityEnum {invalid, valid, not_provided} validity;
      std::string pathname;
      std::string member_name;
    };

    static bool fileExists(const std::string& filepath);
    static FilenameInfo isValidFilename(const std::string& filename, const std::string& member_name);

    static std::string getSequenceInfo(
      const SequenceHandler& sequenceHandler,
      const std::string& delimiter
    );

    static std::string getParametersInfo(
      const std::map<std::string,std::vector<std::map<std::string,std::string>>>& parameters
    );

    static std::string getTraMLInfo(
      const RawDataHandler& rawDataHandler
    );

    static std::string getComponentsAndGroupsInfo(
      const RawDataHandler& rawDataHandler,
      const bool is_feature_filter // else is feature qc
    );

    static std::string getQuantitationMethodsInfo(
      const SequenceSegmentHandler& sequenceSegmentHandler
    );

    static std::string getStandardsConcentrationsInfo(
      const SequenceSegmentHandler& sequenceSegmentHandler
    );

    static bool sampleNamesAreConsistent(
      const SequenceHandler& sequenceHandler
    );

    static bool componentNamesAreConsistent(
      const SequenceHandler& sequenceHandler
    );

    static bool componentNameGroupsAreConsistent(
      const SequenceHandler& sequenceHandler
    );

    static bool heavyComponentsAreConsistent(
      const SequenceHandler& sequenceHandler
    );

    static bool validateNamesInStructures(
      const std::set<std::string>& names1,
      const std::set<std::string>& names2,
      const std::string& structure_ref1,
      const std::string& structure_ref2,
      const bool check_both_directions = true
    );

    static std::set<std::string> findMissingNames(
      const std::set<std::string>& names,
      const std::set<std::string>& bucket
    );

    static std::string logMissingNames(
      const std::set<std::string>& missing_names,
      const std::string& structure_ref1,
      const std::string& structure_ref2
    );
  };
}
