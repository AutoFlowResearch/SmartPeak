// TODO: Add copyright

#include <SmartPeak/io/InputDataValidation.h>
#include <SmartPeak/core/Filenames.h>
#include <SmartPeak/core/SequenceHandler.h>
#include <SmartPeak/io/FileReader.h>
#include <SmartPeak/io/OpenMSFile.h>
#include <SmartPeak/io/SequenceParser.h>
#include <fstream>
#include <string>

namespace SmartPeak
{
  bool InputDataValidation::fileExists(const std::string& filepath)
  {
    std::ifstream ifs(filepath);
    return ifs.is_open();
  }

  bool InputDataValidation::isValidFilename(const std::string& filename, const std::string& member_name)
  {
    std::cout << "[" << member_name << "]: ";
    if (filename.size()) {
      const bool is_valid = fileExists(filename);
      std::cout << filename << ":\t" << (is_valid ? "SUCCESS" : "FAILURE") << '\n';
      return is_valid;
    } else {
      std::cout << "\tNOT PROVIDED\n";
      return true;
    }
  }

  void InputDataValidation::validateFilenames(const Filenames& filenames)
  {
    std::cout << "Filenames validation\n";
    isValidFilename(filenames.sequence_csv_i, "sequence_csv_i");
    isValidFilename(filenames.parameters_csv_i, "parameters_csv_i");
    isValidFilename(filenames.traML_csv_i, "traML_csv_i");
    isValidFilename(filenames.featureFilterComponents_csv_i, "featureFilterComponents_csv_i");
    isValidFilename(filenames.featureFilterComponentGroups_csv_i, "featureFilterComponentGroups_csv_i");
    isValidFilename(filenames.featureQCComponents_csv_i, "featureQCComponents_csv_i");
    isValidFilename(filenames.featureQCComponentGroups_csv_i, "featureQCComponentGroups_csv_i");
    isValidFilename(filenames.quantitationMethods_csv_i, "quantitationMethods_csv_i");
    isValidFilename(filenames.standardsConcentrations_csv_i, "standardsConcentrations_csv_i");
    isValidFilename(filenames.referenceData_csv_i, "referenceData_csv_i");
    isValidFilename(filenames.mzML_i, "mzML_i");
    isValidFilename(filenames.featureXML_o, "featureXML_o");
    isValidFilename(filenames.feature_csv_o, "feature_csv_o");
    isValidFilename(filenames.featureXML_i, "featureXML_i");
    isValidFilename(filenames.features_pdf_o, "features_pdf_o");
    isValidFilename(filenames.quantitationMethods_csv_o, "quantitationMethods_csv_o");
    isValidFilename(filenames.componentsToConcentrations_csv_o, "componentsToConcentrations_csv_o");
    isValidFilename(filenames.sequenceSummary_csv_o, "sequenceSummary_csv_o");
    isValidFilename(filenames.featureSummary_csv_o, "featureSummary_csv_o");
  }

  std::string InputDataValidation::getSequenceInfo(
    const std::string& filename,
    const std::string& delimiter,
    const bool verbose
  )
  {
    if (verbose) {
      std::cout << "==== START getSequenceInfo" << std::endl;
    }

    SequenceHandler sequenceHandler;
    SequenceParser::readSequenceFile(sequenceHandler, filename, delimiter);

    std::ostringstream oss;
    oss << "Number of samples in the sequence file: " << sequenceHandler.getSequence().size() << "\n";
    oss << "Listing injections' names: \n";

    for (const SampleHandler& sampleHandler : sequenceHandler.getSequence()) {
      const MetaDataHandler& sample_meta_data = sampleHandler.getMetaData();
      oss << sample_meta_data.getInjectionName() << '\n';
    }

    if (verbose) {
      std::cout << "==== END   getSequenceInfo" << std::endl;
    }

    return oss.str();
  }

  std::string InputDataValidation::getParametersInfo(const std::string& filename, const bool verbose)
  {
    if (verbose) {
      std::cout << "==== START getParametersInfo" << std::endl;
    }

    std::map<std::string,std::vector<std::map<std::string,std::string>>> parameters;
    FileReader::parseOpenMSParams(filename, parameters);

    std::ostringstream oss;
    oss << "Number of functions affected by the parameters: " << parameters.size();
    oss << "Listing the functions' names and number of parameters: \n";

    for (const std::pair<std::string,std::vector<std::map<std::string,std::string>>>& p : parameters) {
      oss << p.first << ": " << p.second.size() << "\n";
    }

    if (verbose) {
      std::cout << "==== END   getParametersInfo" << std::endl;
    }

    return oss.str();
  }

  std::string InputDataValidation::getTraMLInfo(
    const std::string& filename,
    const std::string& format, // TODO: implement some logic to automatically find out the format? i.e. check if extension is .csv or .traML
    const bool verbose
  )
  {
    if (verbose) {
      std::cout << "==== START getTraMLInfo" << std::endl;
    }

    RawDataHandler rawDataHandler;
    OpenMSFile::loadTraML(rawDataHandler, filename, format, verbose);
    const OpenMS::TargetedExperiment targeted_exp = rawDataHandler.getTargetedExperiment();
    const std::vector<OpenMS::ReactionMonitoringTransition>& transitions = targeted_exp.getTransitions();

    std::ostringstream oss;
    oss << "Number of transitions: " << transitions.size();
    oss << "Listing transitions' information: \n";

    const std::string delimiter = ", ";

    for (const OpenMS::ReactionMonitoringTransition& t : transitions) {
      oss << t.getName() << delimiter
        << t.getNativeID() << delimiter
        << t.getPeptideRef() << delimiter
        << t.getCompoundRef() << delimiter
        << t.getPrecursorMZ() << delimiter
        << t.getProductMZ();
    }

    if (verbose) {
      std::cout << "==== END   getTraMLInfo" << std::endl;
    }

    return oss.str();
  }

  std::string InputDataValidation::getFeatureFilterComponentsAndGroupsInfo(
    const std::string& filename_components,
    const std::string& filename_components_groups,
    const bool verbose
  )
  {
    if (verbose) {
      std::cout << "==== START getFeatureFilterComponentsAndGroupsInfo" << std::endl;
    }

    RawDataHandler rawDataHandler;
    OpenMSFile::loadFeatureFilter(
      rawDataHandler,
      filename_components,
      filename_components_groups,
      verbose
    );

    const OpenMS::MRMFeatureQC& featureQC = rawDataHandler.getFeatureFilter();

    std::ostringstream oss;
    oss << "Number of ComponentQCs: " << featureQC.component_qcs.size();
    oss << "Number of ComponentGroupQCs: " << featureQC.component_group_qcs.size();

    oss << "Listing ComponentQCs' information: \n";
    for (const OpenMS::MRMFeatureQC::ComponentQCs& qc : featureQC.component_qcs) {
      oss << qc.component_name
        << " RT[" << qc.retention_time_l << "," << qc.retention_time_u << "]"
        << " INT[" << qc.intensity_l << "," << qc.intensity_u << "]"
        << " OQ[" << qc.overall_quality_l << "," << qc.overall_quality_u << "]"
        << " and # of metavalues pairs: " << qc.meta_value_qc.size() << '\n';
    }

    oss << "Listing ComponentGroupQCs' information: \n";
    for (const OpenMS::MRMFeatureQC::ComponentGroupQCs& qc : featureQC.component_group_qcs) {
      oss << qc.component_group_name
        << " RT[" << qc.retention_time_l << "," << qc.retention_time_u << "]"
        << " INT[" << qc.intensity_l << "," << qc.intensity_u << "]"
        << " OQ[" << qc.overall_quality_l << "," << qc.overall_quality_u << "]"
        // TODO: show more info? i.e. the group-related info
        << " and # of metavalues pairs: " << qc.meta_value_qc.size() << '\n';
    }

    if (verbose) {
      std::cout << "==== END   getFeatureFilterComponentsAndGroupsInfo" << std::endl;
    }

    return oss.str();
  }

  std::string InputDataValidation::getFeatureQCComponentsAndGroupsInfo(
    const std::string& filename_components,
    const std::string& filename_components_groups,
    const bool verbose
  )
  {
    if (verbose) {
      std::cout << "==== START getFeatureQCComponentsAndGroupsInfo" << std::endl;
    }

    RawDataHandler rawDataHandler;
    OpenMSFile::loadFeatureQC(
      rawDataHandler,
      filename_components,
      filename_components_groups,
      verbose
    );

    const OpenMS::MRMFeatureQC& featureQC = rawDataHandler.getFeatureQC();

    std::ostringstream oss;
    oss << "Number of ComponentQCs: " << featureQC.component_qcs.size();
    oss << "Number of ComponentGroupQCs: " << featureQC.component_group_qcs.size();

    oss << "Listing ComponentQCs' information: \n";
    for (const OpenMS::MRMFeatureQC::ComponentQCs& qc : featureQC.component_qcs) {
      oss << qc.component_name
        << " RT[" << qc.retention_time_l << "," << qc.retention_time_u << "]"
        << " INT[" << qc.intensity_l << "," << qc.intensity_u << "]"
        << " OQ[" << qc.overall_quality_l << "," << qc.overall_quality_u << "]"
        << " and # of metavalues pairs: " << qc.meta_value_qc.size() << '\n';
    }

    oss << "Listing ComponentGroupQCs' information: \n";
    for (const OpenMS::MRMFeatureQC::ComponentGroupQCs& qc : featureQC.component_group_qcs) {
      oss << qc.component_group_name
        << " RT[" << qc.retention_time_l << "," << qc.retention_time_u << "]"
        << " INT[" << qc.intensity_l << "," << qc.intensity_u << "]"
        << " OQ[" << qc.overall_quality_l << "," << qc.overall_quality_u << "]"
        // TODO: show more info? i.e. the group-related info
        << " and # of metavalues pairs: " << qc.meta_value_qc.size() << '\n';
    }

    if (verbose) {
      std::cout << "==== END   getFeatureQCComponentsAndGroupsInfo" << std::endl;
    }

    return oss.str();
  }

  std::string InputDataValidation::getQuantitationMethodsInfo(
    const std::string& filename,
    const bool verbose
  )
  {
    if (verbose) {
      std::cout << "==== START getQuantitationMethodsInfo" << std::endl;
    }

    SequenceSegmentHandler sequenceSegmentHandler;
    OpenMSFile::loadQuantitationMethods(
      sequenceSegmentHandler,
      filename,
      verbose
    );

    const std::vector<OpenMS::AbsoluteQuantitationMethod>& quantitation_methods =
      sequenceSegmentHandler.getQuantitationMethods();

    std::ostringstream oss;
    oss << "Number of quantitation methods: " << quantitation_methods.size();

    oss << "Listing quantitation methods' information: \n";

    const std::string delimiter = ", ";

    for (const OpenMS::AbsoluteQuantitationMethod& qm : quantitation_methods) {
      oss << qm.getComponentName() << delimiter
        << qm.getFeatureName() << delimiter
        << qm.getISName() << delimiter
        << "LOD[" << qm.getLLOD() << "," << qm.getULOD() << delimiter
        << "LOQ[" << qm.getLLOQ() << "," << qm.getULOQ() << delimiter
        << "#points[" << qm.getNPoints() << "]" << delimiter
        << "corr_coeff[" << qm.getCorrelationCoefficient() << "]" << delimiter
        << "conc_units[" << qm.getConcentrationUnits() << "]" << delimiter
        << "transf_model[" << qm.getTransformationModel() << "]\n";
    }

    if (verbose) {
      std::cout << "==== END   getQuantitationMethodsInfo" << std::endl;
    }

    return oss.str();
  }

  std::string InputDataValidation::getStandardsConcentrationsInfo(
    const std::string& filename,
    const bool verbose
  )
  {
    if (verbose) {
      std::cout << "==== START getStandardsConcentrationsInfo" << std::endl;
    }

    SequenceSegmentHandler sequenceSegmentHandler;
    OpenMSFile::loadStandardsConcentrations(
      sequenceSegmentHandler,
      filename,
      verbose
    );

    const std::vector<OpenMS::AbsoluteQuantitationStandards::runConcentration>& standards =
      sequenceSegmentHandler.getStandardsConcentrations();

    std::ostringstream oss;
    oss << "Number of standards concentrations: " << standards.size();

    oss << "Listing standards concentrations' information: \n";

    const std::string delimiter = ", ";

    for (const OpenMS::AbsoluteQuantitationStandards::runConcentration& conc : standards) {
      oss << conc.sample_name << delimiter
        << conc.component_name << delimiter
        << conc.IS_component_name << delimiter
        << conc.actual_concentration << delimiter
        << conc.IS_actual_concentration << delimiter
        << conc.concentration_units << delimiter
        << conc.dilution_factor << '\n';
    }

    if (verbose) {
      std::cout << "==== END   getStandardsConcentrationsInfo" << std::endl;
    }

    return oss.str();
  }
}
