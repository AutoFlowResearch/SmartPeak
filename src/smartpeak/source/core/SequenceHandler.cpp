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

    dir_static_.clear();
    dir_dynamic_.clear();
    filenames_.clear();
  }

  void SequenceHandler::setFilenames(const std::map<std::string, std::string>& filenames)
  {
    filenames_ = filenames;
  }

  std::map<std::string, std::string>& SequenceHandler::getFilenames()
  {
    return filenames_;
  }

  std::map<std::string, std::string> SequenceHandler::getFilenames() const
  {
    return filenames_;
  }

  void SequenceHandler::setDirStatic(const std::string& dir_static)
  {
    dir_static_ = dir_static;
    setFilenames(getDefaultStaticFilenames(dir_static));
  }

  std::string& SequenceHandler::getDirStatic()
  {
    return dir_static_;
  }

  std::string SequenceHandler::getDirStatic() const
  {
    return dir_static_;
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

  std::map<std::string, std::string> SequenceHandler::getDefaultStaticFilenames(const std::string& dir)
  {
    return {
      {"sequence_csv_i", dir + "/sequence.csv"},
      {"parameters_csv_i", dir + "/parameters.csv"},
      {"traML_csv_i", dir + "/traML.csv"},
      {"featureFilterComponents_csv_i", dir + "/featureFilterComponents.csv"},
      {"featureFilterComponentGroups_csv_i", dir + "/featureFilterComponentGroups.csv"},
      {"featureQCComponents_csv_i", dir + "/featureQCComponents.csv"},
      {"featureQCComponentGroups_csv_i", dir + "/featureQCComponentGroups.csv"},
      {"quantitationMethods_csv_i", dir + "/quantitationMethods.csv"},
      {"standardsConcentrations_csv_i", dir + "/standardsConcentrations.csv"},
      {"db_json_i", dir + "/db.json"}
    };
  }

  std::map<std::string, std::string> SequenceHandler::getDefaultDynamicFilenames(
    const std::string& dir,
    const std::string& sample_name
  ) const
  {
    const std::string features = dir + "/features/" + sample_name;
    return {
      {"mzML_i", dir + "/mzML/" + sample_name + ".mzML"},
      {"featureXML_o", features + ".featureXML"},
      {"feature_csv_o", features + ".csv"},
      {"featureXML_i", features + ".featureXML"},
      {"features_pdf_o", features},
      {"calibrators_pdf_o", features},
      {"quantitationMethods_csv_o", features + "_quantitationMethods.csv"},
      {"componentsToConcentrations_csv_o", features + "_componentsToConcentrations.csv"}
    };
  }

  void SequenceHandler::addSampleToSequence(const MetaDataHandler& meta_data_I, const OpenMS::FeatureMap& featureMap_I)
  {
    MetaDataHandler::validateMetaData(meta_data_I);

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

  void SequenceHandler::getSamplesInSequence(
    const std::vector<std::string>& sample_names,
    std::vector<SampleHandler>& samples
  ) const
  {
    samples.clear();

    for (const std::string& name : sample_names) {
      if (sample_to_index_.count(name)) {
        samples.push_back(sequence_[sample_to_index_.at(name)]);
      }
    }
  }

  void SequenceHandler::getMetaValue(
    const OpenMS::Feature& feature,
    const OpenMS::Feature& subordinate,
    const std::string& meta_value,
    Utilities::CastValue& cast
  )
  {
    cast.clear();

    if (meta_value == "RT") {
      cast = static_cast<float>(feature.getRT());
    } else if (feature.metaValueExists(meta_value) && !feature.getMetaValue(meta_value).isEmpty()) {
      cast = static_cast<float>(feature.getMetaValue(meta_value));
    } else if (subordinate.metaValueExists(meta_value) && !subordinate.getMetaValue(meta_value).isEmpty()) {
      cast = static_cast<float>(subordinate.getMetaValue(meta_value));
    } else {
      throw std::invalid_argument("meta_value \"" + meta_value + "\" not found.");
    }
  }
}
