#pragma once

#include <SmartPeak/ui/Widget.h>
#include <SmartPeak/core/ApplicationHandler.h>
#include <SmartPeak/ui/FilePicker.h>
#include <string>
#include <vector>

namespace SmartPeak
{
  class Workflow final : public Widget
  {
    std::vector<ApplicationHandler::Command> commands_;
    ApplicationHandler* application_handler_ = nullptr;

  public:
    bool draw_ = false;

    void draw() override;

    std::vector<ApplicationHandler::Command> getCommands() const;

    void setApplicationHandler(ApplicationHandler& application_handler);

  protected:
    FilePicker file_picker_;
    bool popup_file_picker_ = false;
    std::unique_ptr<LoadWorkflow> load_workflow_;
    std::unique_ptr<SaveWorkflow> save_workflow_;
  };
}
