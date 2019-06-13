#pragma once

#include <SmartPeak/ui/Widget.h>
#include <SmartPeak/core/AppState.h>
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
    std::array<bool, 7>   st_checks_; // TODO: find a way to not forget about these magic numbers
    std::array<bool, 20>  md_checks_;
    std::set<SampleType>  summarySampleTypes_;
    std::vector<FeatureMetadata> summaryMetaData_;
    AppState*             state_ = nullptr;

    void initializeMetadataAndSampleTypes();

  public:
    bool draw_ = false;

    Report();

    void draw() override;

    void setState(AppState& state);
  };
}
