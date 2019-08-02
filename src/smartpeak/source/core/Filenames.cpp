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
    const std::string& output_inj_name
  )
  {
    Filenames dynamic_filenames;
    dynamic_filenames.mzML_i       = mzml_input_path + "/" + input_inj_name + ".mzML";
    dynamic_filenames.featureXML_i = features_input_path + "/" + output_inj_name + ".featureXML";
    // NOTE: input featureXML is almost always derived from the output featureXML

    const std::string prefix = output_path + "/" + output_inj_name;
    dynamic_filenames.featureXML_o                     = prefix + ".featureXML";
    dynamic_filenames.feature_csv_o                    = prefix + ".csv";
    dynamic_filenames.features_pdf_o                   = prefix;
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

    const fs::path prefix { output_path };

    filenames.featureXML_o = fs::path(prefix).append(fs::path(filenames.featureXML_o).filename().c_str()).string();

    filenames.feature_csv_o = fs::path(prefix).append(fs::path(filenames.feature_csv_o).filename().c_str()).string();

    filenames.features_pdf_o = fs::path(prefix).append(fs::path(filenames.features_pdf_o).filename().c_str()).string();

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
    quantitationMethods_csv_i.clear();
    standardsConcentrations_csv_i.clear();
    referenceData_csv_i.clear();
    mzML_i.clear();
    featureXML_o.clear();
    feature_csv_o.clear();
    featureXML_i.clear();
    features_pdf_o.clear();
    quantitationMethods_csv_o.clear();
    componentsToConcentrations_csv_o.clear();
    pivotTable_csv_o.clear();
    featureDB_csv_o.clear();
  }
}
