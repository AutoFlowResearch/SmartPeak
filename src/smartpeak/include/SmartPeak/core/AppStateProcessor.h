#pragma once

#include <SmartPeak/core/AppState.h>
#include <SmartPeak/core/Filenames.h>
#include <SmartPeak/io/InputDataValidation.h>
#include <string>
#include <vector>

namespace SmartPeak
{
  struct AppStateProcessor {
    // C.67: A polymorphic class should suppress copying
    AppStateProcessor(const AppStateProcessor& other) = delete;
    AppStateProcessor& operator=(const AppStateProcessor& other) = delete;

    // C.35: A base class destructor should be either public and virtual, or protected and nonvirtual
    // C.127: A class with a virtual function should have a virtual or protected destructor
    virtual ~AppStateProcessor() = default;

    // Each of the derived classes implement one of the following virtual methods
    virtual void operator()(const std::vector<AppState::Command>& commands) {}
    virtual bool operator()(const int n, AppState::Command& cmd) { return false; }
    virtual void operator()(const char* const pathname) {}
    virtual std::vector<AppState::Command> operator()(const std::string& ids) { return std::vector<AppState::Command>(); }

    AppState& state_;

  protected:
    // Forced to write this, because the other user-defined constructors inhibit
    // the implicit definition of a default constructor
    // I implement these constructors as protected so nobody will instantiate
    // an AppStateProcessor outside of a derived class
    AppStateProcessor() = default;
    AppStateProcessor(AppState& state) : state_(state) {}
  };

  struct ProcessCommands : AppStateProcessor {
    ProcessCommands(AppState& state) : AppStateProcessor(state) {}
    void operator()(const std::vector<AppState::Command>& commands) override;
  };

  struct CreateCommand : AppStateProcessor {
    CreateCommand(AppState& state) : AppStateProcessor(state) {}
    bool operator()(const int n, AppState::Command& cmd) override;
  };

  struct LoadSessionFromSequence : AppStateProcessor {
    LoadSessionFromSequence(AppState& state) : AppStateProcessor(state) {}
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

  struct BuildCommandsFromIds : AppStateProcessor {
    BuildCommandsFromIds(AppState& state) : AppStateProcessor(state) {}
    std::vector<AppState::Command> operator()(const std::string& ids) override;
  };

  struct SetRawDataPathname : AppStateProcessor {
    SetRawDataPathname(AppState& state) : AppStateProcessor(state) {}
    void operator()(const char* const pathname) override;
  };

  struct SetInputFeaturesPathname : AppStateProcessor {
    SetInputFeaturesPathname(AppState& state) : AppStateProcessor(state) {}
    void operator()(const char* const pathname) override;
  };

  struct SetOutputFeaturesPathname : AppStateProcessor {
    SetOutputFeaturesPathname(AppState& state) : AppStateProcessor(state) {}
    void operator()(const char* const pathname) override;
  };
}
