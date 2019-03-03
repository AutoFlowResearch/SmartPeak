/**TODO: Add copyright*/

#pragma once

#include <SmartPeak/core/MetaDataHandler.h>
#include <SmartPeak/core/RawDataHandler.h>

namespace SmartPeak
{
  class InjectionHandler
  {
public:
    InjectionHandler();  ///< constructor to initialize shared resources
    void clear();

    void setMetaData(const MetaDataHandler& meta_data);
    void setMetaData(std::shared_ptr<MetaDataHandler>& meta_data);

    MetaDataHandler& getMetaData();
    const MetaDataHandler& getMetaData() const;
    std::shared_ptr<MetaDataHandler>& getMetaDataShared();

    void setRawData(const RawDataHandler& raw_data);
    void setRawData(std::shared_ptr<RawDataHandler>& raw_data);

    RawDataHandler& getRawData();
    const RawDataHandler& getRawData() const;
    std::shared_ptr<RawDataHandler>& getRawDataShared();

private:
    std::shared_ptr<MetaDataHandler> meta_data_ = nullptr;
    std::shared_ptr<RawDataHandler> raw_data_ = nullptr;
  };
}
