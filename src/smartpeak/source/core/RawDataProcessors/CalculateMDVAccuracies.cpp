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
#include <SmartPeak/core/RawDataProcessors/CalculateMDVAccuracies.h>
#include <SmartPeak/core/Filenames.h>
#include <SmartPeak/core/Utilities.h>
#include <SmartPeak/core/FeatureFiltersUtils.h>

#include <OpenMS/ANALYSIS/QUANTITATION/IsotopeLabelingMDVs.h>

#include <plog/Log.h>

#include <algorithm>
#include <exception>

namespace SmartPeak
{

  std::vector<std::string> CalculateMDVAccuracies::getRequirements() const
  {
    return { "sequence", "traML" };
  }

  ParameterSet CalculateMDVAccuracies::getParameterSchema() const
  {
    std::map<std::string, std::vector<std::map<std::string, std::string>>> param_struct({
    {"CalculateMDVAccuracies", {
      {
        {"name", "feature_name"},
        {"type", "string"},
        {"value", ""},
        {"description", "The name of the FeatureMap attribute to use. Examples include peak_apex_int, peak_area, and intensity."},
      },
    }} });
    return ParameterSet(param_struct);
  }

  void CalculateMDVAccuracies::process(
    RawDataHandler& rawDataHandler_IO,
    const ParameterSet& params_I,
    Filenames& filenames_I
  ) const
  {
    LOGD << "START CalculateMDVAccuracies";
    getFilenames(filenames_I);

    // Complete user parameters with schema
    ParameterSet params(params_I);
    params.merge(getParameterSchema());

    // Set up CalculateMDVs and parse params
    OpenMS::IsotopeLabelingMDVs isotopelabelingmdvs;
    OpenMS::Param parameters = isotopelabelingmdvs.getParameters();
    isotopelabelingmdvs.setParameters(parameters);

    OpenMS::FeatureMap featureMap_with_accuracy_info;
    auto& CalculateMDVAccuracies_params = params.at("CalculateMDVAccuracies");
      
    //std::vector<double> fragment_isotopomer_measured;
    std::string fragment_isotopomer_theoretical_formula, fragment_isotopomer_measured_s, feature_name;
    std::map<std::string, std::string> proteinName_to_SumFormula;
      
    for (auto& peptide : rawDataHandler_IO.getTargetedExperiment().getPeptides())
    {
      if (peptide.metaValueExists("SumFormula") && !peptide.id.empty()
          && proteinName_to_SumFormula.find((std::string)(peptide.id)) == proteinName_to_SumFormula.end())
      {
        std::string sum_formula = (std::string)peptide.getMetaValue("SumFormula");
        std::string peptide_id = (std::string)(peptide.id);
        sum_formula.erase(std::remove_if(sum_formula.begin(), sum_formula.end(), [](unsigned char c){return std::isspace(c);}), sum_formula.end());
        peptide_id.erase(std::remove_if(peptide_id.begin(), peptide_id.end(), [](unsigned char c){return std::isspace(c);}), peptide_id.end());
        proteinName_to_SumFormula.insert(std::make_pair(peptide_id, sum_formula));
      }
    }
      
    for (auto& param : CalculateMDVAccuracies_params)
    {
      if (param.getName() == "feature_name")
      {
        if (!param.getValueAsString().empty())
        {
          feature_name =  param.getValueAsString();
        }
      }
    }
      
    featureMap_with_accuracy_info = rawDataHandler_IO.getFeatureMap();
    isotopelabelingmdvs.calculateMDVAccuracies(featureMap_with_accuracy_info, feature_name, proteinName_to_SumFormula);
    rawDataHandler_IO.setFeatureMap(featureMap_with_accuracy_info);
    rawDataHandler_IO.updateFeatureMapHistory();

    LOGD << "END CalculateMDVAccuracies";
  }

}
