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
#include <SmartPeak/core/RawDataProcessors/SearchAccurateMass.h>
#include <SmartPeak/core/Filenames.h>
#include <SmartPeak/core/Utilities.h>
#include <SmartPeak/core/FeatureFiltersUtils.h>
#include <SmartPeak/io/InputDataValidation.h>

#include <OpenMS/ANALYSIS/ID/AccurateMassSearchEngine.h>

#include <plog/Log.h>

#include <algorithm>
#include <exception>

namespace SmartPeak
{

  std::set<std::string> SearchAccurateMass::getInputs() const
  {
    return { "Features" };
  }

  std::set<std::string> SearchAccurateMass::getOutputs() const
  {
    return { "Features", "Mz Tab"};
  }

  std::vector<std::string> SearchAccurateMass::getRequirements() const
  {
    return { "sequence", "traML" };
  }

  ParameterSet SearchAccurateMass::getParameterSchema() const
  {
    OpenMS::AccurateMassSearchEngine oms_params;
    return ParameterSet({ oms_params });
  }

  void SearchAccurateMass::process(RawDataHandler& rawDataHandler_IO,
    const ParameterSet& params_I,
    Filenames& filenames_I
  ) const
  {
    LOGD << "START SearchAccurateMass";
    getFilenames(filenames_I);
    LOGI << "SearchAccurateMass input size: " << rawDataHandler_IO.getFeatureMap().size();

    // Complete user parameters with schema
    ParameterSet params(params_I);
    params.merge(getParameterSchema());
    std::filesystem::path main_path(filenames_I.getTagValue(Filenames::Tag::MAIN_DIR));
    Utilities::prepareFileParameterList(params, "AccurateMassSearchEngine", "db:mapping", main_path);
    Utilities::prepareFileParameterList(params, "AccurateMassSearchEngine", "db:struct", main_path);
    Utilities::prepareFileParameter(params, "AccurateMassSearchEngine", "positive_adducts", main_path);
    Utilities::prepareFileParameter(params, "AccurateMassSearchEngine", "negative_adducts", main_path);

    OpenMS::AccurateMassSearchEngine ams;
    Utilities::setUserParameters(ams, params);

    // Run the accurate mass search engine
    OpenMS::MzTab output;
    ams.init();
    ams.run(rawDataHandler_IO.getFeatureMap(), output);

    // Remake the feature map replacing the peptide hits as features/sub-features
    OpenMS::FeatureMap fmap;
    for (const OpenMS::Feature& feature : rawDataHandler_IO.getFeatureMap()) {
      for (const auto& ident : feature.getPeptideIdentifications()) {
        for (const auto& hit : ident.getHits()) {
          OpenMS::Feature f;
          OpenMS::Feature s = feature;
          f.setUniqueId();
          f.setMetaValue("PeptideRef", hit.getMetaValue("identifier").toStringList().at(0));
          s.setUniqueId();
          s.setMetaValue("PeptideRef", hit.getMetaValue("identifier").toStringList().at(0));
          std::string native_id = hit.getMetaValue("chemical_formula").toString() + ";" + hit.getMetaValue("modifications").toString();
          s.setMetaValue("native_id", native_id);
          s.setMetaValue("identifier", hit.getMetaValue("identifier"));
          s.setMetaValue("description", hit.getMetaValue("description"));
          s.setMetaValue("modifications", hit.getMetaValue("modifications"));
          std::string adducts;
          try {
            std::string str = hit.getMetaValue("modifications").toString();
            std::string delimiter = ";";
            adducts = str.substr(1, str.find(delimiter) - 1);
          }
          catch (const std::exception& e) {
            LOGE << e.what();
          }
          s.setMetaValue("adducts", adducts);
          OpenMS::EmpiricalFormula chemform(hit.getMetaValue("chemical_formula").toString());
          double adduct_mass = s.getMZ() * std::abs(hit.getCharge()) + static_cast<double>(hit.getMetaValue("mz_error_Da")) - chemform.getMonoWeight();
          s.setMetaValue("dc_charge_adduct_mass", adduct_mass);
          s.setMetaValue("chemical_formula", hit.getMetaValue("chemical_formula"));
          s.setMetaValue("mz_error_ppm", hit.getMetaValue("mz_error_ppm"));
          s.setMetaValue("mz_error_Da", hit.getMetaValue("mz_error_Da"));
          s.setCharge(hit.getCharge());
          std::vector<OpenMS::Feature> subs = { s };
          f.setSubordinates(subs);
          fmap.push_back(f);
        }
      }
    }
    rawDataHandler_IO.setFeatureMap(fmap);
    rawDataHandler_IO.setMzTab(output);
    rawDataHandler_IO.updateFeatureMapHistory();

    LOGI << "SearchAccurateMass output size: " << rawDataHandler_IO.getFeatureMap().size();
    LOGD << "END SearchAccurateMass";
  }
}
