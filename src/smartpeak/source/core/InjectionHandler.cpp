/**TODO: Add copyright*/

#include <SmartPeak/core/InjectionHandler.h>

namespace SmartPeak
{
  void InjectionHandler::clear()
  {
    meta_data_.clear();
    raw_data_.clear();
  }

  void InjectionHandler::setMetaData(const MetaDataHandler& meta_data)
  {
    meta_data_ = meta_data;
  }

  MetaDataHandler& InjectionHandler::getMetaData()
  {
    return meta_data_;
  }

  const MetaDataHandler& InjectionHandler::getMetaData() const
  {
    return meta_data_;
  }

  void InjectionHandler::setRawData(const RawDataHandler& raw_data)
  {
    raw_data_ = raw_data;
  }

  RawDataHandler& InjectionHandler::getRawData()
  {
    return raw_data_;
  }

  const RawDataHandler& InjectionHandler::getRawData() const
  {
    return raw_data_;
  }
}
