#pragma once

#include <SmartPeak/ui/Widget.h>
#include <SmartPeak/core/ApplicationHandler.h>
#include <string>
#include <vector>

namespace SmartPeak
{
  class Workflow final : public Widget
  {
    ApplicationHandler* application_handler_ = nullptr;

  public:
    void draw() override;

    void setApplicationHandler(ApplicationHandler& application_handler);
  };
}
