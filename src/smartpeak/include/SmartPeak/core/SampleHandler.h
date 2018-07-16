/**TODO: Add copyright*/

#pragma once

#include <SmartPeak/core/MetaDataHandler.h>
#include <SmartPeak/core/RawDataHandler.h>

namespace SmartPeak
{
  class SampleHandler
  {
public:
    SampleHandler() = default;
    ~SampleHandler() = default;

    void clear();

    void setMetaData(const MetaDataHandler& meta_data);

    MetaDataHandler& getMetaData();
    MetaDataHandler getMetaData() const;

    void setRawData(const RawDataHandler& raw_data);

    RawDataHandler& getRawData();
    RawDataHandler getRawData() const;

private:
    MetaDataHandler meta_data_;
    RawDataHandler raw_data_;
  };
}
