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
#include <SmartPeak/core/WorkflowManager.h>
#include <SmartPeak/core/Parameters.h>
#include <SmartPeak/iface/IPropertiesHandler.h>
#include <string>
#include <vector>

namespace SmartPeak
{
  struct ApplicationProcessor : IProcessorDescription, ApplicationProcessorObservable {
    ApplicationProcessor(const ApplicationProcessor& other) = delete;
    ApplicationProcessor& operator=(const ApplicationProcessor& other) = delete;
    virtual ~ApplicationProcessor() = default;

    virtual bool process() = 0;

    /* IProcessorDescription */
    virtual std::string getDescription() const override { return ""; }
    virtual ParameterSet getParameterSchema() const override { return ParameterSet(); };
    virtual std::vector<std::string> getRequirements() const override { return {}; };

  protected:
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
    virtual std::string getName() const override { return "CreateCommand"; };
  };

  struct BuildCommandsFromNames : ApplicationProcessor {
    BuildCommandsFromNames(ApplicationHandler& application_handler) : ApplicationProcessor(application_handler) {}
    bool process() override;
    std::vector<std::string> names_;
    std::vector<ApplicationHandler::Command> commands_;
    virtual std::string getName() const override { return "BuildCommandsFromNames"; };
  };

  struct LoadSession : ApplicationProcessor, IFilePickerHandler
  {
    /* IFilePickerHandler */
    bool onFilePicked(const std::filesystem::path& filename, ApplicationHandler* application_handler) override;

    std::optional<Filenames>    filenames_;           /// Pathnames to load - if not set, read it from session
    std::optional<Filenames>    filenames_override_;  /// Pathnames override
    std::optional<ParameterSet> parameters_override_; /// Parameters override
    std::string      delimiter = ",";                 /// String delimiter of the imported file
    bool             checkConsistency = true;         /// Check consistency of data contained in files

    LoadSession() = default;
    explicit LoadSession(
      ApplicationHandler& application_handler, 
      WorkflowManager& workflow_manager,
      IApplicationProcessorObserver* application_processor_observer = nullptr,
      ISequenceProcessorObserver* sequence_processor_observer = nullptr,
      ISequenceSegmentProcessorObserver* sequence_segment_processor_observer = nullptr,
      ISampleGroupProcessorObserver* sample_group_processor_observer = nullptr)
      : ApplicationProcessor(application_handler),
        workflow_manager_(workflow_manager),
        application_processor_observer_(application_processor_observer),
        sequence_processor_observer_(sequence_processor_observer),
        sequence_segment_processor_observer_(sequence_segment_processor_observer),
        sample_group_processor_observer_(sample_group_processor_observer)
    {}

    /* ApplicationProcessor */
    bool process() override;

    /* IProcessorDescription */
    virtual std::string getName() const override { return "LOAD_SESSION"; }
    virtual std::string getDescription() const override { return "Load an existing session"; }

//    std::vector<IPropertiesHandler*> properties_handlers;
  protected:
    bool overrideFilenames();
    bool overrideParameters();
    WorkflowManager& workflow_manager_;
    IApplicationProcessorObserver* application_processor_observer_;
    ISequenceProcessorObserver* sequence_processor_observer_;
    ISequenceSegmentProcessorObserver* sequence_segment_processor_observer_;
    ISampleGroupProcessorObserver* sample_group_processor_observer_;
  };

  struct LoadPropertiesHandlers : ApplicationProcessor
  {
    LoadPropertiesHandlers() = default;
    explicit LoadPropertiesHandlers(ApplicationHandler& application_handler) : ApplicationProcessor(application_handler) {}

    /* ApplicationProcessor */
    bool process() override;

    /* IProcessorDescription */
    virtual std::string getName() const override { return "LOAD_PROPERTIES_HANDLERS"; }
    virtual std::string getDescription() const override { return "Load a list of properties PropertiesHandlers."; }

    std::vector<IPropertiesHandler*> properties_handlers;
  };

  struct SavePropertiesHandlers : ApplicationProcessor
  {
    SavePropertiesHandlers() = default;
    explicit SavePropertiesHandlers(ApplicationHandler& application_handler) : ApplicationProcessor(application_handler) {}

    /* ApplicationProcessor */
    bool process() override;

    /* IProcessorDescription */
    virtual std::string getName() const override { return "SAVE_PROPERTIES_HANDLERS"; }
    virtual std::string getDescription() const override { return "Save a list of properties PropertiesHandlers."; }

    std::vector<IPropertiesHandler*> properties_handlers;
  };

  struct SaveSession : ApplicationProcessor, IFilePickerHandler
  {
    SaveSession() = default;
    explicit SaveSession(ApplicationHandler& application_handler) : ApplicationProcessor(application_handler) {}

    /* ApplicationProcessor */
    bool process() override;

    /* IFilePickerHandler */
    bool onFilePicked(const std::filesystem::path& filename, ApplicationHandler* application_handler) override;

    /* IProcessorDescription */
    virtual std::string getName() const override { return "SAVE_SESSION"; }
    virtual std::string getDescription() const override { return "Save the session"; }

    std::vector<IPropertiesHandler*> properties_handlers;
  };

  struct LoadFilenames : ApplicationProcessor
  {
    LoadFilenames() = default;
    explicit LoadFilenames(ApplicationHandler& application_handler) : ApplicationProcessor(application_handler) {}

    /* ApplicationProcessor */
    bool process() override;

    /* IProcessorDescription */
    virtual std::string getName() const override { return "LOAD_FILENAMES"; }
    virtual std::string getDescription() const override { return "Load Filenames from the DB"; }

    static std::optional<Filenames> loadFilenamesFromDB(const std::filesystem::path& path_db);
  };

  struct StoreFilenames : ApplicationProcessor
  {
    StoreFilenames() = default;
    explicit StoreFilenames(ApplicationHandler& application_handler) : ApplicationProcessor(application_handler) {}

    /* ApplicationProcessor */
    bool process() override;

    /* IProcessorDescription */
    virtual std::string getName() const override { return "STORE_FILENAMES"; }
    virtual std::string getDescription() const override { return "Store Filenames to the DB"; }
  };

}
