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
#include <SmartPeak/core/RawDataProcessors/StoreFeatureQCsRDP.h>
#include <SmartPeak/core/Filenames.h>
#include <SmartPeak/core/Utilities.h>
#include <SmartPeak/core/FeatureFiltersUtils.h>
#include <SmartPeak/io/InputDataValidation.h>

#include <plog/Log.h>

#include <algorithm>
#include <exception>

namespace SmartPeak
{

  std::set<std::string> StoreFeatureQCsRDP::getInputs() const
  {
    return { "FeatureQC" };
  }

  std::set<std::string> StoreFeatureQCsRDP::getOutputs() const
  {
    return { };
  }

  bool StoreFeatureQCsRDP::onFilePicked(const std::filesystem::path& filename, ApplicationHandler* application_handler)
  {
    if (application_handler->sequenceHandler_.getSequence().size() == 0)
    {
      LOGE << "File cannot be loaded without first loading the sequence.";
      return false;
    }
    RawDataHandler& rawDataHandler = application_handler->sequenceHandler_.getSequence().at(0).getRawData();
    Filenames filenames;
    if (feature_filter_mode_ & FeatureFiltersUtilsMode::EFeatureFiltersModeGroup)
    {
      filenames.setFullPath("featureQCComponentGroups", filename);
    }
    else if (feature_filter_mode_ & FeatureFiltersUtilsMode::EFeatureFiltersModeComponent)
    {
      filenames.setFullPath("featureQCComponents", filename);
    }
    process(rawDataHandler, {}, filenames);
    return true;
  }

  std::vector<std::string> StoreFeatureQCsRDP::getRequirements() const
  {
    return { "sequence", "traML" };
  }

  void StoreFeatureQCsRDP::getFilenames(Filenames& filenames) const
  {
    filenames.addFileName("featureQCComponents", "${MAIN_DIR}/featureQCComponents.csv", "Components QC", true, true);
    filenames.addFileName("featureQCComponentGroups", "${MAIN_DIR}/featureQCComponentGroups.csv", "Components Group QC", true, true);
  };

  void StoreFeatureQCsRDP::process(
    RawDataHandler& rawDataHandler_IO,
    const ParameterSet& params_I,
    Filenames& filenames_I
  ) const
  {
    LOGD << "START storeFeatureQC";
    getFilenames(filenames_I);
    FeatureFiltersUtils::storeFeatureFilters(
      "featureQCComponents",
      "featureQCComponentGroups",
      filenames_I,
      rawDataHandler_IO.getFeatureQC(),
      feature_filter_mode_);
    LOGD << "END storeFeatureQC";
  }

}
