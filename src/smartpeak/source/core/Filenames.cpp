// TODO: Add copyright

#include <SmartPeak/core/Filenames.h>
#include <string>
#include <boost/filesystem.hpp>

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
    static_filenames.featureRSDFilterComponents_csv_i = dir + "/featureRSDFilterComponents.csv";
    static_filenames.featureRSDFilterComponentGroups_csv_i = dir + "/featureRSDFilterComponentGroups.csv";
    static_filenames.featureRSDQCComponents_csv_i = dir + "/featureRSDQCComponents.csv";
    static_filenames.featureRSDQCComponentGroups_csv_i = dir + "/featureRSDQCComponentGroups.csv";
    static_filenames.featureBackgroundFilterComponents_csv_i = dir + "/featureBackgroundFilterComponents.csv";
    static_filenames.featureBackgroundFilterComponentGroups_csv_i = dir + "/featureBackgroundFilterComponentGroups.csv";
    static_filenames.featureBackgroundQCComponents_csv_i = dir + "/featureBackgroundQCComponents.csv";
    static_filenames.featureBackgroundQCComponentGroups_csv_i = dir + "/featureBackgroundQCComponentGroups.csv";
    static_filenames.featureRSDEstimationComponents_csv_i = dir + "/featureRSDEstimationComponents.csv";
    static_filenames.featureRSDEstimationComponentGroups_csv_i = dir + "/featureRSDEstimationComponentGroups.csv";
    static_filenames.featureBackgroundEstimationComponents_csv_i = dir + "/featureBackgroundEstimationComponents.csv";
    static_filenames.featureBackgroundEstimationComponentGroups_csv_i = dir + "/featureBackgroundEstimationComponentGroups.csv";
    static_filenames.quantitationMethods_csv_i = dir + "/quantitationMethods.csv";
    static_filenames.standardsConcentrations_csv_i = dir + "/standardsConcentrations.csv";
    static_filenames.referenceData_csv_i = dir + "/referenceData.csv";
    static_filenames.pivotTable_csv_o = dir + "/PivotTable.csv";
    static_filenames.featureDB_csv_o = dir + "/FeatureDB.csv";
    return static_filenames;
  }

  Filenames Filenames::getDefaultDynamicFilenames(
    const std::string& mzml_input_path,
    const std::string& features_input_path,
    const std::string& output_path,
    const std::string& input_inj_name,
    const std::string& output_inj_name,
    const std::string& input_sample_name,
    const std::string& output_sample_name
  )
  {
    Filenames dynamic_filenames;
    dynamic_filenames.mzML_i       = mzml_input_path + "/" + input_inj_name + ".mzML";
    dynamic_filenames.featureXML_i = features_input_path + "/" + output_inj_name + ".featureXML";
    dynamic_filenames.featureXMLSampleGroup_i = features_input_path + "/" + input_sample_name + ".featureXML";
    dynamic_filenames.mzTab_i = features_input_path + "/" + output_inj_name + ".mzTab";
    // NOTE: input featureXML is almost always derived from the output featureXML

    const std::string prefix = output_path + "/" + output_inj_name;
    dynamic_filenames.featureXML_o                     = prefix + ".featureXML";
    dynamic_filenames.featureXMLSampleGroup_o = output_path + "/" + output_sample_name + ".featureXML";
    dynamic_filenames.mzTab_o = prefix + ".mzTab";
    dynamic_filenames.feature_csv_o                    = prefix + ".csv";
    dynamic_filenames.features_pdf_o                   = prefix;
    dynamic_filenames.featureFilterComponents_csv_o = prefix + "_featureFilterComponents.csv";
    dynamic_filenames.featureFilterComponentGroups_csv_o = prefix + "_featureFilterComponentGroups.csv";
    dynamic_filenames.featureQCComponents_csv_o = prefix + "_featureQCComponents.csv";
    dynamic_filenames.featureQCComponentGroups_csv_o = prefix + "_featureQCComponentGroups.csv";
    dynamic_filenames.featureRSDFilterComponents_csv_o = prefix + "_featureRSDFilterComponents.csv";
    dynamic_filenames.featureRSDFilterComponentGroups_csv_o = prefix + "_featureRSDFilterComponentGroups.csv";
    dynamic_filenames.featureRSDQCComponents_csv_o = prefix + "_featureRSDQCComponents.csv";
    dynamic_filenames.featureRSDQCComponentGroups_csv_o = prefix + "_featureRSDQCComponentGroups.csv";
    dynamic_filenames.featureBackgroundFilterComponents_csv_o = prefix + "_featureBackgroundFilterComponents.csv";
    dynamic_filenames.featureBackgroundFilterComponentGroups_csv_o = prefix + "_featureBackgroundFilterComponentGroups.csv";
    dynamic_filenames.featureBackgroundQCComponents_csv_o = prefix + "_featureBackgroundQCComponents.csv";
    dynamic_filenames.featureBackgroundQCComponentGroups_csv_o = prefix + "_featureBackgroundQCComponentGroups.csv";
    dynamic_filenames.featureRSDEstimationComponents_csv_o = prefix + "_featureRSDEstimationComponents.csv";
    dynamic_filenames.featureRSDEstimationComponentGroups_csv_o = prefix + "_featureRSDEstimationComponentGroups.csv";
    dynamic_filenames.featureBackgroundEstimationComponents_csv_o = prefix + "_featureBackgroundEstimationComponents.csv";
    dynamic_filenames.featureBackgroundEstimationComponentGroups_csv_o = prefix + "_featureBackgroundEstimationComponentGroups.csv";
    dynamic_filenames.quantitationMethods_csv_o        = prefix + "_quantitationMethods.csv";
    dynamic_filenames.componentsToConcentrations_csv_o = prefix + "_componentsToConcentrations.csv";

    dynamic_filenames.mzml_input_path = mzml_input_path;
    dynamic_filenames.features_input_path = features_input_path;
    dynamic_filenames.output_path = output_path;

    return dynamic_filenames;
  }

  void Filenames::updateDefaultDynamicFilenames(
    const std::string& mzml_input_path,
    const std::string& features_input_path,
    const std::string& output_path,
    Filenames& filenames
  )
  {
    namespace fs = boost::filesystem;

    filenames.mzML_i = fs::path(mzml_input_path)
      .append(fs::path(filenames.mzML_i).filename().c_str())
      .string();

    filenames.featureXML_i = fs::path(features_input_path)
      .append(fs::path(filenames.featureXML_i).filename().c_str())
      .string();

    filenames.featureXMLSampleGroup_i = fs::path(features_input_path)
      .append(fs::path(filenames.featureXMLSampleGroup_i).filename().c_str())
      .string();

    filenames.mzTab_i = fs::path(features_input_path)
      .append(fs::path(filenames.mzTab_i).filename().c_str())
      .string();

    const fs::path prefix { output_path };

    filenames.featureXML_o = fs::path(prefix).append(fs::path(filenames.featureXML_o).filename().c_str()).string();
    filenames.featureXMLSampleGroup_o = fs::path(prefix).append(fs::path(filenames.featureXMLSampleGroup_o).filename().c_str()).string();
    filenames.mzTab_o = fs::path(prefix).append(fs::path(filenames.mzTab_o).filename().c_str()).string();

    filenames.feature_csv_o = fs::path(prefix).append(fs::path(filenames.feature_csv_o).filename().c_str()).string();

    filenames.features_pdf_o = fs::path(prefix).append(fs::path(filenames.features_pdf_o).filename().c_str()).string();

    filenames.featureFilterComponents_csv_o = fs::path(prefix).append(fs::path(filenames.featureFilterComponents_csv_o).filename().c_str()).string();
    filenames.featureFilterComponentGroups_csv_o = fs::path(prefix).append(fs::path(filenames.featureFilterComponentGroups_csv_o).filename().c_str()).string();
    filenames.featureQCComponents_csv_o = fs::path(prefix).append(fs::path(filenames.featureQCComponents_csv_o).filename().c_str()).string();
    filenames.featureQCComponentGroups_csv_o = fs::path(prefix).append(fs::path(filenames.featureQCComponentGroups_csv_o).filename().c_str()).string();
    filenames.featureRSDFilterComponents_csv_o = fs::path(prefix).append(fs::path(filenames.featureRSDFilterComponents_csv_o).filename().c_str()).string();
    filenames.featureRSDFilterComponentGroups_csv_o = fs::path(prefix).append(fs::path(filenames.featureRSDFilterComponentGroups_csv_o).filename().c_str()).string();
    filenames.featureRSDQCComponents_csv_o = fs::path(prefix).append(fs::path(filenames.featureRSDQCComponents_csv_o).filename().c_str()).string();
    filenames.featureRSDQCComponentGroups_csv_o = fs::path(prefix).append(fs::path(filenames.featureRSDQCComponentGroups_csv_o).filename().c_str()).string();
    filenames.featureBackgroundFilterComponents_csv_o = fs::path(prefix).append(fs::path(filenames.featureBackgroundFilterComponents_csv_o).filename().c_str()).string();
    filenames.featureBackgroundFilterComponentGroups_csv_o = fs::path(prefix).append(fs::path(filenames.featureBackgroundFilterComponentGroups_csv_o).filename().c_str()).string();
    filenames.featureBackgroundQCComponents_csv_o = fs::path(prefix).append(fs::path(filenames.featureBackgroundQCComponents_csv_o).filename().c_str()).string();
    filenames.featureBackgroundQCComponentGroups_csv_o = fs::path(prefix).append(fs::path(filenames.featureBackgroundQCComponentGroups_csv_o).filename().c_str()).string();
    filenames.featureRSDEstimationComponents_csv_o = fs::path(prefix).append(fs::path(filenames.featureRSDEstimationComponents_csv_o).filename().c_str()).string();
    filenames.featureRSDEstimationComponentGroups_csv_o = fs::path(prefix).append(fs::path(filenames.featureRSDEstimationComponentGroups_csv_o).filename().c_str()).string();
    filenames.featureBackgroundEstimationComponents_csv_o = fs::path(prefix).append(fs::path(filenames.featureBackgroundEstimationComponents_csv_o).filename().c_str()).string();
    filenames.featureBackgroundEstimationComponentGroups_csv_o = fs::path(prefix).append(fs::path(filenames.featureBackgroundEstimationComponentGroups_csv_o).filename().c_str()).string();

    filenames.quantitationMethods_csv_o = fs::path(prefix).append(fs::path(filenames.quantitationMethods_csv_o).filename().c_str()).string();

    filenames.componentsToConcentrations_csv_o = fs::path(prefix).append(fs::path(filenames.componentsToConcentrations_csv_o).filename().c_str()).string();
  }

  void Filenames::clear()
  {
    sequence_csv_i.clear();
    parameters_csv_i.clear();
    traML_csv_i.clear();
    featureFilterComponents_csv_i.clear();
    featureFilterComponentGroups_csv_i.clear();
    featureQCComponents_csv_i.clear();
    featureQCComponentGroups_csv_i.clear();
    featureRSDFilterComponents_csv_i.clear();
    featureRSDFilterComponentGroups_csv_i.clear();
    featureRSDQCComponents_csv_i.clear();
    featureRSDQCComponentGroups_csv_i.clear();
    featureBackgroundFilterComponents_csv_i.clear();
    featureBackgroundFilterComponentGroups_csv_i.clear();
    featureBackgroundQCComponents_csv_i.clear();
    featureBackgroundQCComponentGroups_csv_i.clear();
    featureRSDEstimationComponents_csv_i.clear();
    featureRSDEstimationComponentGroups_csv_i.clear();
    featureBackgroundEstimationComponents_csv_i.clear();
    featureBackgroundEstimationComponentGroups_csv_i.clear();
    quantitationMethods_csv_i.clear();
    standardsConcentrations_csv_i.clear();
    referenceData_csv_i.clear();
    mzML_i.clear();
    mzTab_i.clear();
    mzTab_o.clear();
    featureXML_o.clear();
    featureXMLSampleGroup_o.clear();
    feature_csv_o.clear();
    featureXML_i.clear();
    featureXMLSampleGroup_i.clear();
    features_pdf_o.clear();
    featureFilterComponents_csv_o.clear();
    featureFilterComponentGroups_csv_o.clear();
    featureQCComponents_csv_o.clear();
    featureQCComponentGroups_csv_o.clear();
    featureRSDFilterComponents_csv_o.clear();
    featureRSDFilterComponentGroups_csv_o.clear();
    featureRSDQCComponents_csv_o.clear();
    featureRSDQCComponentGroups_csv_o.clear();
    featureBackgroundFilterComponents_csv_o.clear();
    featureBackgroundFilterComponentGroups_csv_o.clear();
    featureBackgroundQCComponents_csv_o.clear();
    featureBackgroundQCComponentGroups_csv_o.clear();
    featureRSDEstimationComponents_csv_o.clear();
    featureRSDEstimationComponentGroups_csv_o.clear();
    featureBackgroundEstimationComponents_csv_o.clear();
    featureBackgroundEstimationComponentGroups_csv_o.clear();
    quantitationMethods_csv_o.clear();
    componentsToConcentrations_csv_o.clear();
    pivotTable_csv_o.clear();
    featureDB_csv_o.clear();
  }
}