// TODO: Add copyright

#include <OpenMS/ANALYSIS/QUANTITATION/AbsoluteQuantitationMethod.h>
#include <OpenMS/FORMAT/FileTypes.h>
#include <SmartPeak/core/RawDataHandler.h>
#include <SmartPeak/io/OpenMSFile.h>
#include <vector>

namespace SmartPeak
{
  void loadStandardsConcentrations(
    const std::string& filename,
    sequenceSegmentHandler& sequenceSegmentHandler_IO,
    std::vector<OpenMS::AbsoluteQuantitationStandards::runConcentration>& standards_concentrations,
    const bool verbose = false
  ) const
  {
    if (verbose)
      std::cout << "loading standards concentrations" << std::endl;

    if (filename.empty())
      return;

    standards_concentrations.clear();
    AbsoluteQuantitationStandardsFile AQSf;

    try {
      AQSf.load(filename, standards_concentrations);
      sequenceSegmentHandler_IO.setStandardsConcentrations(standards_concentrations);
    } catch (const std::exception& e) {
      std::cout << e.what();
    }
  }

  void loadQuantitationMethods(
    const std::string& filename,
    sequenceSegmentHandler& sequenceSegmentHandler_IO,
    std::vector<OpenMS::AbsoluteQuantitationMethod>& quantitation_methods,
    const bool verbose = false
  ) const
  {
    if (verbose)
      std::cout << "loading quantitation methods" << std::endl;

    if (filename.empty())
      return;

    quantitation_methods_.clear();
    AbsoluteQuantitationMethodFile AQMf;

    try {
      AQMf.load(filename, quantitation_methods);
      sequenceSegmentHandler_IO.setQuantitationMethods(quantitation_methods);
    } catch (const std::exception& e) {
      std::cout << e.what();
    }
  }

  void loadTraML(
    RawDataHandler& rawDataHandler,
    const std::string& filename = "",
    const bool verbose = false
  ) const
  {
    if (verbose)
      std::cout << "Loading TraML" << std::endl;

    if (filename.empty())
      return;

    size_t which_ext {0}; // 1 for csv, 2 for traML

    const std::string csv_ext {".csv"};
    const std::string traml_ext {".traML"};

    if (filename.size() >= csv_ext.size()) {
      const std::string ext = filename.substr(filename.size() - csv_ext.size());
      if (ext.compare(csv_ext) == 0)
        which_ext = 1;
    }
    if (filename.size() >= traml_ext.size() && which_ext != 1) {
      const std::string ext = filename.substr(filename.size() - traml_ext.size());
      if (ext.compare(traml_ext) == 0)
        which_ext = 2;
    }

    TargetedExperiment targeted_exp; // # must use "PeptideSequence"
    if (which_ext == 1) {
      TransitionTSVFile tsvfile;
      tsvfile.convertTSVToTargetedExperiment(filename, FileTypes::Type::TRAML, targeted_exp);
    } else if (which_ext == 2) {
      TraMLFile tramlfile;
      tramlfile.load(filename, targeted_exp);
    } else {
      // TODO: error out? let it continue and have an empty targeted experiment?
    }

    rawDataHandler.setTargetedExperiment(targeted_exp);
  }

// DO NOT IMPLEMENT LOAD_TRAFO
  void load_Trafo(
    RawDataHandler& rawDataHandler,
    const bool trafo_csv_i,
    MRMFeatureFinderScoring_params_I = {},
    const bool verbose = false
  ) const
  {
    if (verbose)
      std::cout << "Loading Trafo" << std::endl;

    MRMFeatureFinderScoring_params = MRMFeatureFinderScoring_params_I

    MRMFeatureFinderScoring featurefinder;

    const Param& parameters = featurefinder.getParameters();
    utilities = Utilities()
    parameters = utilities.updateParameters(parameters, MRMFeatureFinderScoring_params); // TODO: implement Utilities?
    featurefinder.setParameters(parameters);

    TransformationDescription trafo;
    if (trafo_csv_i.empty()) {
      throw std::invalid_argument("invalid input filename");
    }
    // # load and make the transition file for RTNormalization
    TargetedExperiment targeted_rt_norm;
    TransitionTSVReader tramlfile;
    tramlfile.convertTSVToTargetedExperiment(
      trafo_csv_i.encode('utf-8'), 21, targeted_rt_norm
      )
    // # Normalize the RTs
    // # NOTE: same MRMFeatureFinderScoring params will be used to pickPeaks
    OpenSwathRTNormalizer RTNormalizer; // TODO: implement class
    trafo = RTNormalizer.main(
      rawDataHandler.chromatogram_map,
      targeted_rt_norm,
      model_params=None,
      // # model_params=model_params,
      model_type="linear",
      // # model_type="interpolated",
      min_rsq=0.95,
      min_coverage=0.6,
      estimateBestPeptides=True,
      MRMFeatureFinderScoring_params=parameters
      )
    rawDataHandler.setTransformationDescription(trafo); // TODO: implement the setter
  }


    //   """Load MzML into an MSExperiment

    //   Args:
    //       rawDataHandler (RawDataHandler): sample object; updated in place
    //       mzML_i (str): filename
    //       MRMMapping_params_I (list):
    //           list of key:value parameters for OpenMS::MRMMapping
    //       chromatogramExtractor_params_I (list):
    //           list of key:value parameters for OpenMS::ChromatogramExtractor
    //       mzML_params_I (list):
    //           list of key:value parameters to trigger the use of different
    //           file importers (e.g., ChromeleonFile)

    //   Internals:
    //       msExperiment (TargetedExperiment)

    //   """
  void load_MSExperiment(
      RawDataHandler& rawDataHandler,
      const std::string& mzML_i,
      MRMMapping_params_I={},
      chromatogramExtractor_params_I={},
      mzML_params_I={},
      const bool verbose = false
  ) const
  {
    if (verbose)
        std::cout << "Loading mzML" << std::endl;

    // # load chromatograms
    OpenMS::MSExperiment chromatograms;
    if (!mzML_i.empty()) {
      if (mzML_params_I.size()) {
        // # convert parameters
        std::map<std::string, Utilities::CastValue> mzML_params;
        for (const std::map<std::string,std::string>& param : mzML_params_I) {
          Utilities::CastValue c;
          Utilities::castString(param.at("value"), param.at("type"), c);
          mzML_params.insert(param.at("name"), c);
        }
        // if mzML_params["format"] == b"ChromeleonFile": NECESSARY? the parser does not care about the extension
        // mzML_i = mzML_i.replace(".mzML", ".txt")
        OpenMS::ChromeleonFile chfh;
        chfh.load(mzML_i, chromatograms);
      } else {
        OpenMS::FileHandler fh;
        fh.loadExperiment(mzML_i, chromatograms);
      }
    }

    OpenMS::TargetedExperiment& targeted_exp = rawDataHandler.getTargetedExperiment();
    if (chromatogramExtractor_params_I.size() && targeted_exp.getTransitions().size()) {
      // # convert parameters
      std::map<std::string, Utilities::CastValue> chromatogramExtractor_params;
      for (const std::map<std::string,std::string>& param : chromatogramExtractor_params_I) {
        Utilities::CastValue c;
        Utilities::castString(param.at("value"), param.at("type"), c);
        mzML_params.insert(param.at("name"), c);
      }
      // # exctract chromatograms
      OpenMS::MSExperiment chromatograms_copy = chromatograms;
      chromatograms.clear(true);
      if (chromatogramExtractor_params.count("extract_precursors")) {
        const std::vector<OpenMS::ReactionMonitoringTransition>& tr_const = targeted_exp.getTransitions();
        std::vector<OpenMS::ReactionMonitoringTransition>& tr = tr_const;
        for (OpenMS::ReactionMonitoringTransition& t : tr) {
          t.setProductMZ(t.getPrecursorMZ());
        }
        targeted_exp.setTransitions(tr);
        rawDataHandler.setTargetedExperiment(targeted_exp);
      }
      TransformationDescription transfDescr;
      ChromatogramExtractor chromatogramExtractor;
      chromatogramExtractor.extractChromatograms(
        chromatograms_copy,
        chromatograms,
        rawDataHandler.getTargetedExperiment(),
        chromatogramExtractor_params.at("extract_window").f,
        chromatogramExtractor_params['ppm'].b,
        transfDescr,
        chromatogramExtractor_params['rt_extraction_window'].f,
        chromatogramExtractor_params['filter'].s,
      );
    }
    rawDataHandler.setExperiment(chromatograms); // TODO: implement setter

    // # map transitions to the chromatograms
    OpenMS::TargetedExperiment& targeted_exp = rawDataHandler.getTargetedExperiment();
    if (MRMMapping_params_I.size() && targeted_exp.getTransitions().size()) {
      // # set up MRMMapping and
      // # parse the MRMMapping params
      OpenMS::MRMMapping mrmmapper;
      OpenMS::Param& parameters = mrmmapper.getParameters();
      parameters = utilities.updateParameters(
        parameters,
        MRMMapping_params_I,
      );
      mrmmapper.setParameters(parameters);
      OpenMS::MSExperiment chromatogram_map;

      // # mrmmapper = MRMMapper()
      // # chromatogram_map = mrmmapper.algorithm(
      // #     chromatogram_map=chromatograms,
      // #     targeted=rawDataHandler.targeted,
      // #     precursor_tolerance=0.0009, #hard-coded for now
      // #     product_tolerance=0.0009, #hard-coded for now
      // #     allow_unmapped=True,
      // #     allow_double_mappings=True
      // # )

      mrmmapper.mapExperiment(
          chromatograms,
          rawDataHandler.getTargetedExperiment(),
          chromatogram_map
      );
    }
    rawDataHandler.chromatogram_map = chromatogram_map; // TODO: update this line, missing setter in RawDataHandler
  }

  void loadSWATHorDIA(
      RawDataHandler& rawDataHandler,
      const String& dia_csv_i,
      const bool verbose = false
  )
  {
      """Load SWATH or DIA into an MSExperiment

      Args:
          rawDataHandler (RawDataHandler): sample object; updated in place
          dia_csv_i (str): filename

      Internals:
          msExperiment (TargetedExperiment)

      """
      if (verbose)
        std::cout << "Loading SWATH/DIA files" << std::endl;

      // # load in the DIA data
      MSExperiment swath;
      if (!dia_csv_i.empty()) {
        chromatogramExtractor = OpenSwathChromatogramExtractor() // TODO: implement class
        // # read in the DIA data files:
        // # dia_files_i = ...(dia_csv_i)
        swath = chromatogramExtractor.main( // TODO: and also its methods
          infiles=[],
          targeted=rawDataHandler.targeted,
          extraction_window=0.05,
          min_upper_edge_dist=0.0,
          ppm=False,
          is_swath=False,
          rt_extraction_window=-1,
          extraction_function="tophat"
        )
      }
      rawDataHandler.swath = swath; // TODO: implement setter?
  }

  void loadFeatureMap(
      RawDataHandler& rawDataHandler,
      const String& featureXML_i,
      const bool verbose = false
  )
  {
    """Load a FeatureMap

    Args:
        rawDataHandler (RawDataHandler): sample object; updated in place
        featureXML_i (str): filename

    """
    if (verbose)
      std::cout << "Loading FeatureMap" << std::endl;

    // # Store outfile as featureXML
    FeatureXMLFile featurexml;
    FeatureMap output;
    if (!featureXML_i.empty())
      featurexml.load(featureXML_i, output);

    rawDataHandler.setFeatureMap(output);
  }

// SKIP THIS FOR NOW
  def load_validationData(
      self,
      rawDataHandler,
      referenceData_csv_i=None,
      db_json_i=None,
      ReferenceDataMethods_params_I={},
      const bool verbose = false
  ):
      """Load the validation data from file or from a database

      Args:
          rawDataHandler (RawDataHandler): sample object; updated in place
          referenceData_csv_i (str): filename of the reference data
          db_json_i (str): filename of the DB json settings file
          ReferenceDataMethods_params_I (dict): dictionary of DB query parameters

      """
      if verbose:
          print("Loading validation data")

      utilities = Utilities()

      # Parse the input parameters
      ReferenceDataMethods_dict = {d['name']: utilities.parseString(
          d['value'], encode_str_I=False) for d in ReferenceDataMethods_params_I}
      experiment_ids_I = [],
      sample_names_I = [],
      sample_types_I = [],
      acquisition_methods_I = [],
      quantitation_method_ids_I = [],
      component_names_I = [],
      component_group_names_I = [],
      where_clause_I = '',
      used__I = True,
      experiment_limit_I = 10000,
      mqresultstable_limit_I = 1000000,
      # settings_filename_I = 'settings.ini',
      # data_filename_O = ''
      if "experiment_ids_I" in ReferenceDataMethods_dict.keys():
          experiment_ids_I = ReferenceDataMethods_dict["experiment_ids_I"]
      if "sample_names_I" in ReferenceDataMethods_dict.keys():
          sample_names_I = ReferenceDataMethods_dict["sample_names_I"]
      if "sample_types_I" in ReferenceDataMethods_dict.keys():
          sample_types_I = ReferenceDataMethods_dict["sample_types_I"]
      if "acquisition_methods_I" in ReferenceDataMethods_dict.keys():
          acquisition_methods_I = ReferenceDataMethods_dict["acquisition_methods_I"]
      if "quantitation_method_ids_I" in ReferenceDataMethods_dict.keys():
          quantitation_method_ids_I = ReferenceDataMethods_dict[
              "quantitation_method_ids_I"]
      if "component_names_I" in ReferenceDataMethods_dict.keys():
          component_names_I = ReferenceDataMethods_dict["component_names_I"]
      if "component_group_names_I" in ReferenceDataMethods_dict.keys():
          component_group_names_I = ReferenceDataMethods_dict["component_group_names_I"]
      if "where_clause_I" in ReferenceDataMethods_dict.keys():
          where_clause_I = ReferenceDataMethods_dict["where_clause_I"]
      if "used__I" in ReferenceDataMethods_dict.keys():
          used__I = ReferenceDataMethods_dict["used__I"]
      if "experiment_limit_I" in ReferenceDataMethods_dict.keys():
          experiment_limit_I = ReferenceDataMethods_dict["experiment_limit_I"]
      if "mqresultstable_limit_I" in ReferenceDataMethods_dict.keys():
          mqresultstable_limit_I = ReferenceDataMethods_dict["mqresultstable_limit_I"]
      # if "settings_filename_I" in ReferenceDataMethods_dict.keys():
      #     settings_filename_I = ReferenceDataMethods_dict["settings_filename_I"]
      # if "data_filename_O" in ReferenceDataMethods_dict.keys():
      #     data_filename_O = ReferenceDataMethods_dict["data_filename_O"]

      # read in the reference data
      reference_data = []
      if (!referenceData_csv_i.empty()) {
          smartpeak_i = FileReader()
          smartpeak_i.read_csv(referenceData_csv_i)
          reference_data = smartpeak_i.getData()
          smartpeak_i.clear_data()
      elif db_json_i is not None:
          referenceDataMethods = ReferenceDataMethods()
          reference_data = referenceDataMethods.getAndProcess_referenceData_samples(
              experiment_ids_I=experiment_ids_I,
              sample_names_I=sample_names_I,
              sample_types_I=sample_types_I,
              acquisition_methods_I=acquisition_methods_I,
              quantitation_method_ids_I=quantitation_method_ids_I,
              component_names_I=component_names_I,
              component_group_names_I=component_group_names_I,
              where_clause_I=where_clause_I,
              used__I=used__I,
              experiment_limit_I=experiment_limit_I,
              mqresultstable_limit_I=mqresultstable_limit_I,
              settings_filename_I=db_json_i,
              data_filename_O=''
          )
      rawDataHandler.reference_data = reference_data

  void loadFeatureFilter(
      RawDataHandler& rawDataHandler,
      const String& featureFilterComponents_csv_i = "",
      const String& featureFilterComponentGroups_csv_i = "",
      const bool verbose = false
  )
  {
    """Load the feature filters

    Args:
        rawDataHandler (RawDataHandler): sample object; updated in place
        featureFilterComponents_csv_i (str): filename
        featureFilterComponentGroups_csv_i (str): filename

    """
    if (verbose)
      std::cout << "Loading feature_filter" << std::endl;

    // # read in the parameters for the MRMFeatureQC
    MRMFeatureQC featureQC;
    MRMFeatureQCFile featureQCFile;
    if (!featureFilterComponents_csv_i.empty())
      featureQCFile.load(featureFilterComponents_csv_i, featureQC, false);
    if (!featureFilterComponentGroups_csv_i.empty())
      featureQCFile.load(featureFilterComponentGroups_csv_i, featureQC, true);
    rawDataHandler.setFeatureFilter(featureQC);
  }

  void loadFeatureQC(
    RawDataHandler& rawDataHandler,
    const String& featureQCComponents_csv_i,
    const String& featureQCComponentGroups_csv_i,
    const bool verbose = false
  )
  {
    """Load the feature QCs

    Args:
        rawDataHandler (RawDataHandler): sample object; updated in place
        featureQCComponents_csv_i (str): filename
        featureQCComponentGroups_csv_i (str): filename

    """
    if (verbose)
      std::cout << "Loading feature_qc" << std::endl;

    // # read in the parameters for the MRMFeatureQC
    MRMFeatureQC featureQC;
    MRMFeatureQCFile featureQCFile;
    if (!featureQCComponents_csv_i.empty())
      featureQCFile.load(featureQCComponents_csv_i, featureQC, false);
    if (!featureQCComponentGroups_csv_i.empty())
      featureQCFile.load(featureQCComponentGroups_csv_i, featureQC, true)
    rawDataHandler.setFeatureQC(featureQC);
  }

  void readRawDataProcessingParameters(
    RawDataHandler& rawDataHandler, const String& filename, const String& delimiter = ","
  )
  {
    """Import a rawDataProcessing parameters file

    the rawDataProcessing parameters are read in from a .csv file.

    Args:
        rawDataHandler (RawDataHandler)

    """

    // # read in the data
    if (filename.size()) {
      fileReader = FileReader() // TODO: port class
      fileReader.read_openMSParams(filename, delimiter) // TODO: and its methods
      parse_rawDataProcessingParameters(rawDataHandler, fileReader.getData()); // TODO: check this after FileReader is ported
      fileReader.clear_data(); // TODO: implement method
    }
  }

  void parse_rawDataProcessingParameters(
    RawDataHandler& rawDataHandler,
    std::map<std::string, std::vector<std::map<std::string, std::string>>>& parameters_file
  )
  {
    // """Parse a rawDataProcessing file to ensure all headers are present

    // Args:
    //     rawDataHandler (RawDataHandler)
    //     parameters_file (dict): dictionary of parameter
    // """

    // # check for workflow parameters integrity
    std::vector<std::string> required_parameters = {
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
      if (!required_parameters.count(parameter)) {
        required_parameters.emplace(
          parameter,
          std::vector<std::map<std::string, std::string>>() // empty vector
        );
      }
    }
    rawDataHandler.setParameters(parameters_file);
  }

}
