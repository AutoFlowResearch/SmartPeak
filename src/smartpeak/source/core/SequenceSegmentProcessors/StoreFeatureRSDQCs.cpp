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

#include <SmartPeak/core/SequenceSegmentProcessors/StoreFeatureRSDQCs.h>
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

  std::set<std::string> StoreFeatureRSDQCs::getInputs() const
  {
    return { "Feature RSD QCs" };
  }

  std::set<std::string> StoreFeatureRSDQCs::getOutputs() const
  {
    return { };
  }

  std::vector<std::string> StoreFeatureRSDQCs::getRequirements() const
  {
    return { "sequence", "traML" };
  }

  ParameterSet StoreFeatureRSDQCs::getParameterSchema() const
  {
    return ParameterSet();
  }

  bool StoreFeatureRSDQCs::onFilePicked(const std::filesystem::path& directory, ApplicationHandler* application_handler)
  {
    Filenames filenames;
    filenames.setTagValue(Filenames::Tag::FEATURES_OUTPUT_PATH, directory.generic_string());
    static_filenames_ = false;
    for (auto& sequence_segment : application_handler->sequenceHandler_.getSequenceSegments())
    {
      filenames.setTagValue(Filenames::Tag::OUTPUT_SEQUENCE_SEGMENT_NAME, sequence_segment.getSequenceSegmentName());
      process(sequence_segment, application_handler->sequenceHandler_, {}, filenames);
    }
    return true;
  }

  void StoreFeatureRSDQCs::getFilenames(Filenames& filenames) const
  {
    if (feature_filter_mode_ & FeatureFiltersUtilsMode::EFeatureFiltersModeGroup)
    {
      filenames.addFileName("featureRSDQCComponentGroups",
                            constructFilename("featureRSDQCComponentGroups.csv", static_filenames_),
                            "Components Group %RSD QCs");
    }
    else if (feature_filter_mode_ & FeatureFiltersUtilsMode::EFeatureFiltersModeComponent)
    {
      filenames.addFileName("featureRSDQCComponents",
                            constructFilename("featureRSDQCComponents.csv", static_filenames_),
                            "Components %RSD QCs");
    }
  };

  void StoreFeatureRSDQCs::doProcess(
    SequenceSegmentHandler& sequenceSegmentHandler_IO,
    const SequenceHandler& sequenceHandler_I,
    const ParameterSet& params_I,
    Filenames& filenames_I
  ) const
  {
    getFilenames(filenames_I);
    FeatureFiltersUtils::storeFeatureFilters(
      "featureRSDQCComponents",
      "featureRSDQCComponentGroups",
      filenames_I,
      sequenceSegmentHandler_IO.getFeatureRSDQC(),
      feature_filter_mode_);
  }

}
