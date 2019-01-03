// TODO: Add copyright

#include <SmartPeak/core/MetaDataHandler.h>
#include <SmartPeak/core/SequenceHandler.h>
#include <SmartPeak/core/Utilities.h>

namespace SmartPeak
{
  // """A class to manage the mapping of metadata and FeatureMaps
  //     (i.e., multiple samples in a run batch/sequence)"""

  void SequenceHandler::clear()
  {
    sequence_.clear();
    index_to_sample_.clear();
    sample_to_index_.clear();
    sequence_segments_.clear();
    sample_groups_.clear();

    dir_dynamic_.clear();
  }

  void SequenceHandler::setDirDynamic(const std::string& dir_dynamic)
  {
    dir_dynamic_ = dir_dynamic;
  }

  std::string& SequenceHandler::getDirDynamic()
  {
    return dir_dynamic_;
  }

  std::string SequenceHandler::getDirDynamic() const
  {
    return dir_dynamic_;
  }

  void SequenceHandler::setSequence(const std::vector<SampleHandler>& sequence)
  {
    sequence_ = sequence;
  }

  std::vector<SampleHandler>& SequenceHandler::getSequence()
  {
    return sequence_;
  }

  std::vector<SampleHandler> SequenceHandler::getSequence() const
  {
    return sequence_;
  }

  void SequenceHandler::setSequenceSegments(const std::vector<SequenceSegmentHandler>& sequence_segments)
  {
    sequence_segments_ = sequence_segments;
  }

  std::vector<SequenceSegmentHandler>& SequenceHandler::getSequenceSegments()
  {
    return sequence_segments_;
  }

  std::vector<SequenceSegmentHandler> SequenceHandler::getSequenceSegments() const
  {
    return sequence_segments_;
  }

  void SequenceHandler::setSampleGroups(const std::vector<SampleGroupHandler>& sample_groups)
  {
    sample_groups_ = sample_groups;
  }

  std::vector<SampleGroupHandler>& SequenceHandler::getSampleGroups()
  {
    return sample_groups_;
  }

  std::vector<SampleGroupHandler> SequenceHandler::getSampleGroups() const
  {
    return sample_groups_;
  }

  SequenceHandler::Filenames SequenceHandler::Filenames::getDefaultStaticFilenames(
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

  SequenceHandler::Filenames SequenceHandler::Filenames::getDefaultDynamicFilenames(
    const std::string& dir,
    const std::string& sample_name
  )
  {
    const std::string features_dir = dir + "/features/" + sample_name;
    Filenames dynamic_filenames;
    dynamic_filenames.mzML_i = dir + "/mzML/" + sample_name + ".mzML";
    dynamic_filenames.featureXML_o = features_dir + ".featureXML";
    dynamic_filenames.feature_csv_o = features_dir + ".csv";
    dynamic_filenames.featureXML_i = features_dir + ".featureXML";
    dynamic_filenames.features_pdf_o = features_dir;
    dynamic_filenames.calibrators_pdf_o = features_dir;
    dynamic_filenames.quantitationMethods_csv_o = features_dir + "_quantitationMethods.csv";
    dynamic_filenames.componentsToConcentrations_csv_o = features_dir + "_componentsToConcentrations.csv";
    return dynamic_filenames;
  }

  void SequenceHandler::addSampleToSequence(
    const MetaDataHandler& meta_data_I,
    const OpenMS::FeatureMap& featureMap_I
  )
  {
    if (!MetaDataHandler::validateMetaData(meta_data_I))
      throw std::invalid_argument("Metadata argument is not valid.");

    RawDataHandler rdh;
    rdh.setFeatureMap(featureMap_I);

    SampleHandler sh;
    sh.setMetaData(meta_data_I);
    sh.setRawData(rdh);

    sequence_.push_back(sh);

    const size_t pos = sequence_.size() - 1;
    const std::string& sample_name = meta_data_I.getSampleName();

    index_to_sample_.erase(pos);
    index_to_sample_.emplace(pos, sample_name);

    sample_to_index_.erase(sample_name);
    sample_to_index_.emplace(sample_name, pos);
  }

  std::vector<SampleHandler> SequenceHandler::getSamplesInSequence(
    const std::vector<std::string>& sample_names
  ) const
  {
    std::vector<SampleHandler> samples;

    for (const std::string& name : sample_names) {
      if (sample_to_index_.count(name)) {
        samples.push_back(sequence_[sample_to_index_.at(name)]);
      }
    }

    return samples;
  }

  Utilities::CastValue SequenceHandler::getMetaValue(
    const OpenMS::Feature& feature,
    const OpenMS::Feature& subordinate,
    const std::string& meta_value
  )
  {
    Utilities::CastValue cast;

    if (meta_value == "RT") {
      cast = static_cast<float>(feature.getRT());
    } else if (feature.metaValueExists(meta_value) && !feature.getMetaValue(meta_value).isEmpty()) {
      cast = static_cast<float>(feature.getMetaValue(meta_value));
    } else if (subordinate.metaValueExists(meta_value) && !subordinate.getMetaValue(meta_value).isEmpty()) {
      cast = static_cast<float>(subordinate.getMetaValue(meta_value));
    } else {
      // throw std::invalid_argument("meta_value \"" + meta_value + "\" not found.");
      // std::cout << "meta_value \"" + meta_value + "\" not found.\n"; // Log?
      cast = "";
    }

    return cast;
  }

  void SequenceHandler::Filenames::clear()
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
