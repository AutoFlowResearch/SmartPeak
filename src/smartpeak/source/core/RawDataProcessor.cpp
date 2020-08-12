// TODO: Add copyright

#include <SmartPeak/core/RawDataProcessor.h>
#include <SmartPeak/core/Filenames.h>
#include <SmartPeak/core/Utilities.h>

// load/store raw data
#include <OpenMS/FORMAT/FileTypes.h>
#include <OpenMS/FORMAT/ChromeleonFile.h>
#include <OpenMS/FORMAT/TraMLFile.h>  // load traML as well
#include <OpenMS/FORMAT/FileHandler.h>
#include <OpenMS/FORMAT/MzMLFile.h>
#include <OpenMS/FORMAT/MzTabFile.h>
#include <OpenMS/ANALYSIS/OPENSWATH/ChromatogramExtractor.h>
#include <OpenMS/ANALYSIS/TARGETED/MRMMapping.h>
#include <OpenMS/KERNEL/SpectrumHelper.h>

#include <OpenMS/FORMAT/MRMFeatureQCFile.h>  // load featureFilter and featureQC
#include <OpenMS/ANALYSIS/OPENSWATH/TransitionTSVFile.h>  // load traML
#include <OpenMS/FORMAT/FeatureXMLFile.h>  // load/store featureXML
#include <SmartPeak/io/InputDataValidation.h> // check filenames and headers

// load validation data and parameters
#include <SmartPeak/io/FileReader.h>
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

#include <algorithm>
#include <exception>

namespace SmartPeak
{
  void LoadRawData::process(
    RawDataHandler& rawDataHandler_IO,
    const std::map<std::string, std::vector<std::map<std::string, std::string>>>& params_I,
    const Filenames& filenames
  ) const
  {
    LOGD << "START loadMSExperiment";
    // # load chromatograms
    OpenMS::MSExperiment chromatograms;
    if (filenames.mzML_i.size()) {
      if (params_I.at("mzML").size()) {
        // # convert parameters
        std::map<std::string, CastValue> mzML_params;
        for (const std::map<std::string, std::string>& param : params_I.at("mzML")) {
          CastValue c;
          Utilities::castString(param.at("value"), param.at("type"), c);
          mzML_params.emplace(param.at("name"), c);
        }
        // Deal with ChromeleonFile format
        if (mzML_params.count("format") && mzML_params.at("format").s_ == "ChromeleonFile") {
          const size_t pos = filenames.mzML_i.rfind(".");
          std::string txt_name = filenames.mzML_i;
          if (pos != std::string::npos) {
            txt_name.replace(txt_name.cbegin() + pos + 1, txt_name.cend(), "txt"); // replace extension
          }
          OpenMS::ChromeleonFile chfh;
          LOGI << "Loading: " << txt_name;
          chfh.load(txt_name, chromatograms);
        }
        // Deal with .mzXML format
        else if (mzML_params.count("format") && mzML_params.at("format").s_ == "XML") {
          const size_t pos = filenames.mzML_i.rfind(".");
          std::string txt_name = filenames.mzML_i;
          if (pos != std::string::npos) {
            txt_name.replace(txt_name.cbegin() + pos + 1, txt_name.cend(), "xml"); // replace extension
          }
          OpenMS::FileHandler fh;
          LOGI << "Loading: " << txt_name;
          fh.loadExperiment(txt_name, chromatograms, OpenMS::FileTypes::MZXML);
        }
        else {
          OpenMS::FileHandler fh;
          LOGI << "Loading: " << filenames.mzML_i;
          fh.loadExperiment(filenames.mzML_i, chromatograms);
        }
      }
      else {
        OpenMS::FileHandler fh;
        LOGI << "Loading: " << filenames.mzML_i;
        fh.loadExperiment(filenames.mzML_i, chromatograms);
      }
    }

    OpenMS::TargetedExperiment& targeted_exp = rawDataHandler_IO.getTargetedExperiment();
    if (params_I.at("ChromatogramExtractor").size()) {
      // # convert parameters
      std::map<std::string, CastValue> chromatogramExtractor_params;
      for (const std::map<std::string, std::string>& param : params_I.at("ChromatogramExtractor")) {
        CastValue c;
        Utilities::castString(param.at("value"), param.at("type"), c);
        chromatogramExtractor_params.emplace(param.at("name"), c);
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
    else {
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

  void StoreRawData::process(
    RawDataHandler& rawDataHandler_IO,
    const std::map<std::string, std::vector<std::map<std::string, std::string>>>& params_I,
    const Filenames& filenames
  ) const
  {
    LOGD << "START storeMzML";
    LOGI << "Storing: " << filenames.mzML_i;

    if (filenames.mzML_i.empty()) {
      LOGE << "Filename is empty";
      LOGD << "END storeMzML";
      return;
    }

    try {
      OpenMS::MzMLFile mzmlfile;
      mzmlfile.store(filenames.mzML_i, rawDataHandler_IO.getChromatogramMap());
    }
    catch (const std::exception& e) {
      LOGE << e.what();
    }

    LOGD << "END storeMzML";
  }

  void LoadFeatures::process(
    RawDataHandler& rawDataHandler_IO,
    const std::map<std::string, std::vector<std::map<std::string, std::string>>>& params_I,
    const Filenames& filenames
  ) const
  {
    LOGD << "START LoadFeatures";
    LOGI << "Loading: " << filenames.featureXML_i;

    if (filenames.featureXML_i.empty()) {
      LOGE << "Filename is empty";
      LOGD << "END LoadFeatures";
      return;
    }

    if (!InputDataValidation::fileExists(filenames.featureXML_i)) {
      LOGE << "File not found";
      LOGD << "END LoadFeatures";
      return;
    }

    try {
      OpenMS::FeatureXMLFile featurexml;
      featurexml.load(filenames.featureXML_i, rawDataHandler_IO.getFeatureMap());
      rawDataHandler_IO.updateFeatureMapHistory();
    }
    catch (const std::exception& e) {
      LOGE << e.what();
      rawDataHandler_IO.getFeatureMap().clear();
      LOGE << "feature map clear";
    }

    LOGD << "END LoadFeatures";
  }

  void StoreFeatures::process(
    RawDataHandler& rawDataHandler_IO,
    const std::map<std::string, std::vector<std::map<std::string, std::string>>>& params_I,
    const Filenames& filenames
  ) const
  {
    LOGD << "START storeFeatureMap";
    LOGI << "Storing: " << filenames.featureXML_o;

    if (filenames.featureXML_o.empty()) {
      LOGE << "Filename is empty";
      LOGD << "END storeFeatureMap";
      return;
    }

    try {
      // Store outfile as featureXML
      OpenMS::FeatureXMLFile featurexml;
      featurexml.store(filenames.featureXML_o, rawDataHandler_IO.getFeatureMapHistory());
    }
    catch (const std::exception& e) {
      LOGE << e.what();
    }

    LOGD << "END storeFeatureMap";
  }

  void LoadAnnotations::process(
    RawDataHandler& rawDataHandler_IO,
    const std::map<std::string, std::vector<std::map<std::string, std::string>>>& params_I,
    const Filenames& filenames
  ) const
  {
    LOGD << "START LoadAnnotations";
    LOGI << "Loading: " << filenames.mzTab_i;

    if (filenames.mzTab_i.empty()) {
      LOGE << "Filename is empty";
      LOGD << "END LoadAnnotations";
      return;
    }

    if (!InputDataValidation::fileExists(filenames.mzTab_i)) {
      LOGE << "File not found";
      LOGD << "END LoadAnnotations";
      return;
    }

    try {
      OpenMS::MzTabFile mztabfile;
      mztabfile.load(filenames.mzTab_i, rawDataHandler_IO.getMzTab());
      rawDataHandler_IO.updateFeatureMapHistory();
    }
    catch (const std::exception& e) {
      LOGE << e.what();
      rawDataHandler_IO.setMzTab(OpenMS::MzTab());
      LOGE << "feature map clear";
    }

    LOGD << "END LoadAnnotations";
  }

  void StoreAnnotations::process(
    RawDataHandler& rawDataHandler_IO,
    const std::map<std::string, std::vector<std::map<std::string, std::string>>>& params_I,
    const Filenames& filenames
  ) const
  {
    LOGD << "START StoreAnnotations";
    LOGI << "Storing: " << filenames.mzTab_o;

    if (filenames.mzTab_o.empty()) {
      LOGE << "Filename is empty";
      LOGD << "END StoreAnnotations";
      return;
    }

    try {
      // Store outfile as mzTab
      OpenMS::MzTabFile mztabfile;
      mztabfile.store(filenames.mzTab_o, rawDataHandler_IO.getMzTab());
    }
    catch (const std::exception& e) {
      LOGE << e.what();
    }

    LOGD << "END StoreAnnotations";
  }

  void PickMRMFeatures::process(
    RawDataHandler& rawDataHandler_IO,
    const std::map<std::string, std::vector<std::map<std::string, std::string>>>& params_I,
    const Filenames& filenames
  ) const
  {
    LOGD << "START PickMRMFeatures";

    if (params_I.count("MRMFeatureFinderScoring") && params_I.at("MRMFeatureFinderScoring").empty()) {
      LOGE << "No parameters passed to PickMRMFeatures. Not picking";
      LOGD << "END pickFeatures";
      return;
    }

    OpenMS::MRMFeatureFinderScoring featureFinder;
    OpenMS::Param parameters = featureFinder.getParameters();
    Utilities::updateParameters(parameters, params_I.at("MRMFeatureFinderScoring"));
    featureFinder.setParameters(parameters);

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

  void FilterFeatures::process(
    RawDataHandler& rawDataHandler_IO,
    const std::map<std::string, std::vector<std::map<std::string, std::string>>>& params_I,
    const Filenames& filenames
  ) const
  {
    LOGD << "START filterFeatures";
    LOGI << "Feature Filter input size: " << rawDataHandler_IO.getFeatureMap().size();

    if (params_I.count("MRMFeatureFilter.filter_MRMFeatures") &&
        params_I.at("MRMFeatureFilter.filter_MRMFeatures").empty()) {
      LOGE << "No parameters passed to filterFeatures. Not filtering";
      LOGD << "END filterFeatures";
      return;
    }

    OpenMS::MRMFeatureFilter featureFilter;
    OpenMS::Param parameters = featureFilter.getParameters();
    Utilities::updateParameters(parameters, params_I.at("MRMFeatureFilter.filter_MRMFeatures"));
    featureFilter.setParameters(parameters);

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

  void CheckFeatures::process(
    RawDataHandler& rawDataHandler_IO,
    const std::map<std::string, std::vector<std::map<std::string, std::string>>>& params_I,
    const Filenames& filenames
  ) const
  {
    LOGD << "START checkFeatures";
    LOGI << "Feature Checker input size: " << rawDataHandler_IO.getFeatureMap().size();

    if (params_I.count("MRMFeatureFilter.filter_MRMFeatures.qc") &&
        params_I.at("MRMFeatureFilter.filter_MRMFeatures.qc").empty()) {
      LOGE << "No parameters passed to checkFeatures. Not checking";
      LOGD << "END checkFeatures";
      return;
    }

    OpenMS::MRMFeatureFilter featureFilter;
    OpenMS::Param parameters = featureFilter.getParameters();
    Utilities::updateParameters(parameters, params_I.at("MRMFeatureFilter.filter_MRMFeatures.qc"));
    featureFilter.setParameters(parameters);

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
    const std::map<std::string, std::vector<std::map<std::string, std::string>>>& params_I,
    const Filenames& filenames
  ) const
  {
    LOGD << "START selectFeatures";
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

    if (params_I.count("MRMFeatureSelector.schedule_MRMFeatures_qmip")) {
      std::vector<OpenMS::MRMFeatureSelector::SelectorParameters> p =
        Utilities::extractSelectorParameters(params_I.at("MRMFeatureSelector.schedule_MRMFeatures_qmip"), params_I.at("MRMFeatureSelector.select_MRMFeatures_qmip"));
      OpenMS::MRMBatchFeatureSelector::batchMRMFeaturesQMIP(rawDataHandler_IO.getFeatureMap(), output, p);
    } else if (params_I.count("MRMFeatureSelector.schedule_MRMFeatures_score")) {
      std::vector<OpenMS::MRMFeatureSelector::SelectorParameters> p =
        Utilities::extractSelectorParameters(params_I.at("MRMFeatureSelector.schedule_MRMFeatures_score"), params_I.at("MRMFeatureSelector.select_MRMFeatures_score"));
      OpenMS::MRMBatchFeatureSelector::batchMRMFeaturesScore(rawDataHandler_IO.getFeatureMap(), output, p);
    } else {
      LOGE << "Both arguments 'select params' and 'schedule params' are empty";
      LOGD << "END selectFeatures";
      // TODO: replace throw with return?
      throw std::invalid_argument("Both arguments 'select params' and 'schedule params' are empty.");
    }

    output.setPrimaryMSRunPath({rawDataHandler_IO.getMetaData().getFilename()});

    LOGV << "setPrimaryMSRunPath: " << rawDataHandler_IO.getMetaData().getFilename();

    rawDataHandler_IO.setFeatureMap(output);
    rawDataHandler_IO.updateFeatureMapHistory();

    LOGI << "selectFeatures output size: " << output.size();
    LOGD << "END selectFeatures";
  }

  void ValidateFeatures::process(
    RawDataHandler& rawDataHandler_IO,
    const std::map<std::string, std::vector<std::map<std::string, std::string>>>& params_I,
    const Filenames& filenames
  ) const
  {
    LOGD << "START validateFeatures";

    if (params_I.count("MRMFeatureValidator.validate_MRMFeatures") &&
        params_I.at("MRMFeatureValidator.validate_MRMFeatures").empty()) {
      LOGE << "No parameters passed to validateFeatures. Not validating";
      LOGD << "END validateFeatures";
      return;
    }

    OpenMS::FeatureMap mapped_features;
    std::map<std::string, float> validation_metrics; // keys: accuracy, recall, precision

    MRMFeatureValidator::validate_MRMFeatures(
      rawDataHandler_IO.getReferenceData(),
      rawDataHandler_IO.getFeatureMap(),
      rawDataHandler_IO.getMetaData().getInjectionName(),
      mapped_features,
      validation_metrics,
      std::stof(params_I.at("MRMFeatureValidator.validate_MRMFeatures").front().at("value"))
      // TODO: While this probably works, it might be nice to add some check that the parameter passed is the desired one
    );

    rawDataHandler_IO.setFeatureMap(mapped_features);
    rawDataHandler_IO.setValidationMetrics(validation_metrics);

    LOGD << "END validateFeatures";
  }

  void PlotFeatures::process(
    RawDataHandler& rawDataHandler_IO,
    const std::map<std::string, std::vector<std::map<std::string, std::string>>>& params_I,
    const Filenames& filenames
  ) const
  {
    LOGD << "START PlotFeatures (NOT IMPLEMENTED)";
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

  void QuantifyFeatures::process(
    RawDataHandler& rawDataHandler_IO,
    const std::map<std::string, std::vector<std::map<std::string, std::string>>>& params_I,
    const Filenames& filenames
  ) const
  {
    LOGD << "START quantifyComponents";
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

  void LoadTransitions::process(
    RawDataHandler& rawDataHandler_IO,
    const std::map<std::string, std::vector<std::map<std::string, std::string>>>& params_I,
    const Filenames& filenames
  ) const
  {
    // TODO: move to parameters at some point
    std::string format = "csv";
    LOGD << "START loadTraML";
    LOGI << "Loading " << filenames.traML_csv_i;
    LOGI << "Format: " << format;

    if (filenames.traML_csv_i.empty()) {
      LOGE << "Filename is empty";
      LOGD << "END loadTraML";
      return;
    }

    if (!InputDataValidation::fileExists(filenames.traML_csv_i)) {
      LOGE << "File not found";
      LOGD << "END loadTraML";
      return;
    }

    try {
      // must use "PeptideSequence"
      if (format == "csv") {
        // Transitions are appended to the existing experiment in OpenMS
        rawDataHandler_IO.getTargetedExperiment().clear(true);
        OpenMS::TransitionTSVFile tsvfile;
        tsvfile.convertTSVToTargetedExperiment(
          filenames.traML_csv_i.c_str(),
          OpenMS::FileTypes::TRAML,
          rawDataHandler_IO.getTargetedExperiment()
        );
      }
      else if (format == "traML") {
        // Transitions are appended to the existing experiment in OpenMS
        rawDataHandler_IO.getTargetedExperiment().clear(true);
        OpenMS::TraMLFile tramlfile;
        tramlfile.load(filenames.traML_csv_i, rawDataHandler_IO.getTargetedExperiment());
      }
      else {
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

  void LoadFeatureFiltersRDP::process(
    RawDataHandler& rawDataHandler_IO,
    const std::map<std::string, std::vector<std::map<std::string, std::string>>>& params_I,
    const Filenames& filenames
  ) const
  {
    LOGD << "START loadFeatureFilter";
    LOGI << "Loading: " << filenames.featureFilterComponents_csv_i << " and " <<
      filenames.featureFilterComponentGroups_csv_i;

    if (filenames.featureFilterComponents_csv_i.empty() &&
        filenames.featureFilterComponentGroups_csv_i.empty()) {
      LOGE << "Filenames are both empty";
      LOGD << "END loadFeatureFilter";
      return;
    }

    if (filenames.featureFilterComponents_csv_i.size() &&
        !InputDataValidation::fileExists(filenames.featureFilterComponents_csv_i)) {
      LOGE << "File not found: " << filenames.featureFilterComponents_csv_i;
      LOGD << "END loadFeatureFilter";
      return;
    }

    if (filenames.featureFilterComponentGroups_csv_i.size() &&
        !InputDataValidation::fileExists(filenames.featureFilterComponentGroups_csv_i)) {
      LOGE << "File not found: " << filenames.featureFilterComponentGroups_csv_i;
      LOGD << "END loadFeatureFilter";
      return;
    }

    try {
      OpenMS::MRMFeatureQCFile featureQCFile;
      if (filenames.featureFilterComponents_csv_i.size()) { // because we don't know if either of the two names is empty
        featureQCFile.load(filenames.featureFilterComponents_csv_i, rawDataHandler_IO.getFeatureFilter(), false);
      }
      if (filenames.featureFilterComponentGroups_csv_i.size()) {
        featureQCFile.load(filenames.featureFilterComponentGroups_csv_i, rawDataHandler_IO.getFeatureFilter(), true);
      }
    }
    catch (const std::exception& e) {
      LOGE << e.what();
      rawDataHandler_IO.getFeatureFilter().component_qcs.clear();
      rawDataHandler_IO.getFeatureFilter().component_group_qcs.clear();
      rawDataHandler_IO.getFeatureFilter().component_group_pair_qcs.clear();
      LOGI << "feature filter clear";
    }

    LOGD << "END loadFeatureFilter";
  }

  void LoadFeatureQCsRDP::process(
    RawDataHandler& rawDataHandler_IO,
    const std::map<std::string, std::vector<std::map<std::string, std::string>>>& params_I,
    const Filenames& filenames
  ) const
  {
    LOGD << "START loadFeatureQC";
    LOGI << "Loading: " << filenames.featureQCComponents_csv_i << " and " <<
      filenames.featureQCComponentGroups_csv_i;

    if (filenames.featureQCComponents_csv_i.empty() &&
        filenames.featureQCComponentGroups_csv_i.empty()) {
      LOGE << "Filenames are both empty";
      LOGD << "END loadFeatureQC";
      return;
    }

    if (filenames.featureQCComponents_csv_i.size() &&
        !InputDataValidation::fileExists(filenames.featureQCComponents_csv_i)) {
      LOGE << "File not found: " << filenames.featureQCComponents_csv_i;
      LOGD << "END loadFeatureQC";
      return;
    }

    if (filenames.featureQCComponentGroups_csv_i.size() &&
        !InputDataValidation::fileExists(filenames.featureQCComponentGroups_csv_i)) {
      LOGE << "File not found: " << filenames.featureQCComponentGroups_csv_i;
      LOGD << "END loadFeatureQC";
      return;
    }

    try {
      OpenMS::MRMFeatureQCFile featureQCFile;
      if (filenames.featureQCComponents_csv_i.size()) { // because we don't know if either of the two names is empty
        featureQCFile.load(filenames.featureQCComponents_csv_i, rawDataHandler_IO.getFeatureQC(), false);
      }
      if (filenames.featureQCComponentGroups_csv_i.size()) {
        featureQCFile.load(filenames.featureQCComponentGroups_csv_i, rawDataHandler_IO.getFeatureQC(), true);
      }
    }
    catch (const std::exception& e) {
      LOGE << e.what();
      rawDataHandler_IO.getFeatureQC().component_qcs.clear();
      rawDataHandler_IO.getFeatureQC().component_group_qcs.clear();
      rawDataHandler_IO.getFeatureQC().component_group_pair_qcs.clear();
      LOGI << "Feature qc clear";
    }

    LOGD << "END loadFeatureQC";
  }

  void StoreFeatureFiltersRDP::process(
    RawDataHandler& rawDataHandler_IO,
    const std::map<std::string, std::vector<std::map<std::string, std::string>>>& params_I,
    const Filenames& filenames
  ) const
  {
    LOGD << "START storeFeatureFilter";
    LOGI << "Storing: " << filenames.featureFilterComponents_csv_i << " and " <<
      filenames.featureFilterComponentGroups_csv_i;

    if (filenames.featureFilterComponents_csv_i.empty() &&
      filenames.featureFilterComponentGroups_csv_i.empty()) {
      LOGE << "Filenames are both empty";
      LOGD << "END storeFeatureFilter";
      return;
    }

    try {
      OpenMS::MRMFeatureQCFile featureQCFile;
      if (filenames.featureFilterComponents_csv_i.size()) { // because we don't know if either of the two names is empty
        featureQCFile.store(filenames.featureFilterComponents_csv_i, rawDataHandler_IO.getFeatureFilter(), false);
      }
      if (filenames.featureFilterComponentGroups_csv_i.size()) {
        featureQCFile.store(filenames.featureFilterComponentGroups_csv_i, rawDataHandler_IO.getFeatureFilter(), true);
      }
    }
    catch (const std::exception& e) {
      LOGE << e.what();
      LOGI << "feature filter store exception";
    }

    LOGD << "END storeFeatureFilter";
  }

  void StoreFeatureQCsRDP::process(
    RawDataHandler& rawDataHandler_IO,
    const std::map<std::string, std::vector<std::map<std::string, std::string>>>& params_I,
    const Filenames& filenames
  ) const
  {
    LOGD << "START storeFeatureQC";
    LOGI << "Loading: " << filenames.featureQCComponents_csv_i << " and " <<
      filenames.featureQCComponentGroups_csv_i;

    if (filenames.featureQCComponents_csv_i.empty() &&
      filenames.featureQCComponentGroups_csv_i.empty()) {
      LOGE << "Filenames are both empty";
      LOGD << "END storeFeatureQC";
      return;
    }

    try {
      OpenMS::MRMFeatureQCFile featureQCFile;
      if (filenames.featureQCComponents_csv_i.size()) { // because we don't know if either of the two names is empty
        featureQCFile.store(filenames.featureQCComponents_csv_i, rawDataHandler_IO.getFeatureQC(), false);
      }
      if (filenames.featureQCComponentGroups_csv_i.size()) {
        featureQCFile.store(filenames.featureQCComponentGroups_csv_i, rawDataHandler_IO.getFeatureQC(), true);
      }
    }
    catch (const std::exception& e) {
      LOGE << e.what();
      LOGI << "Feature qc store exception";
    }

    LOGD << "END storeFeatureQC";
  }

  void LoadValidationData::process(
    RawDataHandler& rawDataHandler_IO,
    const std::map<std::string, std::vector<std::map<std::string, std::string>>>& params_I,
    const Filenames& filenames
  ) const
  {
    LOGD << "START loadValidationData";
    LOGI << "Loading: " << filenames.referenceData_csv_i;

    if (filenames.referenceData_csv_i.empty()) {
      LOGE << "Filename is empty";
      LOGD << "END loadValidationData";
      return;
    }

    if (!InputDataValidation::fileExists(filenames.referenceData_csv_i)) {
      LOGE << "File not found";
      LOGD << "END loadValidationData";
      return;
    }

    io::CSVReader<17, io::trim_chars<>, io::no_quote_escape<','>> in(filenames.referenceData_csv_i);

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
      mdh.sample_name = sample_name;
      mdh.inj_number = sample_index;
      mdh.batch_name = experiment_id;
      mdh.setAcquisitionDateAndTimeFromString(acquisition_date_and_time, "%m-%d-%Y %H:%M");
      m.emplace("injection_name", mdh.getInjectionName());
      reference_data.push_back(std::move(m));
    }

    rawDataHandler_IO.setReferenceData(reference_data);

    LOGD << "END loadValidationData";
  }

  void LoadParameters::process(
    RawDataHandler& rawDataHandler_IO,
    const std::map<std::string, std::vector<std::map<std::string, std::string>>>& params_I,
    const Filenames& filenames
  ) const
  {
    LOGD << "START readRawDataProcessingParameters";
    LOGI << "Loading: " << filenames.parameters_csv_i;

    if (filenames.parameters_csv_i.empty()) {
      LOGE << "Filename is empty";
      LOGD << "END readRawDataProcessingParameters";
      return;
    }

    if (!InputDataValidation::fileExists(filenames.parameters_csv_i)) {
      LOGE << "File not found";
      LOGD << "END readRawDataProcessingParameters";
      return;
    }

    try {
      FileReader::parseOpenMSParams(filenames.parameters_csv_i, rawDataHandler_IO.getParameters());
      sanitizeParameters(rawDataHandler_IO.getParameters());
    }
    catch (const std::exception& e) {
      LOGE << e.what();
    }

    LOGD << "END readRawDataProcessingParameters";
  }

  void LoadParameters::sanitizeParameters(
    std::map<std::string, std::vector<std::map<std::string, std::string>>>& params_I
  )
  {
    LOGD << "START sanitizeRawDataProcessorParameters";

    // # check for workflow parameters integrity
    const std::vector<std::string> required_parameters = {
      "SequenceSegmentPlotter",
      "FeaturePlotter",
      "AbsoluteQuantitation",
      "mzML",
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
      "AccurateMassSearchEngine"
    };
    for (const std::string& parameter : required_parameters) {
      if (!params_I.count(parameter)) {
        params_I.emplace(
          parameter,
          std::vector<std::map<std::string, std::string>>() // empty vector
        );
      }
    }

    LOGD << "END sanitizeRawDataProcessorParameters";
  }

  void ZeroChromatogramBaseline::process(
    RawDataHandler& rawDataHandler_IO,
    const std::map<std::string, std::vector<std::map<std::string, std::string>>>& params_I,
    const Filenames & filenames
  ) const
  {
    LOGD << "START ZeroChromatogramBaseline";

    std::vector<OpenMS::MSChromatogram>& chroms = rawDataHandler_IO.getChromatogramMap().getChromatograms();
    for (OpenMS::MSChromatogram& ch : chroms) {
      OpenMS::subtractMinimumIntensity(ch);
    }

    LOGD << "END ZeroChromatogramBaseline";
  }

  void MapChromatograms::process(
    RawDataHandler& rawDataHandler_IO,
    const std::map<std::string, std::vector<std::map<std::string, std::string>>>& params_I,
    const Filenames& filenames
  ) const
  {
    LOGD << "START MapChromatograms";

    if (params_I.count("MRMMapping") && params_I.at("MRMMapping").empty()) {
      LOGE << "No parameters passed to MRMMapping. No transition mapping will be done";
      LOGD << "END MapChromatograms";
      return;
    }

    // Set up MRMMapping and parse the MRMMapping params
    OpenMS::MRMMapping mrmmapper;
    OpenMS::Param parameters = mrmmapper.getParameters();
    Utilities::updateParameters(parameters, params_I.at("MRMMapping"));
    mrmmapper.setParameters(parameters);

    mrmmapper.mapExperiment(
      rawDataHandler_IO.getExperiment(),
      rawDataHandler_IO.getTargetedExperiment(),
      rawDataHandler_IO.getChromatogramMap()
    );

    LOGD << "END MapChromatograms";
  }

  void ExtractChromatogramWindows::process(
    RawDataHandler& rawDataHandler_IO,
    const std::map<std::string, std::vector<std::map<std::string, std::string>>>& params_I,
    const Filenames& filenames
  ) const
  {
    LOGD << "START ExtractChromatogramWindows";

    for (const OpenMS::MRMFeatureQC::ComponentQCs& transition_filters : rawDataHandler_IO.getFeatureFilter().component_qcs) {
      for (OpenMS::MSChromatogram& ch : rawDataHandler_IO.getChromatogramMap().getChromatograms()) {
        if (transition_filters.component_name == ch.getNativeID()) {
          OpenMS::removePeaks(ch, transition_filters.retention_time_l, transition_filters.retention_time_u);
        }
      }
    }

    LOGD << "END ExtractChromatogramWindows";
  }

  void ExtractSpectraWindows::process(RawDataHandler& rawDataHandler_IO, const std::map<std::string, std::vector<std::map<std::string, std::string>>>& params_I, const Filenames& filenames) const
  {
    LOGD << "START ExtractSpectraWindows";

    float start = 0, stop = 0;
    if (params_I.count("FIAMS") && params_I.at("FIAMS").size()){
      for (const auto& fia_params: params_I.at("FIAMS")){
        if (fia_params.at("name") == "acquisition_start") {
          try {
            start = std::stof(fia_params.at("value"));
          }
          catch (const std::exception& e) {
            LOGE << e.what();
          }
        }
        if (fia_params.at("name") == "acquisition_end") {
          try {
            stop = std::stof(fia_params.at("value"));
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
    rawDataHandler_IO.getExperiment().setSpectra(output);

    LOGD << "END ExtractSpectraWindows";
  }

  void FitFeaturesEMG::process(
    RawDataHandler& rawDataHandler_IO,
    const std::map<std::string, std::vector<std::map<std::string, std::string>>>& params_I,
    const Filenames& filenames
  ) const
  {
    LOGD << "START FitFeaturesEMG";

    OpenMS::EmgGradientDescent emg;

    if (params_I.count("EmgGradientDescent") && params_I.at("EmgGradientDescent").size()) {
      OpenMS::Param parameters = emg.getParameters();
      Utilities::updateParameters(parameters, params_I.at("EmgGradientDescent"));
      emg.setParameters(parameters);
    }

    // TODO: Remove these lines after testing/debugging is done
    OpenMS::Param parameters = emg.getParameters();
    parameters.setValue("print_debug", 1);
    parameters.setValue("max_gd_iter", 10000u);
    emg.setParameters(parameters);

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

  void FilterFeaturesRSDs::process(
    RawDataHandler& rawDataHandler_IO,
    const std::map<std::string, std::vector<std::map<std::string, std::string>>>& params_I,
    const Filenames& filenames
  ) const
  {
    LOGD << "START filterFeaturesRSDs";
    LOGI << "Feature Filter input size: " << rawDataHandler_IO.getFeatureMap().size();

    if (params_I.count("MRMFeatureFilter.filter_MRMFeaturesRSDs") &&
      params_I.at("MRMFeatureFilter.filter_MRMFeaturesRSDs").empty()) {
      LOGE << "No parameters passed to filterFeatures. Not filtering";
      LOGD << "END filterFeaturesRSDs";
      return;
    }

    OpenMS::MRMFeatureFilter featureFilter;
    OpenMS::Param parameters = featureFilter.getParameters();
    Utilities::updateParameters(parameters, params_I.at("MRMFeatureFilter.filter_MRMFeaturesRSDs"));
    featureFilter.setParameters(parameters);

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

  void CheckFeaturesRSDs::process(
    RawDataHandler& rawDataHandler_IO,
    const std::map<std::string, std::vector<std::map<std::string, std::string>>>& params_I,
    const Filenames& filenames
  ) const
  {
    LOGD << "START checkFeaturesRSDs";
    LOGI << "Feature Checker input size: " << rawDataHandler_IO.getFeatureMap().size();

    if (params_I.count("MRMFeatureFilter.filter_MRMFeaturesRSDs.qc") &&
      params_I.at("MRMFeatureFilter.filter_MRMFeaturesRSDs.qc").empty()) {
      LOGE << "No parameters passed to checkFeatures. Not checking";
      LOGD << "END checkFeaturesRSDs";
      return;
    }

    OpenMS::MRMFeatureFilter featureFilter;
    OpenMS::Param parameters = featureFilter.getParameters();
    Utilities::updateParameters(parameters, params_I.at("MRMFeatureFilter.filter_MRMFeaturesRSDs.qc"));
    featureFilter.setParameters(parameters);

    featureFilter.FilterFeatureMapPercRSD(
      rawDataHandler_IO.getFeatureMap(),
      rawDataHandler_IO.getFeatureRSDQC(),
      rawDataHandler_IO.getFeatureRSDEstimations()
    );

    rawDataHandler_IO.updateFeatureMapHistory();

    LOGI << "Feature Checker output size: " << rawDataHandler_IO.getFeatureMap().size();
    LOGD << "END checkFeaturesRSDs";
  }

  void FilterFeaturesBackgroundInterferences::process(
    RawDataHandler& rawDataHandler_IO,
    const std::map<std::string, std::vector<std::map<std::string, std::string>>>& params_I,
    const Filenames& filenames
  ) const
  {
    LOGD << "START filterFeaturesBackgroundInterferences";
    LOGI << "Feature Filter input size: " << rawDataHandler_IO.getFeatureMap().size();

    if (params_I.count("MRMFeatureFilter.filter_MRMFeaturesBackgroundInterferences") &&
      params_I.at("MRMFeatureFilter.filter_MRMFeaturesBackgroundInterferences").empty()) {
      LOGE << "No parameters passed to filterFeatures. Not filtering";
      LOGD << "END filterFeaturesBackgroundInterferences";
      return;
    }

    OpenMS::MRMFeatureFilter featureFilter;
    OpenMS::Param parameters = featureFilter.getParameters();
    Utilities::updateParameters(parameters, params_I.at("MRMFeatureFilter.filter_MRMFeaturesBackgroundInterferences"));
    featureFilter.setParameters(parameters);

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

  void CheckFeaturesBackgroundInterferences::process(
    RawDataHandler& rawDataHandler_IO,
    const std::map<std::string, std::vector<std::map<std::string, std::string>>>& params_I,
    const Filenames& filenames
  ) const
  {
    LOGD << "START checkFeaturesBackgroundInterferences";
    LOGI << "Feature Checker input size: " << rawDataHandler_IO.getFeatureMap().size();

    if (params_I.count("MRMFeatureFilter.filter_MRMFeaturesBackgroundInterferences.qc") &&
      params_I.at("MRMFeatureFilter.filter_MRMFeaturesBackgroundInterferences.qc").empty()) {
      LOGE << "No parameters passed to checkFeatures. Not checking";
      LOGD << "END checkFeaturesBackgroundInterferences";
      return;
    }

    OpenMS::MRMFeatureFilter featureFilter;
    OpenMS::Param parameters = featureFilter.getParameters();
    Utilities::updateParameters(parameters, params_I.at("MRMFeatureFilter.filter_MRMFeaturesBackgroundInterferences.qc"));
    featureFilter.setParameters(parameters);

    featureFilter.FilterFeatureMapBackgroundInterference(
      rawDataHandler_IO.getFeatureMap(),
      rawDataHandler_IO.getFeatureBackgroundQC(),
      rawDataHandler_IO.getFeatureBackgroundEstimations()
    );

    rawDataHandler_IO.updateFeatureMapHistory();

    LOGI << "Feature Checker output size: " << rawDataHandler_IO.getFeatureMap().size();
    LOGD << "END checkFeaturesBackgroundInterferences";
  }

  void MergeSpectra::process(RawDataHandler& rawDataHandler_IO, const std::map<std::string, std::vector<std::map<std::string, std::string>>>& params_I, const Filenames& filenames) const
  {
    LOGD << "START MergeSpectra";

    float resolution = 0, max_mz = 0, bin_step = 0;
    if (params_I.count("FIAMS") && params_I.at("FIAMS").size()) {
      for (const auto& fia_params : params_I.at("FIAMS")) {
        if (fia_params.at("name") == "max_mz") {
          try {
            max_mz = std::stof(fia_params.at("value"));
          }
          catch (const std::exception& e) {
            LOGE << e.what();
          }
        }
        if (fia_params.at("name") == "bin_step") {
          try {
            bin_step = std::stof(fia_params.at("value"));
          }
          catch (const std::exception& e) {
            LOGE << e.what();
          }
        }
        if (fia_params.at("name") == "resolution") {
          try {
            resolution = std::stof(fia_params.at("value"));
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
    rawDataHandler_IO.getExperiment().setSpectra({ output });

    LOGD << "END MergeSpectra";
  }

  void PickMS1Features::process(RawDataHandler& rawDataHandler_IO, const std::map<std::string, std::vector<std::map<std::string, std::string>>>& params_I, const Filenames& filenames) const
  {
    LOGD << "START PickMS1Features";

    if (params_I.count("PickMS1Features") && params_I.at("PickMS1Features").empty()) {
      LOGE << "No parameters passed to PickMS1Features. Not picking";
      LOGD << "END PickMS1Features";
      return;
    }

    double sn_window = 0;
    //bool write_convex_hull = false;
    for (const auto& fia_params : params_I.at("PickMS1Features")) {
      if (fia_params.at("name") == "sne:window") {
        try {
          sn_window = std::stod(fia_params.at("value"));
        }
        catch (const std::exception& e) {
          LOGE << e.what();
        }
      }
      //if (fia_params.at("name") == "write_convex_hull") {
      //  try {
      //    std::string value = fia_params.at("value");
      //    std::transform(value.begin(), value.end(), value.begin(), ::tolower);
      //    write_convex_hull = (value == "true")?true:false;
      //  }
      //  catch (const std::exception& e) {
      //    LOGE << e.what();
      //  }
      //}
    }
    if (sn_window == 0) {
      LOGE << "Missing sne:window parameter for PickMS1Features. Not picking";
      LOGD << "END PickMS1Features";
      return;
    }

    OpenMS::SavitzkyGolayFilter sgfilter;
    OpenMS::Param parameters = sgfilter.getParameters();
    Utilities::updateParameters(parameters, params_I.at("PickMS1Features"));
    sgfilter.setParameters(parameters);

    OpenMS::PeakPickerHiRes picker;
    parameters = picker.getParameters();
    Utilities::updateParameters(parameters, params_I.at("PickMS1Features"));
    picker.setParameters(parameters);

    OpenMS::FeatureMap featureMap;
    try {
      for (const OpenMS::MSSpectrum& spec : rawDataHandler_IO.getExperiment().getSpectra()) {
        // Smooth and pick
        OpenMS::MSSpectrum input(spec);
        sgfilter.filter(input);
        OpenMS::MSSpectrum output;
        picker.pick(input, output);

        if (output.size() > 0) {
          // Estimate the S/N
          OpenMS::SignalToNoiseEstimatorMedianRapid sne(sn_window);
          std::vector<double> mzs, intensities;
          mzs.reserve(output.size());
          intensities.reserve(output.size());
          for (auto it = output.begin(); it != output.end(); ++it)
          {
            mzs.push_back(it->getMZ());
            intensities.push_back(it->getIntensity());
          }
          OpenMS::SignalToNoiseEstimatorMedianRapid::NoiseEstimator e = sne.estimateNoise(mzs, intensities);

          // Create the featureMap
          for (auto it = output.begin(); it != output.end(); ++it)
          {
            OpenMS::Feature f;
            f.setUniqueId();
            f.setIntensity(it->getIntensity());
            f.setMZ(it->getMZ());
            f.setRT(0);
            f.setMetaValue("scan_polarity", rawDataHandler_IO.getMetaData().scan_polarity);
            f.setMetaValue("peak_apex_int", it->getIntensity());
            f.setMetaValue("signal_to_noise", e.get_noise_value(it->getMZ()));
            // TODO: convex hull points
            featureMap.push_back(f);
          }
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

  void SearchAccurateMass::process(RawDataHandler& rawDataHandler_IO, const std::map<std::string, std::vector<std::map<std::string, std::string>>>& params_I, const Filenames& filenames) const
  {
    LOGD << "START SearchAccurateMass";

    if (params_I.count("AccurateMassSearchEngine") && params_I.at("AccurateMassSearchEngine").empty()) {
      LOGE << "No parameters passed to AccurateMassSearchEngine. Not searching.";
      LOGD << "END SearchAccurateMass";
      return;
    }

    OpenMS::AccurateMassSearchEngine ams;
    OpenMS::Param parameters = ams.getParameters();
    Utilities::updateParameters(parameters, params_I.at("AccurateMassSearchEngine"));
    ams.setParameters(parameters);

    OpenMS::MzTab output;
    try {
      ams.init();
      ams.run(rawDataHandler_IO.getFeatureMap(), output);
    }
    catch (const std::exception& e) {
      LOGE << e.what();
    }
    rawDataHandler_IO.setMzTab(output);

    LOGD << "END SearchAccurateMass";
  }
}
