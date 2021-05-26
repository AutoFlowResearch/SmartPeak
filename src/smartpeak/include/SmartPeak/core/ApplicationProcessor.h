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

#pragma once

#include <SmartPeak/core/ApplicationHandler.h>
#include <SmartPeak/core/Filenames.h>
#include <SmartPeak/iface/IProcessorDescription.h>
#include <SmartPeak/iface/ISequenceProcessorObserver.h>
#include <SmartPeak/iface/ISequenceSegmentProcessorObserver.h>
#include <SmartPeak/iface/ISampleGroupProcessorObserver.h>
#include <SmartPeak/core/ApplicationProcessorObservable.h>
#include <SmartPeak/io/InputDataValidation.h>
#include <string>
#include <vector>

namespace SmartPeak
{
  struct ApplicationProcessor : IProcessorDescription, ApplicationProcessorObservable {
    ApplicationProcessor(const ApplicationProcessor& other) = delete;
    ApplicationProcessor& operator=(const ApplicationProcessor& other) = delete;
    virtual ~ApplicationProcessor() = default;

    // Each of the derived classes implement the following virtual methods
    virtual bool process() = 0;

    /* IProcessorDescription */
    int getID() const override { return -1; }
    std::string getDescription() const override { return ""; }
    ParameterSet getParameterSchema() const override { return ParameterSet(); };

    ApplicationHandler& application_handler_;

  protected:
    ApplicationProcessor() = default;
    ApplicationProcessor(ApplicationHandler& application_handler) : application_handler_(application_handler) {}
  };

  namespace ApplicationProcessors {
    ParameterSet getParameterSchema();
    void processCommands(ApplicationHandler& application_handler, 
      std::vector<ApplicationHandler::Command> commands, 
      const std::set<std::string>& injection_names, 
      const std::set<std::string>& sequence_segment_names, 
      const std::set<std::string>& sample_group_names,
      IApplicationProcessorObserver* application_processor_observer = nullptr,
      ISequenceProcessorObserver* sequence_processor_observer = nullptr,
      ISequenceSegmentProcessorObserver* sequence_segment_processor_observer = nullptr,
      ISampleGroupProcessorObserver* sample_group_processor_observer = nullptr);
  }

  struct CreateCommand : ApplicationProcessor {
    CreateCommand(ApplicationHandler& application_handler) : ApplicationProcessor(application_handler) {}
    bool process() override;
    std::string name_;
    ApplicationHandler::Command cmd_; 
    std::string getName() const override { return "CreateCommand"; };
  };

  struct BuildCommandsFromNames : ApplicationProcessor {
    BuildCommandsFromNames(ApplicationHandler& application_handler) : ApplicationProcessor(application_handler) {}
    bool process() override;
    std::vector<std::string> names_;
    std::vector<ApplicationHandler::Command> commands_;
    std::string getName() const override { return "BuildCommandsFromNames"; };
  };

  struct FilePickerProcessor {
    FilePickerProcessor(ApplicationHandler& application_handler) : application_handler_(application_handler) {}
    std::string pathname_;
    ApplicationHandler& application_handler_;
    virtual bool processFilePicker() = 0;
  };

  struct RawDataFilePickerProcessor : FilePickerProcessor {
    RawDataFilePickerProcessor(
      ApplicationHandler& application_handler, 
      std::shared_ptr<RawDataProcessor> raw_data_processor) :
      FilePickerProcessor(application_handler),
      raw_data_processor_(raw_data_processor)
    {}
    bool processFilePicker() override;
    void setFileNameOuputPtr(std::string* filename_output) { filename_output_ = filename_output; };
    //    std::string getName() const override { return "LoadSequenceParameters"; };
    std::shared_ptr<RawDataProcessor> raw_data_processor_;
    Filenames filenames_;
    std::string * filename_output_ = nullptr;
  };

  struct SequenceSegmentFilePickerProcessor : FilePickerProcessor {
    SequenceSegmentFilePickerProcessor(
      ApplicationHandler& application_handler,
      std::shared_ptr<SequenceSegmentProcessor> sequence_segment_processor,
      bool group = false) :
      FilePickerProcessor(application_handler),
      sequence_segment_processor_(sequence_segment_processor),
      group_(group)
    {}
    bool processFilePicker() override;
    void setFileNameOuputPtr(std::string* filename_output, std::string* filename_output_group = nullptr)
    { 
      filename_output_ = filename_output; 
      filename_output_group_ = filename_output_group;
    };
    //    std::string getName() const override { return "LoadSequenceParameters"; };
    std::shared_ptr<SequenceSegmentProcessor> sequence_segment_processor_;
    Filenames filenames_;
    std::string* filename_output_ = nullptr;
    std::string* filename_output_group_ = nullptr;
    bool group_;
  };

  struct ApplicationFilePickerProcessor : FilePickerProcessor {
    ApplicationFilePickerProcessor(
      ApplicationHandler& application_handler,
      std::shared_ptr<ApplicationProcessor> application_processor) :
      FilePickerProcessor(application_handler),
      application_processor_(application_processor)
    {}
    bool processFilePicker() override;
    void setFileNameOuputPtr(std::string* filename_output) { filename_output_ = filename_output; };
    //    std::string getName() const override { return "LoadSequenceParameters"; };
    std::shared_ptr<ApplicationProcessor> application_processor_;
    std::string* filename_output_ = nullptr;
  };

  struct LoadSessionFromSequence : FilePickerProcessor {
    LoadSessionFromSequence(ApplicationHandler& application_handler) : FilePickerProcessor(application_handler) {}
    bool processFilePicker() override;
//    std::string getName() const override { return "LoadSessionFromSequence"; };
  private:
    bool buildStaticFilenames();
    void updateFilenames(Filenames& f, const std::string& pathname);
    bool requiredPathnamesAreValid(const std::vector<InputDataValidation::FilenameInfo>& validation);
    void clearNonExistantDefaultGeneratedFilenames(Filenames& f);
    void clearNonExistantFilename(std::string& filename);
    std::string getValidPathnameOrPlaceholder(const std::string& pathname, const bool is_valid);
  };

  struct StoreSequence : FilePickerProcessor {
    StoreSequence(ApplicationHandler& application_handler) :
      FilePickerProcessor(application_handler) {}
    bool processFilePicker() override;
//    std::string getName() const override { return "StoreSequence"; };
  };

  struct StoreSequenceFileSmartPeak : ApplicationProcessor {
    StoreSequenceFileSmartPeak(ApplicationHandler& application_handler) : ApplicationProcessor(application_handler) {}
    bool process() override;
    std::string getName() const override { return "StoreSequenceFileSmartPeak"; };
    std::string pathname_;
  };

  struct StoreSequenceFileAnalyst : ApplicationProcessor {
    StoreSequenceFileAnalyst(ApplicationHandler& application_handler) : ApplicationProcessor(application_handler) {}
    bool process() override;
    std::string getName() const override { return "StoreSequenceFileAnalyst"; };
    std::string filename_;
  };

  struct StoreSequenceFileMasshunter : ApplicationProcessor {
    StoreSequenceFileMasshunter(ApplicationHandler& application_handler) : ApplicationProcessor(application_handler) {}
    bool process() override;
    std::string getName() const override { return "StoreSequenceFileMasshunter"; };
    std::string filename_;
  };

  struct StoreSequenceFileXcalibur : ApplicationProcessor {
    StoreSequenceFileXcalibur(ApplicationHandler& application_handler) : ApplicationProcessor(application_handler) {}
    bool process() override;
    std::string getName() const override { return "StoreSequenceFileXcalibur"; };
    std::string filename_;
  };

  struct SetRawDataPathname : FilePickerProcessor {
    SetRawDataPathname(ApplicationHandler& application_handler) : FilePickerProcessor(application_handler) {}
    bool processFilePicker() override;
//    std::string getName() const override { return "SetRawDataPathname"; };
  };

  struct SetInputFeaturesPathname : FilePickerProcessor {
    SetInputFeaturesPathname(ApplicationHandler& application_handler) : FilePickerProcessor(application_handler) {}
    bool processFilePicker() override;
//    std::string getName() const override { return "SetInputFeaturesPathname"; };
  };

  struct SetOutputFeaturesPathname : FilePickerProcessor {
    SetOutputFeaturesPathname(ApplicationHandler& application_handler) : FilePickerProcessor(application_handler) {}
    bool processFilePicker() override;
//    std::string getName() const override { return "SetOutputFeaturesPathname"; };
  };

  struct LoadSequenceWorkflow : FilePickerProcessor {
    LoadSequenceWorkflow(ApplicationHandler& application_handler) : 
      FilePickerProcessor(application_handler) {}
    bool processFilePicker() override;
//    std::string getName() const override { return "LoadSequenceWorkflow"; };
  };

  struct StoreSequenceWorkflow : FilePickerProcessor {
    StoreSequenceWorkflow(ApplicationHandler& application_handler) :
      FilePickerProcessor(application_handler) {}
    bool processFilePicker() override;
//    std::string getName() const override { return "StoreSequenceWorkflow"; };
  };
}
