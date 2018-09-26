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

    void clear();

    void setFilenames(const std::map<std::string, std::string>& filenames);
    std::map<std::string, std::string>& getFilenames();
    std::map<std::string, std::string> getFilenames() const;

    void setDirStatic(const std::string& dir_static);
    std::string& getDirStatic();
    std::string getDirStatic() const;

    void setDirDynamic(const std::string& dir_dynamic);
    std::string& getDirDynamic();
    std::string getDirDynamic() const;

    void setSequence(const std::vector<SampleHandler>& sequence);
    std::vector<SampleHandler>& getSequence();
    std::vector<SampleHandler> getSequence() const;

    void setSequenceSegments(const std::vector<SequenceSegmentHandler>& sequence_segments);
    std::vector<SequenceSegmentHandler>& getSequenceSegments();
    std::vector<SequenceSegmentHandler> getSequenceSegments() const;

    void setSampleGroups(const std::vector<SampleGroupHandler>& sample_groups);
    std::vector<SampleGroupHandler>& getSampleGroups();
    std::vector<SampleGroupHandler> getSampleGroups() const;

    std::map<std::string, std::string> getDefaultStaticFilenames(const std::string& dir);

    std::map<std::string, std::string> getDefaultDynamicFilenames(
      const std::string& dir,
      std::string& sample_name
    ) const;

    void addSampleToSequence(const MetaDataHandler& meta_data_I, const OpenMS::FeatureMap& featureMap_I);

    void getSamplesInSequence(const std::vector<std::string>& sample_names, std::vector<SampleHandler>& samples) const;

    void getMetaValue(
      const OpenMS::Feature& feature,
      const OpenMS::Feature& subordinate,
      const std::string& meta_value,
      Utilities::CastValue cast
    ) const;

    std::map<size_t, std::string> index_to_sample_;
    std::map<std::string, size_t> sample_to_index_;
private:
    std::vector<SampleHandler> sequence_;
    std::vector<SequenceSegmentHandler> sequence_segments_;
    std::vector<SampleGroupHandler> sample_groups_;

    std::string dir_static_;
    std::string dir_dynamic_;
    std::map<std::string, std::string> filenames_;
  };
}
