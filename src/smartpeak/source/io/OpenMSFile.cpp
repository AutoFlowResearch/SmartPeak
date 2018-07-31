// TODO: Add copyright

#include <OpenMS/ANALYSIS/QUANTITATION/AbsoluteQuantitationMethod.h>
#include <OpenMS/FORMAT/FileTypes.h>
#include <SmartPeak/core/RawDataHandler.h>
#include <SmartPeak/io/OpenMSFile.h>
#include <vector>

namespace SmartPeak
{
  void loadStandardsConcentrations(
    SequenceSegmentHandler& sequenceSegmentHandler_IO,
    const std::string& filename,
    const bool verbose = false
  ) const
  {
    if (verbose)
      std::cout << "loading standards concentrations" << std::endl;

    if (filename.empty())
      return;

    try {
      AbsoluteQuantitationStandardsFile AQSf;
      std::vector<OpenMS::AbsoluteQuantitationStandards::runConcentration> standards;
      AQSf.load(filename, standards);
      sequenceSegmentHandler_IO.setStandardsConcentrations(standards);
    } catch (const std::exception& e) {
      std::cout << e.what();
    }
  }

  void loadQuantitationMethods(
    sequenceSegmentHandler& sequenceSegmentHandler_IO,
    const std::string& filename,
    const bool verbose = false
  ) const
  {
    if (verbose)
      std::cout << "loading quantitation methods" << std::endl;

    if (filename.empty())
      return;

    try {
      AbsoluteQuantitationMethodFile AQMf;
      std::vector<OpenMS::AbsoluteQuantitationMethod> quantitation_methods;
      AQMf.load(filename, quantitation_methods);
      sequenceSegmentHandler_IO.setQuantitationMethods(quantitation_methods);
    } catch (const std::exception& e) {
      std::cout << e.what();
    }
  }

  void loadTraML(
    RawDataHandler& rawDataHandler,
    const std::string& filename,
    const std::string& format,
    const bool verbose = false
  ) const
  {
    if (verbose)
      std::cout << "Loading TraML" << std::endl;

    if (filename.empty() || (format != "csv" && format != "traML"))
      return;

    TargetedExperiment targeted_exp; // # must use "PeptideSequence"
    if (format == "csv") {
      TransitionTSVFile tsvfile;
      tsvfile.convertTSVToTargetedExperiment(filename, FileTypes::Type::TRAML, targeted_exp);
    } else {
      TraMLFile tramlfile;
      tramlfile.load(filename, targeted_exp);
    }

    rawDataHandler.setTargetedExperiment(targeted_exp);
  }

  void loadMSExperiment(
      RawDataHandler& rawDataHandler,
      const std::string& mzML_i,
      const std::vector<std::map<std::string, std::string>>& MRMMapping_params_I = std::vector<std::map<std::string, std::string>>(),
      const std::vector<std::map<std::string, std::string>>& chromatogramExtractor_params_I = std::vector<std::map<std::string, std::string>>(),
      const std::vector<std::map<std::string, std::string>>& mzML_params_I = std::vector<std::map<std::string, std::string>>(),
      const bool verbose = false
  ) const
  {
    if (verbose)
        std::cout << "Loading mzML" << std::endl;

    // # load chromatograms
    OpenMS::MSExperiment chromatograms;
    if (mzML_i.size()) {
      if (mzML_params_I.size()) {
        // # convert parameters
        std::map<std::string, Utilities::CastValue> mzML_params;
        for (const std::map<std::string,std::string>& param : mzML_params_I) {
          Utilities::CastValue c;
          Utilities::castString(param.at("value"), param.at("type"), c);
          mzML_params.insert(param.at("name"), c);
        }
        if (mzML_params.count("format") && mzML_params.at("format").s == "ChromeleonFile") {
          OpenMS::ChromeleonFile chfh;
          chfh.load(mzML_i, chromatograms);
        }
      } else {
        OpenMS::FileHandler fh;
        fh.loadExperiment(mzML_i, chromatograms);
      }
    }

    OpenMS::TargetedExperiment& targeted_exp = rawDataHandler.getTargetedExperiment();
    if (chromatogramExtractor_params_I.size()) {
      // # convert parameters
      std::map<std::string, Utilities::CastValue> chromatogramExtractor_params;
      for (const std::map<std::string,std::string>& param : chromatogramExtractor_params_I) {
        Utilities::CastValue c;
        Utilities::castString(param.at("value"), param.at("type"), c);
        chromatogramExtractor_params.insert(param.at("name"), c);
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
        rawDataHandler.setTargetedExperiment(targeted_exp);
      }
      TransformationDescription transfDescr;
      ChromatogramExtractor chromatogramExtractor;
      chromatogramExtractor.extractChromatograms(
        chromatograms_copy,
        chromatograms,
        rawDataHandler.getTargetedExperiment(),
        chromatogramExtractor_params.at("extract_window").f,
        chromatogramExtractor_params.at("ppm").b,
        transfDescr,
        chromatogramExtractor_params.at("rt_extraction_window").f,
        chromatogramExtractor_params.at("filter").s,
      );
    }
    rawDataHandler.setExperiment(chromatograms);

    // # map transitions to the chromatograms
    OpenMS::TargetedExperiment& targeted_exp = rawDataHandler.getTargetedExperiment();
    if (MRMMapping_params_I.size()) {
      // # set up MRMMapping and
      // # parse the MRMMapping params
      OpenMS::MRMMapping mrmmapper;
      OpenMS::Param parameters = utilities.updateParameters(
        mrmmapper.getParameters(),
        MRMMapping_params_I,
      );
      mrmmapper.setParameters(parameters);
      OpenMS::MSExperiment chromatogram_map;

      mrmmapper.mapExperiment(
          chromatograms,
          rawDataHandler.getTargetedExperiment(),
          chromatogram_map
      );
    }
    rawDataHandler.setChromatogramMap(chromatogram_map);
  }

  void loadSWATHorDIA(
      RawDataHandler& rawDataHandler,
      const String& dia_csv_i,
      const bool verbose = false
  )
  {
      if (verbose)
        std::cout << "Loading SWATH/DIA files" << std::endl;

      // # load in the DIA data
      MSExperiment swath;
      if (!dia_csv_i.empty()) {
        chromatogramExtractor = OpenSwathChromatogramExtractor() // TODO: implement class?
        // # read in the DIA data files:
        // # dia_files_i = ...(dia_csv_i)
        swath = chromatogramExtractor.main(
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
      rawDataHandler.swath = swath; // TODO: eventually implement setter
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
    FeatureMap output;
    if (featureXML_i.size()) {
      FeatureXMLFile featurexml;
      featurexml.load(featureXML_i, output);
    }
    rawDataHandler.setFeatureMap(output);
  }

  void loadFeatureFilter(
      RawDataHandler& rawDataHandler,
      const String& featureFilterComponents_csv_i = "",
      const String& featureFilterComponentGroups_csv_i = "",
      const bool verbose = false
  )
  {
    if (verbose)
      std::cout << "Loading feature_filter" << std::endl;

    // # read in the parameters for the MRMFeatureQC
    MRMFeatureQC featureQC;
    MRMFeatureQCFile featureQCFile;
    if (featureFilterComponents_csv_i.size())
      featureQCFile.load(featureFilterComponents_csv_i, featureQC, false);
    if (featureFilterComponentGroups_csv_i.size())
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
    if (verbose)
      std::cout << "Loading feature_qc" << std::endl;

    // # read in the parameters for the MRMFeatureQC
    MRMFeatureQC featureQC;
    MRMFeatureQCFile featureQCFile;
    if (featureQCComponents_csv_i.size())
      featureQCFile.load(featureQCComponents_csv_i, featureQC, false);
    if (featureQCComponentGroups_csv_i.size())
      featureQCFile.load(featureQCComponentGroups_csv_i, featureQC, true)
    rawDataHandler.setFeatureQC(featureQC);
  }

  void readRawDataProcessingParameters(
    RawDataHandler& rawDataHandler, const String& filename, const String& delimiter = ","
  )
  {

    if (filename.empty())
      return;

    FileReader fileReader; // TODO: must implement class
    fileReader.read_openMSParams(filename, delimiter);
    parse_rawDataProcessingParameters(rawDataHandler, fileReader.getData());
  }

  void parse_rawDataProcessingParameters(
    RawDataHandler& rawDataHandler,
    std::map<std::string, std::vector<std::map<std::string, std::string>>>& parameters_file
  )
  {
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
      if (!parameters_file.count(parameter)) {
        parameters_file.emplace(
          parameter,
          std::vector<std::map<std::string, std::string>>() // empty vector
        );
      }
    }
    rawDataHandler.setParameters(parameters_file);
  }

  void storeQuantitationMethods(
      const SequenceSegmentHandler& sequenceSegmentHandler_IO,
      const String& quantitationMethods_csv_o,
      const bool verbose = false
  )
  {
    if (verbose)
      std::cout << "storing quantitation methods" << std::endl;

    if (quantitationMethods_csv_o.size()) {
      AbsoluteQuantitationMethodFile aqmf;
      aqmf.store(
        quantitationMethods_csv_o,
        sequenceSegmentHandler_IO.getQuantitationMethods()
      );
    }
  }

  void storeFeatureMap(
    RawDataHandler& rawDataHandler_IO,
    const std::string& featureXML_o,
    const std::string& feature_csv_o,
    const bool verbose = false
  )
  {
    if (verbose)
      std::cout << "Storing FeatureMap" << std::endl;

    // # Store outfile as featureXML
    FeatureXMLFile featurexml;
    if (featureXML_o.size())
      featurexml.store(featureXML_o, rawDataHandler_IO.getFeatureMap());

    // # Store the outfile as csv
    featurescsv = OpenSwathFeatureXMLToTSV() // TODO: implement it?
    if (feature_csv_o.size()) {
      featurescsv.store(
        feature_csv_o,
        rawDataHandler_IO.getFeatureMap(),
        rawDataHandler_IO.getTargetedExperiment(),
        rawDataHandler_IO.getMetaData().getSampleName(),
        rawDataHandler_IO.getMetaData().getFilename()
      );
    }
  }

  void storeMzML(const std::string& out, const MSExperiment& output)
  {
    MzMLFile mzmlf;
    mzmlf.store(out, output);
  }
}
