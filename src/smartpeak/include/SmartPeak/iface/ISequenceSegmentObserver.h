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
// $Maintainer: Douglas McCloskey, Bertrand Boudaud $
// $Authors: Douglas McCloskey, Bertrand Boudaud $
// --------------------------------------------------------------------------

#pragma once

namespace SmartPeak 
{
  struct ISequenceSegmentObserver 
  {
    virtual void onQuantitationMethodsUpdated() = 0;
    virtual void onStandardsConcentrationsUpdated() = 0;
    virtual void onFeatureFiltersComponentsUpdated() = 0;
    virtual void onFeatureFiltersComponentGroupsUpdated() = 0;
    virtual void onFeatureQCComponentsUpdated() = 0;
    virtual void onFeatureQCComponentGroupsUpdated() = 0;
    virtual void onFeatureRSDFilterComponentsUpdated() = 0;
    virtual void onFeatureRSDFilterComponentGroupsUpdated() = 0;
    virtual void onFeatureRSDQCComponentsUpdated() = 0;
    virtual void onFeatureRSDQCComponentGroupsUpdated() = 0;
    virtual void onFeatureBackgroundFilterComponentsUpdated() = 0;
    virtual void onFeatureBackgroundFilterComponentGroupsUpdated() = 0;
    virtual void onFeatureBackgroundQCComponentsUpdated() = 0;
    virtual void onFeatureBackgroundQCComponentGroupsUpdated() = 0;
  };
}