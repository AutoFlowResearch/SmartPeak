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
#include <SmartPeak/core/RawDataProcessors/ConstructTransitionsList.h>
#include <SmartPeak/core/Filenames.h>
#include <SmartPeak/core/Utilities.h>
#include <SmartPeak/core/FeatureFiltersUtils.h>

#include <OpenMS/ANALYSIS/OPENSWATH/TargetedSpectraExtractor.h>
#include <OpenMS/ANALYSIS/OPENSWATH/TransitionTSVFile.h>
#include <OpenMS/FORMAT/TraMLFile.h>

#include <plog/Log.h>

#include <algorithm>
#include <exception>

namespace SmartPeak
{

  std::vector<std::string> ConstructTransitionsList::getRequirements() const
  {
    return { "sequence", "traML" };
  }

  std::set<std::string> ConstructTransitionsList::getInputs() const
  {
    return { "Features", "MS1 Features" };
  }

  std::set<std::string> ConstructTransitionsList::getOutputs() const
  {
    return { "Features" };
  }

  ParameterSet ConstructTransitionsList::getParameterSchema() const
  {
    OpenMS::TargetedSpectraExtractor oms_params;
    ParameterSet parameters({ oms_params });
    std::map<std::string, std::vector<std::map<std::string, std::string>>> param_struct({
    {"ConstructTransitionsList", {
    {
      {"name", "transitions_file_format"},
      {"type", "string"},
      {"value", "csv"},
      {"description", "Transition file format."},
      {"valid_strings", "['traML','csv']"}
    }
    }} });
    ParameterSet dda_params(param_struct);
    parameters.merge(dda_params);
    return parameters;
  }

  void ConstructTransitionsList::getFilenames(Filenames& filenames) const
  {
    filenames.addFileName("output_transitions_traML", "${FEATURES_OUTPUT_PATH}/${OUTPUT_INJECTION_NAME}.traML");
    filenames.addFileName("output_transitions_csv", "${FEATURES_OUTPUT_PATH}/${OUTPUT_INJECTION_NAME}.csv");
  };

  void ConstructTransitionsList::doProcess(RawDataHandler& rawDataHandler_IO,
    const ParameterSet& params_I,
    Filenames& filenames_I
  ) const
  {
    getFilenames(filenames_I);

    // Complete user parameters with schema
    ParameterSet params(params_I);
    params.merge(getParameterSchema());

    OpenMS::TargetedSpectraExtractor targeted_spectra_extractor;
    Utilities::setUserParameters(targeted_spectra_extractor, params);

    const auto& ms2_merged_features = rawDataHandler_IO.getFeatureMap();

    // Store - we want to store MS1 and the associated MS2 features 
    OpenMS::Param oms_params = targeted_spectra_extractor.getParameters();
    oms_params.setValue("deisotoping:use_deisotoper", "true");
    targeted_spectra_extractor.setParameters(oms_params);
    OpenMS::TargetedExperiment t_exp;
    auto ms1_merged_features = rawDataHandler_IO.getFeatureMap("ms1_merged_features");
    targeted_spectra_extractor.constructTransitionsList(ms1_merged_features, ms2_merged_features, t_exp);

    // write transitions
    auto transition_file_format = params.findParameter("ConstructTransitionsList", "transitions_file_format");
    if (!transition_file_format)
    {
      throw std::invalid_argument("ConstructTransitionsList:transitions_file_format Parameter not found");
    }

    if (transition_file_format->getValueAsString() == "traML")
    {
      LOGI << "Storing: " << filenames_I.getFullPath("output_traML").generic_string();
      auto transitions_filename = filenames_I.getFullPath("output_transitions_traML").generic_string();
      OpenMS::TraMLFile traml_file;
      traml_file.store(transitions_filename, t_exp);
    }
    else if (transition_file_format->getValueAsString() == "csv")
    {
      LOGI << "Storing: " << filenames_I.getFullPath("output_transitions_csv").generic_string();
      auto transitions_filename = filenames_I.getFullPath("output_transitions_csv").generic_string();
      OpenMS::TransitionTSVFile transition_tsv_file;
      transition_tsv_file.convertTargetedExperimentToTSV(transitions_filename.c_str(), t_exp);
    }

    // save the targeted experiments
    rawDataHandler_IO.setTargetedExperiment(t_exp);
  }
}
