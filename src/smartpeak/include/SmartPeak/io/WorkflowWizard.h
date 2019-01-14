// TODO: Add copyright

#pragma once

#include <SmartPeak/core/Filenames.h>
#include <SmartPeak/core/SequenceHandler.h>
#include <string>

namespace SmartPeak
{
  class WorkflowWizard
  {
public:
    WorkflowWizard() = delete;
    ~WorkflowWizard() = delete;

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
      const std::string& format,
      const bool verbose
    );

    static std::string getFeatureFilterComponentsAndGroupsInfo(
      const std::string& filename_components,
      const std::string& filename_components_groups,
      const bool verbose
    );

    static std::string getFeatureQCComponentsAndGroupsInfo(
      const std::string& filename_components,
      const std::string& filename_components_groups,
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
  };
}
