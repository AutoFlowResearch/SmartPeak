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
    SequenceParser::readSequenceFile(sequenceHandler, filename, delimiter, verbose);

    std::ostringstream oss;
    oss << "Number of samples in the sequence file: " << sequenceHandler.getSequence().size() << "\n";
    oss << "Listing injections' name and type: \n";

    for (const SampleHandler& sampleHandler : sequenceHandler.getSequence()) {
      const MetaDataHandler& sample_meta_data = sampleHandler.getMetaData();
      oss << sample_meta_data.getInjectionName() << "\t" << MetaDataHandler::SampleTypeToString(sample_meta_data.sample_type) << "\n";
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
    const bool verbose
  )
  {
    if (verbose) {
      std::cout << "==== START getTraMLInfo" << std::endl;
    }

    RawDataHandler rawDataHandler;
    OpenMSFile::loadTraML(rawDataHandler, filename, "csv", verbose);
    const std::vector<OpenMS::ReactionMonitoringTransition>& transitions =
      rawDataHandler.getTargetedExperiment().getTransitions();

    std::ostringstream oss;
    oss << "Number of transitions: " << transitions.size();
    oss << "Listing transitions' information: \n";

    const std::string delimiter = "\n";

    for (const OpenMS::ReactionMonitoringTransition& t : transitions) {
      const std::string rt = t.getRetentionTime().isRTset()
        ? std::to_string(t.getRetentionTime().getRT())
        : "NA";
      oss << "Transition:\t" << t.getName() << delimiter
        << "Native ID:\t" << t.getNativeID() << delimiter
        << "Peptide Ref:\t" << t.getPeptideRef() << delimiter
        << "Compound Ref:\t" << t.getCompoundRef() << delimiter
        << "Precursor MZ:\t" << t.getPrecursorMZ() << delimiter
        << "Product MZ:\t" << t.getProductMZ() << delimiter
        << "Transition RT:\t" << rt << "\n\n";
    }

    if (verbose) {
      std::cout << "==== END   getTraMLInfo" << std::endl;
    }

    return oss.str();
  }

  std::string InputDataValidation::getComponentsAndGroupsInfo(
    const std::string& filename_components,
    const std::string& filename_components_groups,
    const bool is_feature_filter, // else is feature qc
    const bool verbose
  )
  {
    if (verbose) {
      std::cout << "==== START getComponentsAndGroupsInfo" << std::endl;
    }

    RawDataHandler rawDataHandler;
    if (is_feature_filter) {
      OpenMSFile::loadFeatureFilter(
        rawDataHandler,
        filename_components,
        filename_components_groups,
        verbose
      );
    } else {
      OpenMSFile::loadFeatureQC(
        rawDataHandler,
        filename_components,
        filename_components_groups,
        verbose
      );
    }

    const OpenMS::MRMFeatureQC& featureQC = is_feature_filter
      ? rawDataHandler.getFeatureFilter()
      : rawDataHandler.getFeatureQC();

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
        << " and # of metavalues pairs: " << qc.meta_value_qc.size() << '\n';
    }

    if (verbose) {
      std::cout << "==== END   getComponentsAndGroupsInfo" << std::endl;
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

    const std::string delimiter = "\n";

    for (const OpenMS::AbsoluteQuantitationMethod& qm : quantitation_methods) {
      oss << "Component name:\t" << qm.getComponentName() << delimiter
        << "Feature name:\t" << qm.getFeatureName() << delimiter
        << "IS name:\t"      << qm.getISName() << delimiter
        << "LOD:\t["         << qm.getLLOD() << ", " << qm.getULOD() << "]" << delimiter
        << "LOQ:\t["         << qm.getLLOQ() << ", " << qm.getULOQ() << "]" << delimiter
        << "N. points:\t"    << qm.getNPoints() << delimiter
        << "Corr. coeff.:\t"   << qm.getCorrelationCoefficient() << delimiter
        << "Conc. units:\t"   << qm.getConcentrationUnits() << delimiter
        << "Transf. model:\t" << qm.getTransformationModel() << delimiter;
      if (qm.getTransformationModelParams().size()) {
        oss << "Transf. model params:\n";
        for (const OpenMS::Param::ParamEntry& param : qm.getTransformationModelParams()) {
          oss << "  " << param.name << ":\t" << param.value << "\n";
        }
      }
      oss << "\n";
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

  bool InputDataValidation::sampleNamesAreConsistent(
    const std::string& sequence_filename,
    const std::string& delimiter,
    const std::string& standards_filename
  )
  {
    SequenceHandler sequenceHandler;
    SequenceSegmentHandler sequenceSegmentHandler;

    SequenceParser::readSequenceFile(sequenceHandler, sequence_filename, delimiter, false);
    OpenMSFile::loadStandardsConcentrations(sequenceSegmentHandler, standards_filename, false);

    const std::vector<SampleHandler>& samples = sequenceHandler.getSequence();
    const std::vector<OpenMS::AbsoluteQuantitationStandards::runConcentration>& standards =
      sequenceSegmentHandler.getStandardsConcentrations();

    std::set<std::string> names1;
    std::set<std::string> names2;

    for (const SampleHandler& sample : samples) {
      names1.insert(sample.getMetaData().sample_name);
    }

    for (const OpenMS::AbsoluteQuantitationStandards::runConcentration& run : standards) {
      names2.insert(run.sample_name);
    }

    const bool check_passed = validateNamesInFiles(names1, names2, sequence_filename, standards_filename);

    return check_passed;
  }

  bool InputDataValidation::componentNamesAreConsistent(
    const std::string& traML_filename,
    const std::string& featureFilter_filename,
    const std::string& featureQC_filename,
    const std::string& quantitationMethods_filename,
    const std::string& standardConcentrations_filename
  )
  {
    RawDataHandler rawDataHandler;
    SequenceSegmentHandler sequenceSegmentHandler;

    OpenMSFile::loadTraML(rawDataHandler, traML_filename, "csv", false);
    OpenMSFile::loadFeatureFilter(rawDataHandler, featureFilter_filename, "", false);
    OpenMSFile::loadFeatureQC(rawDataHandler, featureQC_filename, "", false);
    OpenMSFile::loadQuantitationMethods(sequenceSegmentHandler, quantitationMethods_filename, false);
    OpenMSFile::loadStandardsConcentrations(sequenceSegmentHandler, standardConcentrations_filename, false);

    const std::vector<OpenMS::ReactionMonitoringTransition>& transitions =
      rawDataHandler.getTargetedExperiment().getTransitions();
    const OpenMS::MRMFeatureQC& featureFilter = rawDataHandler.getFeatureFilter();
    const OpenMS::MRMFeatureQC& featureQC = rawDataHandler.getFeatureQC();
    const std::vector<OpenMS::AbsoluteQuantitationMethod>& quantitation_methods =
      sequenceSegmentHandler.getQuantitationMethods();
    const std::vector<OpenMS::AbsoluteQuantitationStandards::runConcentration>& standards =
      sequenceSegmentHandler.getStandardsConcentrations();

    std::set<std::string> names1;
    std::set<std::string> names2;
    std::set<std::string> names3;
    std::set<std::string> names4;
    std::set<std::string> names5;

    for (const OpenMS::ReactionMonitoringTransition& transition : transitions) {
      names1.insert(transition.getPeptideRef()); // getName, getNativeID, getPeptideRef, getCompoundRef
    }

    for (const OpenMS::MRMFeatureQC::ComponentQCs& qc : featureFilter.component_qcs) {
      names2.insert(qc.component_name);
    }

    for (const OpenMS::MRMFeatureQC::ComponentQCs& qc : featureQC.component_qcs) {
      names3.insert(qc.component_name);
    }

    for (const OpenMS::AbsoluteQuantitationMethod& aqm : quantitation_methods) {
      names4.insert(aqm.getComponentName());
    }

    for (const OpenMS::AbsoluteQuantitationStandards::runConcentration& run : standards) {
      names5.insert(run.component_name);
    }

    std::vector<bool> check_passed(10);

    check_passed[0] = validateNamesInFiles(names1, names2, traML_filename, featureFilter_filename);
    check_passed[1] = validateNamesInFiles(names1, names3, traML_filename, featureQC_filename);
    check_passed[2] = validateNamesInFiles(names1, names4, traML_filename, quantitationMethods_filename);
    check_passed[3] = validateNamesInFiles(names1, names5, traML_filename, standardConcentrations_filename);
    check_passed[4] = validateNamesInFiles(names2, names3, featureFilter_filename, featureQC_filename);
    check_passed[5] = validateNamesInFiles(names2, names4, featureFilter_filename, quantitationMethods_filename);
    check_passed[6] = validateNamesInFiles(names2, names5, featureFilter_filename, standardConcentrations_filename);
    check_passed[7] = validateNamesInFiles(names3, names4, featureQC_filename, quantitationMethods_filename);
    check_passed[8] = validateNamesInFiles(names3, names5, featureQC_filename, standardConcentrations_filename);
    check_passed[9] = validateNamesInFiles(names4, names5, quantitationMethods_filename, standardConcentrations_filename);

    return std::all_of(check_passed.cbegin(), check_passed.cend(), [](const bool has_passed){ return has_passed; });
  }

  bool InputDataValidation::componentNameGroupsAreConsistent(
    const std::string& traML_filename,
    const std::string& featureFilter_filename,
    const std::string& featureQC_filename
  )
  {
    RawDataHandler rawDataHandler;
    SequenceSegmentHandler sequenceSegmentHandler;

    OpenMSFile::loadTraML(rawDataHandler, traML_filename, "csv", false);
    OpenMSFile::loadFeatureFilter(rawDataHandler, "", featureFilter_filename, false);
    OpenMSFile::loadFeatureQC(rawDataHandler, "", featureQC_filename, false);

    const std::vector<OpenMS::ReactionMonitoringTransition>& transitions =
      rawDataHandler.getTargetedExperiment().getTransitions();
    const OpenMS::MRMFeatureQC& featureFilter = rawDataHandler.getFeatureFilter();
    const OpenMS::MRMFeatureQC& featureQC = rawDataHandler.getFeatureQC();

    std::set<std::string> names1;
    std::set<std::string> names2;
    std::set<std::string> names3;

    for (const OpenMS::ReactionMonitoringTransition& transition : transitions) {
      names1.insert(transition.getCompoundRef()); // getName, getNativeID, getPeptideRef, getCompoundRef
    }

    for (const OpenMS::MRMFeatureQC::ComponentGroupQCs& qc : featureFilter.component_group_qcs) {
      names2.insert(qc.component_group_name);
    }

    for (const OpenMS::MRMFeatureQC::ComponentGroupQCs& qc : featureQC.component_group_qcs) {
      names3.insert(qc.component_group_name);
    }

    std::vector<bool> check_passed(3);

    check_passed[0] = validateNamesInFiles(names1, names2, traML_filename, featureFilter_filename);
    check_passed[1] = validateNamesInFiles(names1, names3, traML_filename, featureQC_filename);
    check_passed[2] = validateNamesInFiles(names2, names3, featureFilter_filename, featureQC_filename);

    return std::all_of(check_passed.cbegin(), check_passed.cend(), [](const bool has_passed){ return has_passed; });
  }

  bool InputDataValidation::heavyComponentsAreConsistent(
    const std::string& traML_filename,
    const std::string& quantitationMethods_filename,
    const std::string& standardConcentrations_filename
  )
  {
    RawDataHandler rawDataHandler;
    SequenceSegmentHandler sequenceSegmentHandler;

    OpenMSFile::loadTraML(rawDataHandler, traML_filename, "csv", false);
    OpenMSFile::loadQuantitationMethods(sequenceSegmentHandler, quantitationMethods_filename, false);
    OpenMSFile::loadStandardsConcentrations(sequenceSegmentHandler, standardConcentrations_filename, false);

    const std::vector<OpenMS::ReactionMonitoringTransition>& transitions =
      rawDataHandler.getTargetedExperiment().getTransitions();
    const std::vector<OpenMS::AbsoluteQuantitationMethod>& quantitation_methods =
      sequenceSegmentHandler.getQuantitationMethods();
    const std::vector<OpenMS::AbsoluteQuantitationStandards::runConcentration>& standards =
      sequenceSegmentHandler.getStandardsConcentrations();

    std::set<std::string> names1;
    std::set<std::string> names2;
    std::set<std::string> names3;

    for (const OpenMS::ReactionMonitoringTransition& transition : transitions) {
      names1.insert(transition.getPeptideRef()); // getName, getNativeID, getPeptideRef, getCompoundRef
    }

    for (const OpenMS::AbsoluteQuantitationMethod& aqm : quantitation_methods) {
      names2.insert(aqm.getComponentName());
    }

    for (const OpenMS::AbsoluteQuantitationStandards::runConcentration& run : standards) {
      names3.insert(run.component_name);
    }

    std::vector<bool> check_passed(3);

    check_passed[0] = validateNamesInFiles(names1, names2, traML_filename, quantitationMethods_filename);
    check_passed[1] = validateNamesInFiles(names1, names3, traML_filename, standardConcentrations_filename);
    check_passed[2] = validateNamesInFiles(names2, names3, quantitationMethods_filename, standardConcentrations_filename);

    return std::all_of(check_passed.cbegin(), check_passed.cend(), [](const bool has_passed){ return has_passed; });
  }

  bool InputDataValidation::validateNamesInFiles(
    const std::set<std::string>& names1,
    const std::set<std::string>& names2,
    const std::string& filename1,
    const std::string& filename2
  )
  {
    const std::set<std::string> missing1 = findMissingNames(names1, names2);
    const std::set<std::string> missing2 = findMissingNames(names2, names1);
    std::cout << logMissingNames(missing1, filename1, filename2);
    std::cout << logMissingNames(missing2, filename2, filename1);
    return missing1.empty() && missing2.empty();
  }

  std::set<std::string> InputDataValidation::findMissingNames(
    const std::set<std::string>& names,
    const std::set<std::string>& bucket
  )
  {
    std::set<std::string> missing;
    std::set_difference(names.begin(), names.end(), bucket.begin(), bucket.end(),
      std::inserter(missing, missing.end()));
    return missing;
  }

  std::string InputDataValidation::logMissingNames(
    const std::set<std::string>& missing_names,
    const std::string& filename1,
    const std::string& filename2
  )
  {
    std::ostringstream oss;
    oss << "\nComparing names...\n"
      << "      From: " << filename1 << "\n"
      << "        To: " << filename2 << "\n"
      << "Mismatches: " << missing_names.size() << "\n";
    size_t i = 1;
    for (const std::string& name : missing_names) {
      oss << "[" << i++ << "] " << name << "\n";
    }
    oss << "\n";
    return oss.str();
  }
}
