// TODO: Add copyright

#include <OpenMS/ANALYSIS/QUANTITATION/AbsoluteQuantitationMethod.h>
#include <SmartPeak/core/RawDataHandler.h>
#include <SmartPeak/io/OpenMSFile.h>
#include <vector>

namespace SmartPeak
{
  void load_standardsConcentrations(
    const RawDataHandler& sequenceSegmentHandler_IO,
    std::vector<OpenMS::AbsoluteQuantitationMethod>& standardsConcentrations_csv_i,
    verbose_I = false
  ) const
  {
      """Load AbsoluteQuantitationStandards

      Args:
          sequenceSegmentHandler_IO (RawDataHandler)
          standardsConcentrations_csv_i (str): filename

      Internals:
          standardsConcentrations (list): list of AbsoluteQuantitationMethod objects

      """
      if (verbose_I)
        std::cout << "loading standards concentrations" << std::endl;

      try {
        std::vector<OpenMS::AbsoluteQuantitationMethod> standards_concentrations;
        if (standardsConcentrations_csv_i.empty())
          return;
        
        standards_concentrations = []
        if standardsConcentrations_csv_i is not None:
            aqsf = pyopenms.AbsoluteQuantitationStandardsFile()
            aqsf.load(standardsConcentrations_csv_i, standards_concentrations)
        sequenceSegmentHandler_IO.standards_concentrations = standards_concentrations
      } catch (except Exception as e) {
        print(e)
      }
  }

  def load_quantitationMethods(
      self,
      sequenceSegmentHandler_IO,
      quantitationMethods_csv_i,
      verbose_I=False
  ):
      """Load AbsoluteQuantitationMethods

      Args:
          sequenceSegmentHandler_IO (RawDataHandler)
          quantitationMethods_csv_i (str): None

      Internals:
          quantitationMethods (list): list of AbsoluteQuantitationMethod objects

      """
      if verbose_I:
          print("loading quantitation methods")

      try:
          quantitation_methods = []
          if quantitationMethods_csv_i is not None:
              aqmf = pyopenms.AbsoluteQuantitationMethodFile()
              aqmf.load(quantitationMethods_csv_i, quantitation_methods)
          sequenceSegmentHandler_IO.quantitation_methods = quantitation_methods
      except Exception as e:
          print(e)

  def load_TraML(
      self, rawDataHandler_IO,
      traML_csv_i=None,
      traML_i=None,
      verbose_I=False
  ):
      """Load TraML file

      Args:
          rawDataHandler_IO (RawDataHandler): sample object; updated in place
          traML_csv_i (str): .csv traML filename
          traML (str): .traML traML filename

      Internals:
          targeted (TargetedExperiment)

      """
      if verbose_I:
          print("Loading TraML")

      # load and make the transition file
      targeted = pyopenms.TargetedExperiment()  # must use "PeptideSequence"
      if traML_csv_i is not None:
          tramlfile = pyopenms.TransitionTSVFile()
          tramlfile.convertTSVToTargetedExperiment(
              traML_csv_i.encode('utf-8'), 21, targeted)
      elif traML_i is not None:
          targeted = pyopenms.TargetedExperiment()
          tramlfile = pyopenms.TraMLFile()
          tramlfile.load(traML_i.encode('utf-8'), targeted)
      rawDataHandler_IO.targeted = targeted

  def load_Trafo(
      self,
      rawDataHandler_IO,
      trafo_csv_i,
      MRMFeatureFinderScoring_params_I={},
      verbose_I=False
  ):
      """Load Trafo file

      Args:
          rawDataHandler_IO (RawDataHandler): sample object; updated in place
          trafo_csv_i (str): filename
          MRMFeatureFinderScoring_params_I (dict): dictionary of parameter
              names, values, descriptions, and tags

      Internals:
          targeted (TargetedExperiment)

      """
      if verbose_I:
          print("Loading Trafo")

      MRMFeatureFinderScoring_params = MRMFeatureFinderScoring_params_I

      # set up MRMFeatureFinderScoring (featurefinder) and
      # parse the MRMFeatureFinderScoring params
      featurefinder = pyopenms.MRMFeatureFinderScoring()
      parameters = featurefinder.getParameters()
      utilities = Utilities()
      parameters = utilities.updateParameters(
          parameters,
          MRMFeatureFinderScoring_params,
          )
      featurefinder.setParameters(parameters)

      # # prepare the model parameters for RTNormalization (interpolation)
      # model_params_list = [
      #   {'name':'interpolation_type','value': 'linear','type': 'string'},
      #     {'name':' extrapolation_type','value': 'two-point-linear','type': 'string'},
      # ]
      # model_params = pyopenms.Param()
      # model_params = utilities.setParameters(model_params_list,model_params)

      trafo = pyopenms.TransformationDescription()
      if trafo_csv_i is not None:
          # load and make the transition file for RTNormalization
          targeted_rt_norm = pyopenms.TargetedExperiment()
          tramlfile = pyopenms.TransitionTSVReader()
          tramlfile.convertTSVToTargetedExperiment(
              trafo_csv_i.encode('utf-8'), 21, targeted_rt_norm
              )
          # Normalize the RTs
          # NOTE: same MRMFeatureFinderScoring params will be used to pickPeaks
          RTNormalizer = OpenSwathRTNormalizer()
          trafo = RTNormalizer.main(
              rawDataHandler_IO.chromatogram_map,
              targeted_rt_norm,
              model_params=None,
              # model_params=model_params,
              model_type="linear",
              # model_type="interpolated",
              min_rsq=0.95,
              min_coverage=0.6,
              estimateBestPeptides=True,
              MRMFeatureFinderScoring_params=parameters
              )
      rawDataHandler_IO.trafo = trafo

  def load_MSExperiment(
      self,
      rawDataHandler_IO,
      mzML_i,
      MRMMapping_params_I={},
      chromatogramExtractor_params_I={},
      mzML_params_I={},
      verbose_I=False
  ):
      """Load MzML into an MSExperiment

      Args:
          rawDataHandler_IO (RawDataHandler): sample object; updated in place
          mzML_i (str): filename
          MRMMapping_params_I (list):
              list of key:value parameters for OpenMS::MRMMapping
          chromatogramExtractor_params_I (list):
              list of key:value parameters for OpenMS::ChromatogramExtractor
          mzML_params_I (list):
              list of key:value parameters to trigger the use of different
              file importers (e.g., ChromeleonFile)

      Internals:
          msExperiment (TargetedExperiment)

      """
      if verbose_I:
          print("Loading mzML")

      # load chromatograms
      chromatograms = pyopenms.MSExperiment()
      if mzML_i is not None:
          if mzML_params_I and mzML_params_I is not None:
              # convert parameters
              utilities = Utilities()
              mzML_params = {d['name']: utilities.castString(
                  d['value'],
                  d['type']) for d in mzML_params_I}
              if mzML_params["format"] == b"ChromeleonFile":
                  mzML_i = mzML_i.replace(".mzML", ".txt")
                  chfh = pyopenms.ChromeleonFile()
                  chfh.load(mzML_i.encode('utf-8'), chromatograms)
          else:
              fh = pyopenms.FileHandler()
              fh.loadExperiment(mzML_i.encode('utf-8'), chromatograms)

      if chromatogramExtractor_params_I and \
          chromatogramExtractor_params_I is not None and \
          rawDataHandler_IO.targeted is not None:
          # convert parameters
          utilities = Utilities()
          chromatogramExtractor_params = {d['name']: utilities.castString(
              d['value'],
              d['type']) for d in chromatogramExtractor_params_I}
          # chromatogramExtractor_params = {d['name']:utilities.parseString(d['value'])
          #   for d in chromatogramExtractor_params_I}
          # exctract chromatograms
          chromatograms_copy = copy.copy(chromatograms)
          chromatograms.clear(True)
          if chromatogramExtractor_params['extract_precursors']:
              tr = rawDataHandler_IO.targeted.getTransitions()
              for t in tr:
                  t.setProductMZ(t.getPrecursorMZ())
              rawDataHandler_IO.targeted.setTransitions(tr)
          chromatogramExtractor = pyopenms.ChromatogramExtractor()
          chromatogramExtractor.extractChromatograms(
              chromatograms_copy,
              chromatograms,
              rawDataHandler_IO.targeted,
              chromatogramExtractor_params['extract_window'],
              chromatogramExtractor_params['ppm'],
              pyopenms.TransformationDescription(),
              chromatogramExtractor_params['rt_extraction_window'],
              chromatogramExtractor_params['filter'],
              )

      rawDataHandler_IO.msExperiment = chromatograms

      # map transitions to the chromatograms
      if MRMMapping_params_I and \
          MRMMapping_params_I is not None and \
          rawDataHandler_IO.targeted is not None:
          # set up MRMMapping and
          # parse the MRMMapping params
          mrmmapper = pyopenms.MRMMapping()
          utilities = Utilities()
          parameters = mrmmapper.getParameters()
          parameters = utilities.updateParameters(
              parameters,
              MRMMapping_params_I,
              )
          mrmmapper.setParameters(parameters)
          chromatogram_map = pyopenms.MSExperiment()

          # mrmmapper = MRMMapper()
          # chromatogram_map = mrmmapper.algorithm(
          #     chromatogram_map=chromatograms,
          #     targeted=rawDataHandler_IO.targeted,
          #     precursor_tolerance=0.0009, #hard-coded for now
          #     product_tolerance=0.0009, #hard-coded for now
          #     allow_unmapped=True,
          #     allow_double_mappings=True
          # )

          mrmmapper.mapExperiment(
              chromatograms, rawDataHandler_IO.targeted, chromatogram_map)
      rawDataHandler_IO.chromatogram_map = chromatogram_map

  def load_SWATHorDIA(
      self,
      rawDataHandler_IO,
      dia_csv_i,
      verbose_I=False
  ):
      """Load SWATH or DIA into an MSExperiment

      Args:
          rawDataHandler_IO (RawDataHandler): sample object; updated in place
          dia_csv_i (str): filename

      Internals:
          msExperiment (TargetedExperiment)

      """
      if verbose_I:
          print("Loading SWATH/DIA files")

      # load in the DIA data
      swath = pyopenms.MSExperiment()
      if dia_csv_i is not None:
          chromatogramExtractor = OpenSwathChromatogramExtractor()
          # read in the DIA data files:
          # dia_files_i = ...(dia_csv_i)
          swath = chromatogramExtractor.main(
              infiles=[],
              targeted=rawDataHandler_IO.targeted,
              extraction_window=0.05,
              min_upper_edge_dist=0.0,
              ppm=False,
              is_swath=False,
              rt_extraction_window=-1,
              extraction_function="tophat"
          )
      rawDataHandler_IO.swath = swath

  def load_featureMap(
      self,
      rawDataHandler_IO,
      featureXML_i,
      verbose_I=False
  ):
      """Load a FeatureMap

      Args:
          rawDataHandler_IO (RawDataHandler): sample object; updated in place
          featureXML_i (str): filename

      """
      if verbose_I:
          print("Loading FeatureMap")

      # Store outfile as featureXML
      featurexml = pyopenms.FeatureXMLFile()
      output = pyopenms.FeatureMap()
      if featureXML_i is not None:
          featurexml.load(featureXML_i.encode('utf-8'), output)

      rawDataHandler_IO.featureMap = output

  def load_validationData(
      self,
      rawDataHandler_IO,
      referenceData_csv_i=None,
      db_json_i=None,
      ReferenceDataMethods_params_I={},
      verbose_I=False
  ):
      """Load the validation data from file or from a database

      Args:
          rawDataHandler_IO (RawDataHandler): sample object; updated in place
          referenceData_csv_i (str): filename of the reference data
          db_json_i (str): filename of the DB json settings file
          ReferenceDataMethods_params_I (dict): dictionary of DB query parameters

      """
      if verbose_I:
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
      if referenceData_csv_i is not None:
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
      rawDataHandler_IO.reference_data = reference_data

  def load_featureFilter(
      self,
      rawDataHandler_IO,
      featureFilterComponents_csv_i,
      featureFilterComponentGroups_csv_i,
      verbose_I=False
  ):
      """Load the feature filters

      Args:
          rawDataHandler_IO (RawDataHandler): sample object; updated in place
          featureFilterComponents_csv_i (str): filename
          featureFilterComponentGroups_csv_i (str): filename

      """
      if verbose_I:
          print("Loading feature_filter")

      # read in the parameters for the MRMFeatureQC
      featureQC = pyopenms.MRMFeatureQC()
      featureQCFile = pyopenms.MRMFeatureQCFile()
      if featureFilterComponents_csv_i is not None:
          featureQCFile.load(
              featureFilterComponents_csv_i.encode('utf-8'), featureQC, False)
      if featureFilterComponentGroups_csv_i is not None:
          featureQCFile.load(
              featureFilterComponentGroups_csv_i.encode('utf-8'), featureQC, True)
      rawDataHandler_IO.feature_filter = featureQC

  def load_featureQC(
      self,
      rawDataHandler_IO,
      featureQCComponents_csv_i,
      featureQCComponentGroups_csv_i,
      verbose_I=False
  ):
      """Load the feature QCs

      Args:
          rawDataHandler_IO (RawDataHandler): sample object; updated in place
          featureQCComponents_csv_i (str): filename
          featureQCComponentGroups_csv_i (str): filename

      """
      if verbose_I:
          print("Loading feature_qc")

      # read in the parameters for the MRMFeatureQC
      featureQC = pyopenms.MRMFeatureQC()
      featureQCFile = pyopenms.MRMFeatureQCFile()
      if featureQCComponents_csv_i is not None:
          featureQCFile.load(
              featureQCComponents_csv_i.encode('utf-8'), featureQC, False)
      if featureQCComponentGroups_csv_i is not None:
          featureQCFile.load(
              featureQCComponentGroups_csv_i.encode('utf-8'), featureQC, True)
      rawDataHandler_IO.feature_qc = featureQC

  def read_rawDataProcessingParameters(
      self, rawDataHandler_IO, filename, delimiter=','
  ):
      """Import a rawDataProcessing parameters file

      the rawDataProcessing parameters are read in from a .csv file.

      Args:
          rawDataHandler_IO (RawDataHandler)

      """

      # read in the data
      if filename is not None:
          fileReader = FileReader()
          fileReader.read_openMSParams(filename, delimiter)
          self.parse_rawDataProcessingParameters(
              rawDataHandler_IO, fileReader.getData())
          fileReader.clear_data()

  def parse_rawDataProcessingParameters(self, rawDataHandler_IO, parameters_file):
      """Parse a rawDataProcessing file to ensure all headers are present

      Args:
          rawDataHandler_IO (RawDataHandler)
          parameters_file (dict): dictionary of parameter
      """

      # check for workflow parameters integrity
      required_parameters = [
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
      ]
      for parameter in required_parameters:
          if parameter not in parameters_file:
              parameters_file[parameter] = []
      rawDataHandler_IO.setParameters(parameters_file)

}
