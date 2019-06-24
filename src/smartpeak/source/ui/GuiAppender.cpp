#include <SmartPeak/ui/GuiAppender.h>
#include <mutex>
#include <vector>
#include <plog/Log.h>

namespace SmartPeak
{
  void GuiAppender::write(const plog::Record& record)
  {
    plog::util::nstring str = plog::TxtFormatter::format(record);
    std::lock_guard<std::mutex> g(messages_mutex);
    messages.emplace_back(record.getSeverity(), str);
  }

  std::vector<plog::util::nstring> GuiAppender::getMessageList(plog::Severity severity) const
  {
    std::vector<plog::util::nstring> filtered;
    std::lock_guard<std::mutex> g(messages_mutex);
    for (const GARecord& p : messages) {
      if (p.first <= severity) {
        filtered.push_back(p.second);
      }
    }
    return filtered;
  }
}
