/**TODO:  Add copyright*/

#pragma once

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
      sequenceSegmentHandler& sequenceSegmentHandler_IO,
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

    void loadSWATHorDIA(
        RawDataHandler& rawDataHandler,
        const String& dia_csv_i,
        const bool verbose = false
    );

    void loadFeatureMap(
        RawDataHandler& rawDataHandler,
        const String& featureXML_i,
        const bool verbose = false
    );

    void loadFeatureFilter(
        RawDataHandler& rawDataHandler,
        const String& featureFilterComponents_csv_i = "",
        const String& featureFilterComponentGroups_csv_i = "",
        const bool verbose = false
    );

    void loadFeatureQC(
      RawDataHandler& rawDataHandler,
      const String& featureQCComponents_csv_i,
      const String& featureQCComponentGroups_csv_i,
      const bool verbose = false
    );

    void readRawDataProcessingParameters(
      RawDataHandler& rawDataHandler, const String& filename, const String& delimiter = ","
    );

    void parse_rawDataProcessingParameters(
      RawDataHandler& rawDataHandler,
      std::map<std::string, std::vector<std::map<std::string, std::string>>>& parameters_file
    );

    void storeQuantitationMethods(
        const SequenceSegmentHandler& sequenceSegmentHandler_IO,
        const String& quantitationMethods_csv_o,
        const bool verbose = false
    );

    void storeFeatureMap(
      RawDataHandler& rawDataHandler_IO,
      const std::string& featureXML_o,
      const std::string& feature_csv_o,
      const bool verbose = false
    );

    void storeMzML(const std::string& out, const MSExperiment& output);

private:
  };
}
