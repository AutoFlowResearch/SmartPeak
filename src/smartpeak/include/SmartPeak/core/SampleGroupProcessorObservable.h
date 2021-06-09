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
// $Maintainer: Bertrand Boudaud $
// $Authors: Douglas McCloskey, Bertrand Boudaud $
// --------------------------------------------------------------------------

#pragma once

#include <SmartPeak/iface/ISampleGroupProcessorObserver.h>
#include <memory>
#include <vector>
#include <algorithm>

namespace SmartPeak
{
  class SampleGroupProcessorObservable
  {
  public:
    virtual void addSampleGroupProcessorObserver(ISampleGroupProcessorObserver* observer) 
    { 
      if (nullptr != observer)
      {
        observers_.push_back(observer); 
      }
    }
    virtual void removeSampleGroupProcessorObserver(ISampleGroupProcessorObserver* observer) 
    { 
      if (nullptr != observer)
      {
        observers_.erase(std::remove(observers_.begin(), observers_.end(), observer), observers_.end()); 
      }
    }
    void notifySampleGroupProcessorStart(const size_t nb_segments)
    {
      for (auto& observer : observers_)
      {
        observer->onSampleGroupProcessorStart(nb_segments);
      }
    }
    void notifySampleGroupProcessorSampleStart(const std::string sample_group_name)
    {
      for (auto& observer : observers_)
      {
        observer->onSampleGroupProcessorSampleStart(sample_group_name);
      }
    }
    void notifySampleGroupProcessorSampleEnd(const std::string sample_group_name)
    {
      for (auto& observer : observers_)
      {
        observer->onSampleGroupProcessorSampleEnd(sample_group_name);
      }
    }
    void notifySampleGroupProcessorEnd()
    {
      for (auto& observer : observers_)
      {
        observer->onSampleGroupProcessorEnd();
      }
    }
  protected:
    std::vector<ISampleGroupProcessorObserver*> observers_;
  };
}
