#pragma once

#include <SmartPeak/core/AppState.h>
#include <SmartPeak/core/Filenames.h>
#include <SmartPeak/io/InputDataValidation.h>
#include <string>
#include <vector>

namespace SmartPeak
{
  struct AppStateProcessor final {
    AppStateProcessor() = delete;
    AppStateProcessor(AppState& state) : state_(state) {}

    bool buildStaticFilenames();

    bool requiredPathnamesAreValid(const std::vector<InputDataValidation::FilenameInfo>& validation);

    void clearNonExistantDefaultGeneratedFilenames(Filenames& f);

    void clearNonExistantFilename(std::string& filename);

    void generatePathnamesTxt(
      const std::string& pathname,
      const Filenames& f,
      const std::vector<InputDataValidation::FilenameInfo>& is_valid
    );

    std::string getValidPathnameOrPlaceholder(const std::string& pathname, const bool is_valid);

    void updateFilenames(Filenames& f, const std::string& pathname);

    void processCommands(const std::vector<AppState::Command>& commands);

    bool createCommand(const int n, AppState::Command& cmd);

  private:
    AppState& state_;
  };
}
