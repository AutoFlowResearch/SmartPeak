/**TODO: Add copyright*/

#pragma once

#include <SmartPeak/core/MetaDataHandler.h>
#include <SmartPeak/core/RawDataHandler.h>

namespace SmartPeak
{
  class InjectionHandler
  {
public:
    void clear();

    void setMetaData(const MetaDataHandler& meta_data);

    MetaDataHandler& getMetaData();
    const MetaDataHandler& getMetaData() const;

    void setRawData(const RawDataHandler& raw_data);

    RawDataHandler& getRawData();
    const RawDataHandler& getRawData() const;

private:
    MetaDataHandler meta_data_;
    RawDataHandler raw_data_;
  };
}
