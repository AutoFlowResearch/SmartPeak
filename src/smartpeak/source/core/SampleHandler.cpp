/**TODO: Add copyright*/

#include <SmartPeak/core/SampleHandler.h>

namespace SmartPeak
{
  void SampleHandler::clear()
  {
    meta_data_.clear();
    // raw_data_.clear(); // TODO: implement .clear() in RawDataHandler
  }

  void SampleHandler::setMetaData(const MetaDataHandler& meta_data)
  {
    meta_data_ = meta_data;
  }

  MetaDataHandler& SampleHandler::getMetaData()
  {
    return meta_data_;
  }

  MetaDataHandler SampleHandler::getMetaData() const
  {
    return meta_data_;
  }

  void SampleHandler::setRawData(const RawDataHandler& raw_data)
  {
    raw_data_ = raw_data;
  }

  RawDataHandler& SampleHandler::getRawData()
  {
    return raw_data_;
  }

  RawDataHandler SampleHandler::getRawData() const
  {
    return raw_data_;
  }
}
