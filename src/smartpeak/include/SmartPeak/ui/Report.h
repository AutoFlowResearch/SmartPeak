#pragma once

#include <SmartPeak/ui/Widget.h>
#include <SmartPeak/core/ApplicationHandler.h>
#include <SmartPeak/core/FeatureMetadata.h>
#include <SmartPeak/core/SampleType.h>
#include <array>
#include <set>
#include <string>
#include <vector>

namespace SmartPeak
{
  class Report final : public Widget
  {
    std::array<bool, SampleTypeSize>      st_checks_;
    std::array<bool, FeatureMetadataSize> md_checks_;
    std::set<SampleType>                  summarySampleTypes_;
    std::vector<FeatureMetadata>          summaryMetaData_;
    ApplicationHandler*                             state_ = nullptr;

    bool initializeMetadataAndSampleTypes();

    static void run_and_join(
      bool (*data_writer)(const SequenceHandler&, const std::string&, const std::vector<FeatureMetadata>&, const std::set<SampleType>&),
      const std::string& data_writer_label,
      const SequenceHandler sequence,
      const std::string& pathname,
      const std::vector<FeatureMetadata>& meta_data,
      const std::set<SampleType>& sample_types
    );

  public:
    bool draw_ = false;

    Report();

    void draw() override;

    void setState(ApplicationHandler& state);
  };
}
