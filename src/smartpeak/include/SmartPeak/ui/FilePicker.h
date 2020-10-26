#pragma once

#include <SmartPeak/core/ApplicationProcessor.h>
#include <SmartPeak/core/Utilities.h>
#include <SmartPeak/ui/Widget.h>
#include <array>
#include <string>
#include <vector>
// #include <boost/filesystem.hpp>

// namespace fs = boost::filesystem;

namespace SmartPeak
{
  class FilePicker final : public Widget
  {
    std::array<std::vector<std::string>, 4> pathname_content_;
    std::string current_pathname_ = ".";
    std::string picked_pathname_;
    FilePickerProcessor* processor_ = nullptr;
    std::string processor_name_ = "";
    bool loading_is_done_ = true;
    bool file_was_loaded_ = true;
    bool error_loading_file_ = false;

    void run_and_join(
      FilePickerProcessor* processor,
      const std::string& pathname,
      bool& loading_is_done,
      bool& file_was_loaded
    );

  public:
    FilePicker()
    {
      // current_pathname_ = fs::current_path().root_path().string();
      pathname_content_ = Utilities::getPathnameContent(current_pathname_);
    }

    void draw() override;
    std::string getPickedPathname() const;
    void setProcessor(FilePickerProcessor& processor);
    const std::string getProcessorName() const { return processor_name_; };
    void runProcessor();
    void clearProcessor();
    bool fileLoadingIsDone() { return loading_is_done_; };
    bool fileWasLoaded() { return file_was_loaded_; };
    bool errorLoadingFile() { return error_loading_file_; };
  };
}