#pragma once
#include <SmartPeak/core/Utilities.h>
#include <SmartPeak/ui/Widget.h>
#include <SmartPeak/ui/Observer.h>
#include <array>
#include <string>
#include <boost/filesystem.hpp>

namespace fs = boost::filesystem;

namespace SmartPeak
{
  class FilePicker final : public Widget, public Observable<FilePicker>
  {
    std::array<std::vector<std::string>, 4> pathname_content_;
    std::string current_pathname_ = fs::current_path().root_path().string();
    std::string picked_pathname_;
    std::string title_ = "Pick a pathname";
    std::string message_; // to observer(s)

  public:
    bool        show_file_picker_ = false;
    std::function<bool()> is_valid = [](){ return true; };

    FilePicker()
    {
      pathname_content_ = Utilities::getPathnameContent(current_pathname_);
    }

    void draw() override;

    std::string getPickedPathname() const;

    void setTitle(const std::string& title);

    void setMessage(const std::string& message);
  };

  class FilePickerObserver : public Observer<FilePicker> {
    void notification(FilePicker& source, const std::string& message) override
    {
      std::cout << "FilePickerObserver: a notification has been received" << std::endl;
      if (message == "load_session_from_sequence") {
        std::cout << source.getPickedPathname() << std::endl;
        source.unsubscribe(*this);
        if (!am_) {
          return;
        }
        am_->sequence_pathname_ = source.getPickedPathname();
        am_->loadSessionFromSequence();
      }
      // do something in AppManager according to the event/signal
    }
  public:
    AppManager* am_ = nullptr;
  };
}
