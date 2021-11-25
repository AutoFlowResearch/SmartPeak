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

#include <SmartPeak/core/SequenceSegmentProcessor.h>
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

  std::vector<std::string> LoadFeatureRSDFilters::getRequirements() const
  {
    return { "sequence", "traML" };
  }

  ParameterSet LoadFeatureRSDFilters::getParameterSchema() const
  {
    return ParameterSet();
  }

  void LoadFeatureRSDFilters::getFilenames(Filenames& filenames) const
  {
    if (feature_filter_mode_ & FeatureFiltersUtilsMode::EFeatureFiltersModeGroup)
    {
      filenames.addFileName("featureRSDFilterComponentGroups", 
                            constructFilename("featureRSDFilterComponentGroups.csv", static_filenames_),
                            "Components group %RSD Filters",
                            true,
                            true);
    }
    else if (feature_filter_mode_ & FeatureFiltersUtilsMode::EFeatureFiltersModeComponent)
    {
      filenames.addFileName("featureRSDFilterComponents",
                            constructFilename("featureRSDFilterComponents.csv", static_filenames_),
                            "Components %RSD Filters",
                            true,
                            true);
    }
  };

  bool LoadFeatureRSDFilters::onFilePicked(const std::filesystem::path& filename, ApplicationHandler* application_handler)
  {
    Filenames filenames;
    if (!FeatureFiltersUtils::onFilePicked(
      filename,
      application_handler,
      filenames,
      "featureRSDFilterComponents",
      "featureRSDFilterComponentGroups",
      feature_filter_mode_))
    {
      return false;
    }
    processForAllSegments(application_handler->sequenceHandler_.getSequenceSegments(),
      &(application_handler->sequenceHandler_),
      filenames);
    return true;
  }

  void LoadFeatureRSDFilters::process(
    SequenceSegmentHandler& sequenceSegmentHandler_IO,
    const SequenceHandler& sequenceHandler_I,
    const ParameterSet& params_I,
    Filenames& filenames_I
  ) const
  {
    LOGD << "START loadFeatureRSDFilter";
    getFilenames(filenames_I);
    FeatureFiltersUtils::loadFeatureFilters(
      "featureRSDFilterComponents",
      "featureRSDFilterComponentGroups",
      filenames_I,
      sequenceSegmentHandler_IO.getFeatureRSDFilter(),
      [&]() { if (sequence_segment_observable_) sequence_segment_observable_->notifyFeatureRSDFilterComponentsUpdated(); },
      [&]() { if (sequence_segment_observable_) sequence_segment_observable_->notifyFeatureRSDFilterComponentGroupsUpdated(); },
      feature_filter_mode_
    );
    LOGD << "END loadFeatureRSDFilter";
  }

}
