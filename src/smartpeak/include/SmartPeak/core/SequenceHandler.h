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

#pragma once

#include <SmartPeak/core/WorkflowObservable.h>
#include <SmartPeak/core/ParametersObservable.h>
#include <SmartPeak/core/SequenceObservable.h>
#include <SmartPeak/core/MetaDataHandler.h>
#include <SmartPeak/core/SampleGroupHandler.h>
#include <SmartPeak/core/InjectionHandler.h>
#include <SmartPeak/core/SequenceSegmentHandler.h>
#include <SmartPeak/core/CastValue.h>

namespace SmartPeak
{
  class SequenceHandler : 
    public ParametersObservable, 
    public WorkflowObservable,
    public SequenceObservable
  {
public:
    void clear();

    void setSequence(const std::vector<InjectionHandler>& sequence);
    std::vector<InjectionHandler>& getSequence();
    const std::vector<InjectionHandler>& getSequence() const;

    void setSequenceSegments(const std::vector<SequenceSegmentHandler>& sequence_segments);
    std::vector<SequenceSegmentHandler>& getSequenceSegments();
    const std::vector<SequenceSegmentHandler>& getSequenceSegments() const;

    void setSampleGroups(const std::vector<SampleGroupHandler>& sample_groups);
    std::vector<SampleGroupHandler>& getSampleGroups();
    const std::vector<SampleGroupHandler>& getSampleGroups() const;

    void addSampleToSequence(
      const MetaDataHandler& meta_data_I,
      const OpenMS::FeatureMap& featureMap_I
    );

    void setWorkflow(const std::vector<std::string>& commands);
    std::vector<std::string>& getWorkflow();
    const std::vector<std::string>& getWorkflow() const;

    std::vector<InjectionHandler> getSamplesInSequence(
      const std::set<std::string>& injection_names
    ) const;

    static CastValue getMetaValue(
      const OpenMS::Feature& feature,
      const OpenMS::Feature& subordinate,
      const std::string& meta_value
    );

    std::string getRawDataFilesInfo() const;
    std::string getAnalyzedFeaturesInfo() const;
    std::string getSelectedFeaturesInfo() const;
    std::string getPickedPeaksInfo() const;
    std::string getFilteredSelectedPeaksInfo() const;

private:
    std::vector<InjectionHandler> sequence_;
    std::vector<SequenceSegmentHandler> sequence_segments_;
    std::vector<SampleGroupHandler> sample_groups_;
    std::vector<std::string> command_names_;
  };
}
