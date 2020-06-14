#pragma once

#include <SmartPeak/core/ApplicationHandler.h>
#include <SmartPeak/core/Filenames.h>
#include <SmartPeak/io/InputDataValidation.h>
#include <string>
#include <vector>

namespace SmartPeak
{
  struct ApplicationProcessor {
    ApplicationProcessor(const ApplicationProcessor& other) = delete;
    ApplicationProcessor& operator=(const ApplicationProcessor& other) = delete;
    virtual ~ApplicationProcessor() = default;

    // Each of the derived classes implement the following virtual methods
    virtual bool process() = 0;

    ApplicationHandler& application_handler_;

  protected:
    // Forced to write this, because the other user-defined constructors inhibit
    // the implicit definition of a default constructor
    // I implement these constructors as protected so nobody will instantiate
    // an ApplicationProcessor outside of a derived class
    ApplicationProcessor() = default;
    ApplicationProcessor(ApplicationHandler& application_handler) : application_handler_(application_handler) {}
  };

  namespace ApplicationProcessors {
    // Passing a copy of "commands" because "application_handler" is passed by reference, and
    // its "commands" (currently part of the "application_handler" object) could not possibly
    // be passed as const reference.
    // I prefer not to pass "commands" by non-const reference because it would
    // tell the reader/developer "this is an input/output variable".
    // Since this function is called once every N minutes, the cost of the copy
    // can be ignored.
    void processCommands(ApplicationHandler& application_handler, std::vector<ApplicationHandler::Command> commands);
  }

  struct CreateCommand : ApplicationProcessor {
    CreateCommand(ApplicationHandler& application_handler) : ApplicationProcessor(application_handler) {}
    bool process() override;
    std::string name_;
    ApplicationHandler::Command cmd_;
  };

  struct BuildCommandsFromNames : ApplicationProcessor {
    BuildCommandsFromNames(ApplicationHandler& application_handler) : ApplicationProcessor(application_handler) {}
    bool process() override;
    std::string names_;
    std::vector<ApplicationHandler::Command> commands_;
  };

  struct FilePickerProcessor : ApplicationProcessor {
    FilePickerProcessor(ApplicationHandler& application_handler) : ApplicationProcessor(application_handler) {}
    std::string pathname_;
  };

  struct LoadSessionFromSequence : FilePickerProcessor {
    LoadSessionFromSequence(ApplicationHandler& application_handler) : FilePickerProcessor(application_handler) {}
    bool process() override;
  private:
    bool buildStaticFilenames();
    void updateFilenames(Filenames& f, const std::string& pathname);
    bool requiredPathnamesAreValid(const std::vector<InputDataValidation::FilenameInfo>& validation);
    void clearNonExistantDefaultGeneratedFilenames(Filenames& f);
    void clearNonExistantFilename(std::string& filename);
    std::string getValidPathnameOrPlaceholder(const std::string& pathname, const bool is_valid);
  };

  struct SetRawDataPathname : FilePickerProcessor {
    SetRawDataPathname(ApplicationHandler& application_handler) : FilePickerProcessor(application_handler) {}
    bool process() override;
  };

  struct SetInputFeaturesPathname : FilePickerProcessor {
    SetInputFeaturesPathname(ApplicationHandler& application_handler) : FilePickerProcessor(application_handler) {}
    bool process() override;
  };

  struct SetOutputFeaturesPathname : FilePickerProcessor {
    SetOutputFeaturesPathname(ApplicationHandler& application_handler) : FilePickerProcessor(application_handler) {}
    bool process() override;
  };
}
