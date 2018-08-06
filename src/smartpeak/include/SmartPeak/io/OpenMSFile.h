/**TODO:  Add copyright*/

#pragma once

#include <SmartPeak/core/SequenceSegmentHandler.h>
#include <SmartPeak/core/RawDataHandler.h>

namespace SmartPeak
{
  class OpenMSFile
  {
public:
    OpenMSFile() = default;
    ~OpenMSFile() = default;

    void loadStandardsConcentrations(
      SequenceSegmentHandler& sequenceSegmentHandler_IO,
      const std::string& filename,
      const bool verbose = false
    ) const;

    void loadQuantitationMethods(
      SequenceSegmentHandler& sequenceSegmentHandler_IO,
      const std::string& filename,
      const bool verbose = false
    ) const;

    void loadTraML(
      RawDataHandler& rawDataHandler,
      const std::string& filename,
      const std::string& format,
      const bool verbose = false
    ) const;

    void loadMSExperiment(
        RawDataHandler& rawDataHandler,
        const std::string& mzML_i,
        const std::vector<std::map<std::string, std::string>>& MRMMapping_params_I = std::vector<std::map<std::string, std::string>>(),
        const std::vector<std::map<std::string, std::string>>& chromatogramExtractor_params_I = std::vector<std::map<std::string, std::string>>(),
        const std::vector<std::map<std::string, std::string>>& mzML_params_I = std::vector<std::map<std::string, std::string>>(),
        const bool verbose = false
    ) const;

    void loadFeatureMap(
        RawDataHandler& rawDataHandler,
        const std::string& featureXML_i,
        const bool verbose = false
    );

    void loadFeatureFilter(
        RawDataHandler& rawDataHandler,
        const std::string& featureFilterComponents_csv_i = "",
        const std::string& featureFilterComponentGroups_csv_i = "",
        const bool verbose = false
    );

    void loadFeatureQC(
      RawDataHandler& rawDataHandler,
      const std::string& featureQCComponents_csv_i,
      const std::string& featureQCComponentGroups_csv_i,
      const bool verbose = false
    );

    void readRawDataProcessingParameters(
      RawDataHandler& rawDataHandler, const std::string& filename, const std::string& delimiter = ","
    );

    void parse_rawDataProcessingParameters(
      RawDataHandler& rawDataHandler,
      std::map<std::string, std::vector<std::map<std::string, std::string>>>& parameters_file
    );

    void storeQuantitationMethods(
        const SequenceSegmentHandler& sequenceSegmentHandler_IO,
        const std::string& quantitationMethods_csv_o,
        const bool verbose = false
    );

    void storeFeatureMap(
      RawDataHandler& rawDataHandler_IO,
      const std::string& featureXML_o,
      const bool verbose = false
    );

    void storeMzML(const std::string& out, const OpenMS::MSExperiment& output);

private:
  };
}
