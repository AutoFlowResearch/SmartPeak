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
    std::shared_ptr<MetaDataHandler> meta_data_ptr(new MetaDataHandler(meta_data_I));

    // initialize the raw data
    RawDataHandler rdh;
    rdh.setMetaData(meta_data_ptr);
    rdh.setFeatureMap(featureMap_I);
    rdh.updateFeatureMapHistory();

    // initialize the shared raw data attributes either using
    // the first injection added to the sequence or a shared ptr
    if (sequence_.size()) {
      auto parameters_ptr = sequence_.begin()->getRawDataShared()->getParametersShared();
      rdh.setParameters(parameters_ptr);
      auto feature_filters_ptr = sequence_.begin()->getRawDataShared()->getFeatureFilterShared();
      rdh.setFeatureFilter(feature_filters_ptr);
      auto feature_qc_ptr = sequence_.begin()->getRawDataShared()->getFeatureQCShared();
      rdh.setFeatureQC(feature_qc_ptr);
      auto transitions_ptr = sequence_.begin()->getRawDataShared()->getTargetedExperimentShared();
      rdh.setTargetedExperiment(transitions_ptr);
      auto reference_data_ptr = sequence_.begin()->getRawDataShared()->getReferenceDataShared();
      rdh.setReferenceData(reference_data_ptr);
      // look up the sequence segment, 
      // add the sample index to the sequence segment list of injection indices
      // and copy over the quantitation method
      std::shared_ptr<std::vector<OpenMS::AbsoluteQuantitationMethod>> absQuantMethods_ptr(new std::vector<OpenMS::AbsoluteQuantitationMethod>());
      bool found_seq_seg = false;
      for (SequenceSegmentHandler& sequenceSegmentHandler : sequence_segments_) {
        if (meta_data_I.getSequenceSegmentName() == sequenceSegmentHandler.getSequenceSegmentName()) {
          absQuantMethods_ptr = sequenceSegmentHandler.getQuantitationMethodsShared();
          found_seq_seg = true;
          break;
        }
      }
      if (found_seq_seg) {
        rdh.setQuantitationMethods(absQuantMethods_ptr);
      }
      else {  // New sequence segment
        // initialize the sequence segment
        SequenceSegmentHandler sequenceSegmentHandler;
        sequenceSegmentHandler.setSampleIndices({ sequence_.size() }); // index = the size of the sequence
        sequenceSegmentHandler.setQuantitationMethods(absQuantMethods_ptr);
        sequenceSegmentHandler.setSequenceSegmentName(meta_data_I.getSequenceSegmentName());
        sequence_segments_.push_back(sequenceSegmentHandler);

        rdh.setQuantitationMethods(absQuantMethods_ptr);
      }
    }
    else {
      // Not needed because the constructors initialize the shared pointers
      //rdh.setParameters(std::shared_ptr<std::map<std::string, std::vector<std::map<std::string, std::string>>>>(new std::map<std::string, std::vector<std::map<std::string, std::string>>>()));
      //rdh.setFeatureFilter(std::shared_ptr<OpenMS::MRMFeatureQC>(new OpenMS::MRMFeatureQC()));
      //rdh.setFeatureQC(std::shared_ptr<OpenMS::MRMFeatureQC>(new OpenMS::MRMFeatureQC()));
      //rdh.setTargetedExperiment(std::shared_ptr<OpenMS::TargetedExperiment>(new OpenMS::TargetedExperiment()));
      //rdh.setReferenceData(std::shared_ptr<std::vector<std::map<std::string, Utilities::CastValue>>>(new std::vector<std::map<std::string, Utilities::CastValue>>()));

      // initialize the sequence segment
      SequenceSegmentHandler sequenceSegmentHandler;
      std::shared_ptr<std::vector<OpenMS::AbsoluteQuantitationMethod>> absQuantMethods_ptr(new std::vector<OpenMS::AbsoluteQuantitationMethod>());
      sequenceSegmentHandler.setSampleIndices({ 0 }); // first index of the sequence
      sequenceSegmentHandler.setQuantitationMethods(absQuantMethods_ptr);
      sequenceSegmentHandler.setSequenceSegmentName(meta_data_I.getSequenceSegmentName());
      sequence_segments_.push_back(sequenceSegmentHandler);

      rdh.setQuantitationMethods(absQuantMethods_ptr);
    }

    // intialize the meta data
    InjectionHandler sh;
    sh.setMetaData(meta_data_ptr);
    sh.setRawData(rdh);

    sequence_.push_back(sh);
  }

  std::vector<InjectionHandler> SequenceHandler::getSamplesInSequence(
    const std::set<std::string>& injection_names
  ) const
  {
    std::vector<InjectionHandler> samples;

    for (const std::string& name : injection_names) {
      const std::vector<InjectionHandler>::const_iterator it =
        find_if(
          sequence_.cbegin(),
          sequence_.cend(),
          [&name](const InjectionHandler& injection)
          {
            return injection.getMetaData().getInjectionName() == name;
          }
        );
      if (it != sequence_.cend()) {
        samples.push_back(*it);
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
    } else if (meta_value == "intensity") { // Sum of each subordinate intensity
      cast = static_cast<float>(feature.getIntensity());
    } else if (meta_value == "peak_area") { // Subordinate intensity (also called "peak area)
      cast = static_cast<float>(subordinate.getIntensity());
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
