#pragma once

#include <vector>
#include <plog/Log.h>

namespace SmartPeak
{
  class GuiAppender : public plog::IAppender
  {
  public:
    void write(const plog::Record& record) override;

    std::vector<plog::util::nstring> getMessageList() const;

  private:
    std::vector<plog::util::nstring> messages;
  };
}
