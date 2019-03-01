/**TODO:  Add copyright*/

#pragma once

#include <SmartPeak/core/SequenceSegmentHandler.h>
#include <SmartPeak/core/RawDataHandler.h>

namespace SmartPeak
{
  class OpenMSFile
  {
public:
    OpenMSFile()                             = delete;
    ~OpenMSFile()                            = delete;
    OpenMSFile(const OpenMSFile&)            = delete;
    OpenMSFile& operator=(const OpenMSFile&) = delete;
    OpenMSFile(OpenMSFile&&)                 = delete;
    OpenMSFile& operator=(OpenMSFile&&)      = delete;

    static void loadStandardsConcentrations(
      SequenceSegmentHandler& sequenceSegmentHandler_IO,
      const std::string& filename,
      const bool verbose = false
    );

    static void loadQuantitationMethods(
      SequenceSegmentHandler& sequenceSegmentHandler_IO,
      const std::string& filename,
      const bool verbose = false
    );

    static void loadTraML(
      RawDataHandler& rawDataHandler,
      const std::string& filename,
      const std::string& format,
      const bool verbose = false
    );

    static void loadMSExperiment(
      RawDataHandler& rawDataHandler,
      const std::string& mzML_i,
      const std::vector<std::map<std::string, std::string>>& MRMMapping_params_I = std::vector<std::map<std::string, std::string>>(),
      const std::vector<std::map<std::string, std::string>>& chromatogramExtractor_params_I = std::vector<std::map<std::string, std::string>>(),
      const std::vector<std::map<std::string, std::string>>& mzML_params_I = std::vector<std::map<std::string, std::string>>(),
      const bool verbose = false
    );

    static void loadFeatureMap(
      RawDataHandler& rawDataHandler,
      const std::string& filename,
      const bool verbose = false
    );

    static void loadFeatureFilter(
      RawDataHandler& rawDataHandler,
      const std::string& filename_components = "",
      const std::string& filename_components_groups = "",
      const bool verbose = false
    );

    static void loadFeatureQC(
      RawDataHandler& rawDataHandler,
      const std::string& filename_components = "",
      const std::string& filename_components_groups = "",
      const bool verbose = false
    );

    static void loadValidationData(
      RawDataHandler& rawDataHandler,
      const std::string& referenceData_csv_i,
      const bool verbose = false
    );

    static void readRawDataProcessingParameters(
      RawDataHandler& rawDataHandler,
      const std::string& filename,
      const bool verbose = false
    );

    static void sanitizeRawDataProcessorParameters(
      RawDataHandler& rawDataHandler,
      std::map<std::string, std::vector<std::map<std::string, std::string>>>& parameters_file,
      const bool verbose = false
    );

    static void storeQuantitationMethods(
      const SequenceSegmentHandler& sequenceSegmentHandler_IO,
      const std::string& filename,
      const bool verbose = false
    );

    static void storeFeatureMap(
      RawDataHandler& rawDataHandler_IO,
      const std::string& filename,
      const bool verbose = false
    );

    static void storeMzML(
      const std::string& filename,
      const OpenMS::MSExperiment& experiment,
      const bool verbose = false
    );
  };
}
