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

#include <SmartPeak/core/ApplicationProcessor.h>
#include <SmartPeak/core/SequenceProcessor.h>

namespace SmartPeak
{
  bool ApplicationProcessor::process()
  {
    bool result = false;
    LOGD << "START " << getName();
    try
    {
      result = doProcess();
    }
    catch (const std::exception& e)
    {
      LOGE << "END (ERROR) " << getName() << " " << e.what();
      throw;
    }
    LOGD << "END " << getName();
    return result;
  }

  namespace ApplicationProcessors {

  ParameterSet getParameterSchema()
  {
    ParameterSet parameter_set;
    parameter_set.merge(ProcessSequence::getParameterSchemaStatic());
    return parameter_set;
  }

  template<typename COMMANDS_LIST_TYPE>
  void notifyStartCommands(ApplicationProcessorObservable& observable, size_t start_index, const COMMANDS_LIST_TYPE& commands)
  {
    for (const auto command : commands)
    {
      observable.notifyApplicationProcessorCommandStart(start_index, command->getName());
      start_index++;
    }
  }

  template<typename COMMANDS_LIST_TYPE>
  void notifyEndCommands(ApplicationProcessorObservable& observable, size_t start_index, const COMMANDS_LIST_TYPE& commands)
  {
    for (const auto command : commands)
    {
      observable.notifyApplicationProcessorCommandEnd(start_index, command->getName());
      start_index++;
    }
  }

  void processCommands(ApplicationHandler& application_handler,
    std::vector<ApplicationHandler::Command> commands,
    const std::set<std::string>& injection_names, 
    const std::set<std::string>& sequence_segment_names, 
    const std::set<std::string>& sample_group_names,
    IApplicationProcessorObserver* application_processor_observer,
    ISequenceProcessorObserver* sequence_processor_observer,
    ISequenceSegmentProcessorObserver* sequence_segment_processor_observer,
    ISampleGroupProcessorObserver* sample_group_processor_observer)
  {
    ApplicationProcessorObservable observable;
    observable.addApplicationProcessorObserver(application_processor_observer);

    std::vector<std::string> commands_names;
    for (const auto& command : commands)
    {
      commands_names.push_back(command.getName());
    }
    observable.notifyApplicationProcessorStart(commands_names);

    size_t i = 0;
    while (i < commands.size()) {
      const ApplicationHandler::Command::CommandType type = commands[i].type;
      size_t j = i + 1;
      for (; j < commands.size() && type == commands[j].type; ++j) {
        // empty body
      }
      const ApplicationHandler::Command& cmd = commands[i];
      if (cmd.type == ApplicationHandler::Command::RawDataMethod) {
        std::vector<std::shared_ptr<RawDataProcessor>> raw_methods;
        std::map<std::string, Filenames> filenames;
        std::for_each(commands.begin() + i, commands.begin() + j,
          [&](const ApplicationHandler::Command& command) 
        { 
          raw_methods.push_back(command.raw_data_method);
          for (const auto& filename_to_merge : command.dynamic_filenames)
          {
            if (filenames.find(filename_to_merge.first) == filenames.end())
            {
              filenames.emplace(filename_to_merge.first, filename_to_merge.second);
            }
            else
            {
              filenames.at(filename_to_merge.first).merge(filename_to_merge.second);
            }
          }
        });
        ProcessSequence ps(application_handler.sequenceHandler_, sequence_processor_observer);
        ps.filenames_ = filenames;
        ps.raw_data_processing_methods_ = raw_methods;
        ps.injection_names_ = injection_names;
        notifyStartCommands<decltype(raw_methods)>(observable, i, raw_methods);
        ps.process(application_handler.filenames_);
        notifyEndCommands<decltype(raw_methods)>(observable, i, raw_methods);
      } else if (cmd.type == ApplicationHandler::Command::SequenceSegmentMethod) {
        std::vector<std::shared_ptr<SequenceSegmentProcessor>> seq_seg_methods;
        std::map<std::string, Filenames> filenames;
        std::for_each(commands.begin() + i, commands.begin() + j,
          [&](const ApplicationHandler::Command& command)
        {
          seq_seg_methods.push_back(command.seq_seg_method);
          for (const auto& filename_to_merge : command.dynamic_filenames)
          {
            if (filenames.find(filename_to_merge.first) == filenames.end())
            {
              filenames.emplace(filename_to_merge.first, filename_to_merge.second);
            }
            else
            {
              filenames.at(filename_to_merge.first).merge(filename_to_merge.second);
            }
          }
        });
        ProcessSequenceSegments pss(application_handler.sequenceHandler_, sequence_segment_processor_observer);
        pss.filenames_ = filenames;
        pss.sequence_segment_processing_methods_ = seq_seg_methods;
        pss.sequence_segment_names_ = sequence_segment_names;
        notifyStartCommands<decltype(seq_seg_methods)>(observable, i, seq_seg_methods);
        pss.process(application_handler.filenames_);
        notifyEndCommands<decltype(seq_seg_methods)>(observable, i, seq_seg_methods);
      } else if (cmd.type == ApplicationHandler::Command::SampleGroupMethod) {
        std::vector<std::shared_ptr<SampleGroupProcessor>> sample_group_methods;
        std::map<std::string, Filenames> filenames;
        std::for_each(commands.begin() + i, commands.begin() + j,
          [&](const ApplicationHandler::Command& command)
        {
          sample_group_methods.push_back(command.sample_group_method);
          for (const auto& filename_to_merge : command.dynamic_filenames)
          {
            if (filenames.find(filename_to_merge.first) == filenames.end())
            {
              filenames.emplace(filename_to_merge.first, filename_to_merge.second);
            }
            else
            {
              filenames.at(filename_to_merge.first).merge(filename_to_merge.second);
            }
          }
        });
        ProcessSampleGroups psg(application_handler.sequenceHandler_, sample_group_processor_observer);
        psg.filenames_ = filenames;
        psg.sample_group_processing_methods_ = sample_group_methods;
        psg.sample_group_names_ = sample_group_names;
        notifyStartCommands<decltype(sample_group_methods)>(observable, i, sample_group_methods);
        psg.process(application_handler.filenames_);
        notifyEndCommands<decltype(sample_group_methods)>(observable, i, sample_group_methods);
      }
      else 
      {
        LOGW << "Skipping a command: " << cmd.type << "\n";
      }
      i = j;
    }
    observable.notifyApplicationProcessorEnd();
  }
  }
}
