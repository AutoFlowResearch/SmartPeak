// --------------------------------------------------------------------------
//   SmartPeak -- Fast and Accurate CE-, GC- and LC-MS(/MS) Data Processing
// --------------------------------------------------------------------------
// Copyright The SmartPeak Team -- Novo Nordisk Foundation 
// Center for Biosustainability, Technical University of Denmark 2018-2021.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL ANY OF THE AUTHORS OR THE CONTRIBUTING
// INSTITUTIONS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
// OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
// WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
// OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
// ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// --------------------------------------------------------------------------
// $Maintainer: Douglas McCloskey $
// $Authors: Douglas McCloskey, Pasquale Domenico Colaianni $
// --------------------------------------------------------------------------

#include <SmartPeak/core/InjectionHandler.h>

namespace SmartPeak
{
  InjectionHandler::InjectionHandler():
    meta_data_(std::make_shared<MetaDataHandler>(MetaDataHandler())),
    raw_data_(std::make_shared<RawDataHandler>(RawDataHandler()))
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
