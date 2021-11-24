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

  std::vector<std::string> StoreParameters::getRequirements() const
  {
    return { "sequence", "traML" };
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
    if (!InputDataValidation::prepareToStore(filenames_I, "parameters"))
    {
      throw std::invalid_argument("Failed to store output file");
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
        throw std::runtime_error("Failed to save in session database");
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
        throw std::invalid_argument("Failed to store output file");
      }
      ParametersParser::write(filenames_I.getFullPath("parameters").generic_string(), rawDataHandler_IO.getParameters());
    }
    LOGD << "END StoreParameters";
  }

}
