#pragma once

#include <vector>
#include <plog/Log.h>

namespace SmartPeak
{
  typedef std::pair<plog::Severity, plog::util::nstring> GARecord; // GuiAppenderRecord

  class GuiAppender : public plog::IAppender
  {
  public:
    void write(const plog::Record& record) override;

    std::vector<plog::util::nstring> getMessageList(plog::Severity severity) const;

  private:
    std::vector<GARecord> messages;
  };
}
