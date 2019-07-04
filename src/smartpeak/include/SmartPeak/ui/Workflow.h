#pragma once

#include <SmartPeak/ui/Widget.h>
#include <SmartPeak/core/AppState.h>
#include <string>
#include <vector>

namespace SmartPeak
{
  class Workflow final : public Widget
  {
    std::vector<AppState::Command> commands_;
    AppState* state_ = nullptr;

  public:
    bool draw_ = false;

    void draw() override;

    std::vector<AppState::Command> getCommands() const;

    void setState(AppState& state);
  };
}
