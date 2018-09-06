// TODO: Add copyright

#include <OpenMS/ANALYSIS/QUANTITATION/AbsoluteQuantitationMethod.h>
#include <OpenMS/FORMAT/FileTypes.h>
#include <OpenMS/FORMAT/ChromeleonFile.h>
#include <SmartPeak/core/RawDataHandler.h>
#include <SmartPeak/core/Utilities.h>
#include <SmartPeak/io/OpenMSFile.h>
#include <vector>
#include <OpenMS/FORMAT/AbsoluteQuantitationStandardsFile.h>
#include <OpenMS/FORMAT/AbsoluteQuantitationMethodFile.h>
#include <OpenMS/ANALYSIS/OPENSWATH/TransitionTSVFile.h>
#include <OpenMS/FORMAT/FileTypes.h>
#include <OpenMS/FORMAT/TraMLFile.h>
#include <OpenMS/FORMAT/FileHandler.h>
#include <OpenMS/ANALYSIS/MAPMATCHING/TransformationDescription.h>
#include <OpenMS/ANALYSIS/OPENSWATH/ChromatogramExtractor.h>
#include <OpenMS/ANALYSIS/TARGETED/MRMMapping.h>
#include <OpenMS/FORMAT/FeatureXMLFile.h>
#include <OpenMS/FORMAT/MRMFeatureQCFile.h>
#include <OpenMS/FORMAT/MzMLFile.h>
#include <SmartPeak/io/FileReader.h>

namespace SmartPeak
{
  void OpenMSFile::loadStandardsConcentrations(
    SequenceSegmentHandler& sequenceSegmentHandler_IO,
    const std::string& filename,
    const bool verbose
  )
  {
    if (verbose)
      std::cout << "loading standards concentrations" << std::endl;

    if (filename.empty())
      return;

    try {
      OpenMS::AbsoluteQuantitationStandardsFile AQSf;
      std::vector<OpenMS::AbsoluteQuantitationStandards::runConcentration> standards;
      AQSf.load(filename, standards);
      sequenceSegmentHandler_IO.setStandardsConcentrations(standards);
    } catch (const std::exception& e) {
      std::cout << e.what();
    }
  }

  void OpenMSFile::loadQuantitationMethods(
    SequenceSegmentHandler& sequenceSegmentHandler_IO,
    const std::string& filename,
    const bool verbose
  )
  {
    if (verbose)
      std::cout << "loading quantitation methods" << std::endl;

    if (filename.empty())
      return;

    try {
      OpenMS::AbsoluteQuantitationMethodFile AQMf;
      std::vector<OpenMS::AbsoluteQuantitationMethod> quantitation_methods;
      AQMf.load(filename, quantitation_methods);
      sequenceSegmentHandler_IO.setQuantitationMethods(quantitation_methods);
    } catch (const std::exception& e) {
      std::cout << e.what();
    }
  }

  void OpenMSFile::loadTraML(
    RawDataHandler& rawDataHandler,
    const std::string& filename,
    const std::string& format,
    const bool verbose
  )
  {
    if (verbose)
      std::cout << "Loading TraML" << std::endl;

    if (filename.empty() || (format != "csv" && format != "traML"))
      return;

    OpenMS::TargetedExperiment targeted_exp; // # must use "PeptideSequence"
    if (format == "csv") {
      OpenMS::TransitionTSVFile tsvfile;
      tsvfile.convertTSVToTargetedExperiment(filename.c_str(), OpenMS::FileTypes::TRAML, targeted_exp);
    } else {
      OpenMS::TraMLFile tramlfile;
      tramlfile.load(filename, targeted_exp);
    }

    rawDataHandler.setTargetedExperiment(targeted_exp);
  }

  void OpenMSFile::loadMSExperiment(
      RawDataHandler& rawDataHandler,
      const std::string& mzML_i,
      const std::vector<std::map<std::string, std::string>>& MRMMapping_params_I,
      const std::vector<std::map<std::string, std::string>>& chromatogramExtractor_params_I,
      const std::vector<std::map<std::string, std::string>>& mzML_params_I,
      const bool verbose
  )
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
          mzML_params.emplace(param.at("name"), c);
        }
        if (mzML_params.count("format") && mzML_params.at("format").s_ == "ChromeleonFile") {
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
        rawDataHandler.setTargetedExperiment(targeted_exp);
      }
      OpenMS::TransformationDescription transfDescr;
      OpenMS::ChromatogramExtractor chromatogramExtractor;
      chromatogramExtractor.extractChromatograms(
        chromatograms_copy,
        chromatograms,
        rawDataHandler.getTargetedExperiment(),
        chromatogramExtractor_params.at("extract_window").f_,
        chromatogramExtractor_params.at("ppm").b_,
        transfDescr,
        chromatogramExtractor_params.at("rt_extraction_window").f_,
        chromatogramExtractor_params.at("filter").s_
      );
    }
    rawDataHandler.setExperiment(chromatograms);

    // # map transitions to the chromatograms
    if (MRMMapping_params_I.size()) {
      // # set up MRMMapping and
      // # parse the MRMMapping params
      OpenMS::MRMMapping mrmmapper;
      OpenMS::Param parameters = mrmmapper.getParameters();
      Utilities::updateParameters(
        parameters,
        MRMMapping_params_I
      );
      mrmmapper.setParameters(parameters);
      OpenMS::MSExperiment chromatogram_map;

      mrmmapper.mapExperiment(
          chromatograms,
          rawDataHandler.getTargetedExperiment(),
          chromatogram_map
      );
      rawDataHandler.setChromatogramMap(chromatogram_map);
    }
  }

  void OpenMSFile::loadFeatureMap(
      RawDataHandler& rawDataHandler,
      const std::string& featureXML_i,
      const bool verbose
  )
  {
    if (verbose)
      std::cout << "Loading FeatureMap" << std::endl;

    OpenMS::FeatureMap output;
    if (featureXML_i.size()) {
      OpenMS::FeatureXMLFile featurexml;
      featurexml.load(featureXML_i, output);
    }
    rawDataHandler.setFeatureMap(output);
  }

  void OpenMSFile::loadFeatureFilter(
      RawDataHandler& rawDataHandler,
      const std::string& featureFilterComponents_csv_i,
      const std::string& featureFilterComponentGroups_csv_i,
      const bool verbose
  )
  {
    if (verbose)
      std::cout << "Loading feature_filter" << std::endl;

    // # read in the parameters for the MRMFeatureQC
    OpenMS::MRMFeatureQC featureQC;
    OpenMS::MRMFeatureQCFile featureQCFile;
    if (featureFilterComponents_csv_i.size())
      featureQCFile.load(featureFilterComponents_csv_i, featureQC, false);
    if (featureFilterComponentGroups_csv_i.size())
      featureQCFile.load(featureFilterComponentGroups_csv_i, featureQC, true);
    rawDataHandler.setFeatureFilter(featureQC);
  }

  void OpenMSFile::loadFeatureQC(
    RawDataHandler& rawDataHandler,
    const std::string& featureQCComponents_csv_i,
    const std::string& featureQCComponentGroups_csv_i,
    const bool verbose
  )
  {
    if (verbose)
      std::cout << "Loading feature_qc" << std::endl;

    // # read in the parameters for the MRMFeatureQC
    OpenMS::MRMFeatureQC featureQC;
    OpenMS::MRMFeatureQCFile featureQCFile;
    if (featureQCComponents_csv_i.size())
      featureQCFile.load(featureQCComponents_csv_i, featureQC, false);
    if (featureQCComponentGroups_csv_i.size())
      featureQCFile.load(featureQCComponentGroups_csv_i, featureQC, true);
    rawDataHandler.setFeatureQC(featureQC);
  }

  void OpenMSFile::readRawDataProcessingParameters(
    RawDataHandler& rawDataHandler,
    const std::string& filename,
    const std::string& delimiter
  )
  {

    if (filename.empty())
      return;

    std::map<std::string,std::vector<std::map<std::string,std::string>>> parameters;
    FileReader::parse_OpenMSParams(filename, parameters);

    parse_rawDataProcessingParameters(rawDataHandler, parameters);
  }

  void OpenMSFile::parse_rawDataProcessingParameters(
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

  void OpenMSFile::storeQuantitationMethods(
      const SequenceSegmentHandler& sequenceSegmentHandler_IO,
      const std::string& quantitationMethods_csv_o,
      const bool verbose
  )
  {
    if (verbose)
      std::cout << "storing quantitation methods" << std::endl;

    if (quantitationMethods_csv_o.size()) {
      OpenMS::AbsoluteQuantitationMethodFile aqmf;
      aqmf.store(
        quantitationMethods_csv_o,
        sequenceSegmentHandler_IO.getQuantitationMethods()
      );
    }
  }

  void OpenMSFile::storeFeatureMap(
    RawDataHandler& rawDataHandler_IO,
    const std::string& featureXML_o,
    const bool verbose
  )
  {
    if (verbose)
      std::cout << "Storing FeatureMap" << std::endl;

    // # Store outfile as featureXML
    if (featureXML_o.size()) {
      OpenMS::FeatureXMLFile featurexml;
      featurexml.store(featureXML_o, rawDataHandler_IO.getFeatureMap());
    }
  }

  void OpenMSFile::storeMzML(const std::string& out, const OpenMS::MSExperiment& output)
  {
    OpenMS::MzMLFile mzmlf;
    mzmlf.store(out, output);
  }
}
