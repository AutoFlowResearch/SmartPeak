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

#include <SmartPeak/core/SequenceSegmentProcessors/StoreFeatureBackgroundEstimations.h>
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

  std::set<std::string> StoreFeatureBackgroundEstimations::getInputs() const
  {
    return { "Feature Background Estimations" };
  }

  std::set<std::string> StoreFeatureBackgroundEstimations::getOutputs() const
  {
    return { };
  }

  bool StoreFeatureBackgroundEstimations::onFilePicked(const std::filesystem::path& directory, ApplicationHandler* application_handler)
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

  std::vector<std::string> StoreFeatureBackgroundEstimations::getRequirements() const
  {
    return { "sequence", "traML" };
  }

  ParameterSet StoreFeatureBackgroundEstimations::getParameterSchema() const
  {
    return ParameterSet();
  }

  void StoreFeatureBackgroundEstimations::getFilenames(Filenames& filenames) const
  {
    if (feature_filter_mode_ & FeatureFiltersUtilsMode::EFeatureFiltersModeGroup)
    {
      filenames.addFileName("featureBackgroundEstimationComponentGroups",
        constructFilename("featureBackgroundEstimationComponentGroups.csv", static_filenames_),
        "Feature Background Estimation Groups",
        true,
        true
      );
    }
    else if (feature_filter_mode_ & FeatureFiltersUtilsMode::EFeatureFiltersModeComponent)
    {
      filenames.addFileName("featureBackgroundEstimationComponents",
        constructFilename("featureBackgroundEstimationComponents.csv", static_filenames_),
        "Feature Background Estimation",
        true,
        true
        );
    }
  };

  void StoreFeatureBackgroundEstimations::doProcess(
    SequenceSegmentHandler& sequenceSegmentHandler_IO,
    const SequenceHandler& sequenceHandler_I,
    const ParameterSet& params_I,
    Filenames& filenames_I
  ) const
  {
    getFilenames(filenames_I);
    FeatureFiltersUtils::storeFeatureFilters(
      "featureBackgroundEstimationComponents",
      "featureBackgroundEstimationComponentGroups",
      filenames_I,
      sequenceSegmentHandler_IO.getFeatureBackgroundEstimations(),
      feature_filter_mode_);
  }

}
