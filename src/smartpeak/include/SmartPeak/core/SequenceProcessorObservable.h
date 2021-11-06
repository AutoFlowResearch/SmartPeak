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
// $Maintainer: Bertrand Boudaud, Douglas McCloskey $
// $Authors: Douglas McCloskey, Bertrand Boudaud $
// --------------------------------------------------------------------------

#pragma once

#include <SmartPeak/iface/ISequenceProcessorObserver.h>
#include <memory>
#include <vector>
#include <algorithm>

namespace SmartPeak
{
  class SequenceProcessorObservable
  {
  public:
    virtual void addSequenceProcessorObserver(ISequenceProcessorObserver* observer) 
    {
      if (nullptr != observer)
      {
        observers_.push_back(observer);
      }
    }
    virtual void removeSequenceProcessorObserver(ISequenceProcessorObserver* observer) 
    {
      if (nullptr != observer)
      {
        observers_.erase(std::remove(observers_.begin(), observers_.end(), observer), observers_.end());
      }
    }
    void notifySequenceProcessorStart(const size_t nb_injections)
    {
      for (auto& observer : observers_)
      {
        observer->onSequenceProcessorStart(nb_injections);
      }
    }
    void notifySequenceProcessorSampleStart(const std::string sample_name)
    {
      for (auto& observer : observers_)
      {
        observer->onSequenceProcessorSampleStart(sample_name);
      }
    }
    void notifySequenceProcessorSampleEnd(const std::string sample_name)
    {
      for (auto& observer : observers_)
      {
        observer->onSequenceProcessorSampleEnd(sample_name);
      }
    }
    void notifySequenceProcessorEnd()
    {
      for (auto& observer : observers_)
      {
        observer->onSequenceProcessorEnd();
      }
    }
    void notifySequenceProcessorError(const std::string& sample_name, const std::string& processor_name, const std::string& error)
    {
      for (auto& observer : observers_)
      {
        observer->onSequenceProcessorError(sample_name, processor_name, error);
      }
    }
  protected:
    std::vector<ISequenceProcessorObserver*> observers_;
  };
}
