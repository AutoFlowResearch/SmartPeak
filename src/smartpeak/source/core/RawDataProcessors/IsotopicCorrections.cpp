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
#include <SmartPeak/core/RawDataProcessors/IsotopicCorrections.h>
#include <SmartPeak/core/Filenames.h>
#include <SmartPeak/core/Utilities.h>
#include <SmartPeak/core/FeatureFiltersUtils.h>

#include <OpenMS/ANALYSIS/QUANTITATION/IsotopeLabelingMDVs.h>

#include <plog/Log.h>

#include <algorithm>
#include <exception>


namespace SmartPeak
{
  std::set<std::string> IsotopicCorrections::getInputs() const
  {
    return { "Features" };
  }

  std::set<std::string> IsotopicCorrections::getOutputs() const
  {
    return { "Features" };
  }

  std::vector<std::string> IsotopicCorrections::getRequirements() const
  {
    return { "sequence", "traML" };
  }

  ParameterSet IsotopicCorrections::getParameterSchema() const
  {
    std::map<std::string, std::vector<std::map<std::string, std::string>>> param_struct({
    {"IsotopicCorrections", {
      {
        {"name", "correction_matrix_agent"},
        {"type", "string"},
        {"value", "TBDMS"},
        {"description", "The correction matrix corresponding to the derivatization agent used when processing the samples for LC-MS/MS or GC-MS"},
        {"valid_strings", "['TBDMS']"} // only "TBDMS" is supported for now.
      }
    }} });
    return ParameterSet(param_struct);
  }

  void IsotopicCorrections::process(
    RawDataHandler& rawDataHandler_IO,
    const ParameterSet& params_I,
    Filenames& filenames_I
  ) const
  {
    LOGD << "START IsotopicCorrections";
    getFilenames(filenames_I);

    // Complete user parameters with schema
    ParameterSet params(params_I);
    params.merge(getParameterSchema());

    OpenMS::IsotopeLabelingMDVs isotopelabelingmdvs;
    OpenMS::Param parameters = isotopelabelingmdvs.getParameters();
    isotopelabelingmdvs.setParameters(parameters);

    OpenMS::FeatureMap corrected_featureMap;
    auto& IsotopicCorrections_params = params.at("IsotopicCorrections");
      
    OpenMS::IsotopeLabelingMDVs::DerivatizationAgent correction_matrix_agent;
    for (auto& param : IsotopicCorrections_params)
    {
      if (param.getName() == "correction_matrix_agent")
      {
        if (param.getValueAsString() == "TBDMS")
        {
          correction_matrix_agent = OpenMS::IsotopeLabelingMDVs::DerivatizationAgent::TBDMS;
        }
      }
    }
      
    isotopelabelingmdvs.isotopicCorrections(rawDataHandler_IO.getFeatureMap(), corrected_featureMap, {}, correction_matrix_agent);
    rawDataHandler_IO.setFeatureMap(corrected_featureMap);
    rawDataHandler_IO.updateFeatureMapHistory();

    LOGD << "END IsotopicCorrections";
  }

}
