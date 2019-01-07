// TODO: Add copyright

#include <SmartPeak/core/Filenames.h>
#include <string>

namespace SmartPeak
{
  Filenames Filenames::getDefaultStaticFilenames(
    const std::string& dir
  )
  {
    Filenames static_filenames;
    static_filenames.sequence_csv_i = dir + "/sequence.csv";
    static_filenames.parameters_csv_i = dir + "/parameters.csv";
    static_filenames.traML_csv_i = dir + "/traML.csv";
    static_filenames.featureFilterComponents_csv_i = dir + "/featureFilterComponents.csv";
    static_filenames.featureFilterComponentGroups_csv_i = dir + "/featureFilterComponentGroups.csv";
    static_filenames.featureQCComponents_csv_i = dir + "/featureQCComponents.csv";
    static_filenames.featureQCComponentGroups_csv_i = dir + "/featureQCComponentGroups.csv";
    static_filenames.quantitationMethods_csv_i = dir + "/quantitationMethods.csv";
    static_filenames.standardsConcentrations_csv_i = dir + "/standardsConcentrations.csv";
    static_filenames.referenceData_csv_i = dir + "referenceData.csv";
    static_filenames.sequenceSummary_csv_o = dir + "/SequenceSummary.csv";
    static_filenames.featureSummary_csv_o = dir + "/FeatureSummary.csv";
    return static_filenames;
  }

  Filenames Filenames::getDefaultDynamicFilenames(
    const std::string& dir,
    const std::string& injection_name
  )
  {
    const std::string partial_pathname = dir + "/features/" + injection_name;
    Filenames dynamic_filenames;
    dynamic_filenames.mzML_i = dir + "/mzML/" + injection_name + ".mzML"; // goes to its own map
    dynamic_filenames.featureXML_o = partial_pathname + ".featureXML";
    dynamic_filenames.feature_csv_o = partial_pathname + ".csv";
    dynamic_filenames.featureXML_i = partial_pathname + ".featureXML"; // goes to its own map
    dynamic_filenames.features_pdf_o = partial_pathname;
    dynamic_filenames.calibrators_pdf_o = partial_pathname;
    dynamic_filenames.quantitationMethods_csv_o = partial_pathname + "_quantitationMethods.csv";
    dynamic_filenames.componentsToConcentrations_csv_o = partial_pathname + "_componentsToConcentrations.csv";
    return dynamic_filenames;
  }

  void Filenames::clear()
  {
    sequence_csv_i.clear();
    parameters_csv_i.clear();
    traML_csv_i.clear();
    featureFilterComponents_csv_i.clear();
    featureFilterComponentGroups_csv_i.clear();
    featureQCComponents_csv_i.clear();
    featureQCComponentGroups_csv_i;
    quantitationMethods_csv_i.clear();
    standardsConcentrations_csv_i.clear();
    referenceData_csv_i.clear();
    mzML_i.clear();
    featureXML_o.clear();
    feature_csv_o.clear();
    featureXML_i.clear();
    features_pdf_o.clear();
    calibrators_pdf_o.clear();
    quantitationMethods_csv_o.clear();
    componentsToConcentrations_csv_o.clear();
    sequenceSummary_csv_o.clear();
    featureSummary_csv_o.clear();
  }
}
