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
#include <SmartPeak/core/RawDataProcessors/CalculateIsotopicPurities.h>
#include <SmartPeak/core/Filenames.h>
#include <SmartPeak/core/Utilities.h>
#include <SmartPeak/core/FeatureFiltersUtils.h>

#include <OpenMS/ANALYSIS/QUANTITATION/IsotopeLabelingMDVs.h>

#include <plog/Log.h>

#include <algorithm>
#include <exception>

namespace SmartPeak
{

  std::set<std::string> CalculateIsotopicPurities::getInputs() const
  {
    return { };
  }

  std::set<std::string> CalculateIsotopicPurities::getOutputs() const
  {
    return { "Features" };
  }

  std::vector<std::string> CalculateIsotopicPurities::getRequirements() const
  {
    return { "sequence", "traML" };
  }

  ParameterSet CalculateIsotopicPurities::getParameterSchema() const
  {
    std::map<std::string, std::vector<std::map<std::string, std::string>>> param_struct({
    {"CalculateIsotopicPurities", {
      {
        {"name", "isotopic_purity_values"},
        {"type", "string"},
        {"value", ""},
        {"description", "The isotropic purity values"},
      },
      {
        {"name", "isotopic_purity_name"},
        {"type", "list"},
        {"value", "[]"},
        {"description", "The isotropic purity names"},
      }
    }} });
    return ParameterSet(param_struct);
  }

  void CalculateIsotopicPurities::process(
    RawDataHandler& rawDataHandler_IO,
    const ParameterSet& params_I,
    Filenames& filenames_I
  ) const
  {
    LOGD << "START calculateIsotopicPurities";
    getFilenames(filenames_I);

    // Complete user parameters with schema
    ParameterSet params(params_I);
    params.merge(getParameterSchema());

    OpenMS::IsotopeLabelingMDVs isotopelabelingmdvs;
    OpenMS::Param parameters = isotopelabelingmdvs.getParameters();
    isotopelabelingmdvs.setParameters(parameters);

    OpenMS::FeatureMap normalized_featureMap;
    auto& CalculateIsotopicPurities_params = params.at("CalculateIsotopicPurities");
      
    std::vector<std::string> isotopic_purity_names;
    std::vector<std::vector<double>> experiment_data_mat;
    for (auto& param : CalculateIsotopicPurities_params)
    {
      if (param.getName() == "isotopic_purity_values" && !param.getValueAsString().empty())
      {
        std::string experiment_data_s;
        std::vector<double> experiment_data;
        experiment_data_s = param.getValueAsString();
        std::regex regex_double("[+-]?\\d+(?:\\.\\d+)?");
        size_t num_lists = std::count(experiment_data_s.begin(), experiment_data_s.end(), '[') == std::count(experiment_data_s.begin(), experiment_data_s.end(), ']') ? std::count(experiment_data_s.begin(), experiment_data_s.end(), '[') : 0;
        experiment_data_mat.resize(num_lists);
        std::sregex_iterator values_begin = std::sregex_iterator(experiment_data_s.begin(), experiment_data_s.end(), regex_double);
        std::sregex_iterator values_end = std::sregex_iterator();
        auto num_values = std::distance(values_begin , values_end);
        for (std::sregex_iterator it = values_begin; it != values_end; ++it)
          experiment_data.push_back(std::stod(it->str()));
          
        size_t element_idx = 0;
        size_t list_idx = 0;
        do
        {
          do
          {
            experiment_data_mat.at(list_idx).push_back(experiment_data.at(element_idx));
            element_idx++;
                
            if (element_idx > 0 && element_idx % (num_values / num_lists) == 0) list_idx++;
          }
          while ((element_idx > 0 && element_idx % (num_values / num_lists) != 0));
        }
        while (list_idx < num_lists);
      }
      if (param.getName() == "isotopic_purity_name" && !param.getValueAsString().empty())
      {
        std::string isotopic_purity_name_s;
        isotopic_purity_name_s = param.getValueAsString();
        std::regex regex_string_list("[^\"\',\[]+(?=')");
        std::sregex_iterator names_begin = std::sregex_iterator(isotopic_purity_name_s.begin(), isotopic_purity_name_s.end(), regex_string_list);
        for (std::sregex_iterator it = names_begin; it != std::sregex_iterator(); ++it)
          isotopic_purity_names.push_back(it->str());
      }
    }
    normalized_featureMap = rawDataHandler_IO.getFeatureMap();
    isotopelabelingmdvs.calculateIsotopicPurities(normalized_featureMap, experiment_data_mat, isotopic_purity_names);
    rawDataHandler_IO.setFeatureMap(normalized_featureMap);
    rawDataHandler_IO.updateFeatureMapHistory();

    LOGD << "END CalculateIsotopicPurities";
  }

}
