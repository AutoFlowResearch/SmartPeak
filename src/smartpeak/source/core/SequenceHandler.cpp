// TODO: Add copyright

#include <SmartPeak/core/CastValue.h>
#include <SmartPeak/core/MetaDataHandler.h>
#include <SmartPeak/core/SequenceHandler.h>
#include <plog/Log.h>

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
      throw std::invalid_argument("FeatureMetadata argument is not valid.");
    auto meta_data_ptr = std::make_shared<MetaDataHandler>(MetaDataHandler(meta_data_I));

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

      auto transitions_ptr = sequence_.begin()->getRawDataShared()->getTargetedExperimentShared();
      rdh.setTargetedExperiment(transitions_ptr);
      auto reference_data_ptr = sequence_.begin()->getRawDataShared()->getReferenceDataShared();
      rdh.setReferenceData(reference_data_ptr);
      // look up the sequence segment, 
      // add the sample index to the sequence segment list of injection indices
      // and copy over the quantitation method
      auto absQuantMethods_ptr = std::make_shared<std::vector<OpenMS::AbsoluteQuantitationMethod>>(std::vector<OpenMS::AbsoluteQuantitationMethod>());
      auto feature_filters_ptr = std::make_shared<OpenMS::MRMFeatureQC>(OpenMS::MRMFeatureQC());
      auto feature_qc_ptr = std::make_shared<OpenMS::MRMFeatureQC>(OpenMS::MRMFeatureQC());
      auto feature_rsd_filters_ptr = std::make_shared<OpenMS::MRMFeatureQC>(OpenMS::MRMFeatureQC());
      auto feature_rsd_qc_ptr = std::make_shared<OpenMS::MRMFeatureQC>(OpenMS::MRMFeatureQC());
      auto feature_background_filters_ptr = std::make_shared<OpenMS::MRMFeatureQC>(OpenMS::MRMFeatureQC());
      auto feature_background_qc_ptr = std::make_shared<OpenMS::MRMFeatureQC>(OpenMS::MRMFeatureQC());
      auto feature_rsd_estimations_ptr = std::make_shared<OpenMS::MRMFeatureQC>(OpenMS::MRMFeatureQC());
      auto feature_background_estimations_ptr = std::make_shared<OpenMS::MRMFeatureQC>(OpenMS::MRMFeatureQC());
      bool found_seq_seg = false;
      for (SequenceSegmentHandler& sequenceSegmentHandler : sequence_segments_) {
        if (meta_data_I.getSequenceSegmentName() == sequenceSegmentHandler.getSequenceSegmentName()) {
          absQuantMethods_ptr = sequenceSegmentHandler.getQuantitationMethodsShared();
          feature_filters_ptr = sequenceSegmentHandler.getFeatureFilterShared();
          feature_qc_ptr = sequenceSegmentHandler.getFeatureQCShared();
          feature_rsd_filters_ptr = sequenceSegmentHandler.getFeatureRSDFilterShared();
          feature_rsd_qc_ptr = sequenceSegmentHandler.getFeatureRSDQCShared();
          feature_background_filters_ptr = sequenceSegmentHandler.getFeatureBackgroundFilterShared();
          feature_background_qc_ptr = sequenceSegmentHandler.getFeatureBackgroundQCShared();
          feature_rsd_estimations_ptr = sequenceSegmentHandler.getFeatureRSDEstimationsShared();
          feature_background_estimations_ptr = sequenceSegmentHandler.getFeatureBackgroundEstimationsShared();
          found_seq_seg = true;
          sequenceSegmentHandler.getSampleIndices().push_back(sequence_.size()); // index = the size of the sequence
          break;
        }
      }
      if (found_seq_seg) {
        rdh.setQuantitationMethods(absQuantMethods_ptr);
        rdh.setFeatureFilter(feature_filters_ptr);
        rdh.setFeatureQC(feature_qc_ptr);
        rdh.setFeatureRSDFilter(feature_rsd_filters_ptr);
        rdh.setFeatureRSDQC(feature_rsd_qc_ptr);
        rdh.setFeatureBackgroundFilter(feature_background_filters_ptr);
        rdh.setFeatureBackgroundQC(feature_background_qc_ptr);
        rdh.setFeatureRSDEstimations(feature_rsd_estimations_ptr);
        rdh.setFeatureBackgroundEstimations(feature_background_estimations_ptr);
      }
      else {  // New sequence segment
        // initialize the sequence segment
        SequenceSegmentHandler sequenceSegmentHandler;
        sequenceSegmentHandler.setSampleIndices({ sequence_.size() }); // index = the size of the sequence
        sequenceSegmentHandler.setQuantitationMethods(absQuantMethods_ptr);
        sequenceSegmentHandler.setFeatureFilter(feature_filters_ptr);
        sequenceSegmentHandler.setFeatureQC(feature_qc_ptr);
        sequenceSegmentHandler.setFeatureRSDFilter(feature_rsd_filters_ptr);
        sequenceSegmentHandler.setFeatureRSDQC(feature_rsd_qc_ptr);
        sequenceSegmentHandler.setFeatureBackgroundFilter(feature_background_filters_ptr);
        sequenceSegmentHandler.setFeatureBackgroundQC(feature_background_qc_ptr);
        sequenceSegmentHandler.setFeatureRSDEstimations(feature_rsd_estimations_ptr);
        sequenceSegmentHandler.setFeatureBackgroundEstimations(feature_background_estimations_ptr);
        sequenceSegmentHandler.setSequenceSegmentName(meta_data_I.getSequenceSegmentName());
        sequence_segments_.push_back(sequenceSegmentHandler);

        rdh.setQuantitationMethods(absQuantMethods_ptr);
        rdh.setFeatureFilter(feature_filters_ptr);
        rdh.setFeatureQC(feature_qc_ptr);
        rdh.setFeatureRSDFilter(feature_rsd_filters_ptr);
        rdh.setFeatureRSDQC(feature_rsd_qc_ptr);
        rdh.setFeatureBackgroundFilter(feature_background_filters_ptr);
        rdh.setFeatureBackgroundQC(feature_background_qc_ptr);
        rdh.setFeatureRSDEstimations(feature_rsd_estimations_ptr);
        rdh.setFeatureBackgroundEstimations(feature_background_estimations_ptr);
      }

      bool found_sample_group = false;
      for (SampleGroupHandler& sampleGroupHandler : sample_groups_) {
        if (meta_data_I.getSampleGroupName() == sampleGroupHandler.getSampleGroupName()) {
          found_sample_group = true;
          sampleGroupHandler.getSampleIndices().push_back(sequence_.size()); // index = the size of the sequence
          break;
        }
      }
      if (found_sample_group) {
        // Nothing yet...
      }
      else {  // New sample group
        // initialize the sample group
        SampleGroupHandler sampleGroupHandler;
        sampleGroupHandler.setSampleIndices({ sequence_.size() }); // index = the size of the sequence
        sampleGroupHandler.setSampleGroupName(meta_data_I.getSampleGroupName());
        sample_groups_.push_back(sampleGroupHandler);
      }
    }
    else {
      // initialize the sequence segment
      SequenceSegmentHandler sequenceSegmentHandler;
      auto absQuantMethods_ptr = std::make_shared<std::vector<OpenMS::AbsoluteQuantitationMethod>>(std::vector<OpenMS::AbsoluteQuantitationMethod>());
      auto feature_filters_ptr = std::make_shared<OpenMS::MRMFeatureQC>(OpenMS::MRMFeatureQC());
      auto feature_qc_ptr = std::make_shared<OpenMS::MRMFeatureQC>(OpenMS::MRMFeatureQC());
      auto feature_rsd_filters_ptr = std::make_shared<OpenMS::MRMFeatureQC>(OpenMS::MRMFeatureQC());
      auto feature_rsd_qc_ptr = std::make_shared<OpenMS::MRMFeatureQC>(OpenMS::MRMFeatureQC());
      auto feature_background_filters_ptr = std::make_shared<OpenMS::MRMFeatureQC>(OpenMS::MRMFeatureQC());
      auto feature_background_qc_ptr = std::make_shared<OpenMS::MRMFeatureQC>(OpenMS::MRMFeatureQC());
      auto feature_rsd_estimations_ptr = std::make_shared<OpenMS::MRMFeatureQC>(OpenMS::MRMFeatureQC());
      auto feature_background_estimations_ptr = std::make_shared<OpenMS::MRMFeatureQC>(OpenMS::MRMFeatureQC());
      sequenceSegmentHandler.setSampleIndices({ 0 }); // first index of the sequence
      sequenceSegmentHandler.setQuantitationMethods(absQuantMethods_ptr);
      sequenceSegmentHandler.setFeatureFilter(feature_filters_ptr);
      sequenceSegmentHandler.setFeatureQC(feature_qc_ptr);
      sequenceSegmentHandler.setFeatureRSDFilter(feature_rsd_filters_ptr);
      sequenceSegmentHandler.setFeatureRSDQC(feature_rsd_qc_ptr);
      sequenceSegmentHandler.setFeatureBackgroundFilter(feature_background_filters_ptr);
      sequenceSegmentHandler.setFeatureBackgroundQC(feature_background_qc_ptr);
      sequenceSegmentHandler.setFeatureRSDEstimations(feature_rsd_estimations_ptr);
      sequenceSegmentHandler.setFeatureBackgroundEstimations(feature_background_estimations_ptr);
      sequenceSegmentHandler.setSequenceSegmentName(meta_data_I.getSequenceSegmentName());
      sequence_segments_.push_back(sequenceSegmentHandler);

      rdh.setQuantitationMethods(absQuantMethods_ptr);
      rdh.setFeatureFilter(feature_filters_ptr);
      rdh.setFeatureQC(feature_qc_ptr);
      rdh.setFeatureRSDFilter(feature_rsd_filters_ptr);
      rdh.setFeatureRSDQC(feature_rsd_qc_ptr);
      rdh.setFeatureBackgroundFilter(feature_background_filters_ptr);
      rdh.setFeatureBackgroundQC(feature_background_qc_ptr);
      rdh.setFeatureRSDEstimations(feature_rsd_estimations_ptr);
      rdh.setFeatureBackgroundEstimations(feature_background_estimations_ptr);

      // initialize the sample groups
      SampleGroupHandler sampleGroupHandler;
      sampleGroupHandler.setSampleIndices({ 0 }); // first index of the sequence
      sampleGroupHandler.setSampleGroupName(meta_data_I.getSampleGroupName());
      sample_groups_.push_back(sampleGroupHandler);
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

  CastValue SequenceHandler::getMetaValue(
    const OpenMS::Feature& feature,
    const OpenMS::Feature& subordinate,
    const std::string& meta_value
  )
  {
    CastValue cast;

    if (meta_value == "RT") {
      cast = static_cast<float>(feature.getRT());
    } else if (meta_value == "mz") { // Mass to charge ratio
      cast = static_cast<float>(feature.getMZ());
    } else if (meta_value == "charge") { // Mass to charge ratio
      cast = static_cast<int>(feature.getCharge());
    } else if (meta_value == "intensity") { // Sum of each subordinate intensity
      cast = static_cast<float>(feature.getIntensity());
    } else if (meta_value == "peak_area") { // Subordinate intensity (also called "peak area)
      cast = static_cast<float>(subordinate.getIntensity());
    } else if (meta_value == "validation") { // The result of the validation
      cast = static_cast<std::string>(subordinate.getMetaValue(meta_value));
    } else if (feature.metaValueExists(meta_value) && !feature.getMetaValue(meta_value).isEmpty()) {
      cast = static_cast<float>(feature.getMetaValue(meta_value));
    } else if (subordinate.metaValueExists(meta_value) && !subordinate.getMetaValue(meta_value).isEmpty()) {
      cast = static_cast<float>(subordinate.getMetaValue(meta_value));
    } else {
      LOGV << "meta_value not found: " << meta_value;
      cast = "";
    }

    return cast;
  }

  std::string SequenceHandler::getRawDataFilesInfo() const
  {
    size_t cnt {0};
    for (const InjectionHandler& inj : getSequence()) {
      if (inj.getRawData().getExperiment().getChromatograms().size()) {
        ++cnt;
      }
    }
    std::ostringstream oss;
    oss << "==== START getRawDataFilesInfo\n";
    oss << "Number of raw data files: " << std::to_string(cnt) << "\n";
    oss << "==== END   getRawDataFilesInfo\n";
    return oss.str();
  }

  std::string SequenceHandler::getAnalyzedFeaturesInfo() const
  {
    std::map<std::string, size_t> inj_feats;
    for (const InjectionHandler& inj : getSequence()) {
      inj_feats.emplace(
        inj.getMetaData().getInjectionName(),
        inj.getRawData().getFeatureMapHistory().size()
      );
    }
    std::ostringstream oss;
    oss << "==== START getAnalyzedFeaturesInfo\n";
    for (const std::pair<std::string, size_t>& p : inj_feats) {
      oss << p.first << "\t" << p.second << "\n";
    }
    oss << "==== END   getAnalyzedFeaturesInfo\n";
    return oss.str();
  }

  std::string SequenceHandler::getSelectedFeaturesInfo() const
  {
    std::map<std::string, size_t> inj_feats;
    for (const InjectionHandler& inj : getSequence()) {
      inj_feats.emplace(
        inj.getMetaData().getInjectionName(),
        inj.getRawData().getFeatureMap().size()
      );
    }
    std::ostringstream oss;
    oss << "==== START getSelectedFeaturesInfo\n";
    for (const std::pair<std::string, size_t>& p : inj_feats) {
      oss << p.first << "\t" << p.second << "\n";
    }
    oss << "==== END   getSelectedFeaturesInfo\n";
    return oss.str();
  }

  std::string SequenceHandler::getPickedPeaksInfo() const
  {
    std::map<std::string, size_t> inj_feats;
    for (const InjectionHandler& inj : getSequence()) {
      size_t n_peaks {0};
      for (const OpenMS::Feature& f : inj.getRawData().getFeatureMapHistory()) {
        n_peaks += f.getSubordinates().size();
      }
      inj_feats.emplace(inj.getMetaData().getInjectionName(), n_peaks);
    }
    std::ostringstream oss;
    oss << "==== START getPickedPeaksInfo\n";
    for (const std::pair<std::string, size_t>& p : inj_feats) {
      oss << p.first << "\t" << p.second << "\n";
    }
    oss << "==== END   getPickedPeaksInfo\n";
    return oss.str();
  }

  std::string SequenceHandler::getFilteredSelectedPeaksInfo() const
  {
    std::map<std::string, size_t> inj_feats;
    for (const InjectionHandler& inj : getSequence()) {
      size_t n_peaks {0};
      for (const OpenMS::Feature& f : inj.getRawData().getFeatureMap()) {
        n_peaks += f.getSubordinates().size();
      }
      inj_feats.emplace(inj.getMetaData().getInjectionName(), n_peaks);
    }
    std::ostringstream oss;
    oss << "==== START getFilteredSelectedPeaksInfo\n";
    for (const std::pair<std::string, size_t>& p : inj_feats) {
      oss << p.first << "\t" << p.second << "\n";
    }
    oss << "==== END   getFilteredSelectedPeaksInfo\n";
    return oss.str();
  }
}
