// TODO: Add copyright

#pragma once

#include <SmartPeak/core/MetaDataHandler.h>
#include <SmartPeak/core/SampleGroupHandler.h>
#include <SmartPeak/core/SampleHandler.h>
#include <SmartPeak/core/SequenceSegmentHandler.h>

namespace SmartPeak
{
  class SequenceHandler
  {
public:
    SequenceHandler() = default;
    ~SequenceHandler() = default;

  void SequenceHandler::clear();

  void SequenceHandler::setFilenames(const std::map<std::string, std::string>& filenames);

  std::map<std::string, std::string>& SequenceHandler::getFilenames();

  std::map<std::string, std::string> SequenceHandler::getFilenames() const;

  void SequenceHandler::setDirStatic(const std::string& dir_static);

  std::string& SequenceHandler::getDirStatic();

  std::string SequenceHandler::getDirStatic() const;

  void SequenceHandler::setDirDynamic(const std::string& dir_dynamic);

  std::string& SequenceHandler::getDirDynamic();

  std::string SequenceHandler::getDirDynamic() const;

  void SequenceHandler::setSequence(const std::vector<SampleHandler>& sequence);

  std::vector<SampleHandler>& SequenceHandler::getSequence();

  std::vector<SampleHandler> SequenceHandler::getSequence() const;

  void SequenceHandler::setSequenceSegments(const std::vector<SequenceSegmentHandler>& sequence_segments);

  std::vector<SequenceSegmentHandler>& SequenceHandler::getSequenceSegments();

  std::vector<SequenceSegmentHandler> SequenceHandler::getSequenceSegments() const;

  void SequenceHandler::setSampleGroups(const std::vector<SampleGroupHandler>& sample_groups);

  std::vector<SampleGroupHandler>& SequenceHandler::getSampleGroups();

  std::vector<SampleGroupHandler> SequenceHandler::getSampleGroups() const;

  std::map<std::string, std::string> SequenceHandler::getDefaultStaticFilenames(std::string& dir);

  std::map<std::string, std::string> SequenceHandler::getDefaultDynamicFilenames(
    const std::string& dir,
    std::string& sample_name
  ) const;

  void SequenceHandler::addSampleToSequence(const SampleHandler& meta_data_I, const OpenMS::FeatureMap& featureMap_I);

  void SequenceHandler::getSamplesInSequence(std::vector<std::string>& sample_names, std::vector<SampleHandler>& samples) const;

  float SequenceHandler::getMetaValue(
    const OpenMS::Feature& feature,
    const OpenMS::Feature& subordinate,
    const std::string& meta_value
  ) const;

  void SequenceHandler::parse_metaData(const MetaDataHandler& meta_data);

private:
    std::vector<SampleHandler> sequence_;
    std::map<size_t, std::string> index_to_sample_;
    std::map<std::string, size_t> sample_to_index_;
    std::vector<SequenceSegmentHandler> sequence_segments_;
    std::vector<SampleGroupHandler> sample_groups_;

    std::string dir_static_;
    std::string dir_dynamic_;
    std::map<std::string, std::string> filenames_;
  };
}
