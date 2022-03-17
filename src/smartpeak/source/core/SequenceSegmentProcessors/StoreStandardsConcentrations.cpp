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

#include <SmartPeak/core/SequenceSegmentProcessors/StoreStandardsConcentrations.h>
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

  std::set<std::string> StoreStandardsConcentrations::getInputs() const
  {
    return { "Standards Concentrations" };
  }

  std::set<std::string> StoreStandardsConcentrations::getOutputs() const
  {
    return { };
  }

  std::vector<std::string> StoreStandardsConcentrations::getRequirements() const
  {
    return { "sequence", "traML" };
  }

  ParameterSet StoreStandardsConcentrations::getParameterSchema() const
  {
    return ParameterSet();
  }

  bool StoreStandardsConcentrations::onFilePicked(const std::filesystem::path& filename, ApplicationHandler* application_handler)
  {
    if (application_handler->sequenceHandler_.getSequence().size() == 0)
    {
      LOGE << "File cannot be loaded without first loading the sequence.";
      return false;
    }
    Filenames filenames;
    filenames.setFullPath("standardsConcentrations", filename);
    sequence_segment_observable_ = &application_handler->sequenceHandler_;
    process(application_handler->sequenceHandler_.getSequenceSegments()[0], SequenceHandler(), {}, filenames);
    return true;
  }

  void StoreStandardsConcentrations::getFilenames(Filenames& filenames) const
  {
    filenames.addFileName("standardsConcentrations", constructFilename("standardsConcentrations.csv", static_filenames_), "Standards Concentrations", true, true);
  };

  void StoreStandardsConcentrations::process(
    SequenceSegmentHandler& sequenceSegmentHandler_IO,
    const SequenceHandler& sequenceHandler_I,
    const ParameterSet& params_I,
    Filenames& filenames_I
  ) const
  {
    LOGD << "START StoreStandardsConcentrations";

    if (!InputDataValidation::prepareToStore(filenames_I, "standardsConcentrations"))
    {
      throw std::invalid_argument("Failed to store output file");
    }
    if (filenames_I.isEmbedded("standardsConcentrations"))
    {
      auto db_context = filenames_I.getSessionDB().beginWrite(
        "standardsConcentrations",
        "sample_name", "TEXT",
        "component_name", "TEXT",
        "IS_component_name", "TEXT",
        "actual_concentration", "REAL",
        "IS_actual_concentration", "REAL",
        "concentration_units", "TEXT",
        "dilution_factor", "REAL"
        );
      if (!db_context)
      {
        throw std::runtime_error("Failed to save in session database");
      }
      for (const auto& concentration : sequenceSegmentHandler_IO.getStandardsConcentrations())
      {
        filenames_I.getSessionDB().write(
          *db_context,
          concentration.sample_name,
          concentration.component_name,
          concentration.IS_component_name,
          concentration.actual_concentration,
          concentration.IS_actual_concentration,
          concentration.concentration_units,
          concentration.dilution_factor
          );
      }
      filenames_I.getSessionDB().endWrite(*db_context);
    }
    else
    {
      LOGE << "Not implemented";
    }
  }

}
