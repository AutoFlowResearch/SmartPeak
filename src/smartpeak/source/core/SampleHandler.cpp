/**TODO: Add copyright*/

#include <SmartPeak/core/SampleHandler.h>

namespace SmartPeak
{
    void clear()
    {
      meta_data_.clear();
      raw_data_.clear();
    }

    void setMetaData(const META_DATA_TYPE& meta_data)
    {
      meta_data_ = meta_data;
    }

    META_DATA_TYPE& getMetaData() const
    {
      return meta_data_;
    }

    void setRawData(const RAW_DATA_TYPE& raw_data)
    {
      raw_data_ = raw_data;
    }

    RAW_DATA_TYPE& getRawData() const
    {
      return raw_data_;
    }
}
