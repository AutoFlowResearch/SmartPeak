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
#include <SmartPeak/core/RawDataProcessors/LoadRawData.h>
#include <SmartPeak/core/Filenames.h>
#include <SmartPeak/core/Utilities.h>
#include <SmartPeak/core/FeatureFiltersUtils.h>
#include <SmartPeak/io/InputDataValidation.h>

#include <OpenMS/FORMAT/ChromeleonFile.h>
#include <OpenMS/FORMAT/FileHandler.h>
#include <OpenMS/ANALYSIS/OPENSWATH/ChromatogramExtractor.h>

#include <plog/Log.h>

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

  std::set<std::string> LoadRawData::getOutputs() const
  {
    return { "Experiment", "Targeted Experiment", "Chromatogram", "Spectra"};
  }

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
        LOGI << "Loading: " << filenames_I.getFullPath("mzML_i").generic_string();
        fh.loadExperiment(filenames_I.getFullPath("mzML_i").generic_string(), chromatograms);
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

}
