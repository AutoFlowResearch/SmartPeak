/**TODO: Add copyright*/

#pragma once

namespace SmartPeak
{
  class SampleHandler
  {
public:
    SampleHandler() = default;
    ~SampleHandler() = default;

    void clear();

    void setMetaData(const META_DATA_TYPE& meta_data);

    META_DATA_TYPE& getMetaData() const;

    void setRawData(const RAW_DATA_TYPE& raw_data);

    RAW_DATA_TYPE& getRawData() const;

private:
    META_DATA_TYPE meta_data_;
    RAW_DATA_TYPE raw_data_;
  };
}
