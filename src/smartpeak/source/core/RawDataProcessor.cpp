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

#include <SmartPeak/core/RawDataProcessor.h>
#include <SmartPeak/core/Filenames.h>
#include <SmartPeak/core/Utilities.h>
#include <SmartPeak/core/ApplicationHandler.h>
#include <SmartPeak/core/FeatureFiltersUtils.h>

// load/store raw data
#include <OpenMS/FORMAT/FileTypes.h>
#include <OpenMS/FORMAT/ChromeleonFile.h>
#include <OpenMS/FORMAT/TraMLFile.h>  // load traML as well
#include <OpenMS/FORMAT/FileHandler.h>
#include <OpenMS/FORMAT/MzMLFile.h>
#include <OpenMS/FORMAT/MzTabFile.h>
#include <OpenMS/ANALYSIS/OPENSWATH/ChromatogramExtractor.h>
#include <OpenMS/ANALYSIS/OPENSWATH/TargetedSpectraExtractor.h>
#include <OpenMS/ANALYSIS/QUANTITATION/IsotopeLabelingMDVs.h>
#include <OpenMS/ANALYSIS/TARGETED/MRMMapping.h>
#include <OpenMS/KERNEL/SpectrumHelper.h>

#include <OpenMS/FORMAT/MRMFeatureQCFile.h>  // load featureFilter and featureQC
#include <OpenMS/ANALYSIS/OPENSWATH/TransitionTSVFile.h>  // load traML
#include <OpenMS/FORMAT/FeatureXMLFile.h>  // load/store featureXML
#include <SmartPeak/io/InputDataValidation.h> // check filenames and headers

// load validation data and parameters
#include <SmartPeak/io/ParametersParser.h>
#ifndef CSV_IO_NO_THREAD
#define CSV_IO_NO_THREAD
#endif
#include <SmartPeak/io/csv.h>
#include <plog/Log.h>

// feature selection
#include <OpenMS/ANALYSIS/OPENSWATH/MRMFeatureSelector.h>
#include <OpenMS/ANALYSIS/OPENSWATH/MRMBatchFeatureSelector.h>
#include <OpenMS/ANALYSIS/OPENSWATH/PeakIntegrator.h>

#include <SmartPeak/algorithm/MRMFeatureValidator.h>  // feature validaiton
#include <OpenMS/ANALYSIS/OPENSWATH/MRMFeatureFilter.h>  // feature filter/QC
#include <OpenMS/ANALYSIS/OPENSWATH/MRMFeatureFinderScoring.h>  // feature picker
#include <OpenMS/ANALYSIS/QUANTITATION/AbsoluteQuantitation.h> // feature quantification
#include <OpenMS/MATH/MISC/EmgGradientDescent.h>

// FIA-MS
#include <OpenMS/ANALYSIS/OPENSWATH/SpectrumAddition.h> // MergeSpectra
#include <OpenMS/FILTERING/SMOOTHING/SavitzkyGolayFilter.h> // PickMS1Features
#include <OpenMS/TRANSFORMATIONS/RAW2PEAK/PeakPickerHiRes.h> // PickMS1Features
#include <OpenMS/FILTERING/NOISEESTIMATION/SignalToNoiseEstimatorMedianRapid.h> // PickMS1Features
#include <OpenMS/ANALYSIS/ID/AccurateMassSearchEngine.h>

// PickMS2Features
#include <OpenMS/FILTERING/DATAREDUCTION/MassTraceDetection.h>
#include <OpenMS/FILTERING/DATAREDUCTION/ElutionPeakDetection.h>
#include <OpenMS/FILTERING/DATAREDUCTION/FeatureFindingMetabo.h>

#include <algorithm>
#include <exception>

namespace SmartPeak
{

  ParameterSet LoadRawData::getParameterSchema() const
  {
    std::map<std::string, std::vector<std::map<std::string, std::string>>> param_struct({
    {"mzML", {
      {
        {"name", "format"},
        {"type", "string"},
        {"value", "XML"},
        {"description", "XML files are those with the ending .mzML or .mzXML. Chromeleon files are text files generating from the Thermo family of HPLCs using the Chromeleon software."},
        {"valid_strings", "['ChromeleonFile','XML']"}
      },
      {
        {"name", "zero_baseline"},
        {"type", "bool"},
        {"value", "false"},
        {"description", "Zeros the baseline of the chromatogram by adjusting all points so that the minimum point is 0."},
      },
    }},
    {"ChromatogramExtractor", {
      {
        {"name", "extract_window"},
        {"type", "float"},
        {"value", "0.5"},
        {"description", "Extract window."},
      },
      {
        {"name", "ppm"},
        {"type", "bool"},
        {"value", "false"},
        {"description", "ppm"},
      },
      {
        {"name", "rt_extraction_window"},
        {"type", "float"},
        {"value", "-1"},
        {"description", "rt extraction window"},
      },
      {
        {"name", "filter"},
        {"type", "string"},
        {"value", "tophat"},
        {"description", "Filter"},
      },
      {
        {"name", "extract_precursors"},
        {"type", "bool"},
        {"value", "true"},
        {"description", "Extract precursors"},
      },
    }},
    });
    return ParameterSet(param_struct);
  }

  void LoadRawData::getFilenames(Filenames& filenames) const
  {
    filenames.addFileName("mzML_i", "${MZML_INPUT_PATH}/${INPUT_MZML_FILENAME}.mzML");
  };

  void LoadRawData::process(
    RawDataHandler& rawDataHandler_IO,
    const ParameterSet& params_I,
    Filenames& filenames_I
  ) const
  {
    LOGD << "START loadMSExperiment";
    getFilenames(filenames_I);

    // Note: unlike other processors,
    // we don't want to complete user parameters with schema
    // as mZML parameter, if empty (not user defined), will means to use default behavior.
    // same for ChromatogramExtractor parameter

    // # load chromatograms
    OpenMS::MSExperiment chromatograms;
    if (!filenames_I.getFullPath("mzML_i").empty()) {
      if (params_I.at("mzML").size()) {
        // # convert parameters
        std::map<std::string, CastValue> mzML_params;
        for (auto& param : params_I.at("mzML")) {
          CastValue c;
          Utilities::castString(param.getValueAsString(), param.getType(), c);
          mzML_params.emplace(param.getName(), c);
        }
        // Deal with ChromeleonFile format
        if (mzML_params.count("format") && mzML_params.at("format").s_ == "ChromeleonFile") {
          const size_t pos = filenames_I.getFullPath("mzML_i").generic_string().rfind(".");
          std::string txt_name = filenames_I.getFullPath("mzML_i").generic_string();
          if (pos != std::string::npos) {
            txt_name.replace(txt_name.cbegin() + pos + 1, txt_name.cend(), "txt"); // replace extension
          }
          OpenMS::ChromeleonFile chfh;
          LOGI << "Loading: " << txt_name;
          chfh.load(txt_name, chromatograms);
          // If the peak height is less than 1.0 (which is quite common in RI and UV detection), 
          // the peak will not be picked, so we artificially scale the data by 1e3
          for (auto& peak : chromatograms.getChromatograms().at(0)) {
            peak.setIntensity(peak.getIntensity() * 1e3);
          }
        }
        // Deal with .mzXML format
        else if (mzML_params.count("format") && mzML_params.at("format").s_ == "XML") 
        {
          const size_t pos = filenames_I.getFullPath("mzML_i").generic_string().rfind(".");
          std::string txt_name = filenames_I.getFullPath("mzML_i").generic_string();
          if (pos != std::string::npos) {
            txt_name.replace(txt_name.cbegin() + pos + 1, txt_name.cend(), "xml"); // replace extension
          }
          OpenMS::FileHandler fh;
          LOGI << "Loading: " << txt_name;
          fh.loadExperiment(txt_name, chromatograms, OpenMS::FileTypes::MZXML);
        }
        else 
        {
          OpenMS::FileHandler fh;
          LOGI << "Loading: " << filenames_I.getFullPath("mzML_i").generic_string();
          fh.loadExperiment(filenames_I.getFullPath("mzML_i").generic_string(), chromatograms);
        }
      }
      else 
      {
        OpenMS::FileHandler fh;
        try {
          LOGI << "Loading: " << filenames_I.getFullPath("mzML_i").generic_string();
          fh.loadExperiment(filenames_I.getFullPath("mzML_i").generic_string(), chromatograms);
        } catch (const std::exception& e) {
          LOGE << "LoadRawData::process : " << typeid(e).name() << " : " << e.what();
        }
      }
    }

    OpenMS::TargetedExperiment& targeted_exp = rawDataHandler_IO.getTargetedExperiment();
    if (params_I.at("ChromatogramExtractor").size()) {
      // # convert parameters
      std::map<std::string, CastValue> chromatogramExtractor_params;
      for (auto& param : params_I.at("ChromatogramExtractor")) {
        CastValue c;
        Utilities::castString(param.getValueAsString(), param.getType(), c);
        chromatogramExtractor_params.emplace(param.getName(), c);
      }
      // # exctract chromatograms
      OpenMS::MSExperiment chromatograms_copy = chromatograms;
      chromatograms.clear(true);
      if (chromatogramExtractor_params.count("extract_precursors")) {
        const std::vector<OpenMS::ReactionMonitoringTransition>& tr_const = targeted_exp.getTransitions();
        std::vector<OpenMS::ReactionMonitoringTransition> tr = tr_const;
        for (OpenMS::ReactionMonitoringTransition& t : tr) {
          t.setProductMZ(t.getPrecursorMZ());
        }
        targeted_exp.setTransitions(tr);
        rawDataHandler_IO.setTargetedExperiment(targeted_exp);
      }
      OpenMS::TransformationDescription transfDescr;
      OpenMS::ChromatogramExtractor chromatogramExtractor;
      chromatogramExtractor.extractChromatograms(
        chromatograms_copy,
        chromatograms,
        rawDataHandler_IO.getTargetedExperiment(),
        chromatogramExtractor_params.at("extract_window").f_,
        chromatogramExtractor_params.at("ppm").b_,
        transfDescr,
        chromatogramExtractor_params.at("rt_extraction_window").f_,
        chromatogramExtractor_params.at("filter").s_
      );
    }
    rawDataHandler_IO.setExperiment(chromatograms);

    LOGD << "END loadMSExperiment";
  }

  void LoadRawData::extractMetaData(
    RawDataHandler& rawDataHandler_IO
  )
  {
    LOGD << "START extractMetaData";

    std::string filename;
    std::string samplename;

    const std::string loaded_file_path = rawDataHandler_IO.getExperiment().getLoadedFilePath();

    if (loaded_file_path.size()) {
      const std::string prefix{ "file://" };
      filename = !loaded_file_path.find(prefix) ? loaded_file_path.substr(prefix.size()) : loaded_file_path;
    }

    OpenMS::DataValue dv_mzml_id = rawDataHandler_IO.getExperiment().getMetaValue("mzml_id");

    if (!dv_mzml_id.isEmpty() && dv_mzml_id.toString().size()) {
      samplename = dv_mzml_id.toString();
      const size_t pos = samplename.find('-');
      if (pos != std::string::npos)
        samplename = samplename.substr(pos + 1);
    }
    else 
    {
      samplename = filename;
    }

    const OpenMS::MSExperiment& chromatograms = rawDataHandler_IO.getExperiment();

    MetaDataHandler& metaDataHandler = rawDataHandler_IO.getMetaData();

    if (metaDataHandler.getSampleName().empty())
      metaDataHandler.setSampleName(samplename);

    if (metaDataHandler.getFilename().empty())
      metaDataHandler.setFilename(filename);

    if (metaDataHandler.proc_method_name.empty())
      metaDataHandler.proc_method_name = chromatograms.getInstrument().getSoftware().getName();

    if (metaDataHandler.instrument.empty())
      metaDataHandler.instrument = chromatograms.getInstrument().getName();

    if (metaDataHandler.operator_name.empty() && chromatograms.getContacts().size())
      metaDataHandler.operator_name = chromatograms.getContacts()[0].getLastName() + " " + chromatograms.getContacts()[0].getFirstName();

    if (metaDataHandler.acquisition_date_and_time.tm_year == 0) {
      // some noise because OpenMS uses uint and the standard library uses int (for time structure's members)
      struct { OpenMS::UInt tm_mon, tm_mday, tm_year, tm_hour, tm_min, tm_sec; } dt_uint;
      rawDataHandler_IO.getExperiment().getDateTime().get(
        dt_uint.tm_mon,
        dt_uint.tm_mday,
        dt_uint.tm_year,
        dt_uint.tm_hour,
        dt_uint.tm_min,
        dt_uint.tm_sec
      );
      std::tm& dt_int = metaDataHandler.acquisition_date_and_time;
      dt_int.tm_year = dt_uint.tm_year;
      dt_int.tm_mon = dt_uint.tm_mon;
      dt_int.tm_mday = dt_uint.tm_mday;
      dt_int.tm_hour = dt_uint.tm_hour;
      dt_int.tm_min = dt_uint.tm_min;
      dt_int.tm_sec = dt_uint.tm_sec;
    }
    LOGD << "END extractMetaData";
  }

  void StoreRawData::getFilenames(Filenames& filenames) const
  {
    filenames.addFileName("mzML_i", "${MZML_INPUT_PATH}/${INPUT_MZML_FILENAME}.mzML");
  };

  void StoreRawData::process(
    RawDataHandler& rawDataHandler_IO,
    const ParameterSet& params_I,
    Filenames& filenames_I
  ) const
  {
    LOGD << "START storeMzML";
    getFilenames(filenames_I);

    if (!InputDataValidation::prepareToStore(filenames_I, "mzML_i"))
    {
      LOGD << "END " << getName();
      return;
    }

    try {
      OpenMS::MzMLFile mzmlfile;
      if (rawDataHandler_IO.getChromatogramMap().size()) {
        mzmlfile.store(filenames_I.getFullPath("mzML_i").generic_string(), rawDataHandler_IO.getChromatogramMap());
      }
      else 
      {
        mzmlfile.store(filenames_I.getFullPath("mzML_i").generic_string(), rawDataHandler_IO.getExperiment());
      }
      filenames_I.setSavedState("mzML_i", true);
    }
    catch (const std::exception& e) {
      LOGE << e.what();
    }

    LOGD << "END storeMzML";
  }

  void LoadFeatures::getFilenames(Filenames& filenames) const
  {
    filenames.addFileName("featureXML_i", "${FEATURES_INPUT_PATH}/${INPUT_INJECTION_NAME}.featureXML");
  };

  void LoadFeatures::process(
    RawDataHandler& rawDataHandler_IO,
    const ParameterSet& params_I,
    Filenames& filenames_I
  ) const
  {
    LOGD << "START LoadFeatures";
    getFilenames(filenames_I);

    if (!InputDataValidation::prepareToLoad(filenames_I, "featureXML_i"))
    {
      LOGD << "END " << getName();
      return;
    }

    try {
      OpenMS::FeatureXMLFile featurexml;
      featurexml.load(filenames_I.getFullPath("featureXML_i").generic_string(), rawDataHandler_IO.getFeatureMapHistory());
      // NOTE: setPrimaryMSRunPath() is needed for calculate_calibration
      rawDataHandler_IO.getFeatureMapHistory().setPrimaryMSRunPath({ rawDataHandler_IO.getMetaData().getFilename() });
      rawDataHandler_IO.makeFeatureMapFromHistory();
      rawDataHandler_IO.getFeatureMap().setPrimaryMSRunPath({ rawDataHandler_IO.getMetaData().getFilename() });
    }
    catch (const std::exception& e) {
      LOGE << e.what();
      rawDataHandler_IO.getFeatureMapHistory().clear();
      rawDataHandler_IO.getFeatureMap().clear();
      LOGE << "feature map clear";
    }

    LOGD << "END LoadFeatures";
  }

  void StoreFeatures::getFilenames(Filenames& filenames) const
  {
    filenames.addFileName("featureXML_o", "${FEATURES_OUTPUT_PATH}/${OUTPUT_INJECTION_NAME}.featureXML");
  };

  void StoreFeatures::process(
    RawDataHandler& rawDataHandler_IO,
    const ParameterSet& params_I,
    Filenames& filenames_I
  ) const
  {
    LOGD << "START storeFeatureMap";
    getFilenames(filenames_I);

    // We want to overwrite the file in that case
    filenames_I.setSavedState("featureXML_o", false);

    if (!InputDataValidation::prepareToStore(filenames_I, "featureXML_o"))
    {
      LOGD << "END " << getName();
      return;
    }

    try {
      // Store outfile as featureXML
      OpenMS::FeatureXMLFile featurexml;
      featurexml.store(filenames_I.getFullPath("featureXML_o").generic_string(), rawDataHandler_IO.getFeatureMapHistory());
      filenames_I.setSavedState("featureXML_o", true);
    }
    catch (const std::exception& e) {
      LOGE << e.what();
    }

    LOGD << "END storeFeatureMap";
  }

  void LoadAnnotations::getFilenames(Filenames& filenames) const
  {
    filenames.addFileName("mzTab_i", "${FEATURES_INPUT_PATH}/${INPUT_INJECTION_NAME}.mzTab");
  };

  void LoadAnnotations::process(
    RawDataHandler& rawDataHandler_IO,
    const ParameterSet& params_I,
    Filenames& filenames_I
  ) const
  {
    LOGD << "START LoadAnnotations";
    getFilenames(filenames_I);

    if (!InputDataValidation::prepareToLoad(filenames_I, "mzTab_i"))
    {
      LOGD << "END " << getName();
      return;
    }

    try {
      OpenMS::MzTabFile mztabfile;
      mztabfile.load(filenames_I.getFullPath("mzTab_i").generic_string(), rawDataHandler_IO.getMzTab());
      rawDataHandler_IO.updateFeatureMapHistory();
    }
    catch (const std::exception& e) {
      LOGE << e.what();
      rawDataHandler_IO.setMzTab(OpenMS::MzTab());
      LOGE << "feature map clear";
    }

    LOGD << "END LoadAnnotations";
  }

  void StoreAnnotations::getFilenames(Filenames& filenames) const
  {
    filenames.addFileName("mzTab_o", "${FEATURES_OUTPUT_PATH}/${OUTPUT_INJECTION_NAME}.mzTab");
  };

  void StoreAnnotations::process(
    RawDataHandler& rawDataHandler_IO,
    const ParameterSet& params_I,
    Filenames& filenames_I
  ) const
  {
    LOGD << "START StoreAnnotations";
    getFilenames(filenames_I);

    if (!InputDataValidation::prepareToStore(filenames_I, "mzTab_o"))
    {
      LOGD << "END " << getName();
      return;
    }

    try {
      // Store outfile as mzTab
      OpenMS::MzTabFile mztabfile;
      mztabfile.store(filenames_I.getFullPath("mzTab_o").generic_string(), rawDataHandler_IO.getMzTab());
      filenames_I.setSavedState("mzTab_o", true);
    }
    catch (const std::exception& e) {
      LOGE << e.what();
    }

    LOGD << "END StoreAnnotations";
  }

  ParameterSet PickMRMFeatures::getParameterSchema() const
  {
    OpenMS::MRMFeatureFinderScoring oms_params;
    return ParameterSet({ oms_params });
  }

  void PickMRMFeatures::process(
    RawDataHandler& rawDataHandler_IO,
    const ParameterSet& params_I,
    Filenames& filenames_I
  ) const
  {
    LOGD << "START PickMRMFeatures";
    getFilenames(filenames_I);
    OpenMS::MRMFeatureFinderScoring featureFinder;
    Utilities::setUserParameters(featureFinder, params_I);

    OpenMS::FeatureMap featureMap;

    try {
      featureFinder.pickExperiment(
        rawDataHandler_IO.getChromatogramMap(),
        featureMap,
        rawDataHandler_IO.getTargetedExperiment(),
        rawDataHandler_IO.getTransformationDescription(),
        rawDataHandler_IO.getSWATH()
      );
    }
    catch (const std::exception& e) {
      LOGE << e.what();
    }

    // NOTE: setPrimaryMSRunPath() is needed for calculate_calibration
    featureMap.setPrimaryMSRunPath({rawDataHandler_IO.getMetaData().getFilename()});

    LOGD << "setPrimaryMSRunPath: " << rawDataHandler_IO.getMetaData().getFilename();

    rawDataHandler_IO.setFeatureMap(featureMap);
    rawDataHandler_IO.updateFeatureMapHistory();

    LOGI << "Feature Picker output size: " << featureMap.size();
    LOGD << "END PickMRMFeatures";
  }

  ParameterSet FilterFeatures::getParameterSchema() const
  {
    OpenMS::MRMFeatureFilter oms_param;
    return ParameterSet({ oms_param });
  }

  void FilterFeatures::process(
    RawDataHandler& rawDataHandler_IO,
    const ParameterSet& params_I,
    Filenames& filenames_I
  ) const
  {
    LOGD << "START filterFeatures";
    getFilenames(filenames_I);
    LOGI << "Feature Filter input size: " << rawDataHandler_IO.getFeatureMap().size();

    OpenMS::MRMFeatureFilter featureFilter;
    Utilities::setUserParameters(featureFilter, params_I, "MRMFeatureFilter.filter_MRMFeatures");

    OpenMS::FeatureMap& featureMap = rawDataHandler_IO.getFeatureMap();

    featureFilter.FilterFeatureMap(
      featureMap,
      rawDataHandler_IO.getFeatureFilter(),
      rawDataHandler_IO.getTargetedExperiment()
    );

    rawDataHandler_IO.updateFeatureMapHistory();

    LOGI << "Feature Filter output size: " << featureMap.size();
    LOGD << "END filterFeatures";
  }

  ParameterSet CheckFeatures::getParameterSchema() const
  {
    OpenMS::MRMFeatureFilter oms_param;
    return ParameterSet({ oms_param });
  }

  void CheckFeatures::process(
    RawDataHandler& rawDataHandler_IO,
    const ParameterSet& params_I,
    Filenames& filenames_I
  ) const
  {
    LOGD << "START checkFeatures";
    getFilenames(filenames_I);
    LOGI << "Feature Checker input size: " << rawDataHandler_IO.getFeatureMap().size();

    OpenMS::MRMFeatureFilter featureFilter;
    Utilities::setUserParameters(featureFilter, params_I, "MRMFeatureFilter.filter_MRMFeatures.qc");

    featureFilter.FilterFeatureMap(
      rawDataHandler_IO.getFeatureMap(),
      rawDataHandler_IO.getFeatureQC(),
      rawDataHandler_IO.getTargetedExperiment()
    );

    rawDataHandler_IO.updateFeatureMapHistory();

    LOGI << "Feature Checker output size: " << rawDataHandler_IO.getFeatureMap().size();
    LOGD << "END checkFeatures";
  }

  void SelectFeatures::process(
    RawDataHandler& rawDataHandler_IO,
    const ParameterSet& params_I,
    Filenames& filenames_I
  ) const
  {
    LOGD << "START selectFeatures";
    getFilenames(filenames_I);
    LOGI << "selectFeatures input size: " << rawDataHandler_IO.getFeatureMap().size();

    const bool qmip_params_passed_but_empty =
      params_I.count("MRMFeatureSelector.schedule_MRMFeatures_qmip") &&
      params_I.at("MRMFeatureSelector.schedule_MRMFeatures_qmip").empty();
    
    const bool score_params_passed_but_empty =
      params_I.count("MRMFeatureSelector.schedule_MRMFeatures_score") &&
      params_I.at("MRMFeatureSelector.schedule_MRMFeatures_score").empty();

    if (qmip_params_passed_but_empty || score_params_passed_but_empty) {
      LOGE << "Parameters missing for selectFeatures. Not selecting";
      LOGD << "END selectFeatures";
      return;
    }

    OpenMS::FeatureMap output;

    try {
      if (params_I.count("MRMFeatureSelector.schedule_MRMFeatures_qmip")) {
        LOGD << "Using MRMFeatures_qmip";
        std::vector<OpenMS::MRMFeatureSelector::SelectorParameters> p =
          Utilities::extractSelectorParameters(params_I.at("MRMFeatureSelector.schedule_MRMFeatures_qmip"), params_I.at("MRMFeatureSelector.select_MRMFeatures_qmip"));
        OpenMS::MRMBatchFeatureSelector::batchMRMFeaturesQMIP(rawDataHandler_IO.getFeatureMap(), output, p);
      }
      else if (params_I.count("MRMFeatureSelector.schedule_MRMFeatures_score")) 
      {
        LOGD << "Using MRMFeatures_score";
        std::vector<OpenMS::MRMFeatureSelector::SelectorParameters> p =
          Utilities::extractSelectorParameters(params_I.at("MRMFeatureSelector.schedule_MRMFeatures_score"), params_I.at("MRMFeatureSelector.select_MRMFeatures_score"));
        OpenMS::MRMBatchFeatureSelector::batchMRMFeaturesScore(rawDataHandler_IO.getFeatureMap(), output, p);
      }
      else 
      {
        LOGE << "Both arguments 'select params' and 'schedule params' are empty";
        LOGD << "END selectFeatures";
        // TODO: replace throw with return?
        throw std::invalid_argument("Both arguments 'select params' and 'schedule params' are empty.");
      }
    }
    catch (std::exception& e) {
      LOGE << e.what();
    }

    output.setPrimaryMSRunPath({rawDataHandler_IO.getMetaData().getFilename()});

    LOGV << "setPrimaryMSRunPath: " << rawDataHandler_IO.getMetaData().getFilename();

    rawDataHandler_IO.setFeatureMap(output);
    rawDataHandler_IO.updateFeatureMapHistory();

    LOGI << "selectFeatures output size: " << output.size();
    LOGD << "END selectFeatures";
  }

  ParameterSet ValidateFeatures::getParameterSchema() const
  {
    std::map<std::string, std::vector<std::map<std::string, std::string>>> param_struct({
    {"MRMFeatureValidator.validate_MRMFeatures", {
      {
        {"name", "Tr_window"},
        {"type", "float"},
        {"value", "1"},
        {"description", "retention time difference threshold"},
      },
    }},
    });
    return ParameterSet(param_struct);
  }

  void ValidateFeatures::process(
    RawDataHandler& rawDataHandler_IO,
    const ParameterSet& params_I,
    Filenames& filenames_I
  ) const
  {
    LOGD << "START validateFeatures";
    getFilenames(filenames_I);
    // Complete user parameters with schema
    ParameterSet params(params_I);
    params.merge(getParameterSchema());

    OpenMS::FeatureMap mapped_features;
    std::map<std::string, float> validation_metrics; // keys: accuracy, recall, precision

    MRMFeatureValidator::validate_MRMFeatures(
      rawDataHandler_IO.getReferenceData(),
      rawDataHandler_IO.getFeatureMap(),
      rawDataHandler_IO.getMetaData().getInjectionName(),
      mapped_features,
      validation_metrics,
      std::stof(params.at("MRMFeatureValidator.validate_MRMFeatures").front().getValueAsString())
      // TODO: While this probably works, it might be nice to add some check that the parameter passed is the desired one
    );

    rawDataHandler_IO.setFeatureMap(mapped_features);
    rawDataHandler_IO.setValidationMetrics(validation_metrics);

    LOGD << "END validateFeatures";
  }

  void PlotFeatures::process(
    RawDataHandler& rawDataHandler_IO,
    const ParameterSet& params_I,
    Filenames& filenames_I
  ) const
  {
    LOGD << "START PlotFeatures (NOT IMPLEMENTED)";
    getFilenames(filenames_I);
    // TODO: Uncomment once FeaturePlotter is ready

    // if (FeaturePlotter_params_I.empty() || filename.empty())
    //   return;

    // FeaturePlotter featurePlotter;
    // featurePlotter.setParameters(FeaturePlotter_params_I);
    // featurePlotter.plot_peaks(
    //   filename,
    //   rawDataHandler_IO.getTargetedExperiment(),
    //   rawDataHandler_IO.getChromatogramMap(),
    //   rawDataHandler_IO.getFeatureMap()
    // );
    LOGD << "END PlotFeatures (NOT IMPLEMENTED)";
  }

  std::vector<std::string> QuantifyFeatures::getRequirements() const
  {
    return { "quantitationMethods" };
  }

  ParameterSet QuantifyFeatures::getParameterSchema() const
  {
    OpenMS::AbsoluteQuantitation oms_params;
    return ParameterSet({ oms_params });
  }

  void QuantifyFeatures::process(
    RawDataHandler& rawDataHandler_IO,
    const ParameterSet& params_I,
    Filenames& filenames_I
  ) const
  {
    LOGD << "START quantifyComponents";
    getFilenames(filenames_I);
    LOGI << "Processing # quantitation methods: " << rawDataHandler_IO.getQuantitationMethods().size();

    try {
      OpenMS::AbsoluteQuantitation aq;
      aq.setQuantMethods(rawDataHandler_IO.getQuantitationMethods());
      aq.quantifyComponents(rawDataHandler_IO.getFeatureMap());
      rawDataHandler_IO.updateFeatureMapHistory();
    } catch (const std::exception& e) {
      LOGE << e.what();
    }

    LOGD << "END quantifyComponents";
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

  void LoadTransitions::process(
    RawDataHandler& rawDataHandler_IO,
    const ParameterSet& params_I,
    Filenames& filenames_I
  ) const
  {
    LOGD << "START loadTraML";
    getFilenames(filenames_I);
    // Complete user parameters with schema
    ParameterSet params(params_I);
    params.merge(getParameterSchema());

    if (!InputDataValidation::prepareToLoad(filenames_I, "traML"))
    {
      LOGD << "END " << getName();
      return;
    }

    const auto format_param = params.findParameter("LoadTransitions", "format");
    if (!format_param)
    {
      // should actually not happen since we merge with the default params
      LOGE << "LoadTransitions/format parameter not found";
      return;
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
    }

    LOGD << "END loadTraML";
  }

  void LoadFeatureFiltersRDP::getFilenames(Filenames& filenames) const
  {
    filenames.addFileName("featureFilterComponents", "${MAIN_DIR}/featureFilterComponents.csv", "Components Filters", true, true);
    filenames.addFileName("featureFilterComponentGroups", "${MAIN_DIR}/featureFilterComponentGroups.csv", "Components Group Filters", true, true);
  };

  void LoadFeatureFiltersRDP::process(
    RawDataHandler& rawDataHandler_IO,
    const ParameterSet& params_I,
    Filenames& filenames_I
  ) const
  {
    LOGD << "START loadFeatureFilter";
    getFilenames(filenames_I);
    FeatureFiltersUtils::loadFeatureFiltersFromDB(
      "featureFilterComponents",
      "featureFilterComponentGroups",
      filenames_I,
      rawDataHandler_IO.getFeatureFilter(),
      nullptr,
      nullptr
    );
    LOGD << "END loadFeatureFilter";
  }

  void LoadFeatureQCsRDP::getFilenames(Filenames& filenames) const
  {
    filenames.addFileName("featureQCComponents", "${MAIN_DIR}/featureQCComponents.csv", "Components QC", true, true);
    filenames.addFileName("featureQCComponentGroups", "${MAIN_DIR}/featureQCComponentGroups.csv", "Components Group QCs", true, true);
  };

  void LoadFeatureQCsRDP::process(
    RawDataHandler& rawDataHandler_IO,
    const ParameterSet& params_I,
    Filenames& filenames_I
  ) const
  {
    LOGD << "START loadFeatureQC";
    getFilenames(filenames_I);
    FeatureFiltersUtils::loadFeatureFiltersFromDB(
      "featureQCComponents", 
      "featureQCComponentGroups", 
      filenames_I,
      rawDataHandler_IO.getFeatureQC(),
      nullptr,
      nullptr
    );
    LOGD << "END loadFeatureQC";
  }

  bool StoreFeatureFiltersRDP::onFilePicked(const std::filesystem::path& filename, ApplicationHandler* application_handler)
  {
    if (application_handler->sequenceHandler_.getSequence().size() == 0)
    {
      LOGE << "File cannot be loaded without first loading the sequence.";
      return false;
    }
    RawDataHandler& rawDataHandler = application_handler->sequenceHandler_.getSequence().at(0).getRawData();
    Filenames filenames;
    if (component_group_)
    {
      filenames.setFullPath("featureFilterComponentGroups", filename);
    }
    else
    {
      filenames.setFullPath("featureFilterComponents", filename);
    }
    process(rawDataHandler, {}, filenames);
    return true;
  }

  void StoreFeatureFiltersRDP::getFilenames(Filenames& filenames) const
  {
    if (component_group_)
    {
      filenames.addFileName("featureFilterComponentGroups", "${MAIN_DIR}/featureFilterComponentGroups.csv", "Components Group Filters", true, true);
    }
    else
    {
      filenames.addFileName("featureFilterComponents", "${MAIN_DIR}/featureFilterComponents.csv", "Components Filters", true, true);
    }
  };

  void StoreFeatureFiltersRDP::process(
    RawDataHandler& rawDataHandler_IO,
    const ParameterSet& params_I,
    Filenames& filenames_I
  ) const
  {
    LOGD << "START storeFeatureFilter";

    getFilenames(filenames_I);
    FeatureFiltersUtils::storeFeatureFiltersInDB(
      "featureFilterComponents",
      "featureFilterComponentGroups",
      filenames_I,
      rawDataHandler_IO.getFeatureFilter());

    LOGD << "END storeFeatureFilter";
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
    if (component_group_)
    {
      filenames.setFullPath("featureQCComponentGroups", filename);
    }
    else
    {
      filenames.setFullPath("featureQCComponents", filename);
    }
    process(rawDataHandler, {}, filenames);
    return true;
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
    FeatureFiltersUtils::storeFeatureFiltersInDB(
      "featureQCComponents",
      "featureQCComponentGroups",
      filenames_I,
      rawDataHandler_IO.getFeatureQC());
    LOGD << "END storeFeatureQC";
  }

  void LoadValidationData::getFilenames(Filenames& filenames) const
  {
    filenames.addFileName("referenceData", "${MAIN_DIR}/referenceData.csv", "Reference data", true, true);
  };

  bool LoadValidationData::onFilePicked(const std::filesystem::path& filename, ApplicationHandler* application_handler)
  {
    if (application_handler->sequenceHandler_.getSequence().size() == 0)
    {
      LOGE << "File cannot be loaded without first loading the sequence.";
      return false;
    }
    RawDataHandler& rawDataHandler = application_handler->sequenceHandler_.getSequence().at(0).getRawData();
    Filenames filenames;
    filenames.setFullPath("referenceData", filename);
    process(rawDataHandler, {}, filenames);
    return true;
  }

  void LoadValidationData::process(
    RawDataHandler& rawDataHandler_IO,
    const ParameterSet& params_I,
    Filenames& filenames_I
  ) const
  {
    LOGD << "START loadValidationData";
    getFilenames(filenames_I);
    if (!InputDataValidation::prepareToLoad(filenames_I, "referenceData"))
    {
      LOGD << "END " << getName();
      return;
    }

    try
    {
      const std::string s_sample_index{ "sample_index" };
      const std::string s_original_filename{ "original_filename" };
      const std::string s_sample_name{ "sample_name" };
      const std::string s_sample_type{ "sample_type" };
      const std::string s_acquisition_date_and_time{ "acquisition_date_and_time" };
      const std::string s_acq_method_name{ "acq_method_name" };
      const std::string s_component_name{ "component_name" };
      const std::string s_component_group_name{ "component_group_name" };
      const std::string s_retention_time{ "retention_time" };
      const std::string s_start_time{ "start_time" };
      const std::string s_end_time{ "end_time" };
      const std::string s_used{ "used_" };
      const std::string s_calculated_concentration{ "calculated_concentration" };
      const std::string s_experiment_id{ "experiment_id" };
      const std::string s_acquisition_method_id{ "acquisition_method_id" };
      const std::string s_height{ "height" };
      const std::string s_area{ "area" };

      int sample_index;
      std::string original_filename;
      std::string sample_name;
      std::string sample_type;
      std::string acquisition_date_and_time;
      std::string acq_method_name;
      std::string component_name;
      std::string component_group_name;
      float retention_time;
      float start_time;
      float end_time;
      std::string used;
      float calculated_concentration;
      std::string experiment_id;
      std::string acquisition_method_id;
      float height;
      float area;
      std::vector<std::map<std::string, CastValue>>& reference_data = rawDataHandler_IO.getReferenceData();

      if (filenames_I.isEmbedded("referenceData"))
      {
        auto db_context = filenames_I.getSessionDB().beginRead(
          "referenceData",
          s_sample_index,
          s_original_filename,
          s_sample_name,
          s_sample_type,
          s_acquisition_date_and_time,
          s_acq_method_name,
          s_component_name,
          s_component_group_name,
          s_retention_time,
          s_start_time,
          s_end_time,
          s_used,
          s_calculated_concentration,
          s_experiment_id,
          s_acquisition_method_id,
          s_height,
          s_area
        );
        if (!db_context)
        {
          return;
        }
        while (filenames_I.getSessionDB().read(
          *db_context,
          sample_index,
          original_filename,
          sample_name,
          sample_type,
          acquisition_date_and_time,
          acq_method_name,
          component_name,
          component_group_name,
          retention_time,
          start_time,
          end_time,
          used,
          calculated_concentration,
          experiment_id,
          acquisition_method_id,
          height,
          area
        ))
        {
          std::map<std::string, CastValue> m;
          m.emplace(s_sample_index, sample_index);
          m.emplace(s_original_filename, original_filename);
          m.emplace(s_sample_name, sample_name);
          m.emplace(s_sample_type, sample_type);
          m.emplace(s_acquisition_date_and_time, acquisition_date_and_time);
          m.emplace(s_acq_method_name, acq_method_name);
          m.emplace(s_component_name, component_name);
          m.emplace(s_component_group_name, component_group_name);
          m.emplace(s_retention_time, retention_time);
          m.emplace(s_start_time, start_time);
          m.emplace(s_end_time, end_time);
          m.emplace(s_used, used);
          m.emplace(s_calculated_concentration, calculated_concentration);
          m.emplace(s_experiment_id, experiment_id);
          m.emplace(s_acquisition_method_id, acquisition_method_id);
          m.emplace(s_height, height);
          m.emplace(s_area, area);
          MetaDataHandler mdh;
          mdh.setSampleName(sample_name);
          mdh.inj_number = sample_index;
          mdh.batch_name = experiment_id;
          mdh.setAcquisitionDateAndTimeFromString(acquisition_date_and_time, "%m-%d-%Y %H:%M");
          m.emplace("injection_name", mdh.getInjectionName());
          reference_data.push_back(std::move(m));
        }
        filenames_I.getSessionDB().endRead(*db_context);
      }
      else
      {
        io::CSVReader<17, io::trim_chars<>, io::no_quote_escape<','>> in(filenames_I.getFullPath("referenceData").generic_string());
        in.read_header(
          io::ignore_extra_column,
          s_sample_index,
          s_original_filename,
          s_sample_name,
          s_sample_type,
          s_acquisition_date_and_time,
          s_acq_method_name,
          s_component_name,
          s_component_group_name,
          s_retention_time,
          s_start_time,
          s_end_time,
          s_used,
          s_calculated_concentration,
          s_experiment_id,
          s_acquisition_method_id,
          s_height,
          s_area
        );
        while (in.read_row(
          sample_index,
          original_filename,
          sample_name,
          sample_type,
          acquisition_date_and_time,
          acq_method_name,
          component_name,
          component_group_name,
          retention_time,
          start_time,
          end_time,
          used,
          calculated_concentration,
          experiment_id,
          acquisition_method_id,
          height,
          area
        )) {
          std::transform(used.begin(), used.end(), used.begin(), ::tolower);
          if (used == "false")
            continue;
          std::map<std::string, CastValue> m;
          m.emplace(s_sample_index, sample_index);
          m.emplace(s_original_filename, original_filename);
          m.emplace(s_sample_name, sample_name);
          m.emplace(s_sample_type, sample_type);
          m.emplace(s_acquisition_date_and_time, acquisition_date_and_time);
          m.emplace(s_acq_method_name, acq_method_name);
          m.emplace(s_component_name, component_name);
          m.emplace(s_component_group_name, component_group_name);
          m.emplace(s_retention_time, retention_time);
          m.emplace(s_start_time, start_time);
          m.emplace(s_end_time, end_time);
          m.emplace(s_used, used);
          m.emplace(s_calculated_concentration, calculated_concentration);
          m.emplace(s_experiment_id, experiment_id);
          m.emplace(s_acquisition_method_id, acquisition_method_id);
          m.emplace(s_height, height);
          m.emplace(s_area, area);
          MetaDataHandler mdh;
          mdh.setSampleName(sample_name);
          mdh.inj_number = sample_index;
          mdh.batch_name = experiment_id;
          mdh.setAcquisitionDateAndTimeFromString(acquisition_date_and_time, "%m-%d-%Y %H:%M");
          m.emplace("injection_name", mdh.getInjectionName());
          reference_data.push_back(std::move(m));
        }
      }
      rawDataHandler_IO.setReferenceData(reference_data);
    }
    catch (const std::exception& e) {
      LOGE << e.what();
      rawDataHandler_IO.getReferenceData().clear();
      LOGI << "RefereceData clear";
    }
    LOGD << "END loadValidationData";
  }

  void StoreValidationData::getFilenames(Filenames& filenames) const
  {
    filenames.addFileName("referenceData", "${MAIN_DIR}/referenceData.csv", "Reference data", true, true);
  };

  bool StoreValidationData::onFilePicked(const std::filesystem::path& filename, ApplicationHandler* application_handler)
  {
    if (application_handler->sequenceHandler_.getSequence().size() == 0)
    {
      LOGE << "File cannot be loaded without first loading the sequence.";
      return false;
    }
    RawDataHandler& rawDataHandler = application_handler->sequenceHandler_.getSequence().at(0).getRawData();
    Filenames filenames;
    filenames.setFullPath("referenceData", filename);
    process(rawDataHandler, {}, filenames);
    return true;
  }

  void StoreValidationData::process(
    RawDataHandler& rawDataHandler_IO,
    const ParameterSet& params_I,
    Filenames& filenames_I
  ) const
  {
    LOGD << "START StoreValidationData";

    try
    {
      if (!InputDataValidation::prepareToStore(filenames_I, "referenceData"))
      {
        LOGD << "END " << getName();
        return;
      }
      if (filenames_I.isEmbedded("referenceData"))
      {
        auto db_context = filenames_I.getSessionDB().beginWrite(
          "referenceData", 
          "sample_index", "INT",
          "original_filename", "STRING",
          "sample_name", "STRING",
          "sample_type", "STRING",
          "acquisition_date_and_time", "STRING",
          "acq_method_name", "STRING",
          "component_name", "STRING",
          "component_group_name", "STRING",
          "retention_time", "REAL",
          "start_time", "REAL",
          "end_time", "REAL",
          "used_", "STRING",
          "calculated_concentration", "REAL",
          "experiment_id", "STRING",
          "acquisition_method_id", "STRING",
          "height", "REAL",
          "area", "REAL"
        );
        if (!db_context)
        {
          return;
        }
        std::vector<std::map<std::string, CastValue>>& reference_data = rawDataHandler_IO.getReferenceData();
        for (const auto& ref : reference_data)
        {
          const auto& v = ref.at("sample_index");
          const auto& s = ref.at("sample_index").s_;
          filenames_I.getSessionDB().write(
            *db_context,
            ref.at("sample_index").i_,
            ref.at("original_filename").s_,
            ref.at("sample_name").s_,
            ref.at("sample_type").s_,
            ref.at("acquisition_date_and_time").s_,
            ref.at("acq_method_name").s_,
            ref.at("component_name").s_,
            ref.at("component_group_name").s_,
            ref.at("retention_time").f_,
            ref.at("start_time").f_,
            ref.at("end_time").f_,
            ref.at("used_").s_,
            ref.at("calculated_concentration").f_,
            ref.at("experiment_id").s_,
            ref.at("acquisition_method_id").s_,
            ref.at("height").f_,
            ref.at("area").f_
            );
        }
        filenames_I.getSessionDB().endWrite(*db_context);
      }
      else
      {
        LOGE << "Not implemented";
      }
      filenames_I.setSavedState("referenceData", true);
    }
    catch (const std::exception& e)
    {
      LOGE << e.what();
    }
    LOGD << "END StoreValidationData";
  }

  void LoadParameters::getFilenames(Filenames& filenames) const
  {
    filenames.addFileName("parameters", "${MAIN_DIR}/parameters.csv", "Parameters", true, true);
  };

  bool LoadParameters::onFilePicked(const std::filesystem::path& filename, ApplicationHandler* application_handler)
  {
    if (application_handler->sequenceHandler_.getSequence().size() == 0)
    {
      LOGE << "File cannot be loaded without first loading the sequence.";
      return false;
    }
    RawDataHandler& rawDataHandler = application_handler->sequenceHandler_.getSequence().at(0).getRawData();
    parameters_observable_ = &(application_handler->sequenceHandler_);
    Filenames filenames;
    filenames.setFullPath("parameters", filename);
    process(rawDataHandler, {}, filenames);
    return true;
  }

  void LoadParameters::process(
    RawDataHandler& rawDataHandler_IO,
    const ParameterSet& params_I,
    Filenames& filenames_I
  ) const
  {
    LOGD << "START LoadParameters";
    getFilenames(filenames_I);
    if (!InputDataValidation::prepareToLoad(filenames_I, "parameters"))
    {
      LOGD << "END " << getName();
      return;
    }
    try
    {
      if (filenames_I.isEmbedded("parameters"))
      {
        auto db_context = filenames_I.getSessionDB().beginRead(
          "parameters",
          "function",
          "name",
          "value",
          "type"
        );
        if (!db_context)
        {
          return;
        }
        ParameterSet parameters;
        std::string function;
        std::string name;
        std::string value;
        std::string type;
        while (filenames_I.getSessionDB().read(
          *db_context,
          function,
          name,
          value,
          type
        ))
        {
          std::map<std::string, std::string> properties =
          {
            {"name", name},
            {"value", value},
            {"type", type},
          };
          auto p = Parameter(properties);
          parameters.addParameter(function, p);
        };
        rawDataHandler_IO.getParameters() = parameters;
        filenames_I.getSessionDB().endRead(*db_context);
      }
      else
      {
          ParametersParser::read(filenames_I.getFullPath("parameters").generic_string(), rawDataHandler_IO.getParameters());
      }
    }
    catch (const std::exception& e) {
      LOGE << e.what();
      throw;
    }
    sanitizeParameters(rawDataHandler_IO.getParameters());
    if (parameters_observable_) parameters_observable_->notifyParametersUpdated();

    LOGD << "END LoadParameters";
  }

  void LoadParameters::sanitizeParameters(
    ParameterSet& params_I
  )
  {
    LOGD << "START sanitizeRawDataProcessorParameters";

    // # check for workflow parameters integrity
    const std::vector<std::string> required_function_parameter_names = {
      "SequenceSegmentPlotter",
      "ElutionPeakDetection",
      "FeaturePlotter",
      "FeatureFindingMetabo",
      "AbsoluteQuantitation",
      "mzML",
      "MassTraceDetection",
      "MRMMapping",
      "ChromatogramExtractor",
      "MRMFeatureFinderScoring",
      "MRMFeatureFilter.filter_MRMFeatures",
      "MRMFeatureSelector.select_MRMFeatures_qmip",
      // "MRMFeatureSelector.schedule_MRMFeatures_qmip",
      "MRMFeatureSelector.select_MRMFeatures_score",
      "ReferenceDataMethods.getAndProcess_referenceData_samples",
      "MRMFeatureValidator.validate_MRMFeatures",
      "MRMFeatureFilter.filter_MRMFeatures.qc",
      "MRMFeatureFilter.filter_MRMFeaturesBackgroundInterferences",
      "MRMFeatureFilter.filter_MRMFeaturesBackgroundInterferences.qc",
      "MRMFeatureFilter.filter_MRMFeaturesRSDs",
      "MRMFeatureFilter.filter_MRMFeaturesRSDs.qc",
      "SequenceProcessor",
      "FIAMS",
      "PickMS1Features",
      "PickMS2Features",
      "AccurateMassSearchEngine",
      "MergeInjections"
    };

    for (const std::string& function_parameter_name : required_function_parameter_names) {
      if (!params_I.count(function_parameter_name)) {
        FunctionParameters function_parameter(function_parameter_name);
        params_I.addFunctionParameters(function_parameter);
      }
    }

    LOGD << "END sanitizeRawDataProcessorParameters";
  }

  void StoreParameters::getFilenames(Filenames& filenames) const
  {
    filenames.addFileName("parameters", "${MAIN_DIR}/parameters.csv", "Parameters", true, true);
  };

  bool StoreParameters::onFilePicked(const std::filesystem::path& filename, ApplicationHandler* application_handler)
  {
    if (application_handler->sequenceHandler_.getSequence().size() == 0)
    {
      LOGE << "File cannot be loaded without first loading the sequence.";
      return false;
    }
    RawDataHandler& rawDataHandler = application_handler->sequenceHandler_.getSequence().at(0).getRawData();
    Filenames filenames;
    filenames.setFullPath("parameters", filename);
    process(rawDataHandler, {}, filenames);
    return true;
  }

  void StoreParameters::process(
    RawDataHandler& rawDataHandler_IO,
    const ParameterSet& params_I,
    Filenames& filenames_I
  ) const
  {
    LOGD << "START StoreParameters";
    try
    {
      if (!InputDataValidation::prepareToStore(filenames_I, "parameters"))
      {
        LOGD << "END " << getName();
        return;
      }
      if (filenames_I.isEmbedded("parameters"))
      {
        auto db_context = filenames_I.getSessionDB().beginWrite(
          "parameters",
          "function", "TEXT",
          "name", "TEXT",
          "value", "TEXT",
          "type", "TEXT"
        );
        if (!db_context)
        {
          return;
        }
        const auto& parameters = rawDataHandler_IO.getParameters();
        for (const auto& parameter_function : parameters)
        {
          for (const auto& parameter : parameter_function.second)
          {
            filenames_I.getSessionDB().write(
              *db_context,
              parameter_function.first,
              parameter.getName(),
              parameter.getValueAsString(),
              parameter.getType()
              );
          }
        }
        filenames_I.getSessionDB().endWrite(*db_context);
      }
      else
      {
        getFilenames(filenames_I);
        LOGI << "Storing " << filename_;
        if (filenames_I.getFullPath("parameters").empty()) {
          LOGE << "Filename is empty";
          LOGD << "END readRawDataProcessingParameters";
          return;
        }
        ParametersParser::write(filenames_I.getFullPath("parameters").generic_string(), rawDataHandler_IO.getParameters());
      }
      filenames_I.setSavedState("parameters", true);
    }
    catch (const std::exception& e)
    {
      LOGE << e.what();
    }
    LOGD << "END StoreParameters";
  }

  void ZeroChromatogramBaseline::process(
    RawDataHandler& rawDataHandler_IO,
    const ParameterSet& params_I,
    Filenames & filenames_I
  ) const
  {
    LOGD << "START ZeroChromatogramBaseline";

    std::vector<OpenMS::MSChromatogram>& chroms = rawDataHandler_IO.getChromatogramMap().getChromatograms();
    for (OpenMS::MSChromatogram& ch : chroms) {
      OpenMS::subtractMinimumIntensity(ch);
    }

    LOGD << "END ZeroChromatogramBaseline";
  }

  ParameterSet MapChromatograms::getParameterSchema() const
  {
    OpenMS::MRMMapping oms_params;
    return ParameterSet({ oms_params });
  }

  void MapChromatograms::process(
    RawDataHandler& rawDataHandler_IO,
    const ParameterSet& params_I,
    Filenames& filenames_I
  ) const
  {
    LOGD << "START MapChromatograms";
    getFilenames(filenames_I);
    // Set up MRMMapping and parse the MRMMapping params
    OpenMS::MRMMapping mrmmapper;
    Utilities::setUserParameters(mrmmapper, params_I);

    mrmmapper.mapExperiment(
      rawDataHandler_IO.getExperiment(),
      rawDataHandler_IO.getTargetedExperiment(),
      rawDataHandler_IO.getChromatogramMap()
    );

    LOGD << "END MapChromatograms";
  }

  void ExtractChromatogramWindows::process(
    RawDataHandler& rawDataHandler_IO,
    const ParameterSet& params_I,
    Filenames& filenames_I
  ) const
  {
    LOGD << "START ExtractChromatogramWindows";
    getFilenames(filenames_I);

    for (const OpenMS::MRMFeatureQC::ComponentQCs& transition_filters : rawDataHandler_IO.getFeatureFilter().component_qcs) {
      for (OpenMS::MSChromatogram& ch : rawDataHandler_IO.getChromatogramMap().getChromatograms()) {
        if (transition_filters.component_name == ch.getNativeID()) {
          OpenMS::removePeaks(ch, transition_filters.retention_time_l, transition_filters.retention_time_u);
        }
      }
    }

    LOGD << "END ExtractChromatogramWindows";
  }

  // Parameters used by Spetra related processors
  ParameterSet FIAMSParameters()
  {
    std::map<std::string, std::vector<std::map<std::string, std::string>>> param_struct({
    {"FIAMS", {
      {
        {"name", "acquisition_start"},
        {"type", "float"},
        {"value", "0"},
        {"description", "The start time to use when extracting out the spectra windows from the MSExperiment"},
        {"min", "0"}
      },
      {
        {"name", "acquisition_end"},
        {"type", "float"},
        {"value", "30"},
        {"description", "The end time to use when extracting out the spectra windows from the MSExperiment"},
        {"min", "0"}
      },
      {
        {"name", "resolution"},
        {"type", "float"},
        {"value", "12000"},
        {"description", "The instrument settings: resolution"}
      },
      {
        {"name", "max_mz"},
        {"type", "float"},
        {"value", "1500"},
        {"description", "Maximum mz"}
      },
      {
        {"name", "bin_step"},
        {"type", "float"},
        {"value", "20"},
        {"description", "The size of the step to recalculated the bin size used for adding up spectra along the time axis"}
      }
    }} });
    return ParameterSet(param_struct);
  }

  ParameterSet ExtractSpectraWindows::getParameterSchema() const
  {
    return FIAMSParameters();
  }

  void ExtractSpectraWindows::process(
    RawDataHandler& rawDataHandler_IO,
    const ParameterSet& params_I,
    Filenames& filenames_I
  ) const
  {
    LOGD << "START ExtractSpectraWindows";
    getFilenames(filenames_I);
    // Complete user parameters with schema
    ParameterSet params(params_I);
    params.merge(getParameterSchema());

    float start = 0, stop = 0;
    if (params.count("FIAMS") && params.at("FIAMS").size())
    {
      for (const auto& fia_params: params.at("FIAMS")) {
        if (fia_params.getName() == "acquisition_start") 
        {
          try {
            start = std::stof(fia_params.getValueAsString());
          }
          catch (const std::exception& e) {
            LOGE << e.what();
          }
        }
        if (fia_params.getName() == "acquisition_end") 
        {
          try {
            stop = std::stof(fia_params.getValueAsString());
          }
          catch (const std::exception& e) {
            LOGE << e.what();
          }
        }
      }
    }

    if (stop == 0) {
      LOGE << "No parameters passed to ExtractSpectraWindows.  Spectra will not be extracted.";
      LOGD << "END ExtractSpectraWindows";
      return;
    }

    std::vector<OpenMS::MSSpectrum> output;
    for (OpenMS::MSSpectrum& spec : rawDataHandler_IO.getExperiment().getSpectra()) {
      if (spec.getRT() >= start && spec.getRT() <= stop) {
        output.push_back(spec);
      }
    }

    if (output.empty()) {
      LOGW << "No spectra was extracted.  Check that the specified start and stop retention times in the parameters are compatible with the acquired spectra.";
    }
    rawDataHandler_IO.getExperiment().setSpectra(output);

    LOGD << "END ExtractSpectraWindows";
  }

  ParameterSet FitFeaturesEMG::getParameterSchema() const
  {
    OpenMS::EmgGradientDescent oms_params;
    return ParameterSet({ oms_params });
  }

  void FitFeaturesEMG::process(
    RawDataHandler& rawDataHandler_IO,
    const ParameterSet& params_I,
    Filenames& filenames_I
  ) const
  {
    LOGD << "START FitFeaturesEMG";
    getFilenames(filenames_I);
    OpenMS::EmgGradientDescent emg;
    Utilities::setUserParameters(emg, params_I);

    OpenMS::FeatureMap& featureMap = rawDataHandler_IO.getFeatureMap();

    const std::vector<OpenMS::MSChromatogram>& chromatograms {
      rawDataHandler_IO.getChromatogramMap().getChromatograms() };

    auto getChromatogramByName = [&chromatograms](const OpenMS::String& name) -> const OpenMS::MSChromatogram&
    {
      const std::vector<OpenMS::MSChromatogram>::const_iterator it =
        std::find_if(chromatograms.cbegin(), chromatograms.cend(), [&name](const OpenMS::MSChromatogram& chrom){
          return name == chrom.getNativeID();
        });
      if (it == chromatograms.cend()) {
        throw std::string("Can't find a chromatogram with NativeID == ") + name;
      }
      return *it;
    };

    try {
      for (OpenMS::Feature& feature : featureMap) {
        std::cout << "NEW FEATURE\n";
        const double left { feature.getMetaValue("leftWidth") };
        const double right { feature.getMetaValue("rightWidth") };
        std::cout << "Boundaries: [" << left << ", " << right << "]\n";
        std::vector<OpenMS::Feature>& subordinates { feature.getSubordinates() };
        std::cout << "n. subordinates: " << subordinates.size() << "\n";
        for (OpenMS::Feature& subfeature : subordinates) {
          std::cout << "NEW SUBFEATURE\n";
          const OpenMS::String name = subfeature.getMetaValue("native_id");
          std::cout << "Subordinate name: " << name << "\n";
          const OpenMS::MSChromatogram& chromatogram = getChromatogramByName(name);
          std::cout << "Chromatogram found!\n";
          std::vector<double> x;
          std::vector<double> y;
          extractPointsIntoVectors(chromatogram, left, right, x, y);
          std::cout << "Extracted n. points: " << x.size() << "\n";

          if (x.size() < 3) {
            std::cout << "Less than 2 points. Skipping: " << name << "\n\n";
            continue;
          }

          // EMG parameter estimation with gradient descent
          double h, mu, sigma, tau;
          std::cout << "Estimating EMG parameters...\n";
          emg.estimateEmgParameters(x, y, h, mu, sigma, tau);

          // Estimate the intensities for each point
          std::vector<double> out_xs;
          std::vector<double> out_ys;
          std::cout << "Applying estimated parameters...\n";
          emg.applyEstimatedParameters(x, h, mu, sigma, tau, out_xs, out_ys);

          // integrate area and estimate background, update the subfeature

          std::cout << "emg peak # points: " << out_xs.size() << "\n";
          OpenMS::ConvexHull2D::PointArrayType hull_points(out_xs.size());
          OpenMS::MSChromatogram emg_chrom;
          for (size_t i = 0; i < out_xs.size(); ++i) {
            emg_chrom.push_back(OpenMS::ChromatogramPeak(out_xs[i], out_ys[i]));
            hull_points[i][0] = out_xs[i];
            hull_points[i][1] = out_ys[i];
          }
          OpenMS::ConvexHull2D hull;
          hull.addPoints(hull_points);
          subfeature.getConvexHulls().push_back(hull);

          OpenMS::PeakIntegrator pi;
          std::cout << "Updating ranges...\n";
          emg_chrom.updateRanges();
          std::cout << "Ranges updated.\n";
          const double emg_chrom_left { emg_chrom.getMin()[0] };
          const double emg_chrom_right { emg_chrom.getMax()[0] };
          std::cout << "Positions calculated.\n";
          OpenMS::PeakIntegrator::PeakArea pa = pi.integratePeak(emg_chrom, emg_chrom_left, emg_chrom_right);
          std::cout << "Area calculated.\n";
          OpenMS::PeakIntegrator::PeakBackground pb = pi.estimateBackground(emg_chrom, emg_chrom_left, emg_chrom_right, pa.apex_pos);
          std::cout << "Background calculated.\n";
          double peak_integral { pa.area - pb.area };
          double peak_apex_int { pa.height - pb.height };
          if (peak_integral < 0) { peak_integral = 0; }
          if (peak_apex_int < 0) { peak_apex_int = 0; }

          std::cout << "Intensity: " << subfeature.getIntensity() << "\t" << peak_integral << "\n";
          std::cout << "peak_apex_position: " << subfeature.getMetaValue("peak_apex_position") << "\t" << pa.apex_pos << "\n";
          std::cout << "peak_apex_int: " << subfeature.getMetaValue("peak_apex_int") << "\t" << peak_apex_int << "\n";
          std::cout << "area_background_level: " << subfeature.getMetaValue("area_background_level") << "\t" << pb.area << "\n";
          std::cout << "noise_background_level: " << subfeature.getMetaValue("noise_background_level") << "\t" << pb.height << "\n\n";

          subfeature.setIntensity(peak_integral);
          subfeature.setMetaValue("peak_apex_position", pa.apex_pos);
          subfeature.setMetaValue("peak_apex_int", peak_apex_int);
          subfeature.setMetaValue("area_background_level", pb.area);
          subfeature.setMetaValue("noise_background_level", pb.height);
        }
      }
      rawDataHandler_IO.updateFeatureMapHistory();
    }
    catch (const std::exception& e) {
      std::cout << "I catched the exception (in SmartPeak)!\n";
      LOGE << e.what();
    }

    LOGD << "END FitFeaturesEMG";
  }

  void FitFeaturesEMG::extractPointsIntoVectors(
    const OpenMS::MSChromatogram& chromatogram,
    const double left,
    const double right,
    std::vector<double>& x,
    std::vector<double>& y
  ) const
  {
    x.clear();
    y.clear();
    OpenMS::MSChromatogram::ConstIterator it = chromatogram.PosBegin(left);
    const OpenMS::MSChromatogram::ConstIterator end = chromatogram.PosEnd(right);
    std::cout << "empty range: " << (it == end) << "\n";
    for (; it != end; ++it) {
      x.push_back(it->getPos());
      y.push_back(it->getIntensity());
    }
  }

  ParameterSet FilterFeaturesRSDs::getParameterSchema() const
  {
    OpenMS::MRMFeatureFilter oms_params;
    return ParameterSet({ oms_params });
  }

  void FilterFeaturesRSDs::process(
    RawDataHandler& rawDataHandler_IO,
    const ParameterSet& params_I,
    Filenames& filenames_I
  ) const
  {
    LOGD << "START filterFeaturesRSDs";
    getFilenames(filenames_I);
    LOGI << "Feature Filter input size: " << rawDataHandler_IO.getFeatureMap().size();

    OpenMS::MRMFeatureFilter featureFilter;
    Utilities::setUserParameters(featureFilter, params_I, "MRMFeatureFilter.filter_MRMFeaturesRSDs");

    OpenMS::FeatureMap& featureMap = rawDataHandler_IO.getFeatureMap();

    featureFilter.FilterFeatureMapPercRSD(
      featureMap,
      rawDataHandler_IO.getFeatureRSDFilter(),
      rawDataHandler_IO.getFeatureRSDEstimations()
    );

    rawDataHandler_IO.updateFeatureMapHistory();

    LOGI << "Feature Filter output size: " << featureMap.size();
    LOGD << "END filterFeaturesRSDs";
  }

  ParameterSet CheckFeaturesRSDs::getParameterSchema() const
  {
    OpenMS::MRMFeatureFilter oms_params;
    return ParameterSet({ oms_params });
  }

  void CheckFeaturesRSDs::process(
    RawDataHandler& rawDataHandler_IO,
    const ParameterSet& params_I,
    Filenames& filenames_I
  ) const
  {
    LOGD << "START checkFeaturesRSDs";
    getFilenames(filenames_I);
    LOGI << "Feature Checker input size: " << rawDataHandler_IO.getFeatureMap().size();

    OpenMS::MRMFeatureFilter featureFilter;
    Utilities::setUserParameters(featureFilter, params_I, "MRMFeatureFilter.filter_MRMFeaturesRSDs.qc");

    featureFilter.FilterFeatureMapPercRSD(
      rawDataHandler_IO.getFeatureMap(),
      rawDataHandler_IO.getFeatureRSDQC(),
      rawDataHandler_IO.getFeatureRSDEstimations()
    );

    rawDataHandler_IO.updateFeatureMapHistory();

    LOGI << "Feature Checker output size: " << rawDataHandler_IO.getFeatureMap().size();
    LOGD << "END checkFeaturesRSDs";
  }

  ParameterSet FilterFeaturesBackgroundInterferences::getParameterSchema() const
  {
    OpenMS::MRMFeatureFilter oms_params;
    return ParameterSet({ oms_params });
  }

  void FilterFeaturesBackgroundInterferences::process(
    RawDataHandler& rawDataHandler_IO,
    const ParameterSet& params_I,
    Filenames& filenames_I
  ) const
  {
    LOGD << "START filterFeaturesBackgroundInterferences";
    getFilenames(filenames_I);
    LOGI << "Feature Filter input size: " << rawDataHandler_IO.getFeatureMap().size();

    OpenMS::MRMFeatureFilter featureFilter;
    Utilities::setUserParameters(featureFilter, params_I, "MRMFeatureFilter.filter_MRMFeaturesBackgroundInterferences");

    OpenMS::FeatureMap& featureMap = rawDataHandler_IO.getFeatureMap();

    featureFilter.FilterFeatureMapBackgroundInterference(
      featureMap,
      rawDataHandler_IO.getFeatureBackgroundFilter(),
      rawDataHandler_IO.getFeatureBackgroundEstimations()
    );

    rawDataHandler_IO.updateFeatureMapHistory();

    LOGI << "Feature Filter output size: " << featureMap.size();
    LOGD << "END filterFeaturesBackgroundInterferences";
  }

  ParameterSet CheckFeaturesBackgroundInterferences::getParameterSchema() const
  {
    OpenMS::MRMFeatureFilter oms_params;
    return ParameterSet({ oms_params });
  }

  void CheckFeaturesBackgroundInterferences::process(
    RawDataHandler& rawDataHandler_IO,
    const ParameterSet& params_I,
    Filenames& filenames_I
  ) const
  {
    LOGD << "START checkFeaturesBackgroundInterferences";
    getFilenames(filenames_I);
    LOGI << "Feature Checker input size: " << rawDataHandler_IO.getFeatureMap().size();

    OpenMS::MRMFeatureFilter featureFilter;
    Utilities::setUserParameters(featureFilter, params_I, "MRMFeatureFilter.filter_MRMFeaturesBackgroundInterferences.qc");

    featureFilter.FilterFeatureMapBackgroundInterference(
      rawDataHandler_IO.getFeatureMap(),
      rawDataHandler_IO.getFeatureBackgroundQC(),
      rawDataHandler_IO.getFeatureBackgroundEstimations()
    );

    rawDataHandler_IO.updateFeatureMapHistory();

    LOGI << "Feature Checker output size: " << rawDataHandler_IO.getFeatureMap().size();
    LOGD << "END checkFeaturesBackgroundInterferences";
  }

  ParameterSet MergeSpectra::getParameterSchema() const
  {
    return FIAMSParameters();
  }

  void MergeSpectra::process(
    RawDataHandler& rawDataHandler_IO,
    const ParameterSet& params_I,
    Filenames& filenames_I
  ) const
  {
    LOGD << "START MergeSpectra";
    getFilenames(filenames_I);

    // Complete user parameters with schema
    ParameterSet params(params_I);
    params.merge(getParameterSchema());

    float resolution = 0, max_mz = 0, bin_step = 0;
    if (params.count("FIAMS") && params.at("FIAMS").size()) {
      for (const auto& fia_params : params.at("FIAMS")) {
        if (fia_params.getName() == "max_mz") {
          try {
            max_mz = std::stof(fia_params.getValueAsString());
          }
          catch (const std::exception& e) {
            LOGE << e.what();
          }
        }
        if (fia_params.getName() == "bin_step") {
          try {
            bin_step = std::stof(fia_params.getValueAsString());
          }
          catch (const std::exception& e) {
            LOGE << e.what();
          }
        }
        if (fia_params.getName() == "resolution") {
          try {
            resolution = std::stof(fia_params.getValueAsString());
          }
          catch (const std::exception& e) {
            LOGE << e.what();
          }
        }
      }
    }

    if (resolution == 0 || max_mz == 0 || bin_step == 0) {
      LOGE << "Missing parameters for MergeSpectra.  Spectra will not be merged.";
      LOGD << "END MergeSpectra";
      return;
    }

    // calculate the bin sizes and mass buckets
    int n_bins = max_mz / bin_step;
    std::vector<float> mzs;
    std::vector<float> bin_sizes;
    std::vector<std::vector<OpenMS::MSSpectrum>> binned_spectrum;
    mzs.reserve(n_bins);
    bin_sizes.reserve(n_bins);
    binned_spectrum.resize(n_bins);
    for (int i = 0; i < n_bins; i++) {
      mzs.push_back((i + 1) * bin_step);
      bin_sizes.push_back(mzs.at(i) / (resolution * 4.0));
      binned_spectrum.at(i).resize(rawDataHandler_IO.getExperiment().getSpectra().size());
    }

    // Divide the spectra into mass ranges
    for (int s = 0; s < rawDataHandler_IO.getExperiment().getSpectra().size(); ++s) {
      const OpenMS::MSSpectrum spectrum = rawDataHandler_IO.getExperiment().getSpectra().at(s);
      for (auto it = spectrum.begin(); it != spectrum.end(); ++it) {
        for (int i = 0; i < mzs.size() - 1; ++i) {
          if (it->getMZ() >= mzs.at(i) && it->getMZ() < mzs.at(i + 1)) {
            binned_spectrum.at(i).at(s).push_back(*it);
          }
        }
      }
    }

    // Merge spectra along time for each of the different mass ranges
    OpenMS::MSSpectrum output;
    for (int i = 0; i < mzs.size() - 1; ++i) {
      OpenMS::MSSpectrum full_spectrum = OpenMS::SpectrumAddition::addUpSpectra(
        binned_spectrum.at(i), bin_sizes.at(i), false
      );
      for (auto it = full_spectrum.begin(); it != full_spectrum.end(); ++it) {
        output.push_back(*it);
      }
    }
    output.sortByPosition();

    // Update the metavalue and members
    output.setNativeID("MergeSpectra");
    OpenMS::Peak1D::CoordinateType lowest_observed = 0.0;
    OpenMS::Peak1D::CoordinateType highest_observed = 0.0;
    const auto& spectra = rawDataHandler_IO.getExperiment().getSpectra();
    if (spectra.size())
    {
      output.setMSLevel(spectra.front().getMSLevel());
      output.setType(spectra.front().getType());
      for (auto it_spectra = spectra.begin(); it_spectra != spectra.end(); ++it_spectra)
      {
        if (!(*it_spectra).empty())
        {
          lowest_observed = (*it_spectra).front().getMZ();
          break;
        }
      }
      for (auto it_spectra = spectra.rbegin(); it_spectra != spectra.rend(); ++it_spectra)
      {
        if (!(*it_spectra).empty())
        {
          highest_observed = (*it_spectra).back().getMZ();
          break;
        }
      }
    }
    output.setMetaValue("lowest observed m/z", lowest_observed);
    output.setMetaValue("highest observed m/z", highest_observed);
    output.setMetaValue("base peak m/z", 0.0);
    output.setMetaValue("base peak intensity", 0.0);
    output.setMetaValue("total ion current", 0.0);
    rawDataHandler_IO.getExperiment().setSpectra({ output });

    LOGD << "END MergeSpectra";
  }

  ParameterSet PickMS1Features::getParameterSchema() const
  {
    OpenMS::SavitzkyGolayFilter sgfilter;
    OpenMS::PeakPickerHiRes picker;
    OpenMS::PeakIntegrator pi;
    ParameterSet parameters({ sgfilter, picker, pi });

    std::map<std::string, std::vector<std::map<std::string, std::string>>> param_struct({
    {"PickMS1Features", {
    {
      {"name", "frame_length"},
      {"type", "int"},
      {"value", "11"},
      {"description", "SavitzkyGolayFilter parameter. The number of subsequent data points used for smoothing"},
    },
    {
      {"name", "polynomial_order"},
      {"type", "int"},
      {"value", "4"},
      {"description", "SavitzkyGolayFilter parameter. Order or the polynomial that is fitted"},
    },
    {
      {"name", "sne:window"},
      {"type", "float"},
      {"value", "10"},
      {"description", "SignalToNoiseEstimatorMedianRapid parameter. Signal-to-noise estimation window (in mz)"},
    },
    {
      {"name", "write_convex_hull"},
      {"type", "bool"},
      {"value", "false"},
      {"description", "Whether to write out all points of all features into the featureXML"},
    },
    {
      {"name", "compute_peak_shape_metrics"},
      {"type", "bool"},
      {"value", "false"},
      {"description", "Calulates various peak shape metrics (e.g., tailing) that can be used for downstream QC/QA."},
    },
    {
      {"name", "min_intensity"},
      {"type", "float"},
      {"value", "86000"},
      {"description", "All features below the minimum intensity will be discarded"},
    }
    }} });
    ParameterSet pick_ms1_feature_params(param_struct);
    parameters.merge(pick_ms1_feature_params);
    return parameters;
  }

  void PickMS1Features::process(RawDataHandler& rawDataHandler_IO,
    const ParameterSet& params_I,
    Filenames& filenames_I
  ) const
  {
    LOGD << "START PickMS1Features";
    getFilenames(filenames_I);

    // Complete user parameters with schema
    ParameterSet params(params_I);
    params.merge(getParameterSchema());
    
    float sn_window = 0;
    bool compute_peak_shape_metrics = false;
    float min_intensity = 0;
    bool write_convex_hull = false;
    for (const auto& pms1f_params : params.at("PickMS1Features")) {
      if (pms1f_params.getName() == "sne:window") {
        try {
          sn_window = std::stof(pms1f_params.getValueAsString());
        }
        catch (const std::exception& e) {
          LOGE << e.what();
        }
      }
      if (pms1f_params.getName() == "write_convex_hull") {
        try {
          std::string value = pms1f_params.getValueAsString();
          std::transform(value.begin(), value.end(), value.begin(), ::tolower);
          write_convex_hull = (value == "true")?true:false;
        }
        catch (const std::exception& e) {
          LOGE << e.what();
        }
      }
      if (pms1f_params.getName() == "compute_peak_shape_metrics") {
        try {
          std::string value = pms1f_params.getValueAsString();
          std::transform(value.begin(), value.end(), value.begin(), ::tolower);
          compute_peak_shape_metrics = (value == "true") ? true : false;
        }
        catch (const std::exception& e) {
          LOGE << e.what();
        }
      }
      if (pms1f_params.getName() == "min_intensity") {
        try {
          min_intensity = std::stof(pms1f_params.getValueAsString());
        }
        catch (const std::exception& e) {
          LOGE << e.what();
        }
      }
    }
    if (sn_window == 0) {
      LOGE << "Missing sne:window parameter for PickMS1Features. Not picking";
      LOGD << "END PickMS1Features";
      return;
    }
    
    OpenMS::SavitzkyGolayFilter sgfilter;
    Utilities::setUserParameters(sgfilter, params_I, "PickMS1Features");

    OpenMS::PeakPickerHiRes picker;
    Utilities::setUserParameters(picker, params_I, "PickMS1Features");

    OpenMS::PeakIntegrator pi;
    Utilities::setUserParameters(pi, params_I, "PickMS1Features");

    OpenMS::FeatureMap featureMap;
    try {
      for (const OpenMS::MSSpectrum& spec : rawDataHandler_IO.getExperiment().getSpectra()) {
        // Smooth and pick
        OpenMS::MSSpectrum input(spec);
        sgfilter.filter(input);
        std::vector<OpenMS::PeakPickerHiRes::PeakBoundary> boundaries;
        OpenMS::MSSpectrum output;
        picker.pick(input, output, boundaries);
        if (output.size() <= 0) continue;

        // Estimate the S/N
        OpenMS::SignalToNoiseEstimatorMedianRapid sne(sn_window);
        std::vector<double> mzs, intensities;
        mzs.reserve(spec.size());
        intensities.reserve(spec.size());
        for (auto it = spec.begin(); it != spec.end(); ++it)
        {
          mzs.push_back(it->getMZ());
          intensities.push_back(it->getIntensity());
        }
        if (mzs.size() <= 2) continue;
        OpenMS::SignalToNoiseEstimatorMedianRapid::NoiseEstimator e = sne.estimateNoise(mzs, intensities);

        // Create the featureMap
        int i = 0;
        for (auto it = output.begin(); it != output.end(); ++it)
        {
          // set the metadata
          OpenMS::Feature f;
          f.setUniqueId();
          f.setMZ(it->getMZ());
          f.setRT(0);
          f.setMetaValue("native_id", spec.getNativeID());
          f.setMetaValue("PeptideRef", "Unknown");
          f.setMetaValue("scan_polarity", rawDataHandler_IO.getMetaData().scan_polarity);
          f.setMetaValue("logSN", std::log(e.get_noise_value(it->getMZ())));
          f.setMetaValue("leftWidth", boundaries.at(i).mz_min);
          f.setMetaValue("rightWidth", boundaries.at(i).mz_max);

          // compute the peak area
          OpenMS::PeakIntegrator::PeakArea pa = pi.integratePeak(input, boundaries.at(i).mz_min, boundaries.at(i).mz_max);
          f.setIntensity(pa.area);
          f.setMetaValue("peak_apex_int", pa.height);
          f.setMetaValue("peak_apex_position", it->getMZ());

          // check the min intensity threshold
          if (pa.height < min_intensity) {
            //LOGD << "Feature: " << f.getUniqueId() << " with peak_apex_int=" << pa.height << " did not pass the `min_threshold`=" << min_intensity;
            ++i;
            continue;
          }

          // Calculate peak shape metrics that will be used for later QC
          OpenMS::PeakIntegrator::PeakShapeMetrics psm = pi.calculatePeakShapeMetrics(input, boundaries.at(i).mz_min, boundaries.at(i).mz_max, pa.height, it->getMZ());
          f.setMetaValue("width_at_50", psm.width_at_50);
          if (compute_peak_shape_metrics)
          {
            f.setMetaValue("width_at_5", psm.width_at_5);
            f.setMetaValue("width_at_10", psm.width_at_10);
            f.setMetaValue("start_position_at_5", psm.start_position_at_5);
            f.setMetaValue("start_position_at_10", psm.start_position_at_10);
            f.setMetaValue("start_position_at_50", psm.start_position_at_50);
            f.setMetaValue("end_position_at_5", psm.end_position_at_5);
            f.setMetaValue("end_position_at_10", psm.end_position_at_10);
            f.setMetaValue("end_position_at_50", psm.end_position_at_50);
            f.setMetaValue("total_width", psm.total_width);
            f.setMetaValue("tailing_factor", psm.tailing_factor);
            f.setMetaValue("asymmetry_factor", psm.asymmetry_factor);
            f.setMetaValue("slope_of_baseline", psm.slope_of_baseline);
            f.setMetaValue("baseline_delta_2_height", psm.baseline_delta_2_height);
            f.setMetaValue("points_across_baseline", psm.points_across_baseline);
            f.setMetaValue("points_across_half_height", psm.points_across_half_height);
          }

          // extract out the convex hull
          if (write_convex_hull) {
            OpenMS::ConvexHull2D hull;
            hull.setHullPoints(pa.hull_points);
            f.setConvexHulls({ hull });
          }
          featureMap.push_back(f);
          ++i;
        }
      }
    }
    catch (const std::exception& e) {
      LOGE << e.what();
    }

    // NOTE: setPrimaryMSRunPath() is needed for calculate_calibration
    featureMap.setPrimaryMSRunPath({ rawDataHandler_IO.getMetaData().getFilename() });
    LOGD << "setPrimaryMSRunPath: " << rawDataHandler_IO.getMetaData().getFilename();

    rawDataHandler_IO.setFeatureMap(featureMap);
    rawDataHandler_IO.updateFeatureMapHistory();

    LOGI << "Feature Picker output size: " << featureMap.size();
    LOGD << "END PickMS1Features";
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

    OpenMS::AccurateMassSearchEngine ams;
    Utilities::setUserParameters(ams, params_I);

    try {
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
    }
    catch (const std::exception& e) {
      LOGE << e.what();
    }

    LOGI << "SearchAccurateMass output size: " << rawDataHandler_IO.getFeatureMap().size();
    LOGD << "END SearchAccurateMass";
  }

  void MergeFeatures::process(RawDataHandler& rawDataHandler_IO,
    const ParameterSet& params_I,
    Filenames& filenames_I
  ) const
  {
    LOGD << "START MergeFeatures";
    getFilenames(filenames_I);
    LOGI << "MergeFeatures input size: " << rawDataHandler_IO.getFeatureMap().size();

    try {
      // Pass 1: organize into a map by combining features and subordinates with the same `identifier`
      OpenMS::FeatureMap fmap;
      std::map<std::string, std::vector<OpenMS::Feature>> fmapmap;
      for (const OpenMS::Feature& f : rawDataHandler_IO.getFeatureMap()) {
        if (f.metaValueExists("identifier")) {
          auto found_f = fmapmap.emplace(f.getMetaValue("identifier").toStringList().at(0), std::vector<OpenMS::Feature>({ f }));
          if (!found_f.second) {
            fmapmap.at(f.getMetaValue("identifier").toStringList().at(0)).push_back(f);
          }
        }
        for (const OpenMS::Feature& s : f.getSubordinates()) {
          if (s.metaValueExists("identifier")) {
            auto found_s = fmapmap.emplace(s.getMetaValue("identifier").toStringList().at(0), std::vector<OpenMS::Feature>({ s }));
            if (!found_s.second) {
              fmapmap.at(s.getMetaValue("identifier").toStringList().at(0)).push_back(s);
            }
          }
        }
      }

      // Pass 2: compute the consensus manually
      for (const auto& f_map : fmapmap) {

        // compute the total intensity for weighting
        double total_intensity = 0;
        for (const auto& f : f_map.second) {
          if (f.metaValueExists("peak_apex_int")) 
            total_intensity += (double)f.getMetaValue("peak_apex_int");
          else 
            total_intensity += f.getIntensity();
        }

        // compute the weighted averages
        double rt = 0.0, m = 0.0, intensity = 0.0, peak_apex_int = 0.0;
        double weighting_factor = 1.0 / f_map.second.size(); // will be updated
        for (const auto& f : f_map.second) {
          // compute the weighting factor
          if (f.metaValueExists("peak_apex_int")) 
            weighting_factor = (double)f.getMetaValue("peak_apex_int") / total_intensity;
          else 
            weighting_factor = f.getIntensity() / total_intensity;

          // compute the weighted averages
          rt += f.getRT() * weighting_factor;
          if (f.getCharge() == 0)
            LOGW << "ConsensusFeature::computeDechargeConsensus() WARNING: Feature's charge is 0! This will lead to M=0!";
          //m += (f.getMZ() * std::abs(f.getCharge()) + (double)f.getMetaValue("dc_charge_adduct_mass")) * weighting_factor; // weighted mz
          m += f.getMZ() * weighting_factor;
          //intensity += f.getIntensity() * weighting_factor; // weighted intensity
          intensity += f.getIntensity();
          if (f.metaValueExists("peak_apex_int")) 
            //peak_apex_int += (double)f.getMetaValue("peak_apex_int") * weighting_factor; // weighted peak_apex_int
          peak_apex_int += (double)f.getMetaValue("peak_apex_int");
        }

        // make the feature map and assign subordinates
        OpenMS::Feature f;
        f.setUniqueId();
        f.setMetaValue("PeptideRef", f_map.first);
        f.setMZ(m);
        f.setRT(rt);
        f.setMetaValue("scan_polarity", f_map.second.front().getMetaValue("scan_polarity"));
        f.setIntensity(intensity);
        f.setMetaValue("peak_apex_int", peak_apex_int);
        f.setSubordinates(f_map.second);
        fmap.push_back(f);
      }
      rawDataHandler_IO.setFeatureMap(fmap);
    }
    catch (const std::exception& e) {
      LOGE << e.what();
    }
    rawDataHandler_IO.updateFeatureMapHistory();

    LOGI << "MergeFeatures output size: " << rawDataHandler_IO.getFeatureMap().size();
    LOGD << "END MergeFeatures";
  }

  ParameterSet SearchSpectrum::getParameterSchema() const
  {
    OpenMS::TargetedSpectraExtractor oms_params;
    return ParameterSet({ oms_params });
  }
  void SearchSpectrum::process(RawDataHandler& rawDataHandler_IO,
    const ParameterSet& params_I,
    Filenames& filenames_I
  ) const
  {
    LOGD << "START SearchSpectrum";
    getFilenames(filenames_I);

    // Complete user parameters with schema
    ParameterSet params(params_I);
    params.merge(getParameterSchema());

    OpenMS::TargetedSpectraExtractor targeted_spectra_extractor;
    OpenMS::Param parameters = targeted_spectra_extractor.getParameters();
    targeted_spectra_extractor.setParameters(parameters);

    try {
      OpenMS::FeatureMap feat_map_output;
      targeted_spectra_extractor.searchSpectrum(rawDataHandler_IO.getFeatureMap(), feat_map_output);
      rawDataHandler_IO.setFeatureMap(feat_map_output);
    }
    catch (const std::exception& e) {
      LOGE << "SearchSpectrum : " << typeid(e).name() << " : " << e.what();
    }

    LOGD << "END SearchSpectrum";
  }

  ParameterSet DDA::getParameterSchema() const
  {
    OpenMS::TargetedSpectraExtractor oms_params;
    return ParameterSet({ oms_params });
  }

  void DDA::getFilenames(Filenames& filenames) const
  {
    filenames.addFileName("traML", "${FEATURES_OUTPUT_PATH}/${OUTPUT_INJECTION_NAME}.traML");
  };

  void DDA::process(RawDataHandler& rawDataHandler_IO,
    const ParameterSet& params_I,
    Filenames& filenames_I
  ) const
  {
    LOGD << "START DDA";
    getFilenames(filenames_I);

    // Complete user parameters with schema
    ParameterSet params(params_I);
    params.merge(getParameterSchema());

    OpenMS::TargetedSpectraExtractor targeted_spectra_extractor;
    OpenMS::Param parameters = targeted_spectra_extractor.getParameters();
    targeted_spectra_extractor.setParameters(parameters);

    try {
      // merge features (will be on MS1 spectra)
      OpenMS::FeatureMap& ms1_accurate_mass_found_feature_map = rawDataHandler_IO.getFeatureMap();
      OpenMS::FeatureMap ms1_merged_features;
      targeted_spectra_extractor.mergeFeatures(ms1_accurate_mass_found_feature_map, ms1_merged_features);

      // annotateSpectra :  I would like to annotate my MS2 spectra with the likely MS1 feature that it was derived from
      std::vector<OpenMS::MSSpectrum> annotated_spectra;
      OpenMS::FeatureMap ms2_features; // will be the input of annoteSpectra
      targeted_spectra_extractor.annotateSpectra(rawDataHandler_IO.getExperiment().getSpectra(), ms1_merged_features, ms2_features, annotated_spectra);

      // pickSpectra
      std::vector<OpenMS::MSSpectrum> picked_spectra;
      for (const auto& spectrum : annotated_spectra)
      {
        OpenMS::MSSpectrum picked_spectrum;
        targeted_spectra_extractor.pickSpectrum(spectrum, picked_spectrum);
        picked_spectra.push_back(picked_spectrum);
      }

      // score and select
      std::vector<OpenMS::MSSpectrum> scored_spectra;
      targeted_spectra_extractor.scoreSpectra(annotated_spectra, picked_spectra, scored_spectra);

      std::vector<OpenMS::MSSpectrum> selected_spectra;
      OpenMS::FeatureMap selected_features;
      targeted_spectra_extractor.selectSpectra(scored_spectra, ms2_features, selected_spectra, selected_features, true);

      // searchSpectra (will be on MS2 spectra)
      OpenMS::FeatureMap ms2_accurate_mass_found_feature_map;
      targeted_spectra_extractor.searchSpectrum(selected_features, ms2_accurate_mass_found_feature_map);

      // merge features again (on MS2 spectra features)
      OpenMS::FeatureMap ms2_merged_features;
      targeted_spectra_extractor.mergeFeatures(ms2_accurate_mass_found_feature_map, ms2_merged_features);

      // Store - we want to store MS1 and the associated MS2 features 
      OpenMS::Param params = targeted_spectra_extractor.getParameters();
      params.setValue("output_format", "traML");
      params.setValue("deisotoping:use_deisotoper", "true");
      targeted_spectra_extractor.setParameters(params);
      LOGI << "Storing: " << filenames_I.getFullPath("traML").generic_string();
      targeted_spectra_extractor.storeSpectraTraML(filenames_I.getFullPath("traML").generic_string(), ms1_merged_features, ms2_merged_features);

      // build MS1/MS2 features
      OpenMS::FeatureMap ms1_ms2_features;
      for (const auto& ms1_feature : ms1_merged_features)
      {
        OpenMS::Feature ms_level_annotated_feature = ms1_feature;
        ms_level_annotated_feature.setMetaValue("ms_level", 1);
        ms1_ms2_features.push_back(ms_level_annotated_feature);
      }
      for (const auto& ms2_feature : ms2_merged_features)
      {
        OpenMS::Feature ms_level_annotated_feature = ms2_feature;
        ms_level_annotated_feature.setMetaValue("ms_level", 2);
        ms1_ms2_features.push_back(ms_level_annotated_feature);
      }
      rawDataHandler_IO.setFeatureMap(ms1_ms2_features);
      rawDataHandler_IO.updateFeatureMapHistory();
    }
    catch (const std::exception& e) {
      LOGE << "DDA : " << typeid(e).name() << " : " << e.what();
    }

    LOGD << "END DDA";
  }

  void ClearData::process(RawDataHandler& rawDataHandler_IO,
    const ParameterSet& params_I,
    Filenames& filenames_I
  ) const
  {
    LOGD << "START ClearData";
    getFilenames(filenames_I);
    rawDataHandler_IO.clearNonSharedData();
    LOGD << "END ClearData";
  }

  ParameterSet CalculateMDVs::getParameterSchema() const
  {
    std::map<std::string, std::vector<std::map<std::string, std::string>>> param_struct({
    {"CalculateMDVs", {
      {
        {"name", "mass_intensity_type"},
        {"type", "string"},
        {"value", "norm_sum"},
        {"description", "Type of intensity"},
        {"valid_strings", "['norm_sum','norm_max']"}
      },
      {
        {"name", "feature_name"},
        {"type", "string"},
        {"value", "intensity"},
        {"description", "The name of the FeatureMap attribute to use. Examples include peak_apex_int, peak_area, and intensity."},
      }
    }} });
    return ParameterSet(param_struct);
  }

  void CalculateMDVs::process(
    RawDataHandler& rawDataHandler_IO,
    const ParameterSet& params_I,
    Filenames& filenames_I
  ) const
  {
    LOGD << "START CalculateMDVs";
    getFilenames(filenames_I);

    // Complete user parameters with schema
    ParameterSet params(params_I);
    params.merge(getParameterSchema());

    OpenMS::IsotopeLabelingMDVs isotopelabelingmdvs;
    OpenMS::Param parameters = isotopelabelingmdvs.getParameters();
    isotopelabelingmdvs.setParameters(parameters);

    try {
      OpenMS::FeatureMap normalized_featureMap;
      auto& CalculateMDVs_params = params.at("CalculateMDVs");
      
      std::string feature_name;
      OpenMS::IsotopeLabelingMDVs::MassIntensityType mass_intensity_type;
      for (auto& param : CalculateMDVs_params)
      {
        if (param.getName() == "mass_intensity_type")
        {
          if (param.getValueAsString() == "norm_sum")
          {
            mass_intensity_type = OpenMS::IsotopeLabelingMDVs::MassIntensityType::NORM_SUM;
          }
          else if (param.getValueAsString() == "norm_max")
          {
            mass_intensity_type = OpenMS::IsotopeLabelingMDVs::MassIntensityType::NORM_MAX;
          }
        }
        else if (param.getName() == "feature_name")
        {
          feature_name = param.getValueAsString();
        }
      }
      
      isotopelabelingmdvs.calculateMDVs(rawDataHandler_IO.getFeatureMap(), normalized_featureMap, mass_intensity_type, feature_name);
      rawDataHandler_IO.setFeatureMap(normalized_featureMap);
      rawDataHandler_IO.updateFeatureMapHistory();
    }
    catch (const std::exception& e) {
      LOGE << "calculateMDVs : " << typeid(e).name() << " : " << e.what();
    }

    LOGD << "END calculateMDVs";
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

    try {
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
    }
    catch (const std::exception& e) {
      LOGE << "IsotopicCorrections : " << typeid(e).name() << " : " << e.what();
    }

    LOGD << "END IsotopicCorrections";
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

    try {
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
    }
    catch (const std::exception& e) {
      LOGE << "CalculateIsotopicPurities : " << typeid(e).name() << " : " << e.what();
    }

    LOGD << "END CalculateIsotopicPurities";
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

    try {
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
    }
    catch (const std::exception& e) {
      LOGE << "CalculateMDVAccuracies : " << typeid(e).name() << " : " << e.what();
    }

    LOGD << "END CalculateMDVAccuracies";
  }

  ParameterSet PickMS2Features::getParameterSchema() const
  {
    OpenMS::MassTraceDetection mass_trace_detection;
    OpenMS::ElutionPeakDetection elution_peak_detection;
    OpenMS::FeatureFindingMetabo feature_finding_metabo;
    ParameterSet parameters({ mass_trace_detection, elution_peak_detection, feature_finding_metabo });
    std::map<std::string, std::vector<std::map<std::string, std::string>>> param_struct({
    {"PickMS2Features", {
    {
      {"name", "enable_elution"},
      {"type", "bool"},
      {"value", "true"},
      {"description", "Enable elution peak detection"},
    },
    {
      {"name", "force_processing"},
      {"type", "bool"},
      {"value", "false"},
      {"description", "To enforce processing of the data when Profile data is provided but spectra is not centroided"},
    },
    {
      {"name", "max_traces"},
      {"type", "int"},
      {"value", "0"},
      {"description", "Max traces to use for MassTraceDetection. 0 means all traces."},
    }
    }} });
    ParameterSet pick_ms2_feature_params(param_struct);
    parameters.merge(pick_ms2_feature_params);
    return parameters;
  }

  void PickMS2Features::process(
    RawDataHandler& rawDataHandler_IO,
    const ParameterSet& params_I,
    Filenames& filenames_I
  ) const
  {
    LOGD << "START PickMS2Features";
    getFilenames(filenames_I);

    // Complete user parameters with schema
    ParameterSet params(params_I);
    params.merge(getParameterSchema());

    //-------------------------------------------------------------
    // set parameters
    //-------------------------------------------------------------
    FunctionParameters pick_ms2_feature_params = params.at("PickMS2Features");

    OpenMS::MassTraceDetection mtdet;
    Utilities::setUserParameters(mtdet, params_I);

    OpenMS::ElutionPeakDetection epdet;
    Utilities::setUserParameters(epdet, params_I);

    OpenMS::FeatureFindingMetabo ffmet;
    Utilities::setUserParameters(ffmet, params_I);
    auto ffmet_parameters = ffmet.getParameters();

    //-------------------------------------------------------------
    // Setting input
    //-------------------------------------------------------------
    OpenMS::PeakMap ms_peakmap;
    for (OpenMS::MSSpectrum& spec : rawDataHandler_IO.getExperiment().getSpectra()) {
      // we want only MS1 specrtra
      if (spec.getMSLevel() == 1)
      {
        ms_peakmap.addSpectrum(spec);
      }
    }
    if (ms_peakmap.empty())
    {
      LOGW << "The given file does not contain any conventional peak data, but might"
        " contain chromatograms. This tool currently cannot handle them, sorry.";
      return;
    }

    // determine type of spectral data (profile or centroided)
    OpenMS::SpectrumSettings::SpectrumType spectrum_type = ms_peakmap[0].getType();

    if (spectrum_type == OpenMS::SpectrumSettings::PROFILE)
    {
      Parameter* force_processing = pick_ms2_feature_params.findParameter("force_processing");
      if (!force_processing || force_processing->getValueAsString() == "false")
      {
        LOGE << "Error: Profile data provided but centroided spectra expected. To enforce processing of the data set the force_processing parameter.";
        return;
      }
    }

    // make sure the spectra are sorted by m/z
    ms_peakmap.sortSpectra(true);

    std::vector<OpenMS::MassTrace> m_traces;

    //-------------------------------------------------------------
    // configure and run mass trace detection
    //-------------------------------------------------------------

    int max_traces = 0;
    Parameter* max_traces_param = pick_ms2_feature_params.findParameter("max_traces");
    if (max_traces_param)
    {
      try {
        max_traces = std::stoi(max_traces_param->getValueAsString());
      }
      catch (const std::exception& e)
      {
        LOGE << e.what();
        // keep it to 0
      }
    }

    mtdet.run(ms_peakmap, m_traces, max_traces);

    //-------------------------------------------------------------
    // configure and run elution peak detection
    //-------------------------------------------------------------
    std::vector<OpenMS::MassTrace> m_traces_final;
    Parameter* enable_elution = pick_ms2_feature_params.findParameter("enable_elution");
    if (enable_elution && enable_elution->getValueAsString() == "true")
    {
      std::vector<OpenMS::MassTrace> splitted_mtraces;
      // fill mass traces with smoothed data as well .. bad design..
      epdet.detectPeaks(m_traces, splitted_mtraces);
      if (epdet.getParameters().getValue("width_filtering") == "auto")
      {
        m_traces_final.clear();
        epdet.filterByPeakWidth(splitted_mtraces, m_traces_final);
      }
      else
      {
        m_traces_final = splitted_mtraces;
      }
    }
    else // no elution peak detection
    {
      m_traces_final = m_traces;    
      try {
        for (auto & trace: m_traces_final) // estimate FWHM, so .getIntensity() can be called later
        {
          trace.estimateFWHM(false);
        }
      }
      catch (const std::exception& e) {
        LOGE << e.what();
        return;
      }
      if (ffmet_parameters.getValue("use_smoothed_intensities").toBool())
      {
        LOGW << "Without EPD, smoothing is not supported. Setting 'use_smoothed_intensities' to false!";
        ffmet_parameters.setValue("use_smoothed_intensities", "false");
      }
    }

    //-------------------------------------------------------------
    // configure and run feature finding
    //-------------------------------------------------------------

    OpenMS::FeatureMap feat_map;
    std::vector< std::vector< OpenMS::MSChromatogram > > feat_chromatograms;
    ffmet.run(m_traces_final, feat_map, feat_chromatograms);

    size_t trace_count(0);
    for (const auto& feat : feat_map)
    {
      if (!feat.metaValueExists("num_of_masstraces"))
      {
        LOGE << "MetaValue 'num_of_masstraces' missing from FFMetabo output!";
        return;
      }
      trace_count += (size_t)feat.getMetaValue("num_of_masstraces");
    }
    if (trace_count != m_traces_final.size())
    {
      if (!ffmet_parameters.getValue("remove_single_traces").toBool())
      {
        LOGE << "FF-Metabo: Internal error. Not all mass traces have been assembled to features! Aborting.";
        return;
      }
      else
      {
        LOGI << "FF-Metabo: " << (m_traces_final.size() - trace_count) << " unassembled traces have been removed.";
      }
    }

    LOGI << "-- FF-Metabo stats --\n"
      << "Input traces:    " << m_traces_final.size() << "\n"
      << "Output features: " << feat_map.size() << " (total trace count: " << trace_count << ")";

    // merge chromatograms: convert vector of vector of chromatograms to vector of chromatograms;
    std::vector<OpenMS::Chromatogram> merged_chromatograms;
    for (auto& chromatogram_list : feat_chromatograms)
    {
      OpenMS::Chromatogram c = chromatogram_list[0];
      for (auto chromatogram_it = chromatogram_list.begin() + 1; chromatogram_it != chromatogram_list.end(); ++chromatogram_it)
      {
        for (OpenMS::ChromatogramPeak& peak : (*chromatogram_it))
        {
          c.push_back(peak);
        }
      }
      merged_chromatograms.push_back(c);
    }

    // filter features with zero intensity (this can happen if the FWHM is zero (bc of overly skewed shape) and no peaks end up being summed up)
    std::vector<bool> to_filter;
    auto intensity_zero = [&](OpenMS::Feature& f) { return f.getIntensity() == 0; };
    auto remove_it = remove_if(feat_map.begin(), feat_map.end(), intensity_zero);
    // clear corresponding chromatograms as well
    for (auto feat_map_it = remove_it; feat_map_it != feat_map.end(); ++feat_map_it)
    {
      auto chromatogram_id_match = [&](OpenMS::Chromatogram& c) {
        // extract feature_id from chromatogram native_id which is feature_id underscore index
        auto chromatogram_id = c.getNativeID();
        auto pos = chromatogram_id.find("_");
        if (pos != std::string::npos)
        {
          chromatogram_id = chromatogram_id.substr(0, pos);
        }
        return (chromatogram_id == OpenMS::String((*feat_map_it).getUniqueId()));
      };
      merged_chromatograms.erase(remove_if(merged_chromatograms.begin(), merged_chromatograms.end(), chromatogram_id_match), merged_chromatograms.end());
    }
    feat_map.erase(remove_it, feat_map.end());

    // store ionization mode of spectra (useful for post-processing by AccurateMassSearch tool)
    if (!feat_map.empty())
    {
      std::set<OpenMS::IonSource::Polarity> pols;
      for (const auto &peakmap: ms_peakmap)
      {
        pols.insert(peakmap.getInstrumentSettings().getPolarity());
      }
      // concat to single string
      OpenMS::StringList sl_pols;
      for (const auto& pol : pols)
      {
        sl_pols.push_back(OpenMS::String(OpenMS::IonSource::NamesOfPolarity[pol]));
      }
      feat_map[0].setMetaValue("scan_polarity", OpenMS::ListUtils::concatenate(sl_pols, ";"));
    }

    feat_map.setPrimaryMSRunPath({ rawDataHandler_IO.getMetaData().getFilename() });
    LOGD << "setPrimaryMSRunPath: " << rawDataHandler_IO.getMetaData().getFilename();

    rawDataHandler_IO.setFeatureMap(feat_map);
    rawDataHandler_IO.updateFeatureMapHistory();
    rawDataHandler_IO.getExperiment().setChromatograms(merged_chromatograms);

    LOGD << "END PickMS2Features";
  }

}
