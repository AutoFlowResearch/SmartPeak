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
#include <SmartPeak/core/RawDataProcessors/LoadTransitions.h>
#include <SmartPeak/core/Filenames.h>
#include <SmartPeak/core/Utilities.h>
#include <SmartPeak/io/InputDataValidation.h>

#include <OpenMS/FORMAT/TraMLFile.h>
#include <OpenMS/ANALYSIS/OPENSWATH/TransitionTSVFile.h>

#include <plog/Log.h>

#include <algorithm>
#include <exception>

namespace SmartPeak
{
  std::set<std::string> LoadTransitions::getOutputs() const
  {
    return { "Targeted Experiment" };
  }

  bool LoadTransitions::onFilePicked(const std::filesystem::path& filename, ApplicationHandler* application_handler)
  {
    if (application_handler->sequenceHandler_.getSequence().size() == 0)
    {
      LOGE << "File cannot be loaded without first loading the sequence.";
      return false;
    }
    RawDataHandler& rawDataHandler = application_handler->sequenceHandler_.getSequence().at(0).getRawData();
    transitions_observable_ = &(application_handler->sequenceHandler_);
    Filenames filenames;
    filenames.setFullPath("traML", filename);
    process(rawDataHandler, {}, filenames);
    return true;
  }

  ParameterSet LoadTransitions::getParameterSchema() const
  {
    std::map<std::string, std::vector<std::map<std::string, std::string>>> param_struct({
    {"LoadTransitions", {
      {
        {"name", "format"},
        {"type", "string"},
        {"value", "csv"},
        {"description", "Transitions file format"},
        {"valid_strings", "['csv','traML']"}
      }
    }} });
    return ParameterSet(param_struct);
  }

  void LoadTransitions::getFilenames(Filenames& filenames) const
  {
    filenames.addFileName("traML", "${MAIN_DIR}/traML.csv", "Transitions", false, false);
  };

  void LoadTransitions::doProcess(
    RawDataHandler& rawDataHandler_IO,
    const ParameterSet& params_I,
    Filenames& filenames_I
  ) const
  {
    getFilenames(filenames_I);
    // Complete user parameters with schema
    ParameterSet params(params_I);
    params.merge(getParameterSchema());

    if (!InputDataValidation::prepareToLoad(filenames_I, "traML", true))
    {
      throw std::invalid_argument("Failed to load input file");
    }

    const auto format_param = params.findParameter("LoadTransitions", "format");
    if (!format_param)
    {
      // should actually not happen since we merge with the default params
      throw std::invalid_argument("LoadTransitions/format parameter not found");
    }

    const std::string format = format_param->getValueAsString();

    LOGI << "Loading " << filenames_I.getFullPath("traML").generic_string();
    LOGI << "Format: " << format;

    try {
      // must use "PeptideSequence"
      if (format == "csv") {
        // Transitions are appended to the existing experiment in OpenMS
        rawDataHandler_IO.getTargetedExperiment().clear(true);
        OpenMS::TransitionTSVFile tsvfile;
        tsvfile.convertTSVToTargetedExperiment(
          filenames_I.getFullPath("traML").generic_string().c_str(),
          OpenMS::FileTypes::TRAML,
          rawDataHandler_IO.getTargetedExperiment()
        );
        if (transitions_observable_) transitions_observable_->notifyTransitionsUpdated();
      }
      else if (format == "traML") 
      {
        // Transitions are appended to the existing experiment in OpenMS
        rawDataHandler_IO.getTargetedExperiment().clear(true);
        OpenMS::TraMLFile tramlfile;
        tramlfile.load(filenames_I.getFullPath("traML").generic_string(), rawDataHandler_IO.getTargetedExperiment());
        if (transitions_observable_) transitions_observable_->notifyTransitionsUpdated();
      }
      else 
      {
        LOGE << "Format must either be 'csv' or 'traML'";
      }
    }
    catch (const std::exception& e) {
      LOGE << e.what();
      rawDataHandler_IO.getTargetedExperiment().clear(true);
      LOGI << "targeted experiment clear";
      throw;
    }
  }

}
