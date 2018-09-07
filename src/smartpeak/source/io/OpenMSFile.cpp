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
      throw std::invalid_argument("filename is empty");

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
      std::cout << "Loading quantitation methods" << std::endl;

    if (filename.empty())
      throw std::invalid_argument("filename is empty");

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

    if (filename.empty()) {
      throw std::invalid_argument("filename is empty");
    }

    OpenMS::TargetedExperiment targeted_exp; // # must use "PeptideSequence"
    if (format == "csv") {
      OpenMS::TransitionTSVFile tsvfile;
      tsvfile.convertTSVToTargetedExperiment(filename.c_str(), OpenMS::FileTypes::TRAML, targeted_exp);
    } else if (format == "traML") {
      OpenMS::TraMLFile tramlfile;
      tramlfile.load(filename, targeted_exp);
    } else {
      std::cerr << "loadTraML(): format must either be \"csv\" or \"traML\".\n";
      return;
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
    const std::string& filename,
    const bool verbose
  )
  {
    if (verbose)
      std::cout << "Loading FeatureMap" << std::endl;

    if (filename.empty())
      throw std::invalid_argument("filename is empty");

    OpenMS::FeatureMap fm;
    OpenMS::FeatureXMLFile featurexml;
    featurexml.load(filename, fm);
    rawDataHandler.setFeatureMap(fm);
  }

  void OpenMSFile::loadFeatureFilter(
    RawDataHandler& rawDataHandler,
    const std::string& filename_components,
    const std::string& filename_components_groups,
    const bool verbose
  )
  {
    if (verbose)
      std::cout << "Loading feature_filter" << std::endl;

    if (filename_components.empty() && filename_components_groups.empty())
      throw std::invalid_argument("filenames are both empty");

    OpenMS::MRMFeatureQC featureQC;
    OpenMS::MRMFeatureQCFile featureQCFile;
    if (filename_components.size())
      featureQCFile.load(filename_components, featureQC, false);
    if (filename_components_groups.size())
      featureQCFile.load(filename_components_groups, featureQC, true);
    rawDataHandler.setFeatureFilter(featureQC);
  }

  void OpenMSFile::loadFeatureQC(
    RawDataHandler& rawDataHandler,
    const std::string& filename_components,
    const std::string& filename_components_groups,
    const bool verbose
  )
  {
    if (verbose)
      std::cout << "Loading feature_qc" << std::endl;

    if (filename_components.empty() && filename_components_groups.empty())
      throw std::invalid_argument("filenames are both empty");

    OpenMS::MRMFeatureQC featureQC;
    OpenMS::MRMFeatureQCFile featureQCFile;
    if (filename_components.size())
      featureQCFile.load(filename_components, featureQC, false);
    if (filename_components_groups.size())
      featureQCFile.load(filename_components_groups, featureQC, true);
    rawDataHandler.setFeatureQC(featureQC);
  }

  void OpenMSFile::readRawDataProcessingParameters(
    RawDataHandler& rawDataHandler,
    const std::string& filename,
    const std::string& delimiter
  )
  {
    if (filename.empty())
      throw std::invalid_argument("filename is empty");

    std::map<std::string,std::vector<std::map<std::string,std::string>>> parameters;
    FileReader::parseOpenMSParams(filename, parameters);

    parseRawDataProcessingParameters(rawDataHandler, parameters);
  }

  void OpenMSFile::parseRawDataProcessingParameters(
    RawDataHandler& rawDataHandler,
    std::map<std::string, std::vector<std::map<std::string, std::string>>>& parameters_file
  )
  {
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
    const std::string& filename,
    const bool verbose
  )
  {
    if (verbose)
      std::cout << "storing quantitation methods" << std::endl;

    if (filename.empty())
      throw std::invalid_argument("filename is empty");

    OpenMS::AbsoluteQuantitationMethodFile aqmf;
    aqmf.store(
      filename,
      sequenceSegmentHandler_IO.getQuantitationMethods()
    );
  }

  void OpenMSFile::storeFeatureMap(
    RawDataHandler& rawDataHandler_IO,
    const std::string& filename,
    const bool verbose
  )
  {
    if (verbose)
      std::cout << "Storing FeatureMap" << std::endl;

    if (filename.empty())
      throw std::invalid_argument("filename is empty");

    // # Store outfile as featureXML
    OpenMS::FeatureXMLFile featurexml;
    featurexml.store(filename, rawDataHandler_IO.getFeatureMap());
  }

  void OpenMSFile::storeMzML(const std::string& filename, const OpenMS::MSExperiment& experiment)
  {
    if (filename.empty())
      throw std::invalid_argument("filename is empty");

    OpenMS::MzMLFile mzmlf;
    mzmlf.store(filename, experiment);
  }
}
