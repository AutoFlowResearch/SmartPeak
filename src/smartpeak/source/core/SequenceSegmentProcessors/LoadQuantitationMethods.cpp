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

#include <SmartPeak/core/SequenceSegmentProcessors/LoadQuantitationMethods.h>
#include <SmartPeak/core/Filenames.h>
#include <SmartPeak/core/MetaDataHandler.h>
#include <SmartPeak/core/SampleType.h>
#include <SmartPeak/core/SequenceHandler.h>
#include <SmartPeak/core/Utilities.h>
#include <SmartPeak/core/ApplicationHandler.h>
#include <SmartPeak/core/FeatureFiltersUtils.h>
#include <SmartPeak/io/InputDataValidation.h>

#include <OpenMS/FORMAT/AbsoluteQuantitationMethodFile.h>

#include <plog/Log.h>


namespace SmartPeak
{

  std::vector<std::string> LoadQuantitationMethods::getRequirements() const
  {
    return { "sequence", "traML" };
  }

  ParameterSet LoadQuantitationMethods::getParameterSchema() const
  {
    return ParameterSet();
  }

  void LoadQuantitationMethods::getFilenames(Filenames& filenames) const
  {
    filenames.addFileName("quantitationMethods", constructFilename("quantitationMethods.csv", static_filenames_), "Quantitation Methods");
  };

  bool LoadQuantitationMethods::onFilePicked(const std::filesystem::path& filename, ApplicationHandler* application_handler)
  {
    if (application_handler->sequenceHandler_.getSequence().size() == 0)
    {
      LOGE << "File cannot be loaded without first loading the sequence.";
      return false;
    }
    Filenames filenames;
    filenames.setFullPath("quantitationMethods", filename);
    processForAllSegments(application_handler->sequenceHandler_.getSequenceSegments(),
      &(application_handler->sequenceHandler_),
      filenames);
    return true;
  }

  void LoadQuantitationMethods::process(
    SequenceSegmentHandler& sequenceSegmentHandler_IO,
    const SequenceHandler& sequenceHandler_I,
    const ParameterSet& params_I,
    Filenames& filenames_I
  ) const
  {
    LOGD << "START loadQuantitationMethods";
    getFilenames(filenames_I);

    if (!InputDataValidation::prepareToLoad(filenames_I, "quantitationMethods"))
    {
      throw std::invalid_argument("Failed to load input file");
    }

    try {
      auto quantitation_methods_file = filenames_I.getFullPath("quantitationMethods");
      // Sanity checks - OpenMS will not check for missing columns
      if (!Utilities::checkCSVHeader<','>(
        quantitation_methods_file,
        "IS_name",
        "component_name",
        "feature_name",
        "concentration_units",
        "llod",
        "ulod",
        "lloq",
        "uloq",
        "correlation_coefficient",
        "n_points",
        "transformation_model"))
      {
        throw std::invalid_argument(std::string("Missing headers in file \"") + quantitation_methods_file.generic_string() + std::string("\""));
      }
      // load file
      OpenMS::AbsoluteQuantitationMethodFile AQMf;
      AQMf.load(quantitation_methods_file.generic_string(), sequenceSegmentHandler_IO.getQuantitationMethods());
      if (sequence_segment_observable_) sequence_segment_observable_->notifyQuantitationMethodsUpdated();
    }
    catch (const std::exception& e) {
      LOGE << e.what();
      sequenceSegmentHandler_IO.getQuantitationMethods().clear();
      LOGI << "quantitation methods clear";
      throw;
    }
    LOGD << "END loadQuantitationMethods";
  }

}
