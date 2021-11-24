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
#include <SmartPeak/io/InputDataValidation.h> // check filenames and headers

#include <SmartPeak/io/ParametersParser.h>
#ifndef CSV_IO_NO_THREAD
#define CSV_IO_NO_THREAD
#endif
#include <SmartPeak/io/csv.h>

#include <plog/Log.h>

#include <algorithm>
#include <exception>

namespace SmartPeak
{

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
      throw std::invalid_argument("Failed to load input file");
    }
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
        throw std::runtime_error("Failed to load from session database");
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

}
