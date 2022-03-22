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

#include <SmartPeak/core/SequenceSegmentProcessors/LoadStandardsConcentrations.h>
#include <SmartPeak/core/Filenames.h>
#include <SmartPeak/core/MetaDataHandler.h>
#include <SmartPeak/core/SampleType.h>
#include <SmartPeak/core/SequenceHandler.h>
#include <SmartPeak/core/Utilities.h>
#include <SmartPeak/core/ApplicationHandler.h>
#include <SmartPeak/core/FeatureFiltersUtils.h>
#include <SmartPeak/io/InputDataValidation.h>

#include <OpenMS/FORMAT/AbsoluteQuantitationStandardsFile.h>

#include <plog/Log.h>

#include <sstream>

namespace SmartPeak
{

  std::set<std::string> LoadStandardsConcentrations::getInputs() const
  {
    return { };
  }

  std::set<std::string> LoadStandardsConcentrations::getOutputs() const
  {
    return { "Standards Concentrations" };
  }

  std::vector<std::string> LoadStandardsConcentrations::getRequirements() const
  {
    return { "sequence", "traML" };
  }

  ParameterSet LoadStandardsConcentrations::getParameterSchema() const
  {
    return ParameterSet();
  }

  bool LoadStandardsConcentrations::onFilePicked(const std::filesystem::path& filename, ApplicationHandler* application_handler)
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

  void LoadStandardsConcentrations::getFilenames(Filenames& filenames) const
  {
    filenames.addFileName("standardsConcentrations", constructFilename("standardsConcentrations.csv", static_filenames_), "Standards Concentrations", true, true);
  };

  void LoadStandardsConcentrations::doProcess(
    SequenceSegmentHandler& sequenceSegmentHandler_IO,
    const SequenceHandler& sequenceHandler_I,
    const ParameterSet& params_I,
    Filenames& filenames_I
  ) const
  {
    getFilenames(filenames_I);

    if (!InputDataValidation::prepareToLoad(filenames_I, "standardsConcentrations", true))
    {
      throw std::invalid_argument("Failed to load input file");
    }

    try
    {
      if (filenames_I.isEmbedded("standardsConcentrations"))
      {
        auto db_context = filenames_I.getSessionDB().beginRead(
          "standardsConcentrations",
          "sample_name",
          "component_name",
          "IS_component_name",
          "actual_concentration",
          "IS_actual_concentration",
          "concentration_units",
          "dilution_factor"
        );
        if (!db_context)
        {
          throw std::runtime_error("Failed to load from session database");
        }
        OpenMS::AbsoluteQuantitationStandards::runConcentration run_concentration;
        while (filenames_I.getSessionDB().read(
          *db_context,
          run_concentration.sample_name,
          run_concentration.component_name,
          run_concentration.IS_component_name,
          run_concentration.actual_concentration,
          run_concentration.IS_actual_concentration,
          run_concentration.concentration_units,
          run_concentration.dilution_factor
        ))
        {
          sequenceSegmentHandler_IO.getStandardsConcentrations().push_back(run_concentration);
        }
        filenames_I.getSessionDB().endRead(*db_context);
      }
      else
      {
        OpenMS::AbsoluteQuantitationStandardsFile AQSf;
        auto standards_concentrations_file = filenames_I.getFullPath("standardsConcentrations");
        // Sanity checks - OpenMS will not check for missing columns
        if (!Utilities::checkCSVHeader<','>(
          standards_concentrations_file, 
          "sample_name",
          "component_name",
          "IS_component_name",
          "actual_concentration",
          "IS_actual_concentration",
          "concentration_units",
          "dilution_factor"))
        {
          throw std::invalid_argument(std::string("Missing headers in file \"") + standards_concentrations_file.generic_string() + std::string("\""));
        }
        // load file
        AQSf.load(standards_concentrations_file.generic_string(), sequenceSegmentHandler_IO.getStandardsConcentrations());
      }
    }
    catch (const std::exception& e) {
      sequenceSegmentHandler_IO.getStandardsConcentrations().clear();
      LOGI << "Standards concentrations clear";
      throw;
    }
  }

}
