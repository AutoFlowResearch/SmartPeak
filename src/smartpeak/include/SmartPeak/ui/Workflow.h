#pragma once

#include <SmartPeak/ui/Widget.h>
#include <SmartPeak/core/ApplicationHandler.h>
#include <string>
#include <vector>

namespace SmartPeak
{
  class Workflow final : public Widget
  {
    std::vector<ApplicationHandler::Command> commands_;
    ApplicationHandler* state_ = nullptr;

  public:
    bool draw_ = false;

    void draw() override;

    std::vector<ApplicationHandler::Command> getCommands() const;

    void setState(ApplicationHandler& state);
  };
}
