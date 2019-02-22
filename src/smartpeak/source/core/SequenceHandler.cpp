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
  }

  void SequenceHandler::setSequence(const std::vector<InjectionHandler>& sequence)
  {
    sequence_ = sequence;
  }

  std::vector<InjectionHandler>& SequenceHandler::getSequence()
  {
    return sequence_;
  }

  const std::vector<InjectionHandler>& SequenceHandler::getSequence() const
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

  const std::vector<SequenceSegmentHandler>& SequenceHandler::getSequenceSegments() const
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

  const std::vector<SampleGroupHandler>& SequenceHandler::getSampleGroups() const
  {
    return sample_groups_;
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

    InjectionHandler sh;
    sh.setMetaData(meta_data_I);
    sh.setRawData(rdh);

    sequence_.push_back(sh);

    const size_t pos = sequence_.size() - 1;
    const std::string& sample_name = meta_data_I.getInjectionName();

    index_to_sample_[pos] = sample_name;
    sample_to_index_[sample_name] = pos;
  }

  std::vector<InjectionHandler> SequenceHandler::getSamplesInSequence(
    const std::set<std::string>& injection_names
  ) const
  {
    std::vector<InjectionHandler> samples;

    for (const std::string& name : injection_names) {
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
}
