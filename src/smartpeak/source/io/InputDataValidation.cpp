// TODO: Add copyright

#include <SmartPeak/io/InputDataValidation.h>
#include <SmartPeak/core/Filenames.h>
#include <SmartPeak/core/SequenceHandler.h>
#include <SmartPeak/io/FileReader.h>
#include <SmartPeak/io/OpenMSFile.h>
#include <SmartPeak/io/SequenceParser.h>
#include <fstream>
#include <string>
#include <unordered_map>

namespace SmartPeak
{
  bool InputDataValidation::fileExists(const std::string& filepath)
  {
    std::ifstream ifs(filepath);
    return ifs.is_open();
  }

  bool InputDataValidation::isValidFilename(const std::string& filename, const std::string& member_name)
  {
    std::cout << "[" << member_name << "]:\t";
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
    const SequenceHandler& sequenceHandler,
    const std::string& delimiter
  )
  {
    std::ostringstream oss;
    oss << "==== START getSequenceInfo\n" <<
      "Number of samples in the sequence file: " <<
      sequenceHandler.getSequence().size() <<
      "\nListing injections' name and type:\n";

    for (const InjectionHandler& sampleHandler : sequenceHandler.getSequence()) {
      const MetaDataHandler& sample_meta_data = sampleHandler.getMetaData();
      oss << sample_meta_data.getInjectionName() << "\t" << MetaDataHandler::SampleTypeToString(sample_meta_data.sample_type) << "\n";
    }

    oss << "==== END   getSequenceInfo\n";
    return oss.str();
  }

  std::string InputDataValidation::getParametersInfo(
    const std::map<std::string,std::vector<std::map<std::string,std::string>>>& parameters
  )
  {
    std::ostringstream oss;
    oss << "==== START getParametersInfo\n" <<
      "Number of functions affected by the parameters: " << parameters.size() <<
      "\nListing the functions' names and number of parameters:\n";

    for (const std::pair<std::string,std::vector<std::map<std::string,std::string>>>& p : parameters) {
      oss << p.first << ": " << p.second.size() << "\n";
    }

    oss << "==== END   getParametersInfo\n";
    return oss.str();
  }

  std::string InputDataValidation::getTraMLInfo(
    const RawDataHandler& rawDataHandler
  )
  {
    const std::vector<OpenMS::ReactionMonitoringTransition>& transitions =
      rawDataHandler.getTargetedExperiment().getTransitions();

    const std::vector<OpenMS::TargetedExperiment::Peptide>& peptides =
      rawDataHandler.getTargetedExperiment().getPeptides();

    std::ostringstream oss;
    oss << "==== START getTraMLInfo\n" <<
      "Number of transitions: " << transitions.size() <<
      "\nListing transitions' information:\n";

    std::unordered_map<std::string, double> rts;
    for (const OpenMS::TargetedExperiment::Peptide& peptide : peptides) {
      rts[peptide.id] = peptide.getRetentionTime();
    }

    const std::string delimiter = "\n";

    for (const OpenMS::ReactionMonitoringTransition& t : transitions) {
      const std::string& peptide_ref = t.getPeptideRef();
      const std::string rt = rts.count(peptide_ref)
        ? std::to_string(rts[peptide_ref])
        : "Peptide not found";
      oss << "Transition:\t" << t.getName() << delimiter
        << "Native ID:\t" << t.getNativeID() << delimiter
        << "Peptide Ref:\t" << peptide_ref << delimiter
        << "Compound Ref:\t" << t.getCompoundRef() << delimiter
        << "Precursor MZ:\t" << t.getPrecursorMZ() << delimiter
        << "Product MZ:\t" << t.getProductMZ() << delimiter
        << "Transition RT:\t" << rt << "\n\n";
    }

    oss << "==== END   getTraMLInfo\n";
    return oss.str();
  }

  std::string InputDataValidation::getComponentsAndGroupsInfo(
    const RawDataHandler& rawDataHandler,
    const bool is_feature_filter // else is feature qc
  )
  {
    const OpenMS::MRMFeatureQC& featureQC = is_feature_filter
      ? rawDataHandler.getFeatureFilter()
      : rawDataHandler.getFeatureQC();

    std::ostringstream oss;
    oss << "==== START getComponentsAndGroupsInfo " <<
      (is_feature_filter ? "(FeatureFilter)" : "(FeatureQC)") << "\n" <<
      "Number of ComponentQCs: " << featureQC.component_qcs.size() << "\n" <<
      "Number of ComponentGroupQCs: " << featureQC.component_group_qcs.size() << "\n";

    oss << "Listing ComponentQCs' information:\n";
    for (const OpenMS::MRMFeatureQC::ComponentQCs& qc : featureQC.component_qcs) {
      oss << qc.component_name
        << "\tRT[" << qc.retention_time_l << ", " << qc.retention_time_u << "]"
        << "\tINT[" << qc.intensity_l << ", " << qc.intensity_u << "]"
        << "\tOQ[" << qc.overall_quality_l << ", " << qc.overall_quality_u << "]"
        << "\tn. of metavalues pairs: " << qc.meta_value_qc.size() << '\n';
    }

    oss << "Listing ComponentGroupQCs' information:\n";
    for (const OpenMS::MRMFeatureQC::ComponentGroupQCs& qc : featureQC.component_group_qcs) {
      oss << qc.component_group_name
        << "\tRT[" << qc.retention_time_l << ", " << qc.retention_time_u << "]"
        << "\tINT[" << qc.intensity_l << ", " << qc.intensity_u << "]"
        << "\tOQ[" << qc.overall_quality_l << ", " << qc.overall_quality_u << "]"
        << "\tn. of metavalues pairs: " << qc.meta_value_qc.size() << '\n';
    }

    oss << "==== END   getComponentsAndGroupsInfo " <<
      (is_feature_filter ? "(FeatureFilter)" : "(FeatureQC)") << "\n";
    return oss.str();
  }

  std::string InputDataValidation::getQuantitationMethodsInfo(
    const SequenceSegmentHandler& sequenceSegmentHandler
  )
  {
    const std::vector<OpenMS::AbsoluteQuantitationMethod>& quantitation_methods =
      sequenceSegmentHandler.getQuantitationMethods();

    std::ostringstream oss;
    oss << "==== START getQuantitationMethodsInfo\n" <<
      "Number of quantitation methods: " << quantitation_methods.size() << "\n" <<
      "Listing quantitation methods' information:\n";

    const std::string delimiter = "\n";

    for (const OpenMS::AbsoluteQuantitationMethod& qm : quantitation_methods) {
      oss << "Component name:\t" << qm.getComponentName() << delimiter <<
        "Feature name:\t"  << qm.getFeatureName() << delimiter <<
        "IS name:\t"       << qm.getISName() << delimiter <<
        "LOD:\t["          << qm.getLLOD() << ", " << qm.getULOD() << "]" << delimiter <<
        "LOQ:\t["          << qm.getLLOQ() << ", " << qm.getULOQ() << "]" << delimiter <<
        "N. points:\t"     << qm.getNPoints() << delimiter <<
        "Corr. coeff.:\t"  << qm.getCorrelationCoefficient() << delimiter <<
        "Conc. units:\t"   << qm.getConcentrationUnits() << delimiter <<
        "Transf. model:\t" << qm.getTransformationModel() << delimiter;
      if (qm.getTransformationModelParams().size()) {
        oss << "Transf. model params:\n";
        for (const OpenMS::Param::ParamEntry& param : qm.getTransformationModelParams()) {
          oss << "  " << param.name << ":\t" << param.value << "\n";
        }
      }
      oss << "\n";
    }

    oss << "==== END   getQuantitationMethodsInfo\n";
    return oss.str();
  }

  std::string InputDataValidation::getStandardsConcentrationsInfo(
    const SequenceSegmentHandler& sequenceSegmentHandler
  )
  {
    const std::vector<OpenMS::AbsoluteQuantitationStandards::runConcentration>& standards =
      sequenceSegmentHandler.getStandardsConcentrations();

    std::ostringstream oss;
    oss << "==== START getStandardsConcentrationsInfo\n" <<
      "Number of standards concentrations: " << standards.size() << "\n" <<
      "Listing standards concentrations' information:\n";

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

    oss << "==== END   getStandardsConcentrationsInfo\n";
    return oss.str();
  }

  bool InputDataValidation::sampleNamesAreConsistent(
    const SequenceHandler& sequenceHandler
  )
  {
    const std::vector<InjectionHandler>& samples = sequenceHandler.getSequence();
    const std::vector<OpenMS::AbsoluteQuantitationStandards::runConcentration>& standards =
      sequenceHandler.getSequenceSegments().front().getStandardsConcentrations();

    std::set<std::string> names1;
    std::set<std::string> names2;

    for (const InjectionHandler& injection : samples) {
      names1.insert(injection.getMetaData().sample_name);
    }

    for (const OpenMS::AbsoluteQuantitationStandards::runConcentration& run : standards) {
      names2.insert(run.sample_name);
    }

    const bool check_passed = validateNamesInStructures(names1, names2, "SEQUENCE", "STANDARDS", false);

    return check_passed;
  }

  bool InputDataValidation::componentNamesAreConsistent(
    const SequenceHandler& sequenceHandler
  )
  {
    const RawDataHandler& rawDataHandler = sequenceHandler.getSequence().front().getRawData();
    const std::vector<OpenMS::ReactionMonitoringTransition>& transitions =
      rawDataHandler.getTargetedExperiment().getTransitions();
    const OpenMS::MRMFeatureQC& featureFilter = rawDataHandler.getFeatureFilter();
    const OpenMS::MRMFeatureQC& featureQC = rawDataHandler.getFeatureQC();
    const std::vector<OpenMS::AbsoluteQuantitationMethod>& quantitation_methods =
      sequenceHandler.getSequenceSegments().front().getQuantitationMethods();
    const std::vector<OpenMS::AbsoluteQuantitationStandards::runConcentration>& standards =
      sequenceHandler.getSequenceSegments().front().getStandardsConcentrations();

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

    check_passed[0] = validateNamesInStructures(names1, names2, "TRAML", "FEATUREFILTER", false);
    check_passed[1] = validateNamesInStructures(names1, names3, "TRAML", "FEATUREQC", false);
    check_passed[2] = validateNamesInStructures(names1, names4, "TRAML", "QUANTITATIONMETHODS", false);
    check_passed[3] = validateNamesInStructures(names1, names5, "TRAML", "STANDARDS", false);
    // check_passed[4] = validateNamesInStructures(names2, names3, "FEATUREFILTER", "FEATUREQC");
    // check_passed[5] = validateNamesInStructures(names2, names4, "FEATUREFILTER", "QUANTITATIONMETHODS");
    // check_passed[6] = validateNamesInStructures(names2, names5, "FEATUREFILTER", "STANDARDS");
    // check_passed[7] = validateNamesInStructures(names3, names4, "FEATUREQC", "QUANTITATIONMETHODS");
    // check_passed[8] = validateNamesInStructures(names3, names5, "FEATUREQC", "STANDARDS");
    // check_passed[9] = validateNamesInStructures(names4, names5, "QUANTITATIONMETHODS", "STANDARDS");

    return std::all_of(check_passed.cbegin(), check_passed.cend(), [](const bool has_passed){ return has_passed; });
  }

  bool InputDataValidation::componentNameGroupsAreConsistent(
    const SequenceHandler& sequenceHandler
  )
  {
    const RawDataHandler& rawDataHandler = sequenceHandler.getSequence().front().getRawData();
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

    check_passed[0] = validateNamesInStructures(names1, names2, "TRAML", "FEATUREFILTER", false);
    check_passed[1] = validateNamesInStructures(names1, names3, "TRAML", "FEATUREQC", false);
    // check_passed[2] = validateNamesInStructures(names2, names3, "FEATUREFILTER", "FEATUREQC");

    return std::all_of(check_passed.cbegin(), check_passed.cend(), [](const bool has_passed){ return has_passed; });
  }

  bool InputDataValidation::heavyComponentsAreConsistent(
    const SequenceHandler& sequenceHandler
  )
  {
    const std::vector<OpenMS::ReactionMonitoringTransition>& transitions =
      sequenceHandler.getSequence().front().getRawData().getTargetedExperiment().getTransitions();
    const std::vector<OpenMS::AbsoluteQuantitationMethod>& quantitation_methods =
      sequenceHandler.getSequenceSegments().front().getQuantitationMethods();
    const std::vector<OpenMS::AbsoluteQuantitationStandards::runConcentration>& standards =
      sequenceHandler.getSequenceSegments().front().getStandardsConcentrations();

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

    check_passed[0] = validateNamesInStructures(names1, names2, "TRAML", "QUANTITATIONMETHODS", false);
    check_passed[1] = validateNamesInStructures(names1, names3, "TRAML", "STANDARDS", false);
    // check_passed[2] = validateNamesInStructures(names2, names3, "QUANTITATIONMETHODS", "STANDARDS");

    return std::all_of(check_passed.cbegin(), check_passed.cend(), [](const bool has_passed){ return has_passed; });
  }

  bool InputDataValidation::validateNamesInStructures(
    const std::set<std::string>& names1,
    const std::set<std::string>& names2,
    const std::string& structure_ref1,
    const std::string& structure_ref2,
    const bool check_both_directions
  )
  {
    const std::set<std::string> missing1 = findMissingNames(names1, names2);
    if (missing1.size()) {
      std::cout << logMissingNames(missing1, structure_ref1, structure_ref2);
    }
    std::set<std::string> missing2;
    if (check_both_directions) {
      missing2 = findMissingNames(names2, names1);
      if (missing2.size()) {
        std::cout << logMissingNames(missing2, structure_ref2, structure_ref1);
      }
    }
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
    const std::string& structure_ref1,
    const std::string& structure_ref2
  )
  {
    std::ostringstream oss;
    oss << "\nComparing names...\n"
      << "      From: " << structure_ref1 << "\n"
      << "        To: " << structure_ref2 << "\n"
      << "Missing: " << missing_names.size() << "\n";
    size_t i = 1;
    for (const std::string& name : missing_names) {
      oss << "[" << i++ << "] " << name << "\n";
    }
    oss << "\n";
    return oss.str();
  }
}
