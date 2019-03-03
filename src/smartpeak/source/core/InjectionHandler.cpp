/**TODO: Add copyright*/

#include <SmartPeak/core/InjectionHandler.h>

namespace SmartPeak
{
  InjectionHandler::InjectionHandler():
    meta_data_(new MetaDataHandler()),
    raw_data_(new RawDataHandler())
  {
  }

  void InjectionHandler::clear()
  {
    if (meta_data_!=nullptr) meta_data_->clear();
    if (raw_data_!=nullptr) raw_data_->clear();
  }

  void InjectionHandler::setMetaData(const MetaDataHandler& meta_data)
  {
    meta_data_.reset(new MetaDataHandler(meta_data));
  }

  void InjectionHandler::setMetaData(std::shared_ptr<MetaDataHandler>& meta_data)
  {
    meta_data_ = meta_data;
  }

  MetaDataHandler& InjectionHandler::getMetaData()
  {
    return *(meta_data_.get());
  }

  const MetaDataHandler& InjectionHandler::getMetaData() const
  {
    return *(meta_data_.get());
  }

  std::shared_ptr<MetaDataHandler>& InjectionHandler::getMetaDataShared()
  {
    return meta_data_;
  }

  void InjectionHandler::setRawData(const RawDataHandler& raw_data)
  {
    raw_data_.reset(new RawDataHandler(raw_data));
  }

  void InjectionHandler::setRawData(std::shared_ptr<RawDataHandler>& raw_data)
  {
    raw_data_ = raw_data;
  }

  RawDataHandler& InjectionHandler::getRawData()
  {
    return *(raw_data_.get());
  }

  const RawDataHandler& InjectionHandler::getRawData() const
  {
    return *(raw_data_.get());
  }
  std::shared_ptr<RawDataHandler>& InjectionHandler::getRawDataShared()
  {
    return raw_data_;
  }
}
