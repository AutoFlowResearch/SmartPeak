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
#include <SmartPeak/core/RawDataProcessors/StoreValidationData.h>
#include <SmartPeak/core/Filenames.h>
#include <SmartPeak/core/Utilities.h>
#include <SmartPeak/core/FeatureFiltersUtils.h>
#include <SmartPeak/io/InputDataValidation.h>

#include <plog/Log.h>

#include <algorithm>
#include <exception>

namespace SmartPeak
{

  std::vector<std::string> StoreValidationData::getRequirements() const
  {
    return { "sequence", "traML" };
  }

  void StoreValidationData::getFilenames(Filenames& filenames) const
  {
    filenames.addFileName("referenceData", "${MAIN_DIR}/referenceData.csv", "Reference data", true, true);
  };

  bool StoreValidationData::onFilePicked(const std::filesystem::path& filename, ApplicationHandler* application_handler)
  {
    if (application_handler->sequenceHandler_.getSequence().size() == 0)
    {
      LOGE << "File cannot be loaded without first loading the sequence.";
      return false;
    }
    RawDataHandler& rawDataHandler = application_handler->sequenceHandler_.getSequence().at(0).getRawData();
    Filenames filenames;
    filenames.setFullPath("referenceData", filename);
    process(rawDataHandler, {}, filenames);
    return true;
  }

  void StoreValidationData::process(
    RawDataHandler& rawDataHandler_IO,
    const ParameterSet& params_I,
    Filenames& filenames_I
  ) const
  {
    LOGD << "START StoreValidationData";

    if (!InputDataValidation::prepareToStore(filenames_I, "referenceData"))
    {
      throw std::invalid_argument("Failed to load input file");
    }
    if (filenames_I.isEmbedded("referenceData"))
    {
      auto db_context = filenames_I.getSessionDB().beginWrite(
        "referenceData", 
        "sample_index", "INT",
        "original_filename", "STRING",
        "sample_name", "STRING",
        "sample_type", "STRING",
        "acquisition_date_and_time", "STRING",
        "acq_method_name", "STRING",
        "component_name", "STRING",
        "component_group_name", "STRING",
        "retention_time", "REAL",
        "start_time", "REAL",
        "end_time", "REAL",
        "used_", "STRING",
        "calculated_concentration", "REAL",
        "experiment_id", "STRING",
        "acquisition_method_id", "STRING",
        "height", "REAL",
        "area", "REAL"
      );
      if (!db_context)
      {
        throw std::runtime_error("Failed to save in session database");
      }
      std::vector<std::map<std::string, CastValue>>& reference_data = rawDataHandler_IO.getReferenceData();
      for (const auto& ref : reference_data)
      {
        const auto& v = ref.at("sample_index");
        const auto& s = ref.at("sample_index").s_;
        filenames_I.getSessionDB().write(
          *db_context,
          ref.at("sample_index").i_,
          ref.at("original_filename").s_,
          ref.at("sample_name").s_,
          ref.at("sample_type").s_,
          ref.at("acquisition_date_and_time").s_,
          ref.at("acq_method_name").s_,
          ref.at("component_name").s_,
          ref.at("component_group_name").s_,
          ref.at("retention_time").f_,
          ref.at("start_time").f_,
          ref.at("end_time").f_,
          ref.at("used_").s_,
          ref.at("calculated_concentration").f_,
          ref.at("experiment_id").s_,
          ref.at("acquisition_method_id").s_,
          ref.at("height").f_,
          ref.at("area").f_
          );
      }
      filenames_I.getSessionDB().endWrite(*db_context);
    }
    else
    {
      LOGE << "Not implemented";
    }

    LOGD << "END StoreValidationData";
  }

}
