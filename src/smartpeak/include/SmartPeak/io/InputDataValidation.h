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
      const std::string& filename,
      const std::string& delimiter,
      const bool verbose
    );

    static std::string getParametersInfo(const std::string& filename, const bool verbose);

    static std::string getTraMLInfo(
      const std::string& filename,
      const bool verbose
    );

    static std::string getComponentsAndGroupsInfo(
      const std::string& filename_components,
      const std::string& filename_components_groups,
      const bool is_feature_filter, // else is feature qc
      const bool verbose
    );

    static std::string getQuantitationMethodsInfo(
      const std::string& filename,
      const bool verbose
    );

    static std::string getStandardsConcentrationsInfo(
      const std::string& filename,
      const bool verbose
    );

    static bool sampleNamesAreConsistent(
      const std::string& sequence_filename,
      const std::string& delimiter,
      const std::string& standards_filename
    );

    static bool componentNamesAreConsistent(
      const std::string& traML_filename,
      const std::string& featureFilter_filename,
      const std::string& featureQC_filename,
      const std::string& quantitationMethods_filename,
      const std::string& standardConcentrations_filename
    );

    static bool componentNameGroupsAreConsistent(
      const std::string& traML_filename,
      const std::string& featureFilter_filename,
      const std::string& featureQC_filename
    );

    static bool heavyComponentsAreConsistent(
      const std::string& traML_filename,
      const std::string& quantitationMethods_filename,
      const std::string& standardConcentrations_filename
    );

    static bool validateNamesInFiles(
      const std::set<std::string>& names1,
      const std::set<std::string>& names2,
      const std::string& filename1,
      const std::string& filename2
    );

    static std::set<std::string> findMissingNames(
      const std::set<std::string>& names,
      const std::set<std::string>& bucket
    );

    static std::string logMissingNames(
      const std::set<std::string>& missing_names,
      const std::string& filename1,
      const std::string& filename2
    );
  };
}
