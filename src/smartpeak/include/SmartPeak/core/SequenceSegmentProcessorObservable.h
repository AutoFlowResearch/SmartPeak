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

#include <SmartPeak/iface/ISequenceSegmentProcessorObserver.h>
#include <memory>
#include <vector>
#include <algorithm>

namespace SmartPeak
{
  class SequenceSegmentProcessorObservable
  {
  public:
    virtual void addSequenceSegmentProcessorObserver(ISequenceSegmentProcessorObserver* observer) 
    {
      if (nullptr != observer)
      {
        observers_.push_back(observer);
      }
    }
    virtual void removeSequenceSegmentProcessorObserver(ISequenceSegmentProcessorObserver* observer) 
    {
      if (nullptr != observer)
      {
        observers_.erase(std::remove(observers_.begin(), observers_.end(), observer), observers_.end());
      }
    }
    void notifySequenceSegmentProcessorStart(const size_t nb_segments)
    {
      for (auto& observer : observers_)
      {
        observer->onSequenceSegmentProcessorStart(nb_segments);
      }
    }
    void notifySequenceSegmentProcessorSampleStart(const std::string segment_name)
    {
      for (auto& observer : observers_)
      {
        observer->onSequenceSegmentProcessorSampleStart(segment_name);
      }
    }
    void notifySequenceSegmentProcessorSampleEnd(const std::string segment_name)
    {
      for (auto& observer : observers_)
      {
        observer->onSequenceSegmentProcessorSampleEnd(segment_name);
      }
    }
    void notifySequenceSegmentProcessorEnd()
    {
      for (auto& observer : observers_)
      {
        observer->onSequenceSegmentProcessorEnd();
      }
    }
  protected:
    std::vector<ISequenceSegmentProcessorObserver*> observers_;
  };
}
