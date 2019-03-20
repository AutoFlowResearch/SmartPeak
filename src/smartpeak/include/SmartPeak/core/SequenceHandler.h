// TODO: Add copyright

#pragma once

#include <SmartPeak/core/MetaDataHandler.h>
#include <SmartPeak/core/SampleGroupHandler.h>
#include <SmartPeak/core/InjectionHandler.h>
#include <SmartPeak/core/SequenceSegmentHandler.h>
#include <SmartPeak/core/Utilities.h>

namespace SmartPeak
{
  class SequenceHandler
  {
public:
    void clear();

    void setSequence(const std::vector<InjectionHandler>& sequence);
    std::vector<InjectionHandler>& getSequence();
    const std::vector<InjectionHandler>& getSequence() const;

    void setSequenceSegments(const std::vector<SequenceSegmentHandler>& sequence_segments);
    std::vector<SequenceSegmentHandler>& getSequenceSegments();
    const std::vector<SequenceSegmentHandler>& getSequenceSegments() const;

    void setSampleGroups(const std::vector<SampleGroupHandler>& sample_groups);
    std::vector<SampleGroupHandler>& getSampleGroups();
    const std::vector<SampleGroupHandler>& getSampleGroups() const;

    void addSampleToSequence(
      const MetaDataHandler& meta_data_I,
      const OpenMS::FeatureMap& featureMap_I
    );

    std::vector<InjectionHandler> getSamplesInSequence(
      const std::set<std::string>& injection_names
    ) const;

    static Utilities::CastValue getMetaValue(
      const OpenMS::Feature& feature,
      const OpenMS::Feature& subordinate,
      const std::string& meta_value
    );

    std::string getRawDataFilesInfo() const;
    std::string getAnalyzedFeaturesInfo() const;
    std::string getSelectedFeaturesInfo() const;
    std::string getPickedPeaksInfo() const;
    std::string getFilteredSelectedPeaksInfo() const;

private:
    std::vector<InjectionHandler> sequence_;
    std::vector<SequenceSegmentHandler> sequence_segments_;
    std::vector<SampleGroupHandler> sample_groups_;
  };
}
