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

// feature selection
#include <OpenMS/ANALYSIS/OPENSWATH/MRMFeatureSelector.h>
#include <OpenMS/ANALYSIS/OPENSWATH/MRMBatchFeatureSelector.h>

#include <SmartPeak/algorithm/MRMFeatureValidator.h>  // feature validaiton
#include <OpenMS/ANALYSIS/OPENSWATH/MRMFeatureFilter.h>  // feature filter/QC
#include <OpenMS/ANALYSIS/OPENSWATH/MRMFeatureFinderScoring.h>  // feature picker
#include <OpenMS/ANALYSIS/QUANTITATION/AbsoluteQuantitation.h> // feature quantification

namespace SmartPeak
{
  void LoadRawData::process(
    RawDataHandler& rawDataHandler_IO,
    const std::map<std::string, std::vector<std::map<std::string, std::string>>>& params_I,
    const Filenames& filenames,
    const bool verbose_I
  ) const {
    if (verbose_I) {
      std::cout << "==== START loadMSExperiment" << std::endl;
    }

    // # load chromatograms
    OpenMS::MSExperiment chromatograms;
    if (filenames.mzML_i.size()) {
      if (params_I.at("mzML").size()) {
        // # convert parameters
        std::map<std::string, Utilities::CastValue> mzML_params;
        for (const std::map<std::string, std::string>& param : params_I.at("mzML")) {
          Utilities::CastValue c;
          Utilities::castString(param.at("value"), param.at("type"), c);
          mzML_params.emplace(param.at("name"), c);
        }
        if (mzML_params.count("format") && mzML_params.at("format").s_ == "ChromeleonFile") {
          const size_t pos = filenames.mzML_i.rfind(".");
          std::string txt_name = filenames.mzML_i;
          if (pos != std::string::npos) {
            txt_name.replace(txt_name.cbegin() + pos + 1, txt_name.cend(), "txt"); // replace extension
          }
          OpenMS::ChromeleonFile chfh;
          std::cout << "loadMSExperiment(): loading " << txt_name << std::endl;
          chfh.load(txt_name, chromatograms);
        }
        else {
          OpenMS::FileHandler fh;
          std::cout << "loadMSExperiment(): loading " << filenames.mzML_i << std::endl;
          fh.loadExperiment(filenames.mzML_i, chromatograms);
        }
        if (mzML_params.count("zero_baseline") && mzML_params.at("zero_baseline").b_) {
          std::vector<OpenMS::MSChromatogram>& chroms = chromatograms.getChromatograms();
          for (OpenMS::MSChromatogram& ch : chroms) {
            OpenMS::subtractMinimumIntensity(ch);
          }
        }
      }
      else {
        OpenMS::FileHandler fh;
        std::cout << "loadMSExperiment(): loading " << filenames.mzML_i << std::endl;
        fh.loadExperiment(filenames.mzML_i, chromatograms);
      }
    }

    OpenMS::TargetedExperiment& targeted_exp = rawDataHandler_IO.getTargetedExperiment();
    if (params_I.at("ChromatogramExtractor").size()) {
      // # convert parameters
      std::map<std::string, Utilities::CastValue> chromatogramExtractor_params;
      for (const std::map<std::string, std::string>& param : params_I.at("ChromatogramExtractor")) {
        Utilities::CastValue c;
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

    // # map transitions to the chromatograms
    if (params_I.at("MRMMapping").size()) {
      // # set up MRMMapping and
      // # parse the MRMMapping params
      OpenMS::MRMMapping mrmmapper;
      OpenMS::Param parameters = mrmmapper.getParameters();
      Utilities::updateParameters(
        parameters,
        params_I.at("MRMMapping")
      );
      mrmmapper.setParameters(parameters);
      OpenMS::MSExperiment chromatogram_map;

      mrmmapper.mapExperiment(
        chromatograms,
        rawDataHandler_IO.getTargetedExperiment(),
        chromatogram_map
      );
      rawDataHandler_IO.setChromatogramMap(chromatogram_map);
    }

    if (verbose_I) {
      std::cout << "==== END   loadMSExperiment" << std::endl;
    }
  }

  void LoadRawData::extractMetaData(
    RawDataHandler& rawDataHandler_IO,
    const bool verbose_I
  )
  {
    if (verbose_I)
      std::cout << "Extracting metadata" << std::endl;

    std::string filename;
    std::string samplename;

    const std::string loaded_file_path = rawDataHandler_IO.getChromatogramMap().getLoadedFilePath();

    if (loaded_file_path.size()) {
      const std::string prefix{ "file://" };
      filename = !loaded_file_path.find(prefix) ? loaded_file_path.substr(prefix.size()) : loaded_file_path;
    }

    OpenMS::DataValue dv_mzml_id = rawDataHandler_IO.getChromatogramMap().getMetaValue("mzml_id");

    if (!dv_mzml_id.isEmpty() && dv_mzml_id.toString().size()) {
      samplename = dv_mzml_id.toString();
      const size_t pos = samplename.find('-');
      if (pos != std::string::npos)
        samplename = samplename.substr(pos + 1);
    }
    else {
      throw "no mzml_id found\n";
    }

    const OpenMS::MSExperiment& chromatogram_map = rawDataHandler_IO.getChromatogramMap();

    MetaDataHandler& metaDataHandler = rawDataHandler_IO.getMetaData();

    if (metaDataHandler.getSampleName().empty())
      metaDataHandler.setSampleName(samplename);

    if (metaDataHandler.getFilename().empty())
      metaDataHandler.setFilename(filename);

    if (metaDataHandler.proc_method_name.empty())
      metaDataHandler.proc_method_name = chromatogram_map.getInstrument().getSoftware().getName();

    if (metaDataHandler.instrument.empty())
      metaDataHandler.instrument = chromatogram_map.getInstrument().getName();

    if (metaDataHandler.operator_name.empty() && chromatogram_map.getContacts().size())
      metaDataHandler.operator_name = chromatogram_map.getContacts()[0].getLastName() + " " + chromatogram_map.getContacts()[0].getFirstName();

    if (metaDataHandler.acquisition_date_and_time.tm_year == 0) {
      // some noise because OpenMS uses uint and the standard library uses int (for time structure's members)
      struct { OpenMS::UInt tm_mon, tm_mday, tm_year, tm_hour, tm_min, tm_sec; } dt_uint;
      rawDataHandler_IO.getChromatogramMap().getDateTime().get(
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
  }

  void StoreRawData::process(
    RawDataHandler& rawDataHandler_IO,
    const std::map<std::string, std::vector<std::map<std::string, std::string>>>& params_I,
    const Filenames& filenames,
    const bool verbose_I
  ) const {
    if (verbose_I) {
      std::cout << "==== START storeMzML"
        << "\nstoreMzML(): storing " << filenames.mzML_i << std::endl;
    }

    if (filenames.mzML_i.empty()) {
      std::cout << "storeMzML(): filename is empty\n";
      return;
    }

    try {
      OpenMS::MzMLFile mzmlfile;
      mzmlfile.store(filenames.mzML_i, rawDataHandler_IO.getChromatogramMap());
    }
    catch (const std::exception& e) {
      std::cerr << "storeMzML(): " << e.what() << std::endl;
    }

    if (verbose_I) {
      std::cout << "==== END   storeMzML" << std::endl;
    }
  }

  void LoadFeatures::process(
    RawDataHandler& rawDataHandler_IO,
    const std::map<std::string, std::vector<std::map<std::string, std::string>>>& params_I,
    const Filenames& filenames,
    const bool verbose_I
  ) const {
    if (verbose_I) {
      std::cout << "==== START loadFeatureMap"
        << "\nloadFeatureMap(): loading " << filenames.featureXML_i << std::endl;
    }

    if (filenames.featureXML_i.empty()) {
      std::cout << "loadFeatureMap(): filename is empty\n";
      return;
    }

    if (!InputDataValidation::fileExists(filenames.featureXML_i)) {
      std::cout << "loadFeatureMap(): file not found\n";
      return;
    }

    try {
      OpenMS::FeatureXMLFile featurexml;
      featurexml.load(filenames.featureXML_i, rawDataHandler_IO.getFeatureMap());
      rawDataHandler_IO.updateFeatureMapHistory();
    }
    catch (const std::exception& e) {
      std::cerr << "loadFeatureMap(): " << e.what() << std::endl;
      rawDataHandler_IO.getFeatureMap().clear();
      std::cerr << "loadFeatureMap(): feature map clear" << std::endl;
    }

    if (verbose_I) {
      std::cout << "==== END   loadFeatureMap" << std::endl;
    }
  }

  void StoreFeatures::process(
    RawDataHandler& rawDataHandler_IO,
    const std::map<std::string, std::vector<std::map<std::string, std::string>>>& params_I,
    const Filenames& filenames,
    const bool verbose_I
  ) const {
    if (verbose_I) {
      std::cout << "==== START storeFeatureMap"
        << "\nstoreFeatureMap(): storing " << filenames.featureXML_o << std::endl;
    }

    if (filenames.featureXML_o.empty()) {
      std::cout << "storeFeatureMap(): filename is empty\n";
      return;
    }

    try {
      // Store outfile as featureXML
      OpenMS::FeatureXMLFile featurexml;
      featurexml.store(filenames.featureXML_o, rawDataHandler_IO.getFeatureMapHistory());
    }
    catch (const std::exception& e) {
      std::cerr << "storeFeatureMap(): " << e.what() << std::endl;
    }

    if (verbose_I) {
      std::cout << "==== END   storeFeatureMap" << std::endl;
    }
  }

  void PickFeatures::process(
    RawDataHandler& rawDataHandler_IO,
    const std::map<std::string, std::vector<std::map<std::string, std::string>>>& params_I,
    const Filenames& filenames,
    const bool verbose_I
  ) const {
    if (verbose_I) {
      std::cout << "==== START pickFeatures" << std::endl;
      std::cout << "Experiment size: " << rawDataHandler_IO.getChromatogramMap().size() << std::endl;
    }

    if (params_I.find("MRMFeatureFinderScoring") != params_I.end() && params_I.at("MRMFeatureFinderScoring").empty()) {
      std::cout << "No parameters passed to PickFeatures. Not picking." << std::endl;
      return;
    }

    OpenMS::MRMFeatureFinderScoring featureFinder;
    OpenMS::Param parameters = featureFinder.getParameters();
    Utilities::updateParameters(parameters, params_I.at("MRMFeatureFinderScoring"));
    featureFinder.setParameters(parameters);

    OpenMS::FeatureMap featureMap;

    featureFinder.pickExperiment(
      rawDataHandler_IO.getChromatogramMap(),
      featureMap,
      rawDataHandler_IO.getTargetedExperiment(),
      rawDataHandler_IO.getTransformationDescription(),
      rawDataHandler_IO.getSWATH()
    );

    featureMap.setPrimaryMSRunPath({rawDataHandler_IO.getMetaData().getFilename()});  // NOTE: needed for calculate_calibration
    rawDataHandler_IO.setFeatureMap(featureMap);
    rawDataHandler_IO.updateFeatureMapHistory();

    if (verbose_I) {
      std::cout << "pickFeatures: output size: " << featureMap.size() << std::endl;
      std::cout << "==== END   pickFeatures" << std::endl;
    }
  }

  void FilterFeatures::process(
    RawDataHandler& rawDataHandler_IO,
    const std::map<std::string, std::vector<std::map<std::string, std::string>>>& params_I,
    const Filenames& filenames,
    const bool verbose_I
  ) const {
    if (verbose_I) {
      std::cout << "==== START filterFeatures" << std::endl;
      std::cout << "filterFeatures: input size: " << rawDataHandler_IO.getFeatureMap().size() << std::endl;
    }

    if (params_I.find("MRMFeatureFilter.filter_MRMFeatures") != params_I.end() && params_I.at("MRMFeatureFilter.filter_MRMFeatures").empty()) {
      std::cout << "No parameters passed to filterFeatures. Not filtering." << std::endl;
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

    if (verbose_I) {
      std::cout << "filterFeatures: output size: " << featureMap.size() << std::endl;
      std::cout << "==== END   filterFeatures" << std::endl;
    }
  }

  void CheckFeatures::process(
    RawDataHandler& rawDataHandler_IO,
    const std::map<std::string, std::vector<std::map<std::string, std::string>>>& params_I,
    const Filenames& filenames,
    const bool verbose_I
  ) const {
    if (verbose_I) {
      std::cout << "==== START checkFeatures" << std::endl;
      std::cout << "checkFeatures: input size: " << rawDataHandler_IO.getFeatureMap().size() << std::endl;
    }

    if (params_I.find("MRMFeatureFilter.filter_MRMFeatures.qc") != params_I.end() && params_I.at("MRMFeatureFilter.filter_MRMFeatures.qc").empty()) {
      std::cout << "No parameters passed to checkFeatures. Not checking." << std::endl;
      return;
    }

    OpenMS::MRMFeatureFilter featureFilter;
    OpenMS::Param parameters = featureFilter.getParameters();
    Utilities::updateParameters(parameters, params_I.at("MRMFeatureFilter.filter_MRMFeatures.qc"));
    featureFilter.setParameters(parameters);

    OpenMS::FeatureMap& featureMap = rawDataHandler_IO.getFeatureMap();

    featureFilter.FilterFeatureMap(
      featureMap,
      rawDataHandler_IO.getFeatureQC(),
      rawDataHandler_IO.getTargetedExperiment()
    );

    rawDataHandler_IO.updateFeatureMapHistory();

    if (verbose_I) {
      std::cout << "checkFeatures: output size: " << featureMap.size() << std::endl;
      std::cout << "==== END   checkFeatures" << std::endl;
    }
  }

  void SelectFeatures::process(
    RawDataHandler& rawDataHandler_IO,
    const std::map<std::string, std::vector<std::map<std::string, std::string>>>& params_I,
    const Filenames& filenames,
    const bool verbose_I
  ) const {
    if (verbose_I) {
      std::cout << "==== START selectFeatures" << std::endl;
      std::cout << "selectFeatures: input size: " << rawDataHandler_IO.getFeatureMap().size() << std::endl;
    }

    if (params_I.find("MRMFeatureSelector.schedule_MRMFeatures_qmip") != params_I.end() &&
      params_I.find("MRMFeatureSelector.schedule_MRMFeatures_score") != params_I.end()) {
      std::cout << "No parameters passed to selectFeatures. Not selecting." << std::endl;
      return;
    }

    OpenMS::FeatureMap output;
    
    if (params_I.at("MRMFeatureSelector.schedule_MRMFeatures_qmip").size()) {
      std::vector<OpenMS::MRMFeatureSelector::SelectorParameters> p =
        Utilities::extractSelectorParameters(params_I.at("MRMFeatureSelector.schedule_MRMFeatures_qmip"), params_I.at("MRMFeatureSelector.select_MRMFeatures_qmip"));
      OpenMS::MRMBatchFeatureSelector::batchMRMFeaturesQMIP(rawDataHandler_IO.getFeatureMap(), output, p);
    } else if (params_I.at("MRMFeatureSelector.schedule_MRMFeatures_score").size()) {
      std::vector<OpenMS::MRMFeatureSelector::SelectorParameters> p =
        Utilities::extractSelectorParameters(params_I.at("MRMFeatureSelector.schedule_MRMFeatures_score"), params_I.at("MRMFeatureSelector.select_MRMFeatures_score"));
      OpenMS::MRMBatchFeatureSelector::batchMRMFeaturesScore(rawDataHandler_IO.getFeatureMap(), output, p);
    } else {
      throw std::invalid_argument("Both arguments 'select params' and 'schedule params' are empty.");
    }

    output.setPrimaryMSRunPath({rawDataHandler_IO.getMetaData().getFilename()});

    rawDataHandler_IO.setFeatureMap(output);
    rawDataHandler_IO.updateFeatureMapHistory();

    if (verbose_I) {
      std::cout << "selectFeatures: output size: " << output.size() << std::endl;
      std::cout << "==== END   selectFeatures" << std::endl;
    }
  }

  void ValidateFeatures::process(
    RawDataHandler& rawDataHandler_IO,
    const std::map<std::string, std::vector<std::map<std::string, std::string>>>& params_I,
    const Filenames& filenames,
    const bool verbose_I
  ) const {
    if (verbose_I) {
      std::cout << "==== START validateFeatures" << std::endl;
    }

    if (params_I.find("MRMFeatureValidator.validate_MRMFeatures") != params_I.end() && params_I.at("MRMFeatureValidator.validate_MRMFeatures").empty()) {
      std::cout << "No parameters passed to validateFeatures. Not validating." << std::endl;
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
      std::stof(params_I.at("MRMFeatureValidator.validate_MRMFeatures").front().at("value")), // TODO: While this probably works, it might be nice to add some check that the parameter passed is the desired one
      verbose_I
    );

    rawDataHandler_IO.setFeatureMap(mapped_features);
    rawDataHandler_IO.setValidationMetrics(validation_metrics);

    if (verbose_I) {
      std::cout << "==== END   validateFeatures" << std::endl;
    }
  }

  void PlotFeatures::process(
    RawDataHandler& rawDataHandler_IO,
    const std::map<std::string, std::vector<std::map<std::string, std::string>>>& params_I,
    const Filenames& filenames,
    const bool verbose_I
  ) const {
    if (verbose_I)
      std::cout << "Plotting peaks with features (NOT IMPLEMENTED)" << std::endl;

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
  }

  void QuantifyFeatures::process(
    RawDataHandler& rawDataHandler_IO,
    const std::map<std::string, std::vector<std::map<std::string, std::string>>>& params_I,
    const Filenames& filenames,
    const bool verbose_I
  ) const {
    if (verbose_I) {
      std::cout << "==== START quantifyComponents" << std::endl;
      std::cout << "Processing # quantitation methods: " << rawDataHandler_IO.getQuantitationMethods().size() << std::endl;
    }

    try {
      OpenMS::AbsoluteQuantitation aq;
      aq.setQuantMethods(rawDataHandler_IO.getQuantitationMethods());
      aq.quantifyComponents(rawDataHandler_IO.getFeatureMap());
      rawDataHandler_IO.updateFeatureMapHistory();
    } catch (const std::exception& e) {
      std::cerr << "quantifyComponents(): " << e.what() << std::endl;
    }

    if (verbose_I) {
      std::cout << "==== END   quantifyComponents" << std::endl;
    }
  }
  
  void LoadTransitions::process(
    RawDataHandler& rawDataHandler_IO,
    const std::map<std::string, std::vector<std::map<std::string, std::string>>>& params_I,
    const Filenames& filenames,
    const bool verbose_I
  ) const {
    // TODO: move to parameters at some point
    std::string format = "csv";
    if (verbose_I) {
      std::cout << "==== START loadTraML"
        << "\nloadTraML(): loading " << filenames.traML_csv_i << "; format: " << format << std::endl;
    }

    if (filenames.traML_csv_i.empty()) {
      std::cout << "loadTraML(): filename is empty\n";
      return;
    }

    if (!InputDataValidation::fileExists(filenames.traML_csv_i)) {
      std::cout << "loadTraML(): file not found\n";
      return;
    }

    try {
      // must use "PeptideSequence"
      if (format == "csv") {
        OpenMS::TransitionTSVFile tsvfile;
        tsvfile.convertTSVToTargetedExperiment(
          filenames.traML_csv_i.c_str(),
          OpenMS::FileTypes::TRAML,
          rawDataHandler_IO.getTargetedExperiment()
        );
      }
      else if (format == "traML") {
        OpenMS::TraMLFile tramlfile;
        tramlfile.load(filenames.traML_csv_i, rawDataHandler_IO.getTargetedExperiment());
      }
      else {
        std::cerr << "loadTraML(): format must either be \"csv\" or \"traML\".\n";
        return;
      }
    }
    catch (const std::exception& e) {
      std::cerr << "loadTraML(): " << e.what() << std::endl;
      rawDataHandler_IO.getTargetedExperiment().clear(true);
      std::cerr << "loadTraML(): targeted experiment clear" << std::endl;
    }

    // std::cout << InputDataValidation::getTraMLInfo(rawDataHandler_IO);

    if (verbose_I) {
      std::cout << "==== END   loadTraML" << std::endl;
    }
  }

  void LoadFeatureFilters::process(
    RawDataHandler& rawDataHandler_IO,
    const std::map<std::string, std::vector<std::map<std::string, std::string>>>& params_I,
    const Filenames& filenames,
    const bool verbose_I
  ) const {
    if (verbose_I) {
      std::cout << "==== START loadFeatureFilter"
        << "\nloadFeatureFilter(): loading " << filenames.featureFilterComponents_csv_i << " and "
        << filenames.featureFilterComponentGroups_csv_i << std::endl;
    }

    if (filenames.featureFilterComponents_csv_i.empty() && filenames.featureFilterComponentGroups_csv_i.empty()) {
      std::cout << "loadFeatureFilter(): filenames are both empty\n";
      return;
    }

    if (filenames.featureFilterComponents_csv_i.size() &&
      !InputDataValidation::fileExists(filenames.featureFilterComponents_csv_i)) {
      std::cout << "loadFeatureFilter(): file not found (" << filenames.featureFilterComponents_csv_i << ")\n";
      return;
    }

    if (filenames.featureFilterComponentGroups_csv_i.size() &&
      !InputDataValidation::fileExists(filenames.featureFilterComponentGroups_csv_i)) {
      std::cout << "loadFeatureFilter(): file not found (" << filenames.featureFilterComponentGroups_csv_i << ")\n";
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
      std::cerr << "loadFeatureFilter(): " << e.what() << std::endl;
      rawDataHandler_IO.getFeatureFilter().component_qcs.clear();
      rawDataHandler_IO.getFeatureFilter().component_group_qcs.clear();
      rawDataHandler_IO.getFeatureFilter().component_group_pair_qcs.clear();
      std::cerr << "loadFeatureFilter(): feature filter clear" << std::endl;
    }

    // std::cout << InputDataValidation::getComponentsAndGroupsInfo(rawDataHandler_IO, true);

    if (verbose_I) {
      std::cout << "==== END   loadFeatureFilter" << std::endl;
    }
  }

  void LoadFeatureQCs::process(
    RawDataHandler& rawDataHandler_IO,
    const std::map<std::string, std::vector<std::map<std::string, std::string>>>& params_I,
    const Filenames& filenames,
    const bool verbose_I
  ) const {
    if (verbose_I) {
      std::cout << "==== START loadFeatureQC"
        << "\nloadFeatureQC(): loading " << filenames.featureQCComponents_csv_i << " and "
        << filenames.featureQCComponentGroups_csv_i << std::endl;
    }

    if (filenames.featureQCComponents_csv_i.empty() && filenames.featureQCComponentGroups_csv_i.empty()) {
      std::cout << "loadFeatureQC(): filenames are both empty\n";
      return;
    }

    if (filenames.featureQCComponents_csv_i.size() &&
      !InputDataValidation::fileExists(filenames.featureQCComponents_csv_i)) {
      std::cout << "loadFeatureQC(): file not found (" << filenames.featureQCComponents_csv_i << ")\n";
      return;
    }

    if (filenames.featureQCComponentGroups_csv_i.size() &&
      !InputDataValidation::fileExists(filenames.featureQCComponentGroups_csv_i)) {
      std::cout << "loadFeatureQC(): file not found (" << filenames.featureQCComponentGroups_csv_i << ")\n";
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
      std::cerr << "loadFeatureQC(): " << e.what() << std::endl;
      rawDataHandler_IO.getFeatureQC().component_qcs.clear();
      rawDataHandler_IO.getFeatureQC().component_group_qcs.clear();
      rawDataHandler_IO.getFeatureQC().component_group_pair_qcs.clear();
      std::cerr << "loadFeatureQC(): feature qc clear" << std::endl;
    }

    // std::cout << InputDataValidation::getComponentsAndGroupsInfo(rawDataHandler_IO, false);

    if (verbose_I) {
      std::cout << "==== END   loadFeatureQC" << std::endl;
    }
  }

  void LoadValidationData::process(
    RawDataHandler& rawDataHandler_IO,
    const std::map<std::string, std::vector<std::map<std::string, std::string>>>& params_I,
    const Filenames& filenames,
    const bool verbose_I
  ) const {
    if (verbose_I) {
      std::cout << "==== START loadValidationData"
        << "\nloadValidationData(): loading " << filenames.referenceData_csv_i << std::endl;
    }

    if (filenames.referenceData_csv_i.empty()) {
      std::cout << "loadValidationData(): filename is empty\n";
      return;
    }

    if (!InputDataValidation::fileExists(filenames.referenceData_csv_i)) {
      std::cout << "loadValidationData(): file not found\n";
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

    std::vector<std::map<std::string, Utilities::CastValue>> reference_data;

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
      std::map<std::string, Utilities::CastValue> m;
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

    if (verbose_I) {
      std::cout << "==== END   loadValidationData" << std::endl;
    }
  }

  void LoadParameters::process(
    RawDataHandler& rawDataHandler_IO,
    const std::map<std::string, std::vector<std::map<std::string, std::string>>>& params_I,
    const Filenames& filenames,
    const bool verbose_I
  ) const {
    if (verbose_I) {
      std::cout << "==== START readRawDataProcessingParameters"
        << "\nreadRawDataProcessingParameters(): loading " << filenames.parameters_csv_i << std::endl;
    }

    if (filenames.parameters_csv_i.empty()) {
      std::cout << "readRawDataProcessingParameters(): filename is empty\n";
      return;
    }

    if (!InputDataValidation::fileExists(filenames.parameters_csv_i)) {
      std::cout << "readRawDataProcessingParameters(): file not found\n";
      return;
    }

    try {
      std::map<std::string, std::vector<std::map<std::string, std::string>>> parameters;
      FileReader::parseOpenMSParams(filenames.parameters_csv_i, parameters);
      sanitizeParameters(rawDataHandler_IO, parameters);
    }
    catch (const std::exception& e) {
      std::cerr << "readRawDataProcessingParameters(): " << e.what() << std::endl;
    }

    if (verbose_I) {
      std::cout << "==== END   readRawDataProcessingParameters" << std::endl;
    }
  }

  void LoadParameters::sanitizeParameters(
    RawDataHandler& rawDataHandler_IO,
    std::map<std::string, std::vector<std::map<std::string, std::string>>>& params_I,
    const bool verbose_I
  ) {
    if (verbose_I) {
      std::cout << "==== START sanitizeRawDataProcessorParameters" << std::endl;
    }

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
      "MRMFeatureSelector.schedule_MRMFeatures_qmip",
      "MRMFeatureSelector.select_MRMFeatures_score",
      "ReferenceDataMethods.getAndProcess_referenceData_samples",
      "MRMFeatureValidator.validate_MRMFeatures",
      "MRMFeatureFilter.filter_MRMFeatures.qc",
    };
    for (const std::string& parameter : required_parameters) {
      if (!params_I.count(parameter)) {
        params_I.emplace(
          parameter,
          std::vector<std::map<std::string, std::string>>() // empty vector
        );
      }
    }
    rawDataHandler_IO.setParameters(params_I);

    if (verbose_I) {
      std::cout << "==== END   sanitizeRawDataProcessorParameters" << std::endl;
    }
  }
}
