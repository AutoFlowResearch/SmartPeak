/**TODO: Add copyright*/

#include <SmartPeak/core/SampleHandler.h>

namespace SmartPeak
{
    void clear()
    {
      meta_data_.clear();
      raw_data_.clear();
    }

    void setMetaData(const MetaDataHandler& meta_data)
    {
      meta_data_ = meta_data;
    }

    MetaDataHandler& getMetaData() const
    {
      return meta_data_;
    }

    void setRawData(const RawDataHandler& raw_data)
    {
      raw_data_ = raw_data;
    }

    RawDataHandler& getRawData() const
    {
      return raw_data_;
    }
}
