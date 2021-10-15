// --------------------------------------------------------------------------
//   SmartPeak -- Fast and Accurate CE-, GC- and LC-MS(/MS) Data Processing
// --------------------------------------------------------------------------
// Copyright The SmartPeak Team -- Novo Nordisk Foundation 
// Center for Biosustainability, Technical University of Denmark 2018-2021.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL ANY OF THE AUTHORS OR THE CONTRIBUTING
// INSTITUTIONS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
// OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
// WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
// OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
// ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// --------------------------------------------------------------------------
// $Maintainer: Douglas McCloskey $
// $Authors: Douglas McCloskey, Pasquale Domenico Colaianni $
// --------------------------------------------------------------------------

#include <SmartPeak/io/InputDataValidation.h>
#include <SmartPeak/core/Filenames.h>
#include <SmartPeak/core/SampleType.h>
#include <SmartPeak/core/SequenceHandler.h>
#include <SmartPeak/io/ParametersParser.h>
#include <SmartPeak/io/SequenceParser.h>
#include <fstream>
#include <string>
#include <unordered_map>
#include <plog/Log.h>

namespace SmartPeak
{
  bool InputDataValidation::fileExists(const std::filesystem::path& filepath)
  {
    std::ifstream ifs(filepath);
    return ifs.is_open();
  }

  bool InputDataValidation::precheckProcessorInputs(
    const IFilenamesHandler& input_files,
    const std::string& processor_name,
    const Filenames& filenames_I,
    bool required)
  {
    Filenames processor_filenames;
    input_files.getFilenames(processor_filenames);

    Filenames merged_filenames = filenames_I;
    merged_filenames.merge(processor_filenames);

    bool valid = true;
    std::ostringstream msg;
    msg << processor_name << "\n";
    for (const auto& processor_file : processor_filenames.getFileIds())
    {
      const auto& full_path = merged_filenames.getFullPath(processor_file);
      msg << processor_file << " [" << full_path << "] ";
      if (full_path.empty())
      {
        if (required)
        {
          msg << "FAILURE\n";
          valid = false;
        }
        else
        {
          msg << "NOT PROVIDED\n";
        }
      }
      const bool file_exists = fileExists(full_path);
      if (!file_exists)
      {
        if (required)
        {
          msg << "FAILURE\n";
          valid = false;
        }
        else
        {
          msg << "NOT PROVIDED\n";
        }
      }
      else
      {
        msg << "SUCCESS\n";
      }
    }
    if (!valid)
    {
      LOGE << msg.str();
    }
    else
    {
      LOGI << msg.str();
    }
    return valid;
  }

  std::string InputDataValidation::getSequenceInfo(
    const SequenceHandler& sequenceHandler
  )
  {
    std::ostringstream oss;
    oss << "==== START getSequenceInfo\n" <<
      "Number of samples in the sequence file: " <<
      sequenceHandler.getSequence().size() <<
      "\nListing injections' name and type:\n";

    for (const InjectionHandler& sampleHandler : sequenceHandler.getSequence()) {
      const MetaDataHandler& sample_meta_data = sampleHandler.getMetaData();
      oss << sample_meta_data.getInjectionName() << "\t" << sampleTypeToString.at(sample_meta_data.getSampleType()) << "\n";
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
        // << "Native ID:\t" << t.getNativeID() << delimiter
        << "Peptide Ref:\t" << peptide_ref << delimiter
        // << "Compound Ref:\t" << t.getCompoundRef() << delimiter
        << "Precursor MZ:\t" << t.getPrecursorMZ() << delimiter
        << "Product MZ:\t" << t.getProductMZ() << delimiter
        << "Transition RT:\t" << rt << "\n\n";
    }

    oss << "==== END   getTraMLInfo\n";
    return oss.str();
  }

  std::string InputDataValidation::getFeatureFiltersInfo(
    const RawDataHandler& rawDataHandler,
    const bool is_feature_filter // else is feature qc
  )
  {
    const OpenMS::MRMFeatureQC& featureQC = is_feature_filter
      ? rawDataHandler.getFeatureFilter()
      : rawDataHandler.getFeatureQC();

    std::ostringstream oss;
    oss << "==== START getFeatureFiltersInfo " <<
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

    oss << "==== END   getFeatureFiltersInfo " <<
      (is_feature_filter ? "(FeatureFilter)" : "(FeatureQC)") << "\n";
    return oss.str();
  }

  std::string InputDataValidation::getFeatureRSDFiltersInfo(const RawDataHandler & rawDataHandler, const bool is_feature_filter)
  {
    const OpenMS::MRMFeatureQC& featureQC = is_feature_filter
      ? rawDataHandler.getFeatureRSDFilter()
      : rawDataHandler.getFeatureRSDQC();

    std::ostringstream oss;
    oss << "==== START getFeatureRSDFiltersInfo " <<
      (is_feature_filter ? "(FeatureRSDFilter)" : "(FeatureRSDQC)") << "\n" <<
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

    oss << "==== END   getFeatureFiltersInfo " <<
      (is_feature_filter ? "(FeatureRSDFilter)" : "(FeatureRSDQC)") << "\n";
    return oss.str();
  }

  std::string InputDataValidation::getFeatureBackgroundFiltersInfo(const RawDataHandler & rawDataHandler, const bool is_feature_filter)
  {
    const OpenMS::MRMFeatureQC& featureQC = is_feature_filter
      ? rawDataHandler.getFeatureBackgroundFilter()
      : rawDataHandler.getFeatureBackgroundQC();

    std::ostringstream oss;
    oss << "==== START getFeatureBackgroundFiltersInfo " <<
      (is_feature_filter ? "(FeatureBackgroundFilter)" : "(FeatureBackgroundQC)") << "\n" <<
      "Number of ComponentQCs: " << featureQC.component_qcs.size() << "\n" <<
      "Number of ComponentGroupQCs: " << featureQC.component_group_qcs.size() << "\n";

    oss << "Listing ComponentQCs' information:\n";
    for (const OpenMS::MRMFeatureQC::ComponentQCs& qc : featureQC.component_qcs) {
      oss << qc.component_name
        << "\tINT[" << qc.intensity_l << ", " << qc.intensity_u << "]\n";
    }

    oss << "Listing ComponentGroupQCs' information:\n";
    for (const OpenMS::MRMFeatureQC::ComponentGroupQCs& qc : featureQC.component_group_qcs) {
      oss << qc.component_group_name
        << "\tINT[" << qc.intensity_l << ", " << qc.intensity_u << "]\n";
    }

    oss << "==== END   getFeatureFiltersInfo " <<
      (is_feature_filter ? "(FeatureBackgroundFilter)" : "(FeatureBackgroundQC)") << "\n";
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
    LOGD << "START sampleNamesAreConsistent";

    const std::vector<InjectionHandler>& samples = sequenceHandler.getSequence();
    const std::vector<OpenMS::AbsoluteQuantitationStandards::runConcentration>& standards =
      sequenceHandler.getSequenceSegments().front().getStandardsConcentrations();

    std::set<std::string> names1;
    std::set<std::string> names2;

    for (const InjectionHandler& injection : samples) {
      names1.insert(injection.getMetaData().getSampleName());
    }

    for (const OpenMS::AbsoluteQuantitationStandards::runConcentration& run : standards) {
      names2.insert(run.sample_name);
    }

    const bool check_passed = validateNamesInStructures(names1, names2, "SEQUENCE", "STANDARDS", false);

    LOGD << "END sampleNamesAreConsistent";
    return check_passed;
  }

  bool InputDataValidation::componentNamesAreConsistent(
    const SequenceHandler& sequenceHandler
  )
  {
    LOGD << "START componentNamesAreConsistent";

    const RawDataHandler& rawDataHandler = sequenceHandler.getSequence().front().getRawData();
    const std::vector<OpenMS::ReactionMonitoringTransition>& transitions =
      rawDataHandler.getTargetedExperiment().getTransitions();
    const OpenMS::MRMFeatureQC& featureFilter = rawDataHandler.getFeatureFilter();
    const OpenMS::MRMFeatureQC& featureQC = rawDataHandler.getFeatureQC();
    const std::vector<OpenMS::AbsoluteQuantitationMethod>& quantitation_methods =
      sequenceHandler.getSequenceSegments().front().getQuantitationMethods();
    const std::vector<OpenMS::AbsoluteQuantitationStandards::runConcentration>& standards =
      sequenceHandler.getSequenceSegments().front().getStandardsConcentrations();
    const OpenMS::MRMFeatureQC& featureRSDFilter = rawDataHandler.getFeatureRSDFilter();
    const OpenMS::MRMFeatureQC& featureRSDQC = rawDataHandler.getFeatureRSDQC();
    const OpenMS::MRMFeatureQC& featureBackgroundFilter = rawDataHandler.getFeatureBackgroundFilter();
    const OpenMS::MRMFeatureQC& featureBackgroundQC = rawDataHandler.getFeatureBackgroundQC();

    std::set<std::string> names1;
    std::set<std::string> names2;
    std::set<std::string> names3;
    std::set<std::string> names4;
    std::set<std::string> names5;
    std::set<std::string> names6;
    std::set<std::string> names7;
    std::set<std::string> names8;
    std::set<std::string> names9;

    for (const OpenMS::ReactionMonitoringTransition& transition : transitions) {
      names1.insert(transition.getName());
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

    for (const OpenMS::MRMFeatureQC::ComponentQCs& qc : featureRSDFilter.component_qcs) {
      names6.insert(qc.component_name);
    }

    for (const OpenMS::MRMFeatureQC::ComponentQCs& qc : featureRSDQC.component_qcs) {
      names7.insert(qc.component_name);
    }

    for (const OpenMS::MRMFeatureQC::ComponentQCs& qc : featureBackgroundFilter.component_qcs) {
      names8.insert(qc.component_name);
    }

    for (const OpenMS::MRMFeatureQC::ComponentQCs& qc : featureBackgroundQC.component_qcs) {
      names9.insert(qc.component_name);
    }

    std::vector<bool> check_passed(8);

    check_passed.at(0) = validateNamesInStructures(names1, names2, "TRAML", "FEATUREFILTER", false);
    check_passed.at(1) = validateNamesInStructures(names1, names3, "TRAML", "FEATUREQC", false);
    check_passed.at(2) = validateNamesInStructures(names1, names4, "TRAML", "QUANTITATIONMETHODS", false);
    check_passed.at(3) = validateNamesInStructures(names1, names5, "TRAML", "STANDARDS", false);
    check_passed.at(4) = validateNamesInStructures(names1, names6, "TRAML", "FEATURE_RSD_FILTER", false);
    check_passed.at(5) = validateNamesInStructures(names1, names7, "TRAML", "FEATURE_RSD_QC", false);
    check_passed.at(6) = validateNamesInStructures(names1, names8, "TRAML", "FEATURE_BACKGROUND_FILTER", false);
    check_passed.at(7) = validateNamesInStructures(names1, names9, "TRAML", "FEATURE_BACKGROUND_QC", false);

    LOGD << "END componentNamesAreConsistent";

    return std::all_of(check_passed.cbegin(), check_passed.cend(), [](const bool has_passed){ return has_passed; });
  }

  bool InputDataValidation::componentNameGroupsAreConsistent(
    const SequenceHandler& sequenceHandler
  )
  {
    LOGD << "START componentNameGroupsAreConsistent";

    const RawDataHandler& rawDataHandler = sequenceHandler.getSequence().front().getRawData();
    const std::vector<OpenMS::ReactionMonitoringTransition>& transitions =
      rawDataHandler.getTargetedExperiment().getTransitions();
    const OpenMS::MRMFeatureQC& featureFilter = rawDataHandler.getFeatureFilter();
    const OpenMS::MRMFeatureQC& featureQC = rawDataHandler.getFeatureQC();
    const OpenMS::MRMFeatureQC& featureRSDFilter = rawDataHandler.getFeatureRSDFilter();
    const OpenMS::MRMFeatureQC& featureRSDQC = rawDataHandler.getFeatureRSDQC();
    const OpenMS::MRMFeatureQC& featureBackgroundFilter = rawDataHandler.getFeatureBackgroundFilter();
    const OpenMS::MRMFeatureQC& featureBackgroundQC = rawDataHandler.getFeatureBackgroundQC();

    std::set<std::string> names1;
    std::set<std::string> names2;
    std::set<std::string> names3;
    std::set<std::string> names4;
    std::set<std::string> names5;
    std::set<std::string> names6;
    std::set<std::string> names7;

    for (const OpenMS::ReactionMonitoringTransition& transition : transitions) {
      names1.insert(transition.getPeptideRef());
    }

    for (const OpenMS::MRMFeatureQC::ComponentGroupQCs& qc : featureFilter.component_group_qcs) {
      names2.insert(qc.component_group_name);
    }

    for (const OpenMS::MRMFeatureQC::ComponentGroupQCs& qc : featureQC.component_group_qcs) {
      names3.insert(qc.component_group_name);
    }

    for (const OpenMS::MRMFeatureQC::ComponentGroupQCs& qc : featureRSDFilter.component_group_qcs) {
      names4.insert(qc.component_group_name);
    }

    for (const OpenMS::MRMFeatureQC::ComponentGroupQCs& qc : featureRSDQC.component_group_qcs) {
      names5.insert(qc.component_group_name);
    }

    for (const OpenMS::MRMFeatureQC::ComponentGroupQCs& qc : featureBackgroundFilter.component_group_qcs) {
      names6.insert(qc.component_group_name);
    }

    for (const OpenMS::MRMFeatureQC::ComponentGroupQCs& qc : featureBackgroundQC.component_group_qcs) {
      names7.insert(qc.component_group_name);
    }

    std::vector<bool> check_passed(6);

    check_passed.at(0) = validateNamesInStructures(names1, names2, "TRAML", "FEATUREFILTER", false);
    check_passed.at(1) = validateNamesInStructures(names1, names3, "TRAML", "FEATUREQC", false);
    check_passed.at(2) = validateNamesInStructures(names1, names2, "TRAML", "FEATURE_RSD_FILTER", false);
    check_passed.at(3) = validateNamesInStructures(names1, names3, "TRAML", "FEATURE_RSD_QC", false);
    check_passed.at(4) = validateNamesInStructures(names1, names2, "TRAML", "FEATURE_BACKGROUND_FILTER", false);
    check_passed.at(5) = validateNamesInStructures(names1, names3, "TRAML", "FEATURE_BACKGROUND_QC", false);

    LOGD << "END componentNameGroupsAreConsistent";

    return std::all_of(check_passed.cbegin(), check_passed.cend(), [](const bool has_passed){ return has_passed; });
  }

  bool InputDataValidation::heavyComponentsAreConsistent(
    const SequenceHandler& sequenceHandler
  )
  {
    LOGD << "START heavyComponentsAreConsistent";

    const std::vector<OpenMS::AbsoluteQuantitationMethod>& quantitation_methods =
      sequenceHandler.getSequenceSegments().front().getQuantitationMethods();
    const std::vector<OpenMS::AbsoluteQuantitationStandards::runConcentration>& standards =
      sequenceHandler.getSequenceSegments().front().getStandardsConcentrations();

    std::set<std::string> names1;
    std::set<std::string> names2;

    for (const OpenMS::AbsoluteQuantitationMethod& aqm : quantitation_methods) {
      names1.insert(aqm.getISName());
    }

    for (const OpenMS::AbsoluteQuantitationStandards::runConcentration& run : standards) {
      names2.insert(run.IS_component_name);
    }

    const bool check_passed = validateNamesInStructures(names1, names2, "QUANTITATIONMETHODS", "STANDARDS", false);

    LOGD << "END heavyComponentsAreConsistent";
    return check_passed;
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
      LOGI << logMissingNames(missing1, structure_ref1, structure_ref2);
    }
    std::set<std::string> missing2;
    if (check_both_directions) {
      missing2 = findMissingNames(names2, names1);
      if (missing2.size()) {
        LOGI << logMissingNames(missing2, structure_ref2, structure_ref1);
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

  bool InputDataValidation::prepareToLoad(const Filenames& filenames, const std::string& file_id)
  {
    if (filenames.isEmbedded(file_id))
    {
      std::cout << "Loading from Session DB: " << file_id << std::endl;
    }
    else
    {
      std::cout << "Loading: " << filenames.getFullPath(file_id).generic_string() << std::endl;

      const auto& full_path = filenames.getFullPath(file_id);
      const bool is_embedded = filenames.isEmbedded(file_id);

      if (!is_embedded)
      {
        if (full_path.empty()) {
          std::cout << "Filename is empty" << std::endl;
          return false;
        }

        if (!InputDataValidation::fileExists(full_path)) {
          std::cout << "File not found " << full_path.generic_string() << std::endl;
          return false;
        }
      }
    }

    return true;
  }

  bool InputDataValidation::prepareToLoadOneOfTwo(const Filenames& filenames, const std::string& id1, const std::string& id2)
  {
    const auto& full_path_1 = filenames.getFullPath(id1);
    const auto& full_path_2 = filenames.getFullPath(id2);
    const bool is_embedded_1 = filenames.isEmbedded(id1);
    const bool is_embedded_2 = filenames.isEmbedded(id2);

    if (full_path_1.empty() && (!is_embedded_1) &&
        full_path_2.empty() && (!is_embedded_2)) {
      LOGE << "Filenames are both empty";
      return false;
    }

    bool file_1_is_valid = true;
    bool file_2_is_valid = true;

    if (!is_embedded_1 && !full_path_1.empty() &&
      !InputDataValidation::fileExists(full_path_1)) {
      LOGE << "File not found: " << full_path_1.generic_string();
      file_1_is_valid = false;
    }
    else
    {
      if (is_embedded_1)
      {
        LOGI << "Loading from Session DB: " << id1;
      }
      else if (!full_path_1.empty())
      {
        LOGI << "Loading: " << full_path_1.generic_string();
      }
    }
    

    if (!is_embedded_2 && !full_path_2.empty() &&
      !InputDataValidation::fileExists(full_path_2)) {
      LOGE << "File not found: " << full_path_2.generic_string();
      file_2_is_valid = false;
    }
    else
    {
      if (is_embedded_2)
      {
        LOGI << "Loading from Session DB: " << id2;
      }
      else if (!full_path_2.empty())
      {
        LOGI << "Loading: " << full_path_2.generic_string();
      }
    }
    return (file_1_is_valid || file_2_is_valid);
  }

  bool InputDataValidation::prepareToStore(const Filenames& filenames, const std::string& file_id)
  {
    if (filenames.isEmbedded(file_id))
    {
      LOGI << "Storing in Session DB: " << file_id;
    }
    else
    {
      const auto full_path = filenames.getFullPath(file_id);

      LOGI << "Storing: " << full_path.generic_string();

      if (full_path.empty()) {
        LOGE << "Filename is empty";
        return false;
      }
    }
    return true;
  }

  bool InputDataValidation::prepareToStoreOneOfTwo(const Filenames& filenames, const std::string& file_id1, const std::string& file_id2)
  {
    const auto& full_path_1 = filenames.getFullPath(file_id1);
    const auto& full_path_2 = filenames.getFullPath(file_id2);
    const bool is_embedded_1 = filenames.isEmbedded(file_id1);
    const bool is_embedded_2 = filenames.isEmbedded(file_id2);

    if (filenames.getFullPath(file_id1).empty() &&
      filenames.getFullPath(file_id2).empty()) {
      LOGE << "Filenames are both empty";
      return false;
    }

    if (is_embedded_1)
    {
      LOGI << "Storing in Session DB: " << file_id1;
    }
    else if (!full_path_1.empty())
    {
      LOGI << "Storing: " << full_path_1.generic_string();
    }

    if (is_embedded_2)
    {
      LOGI << "Storing in Session DB: " << file_id2;
    }
    else if (!full_path_2.empty())
    {
      LOGI << "Storing: " << full_path_2.generic_string();
    }

    return true;
  }
}
