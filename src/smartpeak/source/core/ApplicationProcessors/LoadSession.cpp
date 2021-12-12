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

#include <SmartPeak/core/ApplicationProcessors/LoadSession.h>
#include <SmartPeak/core/ApplicationProcessors/LoadFilenames.h>
#include <SmartPeak/core/ApplicationProcessors/LoadPropertiesHandlers.h>
#include <SmartPeak/core/ApplicationProcessors/BuildCommandsFromNames.h>
#include <SmartPeak/core/SequenceProcessor.h>

namespace SmartPeak
{

  bool LoadSession::onFilePicked(const std::filesystem::path& filename, ApplicationHandler* application_handler)
  {
    application_handler->closeSession();
    application_handler->filenames_.getSessionDB().setDBFilePath(filename);
    application_handler->main_dir_ = std::filesystem::path(filename).remove_filename().generic_string();
    delimiter = ",";
    checkConsistency = false; // NOTE: Requires a lot of time on large sequences with a large number of components
    return process();
  }

  bool LoadSession::readFilenames()
  {
    if (!filenames_) // if filenames are not provided, we will take it from the DB.
    {
      LoadFilenames load_filenames(application_handler_);
      if (!load_filenames.process())
      {
        notifyApplicationProcessorError("Failed to load session");
        return false;
      }
      filenames_ = application_handler_.filenames_;
    }

    if (!overrideFilenames())
    {
      return false;
    }

    filenames_->log();
    return true;
  }

  bool LoadSession::readInputFiles()
  {
    for (auto& loading_processor : application_handler_.loading_processors_)
    {
      // check if we need to use that loading processor
      Filenames loading_processor_filenames;
      loading_processor->getFilenames(loading_processor_filenames);
      bool load_it = false;
      for (const auto& file_id : loading_processor_filenames.getFileIds())
      {
        load_it |= (filenames_->isEmbedded(file_id)
          || (!filenames_->getFullPath(file_id).empty()));
      }

      if (load_it)
      {
        auto sequence_processor = std::dynamic_pointer_cast<SequenceProcessor>(loading_processor);
        if (sequence_processor)
        {
          try
          {
            sequence_processor->process(*filenames_);
          }
          catch (const std::exception& e)
          {
            LOGE << e.what();
            notifyApplicationProcessorError(e.what());
            return false;
          }
        }
        auto raw_data_processor = std::dynamic_pointer_cast<RawDataProcessor>(loading_processor);
        if (raw_data_processor)
        {
          if (!application_handler_.sequenceHandler_.getSequence().empty())
          {
            RawDataHandler& rawDataHandler = application_handler_.sequenceHandler_.getSequence()[0].getRawData();
            try
            {
              raw_data_processor->process(rawDataHandler, {}, *filenames_);
            }
            catch (const std::exception& e)
            {
              LOGE << e.what();
              notifyApplicationProcessorError(e.what());
              return false;
            }
          }
          else
          {
            LOGE << "No Sequence available, Loading process aborted.";
            notifyApplicationProcessorError("Failed to load session");
            return false;
          }
        }
        auto sequence_segment_processor = std::dynamic_pointer_cast<SequenceSegmentProcessor>(loading_processor);
        if (sequence_segment_processor)
        {
          if (!application_handler_.sequenceHandler_.getSequenceSegments().empty())
          {
            SequenceSegmentHandler& sequenceSegmentHandler = application_handler_.sequenceHandler_.getSequenceSegments().at(0);
            sequence_segment_processor->sequence_segment_observable_ = &application_handler_.sequenceHandler_;
            try
            {
              sequence_segment_processor->process(sequenceSegmentHandler, SequenceHandler(), {}, *filenames_);
            }
            catch (const std::exception& e)
            {
              LOGE << e.what();
              notifyApplicationProcessorError(e.what());
              return false;
            }
          }
          else
          {
            LOGE << "No Sequence Segment available, Loading process aborted.";
            notifyApplicationProcessorError("Failed to load session");
            return false;
          }
        }
      }
    }
    return true;
  }
  
  bool LoadSession::readLoadingWorkflow()
  {
    LoadPropertiesHandlers loading_workflow(application_handler_);
    loading_workflow.properties_handlers = { &application_handler_.session_loader_generator };
    if (!loading_workflow.process())
    {
      LOGE << "Failed to load loading workflow.";
      return false;
    }
    return true;
  }

  bool LoadSession::runLoadingWorkflow()
  {
    std::vector<std::string> commands = application_handler_.session_loader_generator.getLoadingWorkflowCommands();
    BuildCommandsFromNames buildCommandsFromNames(application_handler_);
    buildCommandsFromNames.names_ = commands;
    if (!buildCommandsFromNames.process()) {
      LOGE << "Failed to create Commands, aborting.";
      return false;
    }
    for (auto& cmd : buildCommandsFromNames.commands_)
    {
      for (auto& p : cmd.dynamic_filenames)
      {
        p.second.setTagValue(Filenames::Tag::MZML_INPUT_PATH, application_handler_.filenames_.getTagValue(Filenames::Tag::MZML_INPUT_PATH));
        p.second.setTagValue(Filenames::Tag::FEATURES_INPUT_PATH, application_handler_.filenames_.getTagValue(Filenames::Tag::FEATURES_INPUT_PATH));
        p.second.setTagValue(Filenames::Tag::FEATURES_OUTPUT_PATH, application_handler_.filenames_.getTagValue(Filenames::Tag::FEATURES_OUTPUT_PATH));
      }
    }
    std::set<std::string> injection_names;
    for (const auto& injection : application_handler_.sequenceHandler_.getSequence())
    {
      injection_names.insert(injection.getMetaData().getInjectionName());
    }
    const std::set<std::string> sequence_segment_names = {};
    const std::set<std::string> sample_group_names = {};
    workflow_manager_.addWorkflow(
      application_handler_,
      injection_names,
      sequence_segment_names,
      sample_group_names,
      buildCommandsFromNames.commands_,
      application_processor_observer_,
      sequence_processor_observer_,
      sequence_segment_processor_observer_,
      sample_group_processor_observer_,
      true // At the moment, we are blocking the process of the workflow. put this to true (and other works) to profit from the progress bar.
    );
    return true;
  }

  bool LoadSession::process()
  {
    LOGD << "START LoadSession";

    if (!readFilenames())
    {
      return false;
    }

    if (!readInputFiles())
    {
      return false;
    }

    if (!overrideParameters())
    {
      return false;
    }

    if (checkConsistency)
    {
      if (!application_handler_.sequenceHandler_.getSequenceSegments().empty())
      {
        InputDataValidation::sampleNamesAreConsistent(application_handler_.sequenceHandler_);
        InputDataValidation::componentNamesAreConsistent(application_handler_.sequenceHandler_);
        InputDataValidation::componentNameGroupsAreConsistent(application_handler_.sequenceHandler_);
        InputDataValidation::heavyComponentsAreConsistent(application_handler_.sequenceHandler_);
      }
      else
      {
        LOGW << "No Sequence available, cannot check consistency";
      }
    }

    // Restore state: Load Features
    if (!application_handler_.filenames_.getSessionDB().getDBFilePath().empty())
    {
      if (!readLoadingWorkflow())
      {
        return false;
      }
      if (!runLoadingWorkflow())
      {
        return false;
      }
    }

    application_handler_.sequenceHandler_.notifySequenceUpdated();
    LOGD << "END LoadSession";
    return true;
  }

  bool LoadSession::overrideFilenames()
  {
    if (filenames_override_)
    {
      filenames_override_->setTagValue(Filenames::Tag::MAIN_DIR, filenames_->getTagValue(Filenames::Tag::MAIN_DIR));
      const auto& src = filenames_->getFileIds();
      for (const auto& file_id : filenames_override_->getFileIds())
      {
        if (std::find(src.begin(), src.end(), file_id) == src.end())
        {
          LOGE << "Unknown overriding input file: " << file_id;
          return false;
        }
        else
        {
          const auto value = filenames_override_->getFullPath(file_id);
          LOGW << "Overriding input file: " << file_id << " -> " << "\"" << value.generic_string() << "\"";
          filenames_->setFullPath(file_id, value);
        }
      }
    }
    return true;
  }

  bool LoadSession::overrideParameters()
  {
    if (parameters_override_)
    {
      if (application_handler_.sequenceHandler_.getSequence().size())
      {
        auto schema_params = application_handler_.getWorkflowParameterSchema();
        parameters_override_->merge(schema_params);

        for (auto& function_parameter_override : *parameters_override_)
        {
          for (auto& parameter_override : function_parameter_override.second)
          {
            if (!parameter_override.isSchema())
            {
              if (!parameter_override.getSchema())
              {
                LOGE << "Overridden parameter \"" << function_parameter_override.first << ":" << parameter_override.getName()
                  << "\": " << "Unused or Unknown";
                return false;
              }
              if (!parameter_override.isValid())
              {
                LOGE << "Overridden parameter \"" << function_parameter_override.first << ":" << parameter_override.getName()
                  << "\", Invalid value: \"" << parameter_override.getValueAsString() << "\"";
                LOGE << "Expected Type: " << parameter_override.getType();
                auto constraints = parameter_override.getRestrictionsAsString();
                if (!constraints.empty())
                {
                  LOGE << "Constraints: " << constraints;
                }
                return false;
              }
              else
              {
                // looks ok, override parameter
                ParameterSet& user_parameters = application_handler_.sequenceHandler_.getSequence().at(0).getRawData().getParameters();
                Parameter* existing_parameter = user_parameters.findParameter(function_parameter_override.first, parameter_override.getName());
                LOGW << "Overridden parameter \"" << function_parameter_override.first << ":" << parameter_override.getName()
                  << "\", set value: \"" << parameter_override.getValueAsString() << "\"";
                if (existing_parameter)
                {
                  existing_parameter->setValueFromString(parameter_override.getValueAsString(), false);
                }
                else
                {
                  user_parameters.addParameter(function_parameter_override.first, parameter_override);
                }
              }
            }
          }
        }
      }
    }
    return true;
  }

}
