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

#include <SmartPeak/iface/ISequenceSegmentObserver.h>
#include <memory>
#include <vector>
#include <algorithm>

namespace SmartPeak
{
  class SequenceSegmentObservable
  {
  public:
    virtual void addSequenceSegmentObserver(ISequenceSegmentObserver* observer) 
    { 
      if (nullptr != observer)
      {
        observers_.push_back(observer); 
      }
    }
    virtual void removeSequenceSegmentObserver(ISequenceSegmentObserver* observer) 
    {
      if (nullptr != observer)
      {
        observers_.erase(std::remove(observers_.begin(), observers_.end(), observer), observers_.end()); 
      }
    }
    void notifyQuantitationMethodsUpdated()
    {
      for (auto& observer : observers_)
      {
        observer->onQuantitationMethodsUpdated();
      }
    }
    void notifyStandardsConcentrationsUpdated()
    {
      for (auto& observer : observers_)
      {
        observer->onStandardsConcentrationsUpdated();
      }
    }
    void notifyFeatureFiltersComponentsUpdated()
    {
      for (auto& observer : observers_)
      {
        observer->onFeatureFiltersComponentsUpdated();
      }
    }
    void notifyFeatureFiltersComponentGroupsUpdated()
    {
      for (auto& observer : observers_)
      {
        observer->onFeatureFiltersComponentGroupsUpdated();
      }
    }
    void notifyFeatureQCComponentsUpdated()
    {
      for (auto& observer : observers_)
      {
        observer->onFeatureQCComponentsUpdated();
      }
    }
    void notifyFeatureQCComponentGroupsUpdated()
    {
      for (auto& observer : observers_)
      {
        observer->onFeatureQCComponentGroupsUpdated();
      }
    }
    void notifyFeatureRSDFilterComponentsUpdated()
    {
      for (auto& observer : observers_)
      {
        observer->onFeatureRSDFilterComponentsUpdated();
      }
    }
    void notifyFeatureRSDFilterComponentGroupsUpdated()
    {
      for (auto& observer : observers_)
      {
        observer->onFeatureRSDFilterComponentGroupsUpdated();
      }
    }
    void notifyFeatureRSDQCComponentsUpdated()
    {
      for (auto& observer : observers_)
      {
        observer->onFeatureRSDQCComponentsUpdated();
      }
    }
    void notifyFeatureRSDQCComponentGroupsUpdated()
    {
      for (auto& observer : observers_)
      {
        observer->onFeatureRSDQCComponentGroupsUpdated();
      }
    }
    void notifyFeatureBackgroundFilterComponentsUpdated()
    {
      for (auto& observer : observers_)
      {
        observer->onFeatureBackgroundFilterComponentsUpdated();
      }
    }
    void notifyFeatureBackgroundFilterComponentGroupsUpdated()
    {
      for (auto& observer : observers_)
      {
        observer->onFeatureBackgroundFilterComponentGroupsUpdated();
      }
    }
    void notifyFeatureBackgroundQCComponentsUpdated()
    {
      for (auto& observer : observers_)
      {
        observer->onFeatureBackgroundQCComponentsUpdated();
      }
    }
    void notifyFeatureBackgroundQCComponentGroupsUpdated()
    {
      for (auto& observer : observers_)
      {
        observer->onFeatureBackgroundQCComponentGroupsUpdated();
      }
    }
  protected:
    std::vector<ISequenceSegmentObserver*> observers_;
  };
}
