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

#include <SmartPeak/core/SequenceSegmentProcessors/LoadFeatureBackgroundFilters.h>
#include <SmartPeak/core/Filenames.h>
#include <SmartPeak/core/MetaDataHandler.h>
#include <SmartPeak/core/SampleType.h>
#include <SmartPeak/core/SequenceHandler.h>
#include <SmartPeak/core/Utilities.h>
#include <SmartPeak/core/ApplicationHandler.h>
#include <SmartPeak/core/FeatureFiltersUtils.h>
#include <SmartPeak/io/InputDataValidation.h>

#include <plog/Log.h>

namespace SmartPeak
{

  std::set<std::string> LoadFeatureBackgroundFilters::getInputs() const
  {
    return { };
  }

  std::set<std::string> LoadFeatureBackgroundFilters::getOutputs() const
  {
    return { "Feature Background Filters" };
  }

  std::vector<std::string> LoadFeatureBackgroundFilters::getRequirements() const
  {
    return { "sequence", "traML" };
  }

  ParameterSet LoadFeatureBackgroundFilters::getParameterSchema() const
  {
    return ParameterSet();
  }

  void LoadFeatureBackgroundFilters::getFilenames(Filenames& filenames) const
  {
    if (feature_filter_mode_ & FeatureFiltersUtilsMode::EFeatureFiltersModeGroup)
    {
      filenames.addFileName("featureBackgroundFilterComponentGroups", 
        constructFilename("featureBackgroundFilterComponentGroups.csv", static_filenames_),
        "Components Group % Background Filters",
        true,
        true);
    }
    else if (feature_filter_mode_ & FeatureFiltersUtilsMode::EFeatureFiltersModeComponent)
    {
      filenames.addFileName("featureBackgroundFilterComponents", 
        constructFilename("featureBackgroundFilterComponents.csv", static_filenames_),
        "Components % Background Filters",
        true,
        true);
    }
  };

  bool LoadFeatureBackgroundFilters::onFilePicked(const std::filesystem::path& filename, ApplicationHandler* application_handler)
  {
    Filenames filenames;
    if (!FeatureFiltersUtils::onFilePicked(
      filename,
      application_handler,
      filenames,
      "featureBackgroundFilterComponents",
      "featureBackgroundFilterComponentGroups",
      feature_filter_mode_))
    {
      return false;
    }
    processForAllSegments(application_handler->sequenceHandler_.getSequenceSegments(),
      &(application_handler->sequenceHandler_),
      filenames);
    return true;
  }

  void LoadFeatureBackgroundFilters::doProcess(
    SequenceSegmentHandler& sequenceSegmentHandler_IO,
    const SequenceHandler& sequenceHandler_I,
    const ParameterSet& params_I,
    Filenames& filenames_I
  ) const
  {
    getFilenames(filenames_I);
    FeatureFiltersUtils::loadFeatureFilters(
      "featureBackgroundFilterComponents",
      "featureBackgroundFilterComponentGroups",
      filenames_I,
      sequenceSegmentHandler_IO.getFeatureBackgroundFilter(),
      [&]() { if (sequence_segment_observable_) sequence_segment_observable_->notifyFeatureBackgroundFilterComponentsUpdated(); },
      [&]() { if (sequence_segment_observable_) sequence_segment_observable_->notifyFeatureBackgroundFilterComponentGroupsUpdated(); },
      feature_filter_mode_
    );
  }

}
