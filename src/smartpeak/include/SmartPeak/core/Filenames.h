// TODO: Add copyright

#pragma once

#include <string>

namespace SmartPeak
{
  class Filenames
  {
public:
    std::string sequence_csv_i;
    std::string parameters_csv_i;
    std::string traML_csv_i;
    std::string featureFilterComponents_csv_i;
    std::string featureFilterComponentGroups_csv_i;
    std::string featureQCComponents_csv_i;
    std::string featureQCComponentGroups_csv_i;
    std::string quantitationMethods_csv_i;
    std::string standardsConcentrations_csv_i;
    std::string referenceData_csv_i;
    std::string mzML_i;
    std::string featureXML_o;
    std::string feature_csv_o;
    std::string featureXML_i;
    std::string features_pdf_o;
    std::string quantitationMethods_csv_o;
    std::string componentsToConcentrations_csv_o;
    std::string sequenceSummary_csv_o;
    std::string featureSummary_csv_o;

    static Filenames getDefaultStaticFilenames(
      const std::string& dir
    );

    static Filenames getDefaultDynamicFilenames(
      const std::string& mzml_input_path,
      const std::string& features_input_path,
      const std::string& output_path,
      const std::string& sample_name
    );

    void clear();
  };
}
