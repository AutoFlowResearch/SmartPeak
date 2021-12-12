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
#include <SmartPeak/core/RawDataProcessors/LoadValidationData.h>
#include <SmartPeak/core/Filenames.h>
#include <SmartPeak/core/Utilities.h>
#include <SmartPeak/core/FeatureFiltersUtils.h>
#include <SmartPeak/io/InputDataValidation.h>

#ifndef CSV_IO_NO_THREAD
#define CSV_IO_NO_THREAD
#endif
#include <SmartPeak/io/csv.h>

#include <plog/Log.h>

#include <algorithm>
#include <exception>

namespace SmartPeak
{

  void LoadValidationData::getFilenames(Filenames& filenames) const
  {
    filenames.addFileName("referenceData", "${MAIN_DIR}/referenceData.csv", "Reference data", true, true);
  };

  bool LoadValidationData::onFilePicked(const std::filesystem::path& filename, ApplicationHandler* application_handler)
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

  void LoadValidationData::process(
    RawDataHandler& rawDataHandler_IO,
    const ParameterSet& params_I,
    Filenames& filenames_I
  ) const
  {
    LOGD << "START loadValidationData";
    getFilenames(filenames_I);
    if (!InputDataValidation::prepareToLoad(filenames_I, "referenceData"))
    {
      throw std::invalid_argument("Failed to load input file");
    }

    try
    {
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
      std::vector<std::map<std::string, CastValue>>& reference_data = rawDataHandler_IO.getReferenceData();

      if (filenames_I.isEmbedded("referenceData"))
      {
        auto db_context = filenames_I.getSessionDB().beginRead(
          "referenceData",
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
        if (!db_context)
        {
          throw std::runtime_error("Failed to load from session database");
        }
        while (filenames_I.getSessionDB().read(
          *db_context,
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
        ))
        {
          std::map<std::string, CastValue> m;
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
          mdh.setSampleName(sample_name);
          mdh.inj_number = sample_index;
          mdh.batch_name = experiment_id;
          mdh.setAcquisitionDateAndTimeFromString(acquisition_date_and_time, "%m-%d-%Y %H:%M");
          m.emplace("injection_name", mdh.getInjectionName());
          reference_data.push_back(std::move(m));
        }
        filenames_I.getSessionDB().endRead(*db_context);
      }
      else
      {
        io::CSVReader<17, io::trim_chars<>, io::no_quote_escape<','>> in(filenames_I.getFullPath("referenceData").generic_string());
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
          std::map<std::string, CastValue> m;
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
          mdh.setSampleName(sample_name);
          mdh.inj_number = sample_index;
          mdh.batch_name = experiment_id;
          mdh.setAcquisitionDateAndTimeFromString(acquisition_date_and_time, "%m-%d-%Y %H:%M");
          m.emplace("injection_name", mdh.getInjectionName());
          reference_data.push_back(std::move(m));
        }
      }
      rawDataHandler_IO.setReferenceData(reference_data);
    }
    catch (const std::exception& e) {
      LOGE << e.what();
      rawDataHandler_IO.getReferenceData().clear();
      LOGI << "RefereceData clear";
      throw e;
    }
    LOGD << "END loadValidationData";
  }

}
