#pragma once

#include <mutex>
#include <vector>
#include <plog/Log.h>

namespace SmartPeak
{
  class GuiAppender : public plog::IAppender
  {
  public:
    void write(const plog::Record& record) override;

    std::vector<plog::util::nstring> getMessageList(plog::Severity severity) const;

  private:
    typedef std::pair<plog::Severity, plog::util::nstring> GARecord; // GuiAppenderRecord
    std::vector<GARecord> messages;
    mutable std::mutex messages_mutex;
  };
}
