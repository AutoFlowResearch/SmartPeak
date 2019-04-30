#pragma once
#include <SmartPeak/core/Table.h>
#include <SmartPeak/core/Utilities.h>
#include <array>
#include <string>
#include <imgui.h>

namespace SmartPeak
{
  class FilePicker
  {
    bool&       show_file_picker_;
    std::string current_pathname_ = "/";
    Table       pathname_content_;
    std::string picked_pathname_;

  public:
    FilePicker() = delete;

    FilePicker(bool& show_file_picker) : show_file_picker_(show_file_picker) {
      Utilities::getPathnameContent(current_pathname_, pathname_content_, false);
    }

    void draw(const char* title = "Pick a pathname");
    std::string getPickedPathname() const;
  };
}
