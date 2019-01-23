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
    InputDataValidation() = delete;
    ~InputDataValidation() = delete;

    static bool fileExists(const std::string& filepath);
    static bool isValidFilename(const std::string& filename, const std::string& member_name);
    static void validateFilenames(const Filenames& filenames);

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
      const SequenceHandler& sequenceHandler,
      const SequenceSegmentHandler& sequenceSegmentHandler
    );

    static bool componentNamesAreConsistent(
      const RawDataHandler& rawDataHandler,
      const SequenceSegmentHandler& sequenceSegmentHandler
    );

    static bool componentNameGroupsAreConsistent(
      const RawDataHandler& rawDataHandler,
      const SequenceSegmentHandler& sequenceSegmentHandler
    );

    static bool heavyComponentsAreConsistent(
      const RawDataHandler& rawDataHandler,
      const SequenceSegmentHandler& sequenceSegmentHandler
    );

    static bool validateNamesInStructures(
      const std::set<std::string>& names1,
      const std::set<std::string>& names2,
      const std::string& structure_ref1,
      const std::string& structure_ref2
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
