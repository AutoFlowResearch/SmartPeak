#pragma once
#include <SmartPeak/core/Utilities.h>
#include <array>
#include <string>
#include <boost/filesystem.hpp>

namespace fs = boost::filesystem;

namespace SmartPeak
{
  class FilePicker
  {
    std::array<std::vector<std::string>, 4> pathname_content_;
    std::string current_pathname_ = fs::current_path().root_path().string();
    std::string picked_pathname_;
    bool&       show_file_picker_;

  public:
    FilePicker() = delete;

    FilePicker(bool& show_file_picker) : show_file_picker_(show_file_picker) {
      pathname_content_ = Utilities::getPathnameContent(current_pathname_);
    }

    void draw(const char* title = "Pick a pathname");
    std::string getPickedPathname() const;
  };
}
