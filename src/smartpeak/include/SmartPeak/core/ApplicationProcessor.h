#pragma once

#include <SmartPeak/core/ApplicationHandler.h>
#include <SmartPeak/core/Filenames.h>
#include <SmartPeak/io/InputDataValidation.h>
#include <string>
#include <vector>

namespace SmartPeak
{
  struct ApplicationProcessor {
    // C.67: A polymorphic class should suppress copying
    ApplicationProcessor(const ApplicationProcessor& other) = delete;
    ApplicationProcessor& operator=(const ApplicationProcessor& other) = delete;

    // C.35: A base class destructor should be either public and virtual, or protected and nonvirtual
    // C.127: A class with a virtual function should have a virtual or protected destructor
    virtual ~ApplicationProcessor() = default;

    // Each of the derived classes implement one of the following virtual methods
    virtual void operator()(const std::vector<ApplicationHandler::Command>& commands) {}
    virtual bool operator()(const std::string& name, ApplicationHandler::Command& cmd) { return false; }
    virtual void operator()(const char* const pathname) {}
    virtual std::vector<ApplicationHandler::Command> operator()(const std::string& ids) { return std::vector<ApplicationHandler::Command>(); }

    ApplicationHandler& state_;

  protected:
    // Forced to write this, because the other user-defined constructors inhibit
    // the implicit definition of a default constructor
    // I implement these constructors as protected so nobody will instantiate
    // an ApplicationProcessor outside of a derived class
    ApplicationProcessor() = default;
    ApplicationProcessor(ApplicationHandler& state) : state_(state) {}
  };

  namespace AppStateProcessors {
    // Passing a copy of "commands" because "state" is passed by reference, and
    // its "commands" (currently part of the "state" object) could not possibly
    // be passed as const reference.
    // I prefer not to pass "commands" by non-const reference because it would
    // tell the reader/developer "this is an input/output variable".
    // Since this function is called once every N minutes, the cost of the copy
    // can be ignored.
    void processCommands(ApplicationHandler& state, std::vector<ApplicationHandler::Command> commands);
  }

  struct CreateCommand : ApplicationProcessor {
    CreateCommand(ApplicationHandler& state) : ApplicationProcessor(state) {}
    bool operator()(const std::string& name, ApplicationHandler::Command& cmd) override;
  };

  struct LoadSessionFromSequence : ApplicationProcessor {
    LoadSessionFromSequence(ApplicationHandler& state) : ApplicationProcessor(state) {}
    void operator()(const char* const pathname) override;
  private:
    bool buildStaticFilenames();
    void updateFilenames(Filenames& f, const std::string& pathname);
    bool requiredPathnamesAreValid(const std::vector<InputDataValidation::FilenameInfo>& validation);
    void clearNonExistantDefaultGeneratedFilenames(Filenames& f);
    void clearNonExistantFilename(std::string& filename);
    void generatePathnamesTxt(
      const std::string& pathname,
      const Filenames& f,
      const std::vector<InputDataValidation::FilenameInfo>& is_valid
    );
    std::string getValidPathnameOrPlaceholder(const std::string& pathname, const bool is_valid);
  };

  struct BuildCommandsFromNames : ApplicationProcessor {
    BuildCommandsFromNames(ApplicationHandler& state) : ApplicationProcessor(state) {}
    std::vector<ApplicationHandler::Command> operator()(const std::string& names) override;
  };

  struct SetRawDataPathname : ApplicationProcessor {
    SetRawDataPathname(ApplicationHandler& state) : ApplicationProcessor(state) {}
    void operator()(const char* const pathname) override;
  };

  struct SetInputFeaturesPathname : ApplicationProcessor {
    SetInputFeaturesPathname(ApplicationHandler& state) : ApplicationProcessor(state) {}
    void operator()(const char* const pathname) override;
  };

  struct SetOutputFeaturesPathname : ApplicationProcessor {
    SetOutputFeaturesPathname(ApplicationHandler& state) : ApplicationProcessor(state) {}
    void operator()(const char* const pathname) override;
  };
}
