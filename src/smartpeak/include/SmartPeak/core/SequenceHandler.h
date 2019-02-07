// TODO: Add copyright

#pragma once

#include <SmartPeak/core/MetaDataHandler.h>
#include <SmartPeak/core/SampleGroupHandler.h>
#include <SmartPeak/core/SampleHandler.h>
#include <SmartPeak/core/SequenceSegmentHandler.h>
#include <SmartPeak/core/Utilities.h>

namespace SmartPeak
{
  class SequenceHandler
  {
public:
    SequenceHandler() = default;
    ~SequenceHandler() = default;
    SequenceHandler(const SequenceHandler&) = default;
    SequenceHandler& operator=(const SequenceHandler&) = default;
    SequenceHandler(SequenceHandler&&) = default;
    SequenceHandler& operator=(SequenceHandler&&) = default;

    void clear();

    void setSequence(const std::vector<SampleHandler>& sequence);
    std::vector<SampleHandler>& getSequence();
    const std::vector<SampleHandler>& getSequence() const;

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

    std::vector<SampleHandler> getSamplesInSequence(
      const std::set<std::string>& injection_names
    ) const;

    static Utilities::CastValue getMetaValue(
      const OpenMS::Feature& feature,
      const OpenMS::Feature& subordinate,
      const std::string& meta_value
    );

    std::map<size_t, std::string> index_to_sample_;
    std::map<std::string, size_t> sample_to_index_;
private:
    std::vector<SampleHandler> sequence_;
    std::vector<SequenceSegmentHandler> sequence_segments_;
    std::vector<SampleGroupHandler> sample_groups_;
  };
}
