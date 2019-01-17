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
        // TODO: show more info? i.e. the group-related info
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

    const bool check_passed1 = validateNamesInFiles(names1, names2, sequence_filename, standards_filename);
    const bool check_passed2 = validateNamesInFiles(names2, names1, standards_filename, sequence_filename);

    return check_passed1 && check_passed2;
  }

  bool InputDataValidation::validateNamesInFiles(
    const std::set<std::string>& names1,
    const std::set<std::string>& names2,
    const std::string& filename1,
    const std::string& filename2
  )
  {
    std::vector<std::string> missing1 = findMissingNames(names1, names2);
    std::vector<std::string> missing2 = findMissingNames(names2, names1);
    std::cout << logMissingNames(missing1, filename1, filename2);
    std::cout << logMissingNames(missing2, filename2, filename1);
    return missing1.empty() && missing2.empty();
  }

  std::vector<std::string> InputDataValidation::findMissingNames(
    const std::string& names,
    const std::string& bucket
  )
  {
    std::vector<std::string> missing;
    std::set_difference(names.begin(), names.end(), bucket.begin(), bucket.end(),
      std::inserter(missing, missing.end()));
    return missing;
  }

  void InputDataValidation::logMissingNames(
    const std::vector<std::string>& missing_names,
    const std::string& filename1,
    const std::string& filename2
  )
  {
    std::ostringstream oss;
    oss << "Names of \"" << filename1 << "\"";
    oss << "Verifying names of [" << filename1 << "] in [" << filename2 << "]: ";
    oss << missing.size() << " mismatches.\n";
    for (size_t i = 0; i < missing.size(); ++i) {
      oss << "[" << (i + 1) << "] " << missing[i] << "\n";
    }
    oss << "\n";
    return oss.str();
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

    for (size_t i = 0; i < transitions.size(); ++i) {
      names1.insert(transitions[i].getPeptideRef()); // getName, getNativeID, getPeptideRef, getCompoundRef
    }

    for (size_t i = 0; i < featureFilter.component_qcs.size(); ++i) {
      names2.insert(featureFilter.component_qcs[i].component_name);
    }

    for (size_t i = 0; i < featureQC.component_qcs.size(); ++i) {
      names3.insert(featureQC.component_qcs[i].component_name);
    }

    for (size_t i = 0; i < quantitation_methods.size(); ++i) {
      names4.insert(quantitation_methods[i].getComponentName());
    }

    for (size_t i = 0; i < standards.size(); ++i) {
      names5.insert(standards[i].component_name);
    }

    std::set<std::string>::iterator to_remove1 = std::remove_if(
      names1.begin(),
      names1.end(),
      [&names2, &names3, &names4, &names5] (std::string& s) {
        return names2.count(s) && names3.count(s) && names4.count(s) && names5.count(s);
      }
    );

    std::set<std::string>::iterator to_remove2 = std::remove_if(
      names2.begin(),
      names2.end(),
      [&names1, &names3, &names4, &names5] (std::string& s) {
        return names1.count(s) && names3.count(s) && names4.count(s) && names5.count(s);
      }
    );

    std::set<std::string>::iterator to_remove3 = std::remove_if(
      names3.begin(),
      names3.end(),
      [&names1, &names2, &names4, &names5] (std::string& s) {
        return names1.count(s) && names2.count(s) && names4.count(s) && names5.count(s);
      }
    );

    std::set<std::string>::iterator to_remove4 = std::remove_if(
      names4.begin(),
      names4.end(),
      [&names1, &names2, &names3, &names5] (std::string& s) {
        return names1.count(s) && names2.count(s) && names3.count(s) && names5.count(s);
      }
    );

    std::set<std::string>::iterator to_remove5 = std::remove_if(
      names5.begin(),
      names5.end(),
      [&names1, &names2, &names3, &names4] (std::string& s) {
        return names1.count(s) && names2.count(s) && names3.count(s) && names4.count(s);
      }
    );

    names1.erase(to_remove1, names1.end());
    names2.erase(to_remove2, names2.end());
    names3.erase(to_remove3, names3.end());
    names4.erase(to_remove4, names4.end());
    names5.erase(to_remove5, names5.end());

    return names1.empty() && names2.empty() && names3.empty() && names4.empty() && names5.empty();
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

    for (size_t i = 0; i < transitions.size(); ++i) {
      names1.insert(transitions[i].getCompoundRef()); // getName, getNativeID, getPeptideRef, getCompoundRef
    }

    for (size_t i = 0; i < featureFilter.component_group_qcs.size(); ++i) {
      names2.insert(featureFilter.component_group_qcs[i].component_group_name);
    }

    for (size_t i = 0; i < featureQC.component_group_qcs.size(); ++i) {
      names3.insert(featureQC.component_group_qcs[i].component_group_name);
    }

    std::set<std::string>::iterator to_remove1 = std::remove_if(
      names1.begin(),
      names1.end(),
      [&names2, &names3] (std::string& s) {
        return names2.count(s) && names3.count(s);
      }
    );

    std::set<std::string>::iterator to_remove2 = std::remove_if(
      names2.begin(),
      names2.end(),
      [&names1, &names3] (std::string& s) {
        return names1.count(s) && names3.count(s);
      }
    );

    std::set<std::string>::iterator to_remove3 = std::remove_if(
      names3.begin(),
      names3.end(),
      [&names1, &names2] (std::string& s) {
        return names1.count(s) && names2.count(s);
      }
    );

    names1.erase(to_remove1, names1.end());
    names2.erase(to_remove2, names2.end());
    names3.erase(to_remove3, names3.end());

    return names1.empty() && names2.empty() && names3.empty();
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
    std::set<std::string> names4;
    std::set<std::string> names5;

    for (size_t i = 0; i < transitions.size(); ++i) {
      names1.insert(transitions[i].getPeptideRef()); // getName, getNativeID, getPeptideRef, getCompoundRef
    }

    for (size_t i = 0; i < quantitation_methods.size(); ++i) {
      names4.insert(quantitation_methods[i].getComponentName());
    }

    for (size_t i = 0; i < standards.size(); ++i) {
      names5.insert(standards[i].component_name);
    }

    std::set<std::string>::iterator to_remove1 = std::remove_if(
      names1.begin(),
      names1.end(),
      [&names4, &names5] (std::string& s) {
        return names4.count(s) && names5.count(s);
      }
    );

    std::set<std::string>::iterator to_remove4 = std::remove_if(
      names4.begin(),
      names4.end(),
      [&names1, &names5] (std::string& s) {
        return names1.count(s) && names5.count(s);
      }
    );

    std::set<std::string>::iterator to_remove5 = std::remove_if(
      names5.begin(),
      names5.end(),
      [&names1, &names4] (std::string& s) {
        return names1.count(s) && names4.count(s);
      }
    );

    names1.erase(to_remove1, names1.end());
    names4.erase(to_remove4, names4.end());
    names5.erase(to_remove5, names5.end());

    return names1.empty() && names4.empty() && names5.empty();
  }
}
