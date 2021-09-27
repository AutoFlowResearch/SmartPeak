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

#include <SmartPeak/core/ApplicationHandler.h>
#include <SmartPeak/core/ApplicationProcessor.h>
#include <SmartPeak/core/SharedProcessors.h>
#include <SmartPeak/core/SequenceProcessor.h>

namespace SmartPeak
{
  ApplicationHandler::ApplicationHandler()
  {
    sequenceHandler_.addParametersObserver(this);
    sequenceHandler_.addWorkflowObserver(this);
    loading_processors_ =
    {
        std::make_shared<LoadSequence>(sequenceHandler_), // Order is important, LoadSequence should come 1st
        std::make_shared<LoadTransitions>(),
        std::make_shared<LoadParameters>(),
        std::make_shared<LoadWorkflow>(sequenceHandler_),
        std::make_shared<LoadValidationData>(),
        std::make_shared<LoadQuantitationMethods>(true),
        std::make_shared<LoadStandardsConcentrations>(true),
        std::make_shared<LoadFeatureFilters>(FeatureFiltersUtils::EHandleComponents, true),
        std::make_shared<LoadFeatureFilters>(FeatureFiltersUtils::EHandleComponentsGroups, true),
        std::make_shared<LoadFeatureQCs>(FeatureFiltersUtils::EHandleComponents, true),
        std::make_shared<LoadFeatureQCs>(FeatureFiltersUtils::EHandleComponentsGroups, true),
        std::make_shared<LoadFeatureRSDFilters>(FeatureFiltersUtils::EHandleComponents, true),
        std::make_shared<LoadFeatureRSDFilters>(FeatureFiltersUtils::EHandleComponentsGroups, true),
        std::make_shared<LoadFeatureRSDQCs>(FeatureFiltersUtils::EHandleComponents, true),
        std::make_shared<LoadFeatureRSDQCs>(FeatureFiltersUtils::EHandleComponentsGroups, true),
        std::make_shared<LoadFeatureBackgroundFilters>(FeatureFiltersUtils::EHandleComponents, true),
        std::make_shared<LoadFeatureBackgroundFilters>(FeatureFiltersUtils::EHandleComponentsGroups, true),
        std::make_shared<LoadFeatureBackgroundQCs>(FeatureFiltersUtils::EHandleComponents, true),
        std::make_shared<LoadFeatureBackgroundQCs>(FeatureFiltersUtils::EHandleComponentsGroups, true),
        std::make_shared<LoadFeatureRSDEstimations>(FeatureFiltersUtils::EHandleComponents, true),
        std::make_shared<LoadFeatureRSDEstimations>(FeatureFiltersUtils::EHandleComponentsGroups, true),
        std::make_shared<LoadFeatureBackgroundEstimations>(FeatureFiltersUtils::EHandleComponents, true),
        std::make_shared<LoadFeatureBackgroundEstimations>(FeatureFiltersUtils::EHandleComponentsGroups, true)
    };
    storing_processors_ =
    {
        std::make_shared<StoreSequence>(sequenceHandler_),
        //std::make_shared<StoreTransitions>(),
        std::make_shared<StoreParameters>(),
        std::make_shared<StoreWorkflow>(sequenceHandler_),
        std::make_shared<StoreValidationData>(),
        std::make_shared<StoreQuantitationMethods>(true),
        std::make_shared<StoreStandardsConcentrations>(true),
        std::make_shared<StoreFeatureFilters>(FeatureFiltersUtils::EHandleComponentsGroups, true),
        std::make_shared<StoreFeatureFilters>(FeatureFiltersUtils::EHandleComponents, true),
        std::make_shared<StoreFeatureQCs>(FeatureFiltersUtils::EHandleComponentsGroups, true),
        std::make_shared<StoreFeatureQCs>(FeatureFiltersUtils::EHandleComponents, true),
        std::make_shared<StoreFeatureRSDFilters>(FeatureFiltersUtils::EHandleComponentsGroups, true),
        std::make_shared<StoreFeatureRSDFilters>(FeatureFiltersUtils::EHandleComponents, true),
        std::make_shared<StoreFeatureRSDQCs>(FeatureFiltersUtils::EHandleComponentsGroups, true),
        std::make_shared<StoreFeatureRSDQCs>(FeatureFiltersUtils::EHandleComponents, true),
        std::make_shared<StoreFeatureBackgroundFilters>(FeatureFiltersUtils::EHandleComponentsGroups, true),
        std::make_shared<StoreFeatureBackgroundFilters>(FeatureFiltersUtils::EHandleComponents, true),
        std::make_shared<StoreFeatureBackgroundQCs>(FeatureFiltersUtils::EHandleComponentsGroups, true),
        std::make_shared<StoreFeatureBackgroundQCs>(FeatureFiltersUtils::EHandleComponents, true),
        std::make_shared<StoreFeatureRSDEstimations>(FeatureFiltersUtils::EHandleComponentsGroups, true),
        std::make_shared<StoreFeatureRSDEstimations>(FeatureFiltersUtils::EHandleComponents, true),
        std::make_shared<StoreFeatureBackgroundEstimations>(FeatureFiltersUtils::EHandleComponentsGroups, true),
        std::make_shared<StoreFeatureBackgroundEstimations>(FeatureFiltersUtils::EHandleComponents, true)
    };
  }

  void ApplicationHandler::onParametersUpdated()
  {
    setFileSavedState("parameters", false);
  }

  void ApplicationHandler::onWorkflowUpdated()
  {
    setFileSavedState("workflow", false);
  }

  void ApplicationHandler::closeSession()
  {
    LOGI << "Pathnames for 'mzML', 'INPUT features' and 'OUTPUT features' reset.";
    sequence_pathname_.clear();
    std::filesystem::path main_dir_ = ".";
    mzML_dir_.clear();
    features_in_dir_.clear();
    features_out_dir_.clear();
    sequenceHandler_.clear();
    filenames_ = Filenames();
  }
  
  bool ApplicationHandler::sessionIsOpened() const
  {
    return !filenames_.getFileIds().empty();
  }

  void ApplicationHandler::Command::setMethod(const std::shared_ptr<RawDataProcessor> method)
  {
    type = RawDataMethod;
    raw_data_method = method;
  }

  void ApplicationHandler::Command::setMethod(const std::shared_ptr<SequenceSegmentProcessor> method)
  {
    type = SequenceSegmentMethod;
    seq_seg_method = method;
  }
  
  void ApplicationHandler::Command::setMethod(const std::shared_ptr<SampleGroupProcessor> method)
  {
    type = SampleGroupMethod;
    sample_group_method = method;
  }

  std::string ApplicationHandler::Command::getName() const
  {
    const auto description = getIProcessorDescription();
    return (description ? description->getName() : "");
  }

  ParameterSet ApplicationHandler::Command::getParameterSchema() const
  {
    const auto description = getIProcessorDescription();
    return (description ? description->getParameterSchema() : ParameterSet());
  }

  std::string ApplicationHandler::Command::getDescription() const
  {
    const auto description = getIProcessorDescription();
    return (description ? description->getDescription() : "");
  }

  const IProcessorDescription* ApplicationHandler::Command::getIProcessorDescription() const
  {
    switch (type)
    {
    case RawDataMethod:
      return raw_data_method.get();
    case SequenceSegmentMethod:
      return seq_seg_method.get();
    case SampleGroupMethod:
      return sample_group_method.get();
    default:
      throw "Unsupported CommandType in ApplicationHandler::Command::getIProcessorDescription()";
    }
  }

  bool ApplicationHandler::sessionIsSaved() const
  {
    bool saved = true;
    for (const auto& saved_file_pair : saved_files_)
    {
      if (!saved_file_pair.second)
      {
        saved = false;
        break;
      }
    }
    return saved;
  }


  void ApplicationHandler::setFileSavedState(const std::string& file_id, bool saved_state)
  {
    saved_files_.insert_or_assign(file_id, saved_state);
  }

  bool ApplicationHandler::isFileSaved(const std::string& file_id) const
  {
    if (!saved_files_.count(file_id))
    {
      return true;
    }
    return saved_files_.at(file_id);
  }

}
