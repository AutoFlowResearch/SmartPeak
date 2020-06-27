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
    ApplicationProcessor() = default;
    ApplicationProcessor(ApplicationHandler& application_handler) : application_handler_(application_handler) {}
  };

  namespace ApplicationProcessors {
    void processCommands(ApplicationHandler& application_handler, std::vector<ApplicationHandler::Command> commands, const std::set<std::string>& injection_names, const std::set<std::string>& sequence_segment_names);
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

  struct LoadSequenceParameters : FilePickerProcessor {
    LoadSequenceParameters(ApplicationHandler& application_handler) : FilePickerProcessor(application_handler) {}
    bool process() override;
  };

  struct LoadSequenceTransitions : FilePickerProcessor {
    LoadSequenceTransitions(ApplicationHandler& application_handler) : FilePickerProcessor(application_handler) {}
    bool process() override;
  };

  struct LoadSequenceValidationData : FilePickerProcessor {
    LoadSequenceValidationData(ApplicationHandler& application_handler) : FilePickerProcessor(application_handler) {}
    bool process() override;
  };

  struct LoadSequenceSegmentQuantitationMethods : FilePickerProcessor {
    LoadSequenceSegmentQuantitationMethods(ApplicationHandler& application_handler) : FilePickerProcessor(application_handler) {}
    bool process() override;
  };

  struct LoadSequenceSegmentStandardsConcentrations : FilePickerProcessor {
    LoadSequenceSegmentStandardsConcentrations(ApplicationHandler& application_handler) : FilePickerProcessor(application_handler) {}
    bool process() override;
  };

  struct LoadSequenceSegmentFeatureFilterComponents : FilePickerProcessor {
    LoadSequenceSegmentFeatureFilterComponents(ApplicationHandler& application_handler) : FilePickerProcessor(application_handler) {}
    bool process() override;
  };

  struct LoadSequenceSegmentFeatureFilterComponentGroups : FilePickerProcessor {
    LoadSequenceSegmentFeatureFilterComponentGroups(ApplicationHandler& application_handler) : FilePickerProcessor(application_handler) {}
    bool process() override;
  };

  struct LoadSequenceSegmentFeatureQCComponents : FilePickerProcessor {
    LoadSequenceSegmentFeatureQCComponents(ApplicationHandler& application_handler) : FilePickerProcessor(application_handler) {}
    bool process() override;
  };

  struct LoadSequenceSegmentFeatureQCComponentGroups : FilePickerProcessor {
    LoadSequenceSegmentFeatureQCComponentGroups(ApplicationHandler& application_handler) : FilePickerProcessor(application_handler) {}
    bool process() override;
  };

  struct LoadSequenceSegmentFeatureRSDFilterComponents : FilePickerProcessor {
    LoadSequenceSegmentFeatureRSDFilterComponents(ApplicationHandler& application_handler) : FilePickerProcessor(application_handler) {}
    bool process() override;
  };

  struct LoadSequenceSegmentFeatureRSDFilterComponentGroups : FilePickerProcessor {
    LoadSequenceSegmentFeatureRSDFilterComponentGroups(ApplicationHandler& application_handler) : FilePickerProcessor(application_handler) {}
    bool process() override;
  };

  struct LoadSequenceSegmentFeatureRSDQCComponents : FilePickerProcessor {
    LoadSequenceSegmentFeatureRSDQCComponents(ApplicationHandler& application_handler) : FilePickerProcessor(application_handler) {}
    bool process() override;
  };

  struct LoadSequenceSegmentFeatureRSDQCComponentGroups : FilePickerProcessor {
    LoadSequenceSegmentFeatureRSDQCComponentGroups(ApplicationHandler& application_handler) : FilePickerProcessor(application_handler) {}
    bool process() override;
  };

  struct LoadSequenceSegmentFeatureBackgroundFilterComponents : FilePickerProcessor {
    LoadSequenceSegmentFeatureBackgroundFilterComponents(ApplicationHandler& application_handler) : FilePickerProcessor(application_handler) {}
    bool process() override;
  };

  struct LoadSequenceSegmentFeatureBackgroundFilterComponentGroups : FilePickerProcessor {
    LoadSequenceSegmentFeatureBackgroundFilterComponentGroups(ApplicationHandler& application_handler) : FilePickerProcessor(application_handler) {}
    bool process() override;
  };

  struct LoadSequenceSegmentFeatureBackgroundQCComponents : FilePickerProcessor {
    LoadSequenceSegmentFeatureBackgroundQCComponents(ApplicationHandler& application_handler) : FilePickerProcessor(application_handler) {}
    bool process() override;
  };

  struct LoadSequenceSegmentFeatureBackgroundQCComponentGroups : FilePickerProcessor {
    LoadSequenceSegmentFeatureBackgroundQCComponentGroups(ApplicationHandler& application_handler) : FilePickerProcessor(application_handler) {}
    bool process() override;
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
